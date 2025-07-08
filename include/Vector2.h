#pragma once
#include <cmath>

struct Vector2 {
    double x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(double scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    double magnitude() const {
        return sqrt(x * x + y * y);
    }
    
    Vector2 normalized() const {
        double mag = magnitude();
        if (mag > 0.1) {
            return Vector2(x / mag, y / mag);
        }
        return Vector2(0, 0);
    }
}; 