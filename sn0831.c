#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ln long long
#define serv_port 23456
#define msg_len 1024

ln f;
enum platform { win, lin, and };

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
struct line { char name[64], line[512]; };
void sb(unsigned int* arr, int n) {
    return arr[n / 8] |= (1 << (n % 8));
}

int gb(const unsigned int* arr, int n) {
    return (arr[n / 8] >> (n % 8)) & 1;
}

struct line story[1024] = {
    // T A V E R N  -  Lines 1-250
    {"NARRATOR", "INT. THE RUSTY FLAGON TAVERN NIGHT"},
    {"NARRATOR", "Rain drums on the thatched roof. Smoke hangs thick over tankards and tired faces."},

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
    int lvl, stack, dur, price;
    bool is_taken;
};

struct actor {
    char name[32];
    int stack, lvl, x, y;
    enum class class;
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
struct item items[12] = {
    {"coin"},
    {"box"},
    {"stick"},
    {"potion"},
    {"elixir"},
    {"base set"},
    {"ruby"},
    {"plates", .is_taken = 1},
    {"emerald", .stack = 3, .price = 100, .is_taken = 1},
    {"card"},
    {"ring"}
};

struct item store[8] = {
    {"coin", .stack = 1234, .is_taken = 1},
    {"plates", .stack = 14, .price = 8, .is_taken = 1, .class = armor},
    {"emerald", .stack = 13, .price = 9, .is_taken = 1}
};

struct actor skills[8] = {
    {"vigor", fighter, 1},
    {"fire arrow", mage, 1},
    {"sneak", rogue, 1}
};
struct actor lib[8] = {
    {"ring"},
    {"card"},
};

struct player {
    bool taken;
    char addr[16];
    char name[16], pin[4], mail[64], phone[16], twitch[16];
    enum class class;
    enum location location;
    bool map[4], beasts[8];
    struct item inventory[8];
    int req, port, x, y, rot, lvl, exp, hp, resp, party[8],
        prog, hunt, select_a, select_b;
    ln rtime;
};

enum message { get, say, fight, item, level, place, new, gone };
struct player_resp {
    unsigned int x:11;
    unsigned int y:10;
    int on:1;
    int rot:12;
};
struct actor_resp {
    int x : 10;
    int y : 9;
    int on : 1;
};
struct resp {
    enum message t;
    char user;
    struct player_resp p[64];
    struct actor_resp s[64];
    char text[444];
};

struct base {
    struct color noise[255 * 255];
    struct color tex[255 * 255];
    bool is_logged;
    double time;
    int player, count, requests;
    char req[msg_len], resp[msg_len], input[msg_len];
    struct resp r;
    struct player players[64];
    struct actor scene[128];
    ln start;
};
struct base d;

struct color* screen;

struct point { float x, y; };
void point(struct color* tex, int w, int h, int x, int y) {
    if (x >= 0 && x < w && y >= 0 && y < h)
        tex[y * w + x].r = 255;
}

double lerp(double a, double b, double f) { return a * (1.0 - f) + (b * f); }
void line(struct color* tex, int w, int h, struct point a, struct point b) {
    int d = 123;
    for (ln i = 0; i <= d; i++) {
        point(tex, w, h, lerp(a.x, b.x, i / (double)d),
            lerp(a.y, b.y, i / (double)d));
    }
}
void card(struct color* tex, int w, int h, struct point o, int x, int y) {
    line(screen, w, h, o, (struct point) { o.x + x, o.y });
    line(screen, w, h, o, (struct point) { o.x, o.y + y });
    line(screen, w, h, (struct point) { o.x + x, o.y }, (struct point) { o.x + x, o.y + y });
    line(screen, w, h, (struct point) { o.x, o.y + y }, (struct point) { o.x + x, o.y + y });
    line(screen, w, h, o, (struct point) { o.x + x, o.y + y });
}
void grid(struct color* tex, int w, int h, struct point o, int x, int y) {}

struct point rot(struct point p, struct point c, float a) {
    p.x -= c.x;
    p.y -= c.y;
    float x2 = p.x * cos(a) - p.y * sin(a);
    float y2 = p.x * sin(a) + p.y * cos(a);
    p.x = c.x + x2;
    p.y = c.y + y2;
    return p;
};

void ring(struct color* tex, int w, int h, struct point a, float r) {
    int d = 123;
    struct point o = a;
    a.y += r;
    for (int i = 0; i < d; i++) {
        a = rot(a, o, .1 * i);
        point(tex, w, h, a.x, a.y);
    }
}

int get_room(char a[16], int p) {
    for (ln i = 0; i < 64; i++) {
        if (
            d.players[i].port == p &&
            !strcmp(d.players[i].addr, a) &&
            d.players[i].taken
            )
        {
            return i;
        }
    }
    return -1;
}

int empty_room() {
    for (ln i = 0; i < 64; i++) {
        if (d.players[i].taken == false) { return i; };
    }
    return -1;
};

int get_max_hp(int id) { return 3 + get_lvl(id) * 4; };
int get_lvl(int id) { return 1 + d.players[id].exp / 1000; };

int join(char a[16], int p, char n[16]) {
    int room = empty_room();
    if (room != -1)
    {
        strcpy(d.players[room].addr, a);
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

char inv1[256], inv2[256];
char* inv(int id) {

    memset(&inv1, 0, strlen(inv1));
    for (ln i = 0; i < 8; i++) {
        if (d.players[id].select_a == i) strcat(&inv1, "|");
        if (d.players[id].select_b == i) strcat(&inv1, "~");
        if (d.players[id].inventory[i].is_taken) {
            strcat(&inv1, d.players[id].inventory[i].name);
            if (d.players[id].inventory[i].stack > 0) {
                char s[16];
                sprintf(&s, "[%i]", d.players[id].inventory[i].stack);
                strcat(&inv1, s);
            }
            strcat(&inv1, ", ");
        }
        else strcat(&inv1, "_ ");
    }
    return inv1;
};
char* stores(int id) {

    memset(&inv2, 0, strlen(inv2));
    for (ln i = 0; i < 8; i++) {
        if (d.players[id].select_a == i + 8) strcat(&inv2, "|");
        if (d.players[id].select_b == i + 8) strcat(&inv2, "~");
        char s[16];
        if (store[i].is_taken) {
            strcat(&inv2, store[i].name);
            if (store[i].stack > 1) {
                sprintf(&s, "[%i]", store[i].stack);
                strcat(&inv2, s);
            }
            sprintf(&s, ":%i", store[i].price);
            strcat(&inv2, s);
            strcat(&inv2, ", ");
        }
        else strcat(&inv2, "_ ");
    }
    return inv2;
};

void req(char m[msg_len]) { strcpy(&d.req, m); };
struct resp resp(int id, char m[msg_len], enum message t) {
    sprintf(&d.resp, "%i|%i|%s", t, id, m);

    struct resp r = { 0 };
    strcpy(r.text, "qwertyuy");

    if (t == get) {

        unsigned char st[sizeof(r)];
        for (ln i = 0; i < 64; i++) {
            r.p[i].on = d.players[i].taken;
            r.p[i].x = d.players[i].x;
            r.p[i].y = d.players[i].y;
        }
        for (ln i = 0; i < 64; i++) {
            r.s[i].on = d.scene[i].is_taken;
            r.s[i].x = d.scene[i].x;
            r.s[i].y = d.scene[i].y;
        }

        memcpy(&st, &r, sizeof(r));

        inv(id);
        stores(id);
        sprintf(&r.text,
           "%s::%i/%i t%d reqs:%i p:%i/256,\n\
\
\!\n\
\
R%i L%i(%c)/ex%i HP%i/%i M%i|%i\n%s \n%s \n(%i)%s",
            d.players[id].name,
            id, d.count,
            d.start,
            d.requests,
            d.players[id].prog,

            d.players[id].resp,
            get_lvl(id),
            cnames[d.players[id].class][0],
            d.players[id].exp,
            d.players[id].hp,
            get_max_hp(id),
            d.players[id].x,
            d.players[id].y,
            inv1,
            inv2,
            d.players[id].location,
            places[d.players[id].location].name
        );
        strcat(&d.resp, "\n");
        strcat(&d.resp, m);
        return r;
    };
    return r;
};
int respawn(int id) {
    struct player a = d.players[id];

    int p = d.players[id].port;
    d.players[id] = (struct player){ 0 };
    d.players[id].hp = get_max_hp(id);
    d.players[id].class = rand() % 3;
    d.players[id].taken = true;
    strcpy(d.players[id].addr, a.addr);
    d.players[id].port = a.port;
    d.players[id].resp = a.resp + 1;
};

int name2i(char name[64]) {
    for (ln i = 0; i < 12; i++) {
        if (!strcmp(name, items[i].name)) return i;
    }
    return -1;
}

int has_item(int id, int i) {
    for (ln i2 = 0; i2 < 8; i2++) {
        if (d.players[id].inventory[i2].is_taken &&
            !strcmp(d.players[id].inventory[i2].name, items[i].name))
            return i2;
    }
    return -1;
}

int get_slot(int id) {
    for (ln i = 0; i < 8; i++) {
        if (!d.players[id].inventory[i].is_taken) return i;
    }
    return -1;
}

int add_item(int id, int i, int c) {
    int s = has_item(id, i);
    int s2 = get_slot(id);
    if (s > -1) d.players[id].inventory[s].stack += c;
    else if (s2 != -1) {
        d.players[id].inventory[s2] = items[i];
        d.players[id].inventory[s2].stack = c;
        d.players[id].inventory[s2].price = i;
        d.players[id].inventory[s2].is_taken = true;
    }
    return -1;
}
int rem_item(int id, int i, int c) {
    int s = has_item(id, i);
    if (s > -1) d.players[id].inventory[s].stack -= c;
    if (!d.players[id].inventory[s].stack) {
        d.players[id].inventory[s].is_taken = false;
    }
}
int fight_spawn(int id, int beast) {
    if (rand() % 11 == 0) d.players[id].hp -= (beast + (3 * beast * rand() % 20 == 0)) * 3;
    if (d.players[id].hp <= 0) { respawn(id); }
    d.players[id].hunt++;
    if (rand() % 20 == 0) add_item(id, rand() % 7, 1);

    int l1;
    l1 = get_lvl(id);
    if (d.players[id].exp < 4000)
        d.players[id].exp += 10 * beast;
    if (l1 != get_lvl(id)) d.players[id].hp = get_max_hp(id);
    return beast;
};

struct resp process(char m[msg_len], char ad[16], int p) {

    int room;
    room = get_room(ad, p);
    struct resp r;

    if (room == -1) {
        room = join(ad, p, "player");
        if (room == -1) { strcpy(r.text, "We are full.."); return r; }
        return resp(room, "Join!", new);
    }



    char st[msg_len], st2[msg_len];
    int a, b, res;
    d.requests++;
    //  memset(&d.resp, 0, strlen(d.resp));

    if (m[0] == 'p') {
        return  resp(room, "Pong!", get);
    }

    if (sscanf(m, "m|%i|%i", &a, &b) == 2) {
        d.players[room].x = a;
        d.players[room].y = b;
        d.players[room].location = (a / 78) % 4;
        if (b < 76) d.players[room].location = tavern;
        if (rand() % 8 == 0 && d.players[room].location != tavern) {
            char f[64];
            int beast = fight_spawn(room, rand() % 8);
            sprintf(f, "Fought `|%s(%i)!", beasts[beast].name, beast);
            return resp(room, f, get);
        }

        return resp(room, "moved..", get);
    }

    if (m[0] == 'a' && m[1] == 0) {
        struct item a = d.players[room].inventory[d.players[room].select_a];
        struct item b = d.players[room].inventory[d.players[room].select_b];
        d.players[room].inventory[d.players[room].select_b] = a;
        d.players[room].inventory[d.players[room].select_a] = b;

        return resp(room, "moved item ..", get);

    }

    if (sscanf(m, "a|%i|%i", &a, &b) == 2) {
        d.players[room].inventory[b] =
            d.players[room].inventory[a];
        d.players[room].inventory[a] = (struct item){ 0 };
        return resp(room, "moved item ..", get);
    }

    if (m[0] == 'g') {
        return resp(room, "", get);
    }

    if (!strcmp(m, "sell")) {
        if (d.players[room].select_a < 8 &&
            d.players[room].inventory[d.players[room].select_a].is_taken
            ) {
            add_item(room, 0,
                d.players[room].inventory[d.players[room].select_a].stack *
                d.players[room].inventory[d.players[room].select_a].price
            );
            d.players[room].inventory[d.players[room].select_a] =
                (struct item){ 0 };
        }
        return resp(room, "", get);
    }

    if (!strcmp(m, "buy")) {

        int c = has_item(room, 0);
        if (c > 0) c = d.players[room].inventory[c].stack;
        if (
            d.players[room].select_a >= 8 &&
            store[d.players[room].select_a - 8].is_taken &&
            c >= store[d.players[room].select_a - 8].price &&
            strcmp(store[d.players[room].select_a - 8].name, "coin")
            ) {

            add_item(room, name2i(store[d.players[room].select_a - 8].name), 1);

            store[d.players[room].select_a - 8].stack--;
            if (!store[d.players[room].select_a - 8].stack)
                store[d.players[room].select_a - 8] = (struct item){ 0 };
            rem_item(room, 0, store[d.players[room].select_a - 8].price);
        }
        return resp(room, "", get);
    }

    if (m[0] == 'c') {
        d.players[room].select_a++;
        d.players[room].select_a %= 16;
        return resp(room, "", get);
    }

    if (m[0] == 'v') {
        d.players[room].select_b++;
        d.players[room].select_b %= 16;
        return resp(room, "", get);
    }

    if (m[0] == 'l') {
        leave(room);
        strcpy(r.text, "Bye!"); return r;;
    }

    if (sscanf(m, "s|%[^\n]", &st) == 1) {
        return resp(room, st, say);
    }

    d.requests--;
    strcpy(r.text, "??;"); return r;

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
RECT rect = { 0,0,777,222 };
HFONT font, font2;

WSADATA wsaData;
SOCKET ListenSocket, cli;
struct sockaddr_in c, from;
int sz = sizeof(c);

void files(char* d) {
    char dir[128], dir2[128];
    //GetCurrentDirectoryA(128, dir);
    //printf("%s\n", dir);

    HANDLE hFile;
    //hFile = CreateFileA("theTest.txt",
    //    GENERIC_WRITE | GENERIC_READ, 0, NULL,
    //    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //hFile = CreateDirectoryA("theTest", NULL);

    DWORD a = GetFileAttributesA(d);
    //if (a & FILE_ATTRIBUTE_DIRECTORY && a != INVALID_FILE_ATTRIBUTES) {
    //    printf("Found a directory %s!\n", d);
    //}
    //else { printf("Dir not found( %s \n", d); return; }

    WIN32_FIND_DATA FindFileData;
    bool r = 0;
    int n = 0;
    strcpy(dir, d);
    strcat(dir, "/*");
    LARGE_INTEGER size;
    hFile = FindFirstFileA(dir, &FindFileData);
    size.HighPart = FindFileData.nFileSizeHigh;
    size.LowPart = FindFileData.nFileSizeLow;
    ln s = size.QuadPart;
    //if (
    //    !strcmp(FindFileData.cFileName, ".") &&
    //    !strcmp(FindFileData.cFileName, "..")
    //    ) 
    //    printf("The %i file found is %s %d\n", n, FindFileData.cFileName, s);
    r = 1;
    while (r) {
        r = FindNextFileA(hFile, &FindFileData);
        if (
            !strcmp(FindFileData.cFileName, ".") ||
            !strcmp(FindFileData.cFileName, "..")
            ) continue;
        n++;
        size.HighPart = FindFileData.nFileSizeHigh;
        size.LowPart = FindFileData.nFileSizeLow;
        ln s = size.QuadPart;
        if (
            FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && r) {
            //printf("DIR ");
            strcpy(dir2, d);
            strcat(dir2, "/");
            strcat(dir2, FindFileData.cFileName);

            files(dir2);
            continue;

        }
      //  if (r) printf("The %i file found is %s %d\n", n, FindFileData.cFileName, s);

    }
}

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
            int i, s, p; enum message t;
            char m[msg_len];
            char q[msg_len], w[msg_len], e[msg_len];
            s = sscanf(b, "%i|%i|%s", &t, &i, &m);

            memcpy(&d.r, &b, 1024);

            if (t == say && s == 3) {
                printf("%i`%s: %s\n", i, d.players[i].name, m);
            }
            if (t == new && s == 3) {
                printf("%i joined!\n", i, d.players[i].name, m);
            }
            else
                //printf("%s: %s\n", story[d.players[i].prog].name,
                //    story[d.players[i].prog].line);
                //printf("sock%d, M::%s from %s:%d, %d \n",
                //    cli, b,
                //    inet_ntoa(from.sin_addr),
                //    ntohs(from.sin_port), st);

                for (ln i = 0; i < 64; i++) {
                    d.players[i].taken = d.r.p[i].on;
                    d.players[i].x = d.r.p[i].x;
                    d.players[i].y = d.r.p[i].y;
                }


                strcpy(&d.resp, d.r.text);
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

STARTUPINFO si;
PROCESS_INFORMATION pi;
LRESULT CALLBACK WindowProcessMessage(HWND window_handle,
    UINT message, WPARAM wParam, LPARAM lParam) {

    // mode(first);

    if (message == WM_KEYDOWN && wParam == 'A') req("a");
    if (message == WM_KEYDOWN && wParam == 'Q')
    {
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        CreateProcessA(
            NULL,   // No module name (use command line)
            "sn_bar/sn25j.exe",  // Command line
            NULL,     // Process handle not inheritable
            NULL,     // Thread handle not inheritable
            FALSE,    // Set handle inheritance to FALSE
            CREATE_NEW_CONSOLE,     // No creation flags
            NULL,     // Use parent's environment block
            NULL,     // Use parent's starting directory
            &si,      // Pointer to STARTUPINFO structure
            &pi);    // Pointer to PROCESS_INFORMATION structure
    }

    if (message == WM_KEYDOWN && wParam == 'G') req("get!");
    if (message == WM_KEYDOWN && wParam == 'C') req("c");
    if (message == WM_KEYDOWN && wParam == 'V') req("v");
    if (message == WM_KEYDOWN && wParam == 'T') req("buy");
    if (message == WM_KEYDOWN && wParam == 'Y') req("sell");
    if (message == WM_KEYDOWN && wParam == VK_ESCAPE) quit = true;


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
            screen[i] = (struct color){ 0,0,0,0 };
        }
        for (ln i = 0; i < frame.width * frame.height; i++) {
            int x = i % frame.width;
            int y = i / frame.width;
            screen[i].b = d.noise[x % 128 * y % 128].b;
            screen[i].b /= x / 456.;
        }
        //for (ln i = 0; i < frame.width * frame.height; i++) {
        //    point(screen, frame.width, frame.height, i, i);
        //}

        for (ln i = 0; i < 16; i++) {
            if (!strcmp(d.scene[i].name, "card"))
                card(screen, frame.width, frame.height,
                    (struct point) {
                d.scene[i].x, d.scene[i].y
            }, 16, 16);
            if (!strcmp(d.scene[i].name, "ring"))
                ring(screen, frame.width, frame.height,
                    (struct point) {
                d.scene[i].x, d.scene[i].y
            }, 8);

        }

        for (ln i = 0; i < 64; i++) {
            if (d.players[i].taken)
            {     ring(screen, frame.width, frame.height,
                    (struct point) {
                d.players[i].x, d.players[i].y
            }, 4);

            }
        }

        for (ln i = 0; i < frame.width * frame.height; i++) {
            frame.pixels[i * 4] = screen[i].b;
            frame.pixels[1 + i * 4] = screen[i].g;
            frame.pixels[2 + i * 4] = screen[i].r;
            frame.pixels[3 + i * 4] = screen[i].a;
        }

        SelectObject(frame_device_context, font2);
        SetTextColor(frame_device_context, RGB(0, 255, 0));
        SetBkMode(frame_device_context, 2);
        SetBkColor(frame_device_context, RGB(0, 0, 0));
        for (ln i = 0; i < 64; i++) {
            if (!d.players[i].taken) continue;
            RECT r = {
             2 + d.players[i].x ,
            frame.height - d.players[i].y - 22,
             22 + d.players[i].x + 111,
              frame.height - d.players[i].y - 22 + 24
            };
            char name[64];
            sprintf(name, "Player%i", i);
            DrawTextA(frame_device_context, name, -1, &r, 0);

        }

        SelectObject(frame_device_context, font);
        SetTextColor(frame_device_context, RGB(0, 255, 0));
        SetBkMode(frame_device_context, TRANSPARENT);
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
        SetWindowTextA(window_handle, "snry rpg sn0831");
        InvalidateRect(window_handle, NULL, FALSE);
        // UpdateWindow(window_handle);

    } break;

    case WM_SIZE: {

        frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
        frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

        if (frame_bitmap) DeleteObject(frame_bitmap);
        frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
        SelectObject(frame_device_context, frame_bitmap);

        frame.width = LOWORD(lParam);
        frame.height = HIWORD(lParam);

        free(screen);
        screen = (struct color*)malloc(LOWORD(lParam) * HIWORD(lParam) *
            sizeof(struct color));

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
    //printf("%s message sent to %s:%d \n", msg,
    //    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

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

        //printf("sock%d Received from %s:%d: %s\n", serv, a, p, buffer);
       // memcpy(buffer, process(buffer, a, p), 1024);
        struct resp r = process(buffer, a, p);
        memcpy(&buffer, &r, 1024);

        if (
            buffer[0] != '1'
            ) {
            sendto(serv, buffer, msg_len, 0,
                (const struct sockaddr*)&client_addr, sizeof(client_addr));
            //printf("sock%d %s message sent to %s:%d \n", serv, buffer, a, p);
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

ln time_sec() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    long long milliseconds =
        (long long)ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;
    long long sc = (long long)ts.tv_sec;
    return sc;
}

void init() {
    //  printf("Hello, %s!\n", sys);
    printf("Hello!\n");
    strcpy(&d.resp, "Hello! \n Connecting..");
    cli_start();

    d.start = time_sec();
    printf("t %lld", d.start);
    printf("start");

    if (plat == lin) { serv_start(); }
    if (plat == win) { cli_start(); }

    for (ln i = 0; i < 255 * 255; i++) {
        d.noise[i] = (struct color){ rand(), rand(), rand() };
    }
    //for (ln i = 0; i < 16; i++) {
    //    d.scene[i] = lib[rand() % 2];
    //    d.scene[i].x = 456 + rand() % 111;
    //    d.scene[i].y = 16 + rand() % 111;
    //}

}

void cli_loop() {

    f++;
    if (plat == win) { cmsg("ping ", "93.95.97.124", serv_port); }

}

void rpc() {
    while (1) {

       // printf("t %lld", d.start);

        req("get!");
        clock_t c;
        c = clock;
        sl(40);
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
        222, 222, 789, 211, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }

    font = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, L"Century Gothic");
    font2 = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, L"Century Gothic");

    char buff[128];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    init();

    CreateThread(0, 0, cli_loop, 0, 0, 0);
    CreateThread(0, 0, input, 0, 0, 0);
    CreateThread(0, 0, rpc, 0, 0, 0);

    CreateThread(0, 0, files, "sn_bar", 0, 0);

    //STARTUPINFO si;
    //PROCESS_INFORMATION pi;
    //ZeroMemory(&si, sizeof(si));
    //si.cb = sizeof(si);
    //ZeroMemory(&pi, sizeof(pi));
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
        Sleep(0);
    }

    return 0;
}
#endif

#if defined(__linux__)
int main() {
    init();
    while (true) {};
}
#endif