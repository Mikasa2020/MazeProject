//Cell.h
#ifndef CELL_H
#define CELL_H

// 可以定义一个枚举来表示方向，方便墙壁操作
enum class Direction{NORTH, SOUTH, EAST, WEST, NONE};

class Cell{
public:
    //构造函数：初始化一个格子
    //默认情况下：一个格子四面都有墙，从未被访问过
    Cell();

    //墙壁状态 (为了简单，我们用一个布尔数组，索引对应方向)
    //索引：0:NORTH(上), 1:SOUTH(下), 2:EAST(右), 3:WEST(左)
    bool walls[4]; // true 表示墙存在 false 表示墙被打通

    //访问状态(用于解决和生成算法)
    bool visited;

    //是否被访问
    bool visited;

    //是否为路径
    bool isPath;

    //格子的坐标，如果格子本身需要知道自己的位置
    int row, col;

    // 公共接口
    void removeWall(Direction dir); // 移除指定方向的墙
    bool hasWall(Direction dir) const; // 检查指定方向是否有墙 // const 表示此方法不修改对象状态

    void setVisited(bool v);
    bool isVisited() const;

    void markAsPath(bool p);
    bool isPathCell() const;

    // 获取用于显示的字符
    char getDisplayCharacter() const;

private:
    //...
};

#endif //CELL_H