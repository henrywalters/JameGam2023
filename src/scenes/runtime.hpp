#ifndef JAMEGAM_RUNTIME_HPP
#define JAMEGAM_RUNTIME_HPP

#include <hagame/core/scene.h>
#include <hagame/graphics/text.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/components/sprite.h>
#include <hagame/graphics/primitives/quad.h>

#include "../assets.hpp"
#include "../cameraController.h"
#include "../map.h"
#include "../utils/pathfinding.h"

#include "../components/actor.h"
#include "../components/obstacle.h"
#include "../components/meleeEnemy.h"

using namespace hg::graphics;

enum class FocusState {
    Obstacle,
    Enemy,
    Item,
    Accessible,
    Inaccessible,
};

const int FOCUS_ALPHA = 100;

const std::vector<Color> FocusColors = {
        Color(255, 255, 0, FOCUS_ALPHA),
        Color(255, 0, 0, FOCUS_ALPHA),
        Color(0, 255, 0, FOCUS_ALPHA),
        Color(0, 0, 255, FOCUS_ALPHA),
        Color(255, 255, 255, FOCUS_ALPHA),
};

class Runtime : public hg::Scene {
public:

    Runtime():
        m_actors(this),
        m_obstacles(this),
        m_props(this)
    {}

    Window* window;

protected:

    void onInit() override;
    void onActivate() override;
    void onDeactivate() override;
    void onUpdate(double dt) override;

private:

    Resolution m_resolution = HD;

    CameraController m_camera;
    OrthographicCamera m_uiCamera;

    Map<Actor> m_actors;
    Map<TileObject> m_props;
    Map<Obstacle> m_obstacles;

    std::unique_ptr<Text> m_text;

    hg::Entity* m_player;

    int m_turn = 1;

    FocusState m_focusState = FocusState::Accessible;
    hg::Entity* m_focus;

    void updateFocusState(hg::Vec2i mousePos);
    std::optional<std::vector<hg::Vec2i>> getPath(hg::Vec2i start, hg::Vec2i goal);

    void updateEnemies();
};

void Runtime::onInit() {

    m_uiCamera.size = m_resolution;
    m_camera.setResolution(m_resolution);

    Windows::Events.subscribe(WindowEvents::Resize, [&](auto window) {
    });

    m_text = std::make_unique<Text>();

    m_player = entities.add();

    Sprite* sprite;

    m_player = m_actors.add("balrug", hg::Vec2i(7, 2));
    m_player->getComponent<Actor>()->movement = 2;

    auto dwarf = m_actors.add("dwarf", hg::Vec2i(7, 7));
    dwarf->addComponent<MeleeEnemy>()->target = m_player;

    for (int i = 0; i <= 10; i++) {
        m_obstacles.add("wall", hg::Vec2i(i, 0));
        m_obstacles.add("wall", hg::Vec2i(0, i));
        m_obstacles.add("wall", hg::Vec2i(i, 10));
        m_obstacles.add("wall", hg::Vec2i(10, i));
    }

    for (int i = 1; i < 10; i++) {
        for (int j = 1; j < 10; j++) {
            m_props.add("floor", hg::Vec2i(i, j));
        }
    }

    m_focus = entities.add();
    m_focus->transform.position[2] = 1;
    m_focus->addComponent<Quad>(TILE_SIZE, TILE_SIZE * 0.5);

}

void Runtime::onActivate() {

}

void Runtime::onDeactivate() {
    Scene::onDeactivate();
}

void Runtime::onUpdate(double dt) {

    auto rawMousePos = window->input.keyboardMouse.mouse.position;
    rawMousePos[1] = window->size()[1] - rawMousePos[1];
    auto mousePos = m_actors.getMapPos(m_camera.camera()->getGamePos(rawMousePos));

    updateFocusState(mousePos.cast<int>());
    m_focus->transform.position = m_actors.getWorldPos(mousePos);
    m_focus->transform.position[2] = 1.0f;

    m_camera.update(dt);

    if (window->input.keyboardMouse.mouse.leftPressed) {
        if (m_actors.isValidPos(mousePos) && m_focusState == FocusState::Accessible) {
            m_actors.move(m_player, mousePos.cast<int>());
            m_camera.move(m_player->transform.position, 0.25);

            updateEnemies();
        }
    }

    auto shader = SHADERS.get("sprite");
    shader->use();

    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    entities.forEach<Sprite>([&](auto sprite, auto entity) {
        sprite->mesh()->update(&sprite->quad);
        shader->setMat4("model", entity->transform.getModel());
        auto texture = TEXTURES.get(sprite->texture);
        texture->bind();
        glActiveTexture(GL_TEXTURE0);
        sprite->mesh()->render();
    });

    shader = SHADERS.get("focus");
    shader->use();
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    auto focusQuad = m_focus->getComponent<Quad>();
    shader->setMat4("model", m_focus->transform.getModel());
    shader->setVec4("color", FocusColors[(int)m_focusState]);
    focusQuad->mesh()->render();

    shader = SHADERS.get("sprite");
    shader->use();
    shader->setMat4("model", hg::Mat4::Identity());
    shader->setMat4("view", m_uiCamera.view());
    shader->setMat4("projection", m_uiCamera.projection());

    auto font = FONTS.get("default").get();

    m_text->draw(font, mousePos);
    m_text->draw(font, "AP: " + std::to_string(m_player->getComponent<Actor>()->actionPoints), hg::Vec3(128, 0, 0));
    m_text->draw(font, "HP: " + std::to_string((int) m_player->getComponent<Actor>()->health), hg::Vec3(256, 0, 0));
    m_text->draw(font, "Turn: " + std::to_string(m_turn), hg::Vec3(384, 0, 0));
}

void Runtime::updateFocusState(hg::Vec2i mousePos) {
    auto actor = m_player->getComponent<Actor>();
    auto delta = mousePos - actor->position;

    m_focusState = FocusState::Accessible;

    if (m_actors.entityIsAt(m_player, mousePos)) {
        m_focusState = FocusState::Inaccessible;
        return;
    }

    auto enemies = m_actors.at(mousePos);

    if (enemies.size() > 0) {
        m_focusState = FocusState::Enemy;
        return;
    }

    auto obstacles = m_obstacles.at(mousePos);

    if (obstacles.size() > 0) {
        m_focusState = FocusState::Obstacle;
        return;
    }

    if (std::abs(delta.x()) > actor->movement || std::abs(delta.y()) > actor->movement) {
        m_focusState = FocusState::Inaccessible;
        return;
    }

    auto path = getPath(m_player->getComponent<Actor>()->position, mousePos);

    if (!path.has_value()) {
        m_focusState = FocusState::Inaccessible;
    }
}

std::optional<std::vector<hg::Vec2i>> Runtime::getPath(hg::Vec2i start, hg::Vec2i goal) {

    std::vector<hg::Vec2i> path;

    auto delta = goal - start;
    int dist = std::max(delta.x(), delta.y());
    hg::Vec2i dir = hg::Vec2i(hg::sign(delta.x()), hg::sign(delta.y()));

    if (delta == hg::Vec2i::Zero()) {
        return std::nullopt;
    }

    // It's just a straight line so directly check all the nodes
    if (delta.x() == 0 || delta.y() == 0) {
        for (int i = 1; i < dist; i++) {
            auto pos = start + dir * i;
            path.push_back(pos);
            if (m_actors.at(pos).size() > 0 || m_obstacles.at(pos).size() > 0) {
                return std::nullopt;
            }
        }
        return path;
    }

    // It's a more complex path

    PathFindingGrid grid;

    for (int i = 0; i <= std::abs(delta.x()); i++) {
        std::vector<bool> row;
        for (int j = 0; j <= std::abs(delta.y()); j++) {
            auto pos = start + hg::Vec2i(i, j).prod(dir);
            row.push_back(!(m_actors.at(pos).size() > 0 || m_obstacles.at(pos).size() > 0));
        }
        grid.push_back(row);
    }

    PathFinding pathFinding(grid);

    auto rawPath = pathFinding.search(hg::Vec2i::Zero(), hg::Vec2i(std::abs(delta.x()), std::abs(delta.y())));

    if (!rawPath.has_value()) {
        return std::nullopt;
    }

    for (auto& pos : rawPath.value()) {
        pos = start + pos.prod(dir);
        path.push_back(pos);
    }

    return path;
}

void Runtime::updateEnemies() {
    entities.forEach<MeleeEnemy>([&](MeleeEnemy* enemy, hg::Entity* entity) {
        if (enemy->target == nullptr) {
            return;
        }

        auto actor = entity->getComponent<Actor>();
        auto targetActor = enemy->target->getComponent<Actor>();

        auto path = getPath(actor->position, targetActor->position);

        if (!path.has_value()) {
            return;
        }

        for (int i = 0; i < std::min(actor->movement, (int) path.value().size()); i++) {
            std::cout << "MOVE TO " << path.value()[i] << "\n";
            m_actors.move(entity, path.value()[i]);
        }
    });
}


#endif
