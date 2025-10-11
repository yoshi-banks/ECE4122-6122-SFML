/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: 
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

#include "Application.hpp"
#include "SphericalCamera.hpp"
#include "common/shader.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/texture.hpp"

Application* Application::instance = nullptr;

Application::Application()
    : window(nullptr)
    , camera(nullptr)
    , lightEnabled(true)
{
    instance = this;
    initialize();    
}

Application::~Application()
{
    cleanup();
}

void Application::initialize()
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // TODO replace these magic numbers for window size, replace with parameter file
    window = glfwCreateWindow(1024, 768, "Suzanne Heads", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        exit(-1);
    }

    glfwSetKeyCallback(window, Application::keyCallback);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
    texture = loadDDS("uvmap.DDS");

    // materialDiffuseID = glGetUniformLocation(programID, "MaterialDiffuseColor");
    matrixID = glGetUniformLocation(programID, "MVP");
    viewMatrixID = glGetUniformLocation(programID, "V");
    modelMatrixID = glGetUniformLocation(programID, "M");
    lightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    lightEnabledID = glGetUniformLocation(programID, "LightEnabled");
    textureID = glGetUniformLocation(programID, "myTextureSampler");

    camera = new SphericalCamera(15.0f, 0.0f, 60.0f);

    setupBuffers();
}

void Application::setupBuffers()
{
    // Load suzzane the monkey
    loadAssImp("suzanne.obj", indices, indexedVertices, indexedUVs, indexedNormals);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(glm::vec3), &indexedVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedUVs.size() * sizeof(glm::vec2), &indexedUVs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(glm::vec3), &indexedNormals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    // Green plane
    float planeVertices[] = {
        -10.0f, -10.0f, 0.0f,
         10.0f, -10.0f, 0.0f,
         10.0f,  10.0f, 0.0f,
        -10.0f,  10.0f, 0.0f
    };
    float planeNormals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    float planeUVs[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenBuffers(1, &planeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &planeNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeNormals), planeNormals, GL_STATIC_DRAW);

    glGenBuffers(1, &planeUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeUVs), planeUVs, GL_STATIC_DRAW);
}

void Application::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    if (!instance)
    {
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_W)
        {
            instance->camera->moveCloser(0.5f);
        }
        else if (key == GLFW_KEY_S)
        {
            instance->camera->moveFarther(0.5f);
        }
        else if (key == GLFW_KEY_A)
        {
            instance->camera->rotateLeft(5.0f);
        }
        else if (key == GLFW_KEY_D)
        {
            instance->camera->rotateRight(5.0f);
        }
        else if (key == GLFW_KEY_UP)
        {
            instance->camera->rotateUp(5.0f);
        }
        else if (key == GLFW_KEY_DOWN)
        {
            instance->camera->rotateDown(5.0f);
        }
        else if (key == GLFW_KEY_L && action == GLFW_PRESS)
        {
            instance->lightEnabled = !instance->lightEnabled;
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(win, 1);
        }
    }
}

void Application::run()
{
    while (glfwWindowShouldClose(window) == 0)
    {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera->getViewMatrix();

    glm::vec3 lightPos = glm::vec3(0, 0, 10);
    glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(lightEnabledID, lightEnabled ? 1 : 0);

    // Draw green plane
    glm::mat4 planeModel = glm::mat4(1.0);
    glm::mat4 planeMVP = projectionMatrix * viewMatrix * planeModel;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &planeMVP[0][0]);
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &planeModel[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

    // set plane color
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, planeUVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormalBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Draw 8 heads
    float radius = 4.0f;
    for (int i = 0; i < 8; i++)
    {
        float angle = i * (360.0f / 8.0f);
        glm::mat4 modelMatrix = glm::mat4(1.0);

        // Rotate each head around Z to make a circle
        modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 0, 1));
        // Move head outward from center
        modelMatrix = glm::translate(modelMatrix, glm::vec3(radius, 0, 0));
        // Tilt head upright
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
        // Move head up to touch z=0 plane
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        // rotate to face outwards (pi/2 about y axis)
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));


        glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

        // set color
        // glUniform3f(materialDiffuseID, 0.8f, 0.7f, 0.6f); // Beige heads
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Application::cleanup()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &planeVertexBuffer);
    glDeleteBuffers(1, &planeNormalBuffer);
    glDeleteBuffers(1, &planeUVBuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteTextures(1, &texture);

    delete camera;
    glfwTerminate();
}

