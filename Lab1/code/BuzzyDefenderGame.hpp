/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Declaration of BuzzyDefenderGame class
 * 
 * Details:
 * The BuzzyDefenderGame class manages the overall game state and logic for a Buzzy Defender style game.
 * It handles initialization, loading resources, the main game loop, event handling,
 * rendering, updating game objects, collision detection, and game state transitions.
 * The class utilizes SFML for graphics and input handling, and incorporates various game entities
 * such as the player character, enemies, and laser blasts.
 * I decided to implement the game state management using a class instead of implementing it in the main function.
 * It provided more learning opportunities for classes and I thought kept the code neater.
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <memory>
#include <iostream>
#include <random>

#include "ECE_Buzzy.hpp"
#include "ECE_LaserBlast.hpp"
#include "ECE_Enemy.hpp"

class BuzzyDefenderGame
{
public:
    enum GameState 
    {
        START_SCREEN,
        PLAYING,
        GAME_OVER,
        GAME_WON,
        PAUSED
    };

private:
    sf::RenderWindow window;
    sf::Clock gameClock;
    sf::Clock enemySpawnClock;
    sf::Clock shootClock;
    sf::Clock enemyShootClock;

    // Game state
    GameState currentState;

    // Start screen
    sf::Texture startScreenTexture;
    sf::Sprite startScreenSprite;
    sf::Font font;
    sf::Text instructionText;
    bool startScreenLoaded;

    // Game objects
    std::unique_ptr<ECE_Buzzy> player;
    std::list<std::unique_ptr<ECE_LaserBlast>> playerLasers;
    std::list<std::unique_ptr<ECE_LaserBlast>> enemyLasers;
    std::vector<std::unique_ptr<ECE_Enemy>> enemies;

    // Textures
    sf::Texture playerTexture;
    sf::Texture enemyTexture;
    sf::Texture laserGreenTexture;
    sf::Texture laserRedTexture;

    std::vector<sf::Texture> enemyTextures;

    // Fonts
    sf::Font gameFont;

    // Game state
    int score;
    int level;

    // Screen bounds
    sf::FloatRect screenBounds;

    // Random number generator
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> xPosDist;

public:
    // Constructor
    BuzzyDefenderGame();

    // Destructor
    ~BuzzyDefenderGame() = default;

    // Initialization
    bool initialize();

    // Loading functions
    bool loadStartScreen();
    bool loadGameTextures();
    bool loadEnemyTextures();
    void initializeGameObjects();
    void setupEnemiesLvl1();

    // Runtime
    void run();

private:
    // Game functions
    void handleEvents();
    void handleKeyPress(sf::Keyboard::Key key);
    void handleStartScreenInput(sf::Keyboard::Key key);
    void handleGameInput(sf::Keyboard::Key key);
    void handleGameOverInput(sf::Keyboard::Key key);
    void handleGameWonInput(sf::Keyboard::Key key);
    void handlePausedInput(sf::Keyboard::Key key);

    void render();
    void renderStartScreen();
    void renderGame();
    void renderGameOver();
    void renderGameWon();
    void renderPaused();
    void updateTitle(const std::string& title);
    
    void update();
    void playerShoot();
    void spawnEnemy(const sf::Texture& texture, const float spawnX, const float spawnY, ECE_Enemy::EnemyType type);
    void spawnEnemies();
    void createEnemyLaser(sf::Vector2f position, sf::Vector2f direction);
    void updatePlayerLasers(float deltaTime);
    void updateEnemyLasers(float deltaTime);
    void updateEnemies(float deltaTime);

    const sf::Texture& getRandomEnemyTexture();

    // Other game functions
    void checkCollisions();
    void checkGameOver();
    void cleanup();
    void restartGame();

    void drawUI();
};