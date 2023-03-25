//
// Created by henry on 3/25/23.
//

#ifndef JAMEGAM_TILEOBJECT_H
#define JAMEGAM_TILEOBJECT_H

#include <hagame/core/component.h>
#include <hagame/math/aliases.h>

class TileObject : public hg::Component {
public:
    hg::Vec2i position;
    std::string type;
};

template <typename T>
concept IsTileObject = std::is_base_of<TileObject, T>::value;

#endif //JAMEGAM_TILEOBJECT_H
