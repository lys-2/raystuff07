#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <windows.h>
#define count 1234

struct frame { int width; int height; unsigned char* pixels; } frame;
struct node { char name[8], is_spawned, is_attached; int x, y, at; };
struct state { 
    struct node scene[count];
    int nodes, frames, spawned;
    char quit, log; };
struct state s, def;

void delete(int id) { 
    s.scene[id].is_spawned = 0; s.spawned--;
   printf("- %i\n", id);
}
int slot() { 
    if (!s.scene[s.nodes%count].is_spawned) return s.nodes%count;
    for (int i = 0; i < count; i++) {
        if (!s.scene[i].is_spawned) return i;
    }
    return -1;
}
void spawn(struct node n) {
    int sl = slot();
    if (sl == -1) return;
    n.is_spawned = 1;
    s.scene[slot()] = n;
    s.spawned++;
    s.nodes++;
     printf("++ %i\n", sl);
}

void point(struct frame f, float x, float y) {
    if (x >= 0 && x < f.width && y >= 0 && y < f.height)
    f.pixels[1 + (int)(x+y*f.width) * 4] = 255;
}

void init() {
    printf("Hi!\n");
};
void reset() { s = def; init(); }

void process(float dt) {

    if (s.frames<count) {
        spawn(
            (struct node) {
            .x = rand(s.frames) % frame.width, .y = rand(s.frames) % frame.height
        }
        );
    }

    for (int i = 0; i < count; i++) {
        if (!s.scene[i].is_spawned) continue;
        if (rand() % 12 == 8) {
            s.scene[i].y -= rand() % 2;
            s.scene[i].x += 2-rand()%5;
            }
        if (s.scene[i].y <= 0) delete(i); 
    }
    s.frames++;
}

void clear(struct frame f) {
    for (int i = 0; i < f.width * f.height; i++) {
        f.pixels[0 + i * 4] = 0;
        f.pixels[1 + i * 4] = 0;
        f.pixels[2 + i * 4] = 0;
        f.pixels[3 + i * 4] = 0;
    }
}

static HDC device_context, frame_device_context;
HFONT font;
void text(char* t, float x, float y) {
    SelectObject(frame_device_context, font);
    SetTextColor(frame_device_context, RGB(255, (s.frames / 22) % 255, 255));
    SetBkMode(frame_device_context, 2);
    SetBkColor(frame_device_context, RGB(0, 111, 111));
    RECT r = { x, y, 234, 32 };
    DrawTextA(frame_device_context, t, -1, &r, 0);
}
void paint(struct frame f) {
    for (int i = 0; i < f.width * f.height; i++) {
        int y = i / f.width;
        int x = i % f.width;
        f.pixels[1 + i * 4] = x % 33;
        f.pixels[i * 4] = (y/5)%77;
    }
    for (int i = 0; i < count; i++) {
        if (s.scene[i].is_spawned) { point(f, s.scene[i].x, s.scene[i].y); }
    }
    char str[64]; sprintf(str, "Hi! %i %i", s.spawned, count);
    text(str, 0, 0);
}

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap;
static PAINTSTRUCT ps;
static WNDCLASS window_class;
static HWND window_handle;
MSG msg;
LRESULT CALLBACK wpm(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    int x = LOWORD(lParam);
    int y = frame.height - HIWORD(lParam);


    if (message == WM_KEYDOWN && wParam == 'R') reset();
    if (message == WM_KEYDOWN && wParam == VK_ESCAPE) s.quit=1;
    if (message == WM_RBUTTONDOWN) printf("R click!\n");
    if (message == WM_LBUTTONDOWN) printf("L click! %i %i\n", x, y);


    switch (message) {
    case WM_QUIT: {} break;
    case WM_DESTROY: {
        s.quit = 1;
    } break;
    case WM_MOUSEMOVE: {
        spawn( (struct node) { .x =x, .y = y } );
    } break;
    
    case WM_PAINT: {

        device_context = BeginPaint(window_handle, &ps);

        clear(frame);
        paint(frame);

        BitBlt(device_context,
            ps.rcPaint.left,
            ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left,
            ps.rcPaint.bottom - ps.rcPaint.top,
            frame_device_context,
            ps.rcPaint.left, ps.rcPaint.top,
            SRCCOPY);

        EndPaint(window_handle, &ps);
      //  SetWindowTextA(window_handle, "snry rpg sn0833");

    } break;

    case WM_SIZE: {

        frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
        frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

        if (frame_bitmap) DeleteObject(frame_bitmap);
        frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
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

HANDLE hConsole;
HWND consoleWindow;
void console() {
    FILE* conin = stdin;
    FILE* conout = stdout;
    FILE* conerr = stderr;
    AllocConsole();
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    freopen_s(&conin, "CONIN$", "r", stdin);
    freopen_s(&conout, "CONOUT$", "w", stdout);
    freopen_s(&conerr, "CONOUT$", "w", stderr);
    //SetConsoleTitleA("console ");
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR pCmdLine, int nCmdShow) {

    window_class.lpfnWndProc = wpm;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = "My Window Class";
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&window_class);

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);
    window_handle = CreateWindow("My Window Class", L"snry template", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        123, 123, 512, 256, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }

    font = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, L"Comic Sans MS");

    while (!s.quit) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))  { 
            DispatchMessage(&msg);

        }

        process(.01);

    InvalidateRect(window_handle, NULL, FALSE);
    UpdateWindow(window_handle);

    if (s.frames == 1) {
        console();
        consoleWindow = GetConsoleWindow();
        SetWindowPos(consoleWindow, 0, 33, 432, 512, 256, 0);
        SetForegroundWindow(window_handle);
        init();
    }

    }
    return 0;

}