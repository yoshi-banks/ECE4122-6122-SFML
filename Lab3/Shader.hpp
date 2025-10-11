/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: 
 */

#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>


class Shader
{
    public:
        GLuint programID;

        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();

        void use();
        void setMat4(const std::string& name, const glm::mat4& mat);
        void setVec3(const std::string& name, const glm::vec3& vec);
        void setFloat(const std::string& name, float value);
        void setInt(const std::string& name, int value);

    private:
        GLuint loadShader(const char* path, GLenum shaderType);
        std::string readFile(const char* path);
};