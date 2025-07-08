#include "Game.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>

Game::Game()
    : window(nullptr), renderer(nullptr), font(nullptr), smallFont(nullptr),
      leftPaddle(Constants::LEFT_PADDLE_START_X, Constants::PADDLE_START_Y, 
                 Constants::PADDLE_WIDTH, Constants::PADDLE_HEIGHT, Constants::PADDLE_SPEED),
      rightPaddle(Constants::RIGHT_PADDLE_START_X, Constants::PADDLE_START_Y, 
                  Constants::PADDLE_WIDTH, Constants::PADDLE_HEIGHT, Constants::PADDLE_SPEED),
      player1Score(0), player2Score(0), winner(0), gameRunning(true), gameOver(false),
      wPressed(false), sPressed(false), upPressed(false), downPressed(false),
      roundInProgress(false), scoreThisRound(false), currentFPS(0.0),
      lastPlayerToHit(0), player1ControlsInverted(false), player2ControlsInverted(false),
      gen(rd()), powerUpSpawnDis(0.0, 1.0), powerUpPositionDis(0.0, 1.0) {
    
    lastFrameTime = std::chrono::high_resolution_clock::now();
    lastPowerUpSpawn = std::chrono::high_resolution_clock::now();
    
    // Start with one ball
    balls.emplace_back(Constants::WINDOW_WIDTH / 2, Constants::WINDOW_HEIGHT / 2, Constants::BALL_SIZE);
    balls[0].serve();
    roundInProgress = true;
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!TTF_Init()) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("C++ Pong", 
                             Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 
                             0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Load fonts with proper fallbacks for each platform
    const char* fontPaths[] = {
#ifdef __APPLE__
        "/System/Library/Fonts/Geneva.ttf",
        "/System/Library/Fonts/Monaco.ttf", 
        "/System/Library/Fonts/SFCompact.ttf",
        "/System/Library/Fonts/ArialHB.ttc",
#elif __linux__
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/arial.ttf",
#elif _WIN32
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf",
        "C:\\Windows\\Fonts\\tahoma.ttf",
#endif
        nullptr
    };
    
    // Try to load the main font
    for (int i = 0; fontPaths[i] != nullptr; i++) {
        font = TTF_OpenFont(fontPaths[i], 36);
        if (font) {
            break;
        }
    }
    
    if (!font) {
        std::cerr << "Failed to load any font! Tried all system font paths." << std::endl;
        std::cerr << "SDL_ttf Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Load small font
    for (int i = 0; fontPaths[i] != nullptr; i++) {
        smallFont = TTF_OpenFont(fontPaths[i], 12);
        if (smallFont) break;
    }
    
    return true;
}

void Game::run() {
    while (gameRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
        
        if (deltaTime.count() >= Constants::FRAME_DELAY) {
            handleEvents();
            if (!gameOver) {
                update();
            }
            render();
            updateFPS();
            lastFrameTime = currentTime;
        }
        
        SDL_Delay(1); // Small delay to prevent excessive CPU usage
    }
}

void Game::cleanup() {
    if (font) TTF_CloseFont(font);
    if (smallFont) TTF_CloseFont(smallFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            gameRunning = false;
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.key) {
                case SDLK_W: wPressed = true; break;
                case SDLK_S: sPressed = true; break;
                case SDLK_UP: upPressed = true; break;
                case SDLK_DOWN: downPressed = true; break;
                case SDLK_R: if (gameOver) resetGame(); break;
                case SDLK_ESCAPE: gameRunning = false; break;
            }
        } else if (e.type == SDL_EVENT_KEY_UP) {
            switch (e.key.key) {
                case SDLK_W: wPressed = false; break;
                case SDLK_S: sPressed = false; break;
                case SDLK_UP: upPressed = false; break;
                case SDLK_DOWN: downPressed = false; break;
            }
        }
    }
}

void Game::update() {
    updatePaddles();
    updateBalls();
    updatePowerUps();
    updateControlInversion();
    spawnPowerUp();
    checkCollisions();
    checkPowerUpCollisions();
    checkScore();
}

void Game::updateBalls() {
    // Move all balls and remove those that are off-screen
    auto it = balls.begin();
    while (it != balls.end()) {
        it->move();
        // Check if ball is completely off screen
        if (it->position.x < -it->width - 50 || it->position.x > Constants::WINDOW_WIDTH + 50) {
            it = balls.erase(it);
        } else {
            ++it;
        }
    }
    
    // If no balls remain, the round is over
    if (balls.empty()) {
        roundInProgress = false;
        scoreThisRound = false;
    }
}

void Game::updatePowerUps() {
    // Remove expired power-ups
    auto it = powerUps.begin();
    while (it != powerUps.end()) {
        if ((*it)->isExpired() || !(*it)->active) {
            it = powerUps.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);
    
    // Draw game elements
    drawField();
    drawGravityWell();
    leftPaddle.draw(renderer);
    rightPaddle.draw(renderer);
    for (const auto& ball : balls) {
        ball.draw(renderer);
    }
    
    // Draw power-ups
    for (const auto& powerUp : powerUps) {
        powerUp->draw(renderer);
    }
    
    drawScore();
    drawFPS();
    
    if (gameOver) {
        drawGameOver();
    } else if (player1Score == 0 && player2Score == 0) {
        drawControlsHint();
    }
    
    SDL_RenderPresent(renderer);
}

void Game::updatePaddles() {
    // Handle player 1 controls (swap W/S inputs when controls are inverted)
    bool moveUp1 = player1ControlsInverted ? sPressed : wPressed;
    bool moveDown1 = player1ControlsInverted ? wPressed : sPressed;
    
    if (moveUp1) leftPaddle.moveUp();
    if (moveDown1) leftPaddle.moveDown();
    
    // Handle player 2 controls (swap UP/DOWN inputs when controls are inverted)
    bool moveUp2 = player2ControlsInverted ? downPressed : upPressed;
    bool moveDown2 = player2ControlsInverted ? upPressed : downPressed;
    
    if (moveUp2) rightPaddle.moveUp();
    if (moveDown2) rightPaddle.moveDown();
}

void Game::checkCollisions() {
    for (auto& ball : balls) {
        // Ball with top and bottom walls
        if (ball.position.y <= 0 || ball.position.y >= Constants::WINDOW_HEIGHT - ball.height) {
            ball.reverseY();
            ball.position.y = std::max(0.0, std::min(static_cast<double>(Constants::WINDOW_HEIGHT - ball.height), ball.position.y));
        }
        
        // Ball with paddles
        if (ball.intersects(leftPaddle) && ball.getVelX() < 0) {
            handlePaddleCollision(ball, leftPaddle);
        } else if (ball.intersects(rightPaddle) && ball.getVelX() > 0) {
            handlePaddleCollision(ball, rightPaddle);
        }
    }
}

void Game::handlePaddleCollision(Ball& ball, Paddle& paddle) {
    ball.reverseX();
    
    // Track which player hit the ball
    if (&paddle == &leftPaddle) {
        ball.position.x = paddle.position.x + paddle.width;
        lastPlayerToHit = 1;
    } else {
        ball.position.x = paddle.position.x - ball.width;
        lastPlayerToHit = 2;
    }
    
    double impactPoint = (ball.getCenterY() - paddle.getCenterY()) / (paddle.height / 2.0);
    ball.setVelY(ball.getVelY() + impactPoint * 2.0);
    
    // Limit the speed of just this ball
    double maxSpeed = Constants::BALL_SPEED * 1.5;
    if (std::abs(ball.getVelY()) > maxSpeed) {
        ball.setVelY(ball.getVelY() > 0 ? maxSpeed : -maxSpeed);
    }
}



void Game::checkScore() {
    if (!roundInProgress || scoreThisRound) return; // Already scored this round
    
    for (const auto& ball : balls) {
        if (ball.position.x < 0) {
            player2Score++;
            scoreThisRound = true;
            clearAllBalls();
            checkWinCondition();
            serveBall();
            return;
        } else if (ball.position.x > Constants::WINDOW_WIDTH) {
            player1Score++;
            scoreThisRound = true;
            clearAllBalls();
            checkWinCondition();
            serveBall();
            return;
        }
    }
}

void Game::checkWinCondition() {
    if (player1Score >= Constants::WIN_SCORE) {
        winner = 1;
        gameOver = true;
    } else if (player2Score >= Constants::WIN_SCORE) {
        winner = 2;
        gameOver = true;
    }
}

void Game::serveBall() {
    if (!gameOver) {
        clearAllBalls();
        balls.emplace_back(Constants::WINDOW_WIDTH / 2, Constants::WINDOW_HEIGHT / 2, Constants::BALL_SIZE);
        balls[0].serve();
        roundInProgress = true;
        scoreThisRound = false;
    }
}

void Game::resetGame() {
    player1Score = 0;
    player2Score = 0;
    winner = 0;
    gameOver = false;
    
    // Reset control inversion state when starting a new game
    player1ControlsInverted = false;
    player2ControlsInverted = false;
    controlInversionStart = std::chrono::high_resolution_clock::now();
    
    serveBall();
}

void Game::updateFPS() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
    if (deltaTime.count() > 0) {
        currentFPS = 1000000.0 / deltaTime.count();
    }
}

void Game::drawField() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    
    // Draw dashed center line
    for (int y = 0; y < Constants::WINDOW_HEIGHT; y += 20) {
        if ((y / 10) % 2 == 0) {
            SDL_RenderLine(renderer, Constants::WINDOW_WIDTH / 2, y, 
                             Constants::WINDOW_WIDTH / 2, std::min(y + 10, Constants::WINDOW_HEIGHT));
        }
    }
}

void Game::drawGravityWell() {
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 60); // Semi-transparent blue
    
    // Draw gravity well circle outline
    int centerX = Constants::GRAVITY_CENTER_X;
    int centerY = Constants::GRAVITY_CENTER_Y;
    int radius = static_cast<int>(Constants::GRAVITY_RADIUS);
    
    // Draw circle by drawing points
    for (int angle = 0; angle < 360; angle += 2) {
        double radians = angle * M_PI / 180.0;
        int x = centerX + static_cast<int>(radius * std::cos(radians));
        int y = centerY + static_cast<int>(radius * std::sin(radians));
        SDL_RenderPoint(renderer, x, y);
    }
    
    // Draw center indicator
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 120); // More opaque blue
    int centerSize = 8;
    SDL_FRect centerRect = {static_cast<float>(centerX - centerSize/2), static_cast<float>(centerY - centerSize/2), 
                           static_cast<float>(centerSize), static_cast<float>(centerSize)};
    SDL_RenderFillRect(renderer, &centerRect);
    
    // Draw radial lines
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 40); // Very transparent blue
    for (int angle = 0; angle < 360; angle += 45) {
        double radians = angle * M_PI / 180.0;
        int innerRadius = 20;
        int outerRadius = static_cast<int>(Constants::GRAVITY_RADIUS * 0.8);
        
        int x1 = centerX + static_cast<int>(std::cos(radians) * innerRadius);
        int y1 = centerY + static_cast<int>(std::sin(radians) * innerRadius);
        int x2 = centerX + static_cast<int>(std::cos(radians) * outerRadius);
        int y2 = centerY + static_cast<int>(std::sin(radians) * outerRadius);
        
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}

void Game::drawScore() {
    std::string scoreText = std::to_string(player1Score) + "  :  " + std::to_string(player2Score);
    renderTextCentered(scoreText, 50, font);
}

void Game::drawFPS() {
    if (smallFont) {
        std::stringstream ss;
        ss << "FPS: " << std::fixed << std::setprecision(1) << currentFPS;
        renderText(ss.str(), 10, Constants::WINDOW_HEIGHT - 20, smallFont);
    }
}

void Game::drawGameOver() {
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect overlay = {0, 0, static_cast<float>(Constants::WINDOW_WIDTH), static_cast<float>(Constants::WINDOW_HEIGHT)};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    std::string winText = "Player " + std::to_string(winner) + " Wins!";
    renderTextCentered(winText, Constants::WINDOW_HEIGHT / 2 - 20, font);
    
    std::string restartText = "Press R to restart";
    renderTextCentered(restartText, Constants::WINDOW_HEIGHT / 2 + 30, smallFont);
}

void Game::drawControlsHint() {
    if (smallFont) {
        renderText("P1: W/S  |  P2: ↑/↓  |  ESC: Quit", 10, 20, smallFont);
    }
}

void Game::renderText(const std::string& text, int x, int y, TTF_Font* fontToUse) {
    if (!fontToUse) fontToUse = font;
    if (!fontToUse) return;
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(fontToUse, text.c_str(), text.length(), white);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_FRect destRect = {static_cast<float>(x), static_cast<float>(y), 
                                 static_cast<float>(textSurface->w), static_cast<float>(textSurface->h)};
            SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}

void Game::renderTextCentered(const std::string& text, int y, TTF_Font* fontToUse) {
    if (!fontToUse) fontToUse = font;
    if (!fontToUse) return;
    
    int textWidth, textHeight;
    if (TTF_GetStringSize(fontToUse, text.c_str(), text.length(), &textWidth, &textHeight)) {
        int x = (Constants::WINDOW_WIDTH - textWidth) / 2;
        renderText(text, x, y, fontToUse);
    }
}

void Game::spawnPowerUp() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeSinceLastSpawn = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastPowerUpSpawn);
    
    // Spawn a power-up every 15-25 seconds if none exist
    if (timeSinceLastSpawn.count() >= 15 && powerUps.empty() && powerUpSpawnDis(gen) < 0.1) {
        // Spawn in the middle area of the screen, avoiding paddle zones
        int x = Constants::WINDOW_WIDTH * 0.3 + powerUpPositionDis(gen) * Constants::WINDOW_WIDTH * 0.4;
        int y = 50 + powerUpPositionDis(gen) * (Constants::WINDOW_HEIGHT - 100);
        
        // Randomly choose between MULTIBALL and INVERT_CONTROLS
        PowerUpType type = (powerUpSpawnDis(gen) < 0.5) ? PowerUpType::MULTIBALL : PowerUpType::INVERT_CONTROLS;
        
        powerUps.push_back(std::make_unique<PowerUp>(x, y, type));
        lastPowerUpSpawn = currentTime;
    }
}

void Game::checkPowerUpCollisions() {
    for (auto& powerUp : powerUps) {
        if (!powerUp->active) continue;
        
        for (const auto& ball : balls) {
            SDL_Rect ballRect = ball.getRect();
            SDL_Rect powerUpRect = powerUp->getRect();
            
            if (SDL_HasRectIntersection(&ballRect, &powerUpRect)) {
                // Power-up collected!
                powerUp->active = false;
                
                switch (powerUp->type) {
                    case PowerUpType::MULTIBALL:
                        activateMultiball();
                        break;
                    case PowerUpType::INVERT_CONTROLS:
                        activateInvertControls();
                        break;
                }
                break; // Only one ball can collect the power-up
            }
        }
    }
}

void Game::activateInvertControls() {
    // Apply control inversion to whichever player last hit the ball
    // This ensures the power-up affects the opponent of whoever collected it
    if (lastPlayerToHit == 1) {
        player1ControlsInverted = true;
        player2ControlsInverted = false;
    } else if (lastPlayerToHit == 2) {
        player2ControlsInverted = true;
        player1ControlsInverted = false;
    }
    
    // Start the inversion timer - effect will last for CONTROL_INVERSION_DURATION seconds
    controlInversionStart = std::chrono::high_resolution_clock::now();
}

void Game::updateControlInversion() {
    // Check if any player currently has inverted controls
    if (player1ControlsInverted || player2ControlsInverted) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - controlInversionStart);
        
        // Disable inversion after the duration expires (10 seconds)
        if (elapsed.count() >= CONTROL_INVERSION_DURATION) {
            player1ControlsInverted = false;
            player2ControlsInverted = false;
        }
    }
}

void Game::activateMultiball() {
    if (balls.empty()) return;
    
    // Get the current ball's position and velocity
    Ball& currentBall = balls[0];
    Vector2 pos = currentBall.position;
    Vector2 vel = currentBall.velocity;
    
    // Clear existing balls
    balls.clear();
    
    // Create 5 balls with different angles
    double baseAngle = std::atan2(vel.y, vel.x);
    double speed = vel.magnitude();
    
    for (int i = 0; i < 5; i++) {
        double angle = baseAngle + (i - 2) * 0.3; // Spread balls in a fan pattern
        
        Ball newBall(pos.x, pos.y, Constants::BALL_SIZE);
        newBall.velocity.x = speed * std::cos(angle);
        newBall.velocity.y = speed * std::sin(angle);
        
        balls.push_back(newBall);
    }
}

void Game::clearAllBalls() {
    balls.clear();
    powerUps.clear(); // Clear power-ups when round ends
} 