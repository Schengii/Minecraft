#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Minecraft {

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();
    
    GLFWwindow* getNativeWindow() const { return m_Window; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }
    float getAspectRatio() const { return static_cast<float>(m_Width) / static_cast<float>(m_Height); }

    void setCursorCaptured(bool captured);
    bool isCursorCaptured() const { return m_CursorCaptured; }

private:
    GLFWwindow* m_Window = nullptr;
    int m_Width;
    int m_Height;
    std::string m_Title;
    bool m_CursorCaptured = true;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

}

#endif // WINDOW_HPP
