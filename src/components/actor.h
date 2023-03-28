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
    int movement = 1;
    int attackRange = 1;
    float health = 100.0f;
    float strength = 10;

    float calcDamage(float baseDamage) {
        std::cout << std::log(strength) << "\n";
        return baseDamage * std::log(strength);
    }
};

#endif //JAMEGAM_ACTOR_H
