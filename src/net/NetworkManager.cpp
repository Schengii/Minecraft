#include "NetworkManager.hpp"
#include <iostream>

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
}

void NetworkManager::sendBlockChange(const glm::ivec3& blockPos, BlockType type) {
    if (!m_IsConnected) return;
    BlockChangePacket pkt;
    pkt.blockPos = blockPos;
    pkt.newBlock = type;
}

}
