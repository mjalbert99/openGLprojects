#include "core.h"
#include <iostream>
#include <vector>
#include "Triangle.cpp"
#include "SpringDamper.cpp"

class Cloth {
public:
	std::vector<Particle*> parts;
	std::vector<SpringDamper*> springs;
	std::vector<Triangle*> tris;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> norms;
	std::vector<glm::ivec3> indices;

	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 color = glm::vec3(1.0f, 0.f, 0.f);

	float springConstant = 500.0f;
	float damperConstant = 1.0f;
	Ground* ground;
	glm::vec3 air;

public:
	void createBuffer() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO_positions);
		glGenBuffers(1, &VBO_normals);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * norms.size(), norms.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}																																

	void updateBuffer() {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * positions.size(), positions.data());

		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * norms.size(), norms.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void updateNorm() {
		for (Particle* p : parts) {
			p->normal = glm::vec3(0.f, 0.f, 0.f);
			norms[p->index] = glm::vec3(0.f, 0.f, 0.f);
		}

		for (Triangle* t : tris) {
			Particle* p1 = t->p1;
			Particle* p2 = t->p2;
			Particle* p3 = t->p3;
			glm::vec3 n1 = p2->pos - p1->pos;
			glm::vec3 n2 = p3->pos - p1->pos;
			glm::vec3 new_n = glm::normalize(glm::cross(n1, n2));

			p1->normal += new_n;
			p2->normal += new_n;
			p3->normal += new_n;

			norms[p1->index] += glm::normalize(new_n);
			norms[p2->index] += glm::normalize(new_n);
			norms[p3->index] += glm::normalize(new_n);
		}
	}

	Cloth(int height, int width) {
		air = glm::vec3(0.0f, 0.0f, 0.0f);
		ground = new Ground();
		ground->createBuffer();

		// Create Particles
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				glm::vec3 pos = glm::vec3(0.1f * (((float)j) - width / 2.0), 0.1 * i + 0.5, 0.0);
				Particle* p = new Particle(pos, .1, false, ground);
				// Top corners
				if ((i == height-1 ) ) {
				p->fixed = true;
				}

				// Bottom Corners
				//if ((i == 0) && (j == 0 || j == width - 1)) {
				//	p->fixed = true;
				//}
				p->index = i * width + j;
				positions.push_back(pos);
				parts.push_back(p);
				norms.push_back(glm::vec3(0.0));
			}
		}

		// Create Springs
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (i < height - 1) {
					Particle* p1 = parts[i * width + j];
					Particle* p2 = parts[(i + 1) * width + j];
					SpringDamper* sd = new SpringDamper(p1, p2, springConstant, damperConstant, glm::length(p1->pos - p2->pos));
					springs.push_back(sd);
					if (j > 0) {
						Particle* p1 = parts[i * width + j];
						Particle* p2 = parts[(i + 1) * width + j - 1];
						SpringDamper* sd = new SpringDamper(p1, p2, springConstant, damperConstant, glm::length(p1->pos - p2->pos));
						springs.push_back(sd);
					}
					if (j < width - 1) {
						Particle* p1 = parts[i * width + j];
						Particle* p2 = parts[(i + 1) * width + j + 1];
						SpringDamper* sd = new SpringDamper(p1, p2, springConstant, damperConstant, glm::length(p1->pos - p2->pos));
						springs.push_back(sd);
					}
				}
				if (j < width - 1) {
					Particle* P1 = parts[i * width + j];
					Particle* P2 = parts[i * width + j + 1];
					SpringDamper* sd2 = new SpringDamper(P1, P2, springConstant, damperConstant, glm::length(P1->pos - P2->pos));
					springs.push_back(sd2);
				}
			}
		}

		// Create Triangles
		for (int i = 0; i < height - 1; i++) {
			for (int j = 0; j < width - 1; j++) {
				int index1 = i * width + j;
				int index2 = i * width + j + 1;
				int index3 = (i + 1) * width + j;
				int index4 = (i + 1) * width + j + 1;

				indices.push_back(glm::vec3(index1, index2, index3));
				indices.push_back(glm::ivec3(index2, index4, index3));

				Triangle* t = new Triangle(parts[index1], parts[index2], parts[index3]);
				tris.push_back(t);

				Triangle* t2 = new Triangle(parts[index2], parts[index4], parts[index3]);
				tris.push_back(t2);
			}
		}
		updateNorm();
		createBuffer();
	}

	void update(float deltaTime) {
		for (Particle* p : parts) {
			if (p->fixed) {
				continue;
			}
			p->integrate(deltaTime);
			positions[p->index] = p->pos;
		}

		for (SpringDamper* sd : springs) {
			sd->computeForce();
		}

		for (Triangle* t : tris) {
			t->computeAero(air);
		}

		updateNorm();
		updateBuffer();
	}

	void draw(const glm::mat4& view, GLuint shader) {
		ground->Draw(view, shader);
		glUseProgram(shader);

		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
		glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, (void*)0);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void updateFixed(glm::vec3 offset) {
		for (Particle* p : parts) {
			if (p->fixed && offset.y < 0) {
				if (p->pos.y - offset.y <= 1e-6) {
					return;
				}
			}
		}
		for (Particle* p : parts) {
			if (p->fixed) {
				p->pos += offset;
				if (p->pos.y <= 1e-6) {
					p->pos.y = 1.5e-4;
				}
			}
		}
		updateNorm();
		updateBuffer();

		for (int i = 0; i < parts.size(); i++) {
			positions[i] = parts[i]->pos;
		}
	}
};