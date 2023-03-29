//
// Created by henry on 3/28/23.
//

#ifndef JAMEGAM_WIN_H
#define JAMEGAM_WIN_H

#include <hagame/core/scene.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/text.h>
#include <hagame/graphics/resolution.h>

#include "../assets.hpp"
#include "runtime.hpp"

using namespace hg::graphics;

class Win : public hg::Scene {
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

void Win::onInit() {
    m_uiCamera.size = m_resolution;
}

void Win::onUpdate(double dt) {

    if (window->input.keyboardMouse.keyboard.escPressed) {
        game()->scenes()->activate("title");
    }

    auto fontLarge = FONTS.get("default_large").get();
    auto font = FONTS.get("default").get();
    auto shader = SHADERS.get("text").get();
    shader->use();

    shader->setVec4("textColor", Color::red());
    shader->setMat4("view", m_uiCamera.view());
    shader->setMat4("projection", m_uiCamera.projection());
    shader->setMat4("model", hg::Mat4::Identity());

    m_text.draw(fontLarge, "You Won!", hg::Vec3(100, 250, 0), TextHAlignment::Left);

    m_text.draw(
            font,
            "You managed to defeat the Angels sending in adventurers!",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 200, 0),
            TextHAlignment::Left
    );

    m_text.draw(
            font,
            "Thank you for playing The Monster's Hand. Check out more of my work at https://henrywalters.dev",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 150, 0),
            TextHAlignment::Left
    );

    m_text.draw(
            font,
            "Press escape to exit to title screen...",
            hg::Vec3(500, 200, 0),
            hg::Vec3(100, 100, 0),
            TextHAlignment::Left
    );
}

#endif //JAMEGAM_TITLE_H
