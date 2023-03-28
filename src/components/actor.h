//
// Created by henry on 3/24/23.
//

#ifndef JAMEGAM_ACTOR_H
#define JAMEGAM_ACTOR_H

#include "tileObject.h"

class Actor : public TileObject {
public:

    std::string name = "";

    int maxActionPoints = 1;
    int actionPoints = 1;
    int baseMovement = 1;
    int movement = 1;
    int attackRange = 1;
    float maxHealth = 100.0f;
    float health = 100.0f;
    float strength = 1;

    float calcDamage(float baseDamage) {
        return baseDamage * strength;
    }

    void damage(float amount) {
        health -= amount;
        if (health <= 0) {
            health = 0;
        }
    }

    void heal(float amount) {
        health += amount;
        if (health > maxHealth) {
            health = maxHealth;
        }
    }
};

#endif //JAMEGAM_ACTOR_H
