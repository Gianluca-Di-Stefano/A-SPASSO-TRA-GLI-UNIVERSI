#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "render_text.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path, bool gammaCorrection);
void renderQuad();
void renderCube();

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

struct SphereCollision
{
    glm::vec3      centre;
    float          radius;
}spaceshipSphere, soleSphere, mercurioSphere, venereSphere, terraSphere, 
marteSphere, gioveSphere, saturnoSphere, uranoSphere, nettunoSphere, 
portalUniversoSphere, portalFuturamaSphere, portalInterstellarSphere;

bool collisionTest(SphereCollision& sfera1, const SphereCollision& sfera2) {
    glm::vec3 distanzaCentri(sfera2.centre - sfera1.centre);
    float dist(glm::dot(distanzaCentri, distanzaCentri));
    float rquad(sfera1.radius + sfera2.radius);
    rquad *= rquad;
    int distanza = (int)dist - int(rquad);
    if (dist <= rquad)
        return true;
    return false;
}

// settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraCollided = false;

int contatorePortali = 0;

enum class Universe {
    UNIVERSO,
    FUTURAMA,
    INTERSTELLAR
};

Universe currentUniverse = Universe::UNIVERSO;

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

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

void carica_interstellar(GLFWwindow* window) {
    // build and compile shaders
    Shader shaderGeometryPass("g_buffer.vs", "g_buffer.fs");
    Shader shaderLightingPass("deferred_shading.vs", "deferred_shading.fs");
    Shader shaderLightBox("deferred_light_box.vs", "deferred_light_box.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    // load models
    Model spaceShuttle("resources/objects/interstellar/spaceship/rocket.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    //COMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO(SOSTITUSCE IL SOLE)
    //Model sole("resources/objects/universo/planets/sole/sole.obj");
    //DECOMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO (SOSTITUSCE IL SOLE)
    Model sole("resources/objects/universo/planets/sole/sole.obj");
    Model terra("resources/objects/universo/planets/terra/terra.obj");
    Model giove("resources/objects/universo/planets/giove/giove.obj");
    Model luna("resources/objects/universo/planets/luna/luna.obj");
    Model marte("resources/objects/universo/planets/marte/marte.obj");
    Model mercurio("resources/objects/universo/planets/mercurio/mercurio.obj");
    Model nettuno("resources/objects/universo/planets/nettuno/nettuno.obj");
    Model saturno("resources/objects/universo/planets/saturno/saturno.obj");
    Model urano("resources/objects/universo/planets/urano/urano.obj");
    Model venere("resources/objects/universo/planets/venere/venere.obj");
    Model portalFuturama("resources/objects/portal/portal.obj");
    Model portalUniverso("resources/objects/portal/portal.obj");


    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int cubeTexture = loadTexture("resources/objects/universo/skybox/back.jpg");


    // configure g-buffer framebuffer
// ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 8;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(100);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, 100)); //prova posizione sole 
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        std::cout << "Contatore portale: " << contatorePortali << std::endl;


        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, camera.Position + 0.2f * camera.Front);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.001f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 5.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        // draw solar system
        glm::mat4 modelSole = glm::mat4(1.0f);
        modelSole = glm::scale(modelSole, glm::vec3(1.0f));
        soleSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 250.0f };
        shaderGeometryPass.setMat4("model", modelSole);
        sole.Draw(shaderGeometryPass);

        glm::mat4 modelMercurio = glm::mat4(1.0f);
        modelMercurio = glm::translate(modelMercurio, glm::vec3(300.0f, 0.0f, 0.0f));
        modelMercurio = glm::scale(modelMercurio, glm::vec3(3.4f / 1000.0f));
        mercurioSphere = { glm::vec3(300.0f, 0.0f, 0.0f), 5.0f };
        shaderGeometryPass.setMat4("model", modelMercurio);
        mercurio.Draw(shaderGeometryPass);

        glm::mat4 modelVenere = glm::mat4(1.0f);
        modelVenere = glm::translate(modelVenere, glm::vec3(0.0f, 0.0f, 400.0f));
        modelVenere = glm::scale(modelVenere, glm::vec3(8.6f / 1000.0f));
        venereSphere = { glm::vec3(0.0f, 0.0f, 400.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelVenere);
        venere.Draw(shaderGeometryPass);

        glm::mat4 modelTerra = glm::mat4(1.0f);
        modelTerra = glm::translate(modelTerra, glm::vec3(-500.0f, 0.0f, 0.0f));
        modelTerra = glm::scale(modelTerra, glm::vec3(9.1f / 1000));
        terraSphere = { glm::vec3(-500.0f, 0.0f, 0.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelTerra);
        terra.Draw(shaderGeometryPass);

        glm::mat4 modelLuna = glm::mat4(1.0f);
        modelLuna = glm::translate(modelLuna, glm::vec3(-510.0f, 0.0f, 0.0f));
        modelLuna = glm::scale(modelLuna, glm::vec3(2.0f / 1000));
        shaderGeometryPass.setMat4("model", modelLuna);
        luna.Draw(shaderGeometryPass);

        glm::mat4 modelMarte = glm::mat4(1.0f);
        modelMarte = glm::translate(modelMarte, glm::vec3(0.0f, 0.0f, -600.0f));
        modelMarte = glm::scale(modelMarte, glm::vec3(2.0f / 1000));
        marteSphere = { glm::vec3(0.0f, 0.0f, -600.0f), 7.6f };
        shaderGeometryPass.setMat4("model", modelMarte);
        marte.Draw(shaderGeometryPass);

        glm::mat4 modelGiove = glm::mat4(1.0f);
        modelGiove = glm::translate(modelGiove, glm::vec3(700.0f, 0.0f, 0.0f));
        modelGiove = glm::scale(modelGiove, glm::vec3(102.7f / 1000));
        gioveSphere = { glm::vec3(700.0f, 0.0f, 0.0f), 100.0f };
        shaderGeometryPass.setMat4("model", modelGiove);
        giove.Draw(shaderGeometryPass);

        glm::mat4 modelSaturno = glm::mat4(1.0f);
        modelSaturno = glm::translate(modelSaturno, glm::vec3(0.0f, 0.0f, 800.0f));
        modelSaturno = glm::scale(modelSaturno, glm::vec3(83.7f / 1000));
        saturnoSphere = { glm::vec3(0.0f, 0.0f, 800.0f), 83.6f };
        shaderGeometryPass.setMat4("model", modelSaturno);
        saturno.Draw(shaderGeometryPass);

        glm::mat4 modelUrano = glm::mat4(1.0f);
        modelUrano = glm::translate(modelUrano, glm::vec3(-900.0f, 0.0f, 0.0f));
        modelUrano = glm::scale(modelUrano, glm::vec3(33.7f / 1000));
        uranoSphere = { glm::vec3(-900.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelUrano);
        urano.Draw(shaderGeometryPass);

        glm::mat4 modelNettuno = glm::mat4(1.0f);
        modelNettuno = glm::translate(modelNettuno, glm::vec3(-950.0f, 0.0f, 0.0f));
        modelNettuno = glm::scale(modelNettuno, glm::vec3(32.7f / 1000));
        nettunoSphere = { glm::vec3(-950.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelNettuno);
        nettuno.Draw(shaderGeometryPass);

        //draw portal
        glm::mat4 modelPortalFuturama = glm::mat4(1.0f);
        modelPortalFuturama = glm::translate(modelPortalFuturama, glm::vec3(200.0f, 0.0f, 0.0f));
        modelPortalFuturama = glm::rotate(modelPortalFuturama, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalFuturama = glm::scale(modelPortalFuturama, glm::vec3(15.7f));
        portalInterstellarSphere = { glm::vec3(200.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelPortalFuturama);
        portalFuturama.Draw(shaderGeometryPass);

        glm::mat4 modelPortalUniverso = glm::mat4(1.0f);
        modelPortalUniverso = glm::translate(modelPortalUniverso, glm::vec3(-200.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::rotate(modelPortalUniverso, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::scale(modelPortalUniverso, glm::vec3(15.7f));
        portalUniversoSphere = { glm::vec3(-200.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelPortalUniverso);
        portalUniverso.Draw(shaderGeometryPass);

        //collisioni
        cameraCollided = false;

        bool collisioneSun = collisionTest(spaceshipSphere, soleSphere);
        if (collisioneSun == true) {
            cameraCollided = true;
            std::string Title = "Sole";
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneMercury = collisionTest(spaceshipSphere, mercurioSphere);
        if (collisioneMercury == true) {
            cameraCollided = true;
            std::string Title = "Mercurio";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneVenus = collisionTest(spaceshipSphere, venereSphere);
        if (collisioneVenus == true) {
            cameraCollided = true;
            std::string Title = "Venere";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneEarth = collisionTest(spaceshipSphere, terraSphere);
        if (collisioneEarth == true) {
            cameraCollided = true;
            std::string Title = "Terra e Luna";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneMars = collisionTest(spaceshipSphere, marteSphere);
        if (collisioneMars == true) {
            cameraCollided = true;
            std::string Title = "Marte";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneJupiter = collisionTest(spaceshipSphere, gioveSphere);
        if (collisioneJupiter == true) {
            cameraCollided = true;
            std::string Title = "Giove";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneSaturn = collisionTest(spaceshipSphere, saturnoSphere);
        if (collisioneSaturn == true) {
            cameraCollided = true;
            std::string Title = "Saturno";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneUranus = collisionTest(spaceshipSphere, uranoSphere);
        if (collisioneUranus == true) {
            cameraCollided = true;
            std::string Title = "Urano";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneNeptune = collisionTest(spaceshipSphere, nettunoSphere);
        if (collisioneNeptune == true) {
            cameraCollided = true;
            std::string Title = "Nettuno";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisionePortalFuturama = collisionTest(spaceshipSphere, portalFuturamaSphere);
        if (collisionePortalFuturama == true) {
            std::string Title = "TELETRANSPORT TO FUTURAMA";
            RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f, (float)SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            contatorePortali = 1;
        }

        bool collisionePortalInterstellar = collisionTest(spaceshipSphere, portalInterstellarSphere);
        if (collisionePortalInterstellar == true) {
            std::string Title = "TELETRANSPORT TO INTERSTELLAR";
            RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f, (float)SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            contatorePortali = 2;
        }

        // draw skybox cube
        skyboxShader.use();
        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::scale(modelCube, glm::vec3(2000.0f, 2000.0f, 2000.0f));
        glm::mat4 projectionCube = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000000.0f);
        skyboxShader.setMat4("model", modelCube);
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projectionCube);
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // Abilita il mipmapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Genera i mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);






        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7;
            const float quadratic = 1.8;
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        shaderLightingPass.setVec3("viewPos", camera.Position);
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. render lights on top of scene eliminare questa parte per togliere i cubi luminosi e lasciare solo la luce
        // --------------------------------
        /*
        shaderLightBox.use();
        shaderLightBox.setMat4("projection", projection);
        shaderLightBox.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(10.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            //renderCube();
            renderSphere();
        }
        */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

void carica_futurama(GLFWwindow* window) {
    // build and compile shaders
    Shader shaderGeometryPass("g_buffer.vs", "g_buffer.fs");
    Shader shaderLightingPass("deferred_shading_toon.vs", "deferred_shading_toon.fs");
    Shader shaderLightBox("deferred_light_box.vs", "deferred_light_box.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    // load models
    Model spaceShuttle("resources/objects/futurama/spaceship/rocket.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    //COMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO(SOSTITUSCE IL SOLE)
    //Model sole("resources/objects/universo/planets/sole/sole.obj");
    //DECOMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO (SOSTITUSCE IL SOLE)
    Model sole("resources/objects/universo/planets/sole/sole.obj");
    Model terra("resources/objects/futurama/planets/terra/terra.obj");
    Model luna("resources/objects/futurama/planets/luna/luna.obj");
    Model marte("resources/objects/futurama/planets/marte/marte.obj");
    Model bendergod("resources/objects/futurama/planets/bender_god/bender_god.obj");
    Model decapod("resources/objects/futurama/planets/decapod/decapod.obj");
    Model neardeath("resources/objects/futurama/planets/neardeath/neardeath.obj");
    Model omicron("resources/objects/futurama/planets/omicron/omicron.obj");
    Model simian("resources/objects/futurama/planets/simian/simian.obj");
    Model thunban("resources/objects/futurama/planets/thunban/thunban.obj");
    Model tornadus("resources/objects/futurama/planets/tornadus/tornadus.obj");
    Model wormulon("resources/objects/futurama/planets/wormulon/wormulon.obj");
    Model portalUniverso("resources/objects/portal/portal.obj");
    Model portalInterstellar("resources/objects/portal/portal.obj");

    // configure g-buffer framebuffer
// ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 8;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(100);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, 100)); //prova posizione sole 
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        std::cout << "Contatore portale: " << contatorePortali << std::endl;

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        if (currentUniverse == Universe::FUTURAMA) {
        }

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, camera.Position + 0.2f * camera.Front);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.001f));
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        // draw solar system

        glm::mat4 modelSole = glm::mat4(1.0f);
        modelSole = glm::scale(modelSole, glm::vec3(1.0f));
        shaderGeometryPass.setMat4("model", modelSole);
        sole.Draw(shaderGeometryPass);

        glm::mat4 modelBenderGod = glm::mat4(1.0f);
        modelBenderGod = glm::translate(modelBenderGod, glm::vec3(0.0f, 0.0f, 300.0f));
        modelBenderGod = glm::rotate(modelBenderGod, 180.0f, glm::vec3(0.0f, -1.0f, 0.0f));
        modelBenderGod = glm::scale(modelBenderGod, glm::vec3(1.0f / 10.0f));
        shaderGeometryPass.setMat4("model", modelBenderGod);
        bendergod.Draw(shaderGeometryPass);

        glm::mat4 modelDecapod = glm::mat4(1.0f);
        modelDecapod = glm::translate(modelDecapod, glm::vec3(0.0f, 0.0f, 400.0f));
        modelDecapod = glm::scale(modelDecapod, glm::vec3(8.6f / 1000.0f));
        shaderGeometryPass.setMat4("model", modelDecapod);
        decapod.Draw(shaderGeometryPass);

        glm::mat4 modelTerra = glm::mat4(1.0f);
        modelTerra = glm::translate(modelTerra, glm::vec3(-500.0f, 0.0f, 0.0f));
        modelTerra = glm::scale(modelTerra, glm::vec3(9.1f / 1000));
        shaderGeometryPass.setMat4("model", modelTerra);
        terra.Draw(shaderGeometryPass);

        glm::mat4 modelLuna = glm::mat4(1.0f);
        modelLuna = glm::translate(modelLuna, glm::vec3(-510.0f, 0.0f, 0.0f));
        modelLuna = glm::scale(modelLuna, glm::vec3(2.0f / 1000));
        shaderGeometryPass.setMat4("model", modelLuna);
        luna.Draw(shaderGeometryPass);

        glm::mat4 modelMarte = glm::mat4(1.0f);
        modelMarte = glm::translate(modelMarte, glm::vec3(0.0f, 0.0f, -600.0f));
        modelMarte = glm::scale(modelMarte, glm::vec3(2.0f / 1000));
        shaderGeometryPass.setMat4("model", modelMarte);
        marte.Draw(shaderGeometryPass);

        glm::mat4 modelWormulon = glm::mat4(1.0f);
        modelWormulon = glm::translate(modelWormulon, glm::vec3(0.0f, 0.0f, -400.0f));
        modelWormulon = glm::scale(modelWormulon, glm::vec3(10.0f / 1000));
        marteSphere = { glm::vec3(0.0f, 0.0f, -400.0f), 7.6f };
        shaderGeometryPass.setMat4("model", modelWormulon);
        wormulon.Draw(shaderGeometryPass);

        glm::mat4 modelNeardeath = glm::mat4(1.0f);
        modelNeardeath = glm::translate(modelNeardeath, glm::vec3(700.0f, 0.0f, 0.0f));
        modelNeardeath = glm::scale(modelNeardeath, glm::vec3(102.7f / 1000));
        shaderGeometryPass.setMat4("model", modelNeardeath);
        neardeath.Draw(shaderGeometryPass);

        glm::mat4 modelOmicron = glm::mat4(1.0f);
        modelOmicron = glm::translate(modelOmicron, glm::vec3(0.0f, 0.0f, 800.0f));
        modelOmicron = glm::scale(modelOmicron, glm::vec3(83.7f / 1000));
        shaderGeometryPass.setMat4("model", modelOmicron);
        omicron.Draw(shaderGeometryPass);

        glm::mat4 modelSimian = glm::mat4(1.0f);
        modelSimian = glm::translate(modelSimian, glm::vec3(-900.0f, 0.0f, 0.0f));
        modelSimian = glm::scale(modelSimian, glm::vec3(33.7f / 1000));
        shaderGeometryPass.setMat4("model", modelSimian);
        simian.Draw(shaderGeometryPass);

        glm::mat4 modelThunban = glm::mat4(1.0f);
        modelThunban = glm::translate(modelThunban, glm::vec3(-950.0f, 0.0f, 0.0f));
        modelThunban = glm::scale(modelThunban, glm::vec3(32.7f / 1000));
        shaderGeometryPass.setMat4("model", modelThunban);
        thunban.Draw(shaderGeometryPass);

        glm::mat4 modelTornadus = glm::mat4(1.0f);
        modelTornadus = glm::translate(modelTornadus, glm::vec3(-950.0f, 0.0f, 0.0f));
        modelTornadus = glm::scale(modelTornadus, glm::vec3(32.7f / 1000));
        shaderGeometryPass.setMat4("model", modelTornadus);
        tornadus.Draw(shaderGeometryPass);

        //draw portal
        glm::mat4 modelPortalUniverso = glm::mat4(1.0f);
        modelPortalUniverso = glm::translate(modelPortalUniverso, glm::vec3(200.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::rotate(modelPortalUniverso, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::scale(modelPortalUniverso, glm::vec3(15.7f));
        portalUniversoSphere = { glm::vec3(200.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelPortalUniverso);
        portalUniverso.Draw(shaderGeometryPass);

        glm::mat4 modelPortalInterstellar = glm::mat4(1.0f);
        modelPortalInterstellar = glm::translate(modelPortalInterstellar, glm::vec3(-200.0f, 0.0f, 0.0f));
        modelPortalInterstellar = glm::rotate(modelPortalInterstellar, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalInterstellar = glm::scale(modelPortalInterstellar, glm::vec3(15.7f));
        portalInterstellarSphere = { glm::vec3(-200.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelPortalInterstellar);
        portalInterstellar.Draw(shaderGeometryPass);










        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7;
            const float quadratic = 1.8;
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        shaderLightingPass.setVec3("viewPos", camera.Position);
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. render lights on top of scene eliminare questa parte per togliere i cubi luminosi e lasciare solo la luce
        // --------------------------------
        /*
        shaderLightBox.use();
        shaderLightBox.setMat4("projection", projection);
        shaderLightBox.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(10.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            //renderCube();
            renderSphere();
        }
        */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

void carica_universo(GLFWwindow* window) {
    // build and compile shaders
    Shader shaderGeometryPass("g_buffer.vs", "g_buffer.fs");
    Shader shaderLightingPass("deferred_shading.vs", "deferred_shading.fs");
    Shader shaderLightBox("deferred_light_box.vs", "deferred_light_box.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    
    // load models
    Model spaceShuttle("resources/objects/universo/spaceship/rocket.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    //COMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO(SOSTITUSCE IL SOLE)
    //Model sole("resources/objects/universo/planets/sole/sole.obj");
    //DECOMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO (SOSTITUSCE IL SOLE)
    Model sole("resources/objects/universo/planets/sole/sole.obj");
    Model terra("resources/objects/universo/planets/terra/terra.obj");
    Model giove("resources/objects/universo/planets/giove/giove.obj");
    Model luna("resources/objects/universo/planets/luna/luna.obj");
    Model marte("resources/objects/universo/planets/marte/marte.obj");
    Model mercurio("resources/objects/universo/planets/mercurio/mercurio.obj");
    Model nettuno("resources/objects/universo/planets/nettuno/nettuno.obj");
    Model saturno("resources/objects/universo/planets/saturno/saturno.obj");
    Model urano("resources/objects/universo/planets/urano/urano.obj");
    Model venere("resources/objects/universo/planets/venere/venere.obj");
    Model portalFuturama("resources/objects/portal/portal.obj");
    Model portalInterstellar("resources/objects/portal/portal.obj");


    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int cubeTexture = loadTexture("resources/objects/universo/skybox/back.jpg");


    // configure g-buffer framebuffer
// ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 8;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(100);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, -100, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, -100, 100)); //prova posizione sole 
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        std::cout << "Contatore portale: " << contatorePortali << std::endl;

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, camera.Position + 0.2f * camera.Front);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.001f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 5.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        // draw solar system
        glm::mat4 modelSole = glm::mat4(1.0f);
        modelSole = glm::scale(modelSole, glm::vec3(1.0f));
        soleSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 250.0f };
        shaderGeometryPass.setMat4("model", modelSole);
        sole.Draw(shaderGeometryPass);

        glm::mat4 modelMercurio = glm::mat4(1.0f);
        modelMercurio = glm::translate(modelMercurio, glm::vec3(300.0f, 0.0f, 0.0f));
        modelMercurio = glm::scale(modelMercurio, glm::vec3(3.4f/1000.0f));
        mercurioSphere = { glm::vec3(300.0f, 0.0f, 0.0f), 5.0f };
        shaderGeometryPass.setMat4("model", modelMercurio);
        mercurio.Draw(shaderGeometryPass);

        glm::mat4 modelVenere = glm::mat4(1.0f);
        modelVenere = glm::translate(modelVenere, glm::vec3(0.0f, 0.0f, 400.0f));
        modelVenere = glm::scale(modelVenere, glm::vec3(8.6f/1000.0f));
        venereSphere = { glm::vec3(0.0f, 0.0f, 400.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelVenere);
        venere.Draw(shaderGeometryPass);

        glm::mat4 modelTerra = glm::mat4(1.0f);
        modelTerra = glm::translate(modelTerra, glm::vec3(-500.0f, 0.0f, 0.0f));
        modelTerra = glm::scale(modelTerra, glm::vec3(9.1f/1000));
        terraSphere = { glm::vec3(-500.0f, 0.0f, 0.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelTerra);
        terra.Draw(shaderGeometryPass);

        glm::mat4 modelLuna = glm::mat4(1.0f);
        modelLuna = glm::translate(modelLuna, glm::vec3(-510.0f, 0.0f, 0.0f));
        modelLuna = glm::scale(modelLuna, glm::vec3(2.0f/1000));
        shaderGeometryPass.setMat4("model", modelLuna);
        luna.Draw(shaderGeometryPass);

        glm::mat4 modelMarte = glm::mat4(1.0f);
        modelMarte = glm::translate(modelMarte, glm::vec3(0.0f, 0.0f, -600.0f));
        modelMarte = glm::scale(modelMarte, glm::vec3(2.0f / 1000));
        marteSphere = { glm::vec3(0.0f, 0.0f, -600.0f), 7.6f };
        shaderGeometryPass.setMat4("model", modelMarte);
        marte.Draw(shaderGeometryPass);

        glm::mat4 modelGiove = glm::mat4(1.0f);
        modelGiove = glm::translate(modelGiove, glm::vec3(700.0f, 0.0f, 0.0f));
        modelGiove = glm::scale(modelGiove, glm::vec3(102.7f / 1000));
        gioveSphere = { glm::vec3(700.0f, 0.0f, 0.0f), 100.0f };
        shaderGeometryPass.setMat4("model", modelGiove);
        giove.Draw(shaderGeometryPass);

        glm::mat4 modelSaturno = glm::mat4(1.0f);
        modelSaturno = glm::translate(modelSaturno, glm::vec3(0.0f, 0.0f, 800.0f));
        modelSaturno = glm::scale(modelSaturno, glm::vec3(83.7f / 1000));
        saturnoSphere = { glm::vec3(0.0f, 0.0f, 800.0f), 83.6f };
        shaderGeometryPass.setMat4("model", modelSaturno);
        saturno.Draw(shaderGeometryPass);

        glm::mat4 modelUrano = glm::mat4(1.0f);
        modelUrano = glm::translate(modelUrano, glm::vec3(-900.0f, 0.0f, 0.0f));
        modelUrano = glm::scale(modelUrano, glm::vec3(33.7f / 1000));
        uranoSphere = { glm::vec3(-900.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelUrano);
        urano.Draw(shaderGeometryPass);

        glm::mat4 modelNettuno = glm::mat4(1.0f);
        modelNettuno = glm::translate(modelNettuno, glm::vec3(-950.0f, 0.0f, 0.0f));
        modelNettuno = glm::scale(modelNettuno, glm::vec3(32.7f / 1000));
        nettunoSphere = { glm::vec3(-950.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelNettuno);
        nettuno.Draw(shaderGeometryPass);

        //draw portal
        glm::mat4 modelPortalFuturama = glm::mat4(1.0f);
        modelPortalFuturama = glm::translate(modelPortalFuturama, glm::vec3(200.0f, 0.0f, 0.0f));
        modelPortalFuturama = glm::rotate(modelPortalFuturama, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalFuturama = glm::scale(modelPortalFuturama, glm::vec3(15.7f));
        portalFuturamaSphere = { glm::vec3(200.0f, 0.0f, 0.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelPortalFuturama);
        portalFuturama.Draw(shaderGeometryPass);

        glm::mat4 modelPortalInterstellar = glm::mat4(1.0f);
        modelPortalInterstellar = glm::translate(modelPortalInterstellar, glm::vec3(-200.0f, 0.0f, 0.0f));
        modelPortalInterstellar = glm::rotate(modelPortalInterstellar, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalInterstellar = glm::scale(modelPortalInterstellar, glm::vec3(15.7f));
        portalInterstellarSphere = { glm::vec3(-200.0f, 0.0f, 0.0f), 30.0f };
        shaderGeometryPass.setMat4("model", modelPortalInterstellar);
        portalInterstellar.Draw(shaderGeometryPass);

        //collisioni
        cameraCollided = false;

        bool collisioneSun = collisionTest(spaceshipSphere, soleSphere);
        if (collisioneSun == true) {
            cameraCollided = true;
            std::string Title = "Sole";
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneMercury = collisionTest(spaceshipSphere, mercurioSphere);
        if (collisioneMercury == true) {
            cameraCollided = true;
            std::string Title = "Mercurio";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneVenus = collisionTest(spaceshipSphere, venereSphere);
        if (collisioneVenus == true) {
            cameraCollided = true;
            std::string Title = "Venere";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneEarth = collisionTest(spaceshipSphere, terraSphere);
        if (collisioneEarth == true) {
            cameraCollided = true;
            std::string Title = "Terra e Luna";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneMars = collisionTest(spaceshipSphere, marteSphere);
        if (collisioneMars == true) {
            cameraCollided = true;
            std::string Title = "Marte";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneJupiter = collisionTest(spaceshipSphere, gioveSphere);
        if (collisioneJupiter == true) {
            cameraCollided = true;
            std::string Title = "Giove";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneSaturn = collisionTest(spaceshipSphere, saturnoSphere);
        if (collisioneSaturn == true) {
            cameraCollided = true;
            std::string Title = "Saturno";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneUranus = collisionTest(spaceshipSphere, uranoSphere);
        if (collisioneUranus == true) {
            cameraCollided = true;
            std::string Title = "Urano";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneNeptune = collisionTest(spaceshipSphere, nettunoSphere);
        if (collisioneNeptune == true) {
            cameraCollided = true;
            std::string Title = "Nettuno";
            //RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f - (float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 2.0f, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisionePortalFuturama = collisionTest(spaceshipSphere, portalFuturamaSphere);
        if (collisionePortalFuturama == true) {
            std::string Title = "TELETRANSPORT TO FUTURAMA";
            contatorePortali = 1;
            carica_futurama(window);
            RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f, (float)SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisionePortalInterstellar = collisionTest(spaceshipSphere, portalInterstellarSphere);
        if (collisionePortalInterstellar == true) {
            std::string Title = "TELETRANSPORT TO INTERSTELLAR";
            RenderText(Title.c_str(), (float)SCR_WIDTH / 2.0f, (float)SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            contatorePortali = 2;
        }

        // draw skybox cube
        skyboxShader.use();
        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::scale(modelCube, glm::vec3(2000.0f, 2000.0f, 2000.0f));
        glm::mat4 projectionCube = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000000.0f);
        skyboxShader.setMat4("model", modelCube);
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projectionCube);
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // Abilita il mipmapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Genera i mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);






        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7;
            const float quadratic = 1.8;
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        shaderLightingPass.setVec3("viewPos", camera.Position);
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. render lights on top of scene eliminare questa parte per togliere i cubi luminosi e lasciare solo la luce
        // --------------------------------
        /*
        shaderLightBox.use();
        shaderLightBox.setMat4("projection", projection);
        shaderLightBox.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(10.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            //renderCube();
            renderSphere();
        }
        */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}





int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Deferred Shading Volumes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    initRenderText(SCR_WIDTH, SCR_HEIGHT);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    


    switch (contatorePortali)
    {
    case 0:
    {
        carica_universo(window);

        break;
    }
    case 1:
    {
        carica_futurama(window);

        break;
    }
    case 2:
    {
        carica_interstellar(window);

        break;
    }
    }
    

    return 0;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    // Cambio manuale di universo premendo il tasto 1 o 2
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        contatorePortali = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        contatorePortali = 2;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}