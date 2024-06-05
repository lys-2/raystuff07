
#include "raylib.h"
#define AQ 1111
#define BUFF 1024

struct Player { char name[8]; int score; int controlled; };
struct Actor { char name[8]; Vector3 pos; Vector3 rot; float scale; bool F1, F2, F3; };
struct State {int counter; char name[32]; struct Actor a[AQ]; struct Player p[16];
Color lc[3]; int player;
Color tex[64 * 64]; short rec[128 * 128];};

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

int main(int argc, char* argv[])
{
    SetTargetFPS(222);
    SetAudioStreamBufferSizeDefault(BUFF);
    InitWindow(999, 555, "SN24C");
    InitAudioDevice();
    AudioStream stream = LoadAudioStream(8000, 8, 1);
    AudioStream stream2 = LoadAudioStream(8000, 8, 1);
    SetAudioStreamVolume(stream, .2);
    SetAudioStreamVolume(stream2, .2);


    int i,x,y,r; for (i = 0; i < AQ; i++) {s.a[i].pos.x = GetRandomValue(0, 2999);
    s.a[i].pos.y = GetRandomValue(0, 2999); s.a[i].scale = GetRandomValue(2, 22);
    }; s.p[0].controlled = GetRandomValue(0, 2047);

    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    i = 0; for (i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
    i = 0; for (i = 0; i < 128 * 128; i++) { s.rec[i] = sin(i)*127.0; }

    UnloadImage(image); UnloadImageColors(colors);
 

    while (!WindowShouldClose())
    {
        x = GetMousePosition().x; y = GetMousePosition().y;
        s.counter += 1;
        if (IsKeyDown(KEY_R)) s.counter = 0;
        if (IsKeyDown(KEY_UP)||IsKeyDown(KEY_W)) s.a[s.p[0].controlled].pos.y -= 1;
        if (IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S)) s.a[s.p[0].controlled].pos.y += 1;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) s.a[s.p[0].controlled].pos.x += 1;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) s.a[s.p[0].controlled].pos.x -= 1;
        if (IsKeyPressed(KEY_E)) { 
            UpdateAudioStream(stream2, s.rec, BUFF);  
            SetAudioStreamPitch(stream2, GetRandomValue(1, 4));
            PlayAudioStream(stream2);
        }
        SetAudioStreamVolume(stream, 0);
        if (IsKeyDown(KEY_Q)) { SetAudioStreamVolume(stream, .3);
        SetAudioStreamPitch(stream, GetMousePosition().y / 333);
        i = 0; for (i = 0; i < 128; i++) {
            r = GetRandomValue(1, y / 20);
            DrawText(&s.rec[x * r], 999 - MeasureText(&s.rec[x * r], 10), i * 10, 10, GRAY);
        }
        };
        BeginDrawing();



        ClearBackground(BLACK); 
        DrawTexture(texture, 0, 450, WHITE); drawer();
        DrawText("npuBeT", 11, 22, 20, BLUE);
        DrawText("snrd07A2", 999-55, 4, 10, GRAY);
        DrawText(TextFormat("%i", s.counter), 22, 44, 40, BLUE);
        DrawText(TextFormat("%i", AQ), 188, 44, 40, BLUE);
        DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
        DrawText("*", x - 10, y - 16, 40, PINK);

        EndDrawing();

       if (!IsAudioStreamPlaying(stream) & IsAudioStreamReady(stream)) {PlayAudioStream(stream);}
       if (IsAudioStreamProcessed(stream)) { UpdateAudioStream(stream, &s.rec[x*64], BUFF); };


    }

    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
    return 0;
}