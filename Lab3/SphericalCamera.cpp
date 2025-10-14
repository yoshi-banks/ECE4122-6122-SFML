/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-11-10
 * 
 * @brief: Implements a spherical coordinate cmaera system for 3d scene 
 *         navigation. The camera orbits around a fixed target point
 *         using spherical coordaintes (radius, azimuth angle, elevation
 *         angle)
 */

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

#include "SphericalCamera.hpp"

/**
 * @brief: Construct a new Spherical Camera:: Spherical Camera object with 
 *         specified spherical coordiante. Initialized the camera position 
 *         based on radius and angles, with the camera looking at the origin
 *         and using positive z as up.
 * 
 * @param radius: Distance from the camera to the target point
 * @param theta: Azimuth angle in degrees
 * @param phi: Elevation angle in degrees
 */
SphericalCamera::SphericalCamera(float radius, float theta, float phi)
    : radius(radius)
    , theta(theta)
    , phi(phi)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 0.0f, 1.0f)
{
    // Calculate initial cartesian position
    updatePosition();
}

/**
 * @brief: Destroy the Spherical Camera:: Spherical Camera object
 * 
 */
SphericalCamera::~SphericalCamera()
{
    // nothing to clean up
}

/**
 * @brief: Converts spherical coordinates to cartesian coordate
 *         and updates the camera's position vector. Clamps 
 *         evleation angle to prevent gimbal lock at the poles
 * 
 * @return * void 
 */
void SphericalCamera::updatePosition()
{
    phi = std::max(0.1f, std::min(179.9f, phi));

    float phiRad = glm::radians(phi);
    float thetaRad = glm::radians(theta);

    position.x = radius * sin(phiRad) * cos(thetaRad);
    position.y = radius * sin(phiRad) * sin(thetaRad);
    position.z = radius * cos(phiRad);
}

/**
 * @brief: Generates and returns the view matrix for the camera
 *         The view matrix transformes world coordiantes to 
 *         camera/eye coordinates
 * 
 * @return glm::mat4 matrix that opsitions and oreints the camera 
 *         in the scene
 */
glm::mat4 SphericalCamera::getViewMatrix()
{
    return glm::lookAt(position, target, up);
}

/**
 * @brief: Returns the current Cartesian position of the camera in world space
 * 
 * @return glm::vec3 vector representing the camera's current position
 */
glm::vec3 SphericalCamera::getPosition()
{
    return position;
}

/**
 * @brief: Moves the camera close to the target position by decreasing the radius
 * 
 * @param delta Amount to decrease the radius
 */
void SphericalCamera::moveCloser(float delta)
{
    radius = std::max(1.0f, radius - delta);
    updatePosition();
}

/**
 * @brief: Moves the camera farther from the target position by increasing the radius
 * 
 * @param delta Amount in increase the radius
 */
void SphericalCamera::moveFarther(float delta)
{
    radius += delta;
    updatePosition();
}

/**
 * @brief: Rotates the camera counter-clockwise (left)
 * 
 * @param delta Angle in degrees to rotate counter-clockwise
 */
void SphericalCamera::rotateLeft(float delta)
{
    theta += delta;
    updatePosition();
}

/**
 * @brief: Rotates the camera clockwise (right)
 * 
 * @param delta Angle in degrees to rotate clockwise
 */
void SphericalCamera::rotateRight(float delta)
{
    theta -= delta;
    updatePosition();
}

/**
 * @brief: Rotates the camera upward by decreasing the elevation angle
 * 
 * @param delta Angle in degrees to move upwards
 */
void SphericalCamera::rotateUp(float delta)
{
    phi -= delta;
    updatePosition();
}

/**
 * @brief: Rotates the camera downwards by increasing the elevation angle
 * 
 * @param delta Angle in degre3es to move upwards
 */
void SphericalCamera::rotateDown(float delta)
{
    phi += delta;
    updatePosition();
}