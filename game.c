
#include "raylib.h"
#include <raymath.h>
#include <stdlib.h>

#define AQ 2345
#define BUFF 4096
#define MS_BASE 300.0f

struct Point { Vector3 pos; }; struct Line { int a; int b; };
struct Triangle { int a; int b; int c; };
struct Mesh2 { short start; short end; };
struct Player { char name[8]; int score; int controlled; };
struct Actor {
    char name[8]; Vector3 pos; Vector3 rot; Vector3 scale; float ms;
    int overlap[2]; bool flags [8]; 
};
struct Scene { struct Actor actors[16]; };
struct Event { char event[32]; }; struct Action { char name[4]; double used; };
struct State {int counter; char name[32]; struct Actor a[AQ]; struct Player p[144];
Color lc[3]; int player; Vector3 cam; float cams; struct Point points[AQ]; struct Line lines[AQ];
struct Mesh2 meshes[32];  struct Event log[128]; double time; struct Action actions[12];
Color tex[64 * 64];  unsigned char rec[4096];};

char actions[12][4] = {"W", "A", "S", "D", "E", "Q", "R", "MU", "MR"};
char arr[2][8] = { "box", "sphere" };

struct State s = {0};


short tile(Vector3 v) {
    int id; id = v.x + v.y;
    return (floor(v.y / 123) * 64 + v.x / 123);
}

short collision() {
    short c = s.p[0].controlled;
    short count; count = 0;
    for (short i = 0; i < AQ; i++) {
        if (i != c)
        {
            if (CheckCollisionSpheres(s.a[c].pos, s.a[c].scale.x, s.a[i].pos, s.a[i].scale.x))
            {
                count += 1;
            };
        };
    };
    return count;
};

void turtle(char rule[16], char depth, Vector3 pos, Vector3 rot, short start) {
    int step = start;
    Vector3 A = pos;
    Vector3 B = Vector3Zero();
    for (char i = 0; i < depth; i++) {
        for (char i = 0; i < 16; i++) {
            //if (rule[i] == "") { continue; }
            if (rule[i] == 'f') { B = Vector3Add(A, (Vector3) { 0.0, 17.0, 0.0 }); }
            if (rule[i] == 'r') { B = Vector3Add(A, (Vector3) { 14.0, 0.0, 0.0 }); }
            if (rule[i] == 'l') { B = Vector3Add(A, (Vector3) { -11.0, 0.0, 0.0 }); }
            if (rule[i] == 'b') { B = Vector3Add(A, (Vector3) { 1.0, -4.0, 0.0 }); }
            s.points[step].pos = A;
            s.points[step + 1].pos = B;
            s.lines[step].a = step; s.lines[step].b = step + 1;
            A = B; step += 1;
        }
    }
}

static void reset() {

    for (short i = 0; i < 12; i++) strcpy(s.actions[i].name, actions[i]);

    for (short i = 0; i < AQ; i++) {

        s.a[i].pos.x = GetRandomValue(0, 3999);
        s.a[i].pos.y = GetRandomValue(0, 2999);
        s.a[i].scale.x = GetRandomValue(4, 33);
        s.a[i].ms = MS_BASE;
        strcpy(s.a[i].name, arr[GetRandomValue(0, 1)]);
        if (s.tex[tile(s.a[i].pos)].r == BLACK.r) {
            s.a[i].scale.x = 1; strcpy(s.a[i].name, "..~..");
        };
    };
    s.p[0].controlled = GetRandomValue(0, AQ);
    s.counter = 0;
    s.player = 0;
    s.time = 0;

    for (short i = 0; i < AQ; i++) { s.points[i].pos = (Vector3){ i, GetRandomValue(0, 1233),0.0 }; }
    for (short i = 0; i < AQ / 3; i++) {
        s.lines[i].a = i;
        s.lines[i].b = GetRandomValue(0, 1233);
    }

    turtle("frbrfrbrlbbrr", 24, s.a[s.p[0].controlled].pos, s.a[s.p[0].controlled].rot, 0);
    turtle("llfllflf", 24, s.a[s.p[0].controlled].pos, s.a[s.p[0].controlled].rot, 333);
    turtle("bllflll", 24, s.a[s.p[0].controlled].pos, s.a[s.p[0].controlled].rot, 555);
    turtle("ffrfrflb", 24, s.a[s.p[0].controlled].pos, s.a[s.p[0].controlled].rot, 777);

}

void draw2() {
    for (int i = 0; i < AQ; i++) 
    { DrawPixel(880+s.a[i].pos.x/33.0, 444+s.a[i].pos.y/33.0, s.tex[i]); }
    for (int i = 0; i < AQ; i++)
    {
        DrawPixel(555+(8*(i%64)),  8*(i/64) , s.tex[i]);
    }
    DrawRectangle(880 + s.cam.x / 33, 444 + s.cam.y / 33, 999/33, 555/33, ColorAlpha(BLACK, .5));
}

static void draw()
    {

    int i, x, y;


    for (i = 0; i < AQ; i++) {

        x = s.a[i].pos.x-s.cam.x;
        y = s.a[i].pos.y-s.cam.y;

        s.lc[2] = s.tex[i]; s.lc[2].a = 88;
        if (!strcmp(s.a[i].name, "box")) {
            DrawRectangle(x-s.a[i].scale.x/2, y - s.a[i].scale.x/2, s.a[i].scale.x, s.a[i].scale.x, s.lc[2]);
            DrawRectangleLines(x - s.a[i].scale.x / 2, y - s.a[i].scale.x / 2, s.a[i].scale.x, s.a[i].scale.x, s.tex[i]);
            DrawPixel(x, y, WHITE); 
        };
        if (!strcmp(s.a[i].name, "sphere")) {
            DrawCircle(x, y, s.a[i].scale.x, s.lc[2]);
            DrawPixel(x, y, GREEN); 
        };
        DrawText(TextFormat("%s, %i", s.a[i].name, i), x+20, y, 10, s.tex[i]); }

    for (i = 0; i < AQ; i++) {
        Vector3 VA = s.points[s.lines[i].a].pos;
        Vector3 VB = s.points[s.lines[i].b].pos;
        DrawLine3D((Vector3) {VA.x - s.cam.x, VA.y - s.cam.y },
            (Vector3) { VB.x - s.cam.x, VB.y - s.cam.y }, ColorFromHSV(12,1,1)); }

    for (i = 0; i < AQ; i++) {
        DrawPixel(s.points[i].pos.x - s.cam.x, s.points[i].pos.y - s.cam.y, MAGENTA);
    }

}

void drawUI(int id, Texture texture, short collisions, float distance, int x, int y) {

    DrawTexture(texture, 0, 450, WHITE);
    DrawText("npuBeT", 11, 22, 20, GRAY);
    DrawText("snrd07A3", 999 - 55, 4, 10, GRAY);
    DrawText(TextFormat("%i", s.counter), 22, 44, 40, GRAY);
    DrawText(TextFormat("%i", AQ), 188, 44, 40, GRAY);
    DrawText(TextFormat("%i %f", collisions, distance), 188, 88, 20, GRAY);
    DrawText(TextFormat("%f, %i", s.a[id].pos.x, tile(s.a[id].pos)),
        188, 111, 20, s.tex[tile(s.a[id].pos)]);
    DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
    DrawText(TextFormat("%f", GetFrameTime() * 1000.0f), 64, 4, 10, PINK);
    DrawText(TextFormat("%i", (int)s.time), 4, 16, 10, PINK);

    DrawText(TextFormat("*", (int)floor(s.cam.x + x),
        (int)floor(s.cam.y + y)), x - 10, y - 16, 40, BLACK);

    for (int i = 0; i < 16; i++) { DrawText(s.log[i].event, 777, 444 + i * 10, 10, GRAY); };
    for (int i = 0; i < 12; i++) DrawText(s.actions[i].name, 0, 80 + i * 16, 10, PINK);
}

int main(int argc, char* argv[])
{

    short i, x, y, r, id, collisions; float distance;
    SetTargetFPS(2222);
    SetAudioStreamBufferSizeDefault(BUFF);
    InitWindow(1024, 555, "SN24C");
    InitAudioDevice();
    AudioStream str[16]; for (i = 0; i < 4; ++i) {
        str[i] = LoadAudioStream(44100, 16, 1);
        SetAudioStreamVolume(str[i], .1);
    }

    struct State *data = malloc(sizeof(struct State)*11);

    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    for (int i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
    for (int i = 0; i < 4096; i++) { s.rec[i] = sin(i/16.0)*126.0; }
    UnloadImage(image); UnloadImageColors(colors);

    reset();

    while (!WindowShouldClose())
    {
        s.time += GetFrameTime();
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

        ClearBackground((Color){0,12,44,255});
        draw2(); 
        draw();
        drawUI(id, texture, collisions, distance, x, y);

        s.cam.x = s.a[id].pos.x - 300; s.cam.y = s.a[id].pos.y - 300;
        Vector2 vec =
        { 0 - s.cam.x, 0 - s.cam.y };
        Color g; g = GRAY; g.a = 55;
        DrawTextureEx(texture, vec, 0, 123, g);

        EndDrawing();

       if (!IsAudioStreamPlaying(str[0]) & IsAudioStreamReady(str[0])) {PlayAudioStream(str[0]);}
       if (IsAudioStreamProcessed(str[0])) { UpdateAudioStream(str[0], &s.rec[x*64], BUFF); }
       ;

    }

    CloseAudioDevice();         

    return 0;
}