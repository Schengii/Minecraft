#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Minecraft {

class Input {
public:
    static void init(GLFWwindow* window);
    static bool isKeyPressed(int key);
    static bool isMouseButtonPressed(int button);

    static double getMouseX() { return s_MouseX; }
    static double getMouseY() { return s_MouseY; }
    static double getMouseDX() { return s_MouseDX; }
    static double getMouseDY() { return s_MouseDY; }
    static double getScrollY() { return s_ScrollY; }
    static void resetScroll() { s_ScrollY = 0.0; }
    static void updateMouseDelta();

private:
    static GLFWwindow* s_Window;
    static double s_MouseX;
    static double s_MouseY;
    static double s_LastMouseX;
    static double s_LastMouseY;
    static double s_MouseDX;
    static double s_MouseDY;
    static double s_ScrollY;
    static bool s_FirstMouse;

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

}

#endif // INPUT_HPP
