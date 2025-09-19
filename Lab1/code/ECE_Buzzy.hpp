/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Definition of ECE_Buzzy class
 * 
 * Details: 
 * The ECE_Buzzy class represents the player-controlled character "Buzzy" in the game.
 * It handles movement, rendering, collision detection, and health management.
 * The class inherits from sf::Sprite and encapsulates the behavior
 * and properties of the player character. It includes methods for loading textures,
 * resizing, centering, handling input, updating position and animation, checking
 * collisions, responding to collisions, taking damage, and resetting state.
 * The class also manages the character's health and alive status, providing
 * visual feedback when damaged.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief: ECE_Buzzy class representing the player character "Buzzy"
 * 
 */
class ECE_Buzzy : public sf::Sprite
{
private:
    sf::Texture buzzyTexture;
    sf::Vector2f velocity;
    sf::Vector2f maxVelocity;
    float acceleration;
    bool isAlive;
    int health;
    int maxHealth;
    sf::FloatRect bounds;

    // Animation and visual effects
    sf::Clock animationClock;

public:
    // Constructors
    ECE_Buzzy() = delete;
    ECE_Buzzy(const sf::Texture& texture);

    // Destructor
    ~ECE_Buzzy();

    // Initialization
    bool loadTexture(const std::string& texturePath);
    void resizeSprite(const float width, const float height);
    void centerSprite();
    void setBounds(const sf::FloatRect& screenBounds);

    // Movement and updates
    void handleInput();
    void resetColor();
    void update(float deltaTime);
    void updateAnimation(float deltaTime);
    void move(float deltaTime);

    // Collision detection
    bool checkCollision(const sf::Sprite& other) const;
    bool checkCollision(const sf::FloatRect& bounds) const;
    template<typename T> bool checkCollision(const T& object) const;

    // Collision response
    void onCollision();
    void takeDamage(int damage = 1);
    void reset();

    // Getters
    sf::Vector2f getVelocity() const { return velocity; }
    bool getIsAlive() const { return isAlive; }
    int getHealth() const { return health; }
    sf::FloatRect getBounds() const { return getGlobalBounds(); }

    // Setters
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
    void setMaxVelocity(const sf::Vector2f& maxVel) { maxVelocity = maxVel; }
    void setAcceleration(float accel) { acceleration = accel; }
    void setHealth(int hp) { health = hp; }
    void setAlive(bool alive) { isAlive = alive; }

    // Display
    void draw(sf::RenderWindow& window);
};