//DFSMazeGenerator.cpp
#include "DFSMazeGenerator.h"
#include "Maze.h"
#include <algorithm>
#include <iostream>

DFSMazeGenerator::DFSMazeGenerator() : generation_completed(true){
    //构造 默认完成
}

void DFSMazeGenerator::start(Maze& maze, int start_r, int start_c, std::mt19937& g){
    std::cout << "Starting DFS Maze Generation..." << std::endl;

    //初始化迷宫(访问、路径)
    maze.resetPathFlags();
    maze.resetVisitedFlags();

    //清空栈
    dfs_stack.clear();
    //检测起点是否合法
    //最主要的终止条件
    if (!maze.isValid(start_r, start_c)) {
        std::cerr << "DFS Start: Invalid start coordinates (" << start_r << "," << start_c << "). Defaulting to (0,0)." << std::endl;
        start_r = 0;
        start_c = 0;
    }

    Cell& start_cell = maze.getCell(start_r, start_c);
    start_cell.setVisited(true);
    dfs_stack.push_back(&start_cell); //初始格子入栈

    generation_completed = false;
}

bool DFSMazeGenerator::step(Maze& maze, std::mt19937& g){
    if(dfs_stack.empty()){
        if(!generation_completed){ // 第一次完成时打印
            std::cout << "DFS Maze Generation Complete!" << std::endl;
        }
        generation_completed = true;
        return false;
    }

    while(!dfs_stack.empty()){

        Cell* current_cell_ptr = dfs_stack.back(); // 获取栈顶格子(当前)，不弹出
        Cell& current_cell = *current_cell_ptr;

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, 1, -1};

        std::vector<int> dir_indices = {0,1,2,3};
        std::shuffle(dir_indices.begin(), dir_indices.end(), g);

        for(int i = 0; i < 4; i++){
            int random_dir_idx = dir_indices[i];

            int next_r = current_cell.row + dr[random_dir_idx];
            int next_c = current_cell.col + dc[random_dir_idx];

            //检测是否在坐标轴内
            if(maze.isValid(next_r, next_c)){
                Cell& next_cell = maze.getCell(next_r, next_c);
                //检测是否被访问
                if(!next_cell.isVisited()){
                    //找到未访问的Cell
                    //破墙
                    maze.breakWalls(current_cell, next_cell);

                    next_cell.setVisited(true);//领居以访问
                    dfs_stack.push_back(&next_cell);//入栈
                    return true;
                }
            }
        }
        //所有邻居都被访问过
        dfs_stack.pop_back(); // 回溯
    }

    //执行最后一步回溯后，栈刚好空了
    //杜绝 回溯导致完成
    if (!generation_completed) {
        std::cout << "DFS Maze Generation Complete (stack empty after backtrack)!" << std::endl;
    }
    generation_completed = true;
    return false;
    

}

bool DFSMazeGenerator::isComplete() const{
    return generation_completed;
}