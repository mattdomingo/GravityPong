#include "Paddle.h"
#include <algorithm>

Paddle::Paddle(int x, int y, int width, int height, int speed)
    : position(x, y), width(width), height(height), speed(speed) {
}

void Paddle::moveUp() {
    if (position.y > 0) {
        position.y -= speed;
    }
}

void Paddle::moveDown() {
    if (position.y < Constants::WINDOW_HEIGHT - height) {
        position.y += speed;
    }
}

void Paddle::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_Rect rect = getRect();
    SDL_FRect frect = {static_cast<float>(rect.x), static_cast<float>(rect.y), 
                       static_cast<float>(rect.w), static_cast<float>(rect.h)};
    SDL_RenderFillRect(renderer, &frect);
}

bool Paddle::intersects(const SDL_Rect& other) const {
    SDL_Rect paddleRect = getRect();
    return SDL_HasRectIntersection(&paddleRect, &other);
}

SDL_Rect Paddle::getRect() const {
    return {static_cast<int>(position.x), static_cast<int>(position.y), width, height};
}

double Paddle::getCenterY() const {
    return position.y + height / 2.0;
} 