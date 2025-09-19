/**
 * Auther: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: ECE_Buzzy class implementation
 * This class represents the player-controlled character "Buzzy" in the game.
 * It handles movement, rendering, collision detection, and health management.
 * 
 * Details:
 * The ECE_Buzzy class inherits from sf::Sprite and encapsulates the behavior
 * and properties of the player character. It includes methods for loading textures,
 * resizing, centering, handling input, updating position and animation, checking
 * collisions, responding to collisions, taking damage, and resetting state.
 * The class also manages the character's health and alive status, providing
 * visual feedback when damaged.
 */

#include <iostream>
#include <cmath>
#include <memory>

#include "ECE_Buzzy.hpp"
#include "params.hpp"

/**
 * @brief Construct a new ece buzzy::ece buzzy object
 * 
 * @param texture 
 */
ECE_Buzzy::ECE_Buzzy(const sf::Texture& texture)
    : velocity(0.0f, 0.0f)
    , maxVelocity(200.0f, 200.0f)
    , acceleration(500.0f)
    , isAlive(true)
    , health(3)
    , maxHealth(3)
    , bounds(0, 0, WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX)
{
    // set the texture
    setTexture(texture);

    // set size
    resizeSprite(BUZZY_WIDTH_PX, BUZZY_HEIGHT_PX);

    // Center the sprite origin
    centerSprite();
}

/**
 * @brief Destroy the ece buzzy::ece buzzy object
 * 
 */
ECE_Buzzy::~ECE_Buzzy()
{
    // SFML handles texture cleanup 
}

/**
 * @brief loadTexture from specified texturePath
 * 
 * @param texturePath std::string& path to the texture
 * @return true 
 * @return false 
 */
bool ECE_Buzzy::loadTexture(const std::string& texturePath)
{
    if (!buzzyTexture.loadFromFile(texturePath))
    {
        std::cerr << "Error loading Buzzy texture from: " << texturePath << std::endl;
        return false;
    }

    setTexture(buzzyTexture);

    // set size
    resizeSprite(BUZZY_WIDTH_PX, BUZZY_HEIGHT_PX);

    // Center the sprite origin
    centerSprite();

    return true;
}

/**
 * @brief Resize the sprite to specified width and height
 * 
 * @param widthPx Width to size to
 * @param heightPx Height to size to
 */
void ECE_Buzzy::resizeSprite(const float widthPx, const float heightPx)
{
    sf::FloatRect textureRect = getLocalBounds();
    setScale(widthPx / textureRect.width, heightPx / textureRect.height);
}

/**
 * @brief Center the sprite to make moving easier
 * 
 */
void ECE_Buzzy::centerSprite()
{
    sf::FloatRect textureRect = getLocalBounds();
    setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}


/**
 * @brief Set screen boundaries for collision detection
 * 
 * @param screenBounds 
 */
void ECE_Buzzy::setBounds(const sf::FloatRect& screenBounds)
{
    bounds = screenBounds;
}

/**
 * @brief Handle Keyboard input
 * 
 */
void ECE_Buzzy::handleInput()
{
    sf::Vector2f inputVelocity(0.0f, 0.0f);

    // Check for keyboard input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        inputVelocity.x = -maxVelocity.x;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        inputVelocity.x = maxVelocity.x;
    }

    velocity = inputVelocity;
}

/**
 * @brief Run updates for the buzzy object
 * 
 * @param deltaTime Time since the last update
 */
void ECE_Buzzy::update(float deltaTime)
{
    if (!isAlive)
    {
        return;
    }

    handleInput();
    move(deltaTime);
    updateAnimation(deltaTime);
}

/**
 * @brief Reset the color to white
 * 
 */
void ECE_Buzzy::resetColor()
{
    setColor(sf::Color::White);
}

/**
 * @brief Update the animation effects of buzzy
 * 
 * @param deltaTime Time since last update
 */
void ECE_Buzzy::updateAnimation(float deltaTime)
{
    // Visual effects based on health
    if (health < maxHealth)
    {
        // Flash red when damaged
        float flashTime = animationClock.getElapsedTime().asSeconds();
        if (flashTime < PLAYER_DAMAGED_FLASH_TIME)
        {
            if (fmod(flashTime, 0.2f) < 0.1f)
            {
                setColor(sf::Color(255, 100, 100));
            }
            else
            {
                resetColor();
            }
        }
    }
}

/**
 * @brief Move the sprite and check boundaries
 * 
 * @param deltaTime Time since last update
 */
void ECE_Buzzy::move(float deltaTime)
{
    float originalY = getPosition().y;

    // Apply movement
    sf::Vector2f movement = velocity * deltaTime;
    movement.y = 0;
    sf::Sprite::move(movement);

    // Get current position and sprite bounds
    sf::Vector2f position = getPosition();
    sf::FloatRect spriteBounds = getGlobalBounds();

    // Check boundary collisions and adjust position
    if (spriteBounds.left < bounds.left)
    {
        position.x = bounds.left + spriteBounds.width / 2.0f;
    }
    else if (spriteBounds.left + spriteBounds.width > bounds.left + bounds.width)
    {
        position.x = bounds.left + bounds.width - spriteBounds.width / 2.0f;
    }

    position.y = originalY;

    setPosition(position);
}

/**
 * @brief Check collision with another sprite
 * 
 * @param other Sprite to check boundaries against
 * @return true 
 * @return false 
 */
bool ECE_Buzzy::checkCollision(const sf::Sprite& other) const
{
    return checkCollision(other.getGlobalBounds());
}

/**
 * @brief Check collision with a bounding rectangle
 * 
 * @param otherBounds Rectangle to compare boundaries against
 * @return true 
 * @return false 
 */
bool ECE_Buzzy::checkCollision(const sf::FloatRect& otherBounds) const
{
    return getBounds().intersects(otherBounds);
}

/**
 * @brief Handle collision response
 * 
 */
void ECE_Buzzy::onCollision()
{
    takeDamage();

    // TODO: Add visual feedback, or temporary invincibility
    // TODO: Idea: change color
}

/**
 * @brief Take damage and check if alive
 * 
 * @param damage Amount of damage to take
 */
void ECE_Buzzy::takeDamage(int damage)
{
    animationClock.restart();
    health -= damage;
    if (health <= 0)
    {
        isAlive = false;
        health = 0;
        setColor(sf::Color(255, 255, 255, 128)); // Semi-transparant
    }
}

/**
 * @brief Reset buzzy to initial state
 * 
 */
void ECE_Buzzy::reset()
{
    isAlive = true;
    health = 3;
    velocity = sf::Vector2f(0.0f, 0.0f);
    setColor(sf::Color::White); // Reset color

    // Reset position to center of screen
    setPosition(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

/**
 * @brief Draw sprite to the window
 * 
 * @param window Window to draw to
 */
void ECE_Buzzy::draw(sf::RenderWindow& window)
{
    if (isAlive)
    {
        window.draw(*this);
    }
}