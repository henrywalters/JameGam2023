//
// Created by henry on 3/25/23.
//

#ifndef JAMEGAM_MELEEENEMY_H
#define JAMEGAM_MELEEENEMY_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class MeleeEnemy : public hg::Component {
public:
    hg::Entity* target = nullptr;
};

#endif //JAMEGAM_MELEEENEMY_H
