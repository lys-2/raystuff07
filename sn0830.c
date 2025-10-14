#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define db double
#define ln long long

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand() 
#endif

static bool quit = false;
struct {
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
static unsigned int p = 0;
HANDLE hConsole;

void console() {
    FILE* conin = stdin;
    FILE* conout = stdout;
    FILE* conerr = stderr;
    AllocConsole();
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    freopen_s(&conin, "CONIN$", "r", stdin);
    freopen_s(&conout, "CONOUT$", "w", stdout);
    freopen_s(&conerr, "CONOUT$", "w", stderr);
    SetConsoleTitleA("asdf ");
}

enum type { character = 1, place, item, plant, creature, event, tag };
enum role { fighter = 1, mage, rogue };
enum gen { m = 1, f };
enum result { ok, bad };
enum mode { first, ui };
enum smode { drawing, ready, copied  };
enum emote { wave, happy, sad, touch };
enum anim { idle, sit, fight, lay, swim, jump };
enum mount { skate, bike, scooter };

struct v4 { double x, y, z, w; };
struct v3 { double x, y, z; };
struct v2 { double x, y; };
struct tf { struct v3 loc, rot, scale; };
struct tr { double x, y, z, p, r, yw, sx, sy, sz; };
struct v3 ro = { .0,.0,-1. };
struct v3 rt = { 11.,31.,-21. };
struct m3 { struct v3 a, b, c; };
struct m3 mx = { { 11., 31., -21. }, { 11., 31., -21. }, { 11., 31., -21. } };
struct v3 addv(struct v3 a, struct v3 b) { return (struct v3) { a.x + b.x, a.y + b.y, a.z + b.z }; };
struct v3 subv(struct v3 a, struct v3 b) { return (struct v3) { a.x - b.x, a.y - b.y, a.z - b.z }; };
struct v3 muls(struct v3 a, double b) { return (struct v3) { a.x* b, a.y* b, a.z* b }; };
struct v3 mulv(struct v3 a, struct v3 b) { return (struct v3) { a.x* b.x, a.y* b.y, a.z* b.z }; };
struct v3 divv(struct v3 a, struct v3 b) { return (struct v3) { a.x/ b.x, a.y/ b.y, a.z/ b.z }; };
struct v3 norm(struct v3 a) { return muls(a, (1 / sqrt(a.x * a.x + a.y * a.y + a.z * a.z))); };
double dot(struct v3 a, struct v3 b) { return  a.x * b.x + a.y * b.y + a.z * b.z; };
double dot2(struct v2 a, struct v2 b) { return  a.x * b.x + a.y * b.y; };
float fract(f) { f - (int)f; };
struct v3 cross(struct v3 a, struct v3 b) {
    return (struct v3)
    {
        a.y* b.z - a.z * b.y, a.z* b.x - a.x * b.z, a.x* b.y - a.y * b.x
    };
};
float rand2(double id) {
    return sin(dot2((struct v2) { id, 1.},
        (struct v2) {127.1, 311.7})) * 43758.5453;
}


struct v3 mmv(struct m3 m, struct v3 v) {
    return (struct v3) {
        v.x* m.a.x + v.y * m.a.y + v.z * m.a.z,
            v.x* m.b.x + v.y * m.b.y + v.z * m.b.z,
            v.x* m.c.x + v.y * m.c.y + v.z * m.c.z
    };
}
struct m3 mmm(struct m3 a, struct m3 b) {
    return (struct m3) {
        {
            a.a.x* b.a.x + a.a.y * b.b.x + a.a.z * b.c.x,
            a.a.x* b.a.y + a.a.y * b.b.y + a.a.z * b.c.y,
            a.a.x* b.a.z + a.a.y * b.b.z + a.a.z * b.c.z,
        },
        {
            a.b.x * b.a.x + a.b.y * b.b.x + a.b.z * b.c.x,
            a.b.x * b.a.y + a.b.y * b.b.y + a.b.z * b.c.y,
            a.b.x * b.a.z + a.b.y * b.b.z + a.b.z * b.c.z,
        },
        {
            a.c.x * b.a.x + a.c.y * b.b.x + a.c.z * b.c.x,
            a.c.x * b.a.y + a.c.y * b.b.y + a.c.z * b.c.y,
            a.c.x * b.a.z + a.c.y * b.b.z + a.c.z * b.c.z,
        }
    };
}

struct m3 yawm(double a) {
    return (struct m3) {
        { cos(a), -sin(a), 0. },
        { sin(a), cos(a), 0. },
        { .0, .0, 1. }
    };
};
struct m3 pitchm(double a) {
    return (struct m3) {
        { cos(a), 0., sin(a) },
        { 0., 1., 0. },
        { -sin(a), .0, cos(a) }
    };
};
struct m3 rollm(double a) {
    return (struct m3) {
        { 1., 0., 0. },
        { .0, cos(a), -sin(a) },
        { .0, sin(a), cos(a) }
    };
};

struct m3 rtm(struct v3 r) {
    return mmm(mmm(yawm(r.z), pitchm(r.y)), rollm(r.x)); 
};
struct v3 rot(struct v3 ro, struct v3 rt) { return mmv(rtm(rt), ro); }

struct v4 sphere(struct v3 ro, struct v3 rd, struct v3 r)
{
    float a, b, c, h, t; struct v3 r2, n;
     r2 = mulv(r, r);
     a = dot(rd, divv(rd, r2));
     b = dot(ro, divv(rd, r2));
     c = dot(ro, divv(ro, r2));
     h = b * b - a * (c - 1.0);
    if (h < 0.0) return (struct v4) { -1.0 };
     t = (-b - sqrt(h)) / a;
     n = norm( divv(addv(muls(rd, t), ro), r2));
    return (struct v4) { t, n.x, n.y, n.z };
}

struct actor {
    char name[16], type, age, class, gen, lvl, stack, taken, text[128],
        is_root, is_for_sale;
    float time; int at, channel, owner, hp, price; struct tf t;
};
struct user {
    char name[64], pw[16], mail[16], cell[16], taken, cc, cl;
    struct actor character[4];
    int friend[16], playing, is_playing, slot, x, y, dx, dy;
};

enum place { house, yard, street, town };
struct actor scene[16] = {

    {.name = "house", place, .at = yard},
    {.name = "yard", place, .at = street},
    {.name = "street", place, .at = town},
    {.name = "town", place, .is_root = 1 },
    {.name = "sewer", place, .at = town},
    {.name = "cam", item, .t = {0.,0.,0.,0.,0.,0.,1.,1.,1.}},
    {.name = "sphere", item, .t = {0.,0.,0.,0.,0.,0.,11111.,11111.,11111.}},
    {.name = "sphere", item, .t = {0.,0.,0.,0.,0.,0.,222222.,1.,222222.}},
    {.name = "sphere", item, .t = {0.,0.,0.,0.,0.,0.,111.,111.,111.}},

    //  {.name = "dungeon", place, .parent = town},
};
struct actor items_lib[16] = {
    {.name = "stick", item, .price = 200 },
    {.name = "base set", item, .price = 10000 },
};

struct user def_user = { "Admin", "qwe" };
struct actor def_char =
{ "Player", character, 18, fighter, m, 2, 0, 1, .hp = 20, .at = street,
.t.loc.x = .0, .0, -3., 111.
};
struct user u_lib[16] = {
    {.name = "adm", "qw",
    .character = {
        {"main character", character, 16, fighter, m},
        {"char2", character, 14, mage, m},
    }
    },
};
struct color { unsigned char r, g, b, a; };
#define us g.u[g.session]
#define ch g.s[us.playing]
struct state {
    bool is_mouse_back; enum smode sm; enum mode mode;
    int id, users, actors, requests, session, uc, ac, is_auth, frame;
    struct user u[8];
    struct actor s[16];
    char log[64][64], req[64], name[16], b[255];
    unsigned short sound[24000 * 4];
    struct color screen[1024 * 512];

};

struct state g, def, save, rep[16];

void clear() {
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32)
    system("cls");
    SetConsoleTitle(L"asdfg");
#endif
}

#if defined(_WIN32)
void win() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = { 0, 0, 64, 16 };
    // SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}
#endif

int get_actor() {
    for (int i = 0; i < 15; i++) {
        if (!g.s[i].taken) { return i; }

    }
    return -1;
}
void get() {
    g.requests++;
    printf("%i/4 users, %i/16 actors, F%i\n", g.uc, g.ac, g.frame);
    if (!g.is_auth) { printf("Hello!\n"); return; }
    printf("%i::%s ch:%d/4\n", g.session, g.u[g.session].name,
        g.u[g.session].cc);
    if (!g.u[g.session].character[0].taken) return;
    printf("ID::%i::%s[%i], %i/%iHP - %s %f,%f,%f,%f,%f,%f\n",
        us.playing,
        ch.name,
        ch.lvl,
        g.s[g.u[g.session].playing].hp,
        ch.lvl * 10,
        g.s[ch.at].name,
        ch.t.loc.x,
        ch.t.loc.y,
        ch.t.loc.z,
        ch.t.rot.x,
        ch.t.rot.y,
        ch.t.rot.z

    );
    for (int i = 0; i < 15; i++) {
        if (g.s[i].taken)
            printf("%i::%s  ", i, g.s[i].name);
    }
    //printf("\n\r");
    for (unsigned char i = 0; i < 255; i++) {
        g.b[i] = i + 13;
    };
    //  printf("\n\n\n%s\n", g.b);

};
enum result user_create(struct user u) {
    if (g.uc < 4) {
        g.u[g.uc] = u;
        g.uc++; g.users++; return ok;
    };
    return bad;
};
void user_delete(int id) {};
enum result log_in(int id, char pw[16]) {
    if (!strcmp(g.u[id].pw, pw)) {
        g.session = id; g.is_auth = 1;  return ok;
    }
    return bad;
};
void log_out() { g.is_auth = 0; };
void char_create(struct actor a) {
    if (!g.is_auth) return;
    if (g.u[g.session].cc < 4) {
        g.u[g.session].character
            [g.u[g.session].cc] = a;
        g.u[g.session].cc++; g.u[g.session].cl++;
    };
};
void char_delete(int id) { g.u[g.session].character[id].taken = 0; };
int spawn(struct actor a, int where) {
    if (g.ac != 16) {
        g.s[g.ac] = a;
        g.s[g.ac].at = where; g.s[g.ac].taken = 1; g.ac++; return g.ac - 1;
    }
};
void delete(int what) { g.s[what].taken = 0; };
void play(int id) {
    if (us.is_playing) delete(g.u[g.session].playing);
    g.u[g.session].playing = spawn(g.u[g.session].character[id], street);
    g.u[g.session].slot = id;
    g.u[g.session].is_playing = true;
};
void sel(int what) {};
void move(int what, int where, int channel) {};
void mount(int mount) {};
void emote(int emote) {};
void say(char m[64]) {};
void reset() {
    g = def;
    user_create(def_user);
    log_in(0, "qwe");
    char_create(def_char);
    for (int i = 0; i < 16; i++) {
        if (scene[i].name[0] != '\0') {
            g.s[i] = scene[i]; g.s[i].taken = 1; g.ac++;
        }
    }
    play(0);
    ch.t.rot = mmv(yawm(11.), ch.t.rot);
    ch.t.loc = mmm(mx, mx).a;
    g.sm = copied;
};
void parse(char req[64]) {
    if (req[0] == 'g') { get(); }
    if (req[0] == 'r') { reset(); }
    if (req[0] == 'l') { log_in(0, "qwe"); }
    if (req[0] == 'o') { log_out(); }
    if (req[0] == 'u') { user_create(def_user); }
    if (req[0] == 'c') { char_create(def_char); }
    if (req[0] == 'x') { char_delete(0); }
    if (req[0] == 'p') { play(0); }
    if (req[0] == 'w') { spawn((struct actor) { "box" }, street); }
    if (req[0] == 'm') { move(0, 0, street); }
    if (req[0] == 's') { say("qwertyuy"); }
    if (req[0] == 'a') { ch.t.rot = rot(ro, (struct v3) { rand(), rand(), rand() }); }
    if (req[0] == 'd') { ch.t.rot.x = sin(rand()); }
};

void fun() { while (1) {
    clear();
    get();
    //  printf("%s\n", g.req);
    scanf("%s", g.req);
    parse(g.req);

} };

struct rend { ln th, id; };
void ren(struct rend d) {

        double x = ch.t.rot.x;
        double y = ch.t.rot.y;

    int p;

    for (ln i = 0; i < frame.width * frame.height/ d.th; i++) {
        p = i * d.th;
      // frame.pixels[ 1 + i * 4] = 0.;
     //  frame.pixels[ i * 4] = 0.;
      //  frame.pixels[ 2 + i * 4] = 0.;


        for (ln i2 = 0; i2 < 16; i2++) {
            if (!strcmp(g.s[i2].name, "sphere")) {

                struct v4 r = sphere(
                    (struct v3) {
                    -211., -211., 441.
                },
                    rot((struct v3) { 0., -1., 0. },
                        (struct v3) {
                    y / 111. + p / frame.width / 2222. - .2,
                        x / 111. - p % frame.width / 2222.,
                        0.
                }),
                    g.s[i2].t.scale
                );
                if ((ln)r.x != -1) {

                    frame.pixels[1 + p * 4] = (ln)((r.y + 1.) * 111.);
                   frame.pixels[ p * 4] = (int)((r.z + 1.) * 111.);
                    frame.pixels[2 + p * 4] = (int)((r.w + 1.) * 111.);
                }
                
            }
            
        }

    }
    
    
    }

void init() {
    reset();
#if defined(_WIN32)
    win();
#endif
};

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
   
    if (message == WM_KEYDOWN) printf("%c", wParam);
    if (message == WM_KEYDOWN && wParam == 'R') reset();
    if (message == WM_KEYDOWN && wParam == 0x09) {
        g.mode = ui;
    }
     
    switch (message) {
    case WM_QUIT:
    case WM_DESTROY: {
        quit = true;
    } break;

    case WM_MOUSEMOVE: {

    int x = frame.width / 2;
    int y = frame.height / 2;
    POINT pt;
        pt.x = x; 
        pt.y = y;

    ClientToScreen(window_handle, &pt);

    if (LOWORD(lParam) == x && HIWORD(lParam) == y)
        { break; }

    ch.t.rot.y -= (HIWORD(lParam)- y)/12.;
    ch.t.rot.x -= (LOWORD(lParam)- x)/12.;

    SetCursorPos(pt.x, pt.y);


    } break;

    case WM_PAINT: {

        device_context = BeginPaint(window_handle, &paint);
       // if (g.frame%4 == 1) 
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {

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
        111, 333, 777, 333, NULL, NULL, hInstance, NULL);
    if (window_handle == NULL) { return -1; }
    // SetWindowPos(window_handle, 0, 111, 111, 512, 256, 0);
    // Set the console screen buffer size
    init();
    CreateThread(0, 0, fun, 0, 0, 0);
    struct rend rn[16]; rn[0] = (struct rend){ 12, 0 };

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    RECT r;  GetWindowRect(window_handle, &r);
    r.top += 32;
    r.bottom -= 16;
    r.left += 16;
    r.right -= 16;
    ClipCursor(&r);

    while (!quit) {

        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }
       // frame.pixels[(p++) % (frame.width * frame.height)] = Rand32();
       // frame.pixels[Rand32() % (frame.width * frame.height)] = 0;
        ren(rn[0]);

        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);
        ShowCursor(0);
        char buff[64];
        sprintf(buff, "screen F%i W%i:%i M%i:%i %iC-%i %zu\231 ",
            g.frame, frame.width, frame.height, us.x, us.y,
            si.dwNumberOfProcessors, 
            si.wProcessorArchitecture,
                sizeof(void*)*8
        );
        SetWindowTextA(window_handle, buff);
        g.frame++;
        // Sleep(1);

    }

    return 0;
}