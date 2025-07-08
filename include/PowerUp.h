#pragma once
#include <SDL3/SDL.h>
#include "Vector2.h"
#include "Constants.h"
#include <chrono>

enum class PowerUpType {
    MULTIBALL,
    INVERT_CONTROLS
};

class PowerUp {
public:
    Vector2 position;
    int width, height;
    PowerUpType type;
    std::chrono::high_resolution_clock::time_point spawnTime;
    bool active;
    
    PowerUp(int x, int y, PowerUpType powerUpType);
    
    void draw(SDL_Renderer* renderer);
    bool isExpired() const;
    SDL_Rect getRect() const;
    
private:
    static constexpr float LIFETIME_SECONDS = 10.0f; // Power-up disappears after 10 seconds
    float pulseAnimation;
}; 