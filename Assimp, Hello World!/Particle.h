
#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
class Particle {
public:
    // our new particle class
    // with each object containing an x and y position, and a vector direction as vx and vy and a size
    float size;
    glm::vec3 color, force, position, direction;

    // we also define two methods that the particle object understands
    void update();
    void draw();

    // and define a particle contructor and a destructor (the destructor is denoted by the tilde (~) character prefix
    // the contructor is expecting to be passed an initial x and y co-ordinate for the new particle
    Particle(int x, int y, int z, int hue);
    ~Particle();
};

class AppParticle {
public:
    void setup();
    void update();
    void draw();

    // define a vector containing our new particle class objects
    std::vector<Particle> particles;
};