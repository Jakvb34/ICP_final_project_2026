#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <GL/glew.h>

#include "Mesh.hpp"
#include "OBJloader.hpp"

class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }

    void draw() {
        if (mesh_) {
            mesh_->draw();
        }
    }

private:
    std::unique_ptr<Mesh> mesh_;

    void loadModel(const std::string& path) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        if (!loadOBJ(path, vertices, indices)) {
            throw std::runtime_error("Failed to load OBJ file: " + path);
        }

        mesh_ = std::make_unique<Mesh>(vertices, indices, GL_TRIANGLES);
    }
};