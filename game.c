
#include "raylib.h"
#include <raymath.h>
#if defined(_WIN32)           
//#include <winstuff.c>
#endif


#define AQ 4096
#define BUFF 4096
#define MS_BASE 300.0f
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

struct Point { Vector3 pos; char group[4]; };
struct Line { int a; int b; char group[4]; };
struct Triangle { int a; int b; int c; char group[4]; };
struct Mesh2 { struct Line lines[512]; struct Point pivot; short size; };
struct User { char name[8]; char age;  };
struct Player { 
    char name[8]; int score; int controlled; bool is_selecting;
short highlight[128]; short selected[128];
Vector2 SA;
Vector2 SB;
};

enum Mode {play, input};
enum Class {ELF, MELEE};

struct Actor {
    unsigned char name[8]; unsigned char lvl; Vector3 pos; Vector3 rot; Vector3 scale; float ms;
    int overlap[2]; unsigned char stack;
    enum Class class; short from; short to[16];
     bool hidden; bool high; bool selected; bool free;
};

struct Action { char name[4]; float used; };

struct AKey { int key; short author; float time; };
struct ASay { char message[64]; short author; float time; };
struct AMove { Vector2 move; short author; float time; bool mouse; bool turn; };
struct ASelect { Vector2 A; Vector2 B; short author; float time; };

struct State {int counter; char name[32];
struct Actor alib[16]; struct Actor a[AQ];
struct Player p[144];
enum Mode mode;
Color lc[3];
int player;
Vector3 cam;
float cams;
void* data;
struct Point points[AQ];
struct Line lines[AQ];
struct Mesh2 meshes[32];
struct Mesh mesh;
double time;
struct Action actions[12];
Color tex[64 * 64];
unsigned char rec[4096];
struct AKey key[16]; char key_cur;
struct ASay say[512]; char say_cur;
};

char keys[12][4] = {"W", "A", "S", "D", "E", "Q", "R"};

struct State s = {
    .alib = {
        {"box", 1},
        {"ball"},
        {"player", 1},
        {"camera"},
        {"light"},
        {"saa", 3}
      },
    .actions = {{"a"}, {"b"}},
    .mode = play,
    .cams = 1
};


void tick(short actor) { 

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
    Vector3 A = Vector3Zero();
    Vector3 B = Vector3Zero();
    for (char i2 = 0; i2 < depth; i2++) {
        for (char i = 0; i < 16; i++) {
            //if (rule[i] == "") { continue; }
            if (rule[i] == 'f') { B = Vector3Add(A, (Vector3) { 0.0, 17.0, 0.0 }); }
            if (rule[i] == 'r') { B = Vector3Add(A, (Vector3) { 14.0, 0.0, 0.0 }); }
            if (rule[i] == 'l') { B = Vector3Add(A, (Vector3) { -11.0, 0.0, 0.0 }); }
            if (rule[i] == 'b') { B = Vector3Add(A, (Vector3) { 1.0, -4.0, 0.0 }); }
            if (rule[i] == 'q')
            { B = Vector3Add(A, (Vector3) { GetRandomValue(-15, 15), GetRandomValue(-15, 15), 0.0 }); }
            s.points[step].pos = A;
            s.points[step + 1].pos = B;
            s.lines[step].a = step; s.lines[step].b = step + 1;
            strcmp(s.lines[step].group, "aaaa");
            s.meshes[0].lines[step] = s.lines[step];
            A = B; step += 1;
        }
    }
}

static void reset() {

    for (short i = 0; i < 12; i++) strcpy(s.actions[i].name, keys[i]);

    for (short i = 0; i < AQ; i++) {

        int rand1 = GetRandomValue(0, 1);
        int rand2 = GetRandomValue(1, 14);
        s.a[i].hidden = false;
        s.a[i].rot = (Vector3){0,0,GetRandomValue(0, PI*2)};
        s.a[i].pos.x = GetRandomValue(0, 3999);
        s.a[i].pos.y = GetRandomValue(0, 2999);
        s.a[i].scale = (Vector3){ rand2,rand2,rand2 };
        s.a[i].ms = MS_BASE;
        s.a[i].lvl = s.a[i].scale.x;
        strcpy(s.a[i].name, s.alib[rand1].name);
        if (s.tex[tile(s.a[i].pos)].r == BLACK.r) {
            s.a[i].scale = (Vector3){1,1,1}; strcpy(s.a[i].name, "..~..");
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

    for (short i = 0; i < AQ; i++) { s.a[i].pos = s.points[i].pos; s.a[i].pos.y += 3; }


    turtle("frbqqqbrlqqr", 24, s.a[s.p[0].controlled].pos, s.a[s.p[0].controlled].rot, 0);


}
Vector3 transform(Vector3 A, Vector3 position, Vector3 scale, Vector3 rotate)
{
    Vector3 B; Matrix M; M = MatrixRotateXYZ(rotate);
    B = Vector3Transform(A, M);
    B = Vector3Multiply(B, scale);
    B = Vector3Add(B, position);
    return B; 
}
void drawmesh(int id, Vector3 position, Vector3 scale, Vector3 rotate, Color color)
{
    Vector3 A, B;
    for (short i = 0; i < 128; i++) {
        A = transform(s.points[s.meshes[id].lines[i].a].pos, position, scale, rotate);
        B = transform(s.points[s.meshes[id].lines[i].b].pos, position, scale, rotate);
        DrawLine ( A.x, A.y,B.x, B.y, color); }
};

void draw2() {
    
    for (int i = 0; i < AQ; i++)
    { DrawPixel(880+s.a[i].pos.x/33.0, 444+s.a[i].pos.y/33.0, s.tex[i]); }
    for (int i = 0; i < AQ; i++)
    {
        struct Vector3 v = s.a[i].pos;
        v = Vector3RotateByAxisAngle(v, (Vector3) { 1, 0, 0 }, 3.14/3);
        v = Vector3Add(v, (Vector3) { 1111, 0, 0 });
        DrawPixel(844 + v.x / 33.0, 400 + v.y / 33.0, s.tex[i]);
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
        if (x > 1111 || x < -100) continue;
        if (y > 666 || y < -100) continue;

        Color c = s.tex[i]; c.a = 88;  
        if (s.a[i].high) { c = GREEN; }
        if (s.a[i].selected) { c = BLUE;}
        if (!strcmp(s.a[i].name, "box")) {
            DrawRectangle(x-s.a[i].scale.x * s.cams /2.0, y - s.a[i].scale.x * s.cams /2.0,
                s.a[i].scale.x*s.cams, s.a[i].scale.x * s.cams, c);
            DrawRectangleLines(round(x - s.a[i].scale.x * s.cams / 2.0),
                round(y - s.a[i].scale.x * s.cams / 2.0),
                s.a[i].scale.x * s.cams, s.a[i].scale.x * s.cams, s.tex[i]);
            drawmesh(0, (Vector3) {
                (s.points[i].pos.x - s.cam.x)* s.cams,
                    (s.points[i].pos.y - s.cam.y)* s.cams,
                    0 }, Vector3Scale(s.a[i].scale, s.cams / 4),
                (Vector3) {
                0, sin(s.time / 222) * 2, s.a[i].rot.z
            }, c);
            DrawPixel(x, y, WHITE); 
        };
        if (!strcmp(s.a[i].name, "ball")) {
            DrawCircle(x, y, s.a[i].scale.x*s.cams, c);

            DrawPixel(x, y, GREEN); 
        };

        Vector3 V = {0, s.a[i].scale.x * s.cams, 0};
        V = transform(V, (Vector3) {x,y,0}, Vector3One(), s.a[i].rot);

        DrawLine(x, y, V.x, V.y, WHITE);
        if (s.cams < .5) continue;
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

    for (i = 0; i < 22; i++) { drawmesh(0,
        (Vector3) {i*44, 444, 0},
        Vector3Scale((Vector3) {1, 1, 1}, sin(s.time) / 10),
        (Vector3) {
        i, i, sin(s.time)*110 }, ColorFromHSV(i * 11, .5, i / 2)); };
}

void drawUI(int id, Texture texture, short collisions, float distance, int x, int y) {

    DrawTexture(texture, 0, 450, WHITE);
    DrawText("npuBeT", 11, 22, 20, GRAY);
    DrawText("snrd07A3", 999 - 55, 4, 10, GRAY);
    DrawText(TextFormat("%i", s.counter), 22, 44, 40, GRAY);
    DrawText(TextFormat("%i", AQ), 188, 8, 10, GRAY);
    DrawText(TextFormat("%f", s.cams), 222, 8, 10, GRAY);
    DrawText(TextFormat("%i %f", collisions, distance), 188, 16, 10, GRAY);
    DrawText(TextFormat("%f, %i", s.a[id].pos.x, tile(s.a[id].pos)),
        188, 32, 20, s.tex[tile(s.a[id].pos)]);
    DrawText(TextFormat("fps: %i", GetFPS()), 4, 4, 10, PINK);
    DrawText(TextFormat("%f", GetFrameTime() * 1000.0f), 64, 4, 10, PINK);
    DrawText(TextFormat("%i", (int)s.time), 4, 16, 10, PINK);
    DrawText(TextFormat("%f", s.time), 4, 32, 10, PINK);
    DrawText(TextFormat("%f", s.time), 4, 32, 10, PINK);

    DrawText(TextFormat("*", (int)floor(s.cam.x + x),
        (int)floor(s.cam.y + y)), x - 10, y - 16, 40, BLACK);

    for (int i = 0; i < 12; i++) DrawText(s.actions[i].name,
        0, 80 + i * 22, 20, ColorContrast(PINK,  s.actions[i].used-GetTime()+1));

}


Vector2 w2s(Vector2 w) { return (Vector2) { (w.x-s.cam.x)*s.cams, (w.y-s.cam.y)*s.cams }; };
Vector2 s2w(Vector2 sc) { return (Vector2) { (sc.x / s.cams) + s.cam.x, (sc.y / s.cams) + s.cam.y }; };

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

    void* data = malloc(sizeof(struct State) * 1111); s.data = data;

    s.mesh = GenMeshCube(14, 15, 16);

    Image image = LoadImage("resources/tex.png");
    Texture2D texture = LoadTextureFromImage(image);
    Color* colors = LoadImageColors(image);
    for (int i = 0; i < 64 * 64; i++) { s.tex[i] = colors[i]; }
    for (int i = 0; i < 4096; i++) { s.rec[i] = sin(i/16.0)*126.0; }
    UnloadImage(image); UnloadImageColors(colors);

    float tx, ty;
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

        tx = 0; ty = 0;

        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))  { s.actions[0].used = GetTime();  ty -= 1; }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  { s.actions[2].used = GetTime();ty += 1; } 
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))  {  s.actions[3].used = GetTime();  tx += 1;}
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { s.actions[1].used = GetTime(); tx -= 1; }


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            s.p[0].is_selecting = true;
            s.p[0].SA = s2w((Vector2){x,y});
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            s.p[0].SB = s2w((Vector2){ x,y });
            Vector2 SA; SA = s.p[0].SA;
            Vector2 SB; SB = s.p[0].SB;
            for (int i = 0; i < AQ; i++) {
                s.a[i].high = false;
                if (CheckCollisionPointRec(
                    (Vector2) {s.a[i].pos.x, s.a[i].pos.y},
                    (Rectangle) {
                    min(SA.x, SB.x),
                        min(SA.y, SB.y),
                        abs(SB.x - SA.x),
                        abs(SB.y - SA.y)
                })) {s.a[i].high = true; }
            };
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

            for (int i = 0; i < AQ; i++) { s.a[i].selected=  s.a[i].high; s.a[i].high = false; }
            s.p[0].is_selecting = false;

        }

        if (s.p[0].is_selecting) {
            Vector2 SA; SA = w2s(s.p[0].SA);
            Vector2 SB; SB = w2s(s.p[0].SB);
            DrawRectangle(SA.x, SA.y, 22, 22, ORANGE);
            DrawRectangle(SB.x, SB.y, 22, 22, GREEN);
            DrawRectangle(SB.x, SB.y, 22, 22, GREEN);
            DrawRectangle(
                min(SA.x, SB.x),
                min(SA.y, SB.y),
                abs(SB.x - SA.x),
                abs(SB.y - SA.y),
                (Color) {
                11, 111, 222, 222
            });
        }
        s.a[id].pos.y += distance*ty; 
        s.a[id].pos.x += distance*tx; 

        if  (!(tx == 0.0 && ty == 0.0)) {
            s.a[id].rot.z = Vector2Angle((Vector2) { 0, 1 }, Vector2Normalize((Vector2) { tx, ty }));
            DrawText(TextFormat("%f", s.a[id].rot.z), 4, 100, 10, PINK);
        }

        if (IsKeyDown(KEY_T) || IsKeyDown(KEY_A))
        {}
        if (IsKeyPressed(KEY_E)) {
            s.actions[4].used = GetTime();  if (data != NULL) { data = NULL; free(data); };
            UpdateAudioStream(str[1], s.rec, BUFF);
        SetAudioStreamPitch(str[1], GetRandomValue(4, 16));
        PlayAudioStream(str[1]);
        SetMousePosition(GetRandomValue(0, 999), GetRandomValue(0, 555));

    }


        s.cams += GetMouseWheelMove()/8.0;
        
        SetAudioStreamVolume(str[0], 0);
        if (IsKeyDown(KEY_Q)) { SetAudioStreamVolume(str[0], .1); s.actions[5].used = GetTime();
        SetAudioStreamPitch(str[0], y / 333.0);
        i = 0; for (i = 0; i < 128; i++) {
            r = GetRandomValue(1, y / 20);
            DrawText(&s.rec[x * r], 990 - MeasureText(&s.rec[x * r], 10), i * 10, 10, GRAY);
        }
        };

        for (int i = 0; i < AQ; i++) { s.a[i].pos.z = s.tex[tile(s.a[i].pos)].r; }


        for (int i = 0; i < AQ; i++) {
            //s.a[i].high = false;

            if (
                CheckCollisionPointCircle(
                    (Vector2) { x, y },
                    (Vector2) { (
                        s.a[i].pos.x-s.cam.x)*s.cams,
                    (s.a[i].pos.y - s.cam.y)* s.cams
            },
                    s.a[i].scale.x*s.cams)
                ) {
                s.a[i].high = true;
                DrawText(TextFormat("%i", i), 111, 111, 20, PINK);
            }
        }

        for (int i = 0; i < AQ; i++) { tick(i); };

        int key =  GetKeyPressed();
        if (key) { s.key[s.key_cur].key = key; s.key_cur += 1; s.key_cur %=16; };

        DrawRectangle(16 + (s.key_cur / 4) * 24, 333 + (s.key_cur % 4) * 24, 16, 16, ORANGE);
        for (int i = 0; i < 16; i++) {
            DrawText(TextFormat("%c", s.key[i].key), 16+(i/4)*24, 333+(i%4)*24, 20, GREEN);
        };

        BeginDrawing();
        s.cam.x = s.a[id].pos.x -300.0/s.cams;
        s.cam.y = s.a[id].pos.y - 300.0/s.cams;

        ClearBackground((Color){0,12,44,255});

        drawUI(id, texture, collisions, distance, x, y);
        Vector2 vec =
        { (0 - s.cam.x)*s.cams, (0 - s.cam.y) * s.cams };
        Color g; g = GRAY; g.a = 55;
        DrawTextureEx(texture, vec, 0, 123 * s.cams, g);

        draw2();
        draw();

        EndDrawing();

       if (!IsAudioStreamPlaying(str[0]) & IsAudioStreamReady(str[0])) {PlayAudioStream(str[0]);}
       if (IsAudioStreamProcessed(str[0])) { UpdateAudioStream(str[0], &s.rec[x*64], BUFF); };

    }

    CloseAudioDevice();         

    return 0;
}