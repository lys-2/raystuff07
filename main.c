#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

typedef long long ln;
typedef double db;
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

ln words[16][16] = {
    {1,2},
    1,
    2
};

struct v3 { double x, y, z; };
struct color { char r, g, b, a; };
struct point { struct v3 t; double u, v; struct color a, l; };
struct line { struct point a, b; };
struct triangle { struct point a, b, c; };
struct triangle mesh1[8] = {
    {
        {111,111}, {222,0}, {0,111}
    },
    {{0,0}, {111,111}, {55,0}},
    {{33,200}, {55,200}, {55,150}},
    {{111,111}, {155,155}, {155,111}},
    {{-11,11}, {11,11}, {-11,-11}},
    {{11,11}, {-11,-11}, {11,-11}},
};


struct actor {
    char name[16];
    struct v3 t, r, s;
    db lvl;
    bool spawned;
    struct actor* parent;
};
struct state { 
    struct actor scene[99999];
    struct actor cam;
    struct actor player;
};
struct actor a = {0};
struct state g = {0};
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
    ln i;
    for (i = 0; i < 496 * 216; i++) {
        frame.pixels[i].b = 0;
    }
}

void point(double x, double y, double z, char c) {
    ln i;
    if ((x >= 0 && x < 496) && (y >= 0 && y < 216)) {
        frame.pixels[(ln)(round(x) + (round(y) * 496))].b = 255;
        frame.pixels[(ln)(round(x) + (round(y) * 496))].r = 111;
        p += 1;
    }

}

double lerp(double a, double b, double f) { return a * (1.0 - f) + (b * f); }

void line(struct point a, struct point b) {
    ln i;
    for (i = 0; i <= 32; i++) {
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

db it;
void tri(struct triangle tri) {
    it = 0;
    for (it = 0; it <= 111; it++) {
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
void turtle() {

};



RECT rect;
static PAINTSTRUCT paint;
static HDC device_context;
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



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {

    timeBeginPeriod(1);
    console();

    rect.bottom = 164;
    rect.top = f % 222;
    rect.right = 164;
    rect.left = 16;

     wchar_t window_class_name[] = L"My Window Class";
     WNDCLASS window_class = { 0 };
     window_class.style = 0;
    window_class.lpfnWndProc = WindowProcessMessage;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = window_class_name;
    window_class.hbrBackground = NULL;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&window_class);

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    window_handle = CreateWindow(window_class_name, L"Drawing Pixels",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        111, 111, 512, 256, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }
    SetWindowPos(window_handle, 0, 111, 111, 512, 256, 0);

    win2 = CreateWindow(window_class_name, L"buttons",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        222, 444, 512, 256, NULL, NULL, hInstance, NULL);
    SetWindowPos(win2, 0, 654, 111, 256, 256, 0);
    if (window_handle == NULL) { return -1; }

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
        hwndButton[i + 8] = CreateWindowW(L"button", NULL,
            WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
            200, i * 32, 16, 20, win2, (HMENU)1,
            NULL, NULL);

    }
    i = 0;
    for (i = 0; i < 1024; i++) {
        g.scene[i].t.x = rand() % 495;
        g.scene[i].t.y = rand() % 215;
        g.scene[i].t.z = i;
        g.scene[i].spawned = true;
    }


    while (!quit) {
         MSG message = { 0 };
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            DispatchMessage(&message);
        }
        f += 1;

       clear();
       ln i;
        for (i = 0; i < 1024; i++) {
            if (g.scene[i].spawned==true) {
                point(g.scene[i].t.x, g.scene[i].t.y, 0, rand() & 255);
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
        sprintf_s(buff, 62, "Drawner %i, %i", (int)f, (int)p);
        SetWindowTextA(window_handle, buff);

       InvalidateRect(window_handle, NULL, FALSE);
       UpdateWindow(window_handle);


      //  if (!(rand()%1))Sleep(15);
     //   SetWaitableTimer(waitable_timer, &wait_time, 0, 0, 0, 0);
      //  WaitForSingleObject(waitable_timer, INFINITE);
    }

    return 0;
}


LRESULT CALLBACK WindowProcessMessage(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
    case WM_QUIT:
    case WM_CREATE: {
        printf_s("  %i ", window_handle);
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

                sprintf_s(buff, 64, "  asd ", i);

                DrawTextA(device_context, buff, -1, &rect, DT_LEFT);

            EndPaint(window_handle, &paint);
            break;
    }

    case WM_MOUSEMOVE: { 
        if (window_handle == win2) break;

        printf_s(" x%i y%i", LOWORD(lParam), 216-HIWORD(lParam));
        struct actor a;
        a.t.x = LOWORD(lParam);
        a.t.y = 216 - HIWORD(lParam);
        spawn(a);

    } break;

    case WM_SIZE: {
        if (window_handle == win2) break;
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