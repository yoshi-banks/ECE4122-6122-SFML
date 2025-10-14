/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: Main Application class for rendering the 3D suzanne monkey heads
 *         Arranged in a circle on a green plane. An light source takes and 
 *         elliptical path in the sky above the heads. 
 * 
 *         There are interactive camera controls with a - rotate left, d - rotate right, 
 *         s - zoom out, w - zoom in, up-arrow - rotate up, down-arrow - rotate down, 
 *         l - toggle lighting
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

/**
 * @brief: Construct a new Application:: Application object and run initization
 * 
 */
Application::Application()
    : window(nullptr)
    , camera(nullptr)
    , lightEnabled(true)
{
    instance = this;
    initialize();    
}

/**
 * @brief: Destroy the Application:: Application object and run cleanup to release
 *         resources
 * 
 */
Application::~Application()
{
    cleanup();
}

/**
 * @brief: initialize the glfw window, glew, openGL context, shaders, textures
 *         camera, set up all necessary OpenGL state
 * 
 */
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

    // Create a window of specified dimensions
    // TODO replace these magic numbers for window size, replace with parameter file
    window = glfwCreateWindow(1024, 768, "Suzanne Heads", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW for OpenGL extension loading
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        exit(-1);
    }

    // Register keypress callback
    glfwSetKeyCallback(window, Application::keyCallback);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Dark blue background
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for 3D rendering
    glDepthFunc(GL_LESS);                 // Accept fragment if closer to camera
    glEnable(GL_CULL_FACE);               // Cull face for performance

    // Create and bind vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Load and compile shaders
    programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
    texture = loadDDS("uvmap.DDS");
    planeTexture = loadBMP_custom("grass.bmp");

    // Get uniform locations from shaders
    matrixID = glGetUniformLocation(programID, "MVP");
    viewMatrixID = glGetUniformLocation(programID, "V");
    modelMatrixID = glGetUniformLocation(programID, "M");
    lightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    lightEnabledID = glGetUniformLocation(programID, "LightEnabled");
    textureID = glGetUniformLocation(programID, "myTextureSampler");
    useTextureID = glGetUniformLocation(programID, "UseTexture");
    solidColorID = glGetUniformLocation(programID, "SolidColor");

    // Create new camera obect and initialize position
    camera = new SphericalCamera(15.0f, 0.0f, 60.0f);

    // Set up vertex buffers for geometry
    setupBuffers();
}

/**
 * @brief: 
 * 
 */
void Application::setupBuffers()
{
    // Load suzzane the monkey
    loadAssImp("suzanne.obj", indices, indexedVertices, indexedUVs, indexedNormals);

    // Create and populate vertex buffer for Suzanne
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(glm::vec3), &indexedVertices[0], GL_STATIC_DRAW);

    // Create and populate uv buffer for Suzanne
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedUVs.size() * sizeof(glm::vec2), &indexedUVs[0], GL_STATIC_DRAW);

    // Create and populate normal vector buffer
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(glm::vec3), &indexedNormals[0], GL_STATIC_DRAW);

    // Creaet and populate index buffer for index drawing
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    // Green plane
    // plane vertices
    float planeVertices[] = {
        -10.0f, -10.0f, 0.0f,
         10.0f, -10.0f, 0.0f,
         10.0f,  10.0f, 0.0f,
        -10.0f,  10.0f, 0.0f
    };
    // Normal vectors pointing in positive Z direction
    float planeNormals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    // UV coordinates for plane (only used if SolidColor isn't set, only used by shader)
    float planeUVs[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Create and populate plane vertex buffer
    glGenBuffers(1, &planeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    // Create and pouplate normal vector buffer
    glGenBuffers(1, &planeNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeNormals), planeNormals, GL_STATIC_DRAW);

    // Create and populate UV buffer
    glGenBuffers(1, &planeUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeUVs), planeUVs, GL_STATIC_DRAW);
}

/**
 * @brief: Static callback function for keyboard input
 *         Processes camera input (W/S/A/D/UP/DOWN), lighting toggling (L)
 *         and exit command (ESC). Uses the singleton instance to access camera
 * 
 * @param win: GLFW window that received the key event
 * @param key: The keyboard key that was pressed
 * @param scancode: System-sepcifric scanecode of the key
 * @param action: GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT
 * @param mods: Bit field describing which modifier keys were held down
 */
void Application::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    // Verify instanc exists
    if (!instance)
    {
        return;
    }

    // Only process press and repeat events
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_W)
        {
            // move camera closer to origin
            instance->camera->moveCloser(0.5f);
        }
        else if (key == GLFW_KEY_S)
        {
            // move camear farther from origin
            instance->camera->moveFarther(0.5f);
        }
        else if (key == GLFW_KEY_A)
        {
            // rotate camera left (counter-clockwise) 
            instance->camera->rotateLeft(5.0f);
        }
        else if (key == GLFW_KEY_D)
        {
            // rotate camera right (clockwise)
            instance->camera->rotateRight(5.0f);
        }
        else if (key == GLFW_KEY_UP)
        {
            // Rotate camera up (increase elevation)
            instance->camera->rotateUp(5.0f);
        }
        else if (key == GLFW_KEY_DOWN)
        {
            // Rotate camera down (decrease eleveation)
            instance->camera->rotateDown(5.0f);
        }
        else if (key == GLFW_KEY_L && action == GLFW_PRESS)
        {
            // toggle lighting on/off (only on initial press not on repeat)
            instance->lightEnabled = !instance->lightEnabled;
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            // Singla window should close
            glfwSetWindowShouldClose(win, 1);
        }
    }
}

/**
 * @brief: Main application loop. Continuously renders frames and processes events
 *         until window is closed. Handles buffer swaping and event polling
 */
void Application::run()
{
    while (glfwWindowShouldClose(window) == 0)
    {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

/**
 * @brief: Rdners a single frame fo the scene. Draws the green ground plane and 8
 *         suzanne heads arranged in a circle facing outwards with ears touchign.
 *         Calculates and applies the elliptical light source. Uses SolidColor mode
 *         for plane and texturing for the Suzanne heads.
 */
void Application::render()
{
    // Clear color and depth buffers to prepare for new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    // Calculate projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    
    // Get view matrix from camera
    glm::mat4 viewMatrix = camera->getViewMatrix();

    // Elliptical light movement
    float time = glfwGetTime();
    float speed = 0.5f;                 // Angular velocity of lihgt
    float radiusX = 8.0f;               // Horizontal ellipse radius
    float radiusY = 2.0f;               // Vertical ellipse radius
    glm::vec3 lightPos = glm::vec3(     // X position follows cosine
        radiusX * cos(time * speed),    // Y position follows sine
        radiusY * sin(time * speed),    // fixed height above ground
        4.0f
    );

    // Send lgiht parameters to shader
    glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(lightEnabledID, lightEnabled ? 1 : 0);

    // Draw green plane
    glm::mat4 planeModel = glm::mat4(1.0);
    glm::mat4 planeMVP = projectionMatrix * viewMatrix * planeModel;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &planeMVP[0][0]);
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &planeModel[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

    // Use solid color for the plane
    glUniform1i(useTextureID, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planeTexture);
    glUniform1i(textureID, 0);
    // glUniform3f(solidColorID, 0.0f, 0.8f, 0.0f); // Green color

    // Bind plane vertex attributes
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, planeUVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormalBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Draw plane as a triangle fan
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // =========== Draw textured Suzanne heads =================
    // bind the texture for the heads
    glUniform1i(useTextureID, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureID, 0);

    // Draw 8 heads arrange in a circle
    float radius = 3.8f; // distance from center to head
    for (int i = 0; i < 8; i++)
    {
        // Calculates the angle for this head
        float angle = i * (360.0f / 8.0f);
        glm::mat4 modelMatrix = glm::mat4(1.0);

        // Apply transformations in sequence:
        // 1. Rotate each head around Z to make a circle
        modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 0, 1));
        // 2. Move head outward from center
        modelMatrix = glm::translate(modelMatrix, glm::vec3(radius, 0, 0));
        // 3. Tilt head upright
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
        // 4. Move head up to touch z=0 plane
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        // 5. rotate to face outwards (pi/2 about y axis)
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));

        // Calculate the final MVP matrix
        glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

        // Send trasnformations matrices to shader
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
        
        // Bind Suzanne vertex attributes
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Draw the head using indexed triangles
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    }

    // Disable vertex attribute arrays after rendering
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

/**
 * @brief: Cleans up all OpenGL resources and termaintes GLFW.
 *         Deletes all buffers, shader programs, vertex arrays,
 *         texures, and eallocates the camera object. Should be
 *         called before program exit.
 */
void Application::cleanup()
{
    // Delete all vertex buffer objects
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &planeVertexBuffer);
    glDeleteBuffers(1, &planeNormalBuffer);
    glDeleteBuffers(1, &planeUVBuffer);

    // Delete shader program
    glDeleteProgram(programID);

    // Delete vertex array object
    glDeleteVertexArrays(1, &vertexArrayID);

    // Delete texture
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &planeTexture);

    // Deallocate camera object
    delete camera;

    // Terminate GLFW adn clean up resources
    glfwTerminate();
}

