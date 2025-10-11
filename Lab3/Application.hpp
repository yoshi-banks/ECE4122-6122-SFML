/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: 
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class SphericalCamera;

class Application
{
    public:
        Application();
        ~Application();
        void run();

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    private:
        void initialize();
        void setupBuffers();
        void render();
        void cleanup();

        GLFWwindow* window;
        SphericalCamera* camera;

        GLuint vertexArrayID;
        GLuint programID;
        GLuint matrixID, viewMatrixID, modelMatrixID;
        GLuint lightID, lightEnabledID;
        GLuint texture, textureID;
        GLuint useTextureID, solidColorID;

        // Suzanne buffers
        GLuint vertexBuffer, uvBuffer, normalBuffer, elementBuffer;
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> indexedVertices;
        std::vector<glm::vec2> indexedUVs;
        std::vector<glm::vec3> indexedNormals;

        // Plane buffers
        GLuint planeVertexBuffer, planeNormalBuffer, planeUVBuffer;

        bool lightEnabled;

        static Application* instance;
};