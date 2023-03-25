//
// Created by henry on 3/25/23.
//

#ifndef JAMEGAM_PATHFINDING_H
#define JAMEGAM_PATHFINDING_H

#include <hagame/math/aliases.h>

float distanceMetric(hg::Vec2i a, hg::Vec2i b) {
    auto delta = b - a;
    return std::max(std::abs(delta.x()), std::abs(delta.y()));
}

struct PathFindingNode {
    hg::Vec2i position;
    bool accessible;
    float g, f;
    PathFindingNode* parent;
};

using IsAccessible = bool;
using PathFindingGrid = std::vector<std::vector<IsAccessible>>;

class PathFinding {
public:

    PathFinding(PathFindingGrid grid):
        m_grid(grid)
    {}

    std::optional<std::vector<hg::Vec2i>> search(hg::Vec2i startPos, hg::Vec2i goalPos) {

        m_openList.clear();
        m_closedList.clear();

        auto start = std::make_shared<PathFindingNode>();
        start->position = startPos;
        start->g = 0;
        start->f = start->g + distanceMetric(startPos, goalPos);

        m_openList.push_back(start);

        while (m_openList.size() > 0) {
            auto current = m_openList[0];

            if (current->position == goalPos) {
                return constructPath(current.get());
            }

            removeFromOpenList(current.get());

            m_closedList.push_back(current);

            auto neighbors = findNeighbors(current.get());

            for (const auto& neighbor : neighbors) {
                if (!inClosedList(neighbor.get())) {
                    neighbor->f = neighbor->g + distanceMetric(neighbor->position, goalPos);
                    if (!inOpenList(neighbor.get())) {
                        m_openList.push_back(neighbor);
                    } else {
                        auto openNeighbor = getOpenNeighbor(neighbor.get());
                        openNeighbor->g = neighbor->g;
                        openNeighbor->parent = neighbor->parent;
                    }
                }
            }

            std::sort(m_openList.begin(), m_openList.end(), [](const auto& a, const auto& b) {
                return a->f < b->f;
            });
        }

        return std::nullopt;
    }

private:

    std::vector<std::shared_ptr<PathFindingNode>> m_openList;
    std::vector<std::shared_ptr<PathFindingNode>> m_closedList;
    PathFindingGrid m_grid;

    PathFindingNode* getOpenNeighbor(PathFindingNode* node) {
        int index = -1;
        for (int i = 0; i < m_openList.size(); i++) {
            if (m_openList[i]->position == node->position) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            return m_openList[index].get();
        }

        return nullptr;
    }

    void removeFromOpenList(PathFindingNode* node) {
        int index = -1;
        for (int i = 0; i < m_openList.size(); i++) {
            if (m_openList[i]->position == node->position) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            m_openList.erase(m_openList.begin() + index);
        }
    }

    bool inOpenList(PathFindingNode* node) {
        for (const auto& other : m_openList) {
            if (node->position == other->position) {
                return true;
            }
        }
        return false;
    }

    bool inClosedList(PathFindingNode* node) {
        for (const auto& other : m_closedList) {
            if (node->position == other->position) {
                return true;
            }
        }
        return false;
    }

    std::vector<hg::Vec2i> constructPath(PathFindingNode* node) {
        std::vector<hg::Vec2i> path;
        path.push_back(node->position);
        while (node->parent != nullptr) {
            node = node->parent;
            path.push_back(node->position);
        }
        return path;
    }

    std::vector<std::shared_ptr<PathFindingNode>> findNeighbors(PathFindingNode* node) {
        std::vector<std::shared_ptr<PathFindingNode>> neighbors;

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {

                if (i == 0 && j == 0) {
                    continue;
                }

                hg::Vec2i pos = node->position + hg::Vec2i(i, j);
                if (pos.x() < 0 || pos.x() >= m_grid.size() || pos.y() < 0 || pos.y() >= m_grid[0].size()) {
                    continue;
                }

                if (m_grid[pos.x()][pos.y()]) {
                    auto neighbor = std::make_shared<PathFindingNode>();
                    neighbor->parent = node;
                    neighbor->position = pos;
                    neighbor->g = 1;
                    neighbors.push_back(neighbor);
                }
            }
        }

        return neighbors;
    }

};


#endif //JAMEGAM_PATHFINDING_H
