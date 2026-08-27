#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include "../world/Block.hpp"

namespace Minecraft {

enum class PacketType : uint8_t {
    Handshake = 1,
    PlayerPosition,
    BlockChange,
    ChatMessage,
    TimeSync,
    EntityState
};

struct PlayerPosPacket {
    uint32_t playerId = 0;
    glm::vec3 position{ 0.0f };
    float yaw = 0.0f;
    float pitch = 0.0f;
};

struct BlockChangePacket {
    glm::ivec3 blockPos{ 0 };
    BlockType newBlock = BlockType::Air;
};

struct ChatMessagePacket {
    uint32_t senderId = 0;
    std::string message;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool startServer(uint16_t port = 25565);
    bool connectToServer(const std::string& ip, uint16_t port = 25565);
    void disconnect();
    void update(class World* world = nullptr);

    void sendPlayerPosition(const glm::vec3& pos, float yaw, float pitch);
    void sendBlockChange(const glm::ivec3& blockPos, BlockType type);
    void sendChatMessage(const std::string& message);

    bool isServer() const { return m_IsServer; }
    bool isConnected() const { return m_IsConnected; }

    const std::vector<PlayerPosPacket>& getRemotePlayers() const { return m_RemotePlayers; }
    const std::vector<std::string>& getChatLog() const { return m_ChatLog; }

    // Binary Packet Serialization & Deserialization
    static std::vector<uint8_t> serializePlayerPos(const PlayerPosPacket& packet);
    static bool deserializePlayerPos(const uint8_t* data, size_t size, PlayerPosPacket& outPacket);

    static std::vector<uint8_t> serializeBlockChange(const BlockChangePacket& packet);
    static bool deserializeBlockChange(const uint8_t* data, size_t size, BlockChangePacket& outPacket);

    static std::vector<uint8_t> serializeChatMessage(const ChatMessagePacket& packet);
    static bool deserializeChatMessage(const uint8_t* data, size_t size, ChatMessagePacket& outPacket);

    // Client-Side Prediction & Dead-Reckoning Interpolation
    static glm::vec3 interpolateDeadReckoning(const glm::vec3& currentPos, const glm::vec3& targetPos, const glm::vec3& velocity, float alpha, float deltaTime);
    static float interpolateAngle(float currentAngle, float targetAngle, float alpha);

    void processIncomingPacket(const uint8_t* buffer, size_t size, class World* world = nullptr);

private:
    bool m_IsServer = false;
    bool m_IsConnected = false;
    uint32_t m_LocalPlayerId = 1;
    std::vector<PlayerPosPacket> m_RemotePlayers;
    std::vector<std::string> m_ChatLog;

    uintptr_t m_Socket = ~0ULL; // socket descriptor
    std::string m_RemoteIP = "127.0.0.1";
    uint16_t m_RemotePort = 25565;
};

}

#endif // NETWORKMANAGER_HPP
