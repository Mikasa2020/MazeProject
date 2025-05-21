//Cell.cpp
#include "Cell.h"

Cell::Cell(int r, int c) : row(r), col(c), visited(false), isPath(false){
    // 默认所有墙都存在
    for(int i = 0; i < 4; i++){
        walls[i] = true;
    }
}

void Cell::removeWall(Direction dir){
    if(dir != Direction::NONE){
        walls[static_cast<int>(dir)] = false;
    }
}

bool Cell::hasWall(Direction dir) const{
    if(dir != Direction::NONE){
        return walls[static_cast<int>(dir)];
    }
    return true; // 抛出异常，表示无效方向
}

void Cell::setVisited(bool v){
    visited = v;
}

bool Cell::isVisited() const{
    return visited;
}

void Cell::markAsPath(bool p){
    isPath = p;
}

bool Cell::isPathCell() const{
    return isPath;
}

// 一个简单的显示字符逻辑
char Cell::getDisplayCharacter() const{
    if(isPathCell()) return '*';
    return ' ';
}

int Cell::getRow() const{
    return row;
}

int Cell::getCol() const{
    return col;
}