#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <random>
#include "Paddle.h"
#include "Ball.h"
#include "PowerUp.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* smallFont;
    
    Paddle leftPaddle;
    Paddle rightPaddle;
    std::vector<Ball> balls;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    
    int player1Score;
    int player2Score;
    int winner;
    
    bool gameRunning;
    bool gameOver;
    bool wPressed, sPressed, upPressed, downPressed;
    bool roundInProgress;  // Track if balls are active in current round
    bool scoreThisRound;   // Track if a score has happened this round

    // Control inversion tracking
    int lastPlayerToHit;   // 1 for left player, 2 for right player
    bool player1ControlsInverted;
    bool player2ControlsInverted;
    std::chrono::high_resolution_clock::time_point controlInversionStart;
    static constexpr float CONTROL_INVERSION_DURATION = 10.0f; // 10 seconds
    
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::chrono::high_resolution_clock::time_point lastPowerUpSpawn;
    double currentFPS;
    
    // Random number generation for power-up spawning
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> powerUpSpawnDis;
    std::uniform_real_distribution<> powerUpPositionDis;
    
    void handleEvents();
    void update();
    void render();
    
    void updatePaddles();
    void updateBalls();
    void updatePowerUps();
    void checkCollisions();
    void handlePaddleCollision(Ball& ball, Paddle& paddle);
    void checkScore();
    void checkWinCondition();
    void serveBall();
    void resetGame();
    void updateFPS();
    
    // Power-up related methods
    void spawnPowerUp();
    void checkPowerUpCollisions();
    void activateMultiball();
    void activateInvertControls();
    void updateControlInversion();
    void clearAllBalls();
    
    void drawField();
    void drawGravityWell();
    void drawScore();
    void drawFPS();
    void drawGameOver();
    void drawControlsHint();
    
    void renderText(const std::string& text, int x, int y, TTF_Font* font = nullptr);
    void renderTextCentered(const std::string& text, int y, TTF_Font* font = nullptr);
}; 