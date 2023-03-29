//
// Created by henry on 3/24/23.
//

#ifndef JAMEGAM_GAME_HPP
#define JAMEGAM_GAME_HPP

#include <hagame/core/game.h>
#include <hagame/graphics/windows.h>

#include "assets.hpp"

#include "scenes/runtime.hpp"
#include "scenes/title.h"
#include "scenes/death.h"
#include "scenes/win.h"

using namespace hg::graphics;

class Game : public hg::Game {
public:

    Game():
        hg::Game("The Monster's Hand")
    {}

protected:

    void onInit() override;
    void onBeforeUpdate() override;
    void onUpdate(double dt) override;
    void onAfterUpdate() override;
private:

    Window* m_window;

};

void Game::onInit() {
    m_window = Windows::Create("The Monster's Hand", HD);

    Windows::Events.subscribe(WindowEvents::Close, [&](Window* window) {
        running(false);
    });

    Windows::Events.subscribe(WindowEvents::Resize, [&](Window* window) {

    });

    loadFont("default", "fonts/8bit.ttf")->fontSize(14);
    loadFont("default_large", "fonts/apple_kid.ttf")->fontSize(64);
    loadFont("card_font", "fonts/apple_kid.ttf");

    loadShader("sprite", "shaders/sprite.vert", "shaders/sprite.frag");
    loadShader("focus", "shaders/focus.vert", "shaders/focus.frag");
    loadShader("text", "shaders/text.vert", "shaders/text.frag");

    loadTexture("balrug", "textures/actors/balrug.png");
    loadTexture("elf", "textures/actors/elf.png");
    loadTexture("dwarf", "textures/actors/dwarf.png");
    loadTexture("angel", "textures/actors/angel.png");
    loadTexture("centaur", "textures/actors/centaur.png");
    loadTexture("floor", "textures/tiles/cobble_blood1.png");
    loadTexture("wall", "textures/tiles/wall_vines4.png");
    loadTexture("focus", "textures/misc/focus.png");
    loadTexture("water", "textures/tiles/dngn_shoals_deep_water1.png");
    loadTexture("stairs_up", "textures/misc/stone_stairs_up.png");
    loadTexture("stairs_down", "textures/misc/stone_stairs_down.png");

    loadTexture("card_template", "textures/misc/card_template.png");
    loadTexture("card_back", "textures/misc/card_back.png");
    loadTexture("card_cross", "textures/misc/card_cross.png");
    loadTexture("card_fire", "textures/misc/card_fire.png");
    loadTexture("card_health", "textures/misc/card_health.png");
    loadTexture("card_move", "textures/misc/card_move.png");
    loadTexture("card_shield", "textures/misc/card_shield.png");

    auto runtime = static_cast<Runtime*>(scenes()->add<Runtime>("runtime"));
    runtime->window = m_window;

    auto title = static_cast<Title*>(scenes()->add<Title>("title"));
    title->window = m_window;

    auto death = static_cast<Death*>(scenes()->add<Death>("death"));
    death->window = m_window;

    auto win = static_cast<Win*>(scenes()->add<Win>("win"));
    win->window = m_window;

    scenes()->activate("title");
}

void Game::onBeforeUpdate() {
    m_window->clear();
}

void Game::onUpdate(double dt) {

}

void Game::onAfterUpdate() {
    m_window->render();
}

#endif //JAMEGAM_GAME_HPP
