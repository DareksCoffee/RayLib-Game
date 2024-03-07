#include "raylib.h"
#include "raymath.h"
#include "float.h"

#define GRAVITY 450
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_MAX_SPD 350.0f

#define MAX_ENVIRONMENT_ELEMENTS    10

typedef struct Player {
    Vector2 position;
    float speed;
    int lives;
    bool canJump;
    Color color;
} Player;

typedef struct Enemy {
    Vector2 position;
    float speed;
    Color color;
} Enemy;

typedef struct EnvElement {
    Rectangle rect;
    int blocking;
    Color color;
    bool killable;
} EnvElement;

int main(void)
{
    const int screen_width = 800;
    const int screen_height = 600;

    InitWindow(screen_width, screen_height, "Crazy 2D Game");

    const char* gameDirectory = GetWorkingDirectory();
    const char* iconPath = TextFormat("%s/resources/icon.png", gameDirectory);
    
    bool enableDebug = true;
    Image icon = LoadImage(iconPath);
    SetWindowIcon(icon);
    
    
    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.canJump = false;
    player.lives = 3;
    player.color = RED;
    
    Enemy enemy = { 0 };
    enemy.position = (Vector2){400, 100};
    enemy.speed = 0;
    enemy.color = BLUE;
    
    
    Image cat = LoadImage("resources/cat.png");
    Texture2D cat_texture = LoadTextureFromImage(cat);
    
    EnvElement envElements[MAX_ENVIRONMENT_ELEMENTS] = {
        {{ 0, 0, 1000, 400 }, 0, LIGHTGRAY, false },
        {{ 0, 400, 1000, 200 }, 1, GRAY, false },
        {{ 300, 200, 400, 10 }, 1, GRAY, false },
        {{ 250, 300, 100, 10 }, 1, GRAY, false },
        {{ 650, 300, 100, 10 }, 1, GRAY, false },
        {{ 800, 200, 200, 10 }, 1, RED, true }
    };
    
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        float deltaTime = 0.015f;

        
        if (IsKeyDown(KEY_LEFT)) player.position.x -= PLAYER_HOR_SPD*deltaTime;
        if (IsKeyDown(KEY_RIGHT)) player.position.x += PLAYER_HOR_SPD*deltaTime;
        if (IsKeyDown(KEY_SPACE) && player.canJump)
        {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
        }
        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT)) player.position.x -= PLAYER_MAX_SPD*deltaTime;

        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_RIGHT)) player.position.x += PLAYER_MAX_SPD*deltaTime;
        
        if(IsKeyDown(KEY_D)) enableDebug = !enableDebug;
     
        int hitObstacle = 0;
        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            EnvElement *element = &envElements[i];
            Vector2 *p = &(player.position);
            if (element->blocking &&
                element->rect.x <= p->x &&
                element->rect.x + element->rect.width >= p->x &&
                element->rect.y >= p->y &&
                element->rect.y <= p->y + player.speed*deltaTime)
            {
                if(element->killable){
                    if(player.lives <= 0)
                    {
                        CloseWindow();
                    }
                    else{
                        player.position = (Vector2){ 400, 280 };
                        player.speed = 0;
                        player.canJump = false;
                        player.lives -= 1;

                        camera.target = player.position;
                        camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f };
                        camera.rotation = 0.0f;
                        camera.zoom = 1.0f;   
                    }
                }
                else
                {
                    hitObstacle = 1;
                    player.speed = 0.0f;
                    p->y = element->rect.y;
                }
            }
        }
        int enemy_hitObstacle = 0;
        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            EnvElement *element = &envElements[i];
            Vector2 *e = &(enemy.position);
            if (element->blocking &&
                element->rect.x <= e->x &&
                element->rect.x + element->rect.width >= e->x &&
                element->rect.y >= e->y &&
                element->rect.y <= e->y + enemy.speed*deltaTime)
            {
                enemy_hitObstacle = 1;
                enemy.speed = 0.0f;
                
            }
        }
        if (!hitObstacle)
        {
            player.position.y += player.speed*deltaTime;
            player.speed += GRAVITY*deltaTime;
            player.canJump = false;
        }
        else player.canJump = true;
        
        if(!enemy_hitObstacle)
        {
            enemy.position.y += enemy.speed*deltaTime;
            enemy.speed += GRAVITY*deltaTime;
        }
        if(player.position.x >= screen_width + 200)
        {
            player.position.x = -screen_width + 200;
        }
        if (IsKeyPressed(KEY_R))
        {
            player.position = (Vector2){ 400, 280 };
            player.speed = 0;
            player.canJump = false;

            camera.target = player.position;
            camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f };
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;
        }
        
        camera.target = player.position;
        camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f };
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

        for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
        {
            EnvElement *element = &envElements[i];
            minX = fminf(element->rect.x, minX);
            maxX = fmaxf(element->rect.x + element->rect.width, maxX);
            minY = fminf(element->rect.y, minY);
            maxY = fmaxf(element->rect.y + element->rect.height, maxY);
        }

        Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, camera);
        Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, camera);

        if (max.x < screen_width) camera.offset.x = screen_width - (max.x - screen_width/2);
        if (max.y < screen_height) camera.offset.y = screen_height - (max.y - screen_height/2);
        if (min.x > 0) camera.offset.x = screen_width/2 - min.x;
        if (min.y > 0) camera.offset.y = screen_height/2 - min.y;
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                for (int i = 0; i < MAX_ENVIRONMENT_ELEMENTS; i++)
                {
                    DrawRectangleRec(envElements[i].rect, envElements[i].color);
                }

                DrawRectangleRec((Rectangle){ player.position.x - 20, player.position.y - 40, 40, 40 }, player.color);
                
                DrawRectangleRec((Rectangle){ enemy.position.x - 20, enemy.position.y - 40, 40, 40 }, enemy.color);

            EndMode2D();
            
        if(enableDebug){
            DrawRectangle(0, 0, 200, 50, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(0, 0, 200, 50, BLUE);
            DrawText("DEBUG :", 0, 0, 10, BLACK);
            DrawText(TextFormat("   - Player Position : (%06.3f, %06.3f)", player.position.x, player.position.y), 0, 10, 10, BLACK);
            DrawText("   - D to hide/show debug menu", 0, 20, 10, BLACK);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    UnloadImage(icon);
    return 0;
}