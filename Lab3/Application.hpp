/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: Main Application class for rendering the 3D suzanne monkey heads
 *         Arranged in a circle on a green plane. An light source takes and 
 *         elliptical path in the sky above the heads. 
 * 
 *         There are interactive camera controls with a - rotate left, d - rotate right, 
 *         s - zoom out, w - zoom in, up-arrow - rotate up, down-arrow - rotate down, 
 *         l - toggle lighting
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class SphericalCamera;

/**
 * @brief: Application class. Runs the Suzanne 8 heads in a circle for Lab3
 */
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