#include "particle_generator.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
ParticleGenerator::ParticleGenerator() {
    // Inizializzazioni, se necessario
}

ParticleGenerator::~ParticleGenerator() {
    // Deallocazione delle risorse, se necessario
}

void ParticleGenerator::Update(float deltaTime) {
    // Itera attraverso le particelle e aggiorna le loro proprietà
    for (Particle& particle : particles) {
        if (particle.life > 0.0f) {
            // Aggiorna la posizione in base alla velocità e al tempo
            particle.position += particle.velocity * deltaTime;

            // Riduci il tempo di vita
            particle.life -= deltaTime;
        }
    }
}

void ParticleGenerator::Render() {
    // Itera attraverso le particelle e esegui il rendering di ciascuna
    for (const Particle& particle : particles) {
        if (particle.life > 0.0f) {
            // Esegui il rendering della particella utilizzando shader di particelle
            // Qui dovresti effettuare il rendering del particolare aspetto delle particelle,
            // ad esempio con quad, sprite o altri metodi.
            // Potresti utilizzare OpenGL o altre librerie di rendering come SFML o SDL.

            // Esempio: Disegna una particella come un punto
            glBegin(GL_POINTS);
            glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
            glVertex3f(particle.position.x, particle.position.y, particle.position.z);
            glEnd();
        }
    }
}