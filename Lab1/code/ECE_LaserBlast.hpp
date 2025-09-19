/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Definition of ECE_LaserBlast class
 * 
 * Details:
 * The ECE_LaserBlast class represents laser blasts fired by the player or enemies.
 * It handles movement, rendering, collision detection, and active state management.
 * The class inherits from sf::Sprite and encapsulates the behavior and properties
 * of laser blasts. It includes methods for loading textures, resizing, centering,
 * updating position, checking collisions, and deactivating the laser when it goes
 * out of bounds.
 */

#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief: ECE_LaserBlast class representing laser blasts
 * 
 */
class ECE_LaserBlast : public sf::Sprite 
{
public:
    enum Direction 
    {
        UP = -1,
        DOWN = 1
    };

private: 
    sf::Vector2f velocity;
    Direction direction;
    float speed;
    bool active;
    sf::FloatRect bounds; // Screen boundaries

public:
    // Constructor
    ECE_LaserBlast(const sf::Texture& texture, sf::Vector2f startPosition,
                   Direction dir, float laserSpeed = 300.0f);

    // Destructor
    ~ECE_LaserBlast() = default;

    void resizeSprite(const float width, const float height);

    void centerSprite();

    // Update laser position
    void update(float deltaTime);

    // Check if laser is still active (within bounds)
    bool isActive() const;

    // Set screen boundaries for collision detection
    void setBounds(sf::FloatRect screenBounds);

    // Check collision with another sprite
    bool checkCollision(const sf::Sprite& other) const;

    // Check collision with a rectangular area
    bool checkCollision(const sf::FloatRect& bounds) const;

    // Deactivate the laser (for cleanup)
    void deactivate();

    // Get current velocity
    sf::Vector2f getVelocity() const;

    // Get direction
    Direction getDirection() const;
};