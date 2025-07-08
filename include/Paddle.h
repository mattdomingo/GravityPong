#pragma once
#include <SDL3/SDL.h>
#include "Vector2.h"
#include "Constants.h"

class Paddle {
public:
    Vector2 position;
    int width, height;
    int speed;
    
    Paddle(int x, int y, int width, int height, int speed);
    
    void moveUp();
    void moveDown();
    void draw(SDL_Renderer* renderer);
    
    // Collision detection helpers
    bool intersects(const SDL_Rect& other) const;
    SDL_Rect getRect() const;
    double getCenterY() const;
}; 