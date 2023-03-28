//
// Created by henry on 3/27/23.
//

#ifndef JAMEGAM_RENDERER_H
#define JAMEGAM_RENDERER_H

#include <hagame/graphics/renderPipeline.h>
#include "assets.hpp"
#include "components/actor.h"

using namespace hg;
using namespace hg::graphics;

class Renderer {
public:

    Renderer(Scene* scene):
        m_scene(scene)
    {
        m_uiCamera.size = m_resolution;
        m_camera.setResolution(m_resolution);
    }

    void render(double dt);

    CameraController* camera() { return &m_camera; }
    OrthographicCamera* uiCamera() { return &m_uiCamera; }

private:

    Resolution m_resolution = HD;

    CameraController m_camera;
    OrthographicCamera m_uiCamera;

    Text m_text;

    Scene* m_scene;

    void renderSprites();
    void renderFocus();
    void renderText(double dt);

};

void Renderer::render(double dt) {
    renderSprites();
    renderFocus();
    renderText(dt);
}

void Renderer::renderSprites() {
    auto shader = SHADERS.get("sprite");
    shader->use();
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    m_scene->entities.forEach<Sprite>([&](auto sprite, auto entity) {
        sprite->mesh()->update(&sprite->quad);
        shader->setMat4("model", entity->transform.getModel());
        auto texture = TEXTURES.get(sprite->texture);
        texture->bind();
        glActiveTexture(GL_TEXTURE0);
        sprite->mesh()->render();
    });
}

void Renderer::renderText(double dt) {
    auto font = FONTS.get("default");
    auto shader = SHADERS.get("text");

    shader->use();

    /*
    shader->setVec4("textColor", Color::red());
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    m_scene->entities.forEach<Actor>([&](auto actor, auto entity) {
        shader->setVec4("color", Color::red());
        shader->setMat4("model", entity->transform.getModel());
        m_text->draw(font, actor->name + " (" + std::to_string((int) actor->health) + ")", hg::Vec3(-TILE_SIZE[0] * 0.5, TILE_SIZE[1] * 0.5, 0));
    });
     */
}

void Renderer::renderFocus() {
    auto shader = SHADERS.get("focus");
    shader->use();
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    shader->setMat4("model", hg::Mat4::Identity());
    shader->setVec4("color", Color::red());
    // m_pathFindingLineMesh->render();
}

#endif //JAMEGAM_RENDERER_H
