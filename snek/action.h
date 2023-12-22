#ifndef SNEK_ACTION_H
#define SNEK_ACTION_H

#include "lgfx/v1/platforms/sdl/common.hpp"

// clang-format off
#define BTN_LEFT   39
#define BTN_DOWN   38
#define BTN_RIGHT  37
#define BTN_UP     36

// clang-format on
namespace snek
{
struct Point
{
    int x;
    int y;

    bool operator!=(const Point &rhs) const { return (this->x != rhs.x) || (this->y != rhs.y); }
    bool operator==(const Point &rhs) const { return (this->x == rhs.x) && (this->y == rhs.y); }
    Point operator+(Point rhs) { return {this->x + rhs.x, this->y + rhs.y}; }
    Point operator-(Point rhs) { return {this->x - rhs.x, this->y - rhs.y}; }
};
using DirPoint = struct Point;

enum class Direction
{
    MOVE_LEFT,
    MOVE_DOWN,
    MOVE_RIGHT,
    MOVE_UP,
    MAX_DIRECTION_NUM,
};

static DirPoint direction_map[4] = {
    {-1,  0}, // LEFT
    { 0,  1}, // DOWN
    { 1,  0}, // RIGHT
    { 0, -1}, // UP
};

template <class type>
constexpr std::common_type_t<int, std::underlying_type_t<type>> toInteger(type val)
{
    return static_cast<std::common_type_t<int, std::underlying_type_t<type>>>(val);
}

class Action
{

public:
    void reset()
    {
        moving_dir_ = Direction::MOVE_RIGHT;
        input_dir_ = Direction::MOVE_RIGHT;
    }
    void update(bool change_dir = false);
    Direction getMovingDirection() { return moving_dir_; }

private:
    void scanKeyBoard();

private:
    Direction illegal_map_[toInteger(Direction::MAX_DIRECTION_NUM)]{
        Direction::MOVE_RIGHT,
        Direction::MOVE_UP,
        Direction::MOVE_LEFT,
        Direction::MOVE_DOWN,
    };
    Direction moving_dir_{Direction::MOVE_RIGHT};
    Direction input_dir_{Direction::MOVE_RIGHT};
};

} // namespace snek

#endif