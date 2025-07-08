#include "Ball.h"
#include <cmath>
#include <algorithm>
#include <random>

Ball::Ball(int x, int y, int size)
    : position(x, y), width(size), height(size) {
}

std::mt19937& Ball::getRandomGenerator() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

void Ball::move() {
    // Apply gravity well effect before moving
    applyGravityWell();
    
    position.x += velocity.x;
    position.y += velocity.y;
}

void Ball::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    
    // Draw circle by drawing multiple points
    int centerX = static_cast<int>(position.x + width / 2);
    int centerY = static_cast<int>(position.y + height / 2);
    int radius = width / 2;
    
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Ball::serve() {
    position.x = Constants::WINDOW_WIDTH / 2.0 - width / 2.0;
    position.y = Constants::WINDOW_HEIGHT / 2.0 - height / 2.0;
    
    std::uniform_real_distribution<> dis(0.0, 1.0);
    auto& gen = getRandomGenerator();
    
    // Give the ball a stronger initial boost to escape the gravity well
    velocity.x = (dis(gen) < 0.5 ? Constants::BALL_SPEED : -Constants::BALL_SPEED) * Constants::SERVE_SPEED_MULTIPLIER;
    velocity.y = (dis(gen) - 0.5) * Constants::BALL_SPEED * Constants::SERVE_SPEED_MULTIPLIER;
}

void Ball::reverseX() {
    velocity.x = -velocity.x;
}

void Ball::reverseY() {
    velocity.y = -velocity.y;
}

bool Ball::intersects(const Paddle& paddle) const {
    SDL_Rect ballRect = getRect();
    return paddle.intersects(ballRect);
}

SDL_Rect Ball::getRect() const {
    return {static_cast<int>(position.x), static_cast<int>(position.y), width, height};
}

double Ball::getCenterY() const {
    return position.y + height / 2.0;
}

void Ball::applyGravityWell() {
    // Calculate vector from ball center to gravity well center
    double ballCenterX = position.x + width / 2.0;
    double ballCenterY = position.y + height / 2.0;
    double deltaX = Constants::GRAVITY_CENTER_X - ballCenterX;
    double deltaY = Constants::GRAVITY_CENTER_Y - ballCenterY;
    
    // Use squared distance for performance (avoid sqrt when possible)
    double distanceSquared = deltaX * deltaX + deltaY * deltaY;
    double radiusSquared = Constants::GRAVITY_RADIUS * Constants::GRAVITY_RADIUS;
    
    // Only apply gravity if ball is within the gravity radius
    if (distanceSquared < radiusSquared) {
        // Calculate actual distance (now we need sqrt for the force calculation)
        double distance = std::sqrt(distanceSquared);
        
        // Avoid division by zero when ball is exactly at center
        if (distance > 0.1) {
            // Linear force scaling: stronger when closer to center
            // force = GRAVITY_STRENGTH × (1 − distance / GRAVITY_RADIUS)
            double forceFactor = Constants::GRAVITY_STRENGTH * (1.0 - distance / Constants::GRAVITY_RADIUS);
            
            // Normalize the direction vector (deltaX, deltaY) and apply force
            double normalizedDeltaX = deltaX / distance;
            double normalizedDeltaY = deltaY / distance;
            
            // Calculate the proposed force components
            double forceX = normalizedDeltaX * forceFactor;
            double forceY = normalizedDeltaY * forceFactor;
            
            // Cap the force to not exceed 10% of base speed in any single frame
            double maxForce = Constants::BALL_SPEED * 0.1;
            double currentForce = std::sqrt(forceX * forceX + forceY * forceY);
            
            if (currentForce > maxForce) {
                // Scale down the force to the maximum allowed
                double scaleFactor = maxForce / currentForce;
                forceX *= scaleFactor;
                forceY *= scaleFactor;
            }
            
            // Apply the gravity force to the ball's velocity
            velocity.x += forceX;
            velocity.y += forceY;
        }
    }
} 