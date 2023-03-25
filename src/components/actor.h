//
// Created by henry on 3/24/23.
//

#ifndef JAMEGAM_ACTOR_H
#define JAMEGAM_ACTOR_H

#include "tileObject.h"

class Actor : public TileObject {
public:

    int maxActionPoints = 1;
    int actionPoints = 1;
    int movement = 1;
    float health = 10;
    float defense = 1;
    float strength = 1;
    float luck = 1;

};

#endif //JAMEGAM_ACTOR_H
