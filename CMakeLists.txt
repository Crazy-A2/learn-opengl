# 指定构建项目所需的cmake最低版本
cmake_minimum_required(VERSION 3.10)

# 指定项目名
project(learn_opengl)

# 指定C++标准
set(CMAKE_CXX_STANDARD 20)

# 将源码目录(src)赋值给变量 SOURCE_FILES
aux_source_directory(src SOURCE_FILES)

# 指定生成可执行文件 使用变量
add_executable(learn_opengl ${SOURCE_FILES})

# 指定可执行文件的生成位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 链接静态库
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    message("Current-OS-Linux!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    target_link_libraries(learn_opengl ${PROJECT_SOURCE_DIR}/lib/libglfw3.a libGL.so)
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    message("current platform: Windows")
    target_link_libraries(learn_opengl ${PROJECT_SOURCE_DIR}/lib/glfw3.lib Opengl32.lib)
endif ()

# 添加 include 目录
include_directories(${PROJECT_SOURCE_DIR}/include)
