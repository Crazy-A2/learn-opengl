#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

import newtype;

using namespace std;

// 定义 MSVC 断言宏 捕捉到 false 时中断程序
#define ASSERT(x)       \
    if (!(x)) {         \
        __debugbreak(); \
    }

// 定义宏 捕捉 OpenGL 错误并输出错误信息
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) // #x 会将 x 转换为字符串

// 用于清除命令行中的错误信息
static inline void GLClearError()
{
    // 从错误队列中循环读取错误，直到没有错误为止
    // 用来清除错误队列中的所有错误
    while (glGetError() != GL_NO_ERROR)
        ;
}

/**
 * @brief 用于打印 OpenGL 错误信息
 *
 * @param functionName 函数名
 * @param fileName 文件名
 * @param line 行号
 * @return true 没有错误
 * @return false 有错误
 */
static inline bool GLLogCall(const char* functionName, const char* fileName, u32 line)
{
    while (u32 error = glGetError()) {
        printf("[OpenGL Error] (%d): 执行 %s 时报错，报错位置是 %s: 第%d行\n", error, functionName, fileName, line);
        return false;
    }
    return true;
}

struct ShaderProgramSource final {
    string vertexSource;
    string fragmentSource;
};

/**
 * @brief 读取 shader 文件并生成 ShaderProgramSource
 *
 * @param filePath shader 文件路径
 * @return ShaderProgramSource 着色器源码
 */
static ShaderProgramSource parseShader(const string& filePath)
{
    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ifstream stream(filePath);
    if (!stream.is_open()) {
        cerr << "无法打开文件: " << filePath << endl;
        return { "", "" };
    }

    string line;
    stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    // 结构化读取到的 shader 代码
    while (getline(stream, line)) {
        if (line.find("#shader") != string::npos) {
            if (line.find("vertex") != string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(i32)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

/**
 * @brief 编译着色器
 *
 * 着色器是一种小型程序，用于在 GPU 上运行。它们用于控制渲染管道的某些阶段
 * @param type 着色器类型
 * @param source 着色器源码
 * @return 着色器 ID
 */
static u32 compileShader(u32 type, const string& source)
{
    GLCall(u32 id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    i32 result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        i32 length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        // 在栈上动态分配内存
        // alloca 分配的内存在函数返回时会自动释放
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        printf("%s着色器编译失败: %s\n", type == GL_VERTEX_SHADER ? "顶点" : "片段", message);
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

/**
 * @brief 创建着色器程序
 *
 * @param vertexShader 顶点着色器
 * @param fragmentShader 片段着色器
 * @return 着色器程序 ID
 */
static inline u32 createSharer(const string& vertexShader, const string& fragmentShader)
{
    GLCall(u32 program = glCreateProgram());
    u32 vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    u32 fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

// 设置错误回调函数
// 大多数事件都是通过回调报告的，无论是按下的键、移动的 GLFW 窗口还是发生的错误
// 如果 GLFW 函数失败，则会向 GLFW 错误回调报错
inline void errorCallback(i32 error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// 设置键盘回调函数
// 每个窗口都有大量的回调，可以设置为接收所有各种类型的事件。
// @param window 窗口句柄
static inline void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
    // 当按下 ESC 键时，关闭窗口
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

i32 main(void)
{
    // 必须设置错误回调函数，以便 GLFW 知道要调用它们。
    // 用于设置错误回调的函数是少数几个可以在初始化之前调用的 GLFW 函数之一
    // 它允许您在初始化期间和初始化后收到错误通知。
    glfwSetErrorCallback(errorCallback);

    // 如果初始化失败，退出程序
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // 窗口及其 OpenGL 上下文是通过对 glfwCreateWindow 的一次调用创建的
    // 该调用返回创建的组合窗口和上下文对象的句柄
    GLFWwindow* window = glfwCreateWindow(640, 480, "这是窗口标题", NULL, NULL);
    if (!window) {
        printf("Window or OpenGL 上下文创建失败\n");
        exit(EXIT_FAILURE);
    }

    // 为了在窗口中绘制，需要将 OpenGL 上下文设置为当前上下文
    glfwMakeContextCurrent(window);

    // GLEW 是一个开源的 C/C++ 扩展库，用于管理 OpenGL 扩展
    // 在使用任何 OpenGL 函数之前，必须初始化 GLEW
    // GLEW 使用当前上下文，所以必须在创建窗口和设置当前上下文之后初始化 GLEW
    if (glewInit() != GLEW_OK) {
        printf("GLEW 初始化失败\n");
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInit()));
        exit(EXIT_FAILURE);
    }

    // 打印 OpenGL 版本
    printf("%s\n", glGetString(GL_VERSION));

    // 与其他窗口相关的回调一样，按键回调是按窗口设置的。
    glfwSetKeyCallback(window, keyCallback);

    // 顶点数据
    f32 positions[] {
        -0.5f, -0.5f, // 索引 0
        0.5f, -0.5f, //  索引 1
        0.5f, 0.5f, //   索引 2
        -0.5f, 0.5f //   索引 3
    };

    // 为了绘制图形，需要创建一个顶点缓冲区对象（buffer）（字节数组）
    // buffer 是一个内存缓冲区，通常位于显存中，用于存储顶点数据
    // 通过将顶点数据存储在 buffer 中，可以更快地绘制它们，因为它们存储在 GPU 内存中
    u32 buffer;
    // 1 表示创建一个缓冲区对象，缓冲区 ID 存储在 buffer 中
    GLCall(glGenBuffers(1, &buffer));

    // 为了将数据绑定到缓冲区，需要将缓冲区绑定到 GL_ARRAY_BUFFER 目标
    // 之后，所有对 GL_ARRAY_BUFFER 的操作都将应用于当前绑定的缓冲区
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

    // 为了将数据复制到缓冲区，需要调用 glBufferData 函数
    // 第一个参数是目标，第二个参数是要复制的数据的大小（以字节为单位）
    // 第三个参数是要复制的数据，第四个参数是如何使用这些数据
    // GL_STATIC_DRAW 表示数据不会经常更改，GL_DYNAMIC_DRAW 表示数据会经常更改
    // GL_STREAM_DRAW 表示数据每次使用时都会更改
    GLCall(glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(f32), positions, GL_STATIC_DRAW));

    // 启用通用顶点属性数据的数组 参数 0 表示顶点属性的索引（第几个顶点属性）
    GLCall(glEnableVertexAttribArray(0));
    // 定义通用顶点属性数据的数组
    // 0 表示顶点属性的索引（第几个顶点属性），2 表示顶点属性占几个数组元素（项），GL_FLOAT 表示顶点属性的类型
    // GL_FALSE 表示是否要标准化，2 * sizeof(f32) 表示顶点之间间隔的字节数，0 表示指向实际属性的指针（顶点属性在顶点内的偏移量）
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), 0));

    // 索引缓冲区
    // 顶点缓冲区中的顶点是按顺序排列的，直接绘制所有顶点会浪费很多显存
    // 在 3D 图形绘制顶点时可节省更多显存，节省程度取决于顶点数量
    u32 indices[] {
        0, 1, 2, // 第一个三角形
        2, 3, 0 //  第二个三角形
    };

    u32 ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(u32), indices, GL_STATIC_DRAW));

    // 交换间隔表示在交换缓冲区（通常称为 vsync）之前要等待的帧数。
    // 默认情况下，交换间隔为零，这意味着缓冲区交换将立即发生。
    // 在快速的机器上，其中许多帧将永远不会被看到，因为屏幕通常只更新每秒 60-75 次，
    // 因此这会浪费大量的 CPU 和 GPU 周期。
    // 此外，由于缓冲区将在中间交换，因此屏幕会更新，从而导致屏幕撕裂。
    // 由于这些原因，应用程序通常需要将交换间隔设置为 1。
    // 可以将其设置为更高的值，但通常不建议这样做，因为它会导致输入延迟。
    glfwSwapInterval(1);

    ShaderProgramSource source = parseShader(format("{}/src/triangle.shader", PROJECT_DIR));
    printf("Vertex Shader:\n%s\n", source.vertexSource.c_str());
    printf("Fragment Shader:\n%s\n", source.fragmentSource.c_str());

    u32 shader = createSharer(source.vertexSource, source.fragmentSource);
    GLCall(glUseProgram(shader));

    // 设置 uniform 变量
    // glGetUniformLocation 函数返回 uniform 变量的位置值，如果返回 -1，则表示 uniform 变量不存在
    GLCall(i32 location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    f32 r = 0.0f;
    f32 increment = 0.05f;

    // 每个窗口都有一个标志，指示是否应关闭该窗口。
    // 当用户尝试通过按标题栏中的关闭小组件或使用 Alt+F4 等组合键来关闭窗口时，此标志将设置为 1。
    // 请注意，窗口实际上并未关闭，因此您需要监控此标志并销毁窗口或向用户提供某种反馈。
    while (!glfwWindowShouldClose(window)) {
        // 渲染 OpenGL 内容

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        // 用 OpenGL 渲染一个三角形
        // 参数 1 表示渲染的图元类型，参数 2 表示从顶点数组的第几个顶点开始渲染，参数 3 表示渲染多少个顶点
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // 参数1 表示渲染的图元类型，参数2 表示索引缓冲区中的偏移量，参数3 表示渲染多少个顶点
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // 自定义宏在执行代码时捕捉 OpenGL 错误并在命令行输出错误信息
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f) {
            increment = -0.05f;
        } else if (r < 0.0f) {
            increment = 0.05f;
        }
        r += increment;

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

    glDeleteProgram(shader);

    // 使用 GLFW 后（通常在应用程序退出之前）, 需要终止 GLFW
    // 这将销毁所有剩余的窗口并释放 GLFW 分配的任何其他资源
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
