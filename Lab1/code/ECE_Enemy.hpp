/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Definition of ECE_Enemy class
 * 
 * Details:
 * The ECE_Enemy class represents enemy entities in the game.
 * It handles movement patterns, health management, shooting capabilities,
 * collision detection, and visual effects such as damage feedback and explosions.
 * The class inherits from sf::Sprite and encapsulates various behaviors and properties
 * of different enemy types.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>

/**
 * @brief: ECE_Enemy class representing enemy entities
 * 
 */
class ECE_Enemy : public sf::Sprite
{
public:
    enum EnemyType
    {
        BASIC,
        FAST,
        HEAVY,
        NORMAL,
        SHOOTER
    };

    enum EnemyState
    {
        ALIVE,
        HIT,
        DESTROYED,
        EXPLODING
    };

private:
    EnemyType type;
    EnemyState state;
    sf::Vector2f velocity;
    sf::Vector2f originalPosition;

    // Health and scoring
    int health;
    int maxHealth;
    int pointValue;

    // Movement pattern
    float moveSpeed;
    float amplitude;    // For oscillating movement
    float frequency;    // For oscillating movement
    float timeAlive;    // Time since creation

    // Shooting capability
    bool canShoot;
    float shootCooldown;
    float timeSinceLastShot;

    // Animation and visual effects
    sf::Clock animationClock;
    float explosionTimer;
    const float explosionDuration = 0.5f;

    // Collision bounds
    sf::FloatRect screenBounds;

    // Callback for when enemy is destroyed
    std::function<void(ECE_Enemy*, int)> onDestroyedCallback;
    std::function<void(sf::Vector2f, sf::Vector2f)> onShootCallback;

public:
    // Constructors
    ECE_Enemy(const sf::Texture& texture, EnemyType enemyType = BASIC);
    ECE_Enemy(const sf::Texture& texture, sf::Vector2f position, EnemyType enemyType = BASIC);

    // Destructor
    ~ECE_Enemy() = default;

    // Initialization
    void initialize(sf::Vector2f startPosition);
    void setEnemyType(EnemyType enemyType);
    void resizeSprite(const float widthPx, const float heightPx);
    void centerSprite();

    // Update methods
    void update(float deltaTime);
    void updateMovement(float deltaTime);
    void updateShooting(float deltaTime);
    void updateAnimation(float deltaTime);

    // Movement patterns
    void setLinearMovement(sf::Vector2f vel);
    void setOscillatingMovement(float speed, float amp, float freq);
    void setSpiralMovement(float speed, float radius, float angularSpeed);

    // Combat methods
    void takeDamage(int damage = 1);
    void destroy();
    bool canFire() const;
    void resetShootCooldown();

    // Collision detection
    bool checkCollision(const sf::Sprite& other) const;
    bool checkCollision(const sf::FloatRect& bounds) const;
    bool isOutOfBounds() const;
    bool isTouchingRight() const;
    bool isTouchingLeft() const;
    bool isTouchingTop() const;
    bool isTouchingBottom() const;

    // Getters
    EnemyType getType() const;
    EnemyState getState() const;
    int getHealth() const;
    int getMaxHealth() const;
    int getPointValue() const;
    sf::Vector2f getVelocity() const;
    bool getCanShoot() const;
    float getTimeAlive() const;

    // Setters
    void setScreenBounds(sf::FloatRect bounds);
    void setHealth(int hp);
    void setPointValue(int points);
    void setMoveSpeed(float speed);
    void setShootingCapability(bool canShoot, float cooldown = 2.0f);
    void setOnDestroyedCallback(std::function<void(ECE_Enemy*, int)> callback);
    void setOnShootCallback(std::function<void(sf::Vector2f, sf::Vector2f)> callback);

    // Utility methods
    void reset(sf::Vector2f newPosition);
    sf::FloatRect getCollisionBounds() const;
    void draw(sf::RenderWindow& window);

private:
    void resetColor();
    void setupEnemyStats();
    void handleExplosion(float deltaTime);
};