//Maze.h
#ifndef MAZE_H
#define MAZE_H

#include "Cell.h"
#include <vector>
#include <random>


class Maze{
public:
    //构造函数：传入迷宫的宽度和高度
    Maze(int Maze, int height);

    // (如果使用动态分配的 Cell** grid，则需要析构函数来释放内存)
    // ~Maze();

    void generate(); // 生成迷宫的逻辑在这里调用
    //bool solve(int startRow, int startCol, int endRow, int endCol); // 解决迷宫

    void display() const; // 显示迷宫

    // 辅助方法
    const Cell &getCell(int row, int col) const; // 获取指定位置的格子(返回引用)
    Cell &getCell(int raw, int col);

    int getWidth() const;
    int getHeight() const;

    //检查坐标是否在迷宫范围内
    bool isValid(int row, int col) const;

    //打破两个相邻格子之间的墙
    void breakWalls(Cell &cell1, Cell &cell2);
    void breakWalls(int r1, int c1, int r2, int c2); // 重载版本，根据坐标

    //重置所有格子的访问状态(可能在解决算法前需要)
    void resetVisitedFlags();
    void resetPathFlags();

    //两个格子是否相邻
    bool areCellsAdjacent(const Cell &cell1, const Cell &cell2);
    bool areCellsAdjacent(int r1, int c1, int r2, int c2);

private:
    int mazeWidth;
    int mazeHeight;
    std::vector<std::vector<Cell>> grid; // 二维 vector 存储 Cell 对象
    
    // 私有的生成或解决算法的辅助函数
    void carvePassagesFrom(Cell &start_cell, std::mt19937 &g);

};

#endif //MAZE_H