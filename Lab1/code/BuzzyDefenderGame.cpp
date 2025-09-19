/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Implementation of BuzzyDefenderGame class
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

#include "BuzzyDefenderGame.hpp"
#include "params.hpp"

/**
 * @brief: Construct a new Buzzy Defender Game:: Buzzy Defender Game object
 * 
 */
BuzzyDefenderGame::BuzzyDefenderGame()
    : window(sf::VideoMode(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), "Buzzy Defender - ECE Game")
    , currentState(START_SCREEN)
    , startScreenLoaded(false)
    , score(0)
    , level(1)
    , gen(rd())
    , xPosDist(X_POS_DIST_MARGIN, WINDOW_WIDTH_PX - X_POS_DIST_MARGIN)
    , screenBounds(0, 0, WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX)
{
    window.setFramerateLimit(60);
    initialize();
}

/**
 * @brief: Initialize the game
 * 
 * @return true is initialized successfully
 * @return false is not initialized successfully
 */
bool BuzzyDefenderGame::initialize()
{
    // Load start screen
    if (!loadStartScreen())
    {
        std::cerr << "Failed to load start screen" << std::endl;
        return false;
    }

    if (!loadGameTextures())
    {
        std::cerr << "Failed to load game textures" << std::endl;
        return false;
    }

    if (!gameFont.loadFromFile("assets/fonts/DejaVuSans.ttf"))
    {
        std::cerr << "Failed to load game textures" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief: Load the start screen texture and setup sprite
 * 
 * @return true loaded successfully
 * @return false did not load successfully
 */
bool BuzzyDefenderGame::loadStartScreen()
{
    // Try to load start screen image
    if (!startScreenTexture.loadFromFile("assets/graphics/Start_Screen.png"))
    {
        return false;
    }

    startScreenSprite.setTexture(startScreenTexture);

    // Scale the start screen to fit window if needed
    sf::Vector2u textureSize = startScreenTexture.getSize();
    sf::Vector2u windowSize = window.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    startScreenSprite.setScale(scaleX, scaleY);

    startScreenLoaded = true;
    return true;
}

/**
 * @brief: Load game textures
 * 
 * @return true loaded game textures
 * @return false did not load game textures
 */
bool BuzzyDefenderGame::loadGameTextures()
{
    // Load textures
    if (!playerTexture.loadFromFile("assets/graphics/Buzzy_blue.png"))
    {
        std::cerr << "Failed to load player texture" << std::endl;
        return false;
    }

    if (!loadEnemyTextures())
    {
        std::cerr << "Failed to load enemy textures" << std::endl;
        return false;
    }

    if (!laserGreenTexture.loadFromFile("assets/graphics/laser_green.png"))
    {
        std::cerr << "Failed to load laser_green texture" << std::endl;
        return false;
    }

    if (!laserRedTexture.loadFromFile("assets/graphics/laser_red.png"))
    {
        std::cerr << "Failed to load laser_red texture" << std::endl;
        return false;
    }

    if (!enemyTexture.loadFromFile("assets/graphics/clemson_tigers.png"))
    {
        std::cerr << "Failed to load asdgpoih" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief: Load enemy textures
 * 
 * @return true loaded enemy textures
 * @return false did not load enemy textures
 */
bool BuzzyDefenderGame::loadEnemyTextures()
{
    std::vector<std::string> textureLocations;
    textureLocations.push_back("assets/graphics/clemson_tigers.png");
    textureLocations.push_back("assets/graphics/bulldog.png");

    for (int ii = 0; ii < textureLocations.size(); ++ii)
    {
        std::string str = textureLocations.at(ii);
        std::cout << "Loading " << str << " texture" << std::endl;
        sf::Texture texture;
        if (!texture.loadFromFile(str))
        {
            std::cerr << "Failed to load " << str << " texture" << std::endl;
            return false;
        }
        enemyTextures.push_back(texture);
    }

    return true;
}

/**
 * @brief: Initialize game objects
 * 
 */
void BuzzyDefenderGame::initializeGameObjects()
{
    // Initialize player
    player = std::make_unique<ECE_Buzzy>(playerTexture);
    player->setBounds(screenBounds);
    player->setPosition(WINDOW_WIDTH_PX / 2.0f, BUZZY_SPAWN_Y_MARGIN_PX);

    // Clear any existing game objects
    playerLasers.clear();
    enemyLasers.clear();
    enemies.clear();

    // Reset game state
    score = 0;
    level = 1;

    // Reset clocks
    gameClock.restart();
    enemySpawnClock.restart();
    shootClock.restart();
    enemyShootClock.restart();

    // Setup enemies
    setupEnemiesLvl1();
}

/**
 * @brief: Setup enemies for level 1
 * 
 */
void BuzzyDefenderGame::setupEnemiesLvl1()
{
    enemies.clear();

    const int ENEMIES_PER_ROW = 10;
    const int NUM_ROWS = 2;
    const float ENEMY_SPACING_X = 70.0f;
    const float ENEMY_SPACING_Y = 70.0f;
    const float START_X = 100.0f;
    const float START_Y = WINDOW_HEIGHT_PX - ENEMY_SPAWN_Y_MARGIN_PX;

    // Setup 20 enemies in 2 rows of 10
    for (int row = 0; row < NUM_ROWS; row++)
    {
        for (int col = 0; col < ENEMIES_PER_ROW; col++)
        {
            // Calculate position for this enemy
            float xPos = START_X + (col * ENEMY_SPACING_X);
            float yPos = START_Y - (row * ENEMY_SPACING_Y);

            sf::Vector2f enemyPosition(xPos, yPos);

            ECE_Enemy::EnemyType type = ECE_Enemy::EnemyType::NORMAL;
            spawnEnemy(getRandomEnemyTexture(), xPos, yPos, type);
        }
    }
}

/**
 * @brief: Start the main game loop
 * 
 */
void BuzzyDefenderGame::run()
{
    while (window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
}

/**
 * @brief: Handle SFML events
 * 
 */
void BuzzyDefenderGame::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            handleKeyPress(event.key.code);
        }
    }
}

/**
 * @brief: Handle key press based on current game state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handleKeyPress(sf::Keyboard::Key key)
{
    switch (currentState)
    {
        case START_SCREEN:
            handleStartScreenInput(key);
            break;

        case PLAYING:
            handleGameInput(key);
            break;

        case GAME_OVER:
            handleGameOverInput(key);
            break;

        case GAME_WON:
            handleGameWonInput(key);
            break;

        case PAUSED:
            handlePausedInput(key);
            break;
    }
}

/**
 * @brief: Handle input when in start screen state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handleStartScreenInput(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Enter:
            // Transition to game
            currentState = PLAYING;
            initializeGameObjects();
            break;

        case sf::Keyboard::Escape:
            window.close();
            break;

        default:
            break;
    }
}

/**
 * @brief: Handle input when in playing state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handleGameInput(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Space:
            playerShoot();
            break;

        case sf::Keyboard::P:
            currentState = PAUSED;
            break;

        case sf::Keyboard::Escape:
            // Return to start screen or quit
            if (startScreenLoaded)
            {
                currentState = START_SCREEN;
            }
            else
            {
                window.close();
            }
            break;

        default:
            break;
    }
}

/**
 * @brief: Handle input when in game over state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handleGameOverInput(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::R:
        case sf::Keyboard::Enter:
            // Restart game
            currentState = PLAYING;
            initializeGameObjects();
            break;

        case sf::Keyboard::Escape:
            // Return to start screen or quit
            if (startScreenLoaded)
            {
                currentState = START_SCREEN;
            }
            else
            {
                window.close();
            }
            break;

        default:
            break;
    }
}

/**
 * @brief: Handle input when in game won state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handleGameWonInput(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::R:
        case sf::Keyboard::Enter:
            // Restart game
            currentState = PLAYING;
            initializeGameObjects();
            break;

        case sf::Keyboard::Escape:
            // Return to start screen or quit
            if (startScreenLoaded)
            {
                currentState = START_SCREEN;
            }
            else
            {
                window.close();
            }
            break;

        default:
            break;
    }
}

/**
 * @brief: Handle input when in paused state
 * 
 * @param key key that was pressed
 */
void BuzzyDefenderGame::handlePausedInput(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::P:
        case sf::Keyboard::Enter:
            currentState = PLAYING;
            gameClock.restart(); // Reset time
            break;

        case sf::Keyboard::Escape:
            // Return to start screen or quit
            if (startScreenLoaded)
            {
                currentState = START_SCREEN;
            }
            else
            {
                window.close();
            }
            break;

        default:
            break;
    }
}

/**
 * @brief: Main update game loop
 * 
 */
void BuzzyDefenderGame::update()
{
    // ONly run update if state is PLAYGIN
    if (currentState != PLAYING)
    {
        return;
    }

    float deltaTime = gameClock.restart().asSeconds();

    // Update player
    if (player && player->getIsAlive())
    {
        player->update(deltaTime);
    }

    // Update Game stuff
    updatePlayerLasers(deltaTime);
    updateEnemyLasers(deltaTime);
    updateEnemies(deltaTime);

    // TODO: something fun to add if you want to spawn random enemies
    // I implemented this as something extra to try
    // Spawn enemies
    // spawnEnemies();

    // Check collisions
    checkCollisions();

    // Check game over conditions
    checkGameOver();

    // Clean up destroyed objects
    cleanup();
}

/**
 * @brief: Player shoot a laser if cooldown has elapsed
 * 
 */
void BuzzyDefenderGame::playerShoot()
{
    if (shootClock.getElapsedTime().asSeconds() >= PLAYER_SHOOT_COOLDOWN)
    {
        if (player && player->getIsAlive())
        {
            sf::Vector2f playerPos = player->getPosition();
            std::unique_ptr<ECE_LaserBlast> laser = std::make_unique<ECE_LaserBlast>(
                laserGreenTexture,
                sf::Vector2f(playerPos.x, playerPos.y + PLAYER_LASER_OFFSET_Y),
                ECE_LaserBlast::DOWN,
                PLAYER_LASER_SPEED
            );
            laser->setBounds(screenBounds);
            playerLasers.push_back(std::move(laser));
            shootClock.restart();
        }
    }
}

/**
 * @brief: Spawn an enemy at given position with given type
 * 
 * @param texture texture to use for enemy
 * @param spawnX x location to spawn to
 * @param spawnY y location to spawn to
 * @param type type of enemy to spawn
 */
void BuzzyDefenderGame::spawnEnemy(const sf::Texture& texture, const float spawnX, const float spawnY, ECE_Enemy::EnemyType type)
{
    if (enemies.size() < MAX_ENEMIES)
    {
        sf::Vector2f spawnPos(spawnX, spawnY);
        std::unique_ptr<ECE_Enemy> enemy = std::make_unique<ECE_Enemy>(texture, spawnPos, type);
        enemy->setScreenBounds(screenBounds);

        // Set movement pattern
        switch (type)
        {
            case ECE_Enemy::NORMAL:
                enemy->setLinearMovement(sf::Vector2f(30, 0));
                enemy->setOnShootCallback([this](sf::Vector2f pos, sf::Vector2f dir)
                {
                    createEnemyLaser(pos, dir);
                });
                break;
            case ECE_Enemy::BASIC:
                enemy->setLinearMovement(sf::Vector2f(0, -30));
                break;
            case ECE_Enemy::FAST:
                enemy->setLinearMovement(sf::Vector2f(20, -50));
                break;
            case ECE_Enemy::HEAVY:
                enemy->setLinearMovement(sf::Vector2f(0, -20));
                break;
            case ECE_Enemy::SHOOTER:
                enemy->setLinearMovement(sf::Vector2f(10, -25));
                enemy->setOnShootCallback([this](sf::Vector2f pos, sf::Vector2f dir)
                {
                    createEnemyLaser(pos, dir);
                });
                break;
        }

        enemies.push_back(std::move(enemy));
        enemySpawnClock.restart();
    }
}

/**
 * @brief: Get a random enemy texture from loaded textures
 * 
 * @return sf::Texture Texture to use for enemy
 */
const sf::Texture& BuzzyDefenderGame::getRandomEnemyTexture()
{
    if (enemyTextures.empty())
    {
        throw std::runtime_error("No enemy textures loaded");
    }
    int enemyTextureLocation = static_cast<int>(gen() % enemyTextures.size());
    return enemyTextures.at(enemyTextureLocation);
}

/**
 * @brief: Spawn enemies at random intervals and positions
 * 
 */
void BuzzyDefenderGame::spawnEnemies()
{
    if (enemySpawnClock.getElapsedTime().asSeconds() >= ENEMY_SPAWN_INTERVAL)
    {
        if (enemies.size() < MAX_ENEMIES)
        {
            // Random spawn position
            float xPos = xPosDist(gen);
            sf::Vector2f spawnPos(xPos, WINDOW_HEIGHT_PX - ENEMY_SPAWN_Y_MARGIN_PX);

            // Create enemy with random type
            ECE_Enemy::EnemyType type = static_cast<ECE_Enemy::EnemyType>(gen() % 4);
            spawnEnemy(getRandomEnemyTexture(), spawnPos.x, spawnPos.y, type);
        }
    }
}

/**
 * @brief: Create an enemy laser at given position and direction
 * 
 * @param position position to spawn laser at
 * @param direction direction of laser to travel
 */
void BuzzyDefenderGame::createEnemyLaser(sf::Vector2f position, sf::Vector2f direction)
{
    std::unique_ptr<ECE_LaserBlast> laser = std::make_unique<ECE_LaserBlast>(
        laserRedTexture,
        sf::Vector2f(position.x, position.y - 20),
        ECE_LaserBlast::UP,
        ENEMY_LASER_SPEED
    );
    laser->setBounds(screenBounds);
    enemyLasers.push_back(std::move(laser));
}

/**
 * @brief: Update player lasers
 * 
 * @param deltaTime time since last update
 */
void BuzzyDefenderGame::updatePlayerLasers(float deltaTime)
{
    for (std::unique_ptr<ECE_LaserBlast>& laser : playerLasers)
    {
        laser->update(deltaTime);
    }
}

/**
 * @brief: Update enemy lasers
 * 
 * @param deltaTime time since last update
 */
void BuzzyDefenderGame::updateEnemyLasers(float deltaTime)
{
    for (std::unique_ptr<ECE_LaserBlast>& laser : enemyLasers)
    {
        laser->update(deltaTime);
    }
}

/**
 * @brief: Update enemies
 * 
 * @param deltaTime time since last update
 */
void BuzzyDefenderGame::updateEnemies(float deltaTime)
{
    // Choose 1 random enemy to shoot
    // set their canShoot to true
    // set their shoot cooldown to LARGE_ENEMY_SHOOT_COOLDOWN to prevent them shooting until next update
    if (!enemies.empty() && enemyShootClock.getElapsedTime().asSeconds() >= ENEMY_SHOOT_COOLDOWN)
    {
        enemyShootClock.restart();
        // Reset all enemies to not be able to shoot
        for (std::unique_ptr<ECE_Enemy>& enemy : enemies)
        {
            enemy->setShootingCapability(false);
        }
        // set one enemy to be able to shoot
        int enemyToShoot = static_cast<int>(gen() % enemies.size());
        enemies.at(enemyToShoot)->setShootingCapability(true, ENEMY_SHOOT_COOLDOWN);
    }
    for (std::unique_ptr<ECE_Enemy>& enemy : enemies)
    {
        enemy->update(deltaTime);
    }
}

/**
 * @brief: Check collisions between game objects
 * 
 */
void BuzzyDefenderGame::checkCollisions()
{
    if (!player || !player->getIsAlive())
    {
        return;
    }

    // Player's lasers vrs enemies
    std::list<std::unique_ptr<ECE_LaserBlast>>::iterator playerLaserIt = playerLasers.begin();
    while (playerLaserIt != playerLasers.end())
    {
        if (!(*playerLaserIt)->isActive())
        {
            ++playerLaserIt;
            continue;
        }

        bool laserHit = false;
        for (std::unique_ptr<ECE_Enemy>& enemy : enemies)
        {
            if (enemy->getState() == ECE_Enemy::ALIVE ||
                enemy->getState() == ECE_Enemy::HIT)
            {
                if ((*playerLaserIt)->checkCollision(*enemy))
                {
                    enemy->takeDamage(1);
                    (*playerLaserIt)->deactivate();

                    // Add score
                    if (enemy->getState() == ECE_Enemy::DESTROYED ||
                        enemy->getState() == ECE_Enemy::EXPLODING)
                    {
                        score += enemy->getPointValue();
                    }

                    laserHit = true;
                    break;
                }
            }
        }
        ++playerLaserIt;
    }

    // Enemy lasers vrs player
    for (std::unique_ptr<ECE_LaserBlast>& laser : enemyLasers)
    {
        if (laser->isActive() && laser->checkCollision(*player))
        {
            player->onCollision();
            laser->deactivate();
        }
    }

    // Enemies vrs player (direct collision)
    for (std::unique_ptr<ECE_Enemy>& enemy : enemies)
    {
        if ((enemy->getState() == ECE_Enemy::ALIVE ||
             enemy->getState() == ECE_Enemy::HIT) &&
             enemy->checkCollision(*player))
        {
            player->onCollision();
            enemy->takeDamage(enemy->getHealth()); // Destroy enemy on contact
        }
    }
}

/**
 * @brief: Check game over conditions
 * 
 */
void BuzzyDefenderGame::checkGameOver()
{
    if (currentState == PLAYING)
    {
        // End game if player is dead
        if (player && !player->getIsAlive())
        {
            currentState = GAME_OVER;
        }

        // Or if enemies made it to the top
        for (const std::unique_ptr<ECE_Enemy>& enemy : enemies)
        {
            if (enemy->isTouchingTop())
            {
                currentState = GAME_OVER;
            }
        }

        // TODO Add win condition if you want
        // For example, if all enemies are destroyed
        if (enemies.empty())
        {
            // For now, just restart the level
            currentState = GAME_WON;
        }
    }
}

/**
 * @brief: Clean up inactive or destroyed game objects
 * 
 */
void BuzzyDefenderGame::cleanup()
{
    // Remove inactive player lasers
    playerLasers.remove_if([](const std::unique_ptr<ECE_LaserBlast>& laser)
    {
        return !laser->isActive();
    });

    // Remove inactive enemy lasers
    enemyLasers.remove_if([](const std::unique_ptr<ECE_LaserBlast>& laser)
    {
        return !laser->isActive();
    });

    // Remove destroyed enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<ECE_Enemy>& enemy)
            {
                return enemy->getState() == ECE_Enemy::DESTROYED;
            }),
        enemies.end()
    );
}

/**
 * @brief: Render the game based on current state
 * 
 */
void BuzzyDefenderGame::render()
{
    window.clear(sf::Color::Black);

    switch (currentState)
    {
        case START_SCREEN:
            renderStartScreen();
            break;

        case PLAYING:
            renderGame();
            break;

        case GAME_OVER:
            renderGameOver();
            break;

        case GAME_WON:
            renderGameWon(); 
            break;

        case PAUSED:
            renderPaused();
            break;
    }

    window.display();
}

/**
 * @brief: Render the start screen
 * 
 */
void BuzzyDefenderGame::renderStartScreen()
{
    window.draw(startScreenSprite);
    window.draw(instructionText);

    updateTitle("Buzzy Defender - Press ENTER to Start");
}

/**
 * @brief: Render the main game
 * 
 */
void BuzzyDefenderGame::renderGame()
{
    // Draw player
    if (player && player->getIsAlive())
    {
        player->draw(window);
    }

    // Draw player lasers
    for (const std::unique_ptr<ECE_LaserBlast>& laser : playerLasers)
    {
        if (laser->isActive())
        {
            window.draw(*laser);
        }
    }

    // Draw enemy lasers
    for (const std::unique_ptr<ECE_LaserBlast>& laser : enemyLasers)
    {
        if (laser->isActive())
        {
            window.draw(*laser);
        }
    }

    // Draw enemies
    for (const std::unique_ptr<ECE_Enemy>& enemy : enemies)
    {
        enemy->draw(window);
    }

    std::string title = "Buzzy Defender - Score: " + std::to_string(score) + 
                        " Health: " + std::to_string(player ? player->getHealth() : 0);
    updateTitle(title);
}

/**
 * @brief: Render the game over screen
 * 
 */
void BuzzyDefenderGame::renderGameOver()
{
    // Still show the game background, dimmed
    renderGame();

    // Overlay game over screen
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX));
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    window.draw(overlay);

    sf::Text gameOverText;
    gameOverText.setFont(gameFont);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        (WINDOW_WIDTH_PX - textBounds.width) / 2.0f, 
        (WINDOW_HEIGHT_PX - textBounds.height) / 2.0f - TEXT_MARGIN_PX
    );
    window.draw(gameOverText);

    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setString("Final Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setPosition(
        (WINDOW_WIDTH_PX - scoreBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 20
    );
    window.draw(scoreText);

    sf::Text restartText;
    restartText.setFont(gameFont);
    restartText.setString("Press R or ENTER to Restart");
    restartText.setCharacterSize(18);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition(
        (WINDOW_WIDTH_PX - restartBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 70
    );
    window.draw(restartText);

    sf::Text menuText;
    menuText.setFont(gameFont);
    menuText.setString("Press ESC for Start Screen");
    menuText.setCharacterSize(18);
    menuText.setFillColor(sf::Color::White);
    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setPosition(
        (WINDOW_WIDTH_PX - menuBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 100
    );
    window.draw(menuText);

    updateTitle("Buzzy Defender - GAME OVER");
}

/**
 * @brief: Render the game won screen
 * 
 */
void BuzzyDefenderGame::renderGameWon()
{
    // Still show the game background, dimmed
    renderGame();

    // Overlay game won screen
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX));
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    window.draw(overlay);

    sf::Text gameWonText;
    gameWonText.setFont(gameFont);
    gameWonText.setString("YOU WIN!");
    gameWonText.setCharacterSize(48);
    gameWonText.setFillColor(sf::Color::Green);
    gameWonText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = gameWonText.getLocalBounds();
    gameWonText.setPosition(
        (WINDOW_WIDTH_PX - textBounds.width) / 2.0f, 
        (WINDOW_HEIGHT_PX - textBounds.height) / 2.0f - TEXT_MARGIN_PX
    );
    window.draw(gameWonText);

    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setString("Final Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setPosition(
        (WINDOW_WIDTH_PX - scoreBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 20
    );
    window.draw(scoreText);

    sf::Text restartText;
    restartText.setFont(gameFont);
    restartText.setString("Press R or ENTER to Restart");
    restartText.setCharacterSize(18);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition(
        (WINDOW_WIDTH_PX - restartBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 70
    );
    window.draw(restartText);

    sf::Text menuText;
    menuText.setFont(gameFont);
    menuText.setString("Press ESC for Start Screen");
    menuText.setCharacterSize(18);
    menuText.setFillColor(sf::Color::White);
    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setPosition(
        (WINDOW_WIDTH_PX - menuBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 100
    );
    window.draw(menuText);
}

/**
 * @brief: Render the paused screen
 * 
 */
void BuzzyDefenderGame::renderPaused()
{
    // Still show the game in background, dimmed
    renderGame();

    // Overlay game over screen
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX));
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    window.draw(overlay);

    sf::Text pausedText;
    pausedText.setFont(gameFont);
    pausedText.setString("PAUSED");
    pausedText.setCharacterSize(48);
    pausedText.setFillColor(sf::Color::Yellow);
    pausedText.setStyle(sf::Text::Bold);
    sf::FloatRect pausedBounds = pausedText.getLocalBounds();
    pausedText.setPosition(
        (WINDOW_WIDTH_PX - pausedBounds.width) / 2.0f, 
        (WINDOW_HEIGHT_PX - pausedBounds.height) / 2.0f - 50
    );
    window.draw(pausedText);

    sf::Text resumeText;
    resumeText.setFont(gameFont);
    resumeText.setString("Press P or ENTER to Resume");
    resumeText.setCharacterSize(18);
    resumeText.setFillColor(sf::Color::White);
    sf::FloatRect resumeBounds = resumeText.getLocalBounds();
    resumeText.setPosition(
        (WINDOW_WIDTH_PX - resumeBounds.width) / 2.0f,
        WINDOW_HEIGHT_PX / 2.0f + 20
    );
    window.draw(resumeText);

    updateTitle("Buzzy Defender - PAUSED");
}

/**
 * @brief: Utility function to update window title
 * 
 * @param title title to set window title to
 */
void BuzzyDefenderGame::updateTitle(const std::string& title)
{
    window.setTitle(title);
}