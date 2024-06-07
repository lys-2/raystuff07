
#include "raylib.h"
#define AQ 1234
#define BUFF 4096


struct Player { char name[8]; int score; int controlled; };
struct Actor { char name[8]; Vector3 pos; Vector3 rot; float scale; bool F1, F2, F3; };
struct Level { struct Actor handle; struct Actor actors[16]; };
struct State {int counter; char name[32]; struct Actor a[AQ]; struct Player p[144];
Color lc[3]; int player;
Color tex[64 * 64];  unsigned char rec[4096];};

struct State s = { 0, "game", .p[0].controlled = 0, .p[0].name = "BoBa", .player = 0 };

void drawer()
    {

    int x, y, i;
    for (i = 0; i < AQ; i++) {

        x = s.a[i].pos.x-s.a[s.p[s.player].controlled].pos.x+499;
        y = s.a[i].pos.y- s.a[s.p[s.player].controlled].pos.y+300;
        s.lc[2] = s.tex[i]; s.lc[2].a = 88;
        if (!strcmp(s.a[i].name, "box")) {
            DrawRectangle(x, y, s.a[i].scale, s.a[i].scale, s.lc[2]);
            DrawRectangleLines(x, y, s.a[i].scale, s.a[i].scale, s.tex[i]);
            DrawPixel(x, y + 1, WHITE); DrawPixel(x, y, RED); DrawPixel(x + 1, y + 3, BLUE);
        };
        if (!strcmp(s.a[i].name, "sphere")) {
            DrawCircle(x, y, s.a[i].scale, s.lc[2]);
            DrawPixel(x, y + 1, GREEN); DrawPixel(x, y, RED); DrawPixel(x + 1, y + 3, BLUE);
        };
        DrawText(s.a[i].name, x, y, 10, s.tex[i]);
        DrawText(TextFormat("%i", i), x+20, y, 10, s.tex[i]);
    }
}

int main(int argc, char* argv[])
{

    int i, x, y, r;
    SetTargetFPS(2222);
    SetAudioStreamBufferSizeDefault(BUFF);
    InitWindow(999, 555, "SN24C");
    InitAudioDevice();
    AudioStream str[4]; for (i = 0; i < 4; ++i) {
        str[i] = LoadAudioStream(44100, 16, 1);
        SetAudioStreamVolume(str[i], .1);
    }

    char arr[2][8] = {"box", "sphere"};
    for (i = 0; i < AQ; i++) { 

    s.a[i].pos.x = GetRandomValue(0, 2999);
    s.a[i].pos.y = GetRandomValue(0, 2999); 
    s.a[i].scale = GetRandomValue(4, 33);
    strcpy(s.a[i].name, arr[GetRandomValue(0, 1)]);
    }; 
    s.p[0].controlled = GetRandomValue(0, AQ);


    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    i = 0; for (i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
    i = 0; for (i = 0; i < 4096; i++) { s.rec[i] = sin(i/16.0)*126.0; }

    UnloadImage(image); UnloadImageColors(colors);
 

    while (!WindowShouldClose())
    {
        x = GetMousePosition().x; y = GetMousePosition().y;
        s.counter += 1;
        if (IsKeyDown(KEY_R)) s.counter = 0;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) s.a[s.p[0].controlled].pos.y -= 1;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) s.a[s.p[0].controlled].pos.y += 1;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) s.a[s.p[0].controlled].pos.x += 1;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) s.a[s.p[0].controlled].pos.x -= 1;
        if (IsKeyPressed(KEY_E)) {
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



        ClearBackground(DARKGRAY); 
        DrawTexture(texture, 0, 450, WHITE); drawer();
        DrawText("npuBeT", 11, 22, 20, BLUE);
        DrawText("snrd07A2", 999-55, 4, 10, GRAY);
        DrawText(TextFormat("%i", s.counter), 22, 44, 40, BLUE);
        DrawText(TextFormat("%i", AQ), 188, 44, 40, BLUE);
        DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
        DrawText("*", x - 10, y - 16, 40, PINK);

        EndDrawing();

       if (!IsAudioStreamPlaying(str[0]) & IsAudioStreamReady(str[0])) {PlayAudioStream(str[0]);}
       if (IsAudioStreamProcessed(str[0])) { UpdateAudioStream(str[0], &s.rec[x*64], BUFF); }
       ;

    }

    CloseAudioDevice();         
    return 0;
}