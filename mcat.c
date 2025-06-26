#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

HWND hwnd;
HINSTANCE instance;
int quit = 0;
MSG msg;

char buff[16];

struct actor { char name[8]; int x; int y; bool spawned; };
struct game {
    char name[8];
    int score;
    int spawned;
    int time;
    int x, y;
    int frame;
    char msg[128];
    struct actor scene[12118];
};

struct game g;
struct game g_default;

int i = 0;
void init() {
    memcpy(&g_default, &g, sizeof(struct game));
    memcpy(&g.name, "User", 4);
    g.spawned = 32;
    for (i = 0; i <= 32; i++) {
        g.scene[i].x = rand() % (512 - 64);
        g.scene[i].y = rand() % (256 - 64);
        g.scene[i].spawned = true;

    }
};
void restart() {
    g = g_default;
    init();
}

void paint() {

}

STARTUPINFO si = { sizeof(si) };
PROCESS_INFORMATION pi;

LRESULT WINAPI wndproc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{



    printf(" %i ", g.frame);

    if (msg == WM_QUIT || msg == WM_CLOSE) quit = 1;
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        rect.bottom = 164;
        rect.top = g.frame%222;
        rect.right = 164;
        rect.left = 16;

        DrawTextA(hdc, "...............", -1, &rect, DT_LEFT);

        EndPaint(hwnd, &ps);
        printf(" P  ");
    }
    if (msg == WM_LBUTTONDOWN) printf(" LM AA ");
    if (msg == WM_KEYDOWN && w == 'R') restart();
    if (msg == WM_KEYDOWN && w == 'Q') scanf("%s", g.name);;
    if (msg == WM_LBUTTONUP) printf(" LM2  ");
    if (msg == WM_MOUSEMOVE) { g.x = LOWORD(l), g.y = HIWORD(l); }
    if (msg == WM_MOUSEHOVER) printf(" HW  ");
    return DefWindowProc(hwnd, msg, w, l);
}

void thr() {
    while (1) { 
        for (i = 0; i <= (1111*rand())%10011; i++) {
            // g.frame += 1;
            //printf("qwe %i", g.frame);
           Sleep(1);

        }
    };
}

int main() { 

    for (i = 0; i <= 4; i++) {
        HANDLE thread = CreateThread(NULL, 0, thr, NULL, 0, NULL);
        SetThreadPriority(thread, 1);
    }

    HANDLE hFile = CreateFile(
        "data", // File path
        GENERIC_WRITE,                   // Access mode (write)
        0,                               // Share mode (no sharing)
        NULL,                            // Security attributes
        CREATE_ALWAYS,                   // Creation disposition
        FILE_ATTRIBUTE_NORMAL,           // File attributes
        NULL                             // Template file
    );

    instance = GetModuleHandle(0);

    WNDCLASS window = {
        .hInstance = instance,
        .lpszClassName = "wasd",
        .lpfnWndProc = wndproc,
    };
    window.style = CS_HREDRAW | CS_VREDRAW;
    window.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    window.hCursor = LoadCursor(NULL, IDC_ARROW);
    window.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);

    RegisterClass(&window);

    hwnd = CreateWindow(
        "wasd",        // name of window class 
        "Moth catcher",            // title-bar string 
        WS_OVERLAPPEDWINDOW, // top-level window 
        CW_USEDEFAULT,       // default horizontal position 
        CW_USEDEFAULT,       // default vertical position 
        512,       // default width 
        256,       // default height 
        (HWND)NULL,         // no owner window 
        (HMENU)NULL,        // use class menu 
        instance,           // handle to application instance 
        (LPVOID)NULL);      // no window-creation data 

    ShowWindow(hwnd, 1);

    //ShowWindow(handle2, 1);
    

    HDC wnd = hwnd;
    RECT rect;
    RECT rect2 = { 0,16, 256,32};
    RECT rect3 = { 0, 0, 512, 256 };
    PAINTSTRUCT paint;
    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 12, 22));
    GetClientRect(hwnd, &rect);

    HDC src, dst;
    HBITMAP pic = LoadImage(0, "actor.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //dst = BeginPaint(wnd, &paint);
    src = CreateCompatibleDC(dst);

    SelectObject(src, pic);
    //DeleteDC(src);

    wnd = BeginPaint(hwnd, &paint);
    init();

	while (!quit) {

        g.frame += 1;

        if (g.frame % 3 == 0) {

            FillRect(wnd, &rect3, blueBrush);


            BitBlt(wnd, 0, 111, 512, 256, src, 0, 0, SRCCOPY);

            i = 0;
            for (i = 1; i <= 128; i++) {
                if (g.scene[i].spawned) {
                    if (
                        (g.x > g.scene[i].x
                            && g.x < g.scene[i].x + 32) &&

                        (g.y > g.scene[i].y
                            && g.y < g.scene[i].y + 32
                            )) {
                        g.scene[i].spawned = false;
                        g.spawned -= 1;
                        if (!g.spawned) { restart(); }
                    }
                };
                if (g.scene[i].spawned)
                TransparentBlt(wnd, g.scene[i].x, g.scene[i].y, 32, 32, src, 0, 0, 64, 64, RGB(0, 0, 0));

            }
           // wnd = EndPaint(hwnd, &paint);
            sprintf(buff, "Hello!!, %s, scene:%i", g.name, g.spawned);
            DrawText(wnd, buff, -1, &rect, DT_SINGLELINE);
            sprintf(buff, " f %i x %i y %i", g.frame, g.x, g.y);
            DrawText(wnd, buff, -1, &rect2, DT_SINGLELINE);
            //EndPaint(hwnd, &paint);

        }
        while (PeekMessage(&msg, hwnd, 0, 0, 1))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
         Sleep(12);

    };
	return 0;
};