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

    Card* pullCard();

    void update(hg::Vec2 mousePos);

    Card* add(CardType type);
    void discard(Card* card);
    void returnToHand(Card* card);
    void returnRandomCardToHand();

    void renderSprites(hg::graphics::ShaderProgram* shader);
    void renderText(hg::graphics::ShaderProgram* shader);

private:
    hg::Scene* m_scene;

    std::vector<std::unique_ptr<Card>> m_cards;

    std::vector<Card*> m_hand;
    std::vector<Card*> m_discard;

    int m_hovered = -1;

};

Card *Deck::pullCard() {
    return m_hovered == -1 ? nullptr : m_hand[m_hovered];
}

void Deck::update(hg::Vec2 mousePos) {
    m_hovered = -1;

    hg::Vec3 cardPos = pos;

    for (int i = 0; i < m_discard.size(); i++) {
        cardPos[0] += 5;
    }

    if (m_discard.size() > 0) {
        cardPos[0] += CARD_SIZE[0] + 20;
    }

    for (int i = 0; i < m_hand.size(); i++) {
        if (hg::rectContainsPoint(cardPos.resize<2>() - CARD_SIZE * 0.5, CARD_SIZE, mousePos)) {
            m_hovered = i;
        }
        cardPos[0] += CARD_SIZE[0] + 10;
    }
}

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

    if (m_discard.size() > 0) {
        cardPos[0] += CARD_SIZE[0] + 20;
    }

    for (int i = 0; i < m_hand.size(); i++) {

        auto spritePos = cardPos;

        if (m_hovered == i) {
            spritePos[1] += 50;
        }

        shader->setMat4("model", hg::Mat4::Translation(spritePos));
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

    if (m_discard.size() > 0) {
        cardPos[0] += CARD_SIZE[0] + 20;
    }

    for (int i = 0; i < m_hand.size(); i++) {
        auto textPos = cardPos;

        if (m_hovered == i) {
            textPos[1] += 50;
        }

        shader->setMat4("model", hg::Mat4::Translation(textPos));
        m_hand[i]->renderText();
        cardPos[0] += CARD_SIZE[0] + 10;
    }

}

void Deck::discard(Card *card) {
    int index = -1;
    for (int i = 0; i < m_hand.size(); i++) {
        if (m_hand[i] == card) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        m_hand.erase(m_hand.begin() + index);
        m_discard.push_back(card);
    }
}

void Deck::returnToHand(Card *card) {
    int index = -1;
    for (int i = 0; i < m_discard.size(); i++) {
        if (m_discard[i] == card) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        m_discard.erase(m_discard.begin() + index);
        m_hand.push_back(card);
    }
}

void Deck::returnRandomCardToHand() {
    if (m_discard.size() == 0) {
        return;
    }

    hg::utils::Random rand;

    returnToHand(m_discard[rand.integer<int>(0, m_discard.size())]);
}


#endif //JAMEGAM_DECK_H
