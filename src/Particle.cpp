#include "core.h"
#include <Particle.h>



	Particle::Particle(glm::vec3 p, float m, bool f, Ground* grnd) {
		pos = p;
		mass = m;
		fixed = f;
		force = glm::vec3(0);
		velocity = glm::vec3(0.1f);
		normal = glm::vec3(0);
		index = 0;
		ground = grnd;
	}

	void Particle::applyForce(glm::vec3& f) {
		force += f;
	}

	void Particle::integrate(float deltaTime) {
		if (fixed)
			return;

		force += glm::vec3(0.0, -5.0f, 0.0) * mass; // gravity in m/s^2
		glm::vec3 accel = (1 / mass) * force;
		velocity += accel * deltaTime;
		pos += velocity * deltaTime;
		checkGround();
		force = glm::vec3(0.0);
	}


	void Particle::checkGround() {
		if (glm::abs(pos.x) < glm::abs(ground->positions[0].x)) {
			if (glm::abs(pos.z) < glm::abs(ground->positions[0].z)) {
				if (pos.y <= 1e-6) {
					velocity.y = 0.0f;
					pos.y = 1.5e-4;
				}
			}

		}
	}
