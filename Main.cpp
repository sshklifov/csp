#if 0
#include <Draw.h>
#include <Setup.h>
#include <GLFW/glfw3.h>

#include <vector>

const int WIDTH = 1280;
const int HEIGHT = 720;
const int PIXELS = WIDTH*HEIGHT;

struct Node
{
    Node* leftChld;
    Node* rightSibl;
};

/* Node* root; */
/* void FindNode() */

static void CursorPosCallback(GLFWwindow*, double xpos, double ypos)
{
}

static void KeyCallback(GLFWwindow* window, int key, int, int action, int mods)
{
}

int main()
{
    GLFWwindow* window = InitializeGLFW(WIDTH, HEIGHT);
    Color* fb = (Color*)glfwGetWindowUserPointer(window);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    InitializeGL();

    Line line(Point(100, 100), Point(500, 500));
    Circle c(Point(500, 500), 100);
    Background bg;
    while (!glfwWindowShouldClose(window))
    {
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                int idx = y*WIDTH + x;
                bg.Draw(x, y, &fb[idx]);
                c.Draw(x, y, &fb[idx]);
                line.Draw(x, y, &fb[idx]);
            }
        }

        SwapBuffers(window, (float*)fb);
        glfwPollEvents();
    }
}
#endif
