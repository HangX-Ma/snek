#ifndef SNEK_STATE_H
#define SNEK_STATE_H

#include "snek/action.h"
#include "snek/config.h"
#include "snek/random.h"
#include <algorithm>
#include <cstdint>
#include <queue>
#include <vector>

namespace snek
{

struct Snake
{
    bool alive{true};
    std::vector<Point> body;
    Point residual_tail;

    bool isBody(Point p)
    {
        return std::any_of(body.begin(), body.end(),
                           [=](const auto &body_segment)
                           {
                               return body_segment == p;
                           });
    }
};

struct Food
{
    Point fpos;
    Point epos;

    int getFoodSize() const { return food_size_; }
    void setFoodSize(int size) { food_size_ = size; }

    int getExplodeSize() const { return explode_size_; }
    void setExplodeSize(int size) { explode_size_ = size; }

private:
    int food_size_{0};
    int explode_size_{SNAKE_BODY_SIZE * 2};
};

enum class Message
{
    FOOD_EATEN,
};

// singleton GameState
class GameState
{

public:
    GameState(const GameState &) = delete;
    GameState &operator=(const GameState &) = delete;
    GameState(GameState &&) = delete;
    GameState &operator=(GameState &&) = delete;

    static GameState &getInstance()
    {
        static GameState singleton;
        return singleton;
    }

    void setGameBoardSize(uint32_t width, uint32_t height)
    {
        board_width_ = width;
        board_height_ = height;
    }

    void resetGame();

    void sendMessage(Message msg) { msg_box_.push(msg); };

    const Snake &getSnake() const { return snake_; }
    Food &getFood() { return food_; }
    Action &getAction() { return action_; }
    std::queue<Message> &getMessageBox() { return msg_box_; }
    uint32_t getScore() const { return score_; }

    void moveSnake();
    void checkSelfBite();
    void checkFoodEating();

private:
    bool checkPointLegality(const Point &p);
    bool checkXYLegality(int val);
    void confirmPointBound(Point &p);

    void growSnake();
    void updateFood(bool set_explore = false);

    GameState() = default;
    ~GameState() = default;

    Random random_;
    Snake snake_;
    Action action_;
    Food food_;

    std::queue<Message> msg_box_;

    uint32_t score_;
    uint32_t board_width_;
    uint32_t board_height_;
};

} // namespace snek

#endif // SNEK_STATE_H