//DFSMazeGenerator.h
#ifndef DFSMAZEGENERATOR_H
#define DFSMAZEGENERATOR_H

#include "IMazeGenerator.h"
#include <vector>
#include "Cell.h"

class DFSMazeGenerator : public IMazeGenerator{
public:
    DFSMazeGenerator();

    void start(Maze& maze, int start_r, int start_c, std::mt19937& g) override;
    bool step(Maze& maze, std::mt19937& g) override;
    bool isComplete() const override;

private:
    std::vector<Cell*> dfs_stack; // 用于迭代DFS的栈，存储指向Cell对象的指针
    bool generation_completed;

};

#endif