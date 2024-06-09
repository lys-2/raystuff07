
#include "raylib.h"
#include <raymath.h>
#include <stdlib.h>

#define AQ 1234
#define BUFF 4096
#define MS_BASE 300.0f

struct Player { char name[8]; int score; int controlled; };
struct Actor {
    char name[8]; Vector3 pos; Vector3 rot; float scale; float ms; 
    int overlap[16]; bool F1, F2, F3;
};
struct Level { struct Actor handle; struct Actor actors[16]; };
struct State {int counter; char name[32]; struct Actor a[AQ]; struct Player p[144];
Color lc[3]; int player; Vector3 cam;
Color tex[64 * 64];  unsigned char rec[4096];};

struct State s = {0};

void draw()
    {

    int i, x, y;

    for (i = 0; i < AQ; i++) {

        x = s.a[i].pos.x-s.cam.x;
        y = s.a[i].pos.y-s.cam.y;

        s.lc[2] = s.tex[i]; s.lc[2].a = 88;
        if (!strcmp(s.a[i].name, "box")) {
            DrawRectangle(x-s.a[i].scale/2, y - s.a[i].scale/2, s.a[i].scale, s.a[i].scale, s.lc[2]);
            DrawRectangleLines(x - s.a[i].scale / 2, y - s.a[i].scale / 2, s.a[i].scale, s.a[i].scale, s.tex[i]);
            DrawPixel(x, y + 1, WHITE); 
        };
        if (!strcmp(s.a[i].name, "sphere")) {
            DrawCircle(x, y, s.a[i].scale, s.lc[2]);
            DrawPixel(x, y + 1, GREEN); 
        };
        DrawText(TextFormat("%s, %i", s.a[i].name, i), x+20, y, 10, s.tex[i]);
    }
    for (i = 0; i < 11111; i++) { DrawPixel((x-i*4)%16, (y-i*16)%32, WHITE); }

}

short collision() {
    short c = s.p[0].controlled;
    short count; count = 0;
    for (short i = 0; i < AQ; i++) {
        if (i!=c)
        {if (CheckCollisionSpheres(s.a[c].pos, s.a[c].scale, s.a[i].pos, s.a[i].scale))
        {count += 1;};};};
    return count;
};


short tile(Vector3 v) { 
    int id; id = v.x + v.y;
    return (floor(v.y/123)*64+v.x/123); }

static void reset() {
    char arr[2][8] = { "box", "sphere" };
    for (short i = 0; i < AQ; i++) {

        s.a[i].pos.x = GetRandomValue(0, 2999);
        s.a[i].pos.y = GetRandomValue(0, 2999);
        s.a[i].scale = GetRandomValue(4, 33);
        s.a[i].ms = MS_BASE;
        strcpy(s.a[i].name, arr[GetRandomValue(0, 1)]);
        if (s.tex[tile(s.a[i].pos)].r == BLACK.r) {
            s.a[i].scale = 1; strcpy(s.a[i].name, "..~..");
        };
    };
    s.p[0].controlled = GetRandomValue(0, AQ);
    s.counter = 0;
    s.player = 0;

}

int main(int argc, char* argv[])
{


    short i, x, y, r, id, collisions; float distance;
    SetTargetFPS(2222);
    SetAudioStreamBufferSizeDefault(BUFF);
    InitWindow(999, 555, "SN24C");
    InitAudioDevice();
    AudioStream str[16]; for (i = 0; i < 4; ++i) {
        str[i] = LoadAudioStream(44100, 16, 1);
        SetAudioStreamVolume(str[i], .1);
    }

    int *data = malloc(1234);

    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    i = 0; for (i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
    i = 0; for (i = 0; i < 4096; i++) { s.rec[i] = sin(i/16.0)*126.0; }
    UnloadImage(image); UnloadImageColors(colors);

    reset();
 

    while (!WindowShouldClose())
    {

        collisions = collision();
        x = GetMousePosition().x; y = GetMousePosition().y;
        s.counter += 1;
        id = s.p[0].controlled;
        distance = ((((s.tex[tile(s.a[id].pos)].g)+14)/33.0)/4.0)*s.a[id].ms/(collisions+1) * GetFrameTime();
        if (IsKeyDown(KEY_R))  reset();
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) s.a[id].pos.y -= distance;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) s.a[id].pos.y += distance;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) s.a[id].pos.x += distance;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) s.a[id].pos.x -= distance;
        if (IsKeyPressed(KEY_E)) {
            if (data != NULL) { free(data); data = NULL; };
            UpdateAudioStream(str[1], s.rec, BUFF);
        SetAudioStreamPitch(str[1], GetRandomValue(4, 16));
        PlayAudioStream(str[1]);
    }
        
        SetAudioStreamVolume(str[0], 0);
        if (IsKeyDown(KEY_Q)) { SetAudioStreamVolume(str[0], .1);
        SetAudioStreamPitch(str[0], y / 333.0);
        i = 0; for (i = 0; i < 128; i++) {
            r = GetRandomValue(1, y / 20);
            DrawText(&s.rec[x * r], 990 - MeasureText(&s.rec[x * r], 10), i * 10, 10, GRAY);
        }
        };
        BeginDrawing();

        s.cam.x = s.a[id].pos.x - 300; s.cam.y = s.a[id].pos.y - 300;

        ClearBackground(DARKGRAY); 
        Vector2 vec =
        { 0-s.cam.x, 0-s.cam.y };
        DrawTextureEx(texture, vec, 0, 123, GRAY);

        draw();

        DrawTexture(texture, 0, 450, WHITE); 
        DrawText("npuBeT", 11, 22, 20, BLUE);
        DrawText("snrd07A2", 999-55, 4, 10, GRAY);
        DrawText(TextFormat("%i", s.counter), 22, 44, 40, BLUE);
        DrawText(TextFormat("%i", AQ), 188, 44, 40, BLUE);
        DrawText(TextFormat("%i %f", collisions, distance), 188, 88, 20, BLUE);
        DrawText(TextFormat("%f, %i", s.a[id].pos.x, tile(s.a[id].pos)),
            188, 111, 20, s.tex[tile(s.a[id].pos)]);
        DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
        DrawText(TextFormat("%f", GetFrameTime()*1000.0f), 64, 4, 10, PINK);
        DrawText(TextFormat("*", (int)floor(s.cam.x+x),
            (int)floor(s.cam.y + y)) , x - 10, y - 16, 40, BLACK);

        EndDrawing();

       if (!IsAudioStreamPlaying(str[0]) & IsAudioStreamReady(str[0])) {PlayAudioStream(str[0]);}
       if (IsAudioStreamProcessed(str[0])) { UpdateAudioStream(str[0], &s.rec[x*64], BUFF); }
       ;

    }


    CloseAudioDevice();         

    return 0;
}