#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// 设置错误回调函数
// 大多数事件都是通过回调报告的，无论是按下的键、移动的 GLFW 窗口还是发生的错误
// 如果 GLFW 函数失败，则会向 GLFW 错误回调报错
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// 设置键盘回调函数
// 每个窗口都有大量的回调，可以设置为接收所有各种类型的事件。
// 要接收 key press 和 release 事件，请创建一个 key callback 函数。
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // 当按下 ESC 键时，关闭窗口
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(void)
{
    // 必须设置错误回调函数，以便 GLFW 知道要调用它们。
    // 用于设置错误回调的函数是少数几个可以在初始化之前调用的 GLFW 函数之一
    // 它允许您在初始化期间和初始化后收到错误通知。
    glfwSetErrorCallback(error_callback);

    // 如果初始化失败，退出程序
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // 窗口及其 OpenGL 上下文是通过对 glfwCreateWindow 的一次调用创建的
    // 该调用返回创建的组合窗口和上下文对象的句柄
    GLFWwindow* window = glfwCreateWindow(640, 480, "这是窗口标题", NULL, NULL);
    if (!window) {
        printf("Window or OpenGL 上下文创建失败\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 为了在窗口中绘制，需要将 OpenGL 上下文设置为当前上下文
    glfwMakeContextCurrent(window);

    // 与其他窗口相关的回调一样，按键回调是按窗口设置的。
    glfwSetKeyCallback(window, key_callback);

    // 交换间隔表示在交换缓冲区（通常称为 vsync）之前要等待的帧数。
    // 默认情况下，交换间隔为零，这意味着缓冲区交换将立即发生。
    // 在快速的机器上，其中许多帧将永远不会被看到，因为屏幕通常只更新每秒 60-75 次，
    // 因此这会浪费大量的 CPU 和 GPU 周期。
    // 此外，由于缓冲区将在中间交换，因此屏幕会更新，从而导致屏幕撕裂。
    // 由于这些原因，应用程序通常需要将交换间隔设置为 1。
    // 可以将其设置为更高的值，但通常不建议这样做，因为它会导致输入延迟。
    glfwSwapInterval(1);

    // 每个窗口都有一个标志，指示是否应关闭该窗口。
    // 当用户尝试通过按标题栏中的关闭小组件或使用 Alt+F4 等组合键来关闭窗口时，此标志将设置为 1。
    // 请注意，窗口实际上并未关闭，因此您需要监控此标志并销毁窗口或向用户提供某种反馈。
    while (!glfwWindowShouldClose(window)) {
        // 渲染 OpenGL 内容
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        // 默认情况下，GLFW 窗口使用双缓冲。这意味着每个窗口都有两个渲染缓冲区;前缓冲区和后缓冲区。
        // 前缓冲区是要显示的缓冲区，而后缓冲区是渲染到的缓冲区。
        // 渲染整个帧后，缓冲区需要相互交换，因此 back 缓冲区成为 front 缓冲区，反之亦然。
        glfwSwapBuffers(window);

        // GLFW 需要定期与窗口系统通信，以便接收事件并显示应用程序尚未锁定。
        // 当您有可见窗口时，必须定期进行事件处理，并且通常在缓冲区交换后的每一帧完成。
        // 有两种方法可以处理待处理事件;轮询和等待。此示例将使用事件轮询，它仅处理已接收的事件，然后立即返回。
        glfwPollEvents();

        // 与大多数游戏一样，在连续渲染时，glfwPollEvents 是最佳选择。
        // 相反，如果只需要在收到新输入后更新渲染，则 glfwWaitEvents 是更好的选择。
        // 它会一直等待，直到至少收到一个事件，同时使线程进入休眠状态，然后处理所有收到的事件。
        // 这样可以节省大量的 CPU 周期，并且对于多种编辑工具非常有用。
        // glfwWaitEvents();
    }

    // 当不再需要窗口和上下文时，将其销毁。
    glfwDestroyWindow(window);

    // 使用 GLFW 后（通常在应用程序退出之前）, 需要终止 GLFW
    // 这将销毁所有剩余的窗口并释放 GLFW 分配的任何其他资源
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
