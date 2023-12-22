#include "snek/action.h"
#include "spdlog/spdlog.h"

namespace snek
{

void Action::scanKeyBoard()
{
    if (!lgfx::gpio_in(BTN_LEFT)) {
        input_dir_ = Direction::MOVE_LEFT;
    }
    else if (!lgfx::gpio_in(BTN_DOWN)) {
        input_dir_ = Direction::MOVE_DOWN;
    }
    else if (!lgfx::gpio_in(BTN_RIGHT)) {
        input_dir_ = Direction::MOVE_RIGHT;
    }
    else if (!lgfx::gpio_in(BTN_UP)) {
        input_dir_ = Direction::MOVE_UP;
    }
}

void Action::update(bool change_dir)
{
    scanKeyBoard();
    if (illegal_map_[toInteger(input_dir_)] == moving_dir_) {
        return;
    }

    if (change_dir) {
        spdlog::debug("Set moving direction to {}", toInteger(input_dir_));
        moving_dir_ = input_dir_;
    }
}

} // namespace snek