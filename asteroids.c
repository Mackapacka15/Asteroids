#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const int screenWidth = 1280;
const int screenHeight = 720;

const int asteroidSize = 20;
const int maxAsteroids = 50;

float bulletTimer = 0;
float asteroidTimer = 0;

typedef enum
{
    GameStateMainMenu,
    GameStateGame,
    GameStateDead,
} game_state_e;

game_state_e state = GameStateGame;

typedef struct
{
    Vector2 pos;
    float speed;
    int size;
    double angle;
    int alive;
    int beenOnScreen;
} entity_t;

typedef struct
{
    Vector2 pos;
    Vector2 velocity;
    float speed;
    double angle;
    int size;
    int alive;
} player_t;

typedef enum
{
    ReturnSuccess,
    ReturnError
} return_code_e;

entity_t asteroids[maxAsteroids];
entity_t bullets[20];
player_t player = (player_t){.pos = {screenWidth / 2, screenHeight / 2}, .velocity = {0, 0}, .angle = 0, .size = 20, .speed = 5, .alive = 1};

return_code_e SpawnBullet(entity_t *bullets, player_t *player)
{
    static int currentSlot = 0;

    if (bulletTimer <= 0.5)
    {
        return ReturnError;
    }
    bullets[currentSlot].angle = player->angle;
    bullets[currentSlot].speed = 20;
    bullets[currentSlot].pos = player->pos;
    bullets[currentSlot].alive = 1;
    bullets[currentSlot].size = 4;
    bulletTimer = 0;

    if (currentSlot == 19)
    {
        currentSlot = 0;
    }
    else
    {
        currentSlot++;
    }

    return ReturnSuccess;
}

int DrawEntity(entity_t *entities, Color color, int maxLength)
{
    for (int i = 0; i < maxLength; i++)
    {
        if (entities[i].alive == 1)
        {
            DrawCircleV(entities[i].pos, entities[i].size, color);
        }
    }
    return 1;
}

int UpdateAsteroid(entity_t *asteroids, int maxLength)
{
    for (int i = 0; i < maxLength; i++)
    {

        if (CheckCollisionCircles(asteroids[i].pos, asteroids[i].size, player.pos, player.size) && asteroids[i].alive == 1)
        {
            player.alive = 0;
            state = GameStateDead;
        }

        if (asteroids[i].beenOnScreen == 0)
        {
            if (asteroids[i].pos.x > 0 && asteroids[i].pos.y > 0 && asteroids[i].pos.x < screenWidth && asteroids[i].pos.y < screenHeight)
            {
                asteroids[i].beenOnScreen = 1;
            }
        }

        if ((asteroids[i].pos.x >= screenWidth + 50 || asteroids[i].pos.x <= -50 || asteroids[i].pos.y >= screenHeight + 50 || asteroids[i].pos.y <= -50) && asteroids[i].beenOnScreen == 1)
        {
            asteroids[i].alive = 0;
        }
        asteroids[i].pos.x += asteroids[i].speed * cos(asteroids[i].angle);
        asteroids[i].pos.y += asteroids[i].speed * sin(asteroids[i].angle);
    }
    return 1;
}

int UpdateBullets(entity_t *bullets, entity_t *asteroids, int maxLength)
{
    for (int i = 0; i < maxLength; i++)
    {
        if (bullets[i].beenOnScreen == 0)
        {
            if (bullets[i].pos.x > 0 && bullets[i].pos.y > 0 && bullets[i].pos.x < screenWidth && bullets[i].pos.y < screenHeight)
            {
                bullets[i].beenOnScreen = 1;
            }
        }

        for (int x = 0; x < maxAsteroids; x++)
        {
            if (CheckCollisionCircles(asteroids[x].pos, asteroids[x].size, bullets[i].pos, bullets[i].size) && (bullets[i].alive == 1 && asteroids[x].alive == 1))
            {
                bullets[i].alive = 0;
                asteroids[x].alive = 0;

            }
        }

        bullets[i].pos.x += bullets[i].speed * cos(bullets[i].angle);
        bullets[i].pos.y += bullets[i].speed * sin(bullets[i].angle);
    }
    return 1;
}

int DrawPlayer(player_t *player)
{
    DrawCircleV(player->pos, player->size, GREEN);
    DrawLineV(player->pos, (Vector2){player->pos.x + 50 * cos(player->angle), player->pos.y + 50 * sin(player->angle)}, GREEN);
    return 1;
}

int UpdatePlayer(player_t *player)
{
    if (IsKeyDown(KEY_RIGHT))
    {
        player->angle += 0.1;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        player->angle -= 0.1;
    }
    if (IsKeyDown(KEY_UP))
    {
        player->velocity.x = player->speed * cos(player->angle);
        player->velocity.y = player->speed * sin(player->angle);
    }
    if (IsKeyDown(KEY_DOWN))
    {
        player->velocity = (Vector2){0, 0};
    }

    if (IsKeyDown(KEY_SPACE))
    {
        SpawnBullet(bullets, player);
    }

    player->pos.x += player->velocity.x;
    player->pos.y += player->velocity.y;

    if (player->velocity.x != 0 || player->velocity.y != 0)
    {
        player->velocity.x *= 0.9;
        player->velocity.y *= 0.9;
    }

    return 1;
}

static int currentAsteroid = 0;
int SpawnAsteroid(void)
{
    if (asteroidTimer >= 1)
    {

        int degrees = rand() % 360;
        float rad = (rand() % 360) * DEG2RAD;
        asteroidTimer = 0;

        int x = cos(rad) * screenWidth + screenWidth / 2;
        int y = sin(rad) * screenWidth + screenHeight / 2;

        Vector2 toPlayer = {player.pos.x - x, player.pos.y - y};

        float angle = atan2(toPlayer.y, toPlayer.x);

        asteroids[currentAsteroid].pos.x = x;
        asteroids[currentAsteroid].pos.y = y;
        asteroids[currentAsteroid].size = 20;
        asteroids[currentAsteroid].alive = 1;
        asteroids[currentAsteroid].speed = rand() % 5 + 1;
        asteroids[currentAsteroid].angle = angle;
        asteroids[currentAsteroid].beenOnScreen = 0;

        currentAsteroid++;

        if (currentAsteroid == maxAsteroids)
        {
            currentAsteroid = 0;
        }
    }
    return 1;
}

int main(void)
{
    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Asteroids");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // UPDATE:
        switch (state)
        {
        case GameStateGame:
            bulletTimer += GetFrameTime();
            asteroidTimer += GetFrameTime();
            SpawnAsteroid();
            UpdatePlayer(&player);
            UpdateAsteroid(asteroids, maxAsteroids);
            UpdateBullets(bullets, asteroids, 20);
            break;

        case GameStateDead:
            break;

        default:
            break;
        }

        // RENDER:

        BeginDrawing();
        switch (state)
        {
        case GameStateGame:
            ClearBackground(BLACK);
            DrawText(TextFormat("Asteroids %d\n", currentAsteroid), 100, 100, 50, YELLOW);
          
            DrawPlayer(&player);
            DrawEntity(asteroids, ORANGE, maxAsteroids);
            DrawEntity(bullets, GREEN, 20);
            break;

        case GameStateDead:
            ClearBackground(WHITE);
            DrawText("You Lost", 100, 100, 50, BLACK);
            break;

        default:
            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 1;
}