#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <iostream>


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int PLAYER_WIDTH = 40;
const int PLAYER_HEIGHT = 40;
const int PLAYER_SPEED = 4;
const int JUMP_LIMIT = 20;
const int JUMP_POWER = 8;

const int BULLET_LIMIT = 100;
const int BULLET_WIDTH = 10;
const int BULLET_HEIGHT = 4;
const int BULLET_SPEED = 10;

const int ENEMY_LIMIT = 10;
const int ENEMY_WIDTH = 40;
const int ENEMY_HEIGHT = 40;
const int ENEMY_MIN_SPEED = 2;
const int ENEMY_MAX_SPEED = 5;

struct Entity {
    int x, y;
    bool jumping;
    int jumpCount;
    void (*update)(Entity&);
};

struct Bullet {
    int x, y;
    bool active;
};

struct Enemy {
    int x, y, speed;
    bool active;
};

Bullet bullets[BULLET_LIMIT];
Enemy enemies[ENEMY_LIMIT];

int spawnTimer = 0;
int spawnDelay = 0;
int score = 0;
int lives = 3;

void updatePlayer(Entity& player) {
    if (IsKeyDown(KEY_A) && player.x > 0) player.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_D) && player.x < SCREEN_WIDTH - PLAYER_WIDTH) player.x += PLAYER_SPEED;

    if (IsKeyPressed(KEY_W) && !player.jumping) {
        player.jumping = true;
        player.jumpCount = JUMP_LIMIT;
    }
    if (player.jumping) {
        if (player.jumpCount > 0) {
            player.y -= JUMP_POWER;
            player.jumpCount--;
        }
        else if (player.y < SCREEN_HEIGHT - PLAYER_HEIGHT) {
            player.y += JUMP_POWER;
        }
        else {
            player.jumping = false;
            player.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        for (int i = 0; i < BULLET_LIMIT; i++) {
            if (!bullets[i].active) {
                bullets[i] = { player.x + PLAYER_WIDTH, player.y + (PLAYER_HEIGHT / 2) - (BULLET_HEIGHT / 2), true };
                break;
            }
        }
    }
}

void updateBullets() {
    for (int i = 0; i < BULLET_LIMIT; i++) {
        if (bullets[i].active) {
            bullets[i].x += BULLET_SPEED;
            if (bullets[i].x > SCREEN_WIDTH) bullets[i].active = false;
        }
    }
}

void updateEnemies() {
    spawnTimer++;
    if (spawnTimer >= spawnDelay) {
        for (int i = 0; i < ENEMY_LIMIT; i++) {
            if (!enemies[i].active) {
                enemies[i] = { SCREEN_WIDTH, SCREEN_HEIGHT - ENEMY_HEIGHT, GetRandomValue(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED), true };
                break;
            }
        }
        spawnTimer = 0;
        spawnDelay = GetRandomValue(60, 180);
    }
    for (int i = 0; i < ENEMY_LIMIT; i++) {
        if (enemies[i].active) {
            enemies[i].x -= enemies[i].speed;
            if (enemies[i].x < -ENEMY_WIDTH) enemies[i].active = false;
        }
    }
}

bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

int main() {
    srand(time(0));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Metal Slug Clone");
    SetTargetFPS(60);

    Entity player = { 50, SCREEN_HEIGHT - PLAYER_HEIGHT, false, 0, updatePlayer };

    for (int i = 0; i < BULLET_LIMIT; i++) bullets[i].active = false;
    for (int i = 0; i < ENEMY_LIMIT; i++) enemies[i].active = false;

    spawnDelay = GetRandomValue(60, 180);
    bool gameOver = false;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            player.update(player);
            updateBullets();
            updateEnemies();

            for (int i = 0; i < BULLET_LIMIT; i++) {
                if (bullets[i].active) {
                    for (int j = 0; j < ENEMY_LIMIT; j++) {
                        if (enemies[j].active && checkCollision(bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, enemies[j].x, enemies[j].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {
                            bullets[i].active = false;
                            enemies[j].active = false;
                            score += 10;
                        }
                    }
                }
            }

            for (int i = 0; i < ENEMY_LIMIT; i++) {
                if (enemies[i].active && checkCollision(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {
                    enemies[i].active = false;
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);
        DrawText(TextFormat("Lives: %i", lives), SCREEN_WIDTH - 150, 10, 20, BLACK);

        if (!gameOver) {
            DrawRectangle(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, GREEN);
            for (int i = 0; i < BULLET_LIMIT; i++) if (bullets[i].active) DrawRectangle(bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, BLACK);
            for (int i = 0; i < ENEMY_LIMIT; i++) if (enemies[i].active) DrawRectangle(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, RED);
        }
        else {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, RED);
            DrawText(TextFormat("Final Score: %i", score), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Final Score: %i", score), 20) / 2, SCREEN_HEIGHT / 2 + 10, 20, BLACK);
            DrawText("Press R to Restart", SCREEN_WIDTH / 2 - MeasureText("Press R to Restart", 20) / 2, SCREEN_WIDTH / 2 + 40, 20, DARKGREEN);
            if (IsKeyPressed(KEY_R)) {
                score = 0;
                lives = 3;
                player.x = 50;
                player.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
                player.jumping = false;
                spawnTimer = 0;
                spawnDelay = GetRandomValue(60, 180);
                for (int i = 0; i < BULLET_LIMIT; i++) bullets[i].active = false;
                for (int i = 0; i < ENEMY_LIMIT; i++) enemies[i].active = false;
                gameOver = false;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
