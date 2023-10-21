#include "Particle.h"


void renderSphere()
{
    static unsigned int sphereVAO = 0;
    static unsigned int indexCount = 0;

    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;

        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void AppParticle::setup() {
    hue = 0;
    maxParticles = 2000;
    glEnable(GL_DEPTH_TEST);
    lightPosition = (glm::vec3(510.0f, 0.0f, 0.0f));
}

void AppParticle::update() {
    if (particles.size() > maxParticles) {
        particles.erase(particles.begin());
    }
    for (int i = 0; i < particles.size(); i++) {
        particles[i].update();
    }
}

void AppParticle::draw() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].draw();
    }
}

Particle::Particle(int startX, int startY, int startZ, int hue) {
    //costruttore
    float speed = 20.0;
    position = glm::vec3(startX, startY, startZ);
    force = glm::vec3(0.0f, 0.2f, 0.0f);
    direction = glm::vec3(0.0f, 1.0f, 0.0f); //imposto per ora una direzione lugno Y
    size = 20;
    color = glm::vec3(1.0f, 1.0f, 1.0f); //imposto per ora colore bianco
}

Particle::~Particle() {
    //distruttore
}

void Particle::update() {
    position += direction;
    direction += force;
    if (size > 1) {
        size -= 0.07;
    }
    float brightness = 1.0f;
    float myHue = 1.0f;

    if (brightness > 20.0f) {
        myHue -= 0.5;
    }
}

void Particle::draw() {
    renderSphere();
}