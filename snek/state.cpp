#include "snek/state.h"
#include "snek/config.h"
#include "spdlog/spdlog.h"

namespace snek
{

void GameState::resetGame()
{
    snake_.alive = true;
    snake_.body.clear();
    snake_.body.push_back(
        {(SNAKE_BODY_SIZE + SNAKE_BODY_SIZE / 2), (SNAKE_BODY_SIZE + SNAKE_BODY_SIZE / 2)});
    score_ = 0;
    action_.reset();
    updateFood();
}

void GameState::moveSnake()
{
    Point new_head = snake_.body.front();

    action_.update(checkPointLegality(new_head));

    DirPoint dir = direction_map[toInteger(action_.getMovingDirection())];
    // spdlog::info("original head: ({},{}), new head: ({},{})", new_head.x, new_head.y,
    //              new_head.x + dir.x, new_head.y + dir.y);
    new_head = new_head + dir;

    confirmPointBound(new_head);

    snake_.residual_tail = snake_.body.back();
    snake_.body.insert(snake_.body.begin(), new_head);
    snake_.body.pop_back();
}

void GameState::growSnake()
{
    for (int i = 0; i < SNAKE_BODY_SIZE; i += 1) {
        snake_.body.push_back(snake_.residual_tail);
    }
    msg_box_.push(Message::FOOD_EATEN);
}

void GameState::checkSelfBite()
{
    Point head = snake_.body.front();
    for (auto iter = snake_.body.begin() + 1; iter != snake_.body.end(); iter++) {
        if (*iter == head) {
            snake_.alive = false;
        }
    }
}

bool GameState::checkPointLegality(const Point &p)
{
    return checkXYLegality(p.x) && checkXYLegality(p.y);
}

bool GameState::checkXYLegality(int val)
{
    return (((val + SNAKE_BODY_SIZE / 2) % SNAKE_BODY_SIZE) == 0);
}

void GameState::confirmPointBound(Point &p)
{
    if (p.x < 0) {
        p.x = board_width_;
    }
    if (p.y < 0) {
        p.y = board_height_;
    }

    if (p.x > board_width_) {
        p.x = 0;
    }
    if (p.y > board_height_) {
        p.y = 0;
    }
}
void GameState::checkFoodEating()
{
    Point head = snake_.body.front();
    if (head == food_.fpos) {
        score_ += 1;
        growSnake();
        updateFood(true);
    }
}

void GameState::updateFood(bool set_explore)
{
    while (true) {
        food_.fpos.x
            = random_.next(0 + (SNAKE_BODY_SIZE / 2), board_width_ - (SNAKE_BODY_SIZE / 2));
        food_.fpos.y
            = random_.next(0 + (SNAKE_BODY_SIZE / 2), board_height_ - (SNAKE_BODY_SIZE / 2));
        if (!checkPointLegality(food_.fpos)) {
            continue;
        }
        if (snake_.isBody(food_.fpos)) {
            continue;
        }
        break;
    }
    food_.setFoodSize(0);

    if (set_explore) {
        food_.epos = snake_.body.front();
        food_.setExplodeSize(2);
    }
}

} // namespace snek