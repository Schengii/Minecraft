#include "Window.hpp"
#include <iostream>

namespace Minecraft {

Window::Window(int width, int height, const std::string& title)
    : m_Width(width), m_Height(height), m_Title(title) 
{
    if (!glfwInit()) {
        std::cerr << "[Window Error] Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "[Window Error] Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Window Error] Failed to initialize GLAD loader!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, m_Width, m_Height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    setCursorCaptured(true);
}

Window::~Window() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_Window);
}

void Window::setCursorCaptured(bool captured) {
    m_CursorCaptured = captured;
    if (captured) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        win->m_Width = width;
        win->m_Height = height;
        glViewport(0, 0, width, height);
    }
}

}
