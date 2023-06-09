//
// Created by henry on 3/27/23.
//

#ifndef JAMEGAM_CARD_H
#define JAMEGAM_CARD_H

#include <hagame/core/component.h>

#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/text.h>
#include <hagame/graphics/texture.h>

#include "assets.hpp"
#include "components/actor.h"

const hg::Vec2 CARD_SIZE = hg::Vec2(128, 190);
const std::string CARD_TEMPLATE = "card_template";
const std::string CARD_BACK = "card_back";
const std::string CARD_FONT = "card_font";
const int CARD_NAME_SIZE = 20;
const int CARD_DESC_SIZE = 14;

const float HEALTH_POTION_AMOUNT = 50.0f;
const int RUN_AMOUNT = 5;

const float FIRE_BLAST_DAMAGE = 50.0f;

struct CardDefinition {
    std::string name;
    std::string description;
    std::string cardTexture;
    bool endsTurn;
    int turns;
    // card_f onUse = [](const auto& scene, const auto& entity) {};
    // card_f afterUse = [](const auto& scene, const auto& entity) {};
};

enum class CardType {
    HealthPotion,
    RunAway,
    FireBlast,
    Resurrect,
};

const std::vector<CardDefinition> CARD_TYPES = {
        CardDefinition{"Health Potion", "Restore some health.", "card_health", true, 1},
        CardDefinition{"Run Away", "Run up to " + std::to_string(RUN_AMOUNT) + " tiles for 2 turns." , "card_move", false, 2},
        CardDefinition{"Fire Blast", "Deal fire damage to all enemies in 2 tile radius.", "card_fire", true, 1},
        CardDefinition{"Resurrect", "Bring a random card back from the grave.", "card_cross", true, 1},
};

class CardComponent : public hg::Component {
public:
    CardType type;
};

class Card {
public:

    CardType type;
    int turns = 0;

    Card():
        m_quad(CARD_SIZE),
        m_mesh(&m_quad)
    {}

    static void InitializeFonts() {
        auto nameFont = loadFont("card_name", "fonts/apple_kid.ttf");
        auto descFont = loadFont("card_desc", "fonts/apple_kid.ttf");

        nameFont->fontSize(CARD_NAME_SIZE);
        descFont->fontSize(CARD_DESC_SIZE);
    }

    void renderSprite() {
        auto texture = TEXTURES.get(CARD_TYPES[(int) type].cardTexture);
        texture->bind();
        glActiveTexture(GL_TEXTURE0);
        m_mesh.render();
    }

    void renderBack() {
        auto texture = TEXTURES.get(CARD_BACK);
        texture->bind();
        glActiveTexture(GL_TEXTURE0);
        m_mesh.render();
    }

    void renderText() {
        m_text.draw(FONTS.get("card_name").get(), CARD_TYPES[(int) type].name, hg::Vec3(-27, 35), hg::graphics::TextHAlignment::Left);
        m_text.draw(FONTS.get("card_desc").get(), CARD_TYPES[(int) type].description, hg::Vec3(100, 100), hg::Vec3(-25, -25), hg::graphics::TextHAlignment::Left);
    }

    void render(hg::Vec3 pos, hg::graphics::ShaderProgram* spriteShader, hg::graphics::ShaderProgram* textShader) {
        /*
        spriteShader->use();

        spriteShader->setMat4("model", hg::Mat4::Translation(pos));

        auto texture = TEXTURES.get(CARD_TEMPLATE);
        texture->bind();
        glActiveTexture(GL_TEXTURE0);
        m_mesh.render();

        textShader->use();
        textShader->setVec4("textColor", hg::graphics::Color::black());
        textShader->setMat4("model", hg::Mat4::Translation(pos));

        m_text.draw(FONTS.get("card_name").get(), name, hg::Vec3(-27, 35), hg::graphics::TextHAlignment::Left);
        m_text.draw(FONTS.get("card_desc").get(), description, hg::Vec3(100, 100), hg::Vec3(-25, -25), hg::graphics::TextHAlignment::Left);
         */
    }

private:

    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;
    hg::graphics::Text m_text;
};

#endif //JAMEGAM_CARD_H
