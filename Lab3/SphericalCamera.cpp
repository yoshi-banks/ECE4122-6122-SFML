/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-09
 * 
 * @brief: 
 */

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

#include "SphericalCamera.hpp"


SphericalCamera::SphericalCamera(float radius, float theta, float phi)
    : radius(radius)
    , theta(theta)
    , phi(phi)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 0.0f, 1.0f)
{
    updatePosition();
}

SphericalCamera::~SphericalCamera()
{

}

void SphericalCamera::updatePosition()
{
    phi = std::max(0.1f, std::min(179.9f, phi));

    float phiRad = glm::radians(phi);
    float thetaRad = glm::radians(theta);

    position.x = radius * sin(phiRad) * cos(thetaRad);
    position.y = radius * sin(phiRad) * sin(thetaRad);
    position.z = radius * cos(phiRad);
}

glm::mat4 SphericalCamera::getViewMatrix()
{
    return glm::lookAt(position, target, up);
}

glm::vec3 SphericalCamera::getPosition()
{
    return position;
}

void SphericalCamera::moveCloser(float delta)
{
    radius = std::max(1.0f, radius - delta);
    updatePosition();
}

void SphericalCamera::moveFarther(float delta)
{
    radius += delta;
    updatePosition();
}

void SphericalCamera::rotateLeft(float delta)
{
    theta += delta;
    updatePosition();
}

void SphericalCamera::rotateRight(float delta)
{
    theta -= delta;
    updatePosition();
}

void SphericalCamera::rotateUp(float delta)
{
    phi -= delta;
    updatePosition();
}

void SphericalCamera::rotateDown(float delta)
{
    phi += delta;
    updatePosition();
}