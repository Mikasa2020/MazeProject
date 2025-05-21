#指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

#定义项目名称和可选版本号
project(maze VERSION 1.0)

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# (可选) 添加编译选项，比如警告和调试信息
# 对于GCC/Clang:
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")
endif()

#添加可执行文件目标
add_executable(mazeApp main.cpp Cell.cpp Maze.cpp)