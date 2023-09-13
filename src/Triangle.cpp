#include <core.h>
#include <Particle.h>

class Triangle {
public:
    Particle* p1, * p2, * p3;
    glm::vec3 velocity;
    glm::vec3 normal;
    float area;


    Triangle(Particle* P1, Particle* P2, Particle* P3) {
        p1 = P1;
        p2 = P2;
        p3 = P3;

        area = 0;
        normal = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        p1->normal = glm::vec3(0);
        p2->normal = glm::vec3(0);
        p3->normal = glm::vec3(0);
    }

    void computeNormal() {
        glm::vec3 norm = glm::cross(p2->pos - p1->pos, p3->pos - p1->pos);

        if (glm::length(norm) < 1e-6)
            return;

        norm = glm::normalize(norm);

        p1->normal += norm;
        p2->normal += norm;
        p3->normal += norm;
    }

    void computeAero(const glm::vec3& air_velocity) {

        velocity = (p1->velocity + p2->velocity + p3->velocity) / 3.0f - air_velocity;

        glm::vec3 temp_normal = glm::cross((p2->pos - p1->pos), (p3->pos - p1->pos));

        if (glm::length(temp_normal) == 0.f) {
            return;
        }

        normal = temp_normal / glm::length(temp_normal);

        float temp_area = 0.5 * glm::length(temp_normal);

        if (glm::length(velocity) == 0.f) {
            return;
        }

        area = temp_area * glm::dot((velocity / glm::length(velocity)), normal);

        glm::vec3 force = -0.5f * 1225.f * glm::length(velocity) * glm::length(velocity)
            * 1.28f * area * normal;

        p1->force += force / 3.0f;
        p2->force += force / 3.0f;
        p3->force += force / 3.0f;
    }
};
