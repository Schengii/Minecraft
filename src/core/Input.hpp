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
    static void updateMouseDelta();

private:
    static GLFWwindow* s_Window;
    static double s_MouseX;
    static double s_MouseY;
    static double s_LastMouseX;
    static double s_LastMouseY;
    static double s_MouseDX;
    static double s_MouseDY;
    static bool s_FirstMouse;

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};

}

#endif // INPUT_HPP
