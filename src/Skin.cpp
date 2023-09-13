#include "Tokenizer.h"
#include <iostream>
#include "Skeleton.cpp"
#include<map>

class Skin {
private:
    std::vector<Joint*> joints;
    std::vector<GLuint> indices;

    std::vector<glm::vec3> normals;
    std::vector<glm::vec3>positions;

    std::vector<glm::vec3> newNormals;
    std::vector<glm::vec3> newPositions;

    std::vector < std::vector < std::pair<int, float>>> weights;
    std::map<int, glm::mat4> bindings;

    glm::mat4 model;

    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

public:
    glm::vec3 color = glm::vec3(1.0, 0.0, 0.1);

public:

    Skin() {
        this->model = glm::mat4(1.0f);
    }

    void getJoint(std::vector<Joint*> j){
        this->joints = j;
    }

    void setColor(glm::vec3 vec) {
        color = vec;
    }

    void update() {
        newPositions.clear();
        newNormals.clear();
        for (int i = 0; i < positions.size(); i++) {
            glm::vec4 newPos = glm::vec4(0.0);
            glm::vec4 newNorm = glm::vec4(0.0);

            for (int j = 0; j < weights[i].size(); j++) {
                newPos += weights[i][j].second * joints[weights[i][j].first]->worldMat * glm::inverse(bindings[weights[i][j].first])* glm::vec4(positions[i],1.0f);
                newNorm += weights[i][j].second * joints[weights[i][j].first]->worldMat * glm::inverse(bindings[weights[i][j].first]) * glm::vec4(normals[i], 0.0f);
            }
            newPositions.push_back(newPos);
            newNormals.push_back(newNorm);
        }

        glDeleteBuffers(1, &VBO_positions);
        glDeleteBuffers(1, &VBO_normals);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_positions);
        glGenBuffers(1, &VBO_normals);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * newPositions.size(), newPositions.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * newNormals.size(), newNormals.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

	}

	void draw(const glm::mat4 view, GLuint shader) {
		// actiavte the shader program
		glUseProgram(shader);

		// get the locations and send the uniforms to the shader
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
        glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

		// Bind the VAO
		glBindVertexArray(VAO);

		// draw the points using triangles, indexed with the EBO
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

		// Unbind the VAO and shader program
		glBindVertexArray(0);
		glUseProgram(0);
	}

	bool load(const char* file) {
        Tokenizer token;
        token.Open(file);

        token.FindToken("positions");
        token.FindToken("{");
        bool flag = true;
        bool pos = true;
        bool nor = false;
        bool sw = false;
        bool tri = false;
        bool bind = false;
        int totBind;
        int joint_ind = 0;
        std::vector<glm::vec3> mat4_vec;
        while (flag) {
            token.SkipWhitespace();
            char c = token.CheckChar();
            if (c == '}') {
                if (pos) {
                    token.FindToken("normals");
                    token.FindToken("{");
                    pos = false;
                    nor = true;
                }
                else if (nor) {
                    token.FindToken("skinweights");
                    token.FindToken("{");
                    nor = false;
                    sw = true;
                }
                else if (sw) {
                    token.FindToken("triangles");
                    token.FindToken("{");
                    tri = true;
                    sw = false;
                }
                else if (tri) {
                    token.FindToken("bindings");
                    totBind = token.GetInt();
                    tri = false;
                    bind = true;
                }
                else if (bind) {
                    if (token.FindToken("}"))
                        flag = false;
                }
            }
            else if (pos) {
                glm::vec3 p = { token.GetFloat(),token.GetFloat(),token.GetFloat() };
                positions.push_back(p);
            }
            else if (nor) {
                glm::vec3 n = { token.GetFloat(),token.GetFloat(),token.GetFloat() };
                normals.push_back(n);
            }
            else if (sw) {
                int num_att = token.GetInt();
                std::vector<std::pair<int, float>> skinw;
                while (num_att > 0) {
                    int joint = token.GetInt();
                    float weight = token.GetFloat();
                    skinw.push_back(std::make_pair(joint, weight));
                    num_att--;
                }
                weights.push_back(skinw);
            }
            else if (tri) {
                GLuint ind = token.GetInt();
                indices.push_back(ind);
            }
            else if (bind) {
                for (int i = 0; i < totBind; i++) {
                    token.FindToken("matrix");
                    token.FindToken("{");
                    float ax = token.GetFloat(); float ay = token.GetFloat(); float az = token.GetFloat();
                    float bx = token.GetFloat(); float by = token.GetFloat(); float bz = token.GetFloat();
                    float cx = token.GetFloat(); float cy = token.GetFloat(); float cz = token.GetFloat();
                    float dx = token.GetFloat(); float dy = token.GetFloat(); float dz = token.GetFloat();
                    glm::mat4 tempMat = glm::mat4(ax, ay, az, 0,
                        bx, by, bz, 0,
                        cx, cy, cz, 0,
                        dx, dy, dz, 1);
                    bindings[i] = tempMat;
                }
            }
            else
                token.SkipLine(); // Unrecognized token
        }

        token.Close();
        
		return true;
	}

};