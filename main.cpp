//main.cpp
#include "Maze.h"
#include "Cell.h"
#include <iostream>
#include <stdexcept>

int main(){
    int width, height;
    std::cout << "Enter maze width: ";
    std::cin >> width;
    std::cout << "Enter maze height: ";
    std::cin >> height;

     if (std::cin.fail() || width <= 0 || height <= 0) {
        std::cerr << "Invalid input. Width and height must be positive integers." << std::endl;
        return 1;
    }

    try{
        Maze myMaze(width, height); // 创建对象

        std::cout << "\nGenerating maze ..." << std::endl;
        myMaze.generate();

        std::cout << "\nGenerating Maze:" << std::endl;
        myMaze.display();

    } catch(const std::exception &e){ //e.what() 会返回你在 throw 时提供的描述字符串
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
}