// IMazeGenerator.h
#ifndef IMAZEGENERATOR_H
#define IMAZEGENERATOR_H

#include <random>

class Maze;

class IMazeGenerator{
public:
    virtual ~IMazeGenerator() = default;

    // 初始化或重置生成算法的状态，并准备在给定的 Maze 对象上开始生成
    virtual void start(Maze& maze,int start_r, int start_c, std::mt19937& g) = 0;

    //执行生成算法的一小步
    //如果生成仍在进行，返回 true; 已完成，返回 false;
    virtual bool step(Maze& maze, std::mt19937& g) = 0;

    //check
    virtual bool isComplete() const = 0;

};
#endif