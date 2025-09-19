/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-06
 * 
 * Description: Global parameters for the game
 * 
 * Details:
 * This header file defines various constants used throughout the game,
 * including window dimensions, player and enemy properties, laser characteristics,
 * and text display settings. These parameters make it easy to adjust game settings
 */
#pragma once

// Window Parameters
constexpr int WINDOW_WIDTH_PX = 800;
constexpr int WINDOW_HEIGHT_PX = 600;
constexpr int X_POS_DIST_MARGIN = 50;

// player params
constexpr float BUZZY_WIDTH_PX = 64.0f;
constexpr float BUZZY_HEIGHT_PX = 64.0f;
constexpr float PLAYER_SHOOT_COOLDOWN = 0.2f;
constexpr float PLAYER_DAMAGED_FLASH_TIME = 0.75f;
constexpr float PLAYER_LASER_SPEED = 400.0f;
constexpr float BUZZY_SPAWN_Y_MARGIN_PX = 50.0f;
constexpr float PLAYER_LASER_OFFSET_Y = 20.0f;

// enemy params
constexpr float ENEMY_WIDTH_PX = 64.0f;
constexpr float ENEMY_HEIGHT_PX = 64.0f;
constexpr float ENEMY_EXPLODING_SCALE = 0.5f;
constexpr float ENEMY_SPAWN_INTERVAL = 2.0f;
constexpr int MAX_ENEMIES = 50;
constexpr float ENEMY_DAMAGED_FLASH_TIME = 0.75f;
constexpr float ENEMY_LASER_SPEED = 200.0f;
constexpr float ENEMY_SPAWN_Y_MARGIN_PX = 50.0f;
constexpr float ENEMY_MOVEMENT_ON_WALL_HIT = 20.0f;
constexpr float ENEMY_SHOOT_COOLDOWN = 2.0f;            // time between enemy shots
constexpr float SHORT_ENEMY_SHOOT_COOLDOWN = 2.0f;      // shorter time between enemy shots
constexpr float INF_ENEMY_SHOOT_COOLDOWN = 100.0f;

// laser params
constexpr float LASER_WIDTH_PX = 10.0f;
constexpr float LASER_HEIGHT_PX = 64.0f;

// Text params
constexpr float TEXT_MARGIN_PX = 50.0f;


