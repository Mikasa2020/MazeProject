//Renderer.h
#ifndef RENDERER_H
#define RENDERER_H

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

#include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#include "Maze.h"
//class Maze;
class Renderer{
public:
    Renderer();
    ~Renderer();

    // 核心的绘制函数
    void renderMaze(const Maze& maze, 
                    ImVec2 canvas_top_left, 
                    float cell_render_size) const;

    // (将来可以添加其他绘制函数，比如绘制迷宫的解决方案，或UI控件相关的渲染)

private:
    // 将颜色和样式作为 Renderer 的成员，方便统一管理和修改
    ImU32 wall_color;       // 墙
    ImU32 path_color;       // 路
    ImU32 empty_cell_color; // 空格子
    ImU32 start_cell_color; // 起点
    ImU32 end_cell_color;   // 终点
    float wall_thickness;   // 路宽

};
#endif