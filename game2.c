#include "raylib.h"
#include <stdio.h>

struct Tag { char name[8]; char attr;  float timer; };
struct Line { char line[100]; };
struct Button { char name[8]; char attr; float x; float y; float w; float h; bool hi; bool free; };
struct Slider { char name[8]; float value; char attr; float x; float y; float w; float h; bool hi; };
struct Place { char name[8]; float x; float y; char route[5]; char rlen; };
struct Action_lib { char name[8]; char desc[22]; float cooldown; };
struct Event { char sender;  char str[64]; };

struct Character {
    char name[8]; char place;
    char routes[4]; char routes_len; char bar[16][16];
    char stack; char age; char gen; char lvl; char hp; char hp_base; char ad; char id; long balance;
};

struct Character_lib {
    struct Character ch; char desc[22];
    char talk[8][64]; char talk_len; struct Line lines[16];
};

struct Item {
    char name[10]; char place; char stack; char heal; struct Tag tags[3];
    char attack; char armor; char weight;
};

struct G2 {
    struct Mesh mesh_lib[8];
    struct Character_lib char_lib[8];
    struct Action_lib action_lib[16];
    struct Place place_lib[8];
    struct Item item_lib[8];
    struct Button buttons[333]; short button_cur;
    int button_hovered;
    int score; int frame; struct Character chars[64]; struct Event log[16]; short log_cur;
    char buffer[111]; char buff_cur;
    struct Item items[64]; struct Place map[16];
};

struct G2 game = {
    .action_lib = {
        {"look", "Look around"},
        {"move", ""},
        {"talk", ""},
        {"skip", ""},
        {"magic", "", 3000.0},
        {"magic2", "", 4000.0},
        {"exit", ""},
},

    .char_lib = {
        {{"Cat"}, "It's a cat", {"Meow", "Purr rr", "Maawe mw", "Umw . ", " . ."}, 5},
        {{"Vermin"}, "Does not like cats", {"Squeak", "sq", "shh shh"}, 3},
        {{"Player"}, "Is the main character"},
        {{"NPC"}, "Non playable character"},
        {{"Villager"}, "Some guy outside", .lines = {
        "Good day",
        "Rain or shine",
        "Bye"
}},
        {{"Dragon"}, "Huge and angry", {"Roar!!!"}, 1},

},
.chars = {
    { "Bob", .place = 1, .lvl = 1, .id = 2, .hp = 10, .ad = 1, .hp_base = 10},
    { "Cat", .place = 2, .id = 0},
    { "Cat", .place = 3, .id = 0},
    {"Vermin", 7, 3, .id = 1},
    {"Vermin", 5, 13, .id = 1},
    {"NPC", 8, 100, .id = 3},
    {"Dragon", 12, .lvl = 12, .id = 5}},
.items = {{"Bottle", 9}, {"Stick", 3}, {"Base set", 10}, {"Wood set", 5}, { "Beach set", 5 } },

.map = {
{"Room", 44, 55, .route = {1}, 1},
{"House", 44, 111, {0, 2, 6, 10, 11}, 5},
{"Village", 123, 123, {1, 3, 4, 5}, 4},
{"Shop", 123, 55, {2}, 1},
{"Woods", 222, 77, {2, 12}, 2},
{"Town", 333, 177, {5}, 1},
{"Basement", 33, 144, {1}, 1},
{"City", 444, 222},
{"Backpack", 22, 200},
{"Equip", 22, 233},
{"Kitchen", 11, 77, {1}, 1},
{"Attic", 8, 33, {1}, 1},
{"Field", 300, 55, {4}, 1},
}
};


char screen[256 * 128 * 3];
Image img
= {
   .data = screen,
    .width = 256,
    .height = 128,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8,
    .mipmaps = 1
};

void say(char id, char msg[64]) {
    if (msg[0] != '\0') {
        strcpy(game.log[game.log_cur].str, msg); game.log[game.log_cur % 16].sender = id; game.log_cur += 1;
        game.log_cur %= 16;
        ;
    }
}

void move() {}

void drawpoint(Vector3 v) { screen[round(v.x)] = 255; }
void drawline() {}
void drawtri() {}

int map() {}

int main(void)
{

    InitWindow(555, 333, "sn08");
    SetTargetFPS(75);

    int k = 0;
    int c = 0;

    struct Character* mc = &game.chars[0];
    struct G2* g = &game;
    Texture2D tex = LoadTextureFromImage(img);

    struct Button* b;
    for (int button = 0; button < 114; button++) {
        b = &g->buttons[button];
        b->x = GetRandomValue(333, 444);
        b->y = GetRandomValue(222, 333);;
        b->h = 12;
        b->w = 12;
        b->attr = button;
        strcpy(b->name, "bt");
        g->button_cur += 1;
    }

    for (int character = 0; character < 64; character++) {

        b = &g->buttons[character + g->button_cur];
        b->x = 140 + ((character % 8) * 8);
        b->y = 222 + ((character / 8) * 8);
        b->h = 8;
        b->w = 8;
        b->attr = character;
        strcpy(b->name, "CH");
        g->button_cur += 1;

    }

    for (int item = 0; item < 64; item++) {

        b = &g->buttons[item + g->button_cur];
        b->x = 222 + ((item % 8) * 8);
        b->y = 222 + ((item / 8) * 8);
        b->h = 8;
        b->w = 8;
        b->attr = item;
        strcpy(b->name, "IT");
        g->button_cur += 1;

    }



    FILE* fptr;

    fptr = fopen("resources/data.txt", "w");

    fwrite(g, sizeof(struct G2), 1, fptr);

    // Close the file
    fclose(fptr);

    while (!WindowShouldClose())
    {

        if (g->frame % 24 == 2) {
            for (int pixel = 1; pixel < 256 * 128 * 3; pixel++) {
                screen[pixel] = g->frame * sin(pixel / 124.0);
            }
        }
        for (int pixel = 2; pixel < 22222; pixel++)
        {
            drawpoint((Vector3) { 2 + (pixel * 3 * 1.0), 2.0, 3.0 });

        }
        UpdateTexture(tex, screen);

        DrawTexture(tex, 333, 200, WHITE);
        g->frame += 1;

        DrawText(TextFormat("%s", LoadDirectoryFiles("")), 2, 2, 10, GRAY);

        struct Rectangle rec;
        g->button_hovered = -1;
        for (int button = 0; button < 333; button++) {
            b = &g->buttons[button];
            if (b->name[0] != '\0') {
                rec.x = b->x;
                rec.y = b->y;
                rec.width = b->w;
                rec.height = b->h;
                if (CheckCollisionPointRec(GetMousePosition(), rec))
                {
                    g->button_hovered = button;
                };
                DrawRectangle(b->x, b->y, b->w, b->h, (Color) {
                    0,
                        (g->button_hovered == button) * 222, 0, 255
                });
                DrawText(TextFormat("%c", g->buttons[button].name[0]), b->x + 2, b->y + 2, 10, GREEN);

            };
        };

        for (int bar = 0; bar < 24; bar++) {
            if (g->action_lib[bar].name[0]) {
                DrawRectangle(555 - 30, bar * 10, 32, 8, (Color) { 0, 22, 222, 255 });
                DrawText(TextFormat("%s", g->action_lib[bar]), 555 - 30, bar * 10, 10, WHITE);
            };
            if (IsKeyPressed(KEY_DOWN)) {}
            if (IsKeyPressed(KEY_UP)) {}
        }



        if (g->button_hovered > 0)
        {
            DrawText(TextFormat("%s%i",
                g->buttons[g->button_hovered].name,
                g->buttons[g->button_hovered].attr
            ), 33, 2, 10, GRAY);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (g->button_hovered > 0) { g->buttons[g->button_hovered].name[0] = '\0'; }
        };

        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawText(TextFormat("%i", g->frame), 2, 2, 10, GRAY);

        for (int place = 0; place < 16; place++) {
            if (g->map[place].x) {
                for (int route = 0; route < 4; route++) {
                    if (g->map[place].route[route]) {
                        DrawLine(
                            g->map[place].x,
                            g->map[place].y,
                            g->map[g->map[place].route[route]].x,
                            g->map[g->map[place].route[route]].y,
                            BLACK);
                    }
                    DrawText(TextFormat("%s, %i", g->map[place].name, place),
                        g->map[place].x, g->map[place].y, 15, BLACK);
                    DrawRectangle(500, 12 * place, 10, 10, BLACK);
                    DrawText(TextFormat("%c", g->map[place].name[0]),
                        500, 12 * place, 10, WHITE);
                }
            }
        }
        for (int character = 0; character < 64; character++) {
            if (g->chars[character].name[0]) {

                if (GetRandomValue(1, 1111) < 22) {
                    g->chars[character].place =
                        g->map[g->chars[character].place].route[
                            GetRandomValue(0, g->map[g->chars[character].place].rlen)];
                }


                if (GetRandomValue(1, 111) < 2) say(character,
                    g->char_lib[g->chars[character].id].talk[
                        GetRandomValue(0, g->char_lib[g->chars[character].id].talk_len)
                    ]);

                DrawText(TextFormat("%c", g->chars[character].name[0]),
                    g->map[g->chars[character].place].x - 10,
                    g->map[g->chars[character].place].y - 10,
                    20, BLACK);
                DrawRectangle(488, (12 * character) + 4, 10, 10, BLACK);
                DrawText(TextFormat("%c", g->chars[character].name[0]),
                    488, (12 * character) + 4, 10, WHITE);

            }
        }

        for (int item = 0; item < 64; item++) {
            if (g->items[item].name[0]) {

                DrawText(TextFormat("%c", g->items[item].name[0], item),
                    g->map[g->items[item].place - 1].x + 10,
                    g->map[g->items[item].place - 1].y + 10,
                    20, BLACK);
                DrawRectangle(476, (12 * item) + 6, 10, 10, BLACK);
                DrawText(TextFormat("%c", g->items[item].name[0]),
                    476, (12 * item) + 6, 10, WHITE);
            }
        }



        int ch = GetCharPressed(); if (ch) {
            k = ch; if (g->buff_cur < 63)
            {
                g->buffer[g->buff_cur] = ch; g->buffer[g->buff_cur + 1] = '\0'; g->buff_cur += 1;
            }
        };
        int key = GetKeyPressed(); if (key) { c = key; };
        if (IsKeyDown(KEY_BACKSPACE))
        {
            if (g->buff_cur) { g->buffer[g->buff_cur - 1] = '\0'; g->buff_cur -= 1; }
        }
        if (IsKeyPressed(KEY_ENTER)) {
            say(0, g->buffer);
            g->buff_cur = 0; g->buffer[0] = '\0';
        }

        DrawText(TextFormat("l%i, h%i/%i, a%i, %s",
            mc->lvl, mc->hp, mc->hp_base, mc->ad, g->map[mc->place].name), 12, 12, 20, BLACK);
        DrawText(TextFormat("%s", g->buffer), 12, 300, 20, BLACK);

        for (int route = 0; route < g->map[mc->place].rlen; route++) {
            DrawText(TextFormat("%s", g->map[g->map[mc->place].route[route]].name),
                333, route * 10, 10, BLACK);
        }

        int line_map;
        for (int line = 1; line < 5; line++) {
            line_map = g->log_cur - line;
            if (line_map <= 0) { line_map = 16 - line; };
            if (g->log[line_map].str[0] != '\0') {
                DrawRectangle(50, 330 - (line * 16),
                    MeasureText(TextFormat("%s", g->log[line_map].str), 10), 16, BLACK);
                DrawText(TextFormat("%s", g->log[line_map].str), 50, 330 - (line * 16), 10, GRAY);
                DrawText(TextFormat("%s", g->chars[g->log[line_map].sender].name), 10, 330 - (line * 16), 10, GRAY);
            }
        }



        EndDrawing();

    }
    CloseWindow();
    return 0;
}
