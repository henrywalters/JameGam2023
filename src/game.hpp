//
// Created by henry on 3/24/23.
//

#ifndef JAMEGAM_GAME_HPP
#define JAMEGAM_GAME_HPP

#include <hagame/core/game.h>
#include <hagame/graphics/windows.h>

#include "assets.hpp"

#include "scenes/runtime.hpp"

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

    loadFont("default", "fonts/8bit.ttf");

    loadShader("sprite", "shaders/sprite.vert", "shaders/sprite.frag");
    loadShader("focus", "shaders/focus.vert", "shaders/focus.frag");

    loadTexture("balrug", "textures/actors/balrug.png");
    loadTexture("elf", "textures/actors/elf.png");
    loadTexture("dwarf", "textures/actors/dwarf.png");
    loadTexture("floor", "textures/tiles/cobble_blood1.png");
    loadTexture("wall", "textures/tiles/wall_vines4.png");
    loadTexture("focus", "textures/misc/focus.png");

    auto runtime = static_cast<Runtime*>(scenes()->add<Runtime>("runtime"));
    runtime->window = m_window;

    scenes()->activate("runtime");
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
