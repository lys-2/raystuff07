
#include "raylib.h"
#include <raymath.h>
#if defined(_WIN32)           
//#include <winstuff.c>
#endif

#define AQ 1234
#define BUFF 4096
#define MS_BASE 300.0f

struct Point { Vector3 pos; }; struct Line { int a; int b; };
struct Triangle { int a; int b; int c; };
struct Mesh2 { short start; short end; };
struct Player { char name[8]; int score; int controlled; };

struct Actor {
    char name[8]; short lvl; Vector3 pos; Vector3 rot; Vector3 scale; float ms;
    int overlap[2]; bool hidden; 
};
struct Scene { struct Actor actors[16]; };
struct Event { char event[32]; };
struct Action { char name[4]; double used; };
struct State {int counter; char name[32];
struct Actor alib[16]; struct Actor a[AQ];
struct Player p[144];
Color lc[3];
int player;
Vector3 cam;
float cams;
struct Point points[AQ];
struct Line lines[AQ];
struct Mesh2 meshes[32];
struct Event log[128];
double time;
struct Action actions[12];
Color tex[64 * 64];
unsigned char rec[4096];};

char actions[12][4] = {"W", "A", "S", "D", "E", "Q", "R"};

struct State s = {
    .alib = {
        {"box", 1},
        {"sphere", 2},
        {"player", 1},
        {"camera"},
        {"saa", 3}
      },
    .actions = {{"a"}, {"b"}},
    .cams = 1
};

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
        int pre = GetRandomValue(0, 1);
        s.a[i].hidden = false;
        s.a[i].pos.x = GetRandomValue(0, 3999);
        s.a[i].pos.y = GetRandomValue(0, 2999);
        s.a[i].scale.x = GetRandomValue(4, 33);
        s.a[i].ms = MS_BASE;
        s.a[i].lvl = s.alib[pre].lvl;
        strcpy(s.a[i].name, s.alib[pre].name);
        if (s.tex[tile(s.a[i].pos)].r == BLACK.r) {
            s.a[i].scale.x = 1; strcpy(s.a[i].name, "..~..");
            //s.a[i].hidden = true;
        };
    };
    s.p[0].controlled = GetRandomValue(0, AQ);
    s.counter = 0;
    s.player = 0;
    s.time = 0;
    s.actions[6].used = GetTime();

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
        struct Vector3 v = s.a[i].pos;
        v = Vector3RotateByAxisAngle(v, (Vector3) { .2, 1, .1 }, s.time);
        v = Vector3Add(v, (Vector3) { 1111, 0, 0 });
        DrawPixel(880 + v.x / 33.0, 333 + v.y / 33.0, s.tex[i]);
    }
    for (int i = 0; i < AQ; i++)
    {
        DrawPixel(555+(8*(i%64)),  8*(i/64) , s.tex[i]);
    }
    DrawRectangle(880 + s.cam.x / 33, 444 + s.cam.y / 33, 999/33, 555/33, ColorAlpha(BLACK, .5));
}

static void draw()
    {

    int i;
    float x, y;

    for (i = 0; i < AQ; i++) {

        if (s.a[i].hidden) continue;
        x = (s.a[i].pos.x-s.cam.x)*s.cams;
        y = (s.a[i].pos.y-s.cam.y)*s.cams;

        s.lc[2] = s.tex[i]; s.lc[2].a = 88;
        if (!strcmp(s.a[i].name, "box")) {
            DrawRectangle(x-s.a[i].scale.x * s.cams /2.0, y - s.a[i].scale.x * s.cams /2.0,
                s.a[i].scale.x*s.cams, s.a[i].scale.x * s.cams, s.lc[2]);
            DrawRectangleLines(round(x - s.a[i].scale.x * s.cams / 2.0),
                round(y - s.a[i].scale.x * s.cams / 2.0),
                s.a[i].scale.x * s.cams, s.a[i].scale.x * s.cams, s.tex[i]);
            DrawPixel(x, y, WHITE); 
        };
        if (!strcmp(s.a[i].name, "sphere")) {
            DrawCircle(x, y, s.a[i].scale.x*s.cams, s.lc[2]);
            DrawPixel(x, y, GREEN); 
        };
        DrawText(TextFormat("%s, %i, lvl: %i", s.a[i].name, i, s.a[i].lvl), x + 20.0, y, 10, s.tex[i])
            ; }

    for (i = 0; i < AQ; i++) {

        Vector3 VA = s.points[s.lines[i].a].pos;
        Vector3 VB = s.points[s.lines[i].b].pos;
        DrawLine3D((Vector3) {(VA.x-s.cam.x)*s.cams, (VA.y-s.cam.y)*s.cams },
            (Vector3) {(VB.x-s.cam.x)*s.cams, (VB.y-s.cam.y)*s.cams  }, ColorFromHSV(12,1,1)); }

    for (i = 0; i < AQ; i++) {
        x = (s.points[i].pos.x - s.cam.x) * s.cams;
        y = (s.points[i].pos.y - s.cam.y) * s.cams;

        DrawPixel(x, y, MAGENTA);
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
    DrawText(TextFormat("%f", s.time), 4, 32, 10, PINK);

    DrawText(TextFormat("*", (int)floor(s.cam.x + x),
        (int)floor(s.cam.y + y)), x - 10, y - 16, 40, BLACK);

    for (int i = 0; i < 16; i++) { DrawText(s.log[i].event, 777, 444 + i * 10, 10, GRAY); };
    for (int i = 0; i < 12; i++) DrawText(s.actions[i].name,
        0, 80 + i * 22, 20, ColorContrast(PINK,  s.actions[i].used-GetTime()+1));

}

void action();
void log();

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

    void* data = malloc(sizeof(struct State) * 1111); data = 1;

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
        distance = ((((s.tex[tile(s.a[id].pos)].g*1.0)+14.0)/33.0)/4.0)*s.a[id].ms/(collisions+1.0)
            * GetFrameTime();
        if (IsKeyDown(KEY_R)) { reset(); }
        if (IsKeyPressed(KEY_F)) { s.cams /= 2; }
        if (IsKeyPressed(KEY_G)) { s.cams *= 2; }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) 
        { s.a[id].pos.y -= distance; s.actions[0].used = GetTime();}
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) 
        {s.a[id].pos.y += distance; s.actions[2].used = GetTime();} 
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        { s.a[id].pos.x += distance; s.actions[3].used = GetTime(); }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) 
        {s.a[id].pos.x -= distance; s.actions[1].used = GetTime();}
        if (IsKeyDown(KEY_T) || IsKeyDown(KEY_A))
        {}
        if (IsKeyPressed(KEY_E)) {
            s.actions[4].used = GetTime();  if (data != NULL) { data = NULL; free(data); };
            UpdateAudioStream(str[1], s.rec, BUFF);
        SetAudioStreamPitch(str[1], GetRandomValue(4, 16));
        PlayAudioStream(str[1]);
    }

        s.cams *= 1+GetMouseWheelMove()/2;
        
        SetAudioStreamVolume(str[0], 0);
        if (IsKeyDown(KEY_Q)) { SetAudioStreamVolume(str[0], .1); s.actions[5].used = GetTime();
        SetAudioStreamPitch(str[0], y / 333.0);
        i = 0; for (i = 0; i < 128; i++) {
            r = GetRandomValue(1, y / 20);
            DrawText(&s.rec[x * r], 990 - MeasureText(&s.rec[x * r], 10), i * 10, 10, GRAY);
        }
        };

        for (int i = 0; i < AQ; i++) { s.a[i].pos.z = s.tex[tile(s.a[i].pos)].r*3.0; }


        BeginDrawing();
        s.cam.x = s.a[id].pos.x -300.0/s.cams; s.cam.y = s.a[id].pos.y - 300.0/s.cams;

        ClearBackground((Color){0,12,44,255});
        draw();
        drawUI(id, texture, collisions, distance, x, y);
        Vector2 vec =
        { (0 - s.cam.x)*s.cams, (0 - s.cam.y) * s.cams };
        Color g; g = GRAY; g.a = 55;
        DrawTextureEx(texture, vec, 0, 123 * s.cams, g);

        draw2();

        EndDrawing();

       if (!IsAudioStreamPlaying(str[0]) & IsAudioStreamReady(str[0])) {PlayAudioStream(str[0]);}
       if (IsAudioStreamProcessed(str[0])) { UpdateAudioStream(str[0], &s.rec[x*64], BUFF); }
       ;

    }

    CloseAudioDevice();         

    return 0;
}