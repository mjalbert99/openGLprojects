#include <core.h>
#include <Particle.h>

class SpringDamper {
private:
	Particle* p1, * p2;
	float springConstant, damperConstant;
	float restLength;
	glm::vec3 d;
	float currLen;
	glm::vec3 velocityDifference;

public:
	SpringDamper(Particle* P1, Particle* P2, float sC, float dC, float rL) {
		p1 = P1;
		p2 = P2;
		springConstant = sC;
		damperConstant = dC;
		restLength = rL;
		d = glm::vec3(0.0f);
		currLen = 0.0f;
		velocityDifference = glm::vec3(0.0f);
	}

	glm::vec3 findSpringForce() {
		glm::vec3 springDirection = glm::normalize(d);
		glm::vec3 springForce = -springConstant * (restLength - currLen) * springDirection;
		if (currLen <= 1e-6) {
			springForce = glm::vec3(0.0f);
		}
		return springForce;
	}

	glm::vec3 findDamperForce() {
		float vel = glm::dot(velocityDifference, d);
		glm::vec3 damperDirection = glm::normalize(d);
		return -(damperConstant)*vel * damperDirection;
	}

	void computeForce() {
		d = p2->pos - p1->pos;
		currLen = glm::length(d);

		if (currLen > 1e-6) {
			d = glm::normalize(d);
			glm::vec3 p1Velocity = p1->velocity;
			glm::vec3 p2Velocity = p2->velocity;
			velocityDifference = p1Velocity - p2Velocity;
			glm::vec3 totalForce = findSpringForce() + findDamperForce();
			p1->applyForce(totalForce);
			p2->applyForce(-totalForce);
		}
	}
};
