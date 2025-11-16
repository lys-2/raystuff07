#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ln long long
#define serv_port 23456

ln f;
enum platform {win, lin, and};

struct player {
    char addr[16], name[16], pin[4];
    int req, port, x, y, rot;
};

struct base { 
    char session;
    char message[64];
    struct player players[32];
};
struct base d;

void join() {};
void get() {};
void say() {};

#if defined(_WIN32) 
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

char sys[8] = "windows";
enum platform plat = win;
static bool quit = false;
char buff[8]; int st;
char msg[8];

struct frame {
    int width;
    int height;
    unsigned char* pixels;
} frame;

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap;
static PAINTSTRUCT paint;
static HDC device_context, frame_device_context;
const wchar_t window_class_name[] = L"My Window Class";
static WNDCLASS window_class;
static HWND window_handle;
static MSG message;
HANDLE hConsole;
HWND consoleWindow;

WSADATA wsaData;
SOCKET ListenSocket, cli;
struct sockaddr_in c, from;
int sz = sizeof(c);

void console() {
    FILE* conin = stdin;
    FILE* conout = stdout;
    FILE* conerr = stderr;
    AllocConsole();
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    freopen_s(&conin, "CONIN$", "r", stdin);
    freopen_s(&conout, "CONOUT$", "w", stdout);
    freopen_s(&conerr, "CONOUT$", "w", stderr);
    SetConsoleTitleA("console ");
}

void serv() {
    struct sockaddr_in a, b;
    a.sin_family = AF_INET;
    a.sin_port = htons(serv_port);
    a.sin_addr.s_addr = htonl(INADDR_ANY);


    ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ListenSocket >= 0)
        st = bind(ListenSocket, (SOCKADDR*)&a, sizeof(a));
    if (st >= 0)
        while (1) {
            st = recvfrom(ListenSocket, &msg, 8, 0, &b, &sz);
            printf("Windows recv %i %d %s from %s:%d\n",
                st, WSAGetLastError(), &msg,
                inet_ntoa(b.sin_addr),
                ntohs(b.sin_port));
        }

}
void serv_start() {
    CreateThread(0, 0, serv, 0, 0, 0);
};
cli_start() {
    cli = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long iMode = 1; // 1 for non-blocking, 0 for blocking
    ioctlsocket(cli, FIONBIO, &iMode);
}


void client(char msg[8], char* addr, int port) {
    c.sin_family = AF_INET;
    c.sin_port = htons(port);
    inet_pton(AF_INET, addr, &c.sin_addr.s_addr);

    strcpy(&buff, msg);
    st = sendto(cli, &buff, 8, 0, &c, sz);
    printf("sock%d  %s sent %i %d to %s %i\n", cli, 
        &buff, st, WSAGetLastError(), addr, port);

    char b[8];

    while (1) {
        
        st = recvfrom(cli, &b, 8, 0, &from, &sz);
        //printf("%s got %i %d from %s %i\n", &msg, st, WSAGetLastError());
        if (st > 0) {
            printf("sock%d, M::%s from %s:%d, %d \n",
                cli, b,
                inet_ntoa(from.sin_addr),
                ntohs(from.sin_port), st);
            st = sendto(cli, &buff, 8, 0, &c, sz);
            printf("sock%d  %s sent %i %d to %s:%i\n", cli,
                &buff, st, WSAGetLastError(), addr, port);
        }
        Sleep(1111);
    }

    //closesocket(cli);
    //closesocket(ListenSocket);
    //WSACleanup();
}

void clear() { system("cls"); }

void clears(int i) {
    frame.pixels[1 + i * 4] = 0.;
    frame.pixels[i * 4] = 0.;
    frame.pixels[2 + i * 4] = 0.;
}
void sl(int s) { Sleep(s); }

LRESULT CALLBACK WindowProcessMessage(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    // mode(first);

    if (message == WM_KEYDOWN && wParam == 'W') printf("button W");


    // if (message == WM_KEYDOWN) { clear(); get(); }

    switch (message) {
    case WM_QUIT:
    case WM_DESTROY: {
        quit = true;
    } break;

    case WM_MOUSEMOVE: {

        int x = frame.width / 2;
        int y = frame.height / 2;
        x = LOWORD(lParam);
        y = frame.height - HIWORD(lParam);
        printf("MX%i, MY%i \n", x, y);

    } break;

    case WM_PAINT: {

        RECT rect = { 0,2,999,111 };
        char buff[16];
        sprintf(buff, "H1");
        DrawTextA(frame_device_context, &buff, -1, &rect, 0);
        device_context = BeginPaint(window_handle, &paint);
        
        BitBlt(device_context,
            paint.rcPaint.left,
            paint.rcPaint.top,
            paint.rcPaint.right - paint.rcPaint.left,
            paint.rcPaint.bottom - paint.rcPaint.top,
            frame_device_context,
            paint.rcPaint.left, paint.rcPaint.top,
            SRCCOPY);

        EndPaint(window_handle, &paint);
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

#endif

#if defined(__linux__)

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int cli, serv, res;
struct sockaddr_in server_addr, client_addr;

fd_set read_fds;
struct timeval timeout;

void cli_start() { cli = socket(AF_INET, SOCK_DGRAM, 0); }

    void client(char msg[8], char addr[16], int port) {

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(port);
        client_addr.sin_addr.s_addr = inet_addr(addr);

        sendto(cli, msg, 8, 0,
            (const struct sockaddr*)&client_addr, sizeof(client_addr));
        printf("%s message sent to %s:%d \n", msg,
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    };

    void serv_start() {

        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[8];
        ssize_t bytes_received;

        // Create UDP socket
        serv = socket(AF_INET, SOCK_DGRAM, 0);
        setsockopt(serv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
        setsockopt(serv, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));

        if (serv < 0) {
            perror("Error opening socket");
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(serv_port);

        if (bind(serv,
            (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Error binding socket");
        }
        printf("UDP Server listening on port %d...\n", serv_port);

        while (1) {
            // Receive data from client

            // memset(&client_addr, 0, sizeof(client_addr));

            client_len = sizeof(client_addr);
            bytes_received = recvfrom(serv, buffer, 8, 0,
               &client_addr, &client_len);

            buffer[bytes_received] = '\0'; // Null-terminate the received data

            printf("sock%d Received from %s:%d: %s\n", serv,
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

            buffer[1] = 'o';
            sendto(serv, buffer, 8, 0,
                (const struct sockaddr*)&client_addr, sizeof(client_addr));
            printf("sock%d %s message sent to %s:%d \n", serv, buffer,
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    }

    };

    char sys[8] = "linux";
    enum platform plat = lin;
    void clear() { system("clear"); }
    
    void sl(int s) { sleep(s); }

#endif

void cmsg(char msg[8], char* addr, int port) {
        client(msg, addr, port);
 };

void init() { 
    printf("Hello, %s!\n", sys);
    cli_start();
    if (plat == lin) { serv_start(); }
    if (plat == win) { cli_start(); }
    
}


void cli_loop() { 

    f++; 
    if (plat == win) { cmsg("ping ", "93.95.97.124", serv_port); }

}
void loop() {}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR pCmdLine, int nCmdShow) {

    window_class.lpfnWndProc = WindowProcessMessage;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = window_class_name;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&window_class);
    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    console();
    consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, 33, 33, 777, 333, 0);
    window_handle = CreateWindow(window_class_name, L"screen", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        222, 222, 333, 211, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }

    HFONT font = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, L"Comic Sans MS");
    SelectObject(frame_device_context, font);
    SetTextColor(frame_device_context, RGB(0, 255, 0));
    SetBkMode(frame_device_context, TRANSPARENT);
    char buff[128];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    init();

    CreateThread(0, 0, cli_loop, 0, 0, 0);

    while (!quit) {

        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&message);
        }

    
        sprintf(buff, "title");
        SetWindowTextA(window_handle, "title");
        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);

    }

    return 0;
}
#endif

#if defined(__linux__)
int main() {
    init();
    while (true) { loop(); };
    }
#endif
