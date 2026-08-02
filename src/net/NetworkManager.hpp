#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include "../world/Block.hpp"

namespace Minecraft {

enum class PacketType : uint8_t {
    PlayerPosition = 1,
    BlockChange,
    ChatMessage,
    EntityState
};

struct PlayerPosPacket {
    uint32_t playerId;
    glm::vec3 position;
    float yaw;
    float pitch;
};

struct BlockChangePacket {
    glm::ivec3 blockPos;
    BlockType newBlock;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool startServer(uint16_t port = 25565);
    bool connectToServer(const std::string& ip, uint16_t port = 25565);
    void disconnect();

    void sendPlayerPosition(const glm::vec3& pos, float yaw, float pitch);
    void sendBlockChange(const glm::ivec3& blockPos, BlockType type);

    bool isServer() const { return m_IsServer; }
    bool isConnected() const { return m_IsConnected; }

    const std::vector<PlayerPosPacket>& getRemotePlayers() const { return m_RemotePlayers; }

private:
    bool m_IsServer = false;
    bool m_IsConnected = false;
    uint32_t m_LocalPlayerId = 1;
    std::vector<PlayerPosPacket> m_RemotePlayers;
};

}

#endif // NETWORKMANAGER_HPP
