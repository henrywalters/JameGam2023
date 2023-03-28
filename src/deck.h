//
// Created by henry on 3/27/23.
//

#ifndef JAMEGAM_DECK_H
#define JAMEGAM_DECK_H

#include <hagame/core/scene.h>
#include <hagame/graphics/components/sprite.h>
#include <hagame/graphics/components/textRenderer.h>

#include "card.h"

class Deck {
public:

    hg::Vec3 pos;

    Deck(hg::Scene* scene):
        m_scene(scene)
    {}

    Card* add(CardType type);
    void discard(Card* card);

    void renderSprites(hg::graphics::ShaderProgram* shader);
    void renderText(hg::graphics::ShaderProgram* shader);

private:
    hg::Scene* m_scene;

    std::vector<std::unique_ptr<Card>> m_cards;

    std::vector<Card*> m_hand;
    std::vector<Card*> m_discard;

};

Card* Deck::add(CardType type) {
    m_cards.push_back(std::make_unique<Card>());
    auto card = m_cards[m_cards.size() - 1].get();
    m_hand.push_back(card);
    card->type = type;
    return card;
}

void Deck::renderSprites(hg::graphics::ShaderProgram *shader) {

    hg::Vec3 cardPos = pos;

    for (int i = 0; i < m_discard.size(); i++) {
        shader->setMat4("model", hg::Mat4::Translation(cardPos));
        m_discard[i]->renderBack();
        cardPos[0] += 5;
    }

    cardPos[0] + 50;

    for (int i = 0; i < m_hand.size(); i++) {
        shader->setMat4("model", hg::Mat4::Translation(cardPos));
        m_hand[i]->renderSprite();
        cardPos[0] += CARD_SIZE[0] + 10;
    }
}

void Deck::renderText(hg::graphics::ShaderProgram *shader) {

    shader->setVec4("textColor", hg::graphics::Color::black());

    hg::Vec3 cardPos = pos;

    for (int i = 0; i < m_discard.size(); i++) {
        cardPos[0] += 5;
    }

    cardPos[0] + 50;

    for (int i = 0; i < m_hand.size(); i++) {
        shader->setMat4("model", hg::Mat4::Translation(cardPos));
        m_hand[i]->renderText();
        cardPos[0] += CARD_SIZE[0] + 10;
    }

}


#endif //JAMEGAM_DECK_H
