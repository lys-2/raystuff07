#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

enum TYPES { CHARACTER, ITEM, PLACE, ACTION, CHALLENGE };
struct Type { enum TYPES name; unsigned char id; bool free; };
struct Tag { char name[8]; char attr;  float timer; };
struct Line { char line[100]; };
struct Button {
    char name[10]; struct Type type; float x;
    float y; float w; float h; enum color color; bool hi; bool free;
};
struct Slider { char name[8]; float value; char attr; float x; float y; float w; float h; bool hi; };
struct Place { char name[12]; float x; float y; char route[6]; char rlen; };
struct Action_lib { char name[8]; char desc[22]; float cooldown_base; };
struct Challenge_lib { char name[12]; char desc[22]; };
struct Event { char sender;  char str[64]; };

struct Character {
    char name[8]; char place; char stack;
    char bar[16][16]; struct Type select;
    char age; char gen; char lvl; char hp; char hp_base; char ad; char id; long balance; bool ai;
    bool map_visit[64];
};

struct Character_lib {
    struct Character ch; char desc[22];
    char talk[8][64]; char talk_len; struct Line lines[16];
};

struct Item {
    char name[11]; char place; char stack; char heal; struct Tag tags[3];
    char attack; char armor; char weight;
};

struct G2 {
    struct Mesh mesh_lib[8];
    struct Character_lib char_lib[8];
    struct Action_lib action_lib[16];
    struct Challenge_lib challenge_lib[8];

    struct Place place_lib[8];
    struct Item item_lib[8];
    struct Button buttons[678]; short button_cur;
    int button_hovered;
    int score; int frame; struct Character chars[64]; struct Event log[16]; short log_cur;
    char buffer[111]; char buff_cur;
    struct Item items[64]; struct Place map[64];
};
struct Character* chars2[64];

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

    .challenge_lib = {
        "Move around",
        "Equipment",
        "Villager",
        "Dragon",
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
    { "Bob", .place = 0, .stack = 1, .lvl = 1, .id = 2, .hp = 10, .ad = 1, .hp_base = 10},
    { "Cat", 2, 1, .id = 0, .ai = true },
    { "Cat", 3, 1, .id = 0},
    {"Vermin", 6, 3, .id = 1},
    {"Vermin", 5, 13, .id = 1},
    {"NPC", 7, 100, .id = 3},
    {"Dragon", 12, 1, .lvl = 12, .id = 5}},
.items = {
    {"Bottle", 9}, {"Stick", 3}, {"Base set", 10}, {"Wood set", 5}, { "Beach set", 4 },
    { "Hockey set", 7 }, {"Box", 0}
},

.map = {
{"Room", .x = 44, .y = 55, .route = {1}, .rlen = 1},
{"House", 44, 111, {0, 2, 6, 10, 11}, 5},
{"Village", 123, 123, {1, 3, 4, 5}, 4},
{"Shop", 123, 55, {2}, 1},
{"Woods", 222, 77, {2, 12}, 2},
{"Town", 333, 177, {5}, 1},
{"Cellar", 33, 144, {1}, 1},
{"City", 444, 222},
{"Backpack", 22, 200},
{"Equip", 22, 233},

{"Kitchen", 11, 77, {1}, 1},
{"Attic", 8, 33, {1}, 1},
{"Field", 300, 55, {4}, 1},
{"Creek", 280, 35, {4}, 1},
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

void event() {}

void drawpoint(Vector3 v) { screen[(int)v.x] = 255; }
void drawline() {}
void drawtri() {}

int map() {}

int main(void)
{

    InitWindow(790, 333, "sn08");
    SetTargetFPS(75);

    Image im = LoadImage("resources/art.png");
    Texture2D art = LoadTextureFromImage(im);

    int k = 0;
    int c = 0;

    struct Character* mc = &game.chars[0];
    struct G2* g = &game;
    Texture2D tex = LoadTextureFromImage(img);

    struct Button* b;
    for (short button = 0; button < 88; button++) {
        b = &g->buttons[button];
        b->x = GetRandomValue(166, 266);
        b->y = GetRandomValue(222, 333);;
        b->h = 12;
        b->w = 12;
        b->type.id = button;
        b->type.name = 4;
        strcpy(b->name, "bt");
        g->button_cur += 1;
    }

    for (short character = 0; character < 64; character++) {

        b = &g->buttons[character + 111];
        b->x = 400 + ((character % 8) * 8);
        b->y = 10 + ((character / 8) * 8);
        b->h = 8;
        b->w = 8;
        b->type.id = character;
        b->type.name = CHARACTER;
        strcpy(b->name, g->chars[character].name);
        g->button_cur += 1;

    }

    for (short item = 0; item < 64; item++) {

        b = &g->buttons[item + 222];
        b->x = 400 + ((item % 8) * 8);
        b->y = 80 + ((item / 8) * 8);
        b->h = 8;
        b->w = 8;
        b->type.id = item;
        b->type.name = ITEM;
        strcpy(b->name, g->items[item].name);
        g->button_cur += 1;

    }

    for (short place = 0; place < 64; place++) {

        b = &g->buttons[place + 300];
        b->x = 464 + ((place % 8) * 8);
        b->y = 80 + ((place / 8) * 8);
        b->h = 8;
        b->w = 8;
        b->type.id = place;
        b->type.name = PLACE;
        strcpy(b->name, g->map[place].name);
        g->button_cur += 1;

    }


    FILE* fptr;

    fptr = fopen("resources/data.txt", "w");

    fwrite(g, sizeof(struct G2), 1, fptr);

    // Close the file
    fclose(fptr);

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        if (g->frame % 24 == 2) {
            for (int pixel = 1; pixel < 256 * 128 * 3; pixel++) {
                screen[pixel] = g->frame * sin(pixel / 124.0);
            }
        }
        for (int pixel = 2; pixel < 11; pixel++)
        {
            drawpoint((Vector3) { GetRandomValue(1, 78787), 2.0, 3.0 });

        }
        UpdateTexture(tex, screen);

        DrawTexture(tex, 300, 200, WHITE);
        DrawTextureEx(art, (Vector2) { 555, 0 }, 0, .87, WHITE);


        g->frame += 1;


        for (short c = 32; c < 128; c++) {

            b = &g->buttons[c + 380];
            b->x = 424 + ((c % 16) * 8);
            b->y = 133 + ((c / 16) * 8);
            b->h = 8;
            b->w = 8;
            b->type.id = c;
            b->type.name = CHALLENGE;
            b->name[0] = GetRandomValue(32, 128);
            g->button_cur += 1;

        }

        struct Rectangle rec;
        g->button_hovered = -1;
        mc->select.free = true;
        for (int button = 0; button < 678; button++) {
            b = &g->buttons[button];
            if (b->name[0] != '\0') {
                rec.x = b->x;
                rec.y = b->y;
                rec.width = b->w;
                rec.height = b->h;
                if (CheckCollisionPointRec(GetMousePosition(), rec))
                {
                    g->button_hovered = button;   mc->select = b->type; mc->select.free = false;
                };
                DrawRectangle(b->x, b->y, b->w, b->h, (Color) {
                    0,
                        (g->button_hovered == button) * 222, 0, 255
                });
                DrawText(TextFormat("%c", g->buttons[button].name[0]), b->x, b->y - 1, 10, GREEN);

            };
        };

        for (int bar = 0; bar < 7; bar++) {
            if (g->action_lib[bar].name[0]) {
                DrawRectangle(555 - 30, bar * 10, 32, 8, (Color) { 0, 22, 222, 255 });
                DrawText(TextFormat("%s", g->action_lib[bar]), 555 - 30, bar * 10, 10, WHITE);
            };
            if (IsKeyPressed(KEY_DOWN)) {}
            if (IsKeyPressed(KEY_UP)) {}
        }

        if (!mc->select.free && g->buttons[mc->select.id].name[0] != '\0')
        {
            if (mc->select.name == CHARACTER)
            {
                DrawText(TextFormat("%s [%i] l%i %s",
                    g->chars[mc->select.id].name,
                    g->chars[mc->select.id].stack,
                    g->chars[mc->select.id].lvl,
                    g->char_lib[g->chars[mc->select.id].id].desc
                ),
                    111, 2, 10, GRAY);
            }
            if (mc->select.name == PLACE)
            {
                DrawText(TextFormat("%s [%i]",
                    g->map[mc->select.id].name,
                    g->map[mc->select.id].rlen
                ),
                    111, 2, 10, GRAY);
            }
            if (mc->select.name == ITEM)
            {
                DrawText(TextFormat("%s [%i]",
                    g->items[mc->select.id].name,
                    g->items[mc->select.id].place
                ),
                    111, 2, 10, GRAY);
            }
        }

        DrawText(TextFormat("%i", mc->select.name), 88, 2, 10, GRAY);



        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (!mc->select.free) {
                say(0, TextFormat("%s %i %i", "INT", mc->select.name, mc->select.id));

            }
        };

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
                        g->map[place].x, g->map[place].y, 15 + ((mc->place == place) * 5),
                        (Color) {
                        0, (mc->select.name == PLACE && mc->select.id == place) * 255, 0, 255
                    });
                    //DrawRectangle(500, 12 * place, 10, 10, BLACK);
                    //DrawText(TextFormat("%c", g->map[place].name[0]),
                    //    500, 12 * place, 10, WHITE);
                }
            }
        }
        for (int character = 0; character < 64; character++) {
            if (g->chars[character].name[0]) {


                if (g->chars[character].ai) {
                    if (GetRandomValue(1, 1111) < 22) {
                        g->chars[character].place =
                            g->map[g->chars[character].place].route[
                                GetRandomValue(0, g->map[g->chars[character].place].rlen)];
                    }
                }
                if (GetRandomValue(1, 111) < 2) say(character,
                    g->char_lib[g->chars[character].id].talk[
                        GetRandomValue(0, g->char_lib[g->chars[character].id].talk_len)
                    ]);

                DrawText(TextFormat("%c", g->chars[character].name[0]),
                    g->map[g->chars[character].place].x - 10,
                    g->map[g->chars[character].place].y - 10, 20,
                    (Color) {
                    0, (mc->select.name == CHARACTER && mc->select.id == character) * 255
                        , 0, 255
                });
                //DrawRectangle(488, (12 * character)+4, 10, 10, BLACK);
                //DrawText(TextFormat("%c", g->chars[character].name[0]),
                //    488, (12 * character) + 4, 10, WHITE);

            }
        }

        for (int item = 0; item < 64; item++) {
            if (g->items[item].name[0]) {

                DrawText(TextFormat("%c", g->items[item].name[0], item),
                    g->map[g->items[item].place].x + 10,
                    g->map[g->items[item].place].y + 10,
                    20, (Color) {
                    0, (mc->select.name == ITEM && mc->select.id == item) * 255
                        , 0, 255
                });
                //        DrawRectangle(476, (12 * item) + 6, 10, 10, BLACK);
                //        DrawText(TextFormat("%c", g->items[item].name[0]),
                //            476, (12 * item) + 6, 10, WHITE);
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
