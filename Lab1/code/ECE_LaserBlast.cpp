/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Desciption: Implementation of ECE_LaserBlast class
 * 
 * Details:
 * The ECE_LaserBlast class represents laser blasts fired by the player or enemies.
 * It handles movement, rendering, collision detection, and active state management.
 * The class inherits from sf::Sprite and encapsulates the behavior and properties
 * of laser blasts. It includes methods for loading textures, resizing, centering,
 * updating position, checking collisions, and deactivating the laser when it goes
 * out of bounds.
 */
#include <iostream>

#include "ECE_LaserBlast.hpp"
#include "params.hpp"

/**
 * @brief: Construct a new ece laserblast::ece laserblast object
 * 
 * @param texture texture to set laser to
 * @param startPosition starting position of laser
 * @param dir direction laser travels
 * @param laserSpeed speed laser travels at
 */
ECE_LaserBlast::ECE_LaserBlast(const sf::Texture& texture, sf::Vector2f startPosition,
                               Direction dir, float laserSpeed)
    : direction(dir)
    , speed(laserSpeed)
    , active(true)
{
    setTexture(texture);
    setPosition(startPosition);

    // Set velocity based on direction
    velocity.x = 0.0f;
    velocity.y = static_cast<float>(direction) * speed;

    resizeSprite(LASER_WIDTH_PX, LASER_HEIGHT_PX);

    centerSprite();
}

/**
 * @brief: Resize the sprite to specified width and height
 * 
 * @param widthPx width to size to
 * @param heightPx height to size to
 */
void ECE_LaserBlast::resizeSprite(const float widthPx, const float heightPx)
{
    sf::FloatRect textureRect = getLocalBounds();
    setScale(widthPx / textureRect.width, heightPx / textureRect.height);
}

/**
 * @brief: Center the sprite to make moving easier
 * 
 */
void ECE_LaserBlast::centerSprite()
{
    // Center the sprite origin for better collision detection
    sf::FloatRect textureRect = getLocalBounds();
    setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}

/**
 * @brief: Update laser position
 * 
 * @param deltaTime time since last update
 */
void ECE_LaserBlast::update(float deltaTime)
{
    if (!active)
    {
        return;
    }

    // Update position
    move(velocity * deltaTime);

    // Check if laser is out of bounds
    sf::Vector2f pos = getPosition();
    sf::FloatRect globalBounds = getGlobalBounds();

    if (pos.y + globalBounds.height < bounds.top ||
        pos.y - globalBounds.height > bounds.top + bounds.height ||
        pos.x + globalBounds.width < bounds.left ||
        pos.x - globalBounds.width > bounds.left + bounds.width)
    {
        active = false;
    }
}

/**
 * @brief: Check if laser is still active (within bounds)
 * 
 * @return true is active
 * @return false is not active
 */
bool ECE_LaserBlast::isActive() const
{
    return active;
}

/**
 * @brief: Set screen boundaries for collision detection
 * 
 * @param screenBounds bounds to set to
 */
void ECE_LaserBlast::setBounds(sf::FloatRect screenBounds)
{
    bounds = screenBounds;
}

/**
 * @brief: Check collision with another sprite
 * 
 * @param other other sprite to check against
 * @return true is colliding
 * @return false is not colliding
 */
bool ECE_LaserBlast::checkCollision(const sf::Sprite& other) const
{
    return checkCollision(other.getGlobalBounds());
}

/**
 * @brief: Check collision with a rectangular area
 * 
 * @param rect rectangle to check against
 * @return true is colliding
 * @return false is not colliding
 */
bool ECE_LaserBlast::checkCollision(const sf::FloatRect& rect) const
{
    if (!active)
    {
        return false;
    }

    return getGlobalBounds().intersects(rect);
}

/**
 * @brief: Deactivate the laser (for cleanup)
 * 
 */
void ECE_LaserBlast::deactivate()
{
    active = false;
}

/**
 * @brief: Get current velocity
 * 
 * @return sf::Vector2f velocity of laser
 */
sf::Vector2f ECE_LaserBlast::getVelocity() const
{
    return velocity;
}
/**
 * @brief: Get direction
 * 
 * @return ECE_LaserBlast::Direction direction laser is traveling
 */
ECE_LaserBlast::Direction ECE_LaserBlast::getDirection() const
{
    return direction;
}