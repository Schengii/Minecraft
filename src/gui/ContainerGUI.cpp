#include "ContainerGUI.hpp"
#include "FontRenderer.hpp"
#include "../renderer/TextureAtlas.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <algorithm>

namespace Minecraft {

ContainerGUI::ContainerGUI(int windowWidth, int windowHeight) 
    : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight) {
    if (glCreateShader && glGenVertexArrays) {
        m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        initBuffers();
        FontRenderer::getInstance().init();
    }
}

ContainerGUI::~ContainerGUI() {
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
}

void ContainerGUI::resize(int width, int height) {
    m_WindowWidth = width;
    m_WindowHeight = height;
    FontRenderer::getInstance().resize(width, height);
}

void ContainerGUI::initBuffers() {
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

void ContainerGUI::openChest(const glm::ivec3& pos, std::vector<ItemStack>* chestInv) {
    m_ActivePos = pos;
    m_ActiveChestInv = chestInv;
    m_ActiveContainer = ContainerType::Chest;
    m_SelectedSlot = -1;
    std::cout << "[ContainerGUI] Opened 27-slot Chest GUI at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
}

void ContainerGUI::openFurnace(const glm::ivec3& pos, FurnaceData* furnaceData) {
    m_ActivePos = pos;
    m_ActiveFurnaceData = furnaceData;
    m_ActiveContainer = ContainerType::Furnace;
    m_SelectedSlot = -1;
    std::cout << "[ContainerGUI] Opened Furnace GUI at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
}

void ContainerGUI::close() {
    m_ActiveContainer = ContainerType::None;
    m_ActiveChestInv = nullptr;
    m_ActiveFurnaceData = nullptr;
    m_SelectedSlot = -1;
}

void ContainerGUI::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    m_UIShader->use();
    m_UIShader->setVec4("u_Color", color);
    m_UIShader->setBool("u_UseTexture", false);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_WindowWidth), static_cast<float>(m_WindowHeight), 0.0f, -1.0f, 1.0f);
    m_UIShader->setMat4("u_Projection", projection * model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void ContainerGUI::renderSlotItem(const ItemStack& stack, float x, float y, float size) {
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

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_WindowWidth), static_cast<float>(m_WindowHeight), 0.0f, -1.0f, 1.0f);
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
}

void ContainerGUI::render(Inventory& playerInv) {
    if (!isOpen()) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dark screen background
    renderQuad(glm::vec2(0.0f), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec4(0.0f, 0.0f, 0.0f, 0.65f));

    float winWidth = 420.0f;
    float winHeight = 390.0f;
    float winX = (m_WindowWidth - winWidth) / 2.0f;
    float winY = (m_WindowHeight - winHeight) / 2.0f;

    // Outer frame & main panel
    renderQuad(glm::vec2(winX - 4.0f, winY - 4.0f), glm::vec2(winWidth + 8.0f, winHeight + 8.0f), glm::vec4(0.08f, 0.08f, 0.08f, 0.95f));
    renderQuad(glm::vec2(winX, winY), glm::vec2(winWidth, winHeight), glm::vec4(0.22f, 0.22f, 0.22f, 0.98f));
    renderQuad(glm::vec2(winX + 2.0f, winY + 2.0f), glm::vec2(winWidth - 4.0f, 26.0f), glm::vec4(0.18f, 0.18f, 0.18f, 0.98f));

    float slotSize = 36.0f;
    float padding = 4.0f;
    float startX = winX + 25.0f;

    if (m_ActiveContainer == ContainerType::Chest && m_ActiveChestInv) {
        FontRenderer::getInstance().renderText("Chest (27 Slots)", winX + 20.0f, winY + 8.0f, 1.0f, glm::vec4(0.95f, 0.85f, 0.3f, 1.0f));

        // 3x9 Chest Slots (Top section)
        float chestY = winY + 35.0f;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 9; ++c) {
                int idx = r * 9 + c;
                float x = startX + c * (slotSize + padding);
                float y = chestY + r * (slotSize + padding);

                glm::vec4 slotColor = (idx == m_SelectedSlot) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
                renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), slotColor);
                renderQuad(glm::vec2(x + 2.0f, y + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

                if (idx < static_cast<int>(m_ActiveChestInv->size())) {
                    renderSlotItem((*m_ActiveChestInv)[idx], x + 4.0f, y + 4.0f, slotSize - 8.0f);
                }
            }
        }
    } else if (m_ActiveContainer == ContainerType::Furnace && m_ActiveFurnaceData) {
        FontRenderer::getInstance().renderText("Furnace", winX + 20.0f, winY + 8.0f, 1.0f, glm::vec4(0.95f, 0.85f, 0.3f, 1.0f));

        float furnaceCenterX = winX + winWidth / 2.0f;
        float inputX = furnaceCenterX - 60.0f;
        float inputY = winY + 40.0f;
        float fuelY = winY + 100.0f;
        float outputX = furnaceCenterX + 35.0f;
        float outputY = winY + 65.0f;

        // Input slot
        glm::vec4 inColor = (m_SelectedSlot == 0) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
        renderQuad(glm::vec2(inputX, inputY), glm::vec2(slotSize, slotSize), inColor);
        renderQuad(glm::vec2(inputX + 2.0f, inputY + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));
        renderSlotItem(m_ActiveFurnaceData->input, inputX + 4.0f, inputY + 4.0f, slotSize - 8.0f);

        // Fuel slot
        glm::vec4 fuelColor = (m_SelectedSlot == 1) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
        renderQuad(glm::vec2(inputX, fuelY), glm::vec2(slotSize, slotSize), fuelColor);
        renderQuad(glm::vec2(inputX + 2.0f, fuelY + 2.0f), glm::vec2(slotSize - 4.0f, fuelY - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));
        renderSlotItem(m_ActiveFurnaceData->fuel, inputX + 4.0f, fuelY + 4.0f, slotSize - 8.0f);

        // Animated Flame Indicator
        float flameRatio = std::clamp(m_ActiveFurnaceData->fuelBurnTimer / 10.0f, 0.0f, 1.0f);
        float flameH = 14.0f * flameRatio;
        renderQuad(glm::vec2(inputX + 11.0f, inputY + slotSize + 4.0f + (14.0f - flameH)), glm::vec2(14.0f, flameH), glm::vec4(1.0f, 0.4f, 0.1f, 0.95f));

        // Animated Smelting Cook Progress Bar Arrow
        float cookRatio = std::clamp(m_ActiveFurnaceData->cookTimer / m_ActiveFurnaceData->cookTimeMax, 0.0f, 1.0f);
        renderQuad(glm::vec2(inputX + slotSize + 8.0f, outputY + 12.0f), glm::vec2(36.0f, 10.0f), glm::vec4(0.12f, 0.12f, 0.12f, 0.9f));
        renderQuad(glm::vec2(inputX + slotSize + 8.0f, outputY + 12.0f), glm::vec2(36.0f * cookRatio, 10.0f), glm::vec4(0.3f, 0.9f, 0.3f, 0.95f));
        FontRenderer::getInstance().renderText("->", inputX + slotSize + 14.0f, outputY + 4.0f, 1.5f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));

        // Output slot (larger)
        glm::vec4 outColor = (m_SelectedSlot == 2) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(1.0f, 0.84f, 0.0f, 0.6f);
        renderQuad(glm::vec2(outputX, outputY), glm::vec2(slotSize + 8.0f, slotSize + 8.0f), outColor);
        renderQuad(glm::vec2(outputX + 2.0f, outputY + 2.0f), glm::vec2(slotSize + 4.0f, slotSize + 4.0f), glm::vec4(0.32f, 0.32f, 0.32f, 0.95f));
        renderSlotItem(m_ActiveFurnaceData->output, outputX + 4.0f, outputY + 4.0f, slotSize);
    }

    // Player Inventory (3x9 + 1x9 hotbar at bottom)
    float invStartY = winY + 170.0f;
    FontRenderer::getInstance().renderText("Inventory", winX + 25.0f, invStartY - 15.0f, 1.0f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            int slotIdx = 9 + row * 9 + col;
            float x = startX + col * (slotSize + padding);
            float y = invStartY + row * (slotSize + padding);

            glm::vec4 slotColor = (slotIdx + 100 == m_SelectedSlot) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
            renderQuad(glm::vec2(x, y), glm::vec2(slotSize, slotSize), slotColor);
            renderQuad(glm::vec2(x + 2.0f, y + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

            renderSlotItem(playerInv.getSlot(slotIdx), x + 4.0f, y + 4.0f, slotSize - 8.0f);
        }
    }

    // Player Hotbar
    float hotbarY = invStartY + 3 * (slotSize + padding) + 12.0f;
    for (int col = 0; col < 9; ++col) {
        float x = startX + col * (slotSize + padding);
        glm::vec4 slotColor = (col + 100 == m_SelectedSlot) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.15f, 0.15f, 0.15f, 0.85f);
        renderQuad(glm::vec2(x, hotbarY), glm::vec2(slotSize, slotSize), slotColor);
        renderQuad(glm::vec2(x + 2.0f, hotbarY + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.28f, 0.28f, 0.28f, 0.9f));

        renderSlotItem(playerInv.getSlot(col), x + 4.0f, hotbarY + 4.0f, slotSize - 8.0f);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

bool ContainerGUI::handleMouseClick(Inventory& playerInv, double mouseX, double mouseY, int button) {
    if (!isOpen()) return false;
    (void)button;

    float winWidth = 420.0f;
    float winHeight = 390.0f;
    float winX = (m_WindowWidth - winWidth) / 2.0f;
    float winY = (m_WindowHeight - winHeight) / 2.0f;

    float slotSize = 36.0f;
    float padding = 4.0f;
    float startX = winX + 25.0f;

    int clickedSlot = -1;

    // Check Chest Slots
    if (m_ActiveContainer == ContainerType::Chest && m_ActiveChestInv) {
        float chestY = winY + 35.0f;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 9; ++c) {
                float x = startX + c * (slotSize + padding);
                float y = chestY + r * (slotSize + padding);
                if (mouseX >= x && mouseX <= x + slotSize && mouseY >= y && mouseY <= y + slotSize) {
                    clickedSlot = r * 9 + c;
                }
            }
        }
    } else if (m_ActiveContainer == ContainerType::Furnace && m_ActiveFurnaceData) {
        float furnaceCenterX = winX + winWidth / 2.0f;
        float inputX = furnaceCenterX - 60.0f;
        float inputY = winY + 40.0f;
        float fuelY = winY + 100.0f;
        float outputX = furnaceCenterX + 35.0f;
        float outputY = winY + 65.0f;

        if (mouseX >= inputX && mouseX <= inputX + slotSize && mouseY >= inputY && mouseY <= inputY + slotSize) {
            clickedSlot = 0; // Input
        } else if (mouseX >= inputX && mouseX <= inputX + slotSize && mouseY >= fuelY && mouseY <= fuelY + slotSize) {
            clickedSlot = 1; // Fuel
        } else if (mouseX >= outputX && mouseX <= outputX + slotSize + 8.0f && mouseY >= outputY && mouseY <= outputY + slotSize + 8.0f) {
            clickedSlot = 2; // Output
        }
    }

    // Check Player Inv (3x9)
    float invStartY = winY + 170.0f;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 9; ++col) {
            float x = startX + col * (slotSize + padding);
            float y = invStartY + row * (slotSize + padding);
            if (mouseX >= x && mouseX <= x + slotSize && mouseY >= y && mouseY <= y + slotSize) {
                clickedSlot = 100 + 9 + row * 9 + col;
            }
        }
    }

    // Check Player Hotbar (1x9)
    float hotbarY = invStartY + 3 * (slotSize + padding) + 12.0f;
    for (int col = 0; col < 9; ++col) {
        float x = startX + col * (slotSize + padding);
        if (mouseX >= x && mouseX <= x + slotSize && mouseY >= hotbarY && mouseY <= hotbarY + slotSize) {
            clickedSlot = 100 + col;
        }
    }

    if (clickedSlot == -1) return false;

    if (m_SelectedSlot == -1) {
        m_SelectedSlot = clickedSlot;
        return true;
    }

    // Perform swap or transfer between selected slot and clicked slot
    auto getSlotRef = [&](int slotId) -> ItemStack* {
        if (slotId >= 100) {
            return &playerInv.getSlot(slotId - 100);
        }
        if (m_ActiveContainer == ContainerType::Chest && m_ActiveChestInv && slotId < static_cast<int>(m_ActiveChestInv->size())) {
            return &(*m_ActiveChestInv)[slotId];
        }
        if (m_ActiveContainer == ContainerType::Furnace && m_ActiveFurnaceData) {
            if (slotId == 0) return &m_ActiveFurnaceData->input;
            if (slotId == 1) return &m_ActiveFurnaceData->fuel;
            if (slotId == 2) return &m_ActiveFurnaceData->output;
        }
        return nullptr;
    };

    ItemStack* itemA = getSlotRef(m_SelectedSlot);
    ItemStack* itemB = getSlotRef(clickedSlot);

    if (itemA && itemB) {
        // If clicking output slot of furnace, only take output into inventory
        if (m_ActiveContainer == ContainerType::Furnace && m_SelectedSlot == 2) {
            if (clickedSlot >= 100 && !itemA->isEmpty()) {
                if (itemB->isEmpty()) {
                    *itemB = *itemA;
                    itemA->clear();
                } else if (itemB->type == itemA->type) {
                    itemB->count += itemA->count;
                    itemA->clear();
                }
            }
        } else {
            std::swap(*itemA, *itemB);
        }
    }

    m_SelectedSlot = -1;
    return true;
}

}
