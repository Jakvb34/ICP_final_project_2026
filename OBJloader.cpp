#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "OBJloader.hpp"

bool parseFaceVertex(
    const std::string& text,
    unsigned int& vertexIndex,
    unsigned int& texCoordIndex,
    unsigned int& normalIndex
) {
    vertexIndex = 0;
    texCoordIndex = 0;
    normalIndex = 0;

    size_t firstSlash = text.find('/');
    size_t secondSlash = text.find('/', firstSlash + 1);

    if (firstSlash == std::string::npos) {
        vertexIndex = std::stoul(text);
        return true;
    }

    vertexIndex = std::stoul(text.substr(0, firstSlash));

    if (secondSlash == std::string::npos) {
        texCoordIndex = std::stoul(text.substr(firstSlash + 1));
        return true;
    }

    std::string texPart = text.substr(firstSlash + 1, secondSlash - firstSlash - 1);
    std::string normalPart = text.substr(secondSlash + 1);

    if (!texPart.empty()) {
        texCoordIndex = std::stoul(texPart);
    }

    if (!normalPart.empty()) {
        normalIndex = std::stoul(normalPart);
    }

    return true;
}

bool loadOBJ(
    const std::filesystem::path& filename,
    std::vector<vertex>& vertices,
    std::vector<GLuint>& indices
) {
    std::cout << "Loading model: " << filename << std::endl;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    vertices.clear();
    indices.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Impossible to open the file!\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string header;
        iss >> header;

        if (header == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            temp_vertices.push_back(position);
        }
        else if (header == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (header == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (header == "f") {
            std::vector<std::string> faceParts;
            std::string part;

            while (iss >> part) {
                faceParts.push_back(part);
            }

            if (faceParts.size() < 3) {
                continue;
            }

            for (int i = 1; i + 1 < static_cast<int>(faceParts.size()); ++i) {
                std::array<std::string, 3> triangle = {
                    faceParts[0],
                    faceParts[i],
                    faceParts[i + 1]
                };

                for (const auto& faceVertexText : triangle) {
                    unsigned int vertexIndex;
                    unsigned int texCoordIndex;
                    unsigned int normalIndex;

                    parseFaceVertex(
                        faceVertexText,
                        vertexIndex,
                        texCoordIndex,
                        normalIndex
                    );

                    vertex currentVertex;

                    currentVertex.position = temp_vertices[vertexIndex - 1];

                    if (texCoordIndex > 0 && texCoordIndex <= temp_uvs.size()) {
                        currentVertex.texCoords = temp_uvs[texCoordIndex - 1];
                    } else {
                        currentVertex.texCoords = glm::vec2(0.0f, 0.0f);
                    }

                    if (normalIndex > 0 && normalIndex <= temp_normals.size()) {
                        currentVertex.normal = temp_normals[normalIndex - 1];
                    } else {
                        currentVertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    auto it = std::find_if(
                        vertices.begin(),
                        vertices.end(),
                        [&currentVertex](const vertex& v2) {
                            return currentVertex == v2;
                        }
                    );

                    GLuint currentIndex;

                    if (it == vertices.end()) {
                        vertices.push_back(currentVertex);
                        currentIndex = static_cast<GLuint>(vertices.size() - 1);
                    } else {
                        currentIndex = static_cast<GLuint>(
                            std::distance(vertices.begin(), it)
                        );
                    }

                    indices.push_back(currentIndex);
                }
            }
        }
    }

    std::cout << "Model loaded: " << filename << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Indices: " << indices.size() << std::endl;

    return true;
}