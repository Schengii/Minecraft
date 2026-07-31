#include "InventoryGUI.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

InventoryGUI::InventoryGUI(int windowWidth, int windowHeight)
    : m_Width(windowWidth), m_Height(windowHeight)
{
    m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    initBuffers();
}

InventoryGUI::~InventoryGUI() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void InventoryGUI::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
}

void InventoryGUI::initBuffers() {
    float quadVertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void InventoryGUI::render(Inventory& inventory, bool isOpen) {
    if (!isOpen) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Main dark transparent overlay background
    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2(m_Width, m_Height), glm::vec4(0.0f, 0.0f, 0.0f, 0.65f));

    // Inventory Window Container
    float winWidth = 420.0f;
    float winHeight = 360.0f;
    float winX = (m_Width - winWidth) / 2.0f;
    float winY = (m_Height - winHeight) / 2.0f;

    renderQuad(glm::vec2(winX, winY), glm::vec2(winWidth, winHeight), glm::vec4(0.2f, 0.2f, 0.2f, 0.95f));

    // Render 9x3 Main Inventory Slots
    float slotSize = 36.0f;
    float padding = 4.0f;
    float startX = winX + 25.0f;
    float startY = winY + 140.0f;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            int slotIdx = 9 + row * 9 + col;
            float x = startX + col * (slotSize + padding);
            float y = startY + row * (slotSize + padding);

            glm::vec4 slotColor = (slotIdx == m_SelectedSlotIndex) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.35f, 0.35f, 0.35f, 0.8f);
            renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), slotColor);

            const ItemStack& stack = inventory.getSlot(slotIdx);
            if (!stack.isEmpty()) {
                renderQuad(glm::vec2(x + 4.0f, y + 4.0f), glm::vec2(slotSize - 8.0f, slotSize - 8.0f), glm::vec4(0.6f, 0.5f, 0.3f, 0.9f));
            }
        }
    }

    // Render 9 Hotbar Slots at bottom of window
    float hotbarY = startY + 3 * (slotSize + padding) + 15.0f;
    for (int col = 0; col < 9; ++col) {
        float x = startX + col * (slotSize + padding);
        glm::vec4 slotColor = (col == m_SelectedSlotIndex) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.35f, 0.35f, 0.35f, 0.8f);
        renderQuad(glm::vec2(x, hotbarY), glm::vec2(slotSize, slotSize), slotColor);

        const ItemStack& stack = inventory.getSlot(col);
        if (!stack.isEmpty()) {
            renderQuad(glm::vec2(x + 4.0f, hotbarY + 4.0f), glm::vec2(slotSize - 8.0f, slotSize - 8.0f), glm::vec4(0.4f, 0.7f, 0.3f, 0.9f));
        }
    }

    // Render 2x2 Crafting Grid
    float craftX = winX + 220.0f;
    float craftY = winY + 25.0f;
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            float x = craftX + c * (slotSize + padding);
            float y = craftY + r * (slotSize + padding);
            renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), glm::vec4(0.4f, 0.4f, 0.4f, 0.9f));
        }
    }

    // Render Crafting Result Slot
    float resultX = craftX + 2 * (slotSize + padding) + 25.0f;
    float resultY = craftY + (slotSize + padding) / 2.0f;
    renderQuad(glm::vec2(resultX, resultY), glm::vec2(slotSize + 8.0f, slotSize + 8.0f), glm::vec4(0.8f, 0.7f, 0.2f, 0.9f));

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void InventoryGUI::handleMouseClick(Inventory& inventory, double mouseX, double mouseY, int button) {
    (void)button;
    float winWidth = 420.0f;
    float winHeight = 360.0f;
    float winX = (m_Width - winWidth) / 2.0f;
    float winY = (m_Height - winHeight) / 2.0f;

    float slotSize = 36.0f;
    float padding = 4.0f;
    float startX = winX + 25.0f;
    float startY = winY + 140.0f;

    int clickedSlot = -1;

    // Check main inventory 9x3
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            float x = startX + col * (slotSize + padding);
            float y = startY + row * (slotSize + padding);
            if (mouseX >= x && mouseX <= x + slotSize && mouseY >= y && mouseY <= y + slotSize) {
                clickedSlot = 9 + row * 9 + col;
            }
        }
    }

    // Check hotbar
    float hotbarY = startY + 3 * (slotSize + padding) + 15.0f;
    for (int col = 0; col < 9; ++col) {
        float x = startX + col * (slotSize + padding);
        if (mouseX >= x && mouseX <= x + slotSize && mouseY >= hotbarY && mouseY <= hotbarY + slotSize) {
            clickedSlot = col;
        }
    }

    if (clickedSlot != -1) {
        if (m_SelectedSlotIndex == -1) {
            m_SelectedSlotIndex = clickedSlot;
        } else {
            inventory.swapSlots(m_SelectedSlotIndex, clickedSlot);
            m_SelectedSlotIndex = -1;
        }
    }
}

void InventoryGUI::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    m_UIShader->use();
    m_UIShader->setVec4("u_Color", color);
    m_UIShader->setBool("u_UseTexture", false);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
    m_UIShader->setMat4("u_Projection", projection * model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

}
