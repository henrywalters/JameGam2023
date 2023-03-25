#include <hagame/core/hg.h>
#include "src/game.hpp"

Game game;

void emscripten_tick() {
    game.tick();
}

int main() {
#ifdef __EMSCRIPTEN__
    game.initialize();
    emscripten_set_main_loop(emscripten_tick, 0, false);
    // clickVenture.destroy();
#else
    hg::HG::Run(&game);
#endif
    return 0;
}
