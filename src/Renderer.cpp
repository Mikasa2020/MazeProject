//Renderer.cpp
#include "Renderer.h"
#include "Maze.h"
#include "Cell.h"

//构造函数：初始化颜色和样式
Renderer::Renderer(){
    wall_color = IM_COL32(200, 200, 200, 255);
    path_color = IM_COL32(100, 200, 100, 200);
    empty_cell_color = IM_COL32(50, 50, 50, 255);
    start_cell_color = IM_COL32(80, 80, 255, 255);
    end_cell_color = IM_COL32(255, 80, 80, 255);
    wall_thickness = 1.5f;
}

Renderer::~Renderer(){
    // ...
}

void Renderer::renderMaze(const Maze &maze, ImVec2 canvas_top_left, float cell_render_size) const{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // (可选) 定义起点和终点 (这里只是示例，你需要根据你的 Maze 设计来获取)
    int startR = 0;
    int startC = 0;
    int endR = maze.getHeight() - 1;
    int endC = maze.getWidth() - 1;

    for (int r = 0; r < maze.getHeight(); ++r) {
        for (int c = 0; c < maze.getWidth(); ++c) {
            const Cell& current_cell = maze.getCell(r, c);
            float x0 = canvas_top_left.x + c * cell_render_size;
            float y0 = canvas_top_left.y + r * cell_render_size;
            float x1 = x0 + cell_render_size;
            float y1 = y0 + cell_render_size;

            // 1. 绘制格子背景
            if (current_cell.isPathCell()) {
                draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), path_color);
            } else if (r == startR && c == startC) {
                draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), start_cell_color);
            } else if (r == endR && c == endC) {
                draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), end_cell_color);
            } else {
                draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), empty_cell_color);
            }

            // 2. 绘制墙壁
            if (current_cell.hasWall(Direction::NORTH)) {
                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), wall_color, wall_thickness);
            }
            if (current_cell.hasWall(Direction::WEST)) {
                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), wall_color, wall_thickness);
            }
        }
    }

    // 3. 绘制整个迷宫的南边界和东边界
    float maze_total_pixel_width = maze.getWidth() * cell_render_size;
    float maze_total_pixel_height = maze.getHeight() * cell_render_size;

    draw_list->AddLine(
        ImVec2(canvas_top_left.x + maze_total_pixel_width, canvas_top_left.y),
        ImVec2(canvas_top_left.x + maze_total_pixel_width, canvas_top_left.y + maze_total_pixel_height),
        wall_color, wall_thickness);
    draw_list->AddLine(
        ImVec2(canvas_top_left.x, canvas_top_left.y + maze_total_pixel_height),
        ImVec2(canvas_top_left.x + maze_total_pixel_width, canvas_top_left.y + maze_total_pixel_height),
        wall_color, wall_thickness);
}