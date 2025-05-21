//Cell.cpp
#include "Cell.h"

Cell::Cell() : visited(false), isPath(false){
    // 默认所有墙都存在
    for(int i = 0; i < 4; i++){
        walls[i] = true;
    }
}

