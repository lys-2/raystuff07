#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef long long ln;
typedef double db;
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

struct actor3 { char name[8]; int x; int y; bool spawned; };

struct user { char name[16], paw[16], progress; int score; };
struct actor2 {
    char name[16], type[8], parent; short stack; char lvl, state[8],
        tags[8], user, controlled, sock[4], party[8], path, gen, age, height, weight,
        deep, thick, cover, rain, wet, light;
    bool spawned; short hp; int cost;
};
struct actor2 ad = { "dummy", "item", 0, 1, 1, .hp = 10 };
struct actor2 scene[32] = {
    {"woods", "location"},
    {"road sign", "location"},
    {"castle", "location"},
    {"dungeon", "location"},
    {"player", "char", 3, .user = 0, .controlled = 0, .stack = 1, .lvl = 1, .hp = 10, },
    {"junk robot", "item"},
    {"base set", "item", 4, .tags = "B", .sock = "B1"},
    {"wood set", "item", 3, .lvl = 3, .tags = "B"},
    {"chip", "item", 3, .stack = 10'000},
    {"chip", "item", 4, .stack = 10'000, .sock = "P1"},
    {"merchant", "char"},
    {"monster", "char", 0},
    {"hoe", "item", .tags = "M"},
    {"box", "item",},
    {"hockey stick", "item", .lvl = 2, .tags = "M"},
    {"leaf", .stack = 121 },
    {"leaf", .stack = -1 }
};

struct line2 { char name[16]; char text[64]; char tag[16], tag2[16]; };
struct line2 lines[20] = {
    {"junk robot", "Hello, try some (a)ction and have fun!"},
    {"Player", "Okay"},
    {"Junk robot", "Yes"},
    {"Junk robot", "We are in a dungeon"},
    {"junk robot", "time to level up and explore"},
    {"junk robot", "some items and stuff and magic"},
    {"junk robot", "i'm a junk robot who talks a lot"},
    {"junk robot", "have to keep it up with all the conversations"},
    {"junk robot", "keep it up too"},
    {"junk robot", "why not try this so-called weapon item on"},
    {"junk robot", "It is lying somewhere there"},
    {"junk robot", "then smash some boxes just as usual"},
    {"junk robot", "here we go", "W[HOE]", "R[EXP][100]"},
    {"junk robot", "mind if we go outside?"},

    {"merchant", "i'm up there", "J[128]"},

    {"merchant", "i'm up there", "J[253]"},

    {"junk robot", "What about this huge and evil monster over there"},
    {"junk robot", "Done with it!"},

};

struct weather { char temp, cloud, rain, wind; };

double PI = 3.141592;
double PIa = 1.0 / 3.141592;
struct v3  { double x, y, z; };
struct hit { struct v3 loc, normal; };
struct color { char r, g, b, a; };
struct point { struct v3 t; double u, v; struct color a, l; };
struct line { struct point a, b; };
struct triangle { struct point a, b, c; };
struct triangle mesh1[8] = {
    {{111,111}, {222,0}, {0,111}},
    {{0,0}, {111,111}, {55,0}},
    {{33,200}, {55,200}, {55,150}},
    {{111,111}, {155,155}, {155,111}},
    {{-33,33}, {33,33}, {-33,-33}},
    {{33,33}, {-33,-33}, {33,-33}},
};


struct actor {
    char name[16];
    struct v3 l, r, s;
    db lvl;
    bool spawned;
    struct actor* parent;
};
struct state { 
    ln size;
    struct actor scene[99999];
    struct actor path[99];
    struct actor cam;
    struct actor player;
    struct line2 lines[255];
    struct actor2 s[255];
    struct user user;
    char turn;

    char name[8];
    int score;
    int spawned;
    int time;
    int x, y;
    int frame;
    char msg[128];
    struct actor3 scene3[12118];

};
struct actor a;
struct state g, def;
void spawn(struct actor a) {
    a.spawned = true;
    g.scene[rand() % 1024] = a;
    // memcpy(&g.scene[rand() % 1024], &a, sizeof(a));
}

static bool quit = false;
ln i, f, p;
char buff[64];
struct {
    int width;
    int height;
    struct color *pixels;
} frame = { 0 };
void clear() {
    for (ln i = 0; i < 496 * 216; i++) {
        frame.pixels[i].b = 0;
        frame.pixels[i].g = 0;
        frame.pixels[i].r /= 1.000001;
    }
}

void point(double x, double y, double z, char c) {

    if ((x >= 0 && x < 496) && (y >= 0 && y < 216)) {
        frame.pixels[(ln)(round(x) + (round(y) * 496))].b = 255;
        frame.pixels[(ln)(round(x) + (round(y) * 496))].r = 111;
        p += 1;
    }

}

double lerp(double a, double b, double f) { return a * (1.0 - f) + (b * f); }

void line(struct point a, struct point b) {

    for (ln i = 0; i <= 32; i++) {
        point(
            lerp(a.t.x, b.t.x, i/32.0),
            lerp(a.t.y, b.t.y, i/32.0),
            0,
            232);
    }
}

struct triangle trans(struct triangle tri, struct v3 v) {
    tri.a.t = v;
    tri.b.t = v;
    tri.c.t = v;
}

void tri(struct triangle tri) {
    for (ln it = 0; it <= 111; it++) {
         line(
            (struct point) {tri.a.t.x,tri.a.t.y},
            (struct point) {
             lerp(tri.b.t.x, tri.c.t.x, it / 111.0),
            lerp(tri.b.t.y, tri.c.t.y, it/111.0)
        }
        );
    };
    // line((struct point) { tri.b.t.x, tri.b.t.y }, (struct point) { tri.c.t.x, tri.c.t.y });
}

void mesh(struct triangle *mesh) {
    for (i = 0; i < 8; i++) {
        tri(mesh[i]);
    }
}

struct v3 rotate_point(struct v3 v, struct v3 o, float r) {
    float vx = v.x;
    float vy = v.y;
    v.x = (vx - o.x) * cos(r) - (vy - o.y) * sin(r);
    v.y = (vx - o.x) * sin(r) + (vy - o.y) * cos(r);
    return (struct v3) { v.x + o.x, v.y + o.y };
}

void ring(struct v3 a, struct v3 o, db r) {
    for (int pixel = 0; pixel < 111; pixel++) {
        struct v3 v = rotate_point((struct v3) { a.x + r, a.y }, a, pixel);
        point(v.x,v.y,v.z,255);
    }
};
char rule[64];
void turtle() {};

void ray2sphere() {};
void ray2tri() {};

bool flip() { return rand() % 2; }
bool weight(float chance) { return rand() < chance * RAND_MAX; }
bool flipn(int n) {
    bool result = true;
    for (int i = 0; i < n; ++i) if (!flip()) result = false;
    return result;
}
float rf() { return rand() / (float)RAND_MAX; }
float rflog() { return -1.0 / log(rf()); }
int get_level(int exp) { int lvl = 1 + floor(exp / 1000); return lvl; }

RECT rect;
 PAINTSTRUCT paint;
HDC device_context, src;
void console() {
    FILE* conin = stdin;
    FILE* conout = stdout;
    FILE* conerr = stderr;
    AllocConsole();
    freopen_s(&conin, "CONIN$", "r", stdin);
    freopen_s(&conout, "CONOUT$", "w", stdout);
    freopen_s(&conerr, "CONOUT$", "w", stderr);
    SetConsoleTitleA("Drawner");
}

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);
#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

 BITMAPINFO frame_bitmap_info;
 HBITMAP frame_bitmap = 0;
 HDC frame_device_context = 0;
 HWND window_handle, win2;
 bool resize;

 struct event { float time, tone, duration, volume; };
 struct event graph[99];
 char sampler[16][8000];
 unsigned char sound[2][8000];
 bool chunk = 1;
 HWAVEOUT hWaveOut;
 WAVEHDR header[2] = {
 {sound[0], 8000, 0, 0, 0, 0, 0, 0},
 {sound[1], 8000, 0, 0, 0, 0, 0, 0},
 };
 WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };

 float sample(float time, float start, float dur) {
     return time - start;
 }
 bool hit(float time, float start, float dur) {
     if (time > start && time < (start + dur))  return 1;
     return 0;
 }

 char sgen() {
     for (int e = 0; e < 33; ++e) {
         graph[e] = (struct event){
             (rand() % 111) / 111.0,
             440 + (440 * (rand() % 4) / 3.00),
             (rand() % 128) / 1555.0 ,
             (rand() % 128) / 128.0
         }
         ;
     };

     for (int t = 0; t < 8000; ++t)
     {
         sampler[0][t] = sin((t / 8000.0) * 3.1415 * 2 * 440.0) * 12.0;
         //  printf("%i ", sampler[0][t]);
     }

     for (int t = 0; t < sizeof(sound[0]); ++t) {
         sound[0][t] = 128;
         for (int e = 0; e < 99; ++e) {
             if (hit(t / 8000.0, graph[e].time, graph[e].duration)) {
                 sound[0][t] += graph[e].volume * sampler[0][
                     (int)round((graph[e].tone / 440.0 * 8000.0)
                         * sample(t / 8000.0, graph[e].time, graph[e].duration))
                 ];

                 //  printf("%i \n", 
                   //    (int)round(8000*sample(t / 8000.0, graph[e].time, graph[e].duration)));
             }

         }
         //  sound[0][t] = 128 + sin(t / 32.0) * 128.0;
         // printf("%i ", sound[0][t]);
     }
     for (int t = 0; t < sizeof(sound) / 2; ++t) {
         sound[1][t] = 128 + sin((t / 500.0) * 3.1415 * 2 * 440 * (t / 1111.0)) * 3.0;
         //  printf("%i ", sampler[0][t]);
     }
 }

 char splay() {
     chunk = !chunk;
     waveOutPrepareHeader(hWaveOut, &header[chunk], sizeof(WAVEHDR));
     waveOutWrite(hWaveOut, &header[chunk], sizeof(WAVEHDR));
     // waveOutUnprepareHeader(hWaveOut, &header[chunk], sizeof(WAVEHDR));
   // waveOutClose(hWaveOut);
 }

 void init() {

     memcpy(g.lines, lines, sizeof(lines));

    // memcpy(&g_default, &g, sizeof(struct game));
     memcpy(&g.name, "User", sizeof("User"));
     g.spawned = 32;
     for (i = 0; i <= 32; i++) {
         g.scene3[i].x = rand() % (512 - 64);
         g.scene3[i].y = rand() % (256 - 64);
         g.scene3[i].spawned = true;

     }

     for (int i = 0; i < 255; i++) {
         // g.s[i] = ad;
         g.s[i].hp = rand() % 4 + g.s[i].lvl * 10;
         if (i < 16) {
             g.s[i] = scene[i];
             if (!g.s[i].lvl) g.s[i].lvl = 1;
             if (!g.s[i].name[0]) g.s[i].name[0] = 'D';
             if (!g.s[i].stack) g.s[i].stack = 1;
             g.s[i].spawned = true;
             g.s[i].hp = 3 - rand() % 8 + g.s[i].lvl * 10 + (g.s[i].type[0] == 'l') * 1100;
         }
     }

     for (int i = 0; i < 1024; i++) {
         g.scene[i].l.x = rand() % 495;
         g.scene[i].l.y = rand() % 215;
         g.scene[i].l.z = i;
         g.scene[i].spawned = true;
     }


 };

 void restart() {
     g = def;
     init();
     g.user.progress = 33;
 }

 void parse(char* msg) {
     if (msg[0] == 'a') { g.user.progress += 1; }
     if (msg[0] == 'r') { g = def; init(); }
     if (msg[0] == 'f') { g = def; }
     if (msg[0] == 's') {

         splay();
     }
     if (msg[0] == '\0') { return; }
     g.turn += 1;
 }
 
 void fun() {

     while (1) {

         for (short i = 32; i  >= 0; i--) {
             if (g.s[i].spawned)
                 printf("%s[%c][%s], tg[%s}, %i, %i, %i\n",
                     g.s[i].name, g.s[i].type[0], g.s[g.s[i].parent].name,
                     g.s[i].tags,
                     g.s[i].stack, g.s[i].lvl, g.s[i].hp
                 )
                 ;
         }

         printf("%s: %s. \n", g.lines[g.user.progress].name, g.lines[g.user.progress].text);
         printf("[%s], T[%i], Progress: %i/%i\n", g.msg, g.turn, g.user.progress,
             sizeof(g.lines) / sizeof(struct line2));
         fgets(g.msg, 64, stdin); g.msg[strcspn(g.msg, "\n")] = 0;
         parse(g.msg);
     }
 }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {

    HDC win = GetDC(0);
    timeBeginPeriod(1);
    console();


    // srand((int)time(NULL));
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    init();

   sgen(); splay(); splay();
    CreateThread(0, 0, fun, 0, 0, 0);

    rect.bottom = 164;
    rect.top = f % 222;
    rect.right = 164;
    rect.left = 16;

     WNDCLASS window_class = { 0 };
     window_class.style = 0;
    window_class.lpfnWndProc = WindowProcessMessage;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = L"My Window Class";
    window_class.hbrBackground = NULL;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&window_class);

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    window_handle = CreateWindow(L"My Window Class", L"Drawing Pixels",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        111, 111, 512, 256, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }
    SetWindowPos(window_handle, 0, 111, 111, 512, 256, 0);

    //win2 = CreateWindow(L"My Window Class", L"buttons",
    //    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    //    222, 444, 512, 256, NULL, NULL, hInstance, NULL);
    //SetWindowPos(win2, 0, 654, 111, 256, 256, 0);

    HWND hwndButton[16];
    ln i;
    for (i = 0; i < 4; i++) {
        sprintf_s(buff, 16, " B%i", i);
        hwndButton[i] = CreateWindowA("Button", buff,
            WS_VISIBLE | WS_CHILD, 0, i * 32, 80, 25,
            win2, (HMENU)2, NULL, NULL);
        hwndButton[i+4] = CreateWindowW(L"Edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            100, i * 32, 60, 20, win2, (HMENU)1,
            NULL, NULL);
        hwndButton[i + 8] = CreateWindowW(L"button1", NULL,
            WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
            200, i * 32, 16, 20, win2, (HMENU)1,
            NULL, NULL);

    }

    HBITMAP pic = LoadImage(0, L"actor.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    src = CreateCompatibleDC(0);
    SelectObject(src, pic);

    RECT rect2 = { 0,0, 1256,16 };
    RECT rect3 = { 0,16, 1256,32 };



    while (!quit) {

         MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            DispatchMessage(&message);
        }
        f += 1;

     clear();
    // BeginPaint(window_handle, &paint);


     for (ln i = 0; i < 29; i++) {
         g.path[i] = (struct actor){ "sphere", .l.x = 111, .l.y = 111, .s.x = 10};
     }

     for (ln i = 0; i < 496 * 217; i++) {
         int w = i % 496;
         int h = i / 496;
         frame.pixels[i].g = 255*w/ 496;
        // frame.pixels[i].r = 0;
         frame.pixels[i].b = 255 * h / 217;
     }

    for (ln i = 0; i < 1024; i++) {
         
            if (g.scene[i].spawned==true) {
                point(g.scene[i].l.x, g.scene[i].l.y, 0, rand() & 255);
             //   RECT r = { g.scene[i].t.x, g.scene[i].t.y, g.scene[i].t.x + 16,g.scene[i].t.y + 16 };
                //DrawTextA(frame_device_context, "A", -1, &r, DT_SINGLELINE);
            }
        }
        line((struct point) { 0, 0 }, (struct point) {
            cos(f / 33.0) * 222 - sin(f / 33.0) * 33,
                sin(f / 33.0) * 222 + cos(f / 33.0) * 111
        });


       // tri(mesh1[0]);
        ring(
            mesh1[0].a.t,
        mesh1[3].a.t,
            33
        );

         mesh(mesh1);

        sprintf_s(buff, 62, "Drawner %i", (int)f, (int)p);
        SetWindowTextA(window_handle, buff);

      //  BitBlt(frame_device_context, 0, 111, 64, 64, src, 0, 0, SRCCOPY);

        for (int i = 1; i <= 128; i++) {
            if (g.scene3[i].spawned) {
                if (
                    (g.x > g.scene3[i].x
                        && g.x < g.scene3[i].x + 32) &&

                    (g.y > g.scene3[i].y
                        && g.y < g.scene3[i].y + 32
                        )) {
                    g.scene3[i].spawned = false;
                    g.spawned -= 1;
                    g.user.progress += 1;
                    printf("%s: %s. \n", g.lines[g.user.progress].name, g.lines[g.user.progress].text);
                    printf("[%s], T[%i], Progress: %i/%i\n", g.msg, g.turn, g.user.progress,
                        sizeof(g.lines) / sizeof(struct line2));
                    if (!g.spawned) { restart(); }
                }
            };
            if (g.scene3[i].spawned) {
                TransparentBlt(frame_device_context,
                    g.scene3[i].x, g.scene3[i].y, 32, 32, src, 0, 0, 64, 64, RGB(0, 0, 0));
            }
        }

        SetPixel(win, rand() % 16, rand() % 1024, RGB(0, 255, 0));

        sprintf_s(buff, 64, "Hello, %s, scene:%i", g.name, g.spawned);
        DrawTextA(frame_device_context, buff, -1, &rect2, DT_SINGLELINE);
        sprintf_s(buff, 64, " f %i x %i y %i", (int)f, g.x, g.y);
        DrawTextA(frame_device_context, buff, -1, &rect3, DT_SINGLELINE);


    //    sprintf_s(buff, 64, "  asd ", i);
    //    DrawTextA(frame_device_context, buff, -1, &rect, DT_LEFT);

       InvalidateRect(window_handle, NULL, FALSE);
       UpdateWindow(window_handle);


        if (!(rand()%3))Sleep(1);
     //   SetWaitableTimer(waitable_timer, &wait_time, 0, 0, 0, 0);
      //  WaitForSingleObject(waitable_timer, INFINITE);
    }

    return 0;
}

LRESULT CALLBACK WindowProcessMessage(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_LBUTTONDOWN) printf(" LM AA ");
    if (message == WM_KEYDOWN && wParam == 'R') restart();
    if (message == WM_KEYDOWN && wParam == 'Q') fgets(g.name, 64, stdin);
    if (message == WM_LBUTTONUP) printf(" LM2  ");
    if (message == WM_MOUSEMOVE) { g.x = LOWORD(lParam), g.y = HIWORD(lParam); }
    if (message == WM_MOUSEHOVER) printf(" HW  ");

    switch (message) {
    case WM_QUIT:
    case WM_CREATE: {

    } break;
    case WM_DESTROY: {
        quit = true;
    } break;
    case WM_ERASEBKGND: { return 1; break;
    } 

    case WM_PAINT: {

            device_context = BeginPaint(window_handle, &paint);

            BitBlt(device_context,
                paint.rcPaint.left, paint.rcPaint.top,
                paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                frame_device_context,
                paint.rcPaint.left, paint.rcPaint.top,
                SRCCOPY);

            EndPaint(window_handle, &paint);
            break;
    }

    case WM_MOUSEMOVE: { 
        if (window_handle == win2) break;

      //  printf_s(" x%i y%i", LOWORD(lParam), 216-HIWORD(lParam));
        struct actor a;
        a.l.x = LOWORD(lParam);
        a.l.y = 216 - HIWORD(lParam);
        spawn(a);

    } break;

    case WM_SIZE: {
        if (window_handle == win2) break;
        if (resize) break;
        resize = !resize;
        frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
        frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

        if (frame_bitmap) DeleteObject(frame_bitmap);
        frame_bitmap =
            CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
        SelectObject(frame_device_context, frame_bitmap);

        frame.width = LOWORD(lParam);
        frame.height = HIWORD(lParam);
    } break;

    default: {
        return DefWindowProc(window_handle, message, wParam, lParam);
    }
    }
    return 0;
}