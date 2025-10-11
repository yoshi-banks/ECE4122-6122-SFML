/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: 
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // Load vertex and fragment shader source files respectively
    GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    // create a shader program and attach shaders
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // check if linking succeeded
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
    }

    // delete shaders since they're in the program now
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() 
{
    glDeleteProgram(programID);
}

void Shader::use()
{
    glUseProgram(programID);
}

GLuint Shader::loadShader(const char* path, GLenum shaderType)
{
    // read shader source from file path
    std::string code = readFile(path);
    const char* codePtr = code.c_str();

    // create a shader object and compile
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &codePtr, NULL);
    glCompileShader(shader);

    // check status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed (" << path << "): " << infoLog << std::endl;
    }

    return shader;
}

std::string Shader::readFile(const char* path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec)
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}