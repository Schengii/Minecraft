#include "InventoryGUI.hpp"
#include "FontRenderer.hpp"
#include "../renderer/TextureAtlas.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Minecraft {

InventoryGUI::InventoryGUI(int windowWidth, int windowHeight)
    : m_Width(windowWidth), m_Height(windowHeight)
{
    if (glCreateShader && glGenVertexArrays) {
        m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        initBuffers();
        FontRenderer::getInstance().init();
    }
}

InventoryGUI::~InventoryGUI() {
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
}

void InventoryGUI::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    FontRenderer::getInstance().resize(width, height);
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

    // Window shadow & body
    renderQuad(glm::vec2(winX - 4.0f, winY - 4.0f), glm::vec2(winWidth + 8.0f, winHeight + 8.0f), glm::vec4(0.08f, 0.08f, 0.08f, 0.95f));
    renderQuad(glm::vec2(winX, winY), glm::vec2(winWidth, winHeight), glm::vec4(0.22f, 0.22f, 0.22f, 0.98f));
    renderQuad(glm::vec2(winX + 2.0f, winY + 2.0f), glm::vec2(winWidth - 4.0f, 26.0f), glm::vec4(0.18f, 0.18f, 0.18f, 0.98f));

    // Window Titles
    FontRenderer::getInstance().renderText("Crafting", winX + 220.0f, winY + 10.0f, 1.0f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));
    FontRenderer::getInstance().renderText("Inventory", winX + 25.0f, winY + 125.0f, 1.0f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));

    float slotSize = 36.0f;
    float padding = 4.0f;
    float startX = winX + 25.0f;
    float startY = winY + 140.0f;

    auto renderSlotItem = [&](const ItemStack& stack, float x, float y, float size) {
        if (stack.isEmpty()) return;
        glm::vec2 uv = TextureAtlas::getBlockUV(stack.type, Direction::TOP);
        float uvSize = 1.0f / 16.0f;

        m_UIShader->use();
        m_UIShader->setBool("u_UseTexture", true);
        if (glActiveTexture) glActiveTexture(GL_TEXTURE0);
        TextureAtlas::getInstance().bind(0);
        m_UIShader->setInt("u_Texture", 0);
        m_UIShader->setVec4("u_Color", glm::vec4(1.0f));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(size, size, 1.0f));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
        m_UIShader->setMat4("u_Projection", projection * model);

        float quad[24] = {
            0.0f, 1.0f, uv.x, uv.y + uvSize,
            1.0f, 0.0f, uv.x + uvSize, uv.y,
            0.0f, 0.0f, uv.x, uv.y,
            0.0f, 1.0f, uv.x, uv.y + uvSize,
            1.0f, 1.0f, uv.x + uvSize, uv.y + uvSize,
            1.0f, 0.0f, uv.x + uvSize, uv.y
        };
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        float defaultQuad[24] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(defaultQuad), defaultQuad);
        glBindVertexArray(0);
        TextureAtlas::getInstance().unbind();

        if (stack.count > 1) {
            std::string countStr = std::to_string(stack.count);
            FontRenderer::getInstance().renderText(countStr, x + size - 10.0f, y + size - 10.0f, 1.0f, glm::vec4(1.0f), true);
        }
    };

    // Render 9x3 Main Inventory Slots
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            int slotIdx = 9 + row * 9 + col;
            float x = startX + col * (slotSize + padding);
            float y = startY + row * (slotSize + padding);

            glm::vec4 slotColor = (slotIdx == m_SelectedSlotIndex) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
            renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), slotColor);
            renderQuad(glm::vec2(x + 2.0f, y + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

            const ItemStack& stack = inventory.getSlot(slotIdx);
            renderSlotItem(stack, x + 4.0f, y + 4.0f, slotSize - 8.0f);
        }
    }

    // Render 9 Hotbar Slots at bottom of window
    float hotbarY = startY + 3 * (slotSize + padding) + 15.0f;
    for (int col = 0; col < 9; ++col) {
        float x = startX + col * (slotSize + padding);
        glm::vec4 slotColor = (col == m_SelectedSlotIndex) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
        renderQuad(glm::vec2(x, hotbarY), glm::vec2(slotSize, slotSize), slotColor);
        renderQuad(glm::vec2(x + 2.0f, hotbarY + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

        const ItemStack& stack = inventory.getSlot(col);
        renderSlotItem(stack, x + 4.0f, hotbarY + 4.0f, slotSize - 8.0f);
    }

    // Render 2x2 Crafting Grid
    float craftX = winX + 220.0f;
    float craftY = winY + 25.0f;
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            int craftIdx = r * 2 + c;
            float x = craftX + c * (slotSize + padding);
            float y = craftY + r * (slotSize + padding);

            glm::vec4 slotColor = (craftIdx + 100 == m_SelectedSlotIndex) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
            renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), slotColor);
            renderQuad(glm::vec2(x + 2.0f, y + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

            const ItemStack& craftStack = inventory.getCraftingInput(craftIdx);
            renderSlotItem(craftStack, x + 4.0f, y + 4.0f, slotSize - 8.0f);
        }
    }

    // Render Crafting Arrow Icon ->
    FontRenderer::getInstance().renderText("->", craftX + 2 * (slotSize + padding) + 6.0f, craftY + (slotSize + padding) / 2.0f + 6.0f, 1.5f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));

    // Render Crafting Result Slot
    float resultX = craftX + 2 * (slotSize + padding) + 32.0f;
    float resultY = craftY + (slotSize + padding) / 2.0f - 2.0f;
    renderQuad(glm::vec2(resultX, resultY), glm::vec2(slotSize + 8.0f, slotSize + 8.0f), glm::vec4(1.0f, 0.84f, 0.0f, 0.9f));
    renderQuad(glm::vec2(resultX + 2.0f, resultY + 2.0f), glm::vec2(slotSize + 4.0f, slotSize + 4.0f), glm::vec4(0.32f, 0.32f, 0.32f, 0.95f));

    const ItemStack& outputStack = inventory.getCraftingOutput();
    renderSlotItem(outputStack, resultX + 4.0f, resultY + 4.0f, slotSize);

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

    // Check 2x2 Crafting Grid
    float craftX = winX + 220.0f;
    float craftY = winY + 25.0f;
    int clickedCraftSlot = -1;
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            float x = craftX + c * (slotSize + padding);
            float y = craftY + r * (slotSize + padding);
            if (mouseX >= x && mouseX <= x + slotSize && mouseY >= y && mouseY <= y + slotSize) {
                clickedCraftSlot = r * 2 + c;
            }
        }
    }

    // Check Crafting Output Slot
    float resultX = craftX + 2 * (slotSize + padding) + 32.0f;
    float resultY = craftY + (slotSize + padding) / 2.0f - 2.0f;
    bool clickedResult = (mouseX >= resultX && mouseX <= resultX + slotSize + 8.0f && mouseY >= resultY && mouseY <= resultY + slotSize + 8.0f);

    if (clickedCraftSlot != -1) {
        if (m_SelectedSlotIndex >= 0 && m_SelectedSlotIndex < 36) {
            std::swap(inventory.getSlot(m_SelectedSlotIndex), inventory.getCraftingInput(clickedCraftSlot));
            inventory.updateCraftingRecipe();
            m_SelectedSlotIndex = -1;
        } else if (m_SelectedSlotIndex >= 100 && m_SelectedSlotIndex < 104) {
            int prevCraftIdx = m_SelectedSlotIndex - 100;
            std::swap(inventory.getCraftingInput(prevCraftIdx), inventory.getCraftingInput(clickedCraftSlot));
            inventory.updateCraftingRecipe();
            m_SelectedSlotIndex = -1;
        } else {
            m_SelectedSlotIndex = clickedCraftSlot + 100;
        }
    } else if (clickedResult) {
        ItemStack output = inventory.getCraftingOutput();
        if (!output.isEmpty()) {
            if (inventory.addItem(output.type, output.count)) {
                // Consume 1 item from each crafting input slot
                for (int i = 0; i < 4; ++i) {
                    ItemStack& input = inventory.getCraftingInput(i);
                    if (!input.isEmpty()) {
                        input.count--;
                        if (input.count <= 0) input.clear();
                    }
                }
                inventory.updateCraftingRecipe();
            }
        }
    } else if (clickedSlot != -1) {
        if (m_SelectedSlotIndex == -1) {
            m_SelectedSlotIndex = clickedSlot;
        } else if (m_SelectedSlotIndex >= 100 && m_SelectedSlotIndex < 104) {
            int craftIdx = m_SelectedSlotIndex - 100;
            std::swap(inventory.getCraftingInput(craftIdx), inventory.getSlot(clickedSlot));
            inventory.updateCraftingRecipe();
            m_SelectedSlotIndex = -1;
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
