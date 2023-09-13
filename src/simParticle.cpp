#include "simParticle.h"

simParticle::simParticle(glm::vec3 p, float m, Ground* grnd, float r, float e, float f, float lS, glm::vec3 velVar){
	pos = p;
	mass = m;
	kill = false;
	force = glm::vec3(0);
	velocity = velVar;
	normal = glm::vec3(0);
	index = 0;
	ground = grnd;
	radius = r;
	elascticity = e;
	friction = f;
	lifeSpan = lS;
}

void simParticle::integrate(float deltaTime, glm::vec3 gravity, glm::vec3 wind, float airDensity, float drag){
	timeAlive += deltaTime;
	if (timeAlive >= lifeSpan)
		kill = true;


	force += gravity * mass; // gravity in m/s^2
	force += wind;
	glm::vec3 vel_close = velocity - wind;
	glm::vec3 dragForce = 0.5f * airDensity * vel_close * vel_close * drag * glm::pi<float>() * radius * radius;
	force -= dragForce;
	glm::vec3 accel = (1 / mass) * force;
	velocity += accel * deltaTime;
	pos += velocity * deltaTime;
	checkPlane(radius);
	force = glm::vec3(0.0);
}

void simParticle::applyForce(glm::vec3& f){
	force += f;
}

void simParticle::checkPlane(float r) {
	if (glm::abs(pos.x) < glm::abs(ground->positions[0].x)) {
		if (glm::abs(pos.z) < glm::abs(ground->positions[0].z)) {
			if (pos.y - r <= ground->positions[0].y) {
				force = glm::vec3(0);

				float vel_y = velocity.y;
				if (vel_y < 1e-6) {
					pos.y = ground->positions[0].y + r;
				}
				else {
					pos.y = 2 * (ground->positions[0].y + r) - pos.y + 1e-6;
				}

				velocity.y = -vel_y * elascticity;
				float vel_tan = glm::length(glm::vec2(velocity.x, velocity.z));

				if (vel_tan > 1e-6) {
					glm::vec2 vel_tan_dir = glm::normalize(glm::vec2(velocity.x, velocity.z));
					float temp = abs((1 + elascticity) * vel_y);
					vel_tan = vel_tan - (friction * temp);

					if (vel_tan < 0)
						vel_tan = 0;

					velocity.x = vel_tan * vel_tan_dir.x;
					velocity.z = vel_tan * vel_tan_dir.y;
				}
			}
		}
	}
}

