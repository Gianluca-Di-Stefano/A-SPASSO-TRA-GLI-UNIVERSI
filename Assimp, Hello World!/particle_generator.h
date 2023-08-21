#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

class ParticleGenerator {
public:
    ParticleGenerator();
    ~ParticleGenerator();

    void Update(float deltaTime);
    void Render();

private:
    std::vector<Particle> particles;
    // Altre variabili di configurazione

    // Funzioni di utility, se necessario
};

#endif // PARTICLE_GENERATOR_H
