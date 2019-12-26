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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderSphere(int xSeg = 64, int ySeg = 64);
void renderTorus(double r = 0.1, double c = 0.25,
                 int rSeg = 64, int cSeg = 32);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const float PI = 3.14159265359;
const float TAU = 2 * PI;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader CookTorranceShader("pbr_vert.glsl", "pbr_frag.glsl");

    CookTorranceShader.use();
    CookTorranceShader.setInt("albedoMap", 0);
    CookTorranceShader.setInt("normalMap", 1);
    CookTorranceShader.setInt("metallicMap", 2);
    CookTorranceShader.setInt("roughnessMap", 3);
    CookTorranceShader.setInt("aoMap", 4);

    // load PBR material textures
    unsigned int groundAlbedo    = loadTexture(FileSystem::getPath("resources/textures/pbr/ground/albedo.jpg").c_str());
    unsigned int groundNormal    = loadTexture(FileSystem::getPath("resources/textures/pbr/ground/normal.jpg").c_str());
    unsigned int groundMetallic  = loadTexture(FileSystem::getPath("resources/textures/pbr/ground/metallic.png").c_str());
    unsigned int groundRoughness = loadTexture(FileSystem::getPath("resources/textures/pbr/ground/roughness.jpg").c_str());
    unsigned int groundAo        = loadTexture(FileSystem::getPath("resources/textures/pbr/ground/ao.jpg").c_str());

    unsigned int chainmailAlbedo    = loadTexture(FileSystem::getPath("resources/textures/pbr/chainmail/albedo.jpg").c_str());
    unsigned int chainmailNormal    = loadTexture(FileSystem::getPath("resources/textures/pbr/chainmail/normal.jpg").c_str());
    unsigned int chainmailMetallic  = loadTexture(FileSystem::getPath("resources/textures/pbr/chainmail/metallic.jpg").c_str());
    unsigned int chainmailRoughness = loadTexture(FileSystem::getPath("resources/textures/pbr/chainmail/roughness.jpg").c_str());
    unsigned int chainmailAo        = loadTexture(FileSystem::getPath("resources/textures/pbr/chainmail/ao.jpg").c_str());

    // lights
    int nrRows = 2;
    int nrColumns = 3;
    float spacing = 2.5;

    // initialize static shader uniforms before rendering
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    CookTorranceShader.use();
    CookTorranceShader.setMat4("projection", projection);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CookTorranceShader.use();
        glm::mat4 view = camera.GetViewMatrix();
        CookTorranceShader.setMat4("view", view);
        CookTorranceShader.setVec3("camPos", camera.Position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundAlbedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, groundNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, groundMetallic);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, groundRoughness);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, groundAo);

        // render rows*column number of spheres with material properties defined by textures (ground & chainmail)
        glm::mat4 model = glm::mat4(1.0f);

        for (int col = 0; col < nrColumns; ++col)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                    (float)(col - (nrColumns / 2)) * spacing,
                    (float)(0 - (nrRows / 2)) * spacing,
                    (float)(0 - (nrRows + nrColumns / 2)) * spacing
            ));
            CookTorranceShader.setMat4("model", model);
            renderSphere();
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, chainmailAlbedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, chainmailNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, chainmailMetallic);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, chainmailRoughness);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, chainmailAo);

        for (int col = 0; col < nrColumns; ++col)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                    (float)(col - (nrColumns / 2)) * spacing + 2.5,
                    (float)(1 - (nrRows / 2)) * spacing,
                    (float)(1 - (nrRows + nrColumns / 2)) * spacing + 2.5
            ));
            CookTorranceShader.setMat4("model", model);
            renderTorus();
        }



        // render light source (toruses)
        for (unsigned int i = 0; i < sizeof(pbrLightPositions) / sizeof(pbrLightPositions[0]); ++i)
        {
            glm::vec3 newPos = pbrLightPositions[i] + glm::vec3(std::sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = pbrLightPositions[i];
            CookTorranceShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            CookTorranceShader.setVec3("lightColors[" + std::to_string(i) + "]", pbrLightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            CookTorranceShader.setMat4("model", model);
            renderTorus();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
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
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
            }
        }

        for (int y = 0; y < ySeg; ++y)
        {
            for (int x = 0; x <= xSeg; ++x) {
                indices.push_back(y * (xSeg + 1) + x);
                indices.push_back((y + 1) * (xSeg + 1) + x);
            }
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
