#ifndef SNEK_RENDER_H
#define SNEK_RENDER_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

namespace snek
{

class Render
{

public:
    void setup();
    void loop();

private:
    void renderBackground();
    void renderSnake();
    void renderFood();
    void renderFoodEaten();
    void renderGameOver();
    void gameOverCallback();

    void handleMessage();
    void handleRenderProcess();

private:
    LGFX lcd_{240, 240};
    LGFX_Sprite screen_{&lcd_};

    uint32_t ticks_{0};
    uint32_t food_ticks_{0};
    uint32_t explore_ticks_{0};
};

} // namespace snek

#endif