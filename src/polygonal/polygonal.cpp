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
#include <string>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);
void renderFloor();
void renderCube();
void renderWall();
void renderScene(const Shader &shader, unsigned int flDiffuse, unsigned int flSpecular,
                 unsigned int cDiffuse, unsigned int cSpecular, unsigned int cEmission);
void renderSkybox();
void renderSphere(int xSeg = 64, int ySeg = 64);
void renderTorus(double r = 0.2, double c = 0.45,
                 int rSeg = 64, int cSeg = 32);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1000;
float heightScale = 0.1;
const float PI = 3.14159265359;
const float TAU = 2 * PI;

bool filling = false; //press SPACE to see scene without textures
bool shadows = true;
bool shadowsKeyPressed = false; //press H to enable/disable shadows

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
    Shader lampShader("basic_vert.glsl", "lamp_frag.glsl");
    Shader shadowShader("shadow_mapping_vert.glsl", "shadow_mapping_frag.glsl");
    Shader shadowDepthShader("shadow_mapping_depth_vert.glsl", "shadow_mapping_depth_frag.glsl", "shadow_mapping_depth_geom.glsl");
    Shader parallaxShader("parallax_mapping_vert.glsl", "parallax_mapping_frag.glsl");

    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //shader configuration
    shadowShader.use();
    shadowShader.setInt("diffuseTexture", 0);
    shadowShader.setInt("shadowMap", 1);

    //load textures
    unsigned int floorTexture     = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());
    unsigned int floorSpecularMap = loadTexture(FileSystem::getPath("resources/textures/wood_specular.png").c_str());

    unsigned int boxDiffuseMap  = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
    unsigned int boxSpecularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());
    unsigned int boxEmissionMap = loadTexture(FileSystem::getPath("resources/textures/container2_neon.jpg").c_str());

    unsigned int groundDiffuseMap = loadTexture(FileSystem::getPath("resources/textures/pbr/acoustic/albedo.jpg").c_str());
    unsigned int groundNormalMap  = loadTexture(FileSystem::getPath("resources/textures/pbr/acoustic/normal.jpg").c_str());
    unsigned int groundHeightMap  = loadTexture(FileSystem::getPath("resources/textures/pbr/acoustic/displacement.png").c_str());

    // shader configuration
    shadowShader.use();
    shadowShader.setInt("diffuseTexture", 0);
    shadowShader.setInt("depthMap", 1);

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    parallaxShader.use();
    parallaxShader.setInt("diffuseMap", 0);
    parallaxShader.setInt("normalMap", 1);
    parallaxShader.setInt("depthMap", 2);

    lampShader.use();
    lampShader.setInt("lightColor", 0);

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

        if (shadows) {
            // 0. create depth cubemap transformation matrices
            // only ONE light source used for shadow!
            glm::vec3 lightPos(3.0, 1.0, sin(glfwGetTime() * 0.5) * 3.0);
            float near_plane = 1.0f;
            float far_plane = 25.0f;
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

            // 1. render scene to depth cubemap
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowDepthShader.use();
            for (unsigned int i = 0; i < 6; ++i)
                shadowDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            shadowDepthShader.setFloat("far_plane", far_plane);
            shadowDepthShader.setVec3("lightPos", lightPos);
            renderScene(shadowDepthShader, floorTexture, floorSpecularMap, boxDiffuseMap, boxSpecularMap, boxEmissionMap);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 2.1 render scene using the generated depth/shadow map
            glViewport(0, 0, SCR_WIDTH * 2, SCR_HEIGHT * 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shadowShader.use();
            shadowShader.setMat4("projection", projection);
            shadowShader.setMat4("view", view);
            shadowShader.setVec3("lightPos", lightPos);
            shadowShader.setVec3("viewPos", camera.Position);
            shadowShader.setFloat("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            //render floor
            model = glm::mat4(1.0f);
            shadowShader.setMat4("model", model);
            renderFloor();

            // bind cubes diffuse map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, boxDiffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            // render boxes
            for (unsigned int i = 0; i < 4; i++) {
                float angle = 15.0f;
                model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model,i * ((i & 1) ? (float)glfwGetTime() : angle), glm::vec3(1.0f, 0.3f, 0.5f));
                shadowShader.setMat4("model", model);
                renderCube();
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else {
            // 2.2 render scene with other lights
            glViewport(0, 0, SCR_WIDTH * 2, SCR_HEIGHT * 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            lightingShader.use();
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);
            lightingShader.setVec3("viewPos", camera.Position);
            lightingShader.setFloat("material.shininess", 64.0f);
            lightingShader.setFloat("time", glfwGetTime());
            //point lights
            for (int i = 0; i < 4; i++) {
                lightingShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
                lightingShader.setVec3("pointLights["  + std::to_string(i) + "].ambient", pointLightColors[i].x * 0.1,  pointLightColors[i].y  * 0.1,  pointLightColors[i].z * 0.1);
                lightingShader.setVec3("pointLights["  + std::to_string(i) + "].diffuse", pointLightColors[i].x,  pointLightColors[i].y,  pointLightColors[i].z);
                lightingShader.setVec3("pointLights["  + std::to_string(i) + "].specular", pointLightColors[i].x,  pointLightColors[i].y,  pointLightColors[i].z);
                lightingShader.setFloat("pointLights["  + std::to_string(i) + "].constant", 1.0f);
                lightingShader.setFloat("pointLights["  + std::to_string(i) + "].linear", 0.09);
                lightingShader.setFloat("pointLights["  + std::to_string(i) + "].quadratic", 0.032);
            }
            renderScene(lightingShader, floorTexture, floorSpecularMap, boxDiffuseMap, boxSpecularMap, boxEmissionMap);

            // 3. render lamps
            lampShader.use();
            lampShader.setMat4("projection", projection);
            lampShader.setMat4("view", view);
            for (int i = 0; i < 4; i++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                lampShader.setVec3("lightColor", pointLightColors[i]);
                lampShader.setMat4("model", model);
                renderCube();
            }

            // 4. render parallax-mapped wall
            parallaxShader.use();
            parallaxShader.setMat4("projection", projection);
            parallaxShader.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::translate(model, wallPosition);
            model = glm::rotate(model, glm::radians((float)glfwGetTime() * -5.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
            parallaxShader.setMat4("model", model);
            parallaxShader.setVec3("viewPos", camera.Position);
            parallaxShader.setVec3("lightPos", pointLightPositions[2]);
            parallaxShader.setFloat("heightScale", heightScale); // adjust with Q and E keys
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, groundDiffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, groundNormalMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, groundHeightMap);
            renderWall();
        }

        // 4. render skybox as last
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
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
    {
        shadowsKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
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
    if (cubeVAO == 0) {
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


// renders a 1x1 wall with  tangent vectors
unsigned int wallVAO = 0;
unsigned int wallVBO;
void renderWall()
{
    if (wallVAO == 0) {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float wallVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &wallVAO);
        glGenBuffers(1, &wallVBO);
        glBindVertexArray(wallVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), &wallVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(wallVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    if (sphereVAO == 0) {
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

                positions.emplace_back(xPos, yPos, zPos);
                normals.emplace_back(xPos, yPos, zPos);
                uv.emplace_back(xSegment, ySegment);
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
            if (!normals.empty())
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (!uv.empty())
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
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

                    double u = (float) (i + k) / (float) rSeg;
                    double v = t / (float) cSeg;

                    positions.emplace_back(2 * xPos, 2 * yPos, 2 * zPos);
                    normals.emplace_back(2 * xPos, 2 * yPos, 2 * zPos);
                    uv.emplace_back(u, v);
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
            if (!normals.empty())
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (!uv.empty())
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)nullptr);
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