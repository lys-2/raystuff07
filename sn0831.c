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
#define msg_len 256

ln f;
enum platform {win, lin, and};

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

enum class { fighter, mage, rogue, weapon, armor };
char cnames[3][32] = { "fighter", "mage", "rogue" };

struct tag {
    enum class class;
    char lvl;
    bool is_taken;
};

struct item {
    char name[32];
    enum class class;
    int lvl, stack, dur;
    bool is_taken;
};

struct actor {
    char name[32];
    enum class class;
    int lvl, stack;
    struct item inventory[8];
    bool is_taken;
};

enum location { dungeon, woods, road, town, tavern };

struct actor places[6] = {
    {"dungeon"}, {"woods"}, {"road"}, {"town"}, {"tavern"}, {"tower"}
};
struct actor beasts[8] = {
    {"mold"},
    {"blob"},
    {"wasps"},
    {"rats"},
    {"elemental"},
    {"magic frost wolf"},
    {"mechabear"},
    {"ghost dragon"},
};
struct item items[8] = {
    {"box"},
    {"stick"},
    {"potion"},
    {"elixir"},
    {"coin"},
    {"base set"},
    {"ruby"}
};
struct actor skills[8] = {
    {"vigor", fighter, 1},
    {"fire arrow", mage, 1},
    {"sneak", rogue, 1}
};

struct player {
    bool taken;
    char addr[16];
    char name[16], pin[4], mail[64], phone[16], twitch[16];
    enum class class;
    enum location location;
    bool map[4], beasts[8];
    struct item inventory[8];
    int req, port, x, y, rot, lvl, exp, hp, resp, party[8];
};

struct base {
    bool is_logged;
    int player, count, requests;
    char req[msg_len], resp[msg_len], input[msg_len];
    struct player players[64];
};
struct base d;

int get_room(char a[16], int p) {
    for (ln i = 0; i < 64; i++) {
        if (d.players[i].port == p && d.players[i].taken)
            {
                return i;
            }
    }
    return -1;
}

int empty_room() { 
    for (ln i = 0; i < 64; i++) {
        if (d.players[i].taken==false) { return i; };
}
    return -1;
};

int get_max_hp(int id) { return 3+get_lvl(id)*4; };
int get_lvl(int id) { return 1 + d.players[id].exp / 1000; };

int join(char a[16], int p, char n[16]) {
    int room = empty_room();
    if (room != -1)
    {
        strcpy(a, d.players[room].addr);
        d.players[room].port = p;
        d.players[room].taken = true;
        d.players[room].lvl = 1;
        d.players[room].hp = get_max_hp(room);
        sprintf(d.players[room].name, "Player%i", room);
        d.count++;
        return room;
    }
    return -1;
};

void leave(int i) {
    d.players[i].taken == false;
};

int has_item(int id, int i) {
    for (ln i2 = 0; i2 < 8; i2++) {
        if (d.players[id].inventory[i2].is_taken &&
            !strcmp(d.players[id].inventory[i2].name, items[i].name))
            return i2;
    }
    return -1;
}
char invs[64];
void inv(int id) {
    memset(&invs, 0, strlen(invs));
    for (ln i = 0; i < 8; i++) {
        if (d.players[id].inventory[i].is_taken) {
            strcat(&invs, d.players[id].inventory[i].name);
            strcat(&invs, "|");
            strcat(&invs, " ");
        }
        else strcat(&invs, "_ ");
    }
    return;
};

enum message { get, say };
void req(char m[msg_len]) { strcpy(&d.req, m); };
char* resp(int id, char m[msg_len], enum message t) {

    if (t == say) { 
        sprintf(&d.resp, "s|%i|%s", id, m);
        return d.resp;
    };

    inv(id);
    strcat(&d.resp, "g|");
    sprintf(&d.resp,
        "ID::%i/%i reqs:%i, \nR%i L%i(%c)/ex%i HP%i/%i M%i|%i\n%s \n(%i)%s",
        id, d.count, d.requests,
        d.players[id].resp,
        get_lvl(id),
        cnames[d.players[id].class][0],
        d.players[id].exp,
        d.players[id].hp,
        get_max_hp(id),
        d.players[id].x,
        d.players[id].y,
        invs,
        d.players[id].location,
        places[d.players[id].location].name
    );
    strcat(&d.resp, "\n");
    strcat(&d.resp, m);
    return d.resp;
};
int respawn(int id) { 
    d.players[id].exp = 0;
    d.players[id].x = 0;
    d.players[id].y = 0;
    d.players[id].hp = get_max_hp(id);
    d.players[id].class = rand() % 3;
    d.players[id].resp++;

};

int fight_spawn(int id, int beast) {
    if (rand() % 11 == 0) d.players[id].hp-=beast+(3*beast*rand()%20==0);
    if (d.players[id].hp <= 0) { respawn(id); }
    d.players[id].inventory[0] = items[1];
    d.players[id].inventory[0].is_taken = true;

    int l1;
    l1 = get_lvl(id);
    if (d.players[id].exp < 4000)
    d.players[id].exp += 10 * beast;
    if (l1 != get_lvl(id)) d.players[id].hp = get_max_hp(id);
    return beast;
};

char* process(char m[msg_len], char a[16], int p) {

    int room;
    room = get_room(a, p);

    if (room == -1) { room = join(a, p, "player"); }
    if (room == -1) { return "We are full.."; }

    char st[msg_len], st2[msg_len];
    int i, i2, res;
    d.requests++;
  //  memset(&d.resp, 0, strlen(d.resp));

    if (m[0] == 'p') { 
        return  resp(room, "Pong!", get);;
    }

    if (sscanf(m, "m|%i|%i" , &i, &i2) == 2) { 
        d.players[room].x = i;
        d.players[room].y = i2;
        d.players[room].location = (i/78)%4;
        if (i2<76) d.players[room].location = tavern;
        if (rand()%8==0 && d.players[room].location != tavern) {
            char f[64];
            int beast = fight_spawn(room, rand() % 8);
            sprintf(f, "Fought `|%s(%i)!", beasts[beast].name, beast);
            return resp(room, f, get);
        }

        return resp(room, "moved..", get);
    }

    if (m[0] == 'g') { 
        return resp(room, "", get);
    }

    if (m[0] == 'l') {
        leave(room);
        return "Bye!";
    }

    if (sscanf(m, "s|%[^\n]", &st) == 1) {
        return resp(room, st, say);
    }
    if (m[0] == 'n') { return "Welcome! ID:%i "; }
    d.requests--;
    return ":?";

};


#if defined(_WIN32) 
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

char sys[8] = "windows";
enum platform plat = win;
static bool quit = false;
char buff[msg_len]; int st;
char msg[msg_len];

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
HANDLE hConsole, hConsole2;
HWND consoleWindow;
RECT rect = { 0,0,444,222 };

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
    consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, 33, 456, 777, 333, 0);
}

void clear() { system("cls"); }

void clears(int i) {
    frame.pixels[1 + i * 4] = 0.;
    frame.pixels[i * 4] = 0.;
    frame.pixels[2 + i * 4] = 0.;
}

void input() {

    char i[msg_len];

    while (1) {
        memset(&i, 0, strlen(i));
        strcat(&i, "s|");
        fgets(&d.input, sizeof(d.input), stdin);
        strcat(&i, &d.input);
        req(i);
        Sleep(11);
    }
};

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
            st = recvfrom(ListenSocket, &msg, msg_len, 0, &b, &sz);
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


void client(char msg[msg_len], char* addr, int port) {
    c.sin_family = AF_INET;
    c.sin_port = htons(port);
    inet_pton(AF_INET, addr, &c.sin_addr.s_addr);

    strcpy(&buff, msg);
    st = sendto(cli, &buff, msg_len, 0, &c, sz);
    char b[msg_len];

    while (1) {
        
        st = recvfrom(cli, &b, msg_len, 0, &from, &sz);
        //printf("%s got %i %d from %s %i\n", &msg, st, WSAGetLastError());
        if (st > 0) {
            if (b[0] == 's') {
                int i, s;
                char m[msg_len];
                s = sscanf(b, "s|%i|%s", &i, &m);
                printf("%i`%s: %s\n", i, d.players[i].name, m);
            }
            else
            //printf("sock%d, M::%s from %s:%d, %d \n",
            //    cli, b,
            //    inet_ntoa(from.sin_addr),
            //    ntohs(from.sin_port), st);
            strcpy(&d.resp, &b);
        }
        if (d.req[0] != 0) {
            st = sendto(cli, &d.req, msg_len, 0, &c, sz);
  //          printf("sock%d  %s sent %i %d to %s %i\n", cli,
  //&d.req, st, WSAGetLastError(), addr, port);
            d.req[0] = 0;
        }
        Sleep(0);
    }

    //closesocket(cli);
    //closesocket(ListenSocket);
    //WSACleanup();
}

void sl(int s) { Sleep(s); }

LRESULT CALLBACK WindowProcessMessage(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    // mode(first);

    if (message == WM_KEYDOWN && wParam == 'W') req("up!");
    if (message == WM_KEYDOWN && wParam == 'G') req("get!");

    // if (message == WM_KEYDOWN) { clear(); get(); }

    switch (message) {
    case WM_QUIT:
    case WM_DESTROY: {
        quit = true;
    } break;

    case WM_MOUSEMOVE: {

        int x = LOWORD(lParam);
        int y = frame.height - HIWORD(lParam);
        sprintf(d.req, "m|%i|%i", x, y); req(d.req);

    } break;

    case WM_PAINT: {

        device_context = BeginPaint(window_handle, &paint);
        for (ln i = 0; i < frame.width * frame.height; i++) {
            clears(i);
        }
        for (ln i = 0; i < frame.width * frame.height; i++) {
            frame.pixels[1 + i * 4] = 0;
            frame.pixels[i * 4] = i%222/3;
            frame.pixels[2 + i * 4] = 11+i%222/3;
        }
        DrawTextA(frame_device_context, &d.resp, -1, &rect, 0);

        BitBlt(device_context,
            paint.rcPaint.left,
            paint.rcPaint.top,
            paint.rcPaint.right - paint.rcPaint.left,
            paint.rcPaint.bottom - paint.rcPaint.top,
            frame_device_context,
            paint.rcPaint.left, paint.rcPaint.top,
            SRCCOPY);

        EndPaint(window_handle, &paint);
        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);

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

    void client(char msg[msg_len], char addr[16], int port) {

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(port);
        client_addr.sin_addr.s_addr = inet_addr(addr);

        sendto(cli, msg, msg_len, 0,
            (const struct sockaddr*)&client_addr, sizeof(client_addr));
        printf("%s message sent to %s:%d \n", msg,
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    };

void serv_start() {

        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[msg_len];
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

        char a[16];
        int p;

        while (1) {
            // Receive data from client

            // memset(&client_addr, 0, sizeof(client_addr));

            client_len = sizeof(client_addr);
            bytes_received = recvfrom(serv, buffer, msg_len, 0,
               &client_addr, &client_len);

            buffer[bytes_received] = '\0'; // Null-terminate the received data
            strcpy(a, inet_ntoa(client_addr.sin_addr));
            p = ntohs(client_addr.sin_port);

            printf("sock%d Received from %s:%d: %s\n", serv, a, p, buffer);

            strcpy(buffer, process(buffer, a, p));

            if (buffer[0] != 's') {
            sendto(serv, buffer, msg_len, 0,
                (const struct sockaddr*)&client_addr, sizeof(client_addr));
            printf("sock%d %s message sent to %s:%d \n", serv, buffer, a, p);
            }
            else
            for (ln i = 0; i < 64; i++) {
                if (d.players[i].taken == true) {

                    inet_aton(d.players[i].addr, &client_addr.sin_addr);
                    client_addr.sin_port = htons(d.players[i].port);
                    sendto(serv, buffer, msg_len, 0,
                        (const struct sockaddr*)&client_addr, sizeof(client_addr));
                }
            };
            
    }

    };

    char sys[8] = "linux";
    enum platform plat = lin;
    void clear() { system("clear"); }
    
    void sl(int s) { sleep(s); }

#endif

void cmsg(char msg[msg_len], char* addr, int port) {
        client(msg, addr, port);
 };

void init() { 
  //  printf("Hello, %s!\n", sys);
    printf("Hello!\n");
    strcpy(&d.resp, "Hello! \n Connecting..");
    cli_start();
    if (plat == lin) { serv_start(); }
    if (plat == win) { cli_start(); }
    
}


void cli_loop() { 

    f++; 
    if (plat == win) { cmsg("ping ", "93.95.97.124", serv_port); }

}
void rpc() { 
    while (1) {
        req("get!");
        sl(1000);
    }
}

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
    CreateThread(0, 0, input, 0, 0, 0);
    CreateThread(0, 0, rpc, 0, 0, 0);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //CreateProcessA(
    //    NULL,   // No module name (use command line)
    //    "cmd.exe",  // Command line
    //    NULL,     // Process handle not inheritable
    //    NULL,     // Thread handle not inheritable
    //    FALSE,    // Set handle inheritance to FALSE
    //    CREATE_NEW_CONSOLE,     // No creation flags
    //    NULL,     // Use parent's environment block
    //    NULL,     // Use parent's starting directory
    //    &si,      // Pointer to STARTUPINFO structure
    //    &pi);    // Pointer to PROCESS_INFORMATION structure


   // SetForegroundWindow(consoleWindow);
   // SetFocus(consoleWindow);


    //HDC screen = GetDC(NULL);
    //HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

    //HDC hdcMem = CreateCompatibleDC(screen); // hdc is the window's DC
    //HBITMAP hbmMem = CreateCompatibleBitmap(screen, 500, 500); // width, height of client area
    //SelectObject(hdcMem, hbmMem);

    //Rectangle(hdcMem, 0, 0, 500, 500);
    //BitBlt(screen, 0, 0, 500, 500, hdcMem, 0, 0, SRCCOPY);
    //DeleteObject(hbmMem);
    //DeleteDC(hdcMem);

    while (!quit) {

        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&message);
        }

        sprintf(buff, "title");
        SetWindowTextA(window_handle, "snry rpg sn0831");

        Sleep(10);
    }

    return 0;
}
#endif

#if defined(__linux__)
int main() {
    init();
    while (true) { };
    }
#endif
