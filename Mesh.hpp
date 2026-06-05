#pragma once

#include <vector>
#include <cstddef>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "assets.hpp"
#include "non_copyable.hpp"

class Mesh : private NonCopyable
{
public:
    static constexpr GLuint attribute_location_position{0};
    static constexpr GLuint attribute_location_normal{1};
    static constexpr GLuint attribute_location_texture_coords{2};

    Mesh() = delete;

    Mesh(std::vector<Vertex> const& vertices, GLenum primitive_type)
        : primitive_type_{primitive_type}, vertices{vertices}
    {
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(this->vertices.size() * sizeof(Vertex)),
            this->vertices.data(),
            GL_STATIC_DRAW
        );

        glEnableVertexAttribArray(attribute_location_position);
        glVertexAttribPointer(
            attribute_location_position,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, position))
        );

        glEnableVertexAttribArray(attribute_location_normal);
        glVertexAttribPointer(
            attribute_location_normal,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, normal))
        );

        glEnableVertexAttribArray(attribute_location_texture_coords);
        glVertexAttribPointer(
            attribute_location_texture_coords,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, texCoords))
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    Mesh(
        std::vector<Vertex> const& vertices,
        std::vector<GLuint> const& indices,
        GLenum primitive_type
    )
        : Mesh{vertices, primitive_type}
    {
        this->indices = indices;

        glBindVertexArray(vao_);

        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(this->indices.size() * sizeof(GLuint)),
            this->indices.data(),
            GL_STATIC_DRAW
        );

        glBindVertexArray(0);
    }

    void draw() {
        glBindVertexArray(vao_);

        if (ebo_ == 0) {
            glDrawArrays(
                primitive_type_,
                0,
                static_cast<GLsizei>(vertices.size())
            );
        } else {
            glDrawElements(
                primitive_type_,
                static_cast<GLsizei>(indices.size()),
                GL_UNSIGNED_INT,
                nullptr
            );
        }

        glBindVertexArray(0);
    }

    ~Mesh() {
        if (ebo_ != 0) {
            glDeleteBuffers(1, &ebo_);
        }

        if (vbo_ != 0) {
            glDeleteBuffers(1, &vbo_);
        }

        if (vao_ != 0) {
            glDeleteVertexArrays(1, &vao_);
        }
    }

private:
    GLenum primitive_type_{GL_POINTS};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    GLuint vao_{0};
    GLuint vbo_{0};
    GLuint ebo_{0};
};