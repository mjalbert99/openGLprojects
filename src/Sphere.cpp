#include <vector>
#include "core.h"
class Sphere {
public:
    GLuint VBO_positions, VBO_normals, EBO, VAO;

    glm::vec3 color, position;
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::ivec3> indices;
    glm::mat4 model = glm::mat4(1.0f);

    float radius;
    int numSegments = 16;

public:

    Sphere(glm::vec3 pos, float r, int seg) {
        numSegments = seg;
        position = pos;
        radius = r;
        color = glm::vec3(1, 0, 0); // red
        
        // Generate sphere positions
        for (int i = 0; i <= numSegments; i++) {
            float theta = i * glm::two_pi<float>() / numSegments;
            for (int j = 0; j <= numSegments; j++) {
                float phi = j * glm::pi<float>() / numSegments;
                glm::vec3 vertex(
                    (radius * sin(phi) * cos(theta)) + position.x,
                    (radius * sin(phi) * sin(theta)) + position.y,
                    (radius * cos(phi)) + position.z
                );
                glm::vec3 normal = glm::normalize(vertex - position);
                positions.push_back(vertex);
                normals.push_back(normal);
            }
        }

        // Generate sphere indices
        for (int i = 0; i < numSegments; i++) {
            for (int j = 0; j < numSegments; j++) {
                int i1 = i * (numSegments + 1) + j;
                int i2 = i1 + (numSegments + 1);

                indices.push_back(glm::ivec3(i1, i2, i1 + 1));
                indices.push_back(glm::ivec3(i1 + 1, i2, i2 + 1));
            }
        }

        createBuffer();
    }

    void createBuffer() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_positions);
        glGenBuffers(1, &VBO_normals);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

    void deleteBuffer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO_positions);
        glDeleteBuffers(1, &VBO_normals);
        glDeleteBuffers(1, &EBO);
    }

    void updatePosition(glm::vec3 position) {
        this->position = position;

        for (int i = 0; i <= numSegments; i++) {
            float theta = i * glm::two_pi<float>() / numSegments;
            for (int j = 0; j <= numSegments; j++) {
                float phi = j * glm::pi<float>() / numSegments;
                glm::vec3 vertex(
                    (radius * sin(phi) * cos(theta)) + position.x,
                    (radius * sin(phi) * sin(theta)) + position.y,
                    (radius * cos(phi)) + position.z
                );
                glm::vec3 normal = glm::normalize(vertex - position);
                positions[i * (numSegments + 1) + j] = vertex;
                normals[i * (numSegments + 1) + j] = normal;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);

    }

    void Draw(const glm::mat4& view, GLuint shader) {
        glUseProgram(shader);

        glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&view);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
        glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color.x);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

};