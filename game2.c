#include "raylib.h"

struct Tag { char name[8]; char attr;  float timer; };
struct Place { char name[8]; float x; float y; char route[5]; };
struct Action_lib { char name[8]; char desc[22]; float cooldown; };
struct Event { char sender;  char str[64]; };
struct Character {
    char name[8]; char place;
    char routes[4]; char routes_len; char bar[16][16];
    char stack; char age; char gen; char lvl; char id;
};
struct Character_lib { struct Character c; char desc[22]; char talk[8][64]; char talk_len; };
struct Item {
    char name[8]; char place; char stack; char heal; struct Tag tags[3];
    char attack; char armor; char weight;
};
struct G2 {
    struct Character_lib char_lib[8];
    struct Action_lib action_lib[16];
    struct Place place_lib[8];
    struct Item item_lib[8];
    int score; int frame; struct Character chars[64]; struct Event log[1024]; char log_cur;
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
        {"exit", ""},
},

    .char_lib = {
        {{"Cat"}, "It's a cat", {"Meow", "Purr rr", "Maawe mw", "Umw . ", " . ."}, 5},
        {{"Vermin"}, "Does not like cats", {"Squeak", "sq", "shh shh"}, 3},
        {{"Player"}, "Is the main character"},
        {{"NPC"}, "Non playable character"},
        {{"Villager"}, "Some guy outside"},
        {{"Dragon"}, "Huge and angry", {"Roar!!!"}, 1},

},
.chars = {
    { "Bob", .place = 1, .lvl = 4, .id = 2},
    { "Cat", .place = 2, .id = 0},
    {"Vermin", 7, 3, .id = 1},
    {"NPC", 8, 100, .id = 3},
    {"Dragon", 12, .lvl = 12, .id = 5}},
.items = {{"Bottle", 9}, {"Stick", 3}, {"Base set", 10}, {"Wood set", 5}},
.map = {
{"Room", 44, 55, .route = {2}},
{"House", 44, 111, {3, 7, 11}},
{"Village", 123, 123, {4, 5, 6}},
{"Shop", 123, 55},
{"Woods", 222, 77, {12}},
{"Town", 333, 177},
{"Basement", 33, 144},
{"City", 444, 222},
{"Backpack", 22, 222},
{"Equip", 22, 255},
{"Kitchen", 11, 77},
{"Field", 300, 55},
}
};

void say(char id, char msg[64]) {
    if (msg[0] != '\0') {
        strcpy(game.log[game.log_cur].str, msg); game.log[game.log_cur].sender = id; game.log_cur += 1;
        ;
    }
}

int main(void)
{
    InitWindow(555, 333, "sn08");
    SetTargetFPS(123);


    int k = 0;
    int c = 0;

    struct Character* mc; mc = &game.chars[0];
    struct G2* g; g = &game;

    while (!WindowShouldClose())
    {
        g->frame += 1;

        mc->routes_len = 0;
        for (int route = 0; route < 4; route++) { mc->routes[route] = 0; }
        for (int route = 0; route < 4; route++) {
            g->chars[0].routes_len += 1;
        }

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
                            g->map[g->map[place].route[route] - 1].x,
                            g->map[g->map[place].route[route] - 1].y,
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

                if (GetRandomValue(1, 1111) < 2) {
                    g->chars[character].place =
                        GetRandomValue(1, 5);
                }


                if (GetRandomValue(1, 111) < 2) say(character,
                    g->char_lib[g->chars[character].id].talk[
                        GetRandomValue(0, g->char_lib[g->chars[character].id].talk_len)
                    ]);

                DrawText(TextFormat("%c, %i", g->chars[character].name[0], character),
                    g->map[g->chars[character].place - 1].x - 10,
                    g->map[g->chars[character].place - 1].y - 10,
                    20, BLACK);
                DrawRectangle(488, (12 * character) + 4, 10, 10, BLACK);
                DrawText(TextFormat("%c", g->chars[character].name[0]),
                    488, (12 * character) + 4, 10, WHITE);

            }
        }

        for (int item = 0; item < 64; item++) {
            if (g->items[item].name[0]) {

                DrawText(TextFormat("%c, %i", g->items[item].name[0], item),
                    g->map[g->items[item].place - 1].x + 10,
                    g->map[g->items[item].place - 1].y + 10,
                    20, BLACK);
                DrawRectangle(476, (12 * item) + 6, 10, 10, BLACK);
                DrawText(TextFormat("%c", g->items[item].name[0]),
                    476, (12 * item) + 6, 10, WHITE);
            }
        }

        for (int bar = 0; bar < 24; bar++) {
            if (g->action_lib[bar].name[0]) {
                DrawRectangle(555 - 30, bar * 10, 22, 8, (Color) { 0, 22, 222, 255 });
                DrawText(TextFormat("%s", g->action_lib[bar]), 555 - 30, bar * 10, 10, WHITE);
            };
            if (IsKeyPressed(KEY_DOWN)) {}
            if (IsKeyPressed(KEY_UP)) {}
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
            strcpy(g->log[g->log_cur].str, g->buffer); g->log_cur += 1;
            g->buff_cur = 0; g->buffer[0] = '\0';
        }

        DrawText(TextFormat("LV %i, %s", mc->lvl, g->map[mc->place - 1].name), 12, 12, 20, BLACK);
        DrawText(TextFormat("%s", g->buffer), 12, 300, 20, BLACK);

        for (int line = 1; line < 4; line++) {
            DrawRectangle(50, 330 - (line * 16),
                MeasureText(TextFormat("%s", g->log[g->log_cur - line].str), 10), 16, BLACK);
            DrawText(TextFormat("%s", g->log[g->log_cur - line].str), 50, 330 - (line * 16), 10, GRAY);
            DrawText(TextFormat("%s", g->chars[g->log[g->log_cur - line].sender].name), 10, 330 - (line * 16), 10, GRAY);

        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
