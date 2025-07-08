#pragma once

class Constants {
public:
    // Window dimensions
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;

    // Paddle settings
    static const int PADDLE_WIDTH = 15;
    static const int PADDLE_HEIGHT = 80;
    static const int PADDLE_SPEED = 5;

    // Ball settings
    static const int BALL_SIZE = 15;
    static constexpr double BALL_SPEED = 4.0;
    static constexpr double SERVE_SPEED_MULTIPLIER = 2.0;
    
    // Game settings
    static const int WIN_SCORE = 10;
    static const int FPS = 60;
    static const int FRAME_DELAY = 1000 / FPS;
    
    // Paddle starting positions
    static const int LEFT_PADDLE_START_X = 20;
    static const int RIGHT_PADDLE_START_X = WINDOW_WIDTH - 20 - PADDLE_WIDTH;
    static const int PADDLE_START_Y = WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    
    // Gravity well settings
    static const int GRAVITY_CENTER_X = WINDOW_WIDTH / 2;
    static const int GRAVITY_CENTER_Y = WINDOW_HEIGHT / 2;
    static constexpr double GRAVITY_RADIUS = 200.0;
    static constexpr double GRAVITY_STRENGTH = 0.15;
}; 