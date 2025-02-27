// 顶点着色器
#version 330 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

// 片段着色器
#version 330 core

layout(location = 0) out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
}