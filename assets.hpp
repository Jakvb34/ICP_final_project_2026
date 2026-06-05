#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    bool operator==(const Vertex& other) const {
        return position == other.position &&
               normal == other.normal &&
               texCoords == other.texCoords;
    }
};

// For old OBJloader.cpp compatibility
using vertex = Vertex;