/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: 
 */

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Model
{
    public:
        Model(const char* filePath);
        ~Model();

        void draw();

    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;

        void loadModel(const char* path);
        void setupMesh();
}