#ifndef JAMEGAM_RUNTIME_HPP
#define JAMEGAM_RUNTIME_HPP

#include <hagame/core/scene.h>
#include <hagame/graphics/text.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/components/textRenderer.h>
#include <hagame/graphics/components/sprite.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/primitives/line.h>

#include "../assets.hpp"
#include "../cameraController.h"
#include "../map.h"
#include "../utils/pathfinding.h"
#include "../utils/dice.h"

#include "../deck.h"
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

inline int distance(hg::Vec2i a, hg::Vec2i b) {
    auto delta = b - a;
    return std::max(std::abs(delta.x()), std::abs(delta.y()));
}

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
        m_playerDeck(this),
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

    Deck m_playerDeck;
    std::vector<Card*> m_activePlayerCards;

    Dice<6> m_dice;

    Resolution m_resolution = HD;

    CameraController m_camera;
    OrthographicCamera m_uiCamera;

    Map<Actor> m_actors;
    Map<TileObject> m_props;
    Map<Obstacle> m_obstacles;

    std::unique_ptr<PathFinding> m_pathFinding;
    primitives::Line m_pathFindingLine;
    std::unique_ptr<MeshInstance> m_pathFindingLineMesh;

    std::unique_ptr<Text> m_text;

    hg::Entity* m_player;

    int m_turn = 1;

    FocusState m_focusState = FocusState::Accessible;
    hg::Entity* m_focus;

    void updateFocusState(hg::Vec2i mousePos);
    void updatePathfindingGrid();
    bool updatePlayer();
    void updateEnemies();

    void renderSprites();
    void renderText(double dt);
    void renderFocus();

    void activateCard(hg::Entity* entity, CardType type);
    void deactivateCard(hg::Entity* entity, CardType type);

    bool attack(hg::Entity* attacker, hg::Entity* enemy, bool& died);

    hg::Vec2i getMousePos();
    hg::Vec2 getGlobalMousePos();

    double updateTime = 0;
    double renderTime = 0;
};

void Runtime::onInit() {

    Card::InitializeFonts();

    m_uiCamera.size = m_resolution;
    m_camera.setResolution(m_resolution);

    m_playerDeck.pos = (CARD_SIZE * 0.6).resize<3>();

    Windows::Events.subscribe(WindowEvents::Resize, [&](auto window) {
    });

    m_text = std::make_unique<Text>();

    m_player = entities.add();

    Sprite* sprite;

    m_player = m_actors.add("balrug", hg::Vec2i(7, 2));
    m_player->getComponent<Actor>()->movement = 2;
    m_player->getComponent<Actor>()->baseMovement = 2;
    m_player->getComponent<Actor>()->strength = 10;
    m_player->getComponent<Actor>()->name = "Player";

    m_camera.move(m_player->transform.position, 1);

    auto dwarf = m_actors.add("dwarf", hg::Vec2i(7, 7));
    dwarf->addComponent<MeleeEnemy>()->target = m_player;
    dwarf->getComponent<Actor>()->name = "Dwarf";

    auto elf = m_actors.add("elf", hg::Vec2i(2, 7));
    elf->addComponent<MeleeEnemy>()->target = m_player;
    elf->getComponent<Actor>()->name = "High Elf";

    for (int i = 0; i <= 10; i++) {
        m_obstacles.add("wall", hg::Vec2i(i, 0));
        m_obstacles.add("wall", hg::Vec2i(0, i));
        m_obstacles.add("wall", hg::Vec2i(i, 10));
        m_obstacles.add("wall", hg::Vec2i(10, i));
    }

    m_obstacles.add("water", hg::Vec2i(4, 4));
    m_obstacles.add("water", hg::Vec2i(4, 5));
    m_obstacles.add("water", hg::Vec2i(5, 4));
    m_obstacles.add("water", hg::Vec2i(5, 5));

    m_focus = entities.add();
    m_focus->transform.position[2] = 1;
    m_focus->addComponent<Quad>(TILE_SIZE, TILE_SIZE * 0.5);


    m_pathFinding = std::make_unique<PathFinding>(hg::Vec2i(TILE_COUNT, TILE_COUNT));
    m_pathFindingLine.thickness(3);
    m_pathFindingLineMesh = std::make_unique<MeshInstance>(&m_pathFindingLine);

    m_playerDeck.add(CardType::HealthPotion);
    m_playerDeck.add(CardType::RunAway);
    m_playerDeck.add(CardType::FireBlast);
    m_playerDeck.add(CardType::Resurrect);

}

void Runtime::onActivate() {

}

void Runtime::onDeactivate() {
    Scene::onDeactivate();
}

void Runtime::onUpdate(double dt) {

    auto mousePos = getMousePos();
    auto font = FONTS.get("default").get();

    auto now = hg::utils::Clock::Now();

    updatePathfindingGrid();

    updateFocusState(mousePos);
    m_focus->transform.position = m_actors.getWorldPos(mousePos);
    m_focus->transform.position[2] = 1.0f;

    m_camera.update(dt);

    if (updatePlayer()) {
        updateEnemies();
        m_turn++;

        std::vector<Card*> activeCards;

        for (auto& card : m_activePlayerCards) {
            card->turns++;
            if (card->turns < CARD_TYPES[(int) card->type].turns) {
                activeCards.push_back(card);
            } else {
                deactivateCard(m_player, card->type);
                m_playerDeck.discard(card);
            }
        }

        m_activePlayerCards = activeCards;
    }

    updateTime = hg::utils::Clock::ToSeconds(hg::utils::Clock::Now() - now);

    now = hg::utils::Clock::Now();

    renderSprites();
    renderFocus();
    renderText(dt);

    //renderMapAndActors();
    //renderUI(dt);

    renderTime = hg::utils::Clock::ToSeconds(hg::utils::Clock::Now() - now);
}

void Runtime::updateFocusState(hg::Vec2i mousePos) {
    m_pathFindingLine.clearPoints();
    m_pathFindingLineMesh->update(&m_pathFindingLine);

    auto actor = m_player->getComponent<Actor>();
    auto delta = mousePos - actor->position;

    if (!m_actors.isValidPos(mousePos)) {
        m_focusState = FocusState::Inaccessible;
        return;
    }

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

    auto path = m_pathFinding->search(m_player->getComponent<Actor>()->position, mousePos);

    if (!path.has_value() || path.value().size() - 1 > actor->movement) {
        m_focusState = FocusState::Inaccessible;
        return;
    }

    for (auto& pt : path.value()) {
        m_pathFindingLine.addPoint(m_actors.getWorldPos(pt) + TILE_SIZE.resize<3>() * 0.5);
    }

    m_pathFindingLineMesh->update(&m_pathFindingLine);
}


bool Runtime::updatePlayer() {

    auto mousePos = getMousePos();

    m_playerDeck.update(getGlobalMousePos());

    if (window->input.keyboardMouse.mouse.leftPressed) {

        auto card = m_playerDeck.pullCard();

        if (card != nullptr) {
            m_activePlayerCards.push_back(card);
            activateCard(m_player, card->type);
            if (CARD_TYPES[(int) card->type].endsTurn) {
                return true;
            }
        }

        if (!m_actors.isValidPos(mousePos)) {
            return false;
        }

        if (m_focusState == FocusState::Accessible) {

            m_pathFinding->setGridAt(m_player->getComponent<Actor>()->position, true);

            m_actors.move(m_player, mousePos.cast<int>());
            m_camera.move(m_player->transform.position, 0.25);

            m_pathFinding->setGridAt(m_player->getComponent<Actor>()->position, false);

            return true;
        }

        if (m_focusState == FocusState::Enemy) {
            hg::Entity* enemy = m_actors.at(mousePos)[0];
            bool died;
            if (!attack(m_player, enemy, died)) {
                return false;
            }

            if (died) {
                m_actors.destroy(enemy);
            }

            return true;
        }
    }

    return false;
}


void Runtime::updateEnemies() {
    entities.forEach<MeleeEnemy>([&](MeleeEnemy* enemy, hg::Entity* entity) {
        if (enemy->target == nullptr) {
            return;
        }

        auto actor = entity->getComponent<Actor>();
        auto targetActor = enemy->target->getComponent<Actor>();

        auto path = m_pathFinding->search(actor->position, targetActor->position, 2);

        if (!path.has_value()) {
            std::cout << "No path\n";
            return;
        }

        std::cout << "Path Size = " << path.value().size() << "\n";

        if (path.value().size() > 0) {
            bool died;
            std::cout << "ATTACK PLAYER = " << attack(entity, m_player, died) << "\n";

            std::cout << "PLAYER DIED :0 \n";
        }

        m_pathFinding->setGridAt(actor->position, true);

        for (int i = 0; i < std::min(actor->movement, (int) path.value().size() - 1); i++) {
            m_actors.move(entity, path.value()[i + 1]);
        }

        m_pathFinding->setGridAt(actor->position, false);
    });
}

void Runtime::updatePathfindingGrid() {
    auto start = hg::utils::Clock::Now();

    // TODO: optimize this somehow.
    for (int i = 0; i < TILE_COUNT; i++) {
        for (int j = 0; j < TILE_COUNT; j++) {
            auto pos = hg::Vec2i(j, i);
            m_pathFinding->setGridAt(pos, !(m_actors.at(pos).size() > 0 || m_obstacles.at(pos).size() > 0));
        }
    }
    //std::cout << "UPDATED PATH GRID IN " << hg::utils::Clock::ToSeconds(hg::utils::Clock::Now() - start) << "\n";
}


hg::Vec2 Runtime::getGlobalMousePos() {
    auto rawMousePos = window->input.keyboardMouse.mouse.position;
    rawMousePos[1] = window->size()[1] - rawMousePos[1];
    return rawMousePos;
}

hg::Vec2i Runtime::getMousePos() {
    return m_actors.getMapPos(m_camera.camera()->getGamePos(getGlobalMousePos()));
}

bool Runtime::attack(hg::Entity *attacker, hg::Entity *enemy, bool& died) {
    auto attackActor = attacker->getComponent<Actor>();
    auto enemyActor = enemy->getComponent<Actor>();

    auto dist = distance(attackActor->position, enemyActor->position);

    if (dist > attackActor->attackRange) {
        return false;
    }

    auto diceRoll = m_dice.roll();
    std::cout << "DICE ROLL = " << diceRoll << "\n";

    auto damage = attackActor->calcDamage(diceRoll);
    std::cout << "DAMAGE = " << damage << "\n";

    enemyActor->damage(damage);

    died = enemyActor->health <= 0;

    return true;
}

void Runtime::renderSprites() {
    auto shader = SHADERS.get("sprite").get();
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

    shader->setMat4("view", m_uiCamera.view());
    shader->setMat4("projection", m_uiCamera.projection());

    m_playerDeck.renderSprites(shader);
}

void Runtime::renderFocus() {

    if (!m_actors.isValidPos(getMousePos())) {
        return;
    }

    auto shader = SHADERS.get("focus");

    shader->use();
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    shader->setMat4("model", hg::Mat4::Identity());
    shader->setVec4("color", Color::red());
    m_pathFindingLineMesh->render();

    auto focusQuad = m_focus->getComponent<Quad>();
    shader->setMat4("model", m_focus->transform.getModel());
    shader->setVec4("color", FocusColors[(int)m_focusState]);
    focusQuad->mesh()->render();
}

void Runtime::renderText(double dt) {
    auto font = FONTS.get("default").get();
    auto shader = SHADERS.get("text").get();
    shader->use();

    shader->setVec4("textColor", Color::red());
    shader->setMat4("view", m_camera.camera()->view());
    shader->setMat4("projection", m_camera.camera()->projection());

    entities.forEach<Actor>([&](auto actor, auto entity) {
        shader->setVec4("textColor", entity == m_player ? Color::blue() : Color::red());
        shader->setMat4("model", entity->transform.getModel());
        m_text->draw(font, "(" + std::to_string((int) actor->health) + ")", hg::Vec3(0, TILE_SIZE[1] * 0.5, 0), TextHAlignment::Center);
    });

    shader->setVec4("textColor", hg::graphics::Color::red());
    shader->setMat4("model", hg::Mat4::Identity());
    shader->setMat4("view", m_uiCamera.view());
    shader->setMat4("projection", m_uiCamera.projection());

    m_text->draw(font, "FPS: " + std::to_string((int) (1 / dt)));
    m_text->draw(font, "U: " + std::to_string(updateTime), hg::Vec3(128, 0, 0));
    m_text->draw(font, "R: " + std::to_string(renderTime), hg::Vec3(400, 0, 0));

    entities.forEach<TextRenderer>([&](auto text, auto entity) {
        shader->setVec4("textColor", text->color);
        auto font = FONTS.get(text->font).get();
        if (text->fixedWidth) {
            m_text->draw(font, text->message, text->size, text->pos, text->hAlignment, text->vAlignment);
        } else {
            m_text->draw(font, text->message, text->pos, text->hAlignment, text->vAlignment);
        }
    });

    m_playerDeck.renderText(shader);
}

void Runtime::activateCard(hg::Entity *entity, CardType type) {
    switch (type) {
        case CardType::RunAway:
        {
            entity->getComponent<Actor>()->movement = RUN_AMOUNT;
        }
        break;
        case CardType::HealthPotion:
        {
            entity->getComponent<Actor>()->heal(HEALTH_POTION_AMOUNT);
        }
        break;
        case CardType::FireBlast:
        {
            auto actor = entity->getComponent<Actor>();
            for (int i = -2; i < 2; i++) {
                for (int j = -2; j < 2; j++) {
                    if (i != 0 || j != 0) {
                        auto pos = actor->position + hg::Vec2i(i, j);
                        auto actors = m_actors.at(pos);

                        for (auto& actor : actors) {
                            actor->getComponent<Actor>()->damage(FIRE_BLAST_DAMAGE);
                        }
                    }
                }
            }
        }
        break;
        case CardType::Resurrect:
        {
            m_playerDeck.returnRandomCardToHand();
        }
        break;
        default:
            throw std::runtime_error("Unhandled card type");
    }
}

void Runtime::deactivateCard(hg::Entity *entity, CardType type) {
    switch (type) {
        case CardType::RunAway:
            {
                entity->getComponent<Actor>()->movement = entity->getComponent<Actor>()->baseMovement;
            }
            break;
        default:
            return;
    }
}

#endif
