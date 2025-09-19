/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Implementation of ECE_Enemy class
 * 
 * Details:
 * The ECE_Enemy class represents enemy entities in the game.
 * It handles movement patterns, health management, shooting capabilities,
 * collision detection, and visual effects such as damage feedback and explosions.
 * The class inherits from sf::Sprite and encapsulates various behaviors and properties
 * of different enemy types.
 */

#include <cmath>
#include <iostream>

#include "ECE_Enemy.hpp"
#include "params.hpp"

/**
 * @brief: Construct a new ece enemy::ece enemy object
 * 
 * @param texture texture to use
 * @param enemyType what type of enemy
 */
ECE_Enemy::ECE_Enemy(const sf::Texture& texture, EnemyType enemyType)
    : type(enemyType)
    , state(ALIVE)
    , velocity(0, 0)
    , health(1)
    , maxHealth(1)
    , pointValue(10)
    , moveSpeed(50.0f)
    , amplitude(0)
    , frequency(0)
    , timeAlive(0)
    , canShoot(false)
    , shootCooldown(2.0f)
    , timeSinceLastShot(0)
    , explosionTimer(0)
{
    setTexture(texture);
    setupEnemyStats();

    // resize sprite
    resizeSprite(ENEMY_WIDTH_PX, ENEMY_HEIGHT_PX);

    // Center the sprite origin for better collision detection and movement
    centerSprite();
}

/**
 * @brief: Construct a new ece enemy::ece enemy object
 * 
 * @param texture texture to use
 * @param position position to start at
 * @param enemyType what type of enemy
 */
ECE_Enemy::ECE_Enemy(const sf::Texture& texture, sf::Vector2f position, EnemyType enemyType)
    : ECE_Enemy(texture, enemyType)
{
    setPosition(position);
    originalPosition = position;
}

/**
 * @brief: Initialize enemy properties and state
 * 
 * @param startPosition position to start the enemy at
 */
void ECE_Enemy::initialize(sf::Vector2f startPosition)
{
    setPosition(startPosition);
    originalPosition = startPosition;
    state = ALIVE;
    health = maxHealth;
    timeAlive = 0;
    timeSinceLastShot = 0;
    explosionTimer = 0;
}

/**
 * @brief: Resize the sprite to specified dimensions
 * 
 * @param widthPx Width to size to
 * @param heightPx Height to size to 
 */
void ECE_Enemy::resizeSprite(const float widthPx, const float heightPx)
{
    sf::FloatRect textureRect = getLocalBounds();
    setScale(widthPx / textureRect.width, heightPx / textureRect.height);
}

/**
 * @brief: Center the sprite origin for easier transformations
 * 
 */
void ECE_Enemy::centerSprite()
{
    sf::FloatRect rect = getLocalBounds();
    setOrigin(rect.width / 2.0f, rect.height / 2.0f);
}

/**
 * @brief: Set the enemy type and configure stats accordingly
 * 
 * @param enemyType 
 */
void ECE_Enemy::setEnemyType(EnemyType enemyType)
{
    type = enemyType;
    setupEnemyStats();
}

/**
 * @brief: Reset the sprite color based on enemy type
 * 
 */
void ECE_Enemy::resetColor()
{
    switch (type)
    {
        case BASIC:
            setColor(sf::Color::White);
            break;

        // TODO add move types
        case FAST:
            setColor(sf::Color::Yellow);
            break;
            
        case HEAVY:
            setColor(sf::Color::Red);
            break;

        case NORMAL:
            setColor(sf::Color::White);
            break;

        case SHOOTER:
            setColor(sf::Color::Cyan);
            break;
    }
}

/**
 * @brief: Setup enemy stats based on type
 * 
 */
void ECE_Enemy::setupEnemyStats()
{
    switch (type)
    {
        case BASIC:
            health = maxHealth = 1;
            pointValue = 10;
            moveSpeed = 50.0f;
            canShoot = false;
            break;

        case FAST:
            health = maxHealth = 1;
            pointValue = 20;
            moveSpeed = 100.0f;
            canShoot = false;
            break;
            
        case HEAVY:
            health = maxHealth = 3;
            pointValue = 50;
            moveSpeed = 25.0;
            canShoot = false;
            break;

        case NORMAL:
            health = maxHealth = 1;
            pointValue = 10;
            moveSpeed = 50.0f;
            canShoot = true;
            break;

        case SHOOTER:
            health = maxHealth = 2;
            pointValue = 30;
            moveSpeed = 40.0f;
            canShoot = true;
            shootCooldown = 2.0f;
            break;
    }

    resetColor();
}

/**
 * @brief: Update enemy state
 * 
 * @param deltaTime time since last update
 */
void ECE_Enemy::update(float deltaTime)
{
    if (state == DESTROYED)
    {
        return;
    }

    timeAlive += deltaTime;

    if (state == EXPLODING)
    {
        handleExplosion(deltaTime);
        return;
    }

    if (state == ALIVE || state == HIT)
    {
        updateMovement(deltaTime);
        updateShooting(deltaTime);
        updateAnimation(deltaTime);

        // Reset HIT state after brief period
        if (state == HIT && animationClock.getElapsedTime().asSeconds() > 1.0f)
        {
            state = ALIVE;
            animationClock.restart();
        }
    }
}

/**
 * @brief: Update enemy movement based on velocity and patterns
 * 
 * @param deltaTime time since last update
 */
void ECE_Enemy::updateMovement(float deltaTime)
{
    // Apply velocity-based movement
    move(velocity * deltaTime);

    // If enemy hits the side wall, shift up and reverse horizontal direction
    if (isTouchingRight() || isTouchingLeft())
    {
        setPosition(getPosition().x, getPosition().y - ENEMY_MOVEMENT_ON_WALL_HIT);
        velocity.x = -velocity.x;
    }

    // Add oscillating movement if configured, only applies to certain enemy types
    if (amplitude > 0 && frequency > 0)
    {
        float oscillation = amplitude * sin(frequency * timeAlive);
        setPosition(getPosition().x, originalPosition.y + oscillation);
    }
}

/**
 * @brief: Update shooting logic
 * 
 * @param deltaTime time since last update
 */
void ECE_Enemy::updateShooting(float deltaTime)
{
    if (!canShoot)
    {
        return;
    }

    timeSinceLastShot += deltaTime;

    if (canFire() && onShootCallback)
    {
        sf::Vector2f shootDirection(0, 1); // Shoot downward
        onShootCallback(getPosition(), shootDirection);
        resetShootCooldown();
    }
}

/**
 * @brief: Update animation effects such as damage feedback
 * 
 * @param deltaTime time since last update
 */
void ECE_Enemy::updateAnimation(float deltaTime)
{
    // Visual effects based on health
    if (health < maxHealth)
    {
        // Flash red when damaged
        float flashTime = animationClock.getElapsedTime().asSeconds();
        if (flashTime < ENEMY_DAMAGED_FLASH_TIME)
        {
            // Turn enemy red briefly
            if (fmod(flashTime, 0.2f) < 0.1f)
            {
                setColor(sf::Color(255, 100, 100));
            }
            // Turn off red flash
            else
            {
                resetColor();
            }
        }
    }
}

/**
 * @brief: Handle explosion animation and transition to destroyed state
 * 
 * @param deltaTime time since last update
 */
void ECE_Enemy::handleExplosion(float deltaTime)
{
    explosionTimer += deltaTime;

    // Flash and fade during explosion
    float alpha = 255 * (1.0f - explosionTimer / explosionDuration);
    sf::Color color = getColor();
    color.a = static_cast<sf::Uint8>(std::max(0.0f, alpha));
    setColor(color);

    // Scale effect
    float scale = 1.0f + (explosionTimer / explosionDuration) * ENEMY_EXPLODING_SCALE;
    float explodingWidth = ENEMY_WIDTH_PX * scale;
    float explodingHeight = ENEMY_HEIGHT_PX * scale;
    resizeSprite(explodingWidth, explodingHeight);

    if (explosionTimer >= explosionDuration)
    {
        state = DESTROYED;
    }
}

/**
 * @brief: Set linear movement velocity
 * 
 * @param vel velocity to set to
 */
void ECE_Enemy::setLinearMovement(sf::Vector2f vel)
{
    velocity = vel;
    amplitude = 0;
    frequency = 0;
}

/**
 * @brief: Set oscillating movement parameters
 * 
 * @param speed speed of oscillation
 * @param amp ampltitude of oscillation
 * @param freq frequency of oscillation
 */
void ECE_Enemy::setOscillatingMovement(float speed, float amp, float freq)
{
    velocity.x = speed;
    velocity.y = 0;
    amplitude = amp;
    frequency = freq;
}

/**
 * @brief: Update state when taking damage
 * 
 * @param damage how much damage to take 
 */
void ECE_Enemy::takeDamage(int damage)
{
    if (state == DESTROYED || state == EXPLODING)
    {
        return;
    }

    animationClock.restart();
    health -= damage;
    state = HIT;
    animationClock.restart();

    if (health <= 0)
    {
        destroy();
    }
}

/**
 * @brief: Handle enemy destruction and transition to exploding state
 * 
 */
void ECE_Enemy::destroy()
{
    state = EXPLODING;
    explosionTimer = 0;

    if (onDestroyedCallback)
    {
        onDestroyedCallback(this, pointValue);
    }
}

/**
 * @brief: Check if enemy can fire
 * 
 * @return true 
 * @return false 
 */
bool ECE_Enemy::canFire() const
{
    return canShoot && timeSinceLastShot >= shootCooldown && state != DESTROYED;
}

/**
 * @brief: Reset shooting cooldown timer
 * 
 */
void ECE_Enemy::resetShootCooldown()
{
    timeSinceLastShot = 0;
}

/**
 * @brief: Check collision with another sprite
 * 
 * @param other other sprite to check against
 * @return true is colliding
 * @return false is not colliding
 */
bool ECE_Enemy::checkCollision(const sf::Sprite& other) const
{
    return checkCollision(other.getGlobalBounds());
}

/**
 * @brief: Check collision with bounding rectangle
 * 
 * @param bounds rectangle to check against
 * @return true is colliding
 * @return false is not colliding
 */
bool ECE_Enemy::checkCollision(const sf::FloatRect& bounds) const
{
    if (state != ALIVE && state != HIT)
    {
        return false;
    }

    return getGlobalBounds().intersects(bounds);
}

/**
 * @brief: Check if enemy is out of screen bounds
 * 
 * @return true is not touching a boundary
 * @return false is touching a boundary
 */
bool ECE_Enemy::isOutOfBounds() const
{
    sf::FloatRect bounds = getGlobalBounds();
    return isTouchingRight() ||
           isTouchingLeft() ||
           isTouchingTop() ||
           isTouchingBottom();
}

/**
 * @brief: Check if enemy is touching the right screen boundary
 * 
 * @return true is touching right
 * @return false is not touching right
 */
bool ECE_Enemy::isTouchingRight() const
{
    sf::FloatRect bounds = getGlobalBounds();
    return bounds.left + bounds.width > screenBounds.left + screenBounds.width;
}

/**
 * @brief: Check if enemy is touching the left screen boundary
 * 
 * @return true is touching left
 * @return false is not touching left
 */
bool ECE_Enemy::isTouchingLeft() const
{
    sf::FloatRect bounds = getGlobalBounds();
    return bounds.left < screenBounds.left;
}

/**
 * @brief: Check if enemy is touching the top screen boundary
 * 
 * @return true is touching top
 * @return false is not touching top
 */
bool ECE_Enemy::isTouchingTop() const
{
    sf::FloatRect bounds = getGlobalBounds();
    return bounds.top < screenBounds.top;
}

/**
 * @brief: Check if enemy is touching the bottom screen boundary
 * 
 * @return true enemy touching bottom
 * @return false enemy not touching bottom
 */
bool ECE_Enemy::isTouchingBottom() const
{
    sf::FloatRect bounds = getGlobalBounds();
    return bounds.top + bounds.height > screenBounds.top + screenBounds.height;
}

// Getters
/**
 * @brief: Get the type of enemy
 * 
 * @return ECE_Enemy::EnemyType type of enemy
 */
ECE_Enemy::EnemyType ECE_Enemy::getType() const
{
    return type;
}

/**
 * @brief: Get the state of enemy
 * 
 * @return ECE_Enemy::EnemyState current state of enemy
 */
ECE_Enemy::EnemyState ECE_Enemy::getState() const
{
    return state;
}

/**
 * @brief: Get the current health of enemy
 * 
 * @return int current health of enemy
 */
int ECE_Enemy::getHealth() const
{
    return health;
}

/**
 * @brief: Get the maximum health of enemy
 * 
 * @return int maximum health of enenmy
 */
int ECE_Enemy::getMaxHealth() const
{
    return maxHealth;
}

/**
 * @brief: Get the point value of enemy
 * 
 * @return int point value of enemy
 */
int ECE_Enemy::getPointValue() const
{
    return pointValue;
}

/**
 * @brief: Get the current velocity of enemy
 * 
 * @return sf::Vector2f velocity of enemy
 */
sf::Vector2f ECE_Enemy::getVelocity() const
{
    return velocity;
}

/**
 * @brief: Get if enemy can shoot
 * 
 * @return true can shoot
 * @return false can't shoot
 */
bool ECE_Enemy::getCanShoot() const
{
    return canShoot;
}

/**
 * @brief: Get how long the enemy has been alive
 * 
 * @return float time alive in seconds
 */
float ECE_Enemy::getTimeAlive() const
{
    return timeAlive;
}

// Setters
/**
 * @brief: Set the screen bounds for collision detection
 * 
 * @param bounds bounds to set to
 */
void ECE_Enemy::setScreenBounds(sf::FloatRect bounds)
{
    screenBounds = bounds;
}

/**
 * @brief: Set the health of enemy
 * 
 * @param hp how much hp the enmy should have
 */
void ECE_Enemy::setHealth(int hp)
{
    health = maxHealth = hp;
}

/**
 * @brief: Set the point value of enemy
 * 
 * @param points how many points to set to
 */
void ECE_Enemy::setPointValue(int points)
{
    pointValue = points;
}

/**
 * @brief: Set the movement speed of enemy
 * 
 * @param speed how fast to move
 */
void ECE_Enemy::setMoveSpeed(float speed)
{
    moveSpeed = speed;
}

/**
 * @brief: Set shooting capability of enemy
 * 
 * @param canShootParam set if can shoot
 * @param cooldown set cooldown time
 */
void ECE_Enemy::setShootingCapability(bool canShootParam, float cooldown)
{
    canShoot = canShootParam;
    shootCooldown = cooldown;
}

/**
 * @brief: Set callback for when enemy is destroyed
 * 
 * @param callback callback function when destroyed
 */
void ECE_Enemy::setOnDestroyedCallback(std::function<void(ECE_Enemy*, int)> callback)
{
    onDestroyedCallback = callback;
}

/**
 * @brief: Set callback for when enemy shoots
 *  
 * @param callback callback function when shooting
 */
void ECE_Enemy::setOnShootCallback(std::function<void(sf::Vector2f, sf::Vector2f)> callback)
{
    onShootCallback = callback;
}

/**
 * @brief: Reset enemy to initial state
 * 
 * @param newPosition position to reset to
 */
void ECE_Enemy::reset(sf::Vector2f newPosition)
{
    setPosition(newPosition);
    originalPosition = newPosition;
    state = ALIVE;
    health = maxHealth;
    timeAlive = 0;
    timeSinceLastShot = 0;
    explosionTimer = 0;
    setScale(1.0f, 1.0f);
    setupEnemyStats(); // Reset color and stats
}

/**
 * @brief: Get the collision bounds of the enemy
 * 
 * @return sf::FloatRect 
 */
sf::FloatRect ECE_Enemy::getCollisionBounds() const
{
    return getGlobalBounds();
}

/**
 * @brief: Draw the enemy to the window
 * 
 * @param window window to draw to
 */
void ECE_Enemy::draw(sf::RenderWindow& window)
{
    if (state != DESTROYED)
    {
        window.draw(*this);
    }
}