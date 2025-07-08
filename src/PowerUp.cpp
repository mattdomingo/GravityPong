#include "PowerUp.h"
#include <cmath>

PowerUp::PowerUp(int x, int y, PowerUpType powerUpType) 
    : position(x, y), width(30), height(30), type(powerUpType), 
      spawnTime(std::chrono::high_resolution_clock::now()), active(true), pulseAnimation(0.0f) {
}

void PowerUp::draw(SDL_Renderer* renderer) {
    if (!active) return;
    
    // Update pulse animation
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - spawnTime);
    pulseAnimation = static_cast<float>(elapsed.count()) / 100.0f;
    
    // Calculate pulsing effect
    float pulse = 0.8f + 0.2f * std::sin(pulseAnimation);
    int pulseSize = static_cast<int>(width * pulse);
    int offset = (width - pulseSize) / 2;
    
    // Draw power-up based on type
    switch (type) {
        case PowerUpType::MULTIBALL: {
            // Draw glowing multiball power-up
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold color
            SDL_FRect outerRect = {
                static_cast<float>(position.x + offset), 
                static_cast<float>(position.y + offset), 
                static_cast<float>(pulseSize), 
                static_cast<float>(pulseSize)
            };
            SDL_RenderFillRect(renderer, &outerRect);
            
            // Inner darker core
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange
            int coreSize = pulseSize * 0.6f;
            int coreOffset = (pulseSize - coreSize) / 2;
            SDL_FRect coreRect = {
                static_cast<float>(position.x + offset + coreOffset), 
                static_cast<float>(position.y + offset + coreOffset), 
                static_cast<float>(coreSize), 
                static_cast<float>(coreSize)
            };
            SDL_RenderFillRect(renderer, &coreRect);
            
            // Draw multiple small circles to represent multiball
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            int centerX = position.x + width / 2;
            int centerY = position.y + height / 2;
            int ballRadius = 2;
            
            // Draw 5 small balls in a cross pattern
            SDL_FRect ball1 = {static_cast<float>(centerX - ballRadius), static_cast<float>(centerY - ballRadius), 
                              static_cast<float>(ballRadius * 2), static_cast<float>(ballRadius * 2)};
            SDL_FRect ball2 = {static_cast<float>(centerX - ballRadius - 6), static_cast<float>(centerY - ballRadius), 
                              static_cast<float>(ballRadius * 2), static_cast<float>(ballRadius * 2)};
            SDL_FRect ball3 = {static_cast<float>(centerX - ballRadius + 6), static_cast<float>(centerY - ballRadius), 
                              static_cast<float>(ballRadius * 2), static_cast<float>(ballRadius * 2)};
            SDL_FRect ball4 = {static_cast<float>(centerX - ballRadius), static_cast<float>(centerY - ballRadius - 6), 
                              static_cast<float>(ballRadius * 2), static_cast<float>(ballRadius * 2)};
            SDL_FRect ball5 = {static_cast<float>(centerX - ballRadius), static_cast<float>(centerY - ballRadius + 6), 
                              static_cast<float>(ballRadius * 2), static_cast<float>(ballRadius * 2)};
            
            SDL_RenderFillRect(renderer, &ball1);
            SDL_RenderFillRect(renderer, &ball2);
            SDL_RenderFillRect(renderer, &ball3);
            SDL_RenderFillRect(renderer, &ball4);
            SDL_RenderFillRect(renderer, &ball5);
            break;
        }
            
        case PowerUpType::INVERT_CONTROLS: {
            // Draw glowing invert controls power-up
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
            SDL_FRect outerRect = {
                static_cast<float>(position.x + offset), 
                static_cast<float>(position.y + offset), 
                static_cast<float>(pulseSize), 
                static_cast<float>(pulseSize)
            };
            SDL_RenderFillRect(renderer, &outerRect);
            
            // Inner darker core
            SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255); // Dark red
            int coreSize = pulseSize * 0.6f;
            int coreOffset = (pulseSize - coreSize) / 2;
            SDL_FRect coreRect = {
                static_cast<float>(position.x + offset + coreOffset), 
                static_cast<float>(position.y + offset + coreOffset), 
                static_cast<float>(coreSize), 
                static_cast<float>(coreSize)
            };
            SDL_RenderFillRect(renderer, &coreRect);
            
            // Draw arrows pointing in opposite directions to represent inversion
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            int centerX = position.x + width / 2;
            int centerY = position.y + height / 2;
            
            // Up arrow (pointing down when inverted)
            SDL_RenderLine(renderer, centerX - 4, centerY - 6, centerX, centerY - 2);
            SDL_RenderLine(renderer, centerX + 4, centerY - 6, centerX, centerY - 2);
            SDL_RenderLine(renderer, centerX, centerY - 2, centerX, centerY + 2);
            
            // Down arrow (pointing up when inverted)
            SDL_RenderLine(renderer, centerX - 4, centerY + 6, centerX, centerY + 2);
            SDL_RenderLine(renderer, centerX + 4, centerY + 6, centerX, centerY + 2);
            break;
        }
    }
}

bool PowerUp::isExpired() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - spawnTime);
    return elapsed.count() >= LIFETIME_SECONDS;
}

SDL_Rect PowerUp::getRect() const {
    return {static_cast<int>(position.x), static_cast<int>(position.y), width, height};
} 