#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include "raylib.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

const int W = 800;
const int H = 450;
const int ROWS = 5;
const int COLS = 10;

struct BallType {
    float x, y;
    float dx, dy;
    float r;
} ball;

struct PaddleType {
    float x, y;
    float w, h;
    float speed;
} paddle;

struct BrickType {
    float x, y;
    float w, h;
    int alive;
} bricks[ROWS][COLS];

int score = 0;
int gameOver = 0;
int spaceCount = 0;

void init() {
    paddle.w = 100.0f;
    paddle.h = 15.0f;
    paddle.x = W / 2.0f - paddle.w / 2.0f;
    paddle.y = H - 40.0f;
    paddle.speed = 7.0f;

    ball.x = W / 2.0f;
    ball.y = H / 2.0f;
    ball.dx = 4.0f;
    ball.dy = -4.0f;
    ball.r = 8.0f;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            bricks[i][j].x = j * 75.0f + 25.0f;
            bricks[i][j].y = i * 25.0f + 40.0f;
            bricks[i][j].w = 70.0f;
            bricks[i][j].h = 20.0f;
            bricks[i][j].alive = 1;
        }
    }

    score = 0;
    gameOver = 0;
    spaceCount = 0;
}

void update() {
    if (gameOver != 0) return;

    if (IsKeyDown(KEY_LEFT) && paddle.x > 0) {
        paddle.x -= paddle.speed;
    }
    if (IsKeyDown(KEY_RIGHT) && paddle.x < W - paddle.w) {
        paddle.x += paddle.speed;
    }

    if (IsKeyPressed(KEY_SPACE) && spaceCount < 3) {
        ball.dx = -ball.dx;
        ball.dy = -ball.dy;
        spaceCount++;
    }

    ball.x += ball.dx;
    ball.y += ball.dy;

    if (ball.x + ball.r >= W || ball.x - ball.r <= 0) {
        ball.dx = -ball.dx;
    }
    if (ball.y - ball.r <= 0) {
        ball.dy = -ball.dy;
        if (spaceCount % 2 == 0) {
            if (ball.dx > 0) ball.dx += 0.5f;
            else ball.dx -= 0.5f;
        }
    }

    if (ball.y + ball.r >= H) {
        gameOver = 1;
        return;
    }

    Rectangle paddleRect = { paddle.x, paddle.y, paddle.w, paddle.h };
    if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.r, paddleRect)) {
        if (ball.dy > 0) {
            ball.dy = -ball.dy;
            float offset = (ball.x - (paddle.x + paddle.w / 2.0f)) / (paddle.w / 2.0f);
            ball.dx = offset * 6.0f;
        }
    }

    int bricksLeft = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (bricks[i][j].alive) {
                bricksLeft++;
                Rectangle brickRect = { bricks[i][j].x, bricks[i][j].y, bricks[i][j].w, bricks[i][j].h };
                if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.r, brickRect)) {
                    bricks[i][j].alive = 0;
                    ball.dy = -ball.dy;
                    score += 10;

                    if (score % 50 == 0) {
                        paddle.speed += 1.5f;
                    }
                    break;
                }
            }
        }
    }

    if (bricksLeft == 0) {
        gameOver = 2;
    }
}

void draw() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    if (gameOver == 0) {
        DrawCircleV({ ball.x, ball.y }, ball.r, MAROON);
        DrawRectangleRec({ paddle.x, paddle.y, paddle.w, paddle.h }, BLUE);

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (bricks[i][j].alive) {
                    Rectangle brickRect = { bricks[i][j].x, bricks[i][j].y, bricks[i][j].w, bricks[i][j].h };
                    DrawRectangleRec(brickRect, RED);
                    DrawRectangleLinesEx(brickRect, 1.0f, BLACK);
                }
            }
        }

        char txt[32];
        sprintf(txt, "SCORE: %d", score);
        DrawText(txt, 10, 10, 20, RAYWHITE);
        DrawText("<- ->", W - 70, 10, 20, GRAY);

        char spaceTxt[32];
        sprintf(spaceTxt, "SAVED: %d/3", spaceCount);
        DrawText(spaceTxt, 10, H - 25, 16, GOLD);
    }
    else {
        if (gameOver == 2) {
            DrawText("YOU WIN!", W / 2 - 80, H / 2 - 40, 40, GREEN);
        }
        else {
            DrawText("GAME OVER", W / 2 - 90, H / 2 - 40, 40, RED);
        }
        DrawText("PRESS ENTER TO RESTART", W / 2 - 140, H / 2 + 10, 20, RAYWHITE);

        if (IsKeyPressed(KEY_ENTER)) {
            init();
        }
    }
    EndDrawing();
}

int main() {
    srand((unsigned int)time(NULL));
    InitWindow(W, H, "Arkanoid");
    init();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    CloseWindow();
    return 0;
}