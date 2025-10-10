/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: 
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Application
{

    public:
        Application();
        ~Application();
        void run();
    
    private:
        void handleEvents();
        void render();
        void initialize();
};