/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: 
 */

#pragma once

#include <string>


class Camera 
{
    public:
        Camera();
        ~Camera();

    private:
        void moveRadially();
        void rotateHorizontal();
        void rotateVertical();
};