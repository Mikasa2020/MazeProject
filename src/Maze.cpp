//Maze.cpp
#include "Maze.h"
#include <iostream>
#include <vector>
#include <cmath> // 为了取绝对值 std::abs
//不了解的库
#include <stdexcept> // 用于抛出异常，如 getCell 越界

#include<random> //用于随机数
#include<algorithm> //用于 std::shuffle

Maze::Maze(int width, int height) : mazeWidth(width), mazeHeight(height){
    if(width <= 0 || height <= 0){
        //抛出异常或设置一个最小尺寸
        throw std::invalid_argument("Maze dimensions must be positive.");
    }
     
    //二维数组 vector 储存 Cell
    grid.reserve(mazeHeight); // 为行预分配空间
    for(int r = 0; r < mazeHeight; r++){
        std::vector<Cell> currentRow;
        currentRow.reserve(mazeWidth); // 为当前行的列预分配空间
        for(int c = 0; c < mazeWidth; c++){
            currentRow.emplace_back(r, c);
        }
        grid.push_back(std::move(currentRow)); // 将构建好的一行移入 grid
    }
}

int Maze::getWidth() const{return mazeWidth;}
int Maze::getHeight() const{return mazeHeight;}

bool Maze::isValid(int row, int col) const{
    return row >= 0 && row < mazeHeight && col >= 0 && col < mazeWidth;
}

//获取坐标
const Cell &Maze::getCell(int row, int col)const {
    if (!isValid(row, col)) {
        throw std::out_of_range("Cell coordinates out of bounds.");
    }
    return grid[row][col];
}
Cell &Maze::getCell(int row, int col){
    if(!isValid(row, col)){
        throw std::out_of_range("Cell coordnates out of bounds");
    }
    return grid[row][col];
}

//实现方法
void Maze::display() const{
    // ...
    std::cout << "'display' is coding..." << std::endl;
}

void Maze::breakWalls(Cell &cell1, Cell &cell2){
    // 获取坐标
    // (r1, c1)为cell1坐标； (r2,c2)为cell2坐标
    int r1 = cell1.getRow();
    int c1 = cell1.getCol();
    int r2 = cell2.getRow();
    int c2 = cell2.getCol();

    // 检测 cell1 和 cell2 是否相邻
    if(!areCellsAdjacent(r1, c1, r2, c2)){
        std::cerr << "error:Cells are not adjacent." << std::endl;
        return;
    }

    // 检测 cell1 和 cell2 怎么相邻(上、下、左、右)
    //cell1 在 cell2 上方
    if(r1 < r2 && c1 == c2){
        cell1.removeWall(Direction::SOUTH);
        cell2.removeWall(Direction::NORTH);
    }
    //cell1 在 cell2 下方
    else if(r1 > r2 && c1 == c2){
        cell1.removeWall(Direction::NORTH);
        cell2.removeWall(Direction::SOUTH);
    }
    //cell1 在 cell2 左方
    else if(r1 == r2 && c1 < c2){
        cell1.removeWall(Direction::EAST);
        cell2.removeWall(Direction::WEST);
    }
    //cell1 在 cell2 右方
    else if(r1 == r2 && c1 > c2){
        cell1.removeWall(Direction::WEST);
        cell2.removeWall(Direction::EAST);
    }
    return;
}

void Maze::breakWalls(int r1, int c1, int r2, int c2){
    Cell &cell1 = grid[r1][c1];
    Cell &cell2 = grid[r2][c2];

     // 检测 cell1 和 cell2 是否相邻
    if(!areCellsAdjacent(r1, c1, r2, c2)){
        std::cerr << "error:Cells are not adjacent." << std::endl;
        return;
    }

    // 检测 cell1 和 cell2 怎么相邻(上、下、左、右)
    //cell1 在 cell2 上方
    if(r1 < r2 && c1 == c2){
        cell1.removeWall(Direction::SOUTH);
        cell2.removeWall(Direction::NORTH);
    }
    //cell1 在 cell2 下方
    else if(r1 > r2 && c1 == c2){
        cell1.removeWall(Direction::NORTH);
        cell2.removeWall(Direction::SOUTH);
    }
    //cell1 在 cell2 左方
    else if(r1 == r2 && c1 < c2){
        cell1.removeWall(Direction::EAST);
        cell2.removeWall(Direction::WEST);
    }
    //cell1 在 cell2 右方
    else if(r1 == r2 && c1 > c2){
        cell1.removeWall(Direction::WEST);
        cell2.removeWall(Direction::EAST);
    }
}

void Maze::resetVisitedFlags(){
    // for(int r = 0; r < mazeHeight; r++){
    //     for(int c = 0; c < mazeWidth; c++){
    //         grid[r][c].setVisited(false);
    //     }
    // }

    //更好的方法
    for(std::vector<Cell> &row_vec : grid){
        for(Cell &cell_element : row_vec){
            cell_element.setVisited(false);
        }
    }
}

void Maze::resetPathFlags(){
    for(std::vector<Cell> &row_vec : grid){
        for(Cell &cell_element : row_vec){
            cell_element.markAsPath(false);
        }
    }
}

bool Maze::areCellsAdjacent(const Cell &cell1, const Cell &cell2){
    //获取坐标
    int r1 = cell1.getRow();
    int c1 = cell1.getCol();
    int r2 = cell2.getRow();
    int c2 = cell2.getCol();

    int row_diff = std::abs(r1 - r2);
    int col_diff = std::abs(c1 - c2);

    return (row_diff + col_diff) == 1;
}
bool Maze::areCellsAdjacent(int r1, int c1, int r2, int c2){
    int row_diff = std::abs(r1 - r2);
    int col_diff = std::abs(c1 - c2);

    return (row_diff + col_diff) == 1;
}

//生成迷宫的算法
void Maze::generate(){
    //DFS(Deep First Search)
    //初始化Maze
    resetPathFlags();
    resetVisitedFlags();

    //随机数生成
    std::random_device rd;
    std::mt19937 g(rd());

    //确定一个起点 grid[start_r][start_c]
    // int start_r = (g() % mazeHeight);
    // int start_c = (g() % mazeWidth);
    //更标准的
    std::uniform_int_distribution<> distrib_row(0, mazeHeight - 1);
    std::uniform_int_distribution<> distrib_col(0, mazeWidth - 1);
    int start_r = distrib_row(g);
    int start_c = distrib_col(g);

    Cell &start_cell = getCell(start_r, start_c);

    //调用雕刻函数
    carvePassagesFrom(start_cell, g);
}

//私有辅助函数
void Maze::carvePassagesFrom(Cell &current_cell, std::mt19937 &g){
    // 1. 将当前单元格标记访问
    current_cell.setVisited(true);

    // 2. 定义四个方向变化量
    //定义坐标变化量
    int dr[] = {-1, 1, 0 ,0};
    int dc[] = {0, 0, 1, -1};
    Direction Direction_array[] = {Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST};

    //创建一个包含方向索引的向量，用于随机打乱
    std::vector<int> dir_indices = {0, 1, 2, 3};
    std::shuffle(dir_indices.begin(), dir_indices.end(), g);

    // 3. 遍历
    for(int i = 0; i < 4; i++){
        //随机获取一个索引
        int random_dir_idx = dir_indices[i];
        //计算坐标
        int next_r = current_cell.row + dr[random_dir_idx];
        int next_c = current_cell.col + dc[random_dir_idx];
        //检测是否合法
        if(!isValid(next_r, next_c)){
            //std::cout << "DEBUG:Coordinate is not Vaild" << std::endl;
            continue;
        }
        Cell &next_cell = getCell(next_r, next_c);
        //检测是否被访问
        if(next_cell.isVisited()){
            //DEBUG
            continue;
        }
        //打破墙
        breakWalls(current_cell, next_cell);

        //递归
        carvePassagesFrom(next_cell, g);
    }
}