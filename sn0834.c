#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#include "resource.h"

#include <mfapi.h>
#include <Mfidl.h>
#include <mfreadwrite.h>

#pragma comment (lib, "mfuuid")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mf")
#pragma comment(lib, "mfreadwrite")
#include <shellapi.h> 
#pragma comment(lib, "shell32.lib")
#define MY_WM_NOTIFYICON (WM_USER + 1)
#define ICON_UID 1
NOTIFYICONDATA nid = { 0 };
HICON icon;

#define count 4567
#define ver "sn0834a"
#define $p s.scene[i].is_spawned
#define $t s.scene[i].type
#define $x s.scene[i].x
#define $y s.scene[i].y
#define $r s.scene[i].r
#define $n s.scene[i].name
#define $c s.controlled
#define $lp for (int i = 0; i < count; i++)
#define $v2 (struct v2){

struct v2 { float x, y; };
struct line { float x, y; };
struct frame { int width; int height; unsigned char* pixels; } frame;

unsigned char* data;
UINT32 width;
UINT32 height;

enum type { leaf, env, unit, banana, cam, cur };
enum mode { mDraw, mUnit };
struct action { char a:1, t:1; };
struct node {
    char name[8], is_spawned,
     is_attached, is_controlled; enum type type;
    float x, y, sx, sy, r; int at; };
struct state {
    struct node scene[count];
    int nodes, frames, spawned, actions, controlled;
    char quit, is_log, is_spawn; enum type select_type;
    char version; int size;
    struct action log;
};
struct state s, def;

void sound() {
   PlaySound(&s, 0, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
}

void delete(int id) {
    s.scene[id].is_spawned = 0; s.spawned--;

    printf("- %i  ", id);
}
int slot() {
    if (!s.scene[s.nodes % count].is_spawned) return s.nodes % count;
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

    if (n.type == unit) {
        $lp { s.scene[i].is_controlled = 0; }
        n.is_controlled = 1;
        s.controlled = sl;
    }

    s.spawned++;
    s.nodes++;
    printf("++ %i  ", sl);
}
char p2p(struct v2 a, struct v2 b) {
    char r = 0;
    if (floor(a.x) == floor(b.x) && floor(a.y) == floor(b.y)) r = 1;

    return r;
}

void point(struct frame f, float x, float y) {
    if (x >= 0 && x < f.width && y >= 0 && y < f.height)
        f.pixels[1 + ((int)floor(x) + (int)floor(y) * f.width) * 4] = 255;
}
float lerp(float a, float b, float f) {
    float r = a * (1.0 - f) + (b * f);
   // printf("l %f, %f, %f  = %f\n", a, b,f, r );
    return r;
}
float len(struct v2 v) { return sqrt(v.x * v.x + v.y * v.y); };
void line(struct frame f, struct v2 a, struct v2 b) {
    float d = len((struct v2) { a.x-b.x, a.y-b.y });
    d = d/1.;
    for (float i = 0; i <= d; i++) {
        point(f,
            lerp(a.x, b.x, (i / d)),
            lerp(a.y, b.y, (i / d)));
    }
}

void load() {
    FILE* fptr = fopen("save_nr", "rb");
    if (fptr) {
        fread(&s, sizeof(s), 1, fptr);
        fclose(fptr);
    }
}


void init() {
    printf("Hi!\n");
};

void save() {
    FILE* fptr = fopen("save_nr", "wb");
    if (fptr) {
        fwrite(&s, sizeof(s), 1, fptr);
        fclose(fptr);
    }
}
void reset() { s = def; init(); s.frames = 3; }

void move(int id, float x, float y) {
    float x$ = s.scene[id].x + x;
    float y$ = s.scene[id].y + y;
    $lp {
        if ($p && $t == env) {
            char r = p2p($v2 $x, $y }, $v2 x$, y$ });
            if (r) { return; }
        }
    }
    s.scene[id].x = x$; s.scene[id].y = y$;
}

void tick(float dt, int i) {
    if (!$p) return;
    if ($t==unit) move(i,0,-.01);

}
void process(float dt) {

    if (s.frames < count) {
        spawn(
            (struct node) { .type=leaf,
            .x = rand() % frame.width, .y = rand() % frame.height
        }
        );
    }

    $lp {
        if (!$p) continue;
        if (rand() % 12 == 8 && s.scene[i].type == leaf) {
            s.scene[i].y -= rand() % 2;
            s.scene[i].x += 2 - rand() % 5;
            if (s.scene[i].y <= 0) delete(i);
        }
    }

    $lp{ tick(dt, i); }
    s.frames++;
}

void clear(struct frame f) {
    for (int i = 0; i < f.width * f.height; i++) {
        for (int j = 0; j < 4; j++) {
            f.pixels[j + (i * 4)] = 0;
        }
    }
}

static HDC device_context, frame_device_context;
HFONT font;
void text(char* t, float x, float y) {

    font = CreateFontA(0, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Comic Sans MS");

    SelectObject(frame_device_context, font);
    SetTextColor(frame_device_context, RGB(255, (s.frames / 22) % 255, 255));
    SetBkMode(frame_device_context, 2);
    SetBkColor(frame_device_context, RGB(0, 11, 11));
    RECT r = { x, y, 234, 64 };
    DrawTextA(frame_device_context, t, -1, &r, 0);
}
void paint(struct frame f) {
    for (int i = 0; i < f.width * f.height; i++) {
        int y = i / f.width;
        int x = i % f.width;
        f.pixels[1 + i * 4] = x % 33;
        f.pixels[i * 4] = (y / 5) % 77;
    }

    for (int i = 0; i < width * height; ++i) {
       if(data[i]>115) point(f, i % width, height-(i / width));
    }

    for (int i = 0; i < count; i++) {
        if (!$p)  continue; 
        point(f, s.scene[i].x, s.scene[i].y);
        if ($t==unit) {
            for (int i2 = 0; i2 < 55; i2++) {
                point(f, s.scene[i].x + 6-rand()%13, s.scene[i].y + 6-rand()%13);
            }
            if (i==$c)
                line(f,
                    (struct v2) {s.scene[i].x+13,s.scene[i].y},
                    (struct v2) {s.scene[i].x,s.scene[i].y+13}
                );
        }
     //   if ($t == marker) { text("~~1", $x, frame.height-$y); }

    }
    char str[64]; sprintf(str, "Hi! %i/:%i\n f:%ik", count, s.spawned, s.frames / 1000);
    text(str, 2, 2);
}

// Function to add the tray icon
void AddTrayIcon(HWND hWnd, HINSTANCE hInstance) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ICON_UID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = MY_WM_NOTIFYICON;
    nid.hIcon = icon; 
    lstrcpy(nid.szTip, TEXT("snry template")); 
    Shell_NotifyIcon(NIM_ADD, &nid); 

}

// Function to remove the tray icon
void RemoveTrayIcon(HWND hWnd) {
    nid.hWnd = hWnd;
    nid.uID = ICON_UID;
    Shell_NotifyIcon(NIM_DELETE, &nid); // Delete the icon
}

void cams() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    hr = MFStartup(MF_VERSION, 0);
    IMFAttributes* pAttr = NULL;
    IMFActivate** ppDevices = NULL;
    IMFMediaSource* ppSource = NULL;
    IMFSourceReader* pReader = NULL;
    int c = 0;

    hr = MFCreateAttributes(&pAttr, 0);
    hr = pAttr->lpVtbl->SetGUID(pAttr,
        &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
        &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

    hr = MFEnumDeviceSources(pAttr, &ppDevices, &c);
    printf("MF %i\n", c);
    hr = ppDevices[0]->lpVtbl->ActivateObject(ppDevices[0], 123, &ppSource[0]);

    LPWSTR szFriendlyName = NULL;
    LPWSTR link = NULL;
    int cchName;
    hr = ppDevices[0]->lpVtbl->GetAllocatedString(ppDevices[0],
        &MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
        &szFriendlyName, &cchName);
    hr = ppDevices[0]->lpVtbl->GetAllocatedString(ppDevices[0],
        &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
        &link, &cchName);

    printf("NM %i\n", SUCCEEDED(hr));
    printf("NM %ls\n", szFriendlyName);
    printf("LN %ls\n", link);

    IMFAttributes* attr2;
    hr = MFCreateAttributes(&attr2, 2);
    attr2->lpVtbl->SetGUID(attr2, &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
        &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    attr2->lpVtbl->SetString(attr2, &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, link);


    PCWSTR pszURL = 0;
    IMFSourceReaderCallback* pCallback = 0;
    IMFSourceReader** ppReader = 0;
    hr = attr2->lpVtbl->SetUnknown(attr2, &MF_SOURCE_READER_ASYNC_CALLBACK, pCallback);


    hr = MFCreateDeviceSource(attr2, &ppSource);
    printf("SOU %i\n", SUCCEEDED(hr));
    printf("SOU 0x%x\n", hr);

    hr = MFCreateSourceReaderFromMediaSource(ppSource, attr2, &pReader);
    printf("REA %i\n", SUCCEEDED(hr));
    printf("REA 0x%x\n", hr);

    DWORD dwStreamIndex = 0;
    DWORD dwMediaTypeIndex = 0;
    IMFMediaType* pType = NULL;
    hr = pReader->lpVtbl->GetNativeMediaType(pReader, 0, 17, &pType);
    printf("Type get %i 0x%x\n", SUCCEEDED(hr), hr);
    hr = pReader->lpVtbl->SetCurrentMediaType(pReader,
        MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
    printf("Type set %i 0x%x\n", SUCCEEDED(hr), hr);

    DWORD streamIndex, flags;
    LONGLONG llTimeStamp;
    IMFSample* pSample = NULL;
    hr = pReader->lpVtbl->GetCurrentMediaType(pReader, MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        &pType);
    printf("Type get2 %i 0x%x\n", SUCCEEDED(hr), hr);
    UINT64 tmp;
    hr = pType->lpVtbl->GetUINT64(pType, &MF_MT_FRAME_SIZE, &tmp);
    printf("Res %i 0x%x\n", SUCCEEDED(hr), hr);
    width = (UINT32)(tmp >> 32);
    height = (UINT32)(tmp);
    printf("Size = %ux%u\n", width, height);

    IMFSample* sample;

    DWORD stream1;
    DWORD flags1;
    LONGLONG timestamp1;

    while (1) {
        hr = pReader->lpVtbl->ReadSample(pReader,
            MF_SOURCE_READER_ANY_STREAM,    // Stream index.
            0,                              // Flags.
            &streamIndex,                   // Receives the actual stream index. 
            &flags,                         // Receives status flags.
            &llTimeStamp,                   // Receives the time stamp.
            &sample                        // Receives the sample or NULL.
        );
        printf("Sam %i 0x%x\n", SUCCEEDED(hr), hr);
        if (flags & MF_SOURCE_READERF_STREAMTICK)
        {
            continue;
        }
        break;
    }

    IMFMediaBuffer* buffer = 0;
    hr = sample->lpVtbl->ConvertToContiguousBuffer(sample, &buffer);
    printf("BUF %i 0x%x\n", SUCCEEDED(hr), hr);

    DWORD size = 0;
    hr = buffer->lpVtbl->Lock(buffer, &data, NULL, &size);
    printf("LOCK %i 0x%x\n", SUCCEEDED(hr), hr);
    printf("DT %s 0x%x\n", data);
    //hr = buffer->lpVtbl->Unlock(buffer);
}

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap;
static PAINTSTRUCT ps;
static WNDCLASS window_class;
static HWND window_handle;
MSG msg;

HWAVEOUT wave_out;
#define SAMPLING_RATE 44100
#define CHUNK_SIZE 2222
WAVEHDR header[2] = { 0 };
short chunks[2][CHUNK_SIZE];
char chunk_swap = 0;
float frequency = 659.25, vol = .0;
double wave_position = 0;
float wave_step;
WAVEFORMATEX format = {
            .wFormatTag = WAVE_FORMAT_PCM,
            .nChannels = 1,
            .nSamplesPerSec = SAMPLING_RATE,
            .wBitsPerSample = 16,
            .cbSize = 0,
};

LRESULT CALLBACK wpm(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    int x = LOWORD(lParam);
    int y = frame.height - HIWORD(lParam);

    if (message == WM_KEYDOWN && wParam == 'R') reset();
    if (message == WM_KEYDOWN && wParam == 'Q') $lp{$t = leaf; }
    if (message == WM_KEYDOWN && wParam == 'E') s.select_type = banana;
    if (message == WM_KEYUP && wParam == 'E') s.select_type = unit;
    if (message == WM_KEYDOWN && wParam == 'T') s.select_type = banana;
    if (message == WM_KEYUP && wParam == 'T') s.select_type = unit;
    if (message == WM_KEYDOWN && wParam == 'W') move($c, 0, 1);
    if (message == WM_KEYDOWN && wParam == 'S') move(s.controlled, 0, -1);
    if (message == WM_KEYDOWN && wParam == 'A') move(s.controlled, -1, 0);
    if (message == WM_KEYDOWN && wParam == 'D') move(s.controlled, 1, 0);
    if (message == WM_KEYDOWN && wParam == 'L') load();
    if (message == WM_KEYDOWN && wParam == 'J') save();
    if (message == WM_KEYDOWN && wParam == 'C') cams();
    if (message == WM_KEYDOWN && wParam == VK_ESCAPE) { s.quit = 1; }
    if (message == WM_RBUTTONDOWN) {
        spawn((struct node) { "~~~", .type = s.select_type, .x = x, .y = y });
    }
    if (message == WM_LBUTTONDOWN)    {
        spawn((struct node) { .type= env, .x = x, .y = y });
        s.is_spawn = 1;
        }
    if (message == WM_LBUTTONUP) { s.is_spawn = 0; }
    switch (message) {
    case WM_CREATE:
        AddTrayIcon(window_handle, (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE));
    case MY_WM_NOTIFYICON:
        if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {
            MessageBox(window_handle, TEXT("Hello!"), TEXT("~~~"), MB_OK);
        }
        if (LOWORD(lParam) == WM_RBUTTONDOWN) {
            MessageBox(window_handle, TEXT("Hello2!"), TEXT("~~~"), MB_OK);
        }
        break;
    case WM_QUIT: {} break;
    case WM_DESTROY: {
        s.quit = 1;
    } break;
    case (WM_MOUSEMOVE): {
        if (s.is_spawn) {
            spawn((struct node) { .type = leaf, .x = x, .y = y });
            spawn((struct node) { .type = env, .x = x, .y = y });
        };
        wave_step = 6.283 / (SAMPLING_RATE / (float)x/11.);
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

void CALLBACK WaveOutProc(HWAVEOUT wave_out_handle, UINT message,
    DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
    switch (message) {
    case WOM_CLOSE: printf("WOM_CLOSE\n"); break;
    case WOM_OPEN:  printf("WOM_OPEN\n");  break;
    case WOM_DONE: {
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            chunks[chunk_swap][i] = sin(wave_position) *
                32000*s.spawned/count* s.is_spawn;
            wave_position += wave_step;
        }
        waveOutWrite(wave_out, &header[chunk_swap],
            sizeof(header[chunk_swap]));
        chunk_swap = !chunk_swap;
    } break;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR pCmdLine, int nCmdShow) {

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    window_class.lpfnWndProc = wpm;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = "My Window Class";
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&window_class);
    icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);
    window_handle = CreateWindow("My Window Class", L"snry template",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        123, 123, 512, 256, NULL, NULL, hInstance, NULL);
    SendMessage(window_handle, WM_SETICON, ICON_BIG, (LPARAM)icon);


    //HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_MAP2), L"map");
    //HGLOBAL hResData = LoadResource(NULL, hResInfo);
    //char* d = LockResource(hResData);
    //memcpy(&s, d, sizeof(s));

    format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    waveOutOpen(&wave_out, WAVE_MAPPER, &format,
        (DWORD_PTR)WaveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION);

    waveOutSetVolume(wave_out, 0xFFFFFFFF);
    wave_step = 6.283 / ((float)SAMPLING_RATE / frequency);
    for (int i = 0; i < 2; ++i) {

        header[i].lpData = (CHAR*)chunks[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2;
        waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i]));
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            chunks[chunk_swap][i] = sin(wave_position) * i*vol;
            wave_position += wave_step;
        }
        waveOutWrite(wave_out, &header[i], sizeof(header[i]));

    }
 
    while (!s.quit) {
        process(.01);

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            DispatchMessage(&msg);
        }

        if (s.frames == 1) {
            console();
            consoleWindow = GetConsoleWindow();
            SetWindowPos(consoleWindow, 0, 33, 432, 512, 256, 0);
            SetForegroundWindow(window_handle);
            init();

   /*        hr = MFCreateSourceReaderFromMediaSource(ppSource, pAttr, &pReader);
            printf("R %i\n", (int)hr);

            DWORD dwStreamIndex = 0;
            DWORD dwMediaTypeIndex = 0;
            IMFMediaType* pType = NULL;
           hr = pReader->lpVtbl->GetNativeMediaType(pReader, dwStreamIndex, dwMediaTypeIndex, pType);*/

        }

       if (s.frames == 2) { load(); };

       if (s.frames == 1) {
       
       }

        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);
    }
    return 0;

}