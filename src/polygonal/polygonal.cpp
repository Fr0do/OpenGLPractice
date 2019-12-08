//
// Created by Maxim Kurkin on 04.12.2019.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <helpers/filesystem.h>
#include <helpers/shader.h>
#include <helpers/camera.h>

#include "../objects.h"

#include <iostream>
#include<string>
#include<vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);
void renderFloor();
void renderCube();
void renderScene(const Shader &shader, unsigned int flSpecular, unsigned int flDiffuse,
                 unsigned int cSpecular, unsigned int cDiffuse, unsigned int cEmission);
void renderSkybox();
void renderSphere(int xSeg = 64, int ySeg = 64);
void renderTorus(double r = 0.2, double c = 0.45,
                 int rSeg = 64, int cSeg = 32);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const float PI = 3.14159265359;
const float TAU = 2 * PI;

bool filling = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpengGL CMC MSU 2020", nullptr, nullptr);
    if (window == nullptr) {
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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwWindowHint(GLFW_CURSOR_DISABLED, GL_TRUE);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader skyboxShader("skybox_vert.glsl", "skybox_frag.glsl");
    Shader lightingShader("basic_vert.glsl", "lights_frag.glsl");
    Shader lampShader("basic_vert.glsl", "white_frag.glsl");

    Shader shadowShader("shadow_mapping_vert.glsl", "shadow_mapping_frag.glsl");
    Shader shadowDepthShader("shadow_mapping_depth_vert.glsl", "shadow_mapping_depth_frag.glsl");
//    Shader bloomShader("bloom_vert.glsl", "bloom_frag.glsl");
//    Shader lightShader("bloom_vert.glsl", "light_box.glsl");
//    Shader blurShader("blur_vert.glsl", "blur_frag.glsl");
//    Shader bloomFinalShader("bloom_final_vert.glsl", "bloom_final_frag.glsl");

    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //shader configuration
    shadowShader.use();
    shadowShader.setInt("diffuseTexture", 0);
    shadowShader.setInt("shadowMap", 1);


    // lighting info
    //glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);


    // configure (floating point) framebuffers
//    unsigned int hdrFBO;
//    glGenFramebuffers(1, &hdrFBO);
//    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//    // create 2 floating point color buffers (one for normal rendering, another for brightness treshold values)
//    unsigned int colorBuffers[2];
//    glGenTextures(2, colorBuffers);
//    for (unsigned int i = 0; i < 2; i++)
//    {
//        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        // attach texture to framebuffer
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
//    }
//    // create and attach depth buffer (renderbuffer)
//    unsigned int rboDepth;
//    glGenRenderbuffers(1, &rboDepth);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
//    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
//    glDrawBuffers(2, attachments);
//    // finally check if framebuffer is complete
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // ping-pong-framebuffer for blurring
//    unsigned int pingpongFBO[2];
//    unsigned int pingpongColorbuffers[2];
//    glGenFramebuffers(2, pingpongFBO);
//    glGenTextures(2, pingpongColorbuffers);
//    for (unsigned int i = 0; i < 2; i++)
//    {
//        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
//        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
//        // also check if framebuffers are complete (no need for depth buffer)
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "Framebuffer not complete!" << std::endl;
//    }

    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());
    unsigned int floorSpecularMap = loadTexture(FileSystem::getPath("resources/textures/wood_specular.png").c_str());

    // load lighting shader textures
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());
    unsigned int emissionMap = loadTexture(FileSystem::getPath("resources/textures/container2_neon.jpg").c_str());
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    //load skybox textures
    std::vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/right.tga"),
                    FileSystem::getPath("resources/textures/skybox/left.tga"),
                    FileSystem::getPath("resources/textures/skybox/top.tga"),
                    FileSystem::getPath("resources/textures/skybox/bottom.tga"),
                    FileSystem::getPath("resources/textures/skybox/front.tga"),
                    FileSystem::getPath("resources/textures/skybox/back.tga")
            };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //init uniforms
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 64.0f);
        lightingShader.setFloat("time", glfwGetTime());

        //point lights
        for (int i = 0; i < 4; i++) {
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].ambient", 0.07f, 0.07f, 0.07f);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].constant", 1.0f);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].linear", 0.09);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].quadratic", 0.032);
        }

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        renderScene(lightingShader, floorTexture, floorSpecularMap, diffuseMap, specularMap, emissionMap);

        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        //render lamps
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.2f));
            lampShader.setMat4("model", model);
            renderTorus();
        }

        // render skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        //glDepthMask(GL_FALSE);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        renderSkybox();
        //glDepthMask(GL_TRUE);
        glDepthFunc(GL_FALSE); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!filling) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            filling = true;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            filling = false;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}


// renders the 3D scene
void renderScene(const Shader &shader, unsigned int flDiffuse, unsigned int flSpecular,
                 unsigned int cDiffuse, unsigned int cSpecular, unsigned int cEmission)
{
    //bind floor diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, flDiffuse);
    // bind floor specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, flSpecular);
    //render floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    shader.setBool("withEmission", false);
    renderFloor();

    // bind cubes diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cDiffuse);
    // bind cubes specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cSpecular);
    // bind cubes emission map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cEmission);
    // render boxes
    for (unsigned int i = 0; i < 4; i++) {
        float angle = 15.0f;
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model,i * ((i & 1) ? (float)glfwGetTime() : angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);
        shader.setBool("withEmission", i & 2);
        renderCube();
    }
}

// renders floor
unsigned int floorVAO = 0;
void renderFloor() {
    if (floorVAO == 0) {
        unsigned int floorVBO;
        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);
        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// renders cube
unsigned int cubeVAO = 0;
void renderCube()
{
    if (cubeVAO == 0)
    {
        unsigned int cubeVBO = 0;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        // normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        // texture coord attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// renders skybox
unsigned int skyboxVAO = 0;
void renderSkybox()
{
    if (skyboxVAO == 0) {
        unsigned int skyboxVBO = 0;
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glBindVertexArray(skyboxVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}



// renders (and builds at first invocation) a sphere
unsigned int sphereIndexCount = 0;
unsigned int sphereVAO = 0;
void renderSphere(int xSeg, int ySeg)
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        for (unsigned int y = 0; y <= ySeg; ++y)
        {
            for (unsigned int x = 0; x <= xSeg; ++x)
            {
                float xSegment = (float)x / (float)xSeg;
                float ySegment = (float)y / (float)ySeg;
                float xPos = std::cos(xSegment * TAU) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * TAU) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
            }
        }

        bool oddRow = false;
        for (int y = 0; y < ySeg; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (int x = 0; x <= xSeg; ++x)
                {
                    indices.push_back(y       * (xSeg + 1) + x);
                    indices.push_back((y + 1) * (xSeg + 1) + x);
                }
            }
            else
            {
                for (int x = xSeg; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (xSeg + 1) + x);
                    indices.push_back(y       * (xSeg + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        sphereIndexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 3 + 2) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

// renders (and builds at first invocation) a torus
unsigned int torusVAO = 0;
unsigned int torusIndexCount = 0;
void renderTorus(double r, double c,
                 int rSeg, int cSeg)
{
    if (torusVAO == 0)
    {
        glGenVertexArrays(1, &torusVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i <= rSeg; ++i)
        {
            for (unsigned int j = 0; j <= cSeg; ++j)
            {
                for (int k = 0; k < 2; k++) {
                    double s = (i + k) % rSeg + 0.5;
                    double t = j % (cSeg + 1);

                    double xPos = (c + r * std::cos(s * TAU / rSeg)) * std::cos(t * TAU / cSeg);
                    double yPos = (c + r * std::cos(s * TAU / rSeg)) * std::sin(t * TAU / cSeg);
                    double zPos = r * std::sin(s * TAU / rSeg);

                    double u = (i + k) / (float) rSeg;
                    double v = t / (float) cSeg;

                    positions.push_back(glm::vec3(2 * xPos, 2 * yPos, 2 * zPos));
                    normals.push_back(glm::vec3(2 * xPos, 2 * yPos, 2 * zPos));
                    uv.push_back(glm::vec2(u, v));
                }
            }
        }

        bool oddRow = false;
        for (int j = 0; j < cSeg; ++j)
        {
            if (!oddRow)
            {
                for (int i = 0; i <= rSeg; ++i)
                {
                    indices.push_back(j       * (rSeg + 1) + i);
                    indices.push_back((j + 1) * (rSeg + 1) + i);
                }
            }
            else
            {
                for (int i = rSeg; i >= 0; --i)
                {
                    indices.push_back((j + 1) * (rSeg + 1) + i);
                    indices.push_back(j       * (rSeg + 1) + i);
                }

            }
            oddRow = !oddRow;
        }
        torusIndexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(torusVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 3 + 2) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(torusVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, torusIndexCount);
}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
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

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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