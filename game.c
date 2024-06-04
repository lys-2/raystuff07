
#include "raylib.h"
#define AQ 2048

struct Player { char name[8]; int score; int controlled; };
struct Actor { char name[8]; Vector3 pos; Vector3 rot; float scale; bool F1, F2, F3; };
struct State {int counter; char name[32]; struct Actor a[AQ]; struct Player p[16];
Color lc[3]; int player;
Color tex[64 * 64]; char rec[64 * 64];};

struct State s = { 0, "game", .p[0].controlled = 0, .p[0].name = "BoBa", .player = 0 };


void drawer()
    {

    int x, y, i;
    for (i = 0; i < AQ; i++) {

        x = s.a[i].pos.x-s.a[s.p[s.player].controlled].pos.x+499;
        y = s.a[i].pos.y- s.a[s.p[s.player].controlled].pos.y+300;
        s.lc[2] = s.tex[i]; s.lc[2].a = 88;
        DrawCircle(x, y, s.a[i].scale, s.lc[2]);
        DrawPixel(x, y, WHITE);
        DrawText("tree", x, y, 10, s.lc[0]);
        DrawText(TextFormat("%i", i), x+20, y, 10, s.tex[i]);
    }
}

int main()
{
    SetTargetFPS(111);
    InitWindow(999, 555, "SN24C");
    int i; for (i = 0; i < AQ; i++) {s.a[i].pos.x = GetRandomValue(0, 2999);
    s.a[i].pos.y = GetRandomValue(0, 2999); s.a[i].scale = GetRandomValue(2, 22);
    }; s.p[0].controlled = GetRandomValue(0, 2047);

    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    i = 0; for (i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
        UnloadImage(image); UnloadImageColors(colors);


    while (!WindowShouldClose())
    {
        int x = GetMousePosition().x; int y = GetMousePosition().y;
        s.counter += 1;
        if (IsKeyDown(KEY_R)) s.counter = 0;
        if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) s.a[s.p[0].controlled].pos.y -= 1;
        if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) s.a[s.p[0].controlled].pos.y += 1;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) s.a[s.p[0].controlled].pos.x += 1;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) s.a[s.p[0].controlled].pos.x -= 1;
        BeginDrawing();

        ClearBackground(BLACK); 
        DrawTexture(texture, 0, 450, WHITE); drawer();
        DrawText("npuBeT", 11, 22, 20, BLUE);
        DrawText("snrd07A1", 999-55, 4, 10, GRAY);
        DrawText(TextFormat("%i", s.counter), 22, 44, 40, BLUE);
        DrawText(TextFormat("%i", AQ), 188, 44, 40, BLUE);
        DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
        DrawText("*", x - 10, y - 16, 40, PINK);
        EndDrawing();
    }

    return 0;
}