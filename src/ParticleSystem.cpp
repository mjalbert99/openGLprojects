#include "Sphere.cpp"
#include "simParticle.h"
#include <random>
#include <chrono>
#include <vector>

class ParticleSystem {
public:

	Ground* ground;
	std::vector<simParticle*> particles;
	std::vector<Sphere*> spheres;
	glm::vec3 wind = glm::vec3(0, 0, 0);
	float y = 3, x = 0, z = 0;
	float particle_per_sec = 5;
	float previous_time;
	float remaining_time;

	float elasticity = .75;
	float rad = .1;
	float particleMass = .1;
	float gravitY = 9.8;
	float numPartilces = 5;
	float lifeSpan = 3;
	float upperVar = 10;
	float lowerVar = -10;
	float upperLifeVar = 2;
	float lowerLifeVar = -2;
	float friction = 10;
	float airDensity = 1.225;
	float varX = 0;
	float varY = 0;
	float varZ = 0;
	float varLife = 0;
	float varVelocityX = 0;
	float varVelocityY = 0;
	float varVelocityZ = 0;
	float drag = .3;


public:
	ParticleSystem(Ground* gnd) {
		ground = gnd;

		previous_time = clock();
		remaining_time = 1.f / particle_per_sec;
	}

	void Update(float delta) {
		glm::vec3 gravity = glm::vec3(0, -gravitY, 0);

		clock_t curr_time = clock();
		float time_elapsed = (static_cast<float>(curr_time - previous_time) / CLOCKS_PER_SEC);
		previous_time = curr_time;
		remaining_time -= time_elapsed;

		if (remaining_time <= 0 && particle_per_sec > 0) {
			CreateNewParticle();
			remaining_time = 1.f / particle_per_sec;
		}

		for (int i = 0; i < particles.size(); i++) {
			particles[i]->integrate(delta, gravity, wind, airDensity, drag);
			if (particles[i]->kill) {
				particles.erase(particles.begin() + i);
				spheres.erase(spheres.begin() + i);
			}
			else {
				spheres[i]->updatePosition(particles[i]->pos);
			}
		}
	}

	void CreateNewParticle() {
		float tempX = getRandom(lowerVar, upperVar, varX);
		float tempY = getRandom(lowerVar, upperVar, varY);
		float tempZ = getRandom(lowerVar, upperVar, varZ);
		float velocityX = getRandom(lowerVar, upperVar, varVelocityX);
		float velocityY = getRandom(lowerVar, upperVar, varVelocityY);
		float velocityZ = getRandom(lowerVar, upperVar, varVelocityZ);
		float tempLife = getRandom(lowerLifeVar, upperLifeVar, varLife);
		simParticle* p = new simParticle(glm::vec3(x + tempX, y + tempY, z + tempZ), particleMass,
			ground, rad, elasticity, friction, lifeSpan+tempLife, glm::vec3(velocityX, velocityY, velocityZ));
		Sphere* s = new Sphere(p->pos, rad, 8);
		particles.push_back(p);
		spheres.push_back(s);

		if (particles.size() > numPartilces) {
			particles.resize(numPartilces);
			spheres.resize(numPartilces);
		}
	}


	float getRandom(float& lower, float& upper, float& var) {
		if (var == 0) {
			return 0;
		}

		else {
			unsigned s = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine rand(s);
			std::normal_distribution<float> distrubution((lower + upper) / 2, var);
			float rand_num = distrubution(rand);
			rand_num = std::max(lower, std::min(upper, rand_num));
			return rand_num;
		}
	}


	void Draw(const glm::mat4& view, GLuint shader) {
		for (auto& i : spheres) {
			i->Draw(view, shader);
		}
	}

};