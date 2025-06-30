#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Zero {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    struct Triangle {
        Vertex vertices[3];
    };

    class Mesh {
    public:
        Mesh() = default;
        ~Mesh() = default;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;

        // Get triangles for rendering
        std::vector<Triangle> GetTriangles() const {
            std::vector<Triangle> triangles;

            // Process each triangle (3 indices at a time)
            for (size_t i = 0; i < indices.size(); i += 3) {
                Triangle tri;

                // Get the three vertices of this triangle
                tri.vertices[0] = vertices[indices[i]];
                tri.vertices[1] = vertices[indices[i + 1]];
                tri.vertices[2] = vertices[indices[i + 2]];

                triangles.push_back(tri);
            }

            return triangles;
        }

        // Helper method to compute face normals
        void ComputeFaceNormals() {
            for (size_t i = 0; i < indices.size(); i += 3) {
                uint32_t idx0 = indices[i];
                uint32_t idx1 = indices[i + 1];
                uint32_t idx2 = indices[i + 2];

                glm::vec3 v0 = vertices[idx0].position;
                glm::vec3 v1 = vertices[idx1].position;
                glm::vec3 v2 = vertices[idx2].position;

                glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                vertices[idx0].normal = normal;
                vertices[idx1].normal = normal;
                vertices[idx2].normal = normal;
            }
        }

        // Helper method to compute smooth normals
        void ComputeSmoothNormals() {
            // Initialize normals to zero
            for (auto& vertex : vertices) {
                vertex.normal = glm::vec3(0.0f);
            }

            // Accumulate face normals
            for (size_t i = 0; i < indices.size(); i += 3) {
                uint32_t idx0 = indices[i];
                uint32_t idx1 = indices[i + 1];
                uint32_t idx2 = indices[i + 2];

                glm::vec3 v0 = vertices[idx0].position;
                glm::vec3 v1 = vertices[idx1].position;
                glm::vec3 v2 = vertices[idx2].position;

                glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                vertices[idx0].normal += normal;
                vertices[idx1].normal += normal;
                vertices[idx2].normal += normal;
            }

            // Normalize accumulated normals
            for (auto& vertex : vertices) {
                if (glm::length(vertex.normal) > 0.0f) {
                    vertex.normal = glm::normalize(vertex.normal);
                }
                else {
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default up vector
                }
            }
        }
    };

} // namespace Zero