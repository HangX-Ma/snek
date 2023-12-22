#include "lgfx/v1/platforms/sdl/Panel_sdl.hpp"
#include "snek/render.h"

#if defined(SDL_h_)

static snek::Render render;

// clang-format off
__attribute__((weak))
int userFunc(bool *running)
{
    render.setup();
    do {
        render.loop();
    }
    while (*running);
    return 0;
}

int main(int /*unused*/, char ** /*unused*/) { return lgfx::Panel_sdl::main(userFunc); }
// clang-format on

#endif