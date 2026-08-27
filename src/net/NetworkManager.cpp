#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "NetworkManager.hpp"
#include "../world/World.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

namespace Minecraft {

NetworkManager::NetworkManager() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

NetworkManager::~NetworkManager() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool NetworkManager::startServer(uint16_t port) {
    m_IsServer = true;
    m_IsConnected = true;
    m_RemotePort = port;

#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[NetworkManager] Failed to create UDP server socket." << std::endl;
        return false;
    }

    u_long nonBlocking = 1;
    ioctlsocket(sock, FIONBIO, &nonBlocking);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[NetworkManager] Failed to bind server socket to port " << port << std::endl;
        closesocket(sock);
        return false;
    }

    m_Socket = static_cast<uintptr_t>(sock);
#endif

    std::cout << "[NetworkManager] UDP Server started listening on port " << port << std::endl;
    return true;
}

bool NetworkManager::connectToServer(const std::string& ip, uint16_t port) {
    m_IsServer = false;
    m_IsConnected = true;
    m_RemoteIP = ip;
    m_RemotePort = port;

#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[NetworkManager] Failed to create UDP client socket." << std::endl;
        return false;
    }

    u_long nonBlocking = 1;
    ioctlsocket(sock, FIONBIO, &nonBlocking);
    m_Socket = static_cast<uintptr_t>(sock);
#endif

    std::cout << "[NetworkManager] Connected to server at " << ip << ":" << port << std::endl;
    return true;
}

void NetworkManager::disconnect() {
    if (m_IsConnected) {
        if (m_Socket != static_cast<uintptr_t>(~0ULL)) {
#ifdef _WIN32
            closesocket(static_cast<SOCKET>(m_Socket));
#else
            close(static_cast<int>(m_Socket));
#endif
            m_Socket = ~0ULL;
        }
        std::cout << "[NetworkManager] Network session disconnected." << std::endl;
        m_IsConnected = false;
        m_IsServer = false;
        m_RemotePlayers.clear();
    }
}

void NetworkManager::update(World* world) {
    if (!m_IsConnected || m_Socket == static_cast<uintptr_t>(~0ULL)) return;

    uint8_t buffer[2048];
#ifdef _WIN32
    sockaddr_in fromAddr{};
    socklen_t fromLen = sizeof(fromAddr);

    while (true) {
        int bytesReceived = recvfrom(static_cast<SOCKET>(m_Socket), reinterpret_cast<char*>(buffer), sizeof(buffer), 0, (sockaddr*)&fromAddr, &fromLen);
        if (bytesReceived <= 0) break;

        processIncomingPacket(buffer, static_cast<size_t>(bytesReceived), world);
    }
#endif
}

void NetworkManager::sendPlayerPosition(const glm::vec3& pos, float yaw, float pitch) {
    if (!m_IsConnected) return;
    PlayerPosPacket pkt;
    pkt.playerId = m_LocalPlayerId;
    pkt.position = pos;
    pkt.yaw = yaw;
    pkt.pitch = pitch;

    std::vector<uint8_t> bytes = serializePlayerPos(pkt);

#ifdef _WIN32
    if (m_Socket != static_cast<uintptr_t>(~0ULL)) {
        sockaddr_in destAddr{};
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(m_RemotePort);
        inet_pton(AF_INET, m_RemoteIP.c_str(), &destAddr.sin_addr);

        sendto(static_cast<SOCKET>(m_Socket), reinterpret_cast<const char*>(bytes.data()), static_cast<int>(bytes.size()), 0, (sockaddr*)&destAddr, sizeof(destAddr));
    }
#endif
}

void NetworkManager::sendBlockChange(const glm::ivec3& blockPos, BlockType type) {
    if (!m_IsConnected) return;
    BlockChangePacket pkt;
    pkt.blockPos = blockPos;
    pkt.newBlock = type;

    std::vector<uint8_t> bytes = serializeBlockChange(pkt);

#ifdef _WIN32
    if (m_Socket != static_cast<uintptr_t>(~0ULL)) {
        sockaddr_in destAddr{};
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(m_RemotePort);
        inet_pton(AF_INET, m_RemoteIP.c_str(), &destAddr.sin_addr);

        sendto(static_cast<SOCKET>(m_Socket), reinterpret_cast<const char*>(bytes.data()), static_cast<int>(bytes.size()), 0, (sockaddr*)&destAddr, sizeof(destAddr));
    }
#endif
}

void NetworkManager::sendChatMessage(const std::string& message) {
    if (!m_IsConnected) return;
    ChatMessagePacket pkt;
    pkt.senderId = m_LocalPlayerId;
    pkt.message = message;

    std::vector<uint8_t> bytes = serializeChatMessage(pkt);
    m_ChatLog.push_back("<Player " + std::to_string(m_LocalPlayerId) + "> " + message);

#ifdef _WIN32
    if (m_Socket != static_cast<uintptr_t>(~0ULL)) {
        sockaddr_in destAddr{};
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(m_RemotePort);
        inet_pton(AF_INET, m_RemoteIP.c_str(), &destAddr.sin_addr);

        sendto(static_cast<SOCKET>(m_Socket), reinterpret_cast<const char*>(bytes.data()), static_cast<int>(bytes.size()), 0, (sockaddr*)&destAddr, sizeof(destAddr));
    }
#endif
}

std::vector<uint8_t> NetworkManager::serializePlayerPos(const PlayerPosPacket& packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(PacketType::PlayerPosition));

    uint32_t pid = packet.playerId;
    const uint8_t* pIdBytes = reinterpret_cast<const uint8_t*>(&pid);
    buffer.insert(buffer.end(), pIdBytes, pIdBytes + sizeof(uint32_t));

    const uint8_t* posBytes = reinterpret_cast<const uint8_t*>(&packet.position);
    buffer.insert(buffer.end(), posBytes, posBytes + sizeof(glm::vec3));

    const uint8_t* yawBytes = reinterpret_cast<const uint8_t*>(&packet.yaw);
    buffer.insert(buffer.end(), yawBytes, yawBytes + sizeof(float));

    const uint8_t* pitchBytes = reinterpret_cast<const uint8_t*>(&packet.pitch);
    buffer.insert(buffer.end(), pitchBytes, pitchBytes + sizeof(float));

    return buffer;
}

bool NetworkManager::deserializePlayerPos(const uint8_t* data, size_t size, PlayerPosPacket& outPacket) {
    size_t expectedSize = 1 + sizeof(uint32_t) + sizeof(glm::vec3) + sizeof(float) * 2;
    if (size < expectedSize || data[0] != static_cast<uint8_t>(PacketType::PlayerPosition)) {
        return false;
    }

    size_t offset = 1;
    std::memcpy(&outPacket.playerId, data + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    std::memcpy(&outPacket.position, data + offset, sizeof(glm::vec3));
    offset += sizeof(glm::vec3);

    std::memcpy(&outPacket.yaw, data + offset, sizeof(float));
    offset += sizeof(float);

    std::memcpy(&outPacket.pitch, data + offset, sizeof(float));
    return true;
}

std::vector<uint8_t> NetworkManager::serializeBlockChange(const BlockChangePacket& packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(PacketType::BlockChange));

    const uint8_t* posBytes = reinterpret_cast<const uint8_t*>(&packet.blockPos);
    buffer.insert(buffer.end(), posBytes, posBytes + sizeof(glm::ivec3));

    buffer.push_back(static_cast<uint8_t>(packet.newBlock));
    return buffer;
}

bool NetworkManager::deserializeBlockChange(const uint8_t* data, size_t size, BlockChangePacket& outPacket) {
    size_t expectedSize = 1 + sizeof(glm::ivec3) + 1;
    if (size < expectedSize || data[0] != static_cast<uint8_t>(PacketType::BlockChange)) {
        return false;
    }

    std::memcpy(&outPacket.blockPos, data + 1, sizeof(glm::ivec3));
    outPacket.newBlock = static_cast<BlockType>(data[1 + sizeof(glm::ivec3)]);
    return true;
}

std::vector<uint8_t> NetworkManager::serializeChatMessage(const ChatMessagePacket& packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(PacketType::ChatMessage));

    uint32_t sid = packet.senderId;
    const uint8_t* sidBytes = reinterpret_cast<const uint8_t*>(&sid);
    buffer.insert(buffer.end(), sidBytes, sidBytes + sizeof(uint32_t));

    uint16_t msgLen = static_cast<uint16_t>(packet.message.length());
    const uint8_t* lenBytes = reinterpret_cast<const uint8_t*>(&msgLen);
    buffer.insert(buffer.end(), lenBytes, lenBytes + sizeof(uint16_t));

    buffer.insert(buffer.end(), packet.message.begin(), packet.message.end());
    return buffer;
}

bool NetworkManager::deserializeChatMessage(const uint8_t* data, size_t size, ChatMessagePacket& outPacket) {
    if (size < 1 + sizeof(uint32_t) + sizeof(uint16_t) || data[0] != static_cast<uint8_t>(PacketType::ChatMessage)) {
        return false;
    }

    size_t offset = 1;
    std::memcpy(&outPacket.senderId, data + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint16_t msgLen = 0;
    std::memcpy(&msgLen, data + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    if (size < offset + msgLen) return false;

    outPacket.message = std::string(reinterpret_cast<const char*>(data + offset), msgLen);
    return true;
}

void NetworkManager::processIncomingPacket(const uint8_t* buffer, size_t size, World* world) {
    if (size == 0) return;

    PacketType type = static_cast<PacketType>(buffer[0]);
    if (type == PacketType::PlayerPosition) {
        PlayerPosPacket pkt;
        if (deserializePlayerPos(buffer, size, pkt)) {
            bool found = false;
            for (auto& remote : m_RemotePlayers) {
                if (remote.playerId == pkt.playerId) {
                    remote.position = pkt.position;
                    remote.yaw = pkt.yaw;
                    remote.pitch = pkt.pitch;
                    found = true;
                    break;
                }
            }
            if (!found) {
                m_RemotePlayers.push_back(pkt);
            }
        }
    } else if (type == PacketType::BlockChange) {
        BlockChangePacket pkt;
        if (deserializeBlockChange(buffer, size, pkt)) {
            if (world) {
                world->setBlock(pkt.blockPos.x, pkt.blockPos.y, pkt.blockPos.z, pkt.newBlock);
            }
        }
    } else if (type == PacketType::ChatMessage) {
        ChatMessagePacket pkt;
        if (deserializeChatMessage(buffer, size, pkt)) {
            m_ChatLog.push_back("<Player " + std::to_string(pkt.senderId) + "> " + pkt.message);
        }
    }
}

}
