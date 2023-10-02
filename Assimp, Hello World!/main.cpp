#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrklang/irrKlang.h>
#include <thread>
#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "model_animation.h"
#include "animator.h"
#include "render_text.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>

#pragma comment(lib, "irrKlang.lib") 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
GLuint texture_id;
unsigned int loadTexture(const char* path, bool gammaCorrection);
void renderQuad();
void renderCube();
unsigned int loadCubemap(vector<std::string> faces);


//SOUNDTRACK

using namespace irrklang;
ISoundEngine* SoundEngine = createIrrKlangDevice();
ISoundSource* universoTheme = SoundEngine->addSoundSourceFromFile("resources/music/universo.mp3");
ISoundSource* futuramaTheme = SoundEngine->addSoundSourceFromFile("resources/music/futurama.mp3");
ISoundSource* interstellarTheme = SoundEngine->addSoundSourceFromFile("resources/music/interstellar.mp3");

// Funzione per effettuare il rendering del quadrilatero

void carica_universo(GLFWwindow* window);
void carica_futurama(GLFWwindow* window);
void carica_interstellar(GLFWwindow* window);
void carica_tesseract(GLFWwindow* window);

bool futurama_caricato = false;
bool interstellar_caricato = false;
int startGame = 0;
bool infoVisible = false;
bool mapVisible = false;
glm::vec3 initialPosition = glm::vec3(510.0f, 0.0f, 0.0f);//terra
float initialSpeed = 0.0;
float rotationAngle = 0.0f;
float rotationAngle1 = 0.0f;
float rotationSpeed = 1.0f;

//pianeti visitati variabili
int pianetiScopertiUniverso = 0;
int pianetiScopertiFuturama = 0;
int pianetiScopertiInterstellar = 0;
std::unordered_map<std::string, bool> pianetiVisitatiUniverso;
std::unordered_map<std::string, bool> pianetiVisitatiFuturama;
std::unordered_map<std::string, bool> pianetiVisitatiInterstellar;


//impatti
bool impattoEarth = false;
bool impattoSun = false;
bool impattoMoon = false;
bool impattoMercury = false;
bool impattoVenus = false;
bool impattoMars = false;
bool impattoJupiter = false;
bool impattoSaturn = false;
bool impattoUranus = false;
bool impattoNeptune = false;


bool impattoBenderGod = false;
bool impattoDecapod = false;
bool impattoWormulon = false;
bool impattoNearDeath = false;
bool impattoOmicron = false;
bool impattoSimian = false;
bool impattoThunban = false;
bool impattoTornadus = false;




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
}
spaceshipSphere, soleSphere, mercurioSphere, venereSphere, terraSphere, marteSphere, gioveSphere, saturnoSphere, uranoSphere, nettunoSphere,
terraImpatto, soleImpatto, mercurioImpatto, venereImpatto, marteImpatto, gioveImpatto, saturnoImpatto, uranoImpatto, nettunoImpatto, 

benderGodSphere, decapodSphere, lunaSphere, wormulonSphere, neardeathSphere, omicronSphere, simianSphere, thunbanSphere, tornadusSphere, 
benderGodImpatto, decapodImpatto, lunaImpatto, wormulonImpatto, nearDeathImpatto, omicronImpatto, simianImpatto, thunbanImpatto, tornadusImpatto,

gargantuaSphere, gargantuaInnerSphere, mannSphere,millerSphere, portalUniversoSphere, portalFuturamaSphere, portalInterstellarSphere, tesseractSphere;

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
int SCR_WIDTH = 1900;
int SCR_HEIGHT = 1200;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool cameraCollided = false;
bool movementBlocked = false;

// camera
Camera camera(deltaTime, glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

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

void carica_universo(GLFWwindow* window) {
    // build and compile shaders
    Shader shaderGeometryPass("g_buffer.vs", "g_buffer.fs");
    Shader shaderLightingPass("deferred_shading.vs", "deferred_shading.fs");
    Shader shaderLightBox("deferred_light_box.vs", "deferred_light_box.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader asteroidShader("asteroids.vs", "asteroids.fs");

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
    Model asteroids("resources/objects/universo/planets/asteroid/asteroid2.obj");
    Model mercurio("resources/objects/universo/planets/mercurio/mercurio.obj");
    Model nettuno("resources/objects/universo/planets/nettuno/nettuno.obj");
    Model saturno("resources/objects/universo/planets/saturno/saturno.obj");
    Model urano("resources/objects/universo/planets/urano/urano.obj");
    Model venere("resources/objects/universo/planets/venere/venere.obj");
    Model portalFuturama("resources/objects/futurama/box/box.obj");
    Model portalInterstellar("resources/objects/interstellar/planets/wormhole/wormhole.obj");
    Model skybox("resources/objects/universo/skybox/skybox.obj");
    Model info("resources/objects/schermate/info.obj");
    Model iniz("resources/objects/schermate/iniz.obj");
    Model tutorial("resources/objects/schermate/tutorial.obj");
    Model display("resources/objects/schermate/display.obj");
    Model mappa("resources/objects/schermate/mappa.obj");
    Model fine("resources/objects/schermate/fine.obj");

    SoundEngine->stopAllSounds();
    ISound* ambientSound = SoundEngine->play2D(universoTheme, true);

    // generate a large list of semi-random model transformation matrices
  // ------------------------------------------------------------------
    unsigned int amount = 10000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float innerRadius = 510.0f; // Inner radius of the disk
    float outerRadius = 690.0f; // Outer radius of the disk
    float diskHeight = 0.0f; // Height of the disk (z-coordinate)
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // 1. translation: displace in a circular path within the disk
        float angle = ((float)rand() / RAND_MAX) * 360.0f; // Random angle in degrees
        float radius = innerRadius + ((float)rand() / RAND_MAX) * (outerRadius - innerRadius); // Random radius within the disk width
        float x = radius * cos(glm::radians(angle));
        float y = diskHeight + ((float)rand() / RAND_MAX)* 200.0f - 200.0f; // Randomize z-coordinate slightly
        float z = radius * sin(glm::radians(angle));
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = ((rand() % 20) / 100.0f + 0.05);
        model = glm::scale(model, glm::vec3(scale * 2));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));


        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
  // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    

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
    const unsigned int NR_LIGHTS = 64;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    float radius = 120.0;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        float phi = glm::acos(-1.0 + (2.0 * float(i)) / float(NR_LIGHTS - 1)); // Inclinazione sull'asse z
        float theta = glm::sqrt(float(NR_LIGHTS) * glm::pi<float>()) * phi; // Angolo attorno all'asse y

        float xPos = radius * glm::sin(phi) * glm::cos(theta);
        float yPos = radius * glm::sin(phi) * glm::sin(theta);
        float zPos = radius * glm::cos(phi);
        float time = glfwGetTime();
        glm::vec3 emission = glm::vec3(1.0, 1.0, 1.0); // Esempio di variazione nel canale verde
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        // Imposta i colori delle luci come preferisci
        lightColors.push_back(emission);
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);

    // render loop
    // -----------

    camera.Position = initialPosition;
    camera.MovementSpeed = initialSpeed;

    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        rotationAngle += rotationSpeed * deltaTime;
        rotationAngle1 += deltaTime;
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000000000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Calcola la direzione in cui la telecamera dovrebbe guardare
        glm::vec3 cameraFront = glm::normalize(camera.Front);
        glm::vec3 cameraUp = glm::normalize(camera.Up);

        // Definisci un vettore di offset dalla posizione della telecamera
        float distanceBehind = 0.16f; // Sposta la telecamera dietro la navicella
        float distanceAbove = -0.03f;   // Sposta la telecamera sopra la navicella
        glm::vec3 cameraOffset = distanceBehind * cameraFront + distanceAbove * cameraUp;

        // Calcola la nuova posizione del modello
        glm::vec3 newModelPosition = camera.Position + cameraOffset;

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, newModelPosition);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.0004f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 1.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        //draw schermata iniziale
        if (startGame == 0) {
            glm::mat4 modelIniz = glm::mat4(1.0f);
            modelIniz = glm::translate(modelIniz, camera.Position + 0.13f * camera.Front);
            modelIniz = glm::rotate(modelIniz, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelIniz = glm::rotate(modelIniz, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelIniz = glm::scale(modelIniz, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelIniz);
            iniz.Draw(shaderGeometryPass);
        }
        else if (startGame > 0 && startGame < 2) {
            glm::mat4 modelTutorial = glm::mat4(1.0f);
            modelTutorial = glm::translate(modelTutorial, camera.Position + 0.13f * camera.Front);
            modelTutorial = glm::rotate(modelTutorial, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelTutorial = glm::rotate(modelTutorial, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelTutorial = glm::scale(modelTutorial, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelTutorial);
            tutorial.Draw(shaderGeometryPass);
        }

        // Definisci i raggi delle orbite per i pianeti
        float radiusMercurio = 350.0f;
        float radiusVenere = 400.0f;
        float radiusTerra = 500.0f;
        float radiusMarte = 550.0f;
        float radiusGiove = 900.0f;
        float radiusSaturno = 1000.0f;
        float radiusUrano = 1050.0f;
        float radiusNettuno = 1080.0f;

        // Aggiorna la posizione degli oggetti in base all'angolo di orbita
        glm::vec3 positionMercurio = glm::vec3(cos(glm::radians(+rotationAngle)*0.1f) * radiusMercurio,
            0.0f,
            sin(glm::radians( + rotationAngle) * 0.1f) * radiusMercurio);

        glm::vec3 positionVenere = glm::vec3(cos(glm::radians(200 + rotationAngle)*0.08f) * radiusVenere,
            0.0f,
            sin(glm::radians(200 + rotationAngle) * 0.08f) * radiusVenere);

        glm::vec3 positionTerra = glm::vec3(cos(glm::radians(400 + rotationAngle)*0.07f) * radiusTerra,
            0.0f,
            sin(glm::radians(400 + rotationAngle) * 0.07f) * radiusTerra);

        glm::vec3 positionMarte = glm::vec3(cos(glm::radians(500+ rotationAngle)*0.06f) * radiusMarte,
            0.0f,
            sin(glm::radians(500 + rotationAngle) * 0.06f) * radiusMarte);

        glm::vec3 positionGiove = glm::vec3(cos(glm::radians(600+ rotationAngle)*0.05f) * radiusGiove,
            0.0f,
            sin(glm::radians(600+ rotationAngle) * 0.05f) * radiusGiove);

        glm::vec3 positionSaturno = glm::vec3(cos(glm::radians(700+ rotationAngle)*0.04f) * radiusSaturno,
            0.0f,
            sin(glm::radians(700+ rotationAngle) * 0.04f) * radiusSaturno);

        glm::vec3 positionUrano = glm::vec3(cos(glm::radians(800+ rotationAngle)*0.02) * radiusUrano,
            0.0f,
            sin(glm::radians(800+ rotationAngle) * 0.02) * radiusUrano);

        glm::vec3 positionNettuno = glm::vec3(cos(glm::radians(900+rotationAngle)*0.01f) * radiusNettuno,
            0.0f,
            sin(glm::radians(900+rotationAngle) * 0.01f) * radiusNettuno);

        glm::vec3 cameraOffsetDisplay = distanceBehind * cameraFront + distanceAbove * cameraUp;
        //draw display
        // Calcola la posizione del display in base alla posizione della telecamera
        glm::vec3 displayPosition = camera.Position + cameraOffsetDisplay - 0.12f * camera.Right;
        displayPosition.y = camera.Position.y + cameraOffsetDisplay.y;
        glm::mat4 modelDisplay = glm::mat4(1.0f);
        modelDisplay = glm::translate(modelDisplay, displayPosition);
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelDisplay = glm::scale(modelDisplay, glm::vec3(0.065f));
        shaderGeometryPass.setMat4("model", modelDisplay);
        display.Draw(shaderGeometryPass);

        if (mapVisible) {
            movementBlocked = true;
            glm::mat4 modelMap = glm::mat4(1.0f);
            modelMap = glm::translate(modelMap, camera.Position + 0.13f * camera.Front);
            modelMap = glm::rotate(modelMap, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelMap = glm::rotate(modelMap, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelMap = glm::scale(modelMap, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelMap);
            mappa.Draw(shaderGeometryPass);
        }

        // draw solar system
        glm::mat4 modelSole = glm::mat4(1.0f);
        modelSole = glm::scale(modelSole, glm::vec3(1.0f));
        modelSole = glm::rotate(modelSole, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        soleSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 250.0f };
        soleImpatto = { glm::vec3(0.0f, 0.0f, 0.0f), 150.0f };
        shaderGeometryPass.setMat4("model", modelSole);
        sole.Draw(shaderGeometryPass);

        glm::mat4 modelMercurio = glm::mat4(1.0f);
        modelMercurio = glm::translate(modelMercurio, positionMercurio);
        modelMercurio = glm::rotate(modelMercurio, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMercurio = glm::scale(modelMercurio, glm::vec3(3.4f / 1000.0f));
        mercurioSphere = { positionMercurio, 5.0f };
        mercurioImpatto = { positionMercurio, 0.1f };
        shaderGeometryPass.setMat4("model", modelMercurio);
        mercurio.Draw(shaderGeometryPass);

        glm::mat4 modelVenere = glm::mat4(1.0f);
        modelVenere = glm::translate(modelVenere, positionVenere);
        modelVenere = glm::rotate(modelVenere, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelVenere = glm::scale(modelVenere, glm::vec3(8.6f / 1000.0f));
        venereSphere = { positionVenere, 10.0f };
        venereImpatto = { positionVenere, 0.1f };
        shaderGeometryPass.setMat4("model", modelVenere);
        venere.Draw(shaderGeometryPass);

        // Definisci la distanza tra la Terra e la Luna
        float distanceFromEarth = 10.0f;

        // Calcola la posizione della Luna rispetto alla Terra
        glm::vec3 positionLuna = positionTerra + glm::vec3(cos(glm::radians(rotationAngle1)) * distanceFromEarth,
            0.0f,
            sin(glm::radians(rotationAngle)) * distanceFromEarth);

        glm::mat4 modelTerra = glm::mat4(1.0f);
        modelTerra = glm::translate(modelTerra, positionTerra);
        modelTerra = glm::rotate(modelTerra, glm::radians(rotationAngle1 * 10), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTerra = glm::scale(modelTerra, glm::vec3(9.1f / 1000));
        terraSphere = { positionTerra, 3.0f };
        terraImpatto = { positionTerra, 0.1f };

        shaderGeometryPass.setMat4("model", modelTerra);
        terra.Draw(shaderGeometryPass);

        glm::mat4 modelLuna = glm::mat4(1.0f);
        modelLuna = glm::translate(modelLuna, positionLuna);
        modelLuna = glm::rotate(modelLuna, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLuna = glm::scale(modelLuna, glm::vec3(2.0f / 1000));
        lunaSphere = { positionLuna, 1.0f };
        lunaImpatto = { positionLuna, 0.001f };
        shaderGeometryPass.setMat4("model", modelLuna);
        luna.Draw(shaderGeometryPass);

        glm::mat4 modelMarte = glm::mat4(1.0f);
        modelMarte = glm::translate(modelMarte, positionMarte);
        modelMarte = glm::rotate(modelMarte, glm::radians(rotationAngle1 *10), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMarte = glm::scale(modelMarte, glm::vec3(2.0f / 1000));
        marteSphere = { positionMarte, 7.6f };
        marteImpatto = { positionMarte, 0.8f };

        shaderGeometryPass.setMat4("model", modelMarte);
        marte.Draw(shaderGeometryPass);

        glm::mat4 modelAsteroids = glm::mat4(1.0f);
        //modelAsteroids = glm::rotate(modelAsteroids, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelAsteroids = glm::scale(modelAsteroids, glm::vec3(1.0f));
        modelAsteroids = glm::rotate(modelAsteroids, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        shaderGeometryPass.setMat4("model", modelAsteroids);
        asteroids.Draw(shaderGeometryPass);

        glm::mat4 modelGiove = glm::mat4(1.0f);
        modelGiove = glm::translate(modelGiove, positionGiove);
        modelGiove = glm::rotate(modelGiove, glm::radians(rotationAngle1 * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        modelGiove = glm::scale(modelGiove, glm::vec3(102.7f / 1000));
        gioveSphere = { positionGiove, 70.0f };
        gioveImpatto = { positionGiove, 10.0f };
        shaderGeometryPass.setMat4("model", modelGiove);
        giove.Draw(shaderGeometryPass);

        glm::mat4 modelSaturno = glm::mat4(1.0f);
        modelSaturno = glm::translate(modelSaturno, positionSaturno);
        modelSaturno = glm::rotate(modelSaturno, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelSaturno = glm::rotate(modelSaturno, glm::radians(rotationAngle1 * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        modelSaturno = glm::scale(modelSaturno, glm::vec3(83.7f / 1000));
        saturnoSphere = { positionSaturno, 83.6f };
        saturnoImpatto = { positionSaturno, 8.36f };

        shaderGeometryPass.setMat4("model", modelSaturno);
        saturno.Draw(shaderGeometryPass);

        glm::mat4 modelUrano = glm::mat4(1.0f);
        modelUrano = glm::translate(modelUrano, positionUrano);
        modelUrano = glm::rotate(modelUrano, glm::radians(rotationAngle1 * 15), glm::vec3(0.0f, 1.0f, 0.0f));
        modelUrano = glm::scale(modelUrano, glm::vec3(33.7f / 1000));
        uranoSphere = { positionUrano, 30.0f };
        uranoImpatto = { positionUrano, 3.0f };

        shaderGeometryPass.setMat4("model", modelUrano);
        urano.Draw(shaderGeometryPass);

        glm::mat4 modelNettuno = glm::mat4(1.0f);
        modelNettuno = glm::translate(modelNettuno, positionNettuno); 
        modelNettuno = glm::rotate(modelNettuno, glm::radians(rotationAngle1 * 15), glm::vec3(0.0f, 1.0f, 0.0f));
        modelNettuno = glm::scale(modelNettuno, glm::vec3(32.7f / 1000));
        nettunoSphere = { positionNettuno, 30.0f };
        nettunoImpatto = { positionNettuno, 3.0f };

        shaderGeometryPass.setMat4("model", modelNettuno);
        nettuno.Draw(shaderGeometryPass);

        //draw portal
        glm::mat4 modelPortalFuturama = glm::mat4(1.0f);
        modelPortalFuturama = glm::translate(modelPortalFuturama, glm::vec3(200.0f, 0.0f, 0.0f));
        //modelPortalFuturama = glm::rotate(modelPortalFuturama, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalFuturama = glm::scale(modelPortalFuturama, glm::vec3(2.0f));
        portalFuturamaSphere = { glm::vec3(200.0f, 0.0f, 0.0f), 1.0f };
        shaderGeometryPass.setMat4("model", modelPortalFuturama);
        portalFuturama.Draw(shaderGeometryPass);
   
        glm::mat4 modelPortalInterstellar = glm::mat4(1.0f);
        modelPortalInterstellar = glm::translate(modelPortalInterstellar, glm::vec3(-300.0f, 0.0f, 0.0f));
        //modelPortalInterstellar = glm::rotate(modelPortalInterstellar, glm::radians(rotationAngle) *20000, glm::vec3(0.0f, 0.0f, 1.0f));
        //modelPortalInterstellar = glm::rotate(modelPortalInterstellar, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        modelPortalInterstellar = glm::scale(modelPortalInterstellar, glm::vec3(15.7f));
        portalInterstellarSphere = { glm::vec3(-300.0f, 0.0f, 0.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelPortalInterstellar);
        portalInterstellar.Draw(shaderGeometryPass);

        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::scale(modelSkybox, glm::vec3(5000.2f));
        shaderGeometryPass.setMat4("model", modelSkybox);
        skybox.Draw(shaderGeometryPass);

        glm::mat4 modelInfo = glm::mat4(1.0f);
        modelInfo = glm::translate(modelInfo, camera.Position + 0.13f * camera.Front);
        //modelInfo = glm::rotate(modelInfo, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));

        //collisioni
        cameraCollided = false;

        bool collisioneSun = collisionTest(spaceshipSphere, soleSphere);
        impattoSun = collisionTest(spaceshipSphere, soleImpatto);

        if (collisioneSun == true) {
            cameraCollided = true;
            std::string Title = "Sole";
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        bool collisioneMercury = collisionTest(spaceshipSphere, mercurioSphere);
        impattoMercury = collisionTest(spaceshipSphere, mercurioImpatto);
        std::string nomePianeta = "Mercurio";
        if (collisioneMercury == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/mercurio.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneVenus = collisionTest(spaceshipSphere, venereSphere);
       impattoVenus = collisionTest(spaceshipSphere, venereImpatto);

        nomePianeta = "Venere";
        if (collisioneVenus == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/venere.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneEarth = collisionTest(spaceshipSphere, terraSphere);
        impattoEarth = collisionTest(spaceshipSphere, terraImpatto);

        nomePianeta = "Terra";
        if (collisioneEarth == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/terra.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }


        bool collisioneLuna = collisionTest(spaceshipSphere, lunaSphere);
        impattoMoon = collisionTest(spaceshipSphere, lunaImpatto);

        nomePianeta = "Luna";
        if (collisioneLuna == true && infoVisible == true) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/luna.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneMars = collisionTest(spaceshipSphere, marteSphere);
        impattoMars = collisionTest(spaceshipSphere, marteImpatto);

        nomePianeta = "Marte";
        if (collisioneMars == true && infoVisible == true) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/marte.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneJupiter = collisionTest(spaceshipSphere, gioveSphere);
        impattoJupiter = collisionTest(spaceshipSphere, gioveImpatto);

        nomePianeta = "Giove";
        if (collisioneJupiter == true && infoVisible == true) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/giove.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneSaturn = collisionTest(spaceshipSphere, saturnoSphere);
        impattoSaturn = collisionTest(spaceshipSphere, saturnoImpatto);

        nomePianeta = "Saturno";
        if (collisioneSaturn == true && infoVisible == true) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/saturno.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }
        
        bool collisioneUranus = collisionTest(spaceshipSphere, uranoSphere);
        impattoUranus = collisionTest(spaceshipSphere, uranoImpatto);

        nomePianeta = "Urano";
        if (collisioneUranus == true && infoVisible == true) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/urano.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisioneNeptune = collisionTest(spaceshipSphere, nettunoSphere);
        impattoNeptune = collisionTest(spaceshipSphere, nettunoImpatto);

        nomePianeta = "Nettuno";
        if (collisioneNeptune == true && infoVisible == true ) {
            movementBlocked = true;
            cameraCollided = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/universo/info/nettuno.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiUniverso[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiUniverso++;
                // Imposta il pianeta come visitato
                pianetiVisitatiUniverso[nomePianeta] = true;
            }
        }

        bool collisionePortalFuturama = collisionTest(spaceshipSphere, portalFuturamaSphere);
        if (collisionePortalFuturama == true) {
            carica_futurama(window);
            contatorePortali = 1;
        }

        bool collisionePortalInterstellar = collisionTest(spaceshipSphere, portalInterstellarSphere);
        if (collisionePortalInterstellar == true) {
            carica_interstellar(window);
            contatorePortali = 2;
        }



        // testo su schermo

        if (infoVisible == true) {
            std::string Info = "info: on";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (infoVisible == false) {
            std::string Info = "info: off";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Pianeti = "pianeti scoperti':" + std::to_string(pianetiScopertiUniverso) + "/9";
        if (pianetiScopertiUniverso < 9)
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        
        if (pianetiScopertiUniverso == 9) {
            Pianeti = "sistema solare esplorato";
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Velocity = "velocita':" + std::to_string((int)camera.MovementSpeed * 1000) + " km/h";
        RenderText(Velocity.c_str(), 15.0f, (float)SCR_HEIGHT / 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        //fine gioco
        if (camera.Position[0] > 10000.0f || camera.Position[0] < -10000.0f || camera.Position[1] > 10000.0f || camera.Position[1] < -10000.0f || camera.Position[2] > 10000.0f || camera.Position[2] < -10000.0f) {
            camera.MovementSpeed = initialSpeed;
            camera.Position = initialPosition;
            carica_tesseract(window);
        }

        // draw skybox cube
        /* codice skybox iniziale
        skyboxShader.use();
        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::scale(modelCube, glm::vec3(20000.0f, 20000.0f, 20000.0f));
        glm::mat4 projectionCube = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20000.0f);
        skyboxShader.setMat4("model", modelCube);
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projectionCube);
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // Abilita il mipmapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        */



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
            model = glm::scale(model, glm::vec3(1.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            renderCube();
            //renderSphere();
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

    // load models
    Model spaceShuttle("resources/objects/futurama/spaceship/rocket.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    Model sole("resources/objects/futurama/planets/sole/sole.obj");
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
    Model portalUniverso("resources/objects/futurama/box/boxUniverso.obj");
    Model portalInterstellar("resources/objects/futurama/box/boxInterstellar.obj");
    Model info("resources/objects/schermate/info.obj");
    Model skybox("resources/objects/futurama/skybox/skybox.obj");
    Model display("resources/objects/schermate/display.obj");
    Model mappa("resources/objects/schermate/mappa.obj");

     SoundEngine->stopAllSounds();
     ISound* ambientSound = SoundEngine->play2D(futuramaTheme, true);
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
    const unsigned int NR_LIGHTS = 64;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    float radius = 120.0;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        float phi = glm::acos(-1.0 + (2.0 * float(i)) / float(NR_LIGHTS - 1)); // Inclinazione sull'asse z
        float theta = glm::sqrt(float(NR_LIGHTS) * glm::pi<float>()) * phi; // Angolo attorno all'asse y

        float xPos = radius * glm::sin(phi) * glm::cos(theta);
        float yPos = radius * glm::sin(phi) * glm::sin(theta);
        float zPos = radius * glm::cos(phi);
        float time = glfwGetTime();
        glm::vec3 emission = glm::vec3(1.0, 1.0, 1.0); // Esempio di variazione nel canale verde
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        // Imposta i colori delle luci come preferisci
        lightColors.push_back(emission);
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);
    
    camera.Position = initialPosition;
    camera.MovementSpeed = initialSpeed;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        int pianetiScoperti = 0;

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        rotationAngle += rotationSpeed * deltaTime;
        rotationAngle1 += deltaTime;
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000000000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Definisci un vettore di offset dalla posizione della telecamera
        float distanceBehind = 0.16f; // Sposta la telecamera dietro la navicella
        float distanceAbove = -0.03f;   // Sposta la telecamera sopra la navicella
        glm::vec3 cameraOffset = distanceBehind * camera.Front + distanceAbove * camera.Up;

        // Calcola la nuova posizione del modello
        glm::vec3 newModelPosition = camera.Position + cameraOffset;

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, newModelPosition);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.0005f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 5.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        glm::vec3 cameraOffsetDisplay = distanceBehind * camera.Front + distanceAbove * camera.Up;
        //draw display
        // Calcola la posizione del display in base alla posizione della telecamera
        glm::vec3 displayPosition = camera.Position + cameraOffsetDisplay - 0.12f * camera.Right;
        displayPosition.y = camera.Position.y + cameraOffsetDisplay.y;
        glm::mat4 modelDisplay = glm::mat4(1.0f);
        modelDisplay = glm::translate(modelDisplay, displayPosition);
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelDisplay = glm::scale(modelDisplay, glm::vec3(0.065f));
        shaderGeometryPass.setMat4("model", modelDisplay);
        display.Draw(shaderGeometryPass);

        if (mapVisible) {
            movementBlocked = true;
            glm::mat4 modelMap = glm::mat4(1.0f);
            modelMap = glm::translate(modelMap, camera.Position + 0.13f * camera.Front);
            modelMap = glm::rotate(modelMap, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelMap = glm::rotate(modelMap, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelMap = glm::scale(modelMap, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelMap);
            mappa.Draw(shaderGeometryPass);
        }

        // Definisci i raggi delle orbite per i pianeti
        float radiusBenderGod = 350.0f;
        float radiusDecapod = 400.0f;
        float radiusTerra = 500.0f;
        float radiusMarte = 550.0f;
        float radiusWormulon = 900.0f;
        float radiusNeardeath = 1000.0f;
        float radiusOmicron = 1050.0f;
        float radiusSimian = 1080.0f;
        float radiusThunban = 1120.0f;
        float radiusTornadus = 1170.0f;

        // Aggiorna la posizione degli oggetti in base all'angolo di orbita
        glm::vec3 positionBenderGod = glm::vec3(cos(glm::radians(+rotationAngle) * 0.1f) * radiusBenderGod,
            0.0f,
            sin(glm::radians(+rotationAngle) * 0.1f) * radiusBenderGod);

        glm::vec3 positionDecapod = glm::vec3(cos(glm::radians(200 + rotationAngle) * 0.08f) * radiusDecapod,
            0.0f,
            sin(glm::radians(200 + rotationAngle) * 0.08f) * radiusDecapod);

        glm::vec3 positionTerra = glm::vec3(cos(glm::radians(400 + rotationAngle) * 0.07f) * radiusTerra,
            0.0f,
            sin(glm::radians(400 + rotationAngle) * 0.07f) * radiusTerra);

        glm::vec3 positionMarte = glm::vec3(cos(glm::radians(500 + rotationAngle) * 0.06f) * radiusMarte,
            0.0f,
            sin(glm::radians(500 + rotationAngle) * 0.06f) * radiusMarte);

        glm::vec3 positionWormulon = glm::vec3(cos(glm::radians(600 + rotationAngle) * 0.05f) * radiusWormulon,
            0.0f,
            sin(glm::radians(600 + rotationAngle) * 0.05f) * radiusWormulon);

        glm::vec3 positionNeardeath = glm::vec3(cos(glm::radians(700 + rotationAngle) * 0.04f) * radiusNeardeath,
            0.0f,
            sin(glm::radians(700 + rotationAngle) * 0.04f) * radiusNeardeath);

        glm::vec3 positionOmicron = glm::vec3(cos(glm::radians(800 + rotationAngle) * 0.02) * radiusOmicron,
            0.0f,
            sin(glm::radians(800 + rotationAngle) * 0.02) * radiusOmicron);

        glm::vec3 positionSimian = glm::vec3(cos(glm::radians(900 + rotationAngle) * 0.01f) * radiusSimian,
            0.0f,
            sin(glm::radians(900 + rotationAngle) * 0.01f) * radiusSimian);

        glm::vec3 positionThunban = glm::vec3(cos(glm::radians(900 + rotationAngle) * 0.01f) * radiusThunban,
            0.0f,
            sin(glm::radians(900 + rotationAngle) * 0.01f) * radiusThunban);

        glm::vec3 positionTornadus = glm::vec3(cos(glm::radians(900 + rotationAngle) * 0.01f) * radiusTornadus,
            0.0f,
            sin(glm::radians(900 + rotationAngle) * 0.01f) * radiusTornadus);

        // draw solar system
        glm::mat4 modelSole = glm::mat4(1.0f);
        modelSole = glm::scale(modelSole, glm::vec3(1.0f));
        
        shaderGeometryPass.setMat4("model", modelSole);
        sole.Draw(shaderGeometryPass);

        glm::mat4 modelBenderGod = glm::mat4(1.0f);
        modelBenderGod = glm::translate(modelBenderGod, positionBenderGod);
        modelBenderGod = glm::rotate(modelBenderGod, 180.0f, glm::vec3(0.0f, -1.0f, 0.0f));
        modelBenderGod = glm::rotate(modelBenderGod, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelBenderGod = glm::scale(modelBenderGod, glm::vec3(1.0f / 10.0f));
        benderGodSphere = { positionBenderGod, 5.0f };
        benderGodImpatto = { positionBenderGod, 50.0f };
        shaderGeometryPass.setMat4("model", modelBenderGod);
        bendergod.Draw(shaderGeometryPass);

        glm::mat4 modelDecapod = glm::mat4(1.0f);
        modelDecapod = glm::translate(modelDecapod, positionDecapod);
        modelDecapod = glm::rotate(modelDecapod, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelDecapod = glm::scale(modelDecapod, glm::vec3(8.6f / 1000.0f));
        decapodSphere = { positionDecapod, 10.0f };
        decapodImpatto = { positionDecapod, 50.0f };
        shaderGeometryPass.setMat4("model", modelDecapod);
        decapod.Draw(shaderGeometryPass);

        // Definisci la distanza tra la Terra e la Luna
        float distanceFromEarth = 8.0f;


        // Calcola la posizione della Luna rispetto alla Terra
        glm::vec3 positionLuna = positionTerra + glm::vec3(cos(glm::radians(rotationAngle1)) * distanceFromEarth,
            0.0f,
            sin(glm::radians(rotationAngle)) * distanceFromEarth);

        glm::mat4 modelTerra = glm::mat4(1.0f);
        modelTerra = glm::translate(modelTerra, positionTerra);
        modelTerra = glm::rotate(modelTerra, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTerra = glm::scale(modelTerra, glm::vec3(9.1f / 1000));
        terraSphere = {positionTerra, 3.0f };
        terraImpatto = { positionTerra, 1.0f };
        shaderGeometryPass.setMat4("model", modelTerra);
        terra.Draw(shaderGeometryPass);

        glm::mat4 modelLuna = glm::mat4(1.0f);
        modelLuna = glm::translate(modelLuna, positionLuna);
        modelLuna = glm::rotate(modelLuna, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLuna = glm::scale(modelLuna, glm::vec3(2.0f / 1000));
        lunaSphere = { positionLuna, 1.0f};
        lunaImpatto = { positionLuna, 0.5f };
        shaderGeometryPass.setMat4("model", modelLuna);
        luna.Draw(shaderGeometryPass);

        glm::mat4 modelMarte = glm::mat4(1.0f);
        modelMarte = glm::translate(modelMarte, positionMarte);
        modelMarte = glm::rotate(modelMarte, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMarte = glm::scale(modelMarte, glm::vec3(2.0f / 1000));
        marteSphere = { positionMarte, 5.0f};
        marteImpatto = { positionMarte, 5.0f };
        shaderGeometryPass.setMat4("model", modelMarte);
        marte.Draw(shaderGeometryPass);

        glm::mat4 modelWormulon = glm::mat4(1.0f);
        modelWormulon = glm::translate(modelWormulon, positionWormulon);
        modelWormulon = glm::rotate(modelWormulon, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelWormulon = glm::rotate(modelWormulon, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelWormulon = glm::scale(modelWormulon, glm::vec3(100.0f / 1000));
        wormulonSphere = { positionWormulon, 100.0f };
        wormulonImpatto = { positionWormulon, 100.0f };
        shaderGeometryPass.setMat4("model", modelWormulon);
        wormulon.Draw(shaderGeometryPass);

        glm::mat4 modelNeardeath = glm::mat4(1.0f);
        modelNeardeath = glm::translate(modelNeardeath,positionNeardeath);
        modelNeardeath = glm::rotate(modelNeardeath, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelNeardeath = glm::scale(modelNeardeath, glm::vec3(102.7f / 1000));
        neardeathSphere = { positionNeardeath, 100.0f };
        nearDeathImpatto = { positionNeardeath, 100.0f };
        shaderGeometryPass.setMat4("model", modelNeardeath);
        neardeath.Draw(shaderGeometryPass);

        glm::mat4 modelOmicron = glm::mat4(1.0f);
        modelOmicron = glm::translate(modelOmicron, positionOmicron);
        modelOmicron = glm::rotate(modelOmicron, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelOmicron = glm::scale(modelOmicron, glm::vec3(83.7f / 1000));
        omicronSphere = { positionOmicron, 83.6f };
        omicronImpatto = { positionOmicron, 83.0f };
        shaderGeometryPass.setMat4("model", modelOmicron);
        omicron.Draw(shaderGeometryPass);

        glm::mat4 modelSimian = glm::mat4(1.0f);
        modelSimian = glm::translate(modelSimian, positionSimian);
        modelSimian = glm::rotate(modelSimian, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelSimian = glm::scale(modelSimian, glm::vec3(33.7f / 1000));
        simianSphere = { positionSimian, 15.6f };
        simianImpatto = { positionSimian, 15.0f };
        shaderGeometryPass.setMat4("model", modelSimian);
        simian.Draw(shaderGeometryPass);

        glm::mat4 modelThunban = glm::mat4(1.0f);
        modelThunban = glm::translate(modelThunban, positionThunban);
        modelThunban = glm::rotate(modelThunban, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelThunban = glm::scale(modelThunban, glm::vec3(32.7f / 1000));
        thunbanSphere = { positionThunban, 15.6f };
        thunbanImpatto = { positionThunban, 15.0f };
        shaderGeometryPass.setMat4("model", modelThunban);
        thunban.Draw(shaderGeometryPass);

        glm::mat4 modelTornadus = glm::mat4(1.0f);
        modelTornadus = glm::translate(modelTornadus, positionTornadus);
        modelTornadus = glm::rotate(modelTornadus, glm::radians(rotationAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTornadus = glm::scale(modelTornadus, glm::vec3(32.7f / 1000));
        tornadusSphere = { positionTornadus, 30.6f };
        tornadusImpatto = { positionTornadus, 30.0f };
        shaderGeometryPass.setMat4("model", modelTornadus);
        tornadus.Draw(shaderGeometryPass);

        //draw portal
        glm::mat4 modelPortalUniverso = glm::mat4(1.0f);
        modelPortalUniverso = glm::translate(modelPortalUniverso, glm::vec3(300.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::scale(modelPortalUniverso, glm::vec3(2.0f));
        portalUniversoSphere = { glm::vec3(300.0f, 0.0f, 0.0f), 1.0f };
        shaderGeometryPass.setMat4("model", modelPortalUniverso);
        portalUniverso.Draw(shaderGeometryPass);

        glm::mat4 modelPortalInterstellar = glm::mat4(1.0f);
        modelPortalInterstellar = glm::translate(modelPortalInterstellar, glm::vec3(-300.0f, 0.0f, 0.0f));
        modelPortalInterstellar = glm::scale(modelPortalInterstellar, glm::vec3(2.0f));
        portalInterstellarSphere = { glm::vec3(-300.0f, 0.0f, 0.0f), 1.0f };
        shaderGeometryPass.setMat4("model", modelPortalInterstellar);
        portalInterstellar.Draw(shaderGeometryPass);

        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::scale(modelSkybox, glm::vec3(10000.2f));
        shaderGeometryPass.setMat4("model", modelSkybox);
        skybox.Draw(shaderGeometryPass);

        //draw info
        glm::mat4 modelInfo = glm::mat4(1.0f);
        modelInfo = glm::translate(modelInfo, camera.Position + 0.13f * camera.Front);
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        //info.Draw(shaderGeometryPass);



                //fine gioco
        if (camera.Position[0] > 20000.0f || camera.Position[0] < -20000.0f || camera.Position[1] > 20000.0f || camera.Position[1] < -20000.0f || camera.Position[2] > 20000.0f || camera.Position[2] < -20000.0f) {
            camera.MovementSpeed = 0.0;
            camera.Position[0] = 30.0f;
            camera.Position[1] = 30.0f;
            camera.Position[2] = 30.0f;
            carica_tesseract(window);
        }

        //collisioni
        cameraCollided = false;

        bool collisioneSun = collisionTest(spaceshipSphere, soleSphere);
        impattoSun = collisionTest(spaceshipSphere, soleImpatto);
        if (collisioneSun == true && infoVisible == true) {
            cameraCollided = true;
            std::string Title = "Sole";
            RenderText(Title.c_str(), 900.0f, (float)SCR_HEIGHT / 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string nomePianeta = "BenderGod";
        bool collisioneBenderGod = collisionTest(spaceshipSphere, benderGodSphere);
        impattoBenderGod = collisionTest(spaceshipSphere, benderGodImpatto);

        if (collisioneBenderGod == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/bender_god.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Decapod";
        bool collisioneDecapod = collisionTest(spaceshipSphere, decapodSphere);
        impattoDecapod = collisionTest(spaceshipSphere, decapodImpatto);

        if (collisioneDecapod == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/decapod.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Terra";
        bool collisioneEarth = collisionTest(spaceshipSphere, terraSphere);
        impattoEarth = collisionTest(spaceshipSphere, terraImpatto);

        if (collisioneEarth == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            std::string Title = "Terra";
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/terra.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Luna";
        bool collisioneLuna = collisionTest(spaceshipSphere, lunaSphere);
        impattoMoon = collisionTest(spaceshipSphere, lunaImpatto);

        if (collisioneLuna == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/luna.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Marte";
        bool collisioneMars = collisionTest(spaceshipSphere, marteSphere);
        impattoMars = collisionTest(spaceshipSphere, marteImpatto);

        if (collisioneMars == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/marte.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Wormulon";
        bool collisioneWormulon = collisionTest(spaceshipSphere, wormulonSphere);
        impattoWormulon = collisionTest(spaceshipSphere, wormulonImpatto);

        if (collisioneWormulon == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/wormulon.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Neardeath";
        bool collisioneNeardeath = collisionTest(spaceshipSphere, neardeathSphere);
        impattoNearDeath = collisionTest(spaceshipSphere, nearDeathImpatto);

        if (collisioneNeardeath == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/neardeath.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Omicron";
        bool collisioneOmicron = collisionTest(spaceshipSphere, omicronSphere);
        impattoOmicron = collisionTest(spaceshipSphere, omicronImpatto);

        if (collisioneOmicron == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/omicron.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Simian";
        bool collisioneSimian = collisionTest(spaceshipSphere, simianSphere);
        impattoSimian = collisionTest(spaceshipSphere, simianImpatto);

        if (collisioneSimian == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/simian.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Thunban";
        bool collisioneThunban = collisionTest(spaceshipSphere, thunbanSphere);
        impattoThunban = collisionTest(spaceshipSphere, thunbanImpatto);

        if (collisioneThunban == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/thunban.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        nomePianeta = "Tornadus";
        bool collisioneTornadus = collisionTest(spaceshipSphere, tornadusSphere);
        impattoTornadus = collisionTest(spaceshipSphere, tornadusImpatto);

        if (collisioneTornadus == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/futurama/info/tornadus.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiFuturama[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiFuturama++;
                // Imposta il pianeta come visitato
                pianetiVisitatiFuturama[nomePianeta] = true;
            }
        }

        bool collisionePortalUniverso = collisionTest(spaceshipSphere, portalUniversoSphere);
        if (collisionePortalUniverso == true) {
            carica_universo(window);
            contatorePortali = 0;
        }

        bool collisionePortalInterstellar = collisionTest(spaceshipSphere, portalInterstellarSphere);
        if (collisionePortalInterstellar == true) {
            carica_interstellar(window);
            contatorePortali = 2;
        }


        // testo su schermo

        if (infoVisible == true) {
            std::string Info = "info: on";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (infoVisible == false) {
            std::string Info = "info: off";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Pianeti = "pianeti scoperti':" + std::to_string(pianetiScopertiFuturama) + "/11";
        if (pianetiScopertiFuturama < 11)
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));


        if (pianetiScopertiFuturama == 11) {
            Pianeti = "universo Futurama esplorato";
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Velocity = "velocita':" + std::to_string((int)camera.MovementSpeed * 1000) + " km/h";
        RenderText(Velocity.c_str(), 15.0f, (float)SCR_HEIGHT / 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

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
    Model gargantua("resources/objects/interstellar/planets/gargantua/gargantua2.obj");
    Model miller("resources/objects/interstellar/planets/miller/miller.obj");
    Model mann("resources/objects/interstellar/planets/mann/mann.obj");
    Model saturno("resources/objects/universo/planets/saturno/saturno.obj");
    Model portalFuturama("resources/objects/portal/portal.obj");
    Model portalUniverso("resources/objects/portal/portal.obj");
    Model skybox("resources/objects/interstellar/skybox/skybox.obj");
    Model info("resources/objects/schermate/info.obj");
    Model display("resources/objects/schermate/display.obj");
    Model mappa("resources/objects/schermate/mappa.obj");
    SoundEngine->stopAllSounds();
    ISound* ambientSound = SoundEngine->play2D(interstellarTheme, true);
   

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
    const unsigned int NR_LIGHTS = 64;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    float radius = 120.0;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        float phi = glm::acos(-1.0 + (2.0 * float(i)) / float(NR_LIGHTS - 1)); // Inclinazione sull'asse z
        float theta = glm::sqrt(float(NR_LIGHTS) * glm::pi<float>()) * phi; // Angolo attorno all'asse y

        float xPos = radius * glm::sin(phi) * glm::cos(theta);
        float yPos = radius * glm::sin(phi) * glm::sin(theta);
        float zPos = radius * glm::cos(phi);
        float time = glfwGetTime();
        glm::vec3 emission = glm::vec3(1.0, 1.0, 1.0); // Esempio di variazione nel canale verde
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        // Imposta i colori delle luci come preferisci
        lightColors.push_back(emission);
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);



    camera.Position = initialPosition;
    camera.MovementSpeed = initialSpeed;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        rotationAngle += rotationSpeed * deltaTime;
        rotationAngle1 += deltaTime;
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Definisci i raggi delle orbite per gli altri oggetti
        float radiusMann = 450.0f;
        float radiusMiller = 600.0f;
        float radiusSaturno = 800.0f;

        // Aggiorna la posizione degli oggetti in base all'angolo di orbita
        glm::vec3 positionMann = glm::vec3(cos(glm::radians(rotationAngle)) * radiusMann,
            40.0f,
            sin(glm::radians(rotationAngle)) * radiusMann);
        glm::vec3 positionMiller = glm::vec3(cos(glm::radians(rotationAngle)) * radiusMiller,
            35.0f,
            sin(glm::radians(rotationAngle)) * radiusMiller);
        glm::vec3 positionSaturno = glm::vec3(cos(glm::radians(rotationAngle)) * radiusSaturno,
            0.0f,
            sin(glm::radians(rotationAngle)) * radiusSaturno);

        // Definisci un vettore di offset dalla posizione della telecamera
        float distanceBehind = 0.2f; // Sposta la telecamera dietro la navicella
        float distanceAbove = -0.03f;   // Sposta la telecamera sopra la navicella
        glm::vec3 cameraOffset = distanceBehind * camera.Front + distanceAbove * camera.Up;

        // Calcola la nuova posizione del modello
        glm::vec3 newModelPosition = camera.Position + cameraOffset;

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, newModelPosition);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.0005f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 5.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        glm::vec3 cameraOffsetDisplay = distanceBehind * camera.Front + distanceAbove * camera.Up;
        //draw display
        // Calcola la posizione del display in base alla posizione della telecamera
        glm::vec3 displayPosition = camera.Position + cameraOffsetDisplay - 0.12f * camera.Right;
        displayPosition.y = camera.Position.y + cameraOffsetDisplay.y;
        glm::mat4 modelDisplay = glm::mat4(1.0f);
        modelDisplay = glm::translate(modelDisplay, displayPosition);
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelDisplay = glm::rotate(modelDisplay, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelDisplay = glm::scale(modelDisplay, glm::vec3(0.065f));
        shaderGeometryPass.setMat4("model", modelDisplay);
        display.Draw(shaderGeometryPass);

        if (mapVisible) {
            movementBlocked = true;
            glm::mat4 modelMap = glm::mat4(1.0f);
            modelMap = glm::translate(modelMap, camera.Position + 0.13f * camera.Front);
            modelMap = glm::rotate(modelMap, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelMap = glm::rotate(modelMap, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelMap = glm::scale(modelMap, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelMap);
            mappa.Draw(shaderGeometryPass);
        }

        // draw solar system
        glm::mat4 modelGargantua = glm::mat4(1.0f);
        modelGargantua = glm::scale(modelGargantua, glm::vec3(400.0f));
        modelGargantua = glm::rotate(modelGargantua, glm::radians(rotationAngle1*60), glm::vec3(0.0f, 1.0f, 0.0f));
        gargantuaSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 400.0f };
        gargantuaInnerSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 200.0f };
        shaderGeometryPass.setMat4("model", modelGargantua);
        gargantua.Draw(shaderGeometryPass);

        glm::mat4 modelMann = glm::mat4(1.0f);
        modelMann = glm::translate(modelMann, positionMann);
        modelMann = glm::rotate(modelMann, glm::radians(rotationAngle1 * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMann = glm::scale(modelMann, glm::vec3(9.4f / 100.0f));
        mannSphere = {positionMann, 30.0f };
        shaderGeometryPass.setMat4("model", modelMann);
        mann.Draw(shaderGeometryPass);

        glm::mat4 modelMiller = glm::mat4(1.0f);
        modelMiller = glm::translate(modelMiller, positionMiller);
        modelMiller = glm::rotate(modelMiller, glm::radians(rotationAngle1 * 20), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMiller = glm::scale(modelMiller, glm::vec3(8.6f / 100.0f));
        millerSphere = {positionMiller, 30.0f };
        shaderGeometryPass.setMat4("model", modelMiller);
        miller.Draw(shaderGeometryPass);

        glm::mat4 modelSaturno = glm::mat4(1.0f);
        modelSaturno = glm::translate(modelSaturno, positionSaturno);
        modelSaturno = glm::rotate(modelSaturno, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelSaturno = glm::rotate(modelSaturno, glm::radians(rotationAngle1 * 10), glm::vec3(0.0f, 1.0f, 0.0f));
        modelSaturno = glm::scale(modelSaturno, glm::vec3(83.7f / 1000));
        saturnoSphere = {positionSaturno, 20.6f };
        shaderGeometryPass.setMat4("model", modelSaturno);
        saturno.Draw(shaderGeometryPass);


        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::scale(modelSkybox, glm::vec3(5000.2f));
        shaderGeometryPass.setMat4("model", modelSkybox);
        skybox.Draw(shaderGeometryPass);

        //draw info
        glm::mat4 modelInfo = glm::mat4(1.0f);
        modelInfo = glm::translate(modelInfo, camera.Position + 0.13f * camera.Front);
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelInfo = glm::rotate(modelInfo, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));

        //collisioni
        cameraCollided = false;

        std::string nomePianeta = "Gargantua";
        bool collisioneGargantua = collisionTest(spaceshipSphere, gargantuaSphere);
        if (collisioneGargantua == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/interstellar/info/gargantua.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiInterstellar[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiInterstellar++;
                // Imposta il pianeta come visitato
                pianetiVisitatiInterstellar[nomePianeta] = true;
            }
        }

        bool collisioneInnerGargantua = collisionTest(spaceshipSphere, gargantuaInnerSphere);
        if (collisioneInnerGargantua) {
            carica_tesseract(window);
        }

        nomePianeta = "Mann";
        bool collisioneMann = collisionTest(spaceshipSphere, mannSphere);
        if (collisioneMann == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/interstellar/info/mann.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiInterstellar[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiInterstellar++;
                // Imposta il pianeta come visitato
                pianetiVisitatiInterstellar[nomePianeta] = true;
            }
        }

        nomePianeta = "Miller";
        bool collisioneMiller = collisionTest(spaceshipSphere, millerSphere);
        if (collisioneMiller == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/interstellar/info/miller.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiInterstellar[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiInterstellar++;
                // Imposta il pianeta come visitato
                pianetiVisitatiInterstellar[nomePianeta] = true;
            }
        }

        nomePianeta = "Saturno";
        bool collisioneSaturn = collisionTest(spaceshipSphere, saturnoSphere);
        if (collisioneSaturn == true && infoVisible == true) {
            cameraCollided = true;
            movementBlocked = true;
            modelInfo = glm::scale(modelInfo, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", modelInfo);
            glActiveTexture(GL_TEXTURE0);
            unsigned int image = loadTexture("resources/objects/interstellar/info/saturno.png");
            glBindTexture(GL_TEXTURE_2D, image);
            info.Draw(shaderGeometryPass);
            if (!pianetiVisitatiInterstellar[nomePianeta]) {
                // Incrementa il numero di pianeti scoperti
                pianetiScopertiInterstellar++;
                // Imposta il pianeta come visitato
                pianetiVisitatiInterstellar[nomePianeta] = true;
            }
        }

        //fine gioco
        if (camera.Position[0] > 10000.0f || camera.Position[0] < -10000.0f || camera.Position[1] > 10000.0f || camera.Position[1] < -10000.0f || camera.Position[2] > 10000.0f || camera.Position[2] < -10000.0f) {
            camera.MovementSpeed = initialSpeed;
            camera.Position = initialPosition;
            carica_tesseract(window);
        }

        // testo su schermo

        if (infoVisible == true) {
            std::string Info = "info: on";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (infoVisible == false) {
            std::string Info = "info: off";
            RenderText(Info.c_str(), 15.0f, (float)SCR_HEIGHT / 6.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Pianeti = "pianeti scoperti':" + std::to_string(pianetiScopertiInterstellar) + "/4";
        if (pianetiScopertiInterstellar < 4)
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));


        if (pianetiScopertiInterstellar == 4) {
            Pianeti = "universo INTERSTELLAR esplorato";
            RenderText(Pianeti.c_str(), 15.0f, (float)SCR_HEIGHT / 8.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        std::string Velocity = "velocita':" + std::to_string((int)camera.MovementSpeed * 1000) + " km/h";
        RenderText(Velocity.c_str(), 15.0f, (float)SCR_HEIGHT / 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

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

void carica_tesseract(GLFWwindow* window) {
    // build and compile shaders
    Shader shaderGeometryPass("g_buffer.vs", "g_buffer.fs");
    Shader shaderLightingPass("deferred_shading.vs", "deferred_shading.fs");
    Shader shaderLightBox("deferred_light_box.vs", "deferred_light_box.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader animShader("anim_model.vs", "anim_model.fs");

    // load models
    Model spaceShuttle("resources/objects/interstellar/astronaut/astronaut.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    //COMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO(SOSTITUSCE IL SOLE)
    //Model sole("resources/objects/universo/planets/sole/sole.obj");
    //DECOMMENTARE PER FARE PROVE SU UN OGGETTO APPENA CREATO (SOSTITUSCE IL SOLE)
    Model tesseract("resources/objects/interstellar/planets/tesseract/scene.gltf");
    Model display("resources/objects/schermate/display.obj");
    Model portalFuturama("resources/objects/portal/portal.obj");
    Model portalUniverso("resources/objects/portal/portal.obj");
    Model portalInterstellar("resources/objects/portal/portal.obj");
    Model fine("resources/objects/schermate/fine.obj");
    Model skybox("resources/objects/interstellar/planets/tesseract/skybox/skybox.obj");
    SoundEngine->stopAllSounds();
    ISound* ambientSound = SoundEngine->play2D(interstellarTheme, true);

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
        lightPositions.push_back(glm::vec3(100, 0, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 0, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, 0, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 0, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(0, 0, -100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(0, 0, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(100, 0, 0)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(-100, 0, 0)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(0, 0, 100)); //prova posizione sole 
        lightPositions.push_back(glm::vec3(0, 0, -100)); //prova posizione sole 
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
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
        lightColors.push_back(glm::vec3(1.0, 1.0, 1.0));
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
        std::string Velocity = "speed:" + std::to_string((int)camera.MovementSpeed * 1000) + " km/h";
        RenderText(Velocity.c_str(), 15.0f, (float)SCR_HEIGHT / 10.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));


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

        animShader.setMat4("projection", projection);
        animShader.setMat4("view", view);

        // Definisci un vettore di offset dalla posizione della telecamera
        float distanceBehind = 0.2f; // Sposta la telecamera dietro la navicella
        float distanceAbove = -0.03f;   // Sposta la telecamera sopra la navicella
        glm::vec3 cameraOffset = distanceBehind * camera.Front + distanceAbove * camera.Up;

        // Calcola la nuova posizione del modello
        glm::vec3 newModelPosition = camera.Position + cameraOffset;

        //draw space shuttle
        glm::mat4 modelSpaceShuttle = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        modelSpaceShuttle = glm::mat4(1.0f);
        modelSpaceShuttle = glm::translate(modelSpaceShuttle, newModelPosition);
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
        modelSpaceShuttle = glm::rotate(modelSpaceShuttle, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        modelSpaceShuttle = glm::scale(modelSpaceShuttle, glm::vec3(0.05f));
        spaceshipSphere = { camera.Position + 2.0f * camera.Front, 5.0f };
        shaderGeometryPass.setMat4("model", modelSpaceShuttle);
        spaceShuttle.Draw(shaderGeometryPass);

        // draw solar system
        glm::mat4 modelTessract = glm::mat4(1.0f);
        modelTessract = glm::scale(modelTessract, glm::vec3(5.0f));
        modelTessract = glm::rotate(modelTessract, 200.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        tesseractSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 100.0f };
        shaderGeometryPass.setMat4("model", modelTessract);
        tesseract.Draw(shaderGeometryPass);

        //draw portal

        glm::mat4 modelPortalUniverso = glm::mat4(1.0f);
        modelPortalUniverso = glm::translate(modelPortalUniverso, glm::vec3(0.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::rotate(modelPortalUniverso, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        modelPortalUniverso = glm::scale(modelPortalUniverso, glm::vec3(15.7f));
        portalUniversoSphere = { glm::vec3(0.0f, 0.0f, 0.0f), 10.0f };
        shaderGeometryPass.setMat4("model", modelPortalUniverso);
        portalUniverso.Draw(shaderGeometryPass);

        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::scale(modelSkybox, glm::vec3(5000.2f));
        shaderGeometryPass.setMat4("model", modelSkybox);
        skybox.Draw(shaderGeometryPass);

        //collisioni
        cameraCollided = false;

        bool collisionePortalUniverso = collisionTest(spaceshipSphere, portalUniversoSphere);
        if (collisionePortalUniverso == true) {
            carica_universo(window);
        }

        //fine gioco
        if (camera.Position[0] > 5000.0f || camera.Position[0] < -5000.0f || camera.Position[1] > 5000.0f || camera.Position[1] < -5000.0f || camera.Position[2] > 5000.0f || camera.Position[2] < -5000.0f) {
            camera.MovementSpeed = 0.0f;
            glm::mat4 modelFine = glm::mat4(1.0f);
            modelFine = glm::translate(modelFine, camera.Position + 0.13f * camera.Front);
            modelFine = glm::rotate(modelFine, glm::radians(camera.Pitch), camera.Right); // Applica la rotazione rispetto all'asse Right della telecamera
            modelFine = glm::rotate(modelFine, glm::radians(camera.Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
            modelFine = glm::scale(modelFine, glm::vec3(0.06f));
            shaderGeometryPass.setMat4("model", modelFine);
            fine.Draw(shaderGeometryPass);
        }

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


bool impatto() {
    if (impattoEarth) {
        return true;
    }
    if (impattoSun) {
        return true;
    }
    if (impattoMercury) {
        return true;
    }
    if (impattoVenus) {
        return true;
    }
    if (impattoMoon) {
        return true;
    }
    if (impattoMars) {
        return true;
    }
    if (impattoJupiter) {
        return true;
    }
    if (impattoSaturn) {
        return true;
    }
    if (impattoUranus) {
        return true;
    }
    if (impattoNeptune) {
        return true;
    }
    if (impattoBenderGod) {
        return true;
    }
    if (impattoDecapod) {
        return true;
    }
    if (impattoWormulon) {
        return true;
    }
    if (impattoNearDeath) {
        return true;
    }
    if (impattoOmicron) {
        return true;
    }
    if (impattoSimian){
        return true;
    }
    if (impattoThunban) {
        return true;
    }
    if (impattoTornadus) {
        return true;
    }


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

    // settings
// Ottenere il monitor primario
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

    // Ottenere la modalità video corrente del monitor primario
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

    SCR_WIDTH = videoMode->width;
    SCR_HEIGHT = videoMode->height;

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A SPASSO TRA GLI UNIVERSI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Impostare la finestra in modalità schermo intero
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

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

bool mouseLeftPressed = false;
bool mouseLeftReleased = false;
bool keyMPressed = false;
bool keyMReleased = false;
bool enterPressed = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!enterPressed) {
            startGame++;
            enterPressed = true;
        }
    }
    else {
        enterPressed = false;
    }

    if (!movementBlocked) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.ProcessKeyboard(FORWARD, deltaTime * 0.2);
            if (impatto()) {
                camera.ProcessKeyboard(BACKWARD, deltaTime * 0.2);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.ProcessKeyboard(BACKWARD, deltaTime * 0.2);
            if (impatto()) {
                camera.ProcessKeyboard(FORWARD, deltaTime * 0.2);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.ProcessKeyboard(RIGHT, deltaTime * 0.2);
            if (impatto()) {
                camera.ProcessKeyboard(LEFT, deltaTime * 0.2);
            }

        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.ProcessKeyboard(LEFT, deltaTime * 0.2);
            if (impatto()) {
                camera.ProcessKeyboard(RIGHT, deltaTime * 0.2);
            }

        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.MovementSpeed += 0.1f; // incrementa la velocità della camera
            
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            camera.MovementSpeed -= 0.1f;

        if (camera.MovementSpeed <= 0.0f) {
            camera.MovementSpeed = 0.0f;
        }

        if (camera.MovementSpeed >= 15.0f) {
            camera.MovementSpeed = 15.0f;
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        mouseLeftReleased = true;
        movementBlocked = false;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (mouseLeftReleased) {
            mouseLeftPressed = true;
            mouseLeftReleased = false;
        }
    }

    if (mouseLeftPressed) {
        if (infoVisible) {
            infoVisible = false;
            rotationSpeed = 1.0f;
        }
        else {
            infoVisible = true;
            rotationSpeed = 0.0f;
        }
        mouseLeftPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        keyMReleased = true;
        movementBlocked = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (keyMReleased) {
            keyMPressed = true;
            keyMReleased = false;
        }
    }

    if (keyMPressed) {
        if (mapVisible) {
            mapVisible = false;
        }
        else {
            mapVisible = true;
        }
        keyMPressed = false;
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
    if (!movementBlocked) {
        camera.ProcessMouseMovement(deltaTime, xoffset, yoffset);
    }
    
}

/*
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
*/

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