#include "snek/render.h"
#include "SDL_timer.h"
#include "snek/config.h"
#include "snek/state.h"
#include "snek/action.h"
#include <cassert>

namespace snek
{

Point snake_eye_map[4][2]{
    {{-1, -1}, {-1, 0}}, // LEFT
    { {-1, 0},  {0, 0}}, // DOWN
    { {0, -1},  {0, 0}}, // RIGHT
    {{-1, -1}, {0, -1}}, // UP
};

void Render::setup()
{
    lcd_.init();
    screen_.createSprite(lcd_.width(), lcd_.height());

    GameState::getInstance().setGameBoardSize(screen_.width(), screen_.height());
    GameState::getInstance().resetGame();
}

void Render::loop()
{
    if (ticks_ == 0) {
        ticks_ = SDL_GetTicks();
    }
    if (SDL_GetTicks() - ticks_ >= GAME_UPDATE_INTERVAL) {
        ticks_ = SDL_GetTicks();
        GameState::getInstance().moveSnake();
        GameState::getInstance().checkSelfBite();
        GameState::getInstance().checkFoodEating();

        // render
        screen_.clear();
        renderBackground();
        renderSnake();
        renderFood();
        handleMessage();
        handleRenderProcess();
        screen_.pushSprite(0, 0);
    }
    GameState::getInstance().getAction().update();
}

void Render::handleMessage()
{
    auto &msg_box = GameState::getInstance().getMessageBox();
    while (!msg_box.empty()) {
        switch (msg_box.front()) {
        case Message::FOOD_EATEN:
        {
            renderFoodEaten();
            break;
        }
        default:
            break;
        }
        msg_box.pop();
    }
    assert(msg_box.empty());
}

void Render::handleRenderProcess()
{
    // food render
    if (food_ticks_ == 0) {
        food_ticks_ = SDL_GetTicks();
    }
    if ((SDL_GetTicks() - food_ticks_) > 10) {
        food_ticks_ = SDL_GetTicks();
        int food_size = GameState::getInstance().getFood().getFoodSize();
        if (food_size < SNAKE_BODY_SIZE) {
            GameState::getInstance().getFood().setFoodSize(food_size + 1);
        }
    }

    // exploring render
    if (explore_ticks_ == 0) {
        explore_ticks_ = SDL_GetTicks();
    }
    if ((SDL_GetTicks() - explore_ticks_) > 15) {
        explore_ticks_ = SDL_GetTicks();

        int explode_size = GameState::getInstance().getFood().getExplodeSize();
        if (explode_size < (SNAKE_BODY_SIZE * 2)) {
            GameState::getInstance().getFood().setExplodeSize(explode_size + 1);
            GameState::getInstance().sendMessage(Message::FOOD_EATEN);
        }
    }

    // check Game time
    if (!GameState::getInstance().getSnake().alive) {
        renderGameOver();
    }
}

void Render::renderBackground()
{
    // render score
    char text_buf[10];
    screen_.setFont(&fonts::Font8x8C64);
    screen_.setTextDatum(top_center);
    screen_.setTextSize(screen_.width() / 20.0);
    snprintf(text_buf, sizeof(text_buf), "%d", GameState::getInstance().getScore());
    screen_.setTextColor(COLOR_BG_DIALOG_SHADOW, TFT_BLACK);
    screen_.drawCenterString(text_buf, screen_.width() / 2 - 4,
                             (screen_.height() / 2.0) - screen_.getTextSizeY() * 4);
    screen_.setTextColor(COLOR_BG_GRID, TFT_BLACK);
    screen_.drawCenterString(text_buf, screen_.width() / 2 - 8,
                             (screen_.height() / 2.0) - screen_.getTextSizeY() * 4);

    // draw grid
    for (int x = -(SNAKE_BODY_SIZE / 2) - 1; x < screen_.width(); x += SNAKE_BODY_SIZE) {
        for (int y = -(SNAKE_BODY_SIZE / 2); y < screen_.height(); y += SNAKE_BODY_SIZE) {
            screen_.drawPixel(x, y, COLOR_BG_GRID);
        }
    }
}

void Render::renderSnake()
{
    const auto &snake = GameState::getInstance().getSnake();
    for (const auto body_segment : snake.body) {
        screen_.fillRoundRect(body_segment.x - (SNAKE_BODY_SIZE / 2) + 2,
                              body_segment.y - (SNAKE_BODY_SIZE / 2) + 2, SNAKE_BODY_SIZE,
                              SNAKE_BODY_SIZE, 1, COLOR_SNAKE_SHADOW);
    }
    for (const auto body_segment : snake.body) {
        screen_.fillRoundRect(body_segment.x - (SNAKE_BODY_SIZE / 2),
                              body_segment.y - (SNAKE_BODY_SIZE / 2), SNAKE_BODY_SIZE,
                              SNAKE_BODY_SIZE, 1, COLOR_SNAKE);
    }

    auto head = snake.body.front();
    auto [e1, e2]
        = snake_eye_map[toInteger(GameState::getInstance().getAction().getMovingDirection())];
    screen_.fillRoundRect(head.x + e1.x * (SNAKE_BODY_SIZE / 2),
                          head.y + e1.y * (SNAKE_BODY_SIZE / 2), SNAKE_BODY_SIZE / 2,
                          SNAKE_BODY_SIZE / 2, 2, COLOR_SNAKE_HEAD);
    screen_.fillRoundRect(head.x + e2.x * (SNAKE_BODY_SIZE / 2),
                          head.y + e2.y * (SNAKE_BODY_SIZE / 2), SNAKE_BODY_SIZE / 2,
                          SNAKE_BODY_SIZE / 2, 2, COLOR_SNAKE_HEAD);
}

void Render::renderFood()
{
    const auto &food = GameState::getInstance().getFood();
    int food_size = food.getFoodSize();
    screen_.fillRoundRect(food.fpos.x - (food_size / 2) + 2, food.fpos.y - (food_size / 2) + 2,
                          food_size, food_size, 1, COLOR_FOOD_SHADOW);
    screen_.fillRoundRect(food.fpos.x - (food_size / 2), food.fpos.y - (food_size / 2), food_size,
                          food_size, 1, COLOR_FOOD);
}

void Render::renderFoodEaten()
{
    /* Render octagon explode */
    int ball_size = SNAKE_BODY_SIZE / 2;
    int explode_size = GameState::getInstance().getFood().getExplodeSize();
    Point c = GameState::getInstance().getFood().epos;

    screen_.fillRoundRect(2 + c.x + explode_size, 2 + c.y, ball_size, ball_size, 1,
                          COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x - explode_size, 2 + c.y, ball_size, ball_size, 1,
                          COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x, 2 + c.y + explode_size, ball_size, ball_size, 1,
                          COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x, 2 + c.y - explode_size, ball_size, ball_size, 1,
                          COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x + (explode_size * 7 / 10), 2 + c.y - (explode_size * 7 / 10),
                          ball_size, ball_size, 1, COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x + (explode_size * 7 / 10), 2 + c.y + (explode_size * 7 / 10),
                          ball_size, ball_size, 1, COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x - (explode_size * 7 / 10), 2 + c.y + (explode_size * 7 / 10),
                          ball_size, ball_size, 1, COLOR_EXPLODE_SHADOW);
    screen_.fillRoundRect(2 + c.x - (explode_size * 7 / 10), 2 + c.y - (explode_size * 7 / 10),
                          ball_size, ball_size, 1, COLOR_EXPLODE_SHADOW);

    screen_.fillRoundRect(c.x + explode_size, c.y, ball_size, ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x - explode_size, c.y, ball_size, ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x, c.y + explode_size, ball_size, ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x, c.y - explode_size, ball_size, ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x + (explode_size * 7 / 10), c.y - (explode_size * 7 / 10), ball_size,
                          ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x + (explode_size * 7 / 10), c.y + (explode_size * 7 / 10), ball_size,
                          ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x - (explode_size * 7 / 10), c.y + (explode_size * 7 / 10), ball_size,
                          ball_size, 1, COLOR_EXPLODE);
    screen_.fillRoundRect(c.x - (explode_size * 7 / 10), c.y - (explode_size * 7 / 10), ball_size,
                          ball_size, 1, COLOR_EXPLODE);
}

void Render::renderGameOver()
{
    /* Render dialog window */
    for (int w = 0; w < ((screen_.width() * 5) / 6); w++) {
        screen_.fillRoundRect((screen_.width() / 12 + 3) + ((screen_.width() * 5) / 12) - (w / 2),
                              screen_.height() / 10 + 3, w, (screen_.height() * 7) / 10, 8,
                              COLOR_BG_DIALOG_SHADOW);
        screen_.fillRoundRect((screen_.width() / 12) + ((screen_.width() * 5) / 12) - (w / 2),
                              screen_.height() / 10, w, (screen_.height() * 7) / 10, 8,
                              COLOR_BG_DIALOG);
        SDL_Delay(2);
        screen_.pushSprite(0, 0);
    }
    SDL_Delay(50);

    char text_buff[10];
    int32_t font_height = 0;
    screen_.setFont(&fonts::Font8x8C64);
    screen_.setTextDatum(top_center);
    screen_.setTextColor(TFT_WHITE, COLOR_BG_GRID);

    screen_.setTextSize(screen_.width() / 128.0);
    snprintf(text_buff, sizeof(text_buff), "GAME OVER");
    screen_.drawCenterString(text_buff, screen_.width() / 2 - 3, (screen_.height() / 2));

    screen_.setTextSize(screen_.width() / 42.0);
    font_height = screen_.fontHeight();
    snprintf(text_buff, sizeof(text_buff), "%d", GameState::getInstance().getScore());
    for (float s = 1; s < (screen_.width() / 42.0); s += 0.1) {
        screen_.setTextSize(s);
        screen_.drawCenterString(text_buff, screen_.width() / 2 - 3,
                                 (screen_.height() / 2) - (font_height / 2 * 3));
        SDL_Delay(2);
        screen_.pushSprite(0, 0);
    }
    screen_.pushSprite(0, 0);

    gameOverCallback();

    /* Close dialog window */
    for (int w = ((screen_.width() * 5) / 6); w > 0; w -= 1) {
        screen_.clear();
        renderBackground();
        screen_.fillRoundRect((screen_.width() / 12 + 3) + ((screen_.width() * 5) / 12) - (w / 2),
                              screen_.height() / 10 + 3, w, (screen_.height() * 7) / 10, 8,
                              COLOR_BG_DIALOG_SHADOW);
        screen_.fillRoundRect((screen_.width() / 12) + ((screen_.width() * 5) / 12) - (w / 2),
                              screen_.height() / 10, w, (screen_.height() * 7) / 10, 8,
                              COLOR_BG_DIALOG);
        SDL_Delay(2);
        screen_.pushSprite(0, 0);
    }

    GameState::getInstance().resetGame();
}

void Render::gameOverCallback()
{
    /* Call return to reset game */
    int press_count = 0;
    while (true) {
        if (!lgfx::gpio_in(BTN_RIGHT)) {
            press_count += 1;
            if (press_count > 30) {
                return;
            }
        }
        else {
            press_count = 0;
        }
        SDL_Delay(5);
    }
}

} // namespace snek
