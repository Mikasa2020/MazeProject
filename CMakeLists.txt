#指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.12)

#定义项目名称和可选版本号
project(maze VERSION 1.0)

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 OpenGL 和 GLFW
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# --- GLAD 文件 --- 
set(GLAD_SOURCES lib/glad/src/glad.c)

# --- Dear ImGui 文件 --- 
#include_directories(lib/imgui)
#include_directories(lib/imgui/backends)
set(IMGUI_SOURCES
    lib/imgui-1.91.9/imgui.cpp
    lib/imgui-1.91.9/imgui_draw.cpp
    lib/imgui-1.91.9/imgui_tables.cpp
    lib/imgui-1.91.9/imgui_widgets.cpp
    lib/imgui-1.91.9/imgui_demo.cpp
    lib/imgui-1.91.9/backends/imgui_impl_glfw.cpp
    lib/imgui-1.91.9/backends/imgui_impl_opengl3.cpp
)

#添加可执行文件目标
add_executable(mazeApp
    src/main.cpp 
    src/Cell.cpp 
    src/Maze.cpp
    src/Renderer.cpp
    ${GLAD_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(mazeApp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(mazeApp PRIVATE # 这些是第三方库的包含路径，设为PRIVATE即可
    lib/glad/include
    lib/imgui-1.91.9
    lib/imgui-1.91.9/backends
)

# --- 链接库 ---
target_link_libraries(mazeApp PRIVATE
    glfw # GLFW 库的 CMake 目标名，通常是 glfw
    OpenGL::GL # OpenGL 库的 CMake 目标名
)