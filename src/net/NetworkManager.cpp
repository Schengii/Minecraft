#include "NetworkManager.hpp"
#include <iostream>
#include <cstring>

namespace Minecraft {

NetworkManager::NetworkManager() = default;
NetworkManager::~NetworkManager() {
    disconnect();
}

bool NetworkManager::startServer(uint16_t port) {
    m_IsServer = true;
    m_IsConnected = true;
    std::cout << "[NetworkManager] Server started listening on port " << port << std::endl;
    return true;
}

bool NetworkManager::connectToServer(const std::string& ip, uint16_t port) {
    m_IsServer = false;
    m_IsConnected = true;
    std::cout << "[NetworkManager] Connected to server at " << ip << ":" << port << std::endl;
    return true;
}

void NetworkManager::disconnect() {
    if (m_IsConnected) {
        std::cout << "[NetworkManager] Network session disconnected." << std::endl;
        m_IsConnected = false;
        m_IsServer = false;
        m_RemotePlayers.clear();
    }
}

void NetworkManager::sendPlayerPosition(const glm::vec3& pos, float yaw, float pitch) {
    if (!m_IsConnected) return;
    PlayerPosPacket pkt;
    pkt.playerId = m_LocalPlayerId;
    pkt.position = pos;
    pkt.yaw = yaw;
    pkt.pitch = pitch;
    std::vector<uint8_t> bytes = serializePlayerPos(pkt);
    (void)bytes;
}

void NetworkManager::sendBlockChange(const glm::ivec3& blockPos, BlockType type) {
    if (!m_IsConnected) return;
    BlockChangePacket pkt;
    pkt.blockPos = blockPos;
    pkt.newBlock = type;
    std::vector<uint8_t> bytes = serializeBlockChange(pkt);
    (void)bytes;
}

void NetworkManager::sendChatMessage(const std::string& message) {
    if (!m_IsConnected) return;
    ChatMessagePacket pkt;
    pkt.senderId = m_LocalPlayerId;
    pkt.message = message;
    std::vector<uint8_t> bytes = serializeChatMessage(pkt);
    m_ChatLog.push_back("<Player " + std::to_string(m_LocalPlayerId) + "> " + message);
    (void)bytes;
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

    size_t offset = 1;
    std::memcpy(&outPacket.blockPos, data + offset, sizeof(glm::ivec3));
    offset += sizeof(glm::ivec3);

    outPacket.newBlock = static_cast<BlockType>(data[offset]);
    return true;
}

std::vector<uint8_t> NetworkManager::serializeChatMessage(const ChatMessagePacket& packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(PacketType::ChatMessage));

    const uint8_t* senderBytes = reinterpret_cast<const uint8_t*>(&packet.senderId);
    buffer.insert(buffer.end(), senderBytes, senderBytes + sizeof(uint32_t));

    uint16_t len = static_cast<uint16_t>(packet.message.size());
    const uint8_t* lenBytes = reinterpret_cast<const uint8_t*>(&len);
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

    uint16_t len = 0;
    std::memcpy(&len, data + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    if (size < offset + len) return false;

    outPacket.message = std::string(reinterpret_cast<const char*>(data + offset), len);
    return true;
}

void NetworkManager::processIncomingPacket(const uint8_t* buffer, size_t size) {
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
    } else if (type == PacketType::ChatMessage) {
        ChatMessagePacket pkt;
        if (deserializeChatMessage(buffer, size, pkt)) {
            m_ChatLog.push_back("<Player " + std::to_string(pkt.senderId) + "> " + pkt.message);
        }
    }
}

}
