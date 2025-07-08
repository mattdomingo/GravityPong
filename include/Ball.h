#pragma once
#include <SDL3/SDL.h>
#include <random>
#include "Vector2.h"
#include "Constants.h"
#include "Paddle.h"

class Ball {
public:
    Vector2 position;
    Vector2 velocity;
    int width, height;
    
    Ball(int x, int y, int size);
    
    void move();
    void draw(SDL_Renderer* renderer) const;
    void serve();
    void reverseX();
    void reverseY();
    
    // Getters and setters
    double getVelX() const { return velocity.x; }
    double getVelY() const { return velocity.y; }
    void setVelY(double velY) { velocity.y = velY; }
    
    // Collision detection helpers
    bool intersects(const Paddle& paddle) const;
    SDL_Rect getRect() const;
    double getCenterY() const;
    
private:
    void applyGravityWell();
    
    // Static random number generator shared by all Ball instances
    static std::mt19937& getRandomGenerator();
}; 