//
// Created by henry on 3/28/23.
//

#ifndef JAMEGAM_TITLE_H
#define JAMEGAM_TITLE_H

#include <hagame/core/scene.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/text.h>
#include <hagame/graphics/resolution.h>

#include "runtime.hpp"
#include "../assets.hpp"

using namespace hg::graphics;

class Title : public hg::Scene {
public:

    Window* window;

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    OrthographicCamera m_uiCamera;
    Resolution m_resolution = HD;
    Text m_text;

};

void Title::onInit() {
    m_uiCamera.size = m_resolution;
}

void Title::onUpdate(double dt) {

    if (window->input.keyboardMouse.keyboard.enterPressed) {
        game()->scenes()->get<Runtime>("runtime")->restartGame();
        game()->scenes()->activate("runtime");
    }

    auto fontLarge = FONTS.get("default_large").get();
    auto font = FONTS.get("default").get();
    auto shader = SHADERS.get("text").get();
    shader->use();

    shader->setVec4("textColor", Color::red());
    shader->setMat4("view", m_uiCamera.view());
    shader->setMat4("projection", m_uiCamera.projection());
    shader->setMat4("model", hg::Mat4::Identity());

    m_text.draw(fontLarge, "The Monsters Hand", hg::Vec3(100, 250, 0), TextHAlignment::Left);

    m_text.draw(
            font,
            "Pesky Adventurers keep trying to take your loot. Reach the exit of your dungeon to stop the source.",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 200, 0),
            TextHAlignment::Left
            );

    m_text.draw(
            font,
            "Use your mouse to move around. Click on cards to activate them. Enemies can be attacked by clicking on them.",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 150, 0),
            TextHAlignment::Left
    );

    m_text.draw(
            font,
            "Press enter to begin...",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 100, 0),
            TextHAlignment::Left
    );
}

#endif //JAMEGAM_TITLE_H
