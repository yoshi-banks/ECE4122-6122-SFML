/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: Implements a spherical coordinate cmaera system for 3d scene 
 *         navigation. The camera orbits around a fixed target point
 *         using spherical coordaintes (radius, azimuth angle, elevation
 *         angle)
 */

#pragma once

#include <glm/glm.hpp>

/**
 * @brief: Spherical Camera class. Implements 3D scene navigation
 */
class SphericalCamera
{
    public:
        SphericalCamera(float radius = 15.0f, float theta = 0.0f, float phi = 60.0f);
        ~SphericalCamera();

        glm::mat4 getViewMatrix();
        glm::vec3 getPosition();

        void moveCloser(float delta);
        void moveFarther(float delta);
        void rotateLeft(float delta);
        void rotateRight(float delta);
        void rotateUp(float delta);
        void rotateDown(float delta);

    private:
        float radius;
        float theta;
        float phi;

        void updatePosition();
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
};