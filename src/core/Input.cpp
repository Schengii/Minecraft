#include "Input.hpp"

namespace Minecraft {

GLFWwindow* Input::s_Window = nullptr;
double Input::s_MouseX = 0.0;
double Input::s_MouseY = 0.0;
double Input::s_LastMouseX = 0.0;
double Input::s_LastMouseY = 0.0;
double Input::s_MouseDX = 0.0;
double Input::s_MouseDY = 0.0;
bool Input::s_FirstMouse = true;

void Input::init(GLFWwindow* window) {
    s_Window = window;
    glfwSetCursorPosCallback(window, cursorPosCallback);
}

bool Input::isKeyPressed(int key) {
    if (!s_Window) return false;
    return glfwGetKey(s_Window, key) == GLFW_PRESS;
}

bool Input::isMouseButtonPressed(int button) {
    if (!s_Window) return false;
    return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
}

void Input::updateMouseDelta() {
    s_MouseDX = 0.0;
    s_MouseDY = 0.0;
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    if (s_FirstMouse) {
        s_LastMouseX = xpos;
        s_LastMouseY = ypos;
        s_FirstMouse = false;
    }

    s_MouseX = xpos;
    s_MouseY = ypos;
    s_MouseDX = xpos - s_LastMouseX;
    s_MouseDY = s_LastMouseY - ypos; // Reversed since y-coordinates go from bottom to top

    s_LastMouseX = xpos;
    s_LastMouseY = ypos;
}

}
