//
// Created by henry on 3/25/23.
//

#ifndef JAMEGAM_MAP_H
#define JAMEGAM_MAP_H

#include <hagame/core/entity.h>
#include <hagame/core/scene.h>
#include <hagame/graphics/components/sprite.h>

#include "components/tileObject.h"

const hg::Vec2 TILE_SIZE = hg::Vec2(64, 64);

template <IsTileObject TileType>
class Map {
public:

    Map(hg::Scene* scene):
        m_scene(scene)
    {}

    // Instantiate an entity and add it to the Map
    hg::Entity* add(std::string texture, hg::Vec2i pos);

    // Remove an entity from map (but don't destroy it)
    void remove(hg::Entity* entity);

    // Remove an entity from map and destroy it
    void destroy(hg::Entity* entity);

    // Move an entity to another tile
    void move(hg::Entity* entity, hg::Vec2i pos);

    std::vector<hg::Entity*> at(hg::Vec2i pos);

    bool entityIsAt(hg::Entity* entity, hg::Vec2i pos);

    bool isValidPos(hg::Vec2i pos) const;

    hg::Vec3 getWorldPos(hg::Vec2i mapPos) const;
    hg::Vec2i getMapPos(hg::Vec2 worldPos) const;
private:

    const static int TILE_COUNT = 1000;

    using map_t = std::array<std::array<std::vector<hg::Entity*>, TILE_COUNT>, TILE_COUNT>;

    hg::Scene* m_scene;
    map_t m_entities;


    void throwInvalidPos(hg::Vec2i pos) const;
};

template<IsTileObject TileType>
hg::Vec2i Map<TileType>::getMapPos(hg::Vec2 worldPos) const {
    return worldPos.div(TILE_SIZE).cast<int>();
}

template<IsTileObject TileType>
hg::Vec3 Map<TileType>::getWorldPos(hg::Vec2i mapPos) const {
    return mapPos.cast<float>().prod(TILE_SIZE).resize<3>();
}


template<IsTileObject TileType>
bool Map<TileType>::entityIsAt(hg::Entity *entity, hg::Vec2i pos) {

    auto entities = at(pos);
    return std::find(entities.begin(), entities.end(), entity) != entities.end();
}

template<IsTileObject TileType>
std::vector<hg::Entity *> Map<TileType>::at(hg::Vec2i pos) {
    if (!isValidPos(pos)) {
        return {};
    }
    return m_entities[pos[0]][pos[1]];
}

template<IsTileObject TileType>
hg::Entity *Map<TileType>::add(std::string texture, hg::Vec2i pos) {

    throwInvalidPos(pos);

    auto entity = m_scene->entities.add();
    entity->transform.position = pos.cast<float>().prod(TILE_SIZE).resize<3>();
    entity->transform.position[2] = 2;
    auto tile = entity->addComponent<TileType>();
    tile->position = pos;
    auto sprite = entity->addComponent<hg::graphics::Sprite>(TILE_SIZE);
    sprite->quad.offset(TILE_SIZE * 0.5);
    sprite->texture = texture;

    m_entities[pos[0]][pos[1]].push_back(entity);

    return entity;
}

template<IsTileObject TileType>
void Map<TileType>::destroy(hg::Entity *entity) {
    remove(entity);
    m_scene->entities.remove(entity);
}

template<IsTileObject TileType>
void Map<TileType>::remove(hg::Entity *entity) {
    auto tile = entity->getComponent<TileType>();
    m_entities[tile->position[0]][tile->position[1]].erase(
            std::remove(
                    m_entities[tile->position[0]][tile->position[1]].begin(),
                    m_entities[tile->position[0]][tile->position[1]].end(),
                    entity
            ),
            m_entities[tile->position[0]][tile->position[1]].end()
    );
}


template<IsTileObject TileType>
void Map<TileType>::move(hg::Entity *entity, hg::Vec2i pos) {
    throwInvalidPos(pos);

    auto tile = entity->getComponent<TileType>();
    remove(entity);
    m_entities[pos[0]][pos[1]].push_back(entity);
    tile->position = pos;
    entity->transform.position = pos.cast<float>().prod(TILE_SIZE).resize<3>();
    entity->transform.position[2] = 2;
}

template<IsTileObject TileType>
bool Map<TileType>::isValidPos(hg::Vec2i pos) const {
    return pos.x() >= 0 && pos.x() < TILE_COUNT && pos.y() >= 0 && pos.y() < TILE_COUNT;
}


template<IsTileObject TileType>
void Map<TileType>::throwInvalidPos(hg::Vec2i pos) const {
    if (!isValidPos(pos)) {
        throw std::runtime_error(("Index: " + (std::string) pos + " out of bounds. Tile Count = " + std::to_string(TILE_COUNT)).c_str());
    }
}

#endif //JAMEGAM_MAP_H
