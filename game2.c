#include "raylib.h"

struct Tag { char name[8]; char attr;  float timer; };
struct Place { char name[8]; float x; float y; char route[5]; };
struct Character {
    char name[8]; char place;
    char routes[4]; char routes_len; char bar[16][16]; char bar_cur;
    char stack; char age; char gen; char lvl;
};
struct Item {
    char name[8]; char place; char stack; char heal; struct Tag tags[3];
    char attack; char armor; char weight;
};
struct G2 {
    int score; int frame; struct Character chars[64]; char log[64][64]; char log_cur;
    char buffer[111]; char buff_cur;
    struct Item items[64]; struct Place map[16];
};
struct G2 game = {
.chars = {{"Bob", 1, .lvl = 4}, {"Cat", 2}, {"Vermin", 7, 3},
    {"NPC", 8, 100}, {"Dragon", 12, .lvl = 12}},
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

int main(void)
{
    InitWindow(555, 333, "sn08");
    SetTargetFPS(123);

    struct Character* mc; mc = &game.chars[0];
    struct G2* g; g = &game;

    int k = 0;
    int c = 0;

    while (!WindowShouldClose())
    {
        g->frame += 1;

        if (GetRandomValue(1, 111) < 2) mc->place =
            GetRandomValue(1, 5);

        mc->routes_len = 0;
        for (int route = 0; route < 4; route++) { mc->routes[route] = 0; }
        for (int route = 0; route < 4; route++) {
            game.chars[0].routes_len += 1;
        }

        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawText(TextFormat("%i", game.frame), 2, 2, 10, GRAY);

        for (int place = 0; place < 16; place++) {
            for (int route = 0; route < 4; route++) {
                if (game.map[place].route[route]) {
                    DrawLine(
                        game.map[place].x,
                        game.map[place].y,
                        game.map[game.map[place].route[route] - 1].x,
                        game.map[game.map[place].route[route] - 1].y,
                        BLACK);
                }
                DrawText(TextFormat("%s, %i", game.map[place].name, place),
                    game.map[place].x, game.map[place].y, 15, BLUE);
            }
        }

        for (int character = 0; character < 64; character++) {
            DrawText(TextFormat("%c, %i", game.chars[character].name[0], character),
                game.map[game.chars[character].place - 1].x - 10,
                game.map[game.chars[character].place - 1].y - 10,
                20, RED);
        }

        for (int item = 0; item < 64; item++) {
            DrawText(TextFormat("%c, %i", game.items[item].name[0], item),
                game.map[game.items[item].place - 1].x + 10,
                game.map[game.items[item].place - 1].y + 10,
                20, BLACK);
        }

        for (int bar = 0; bar < 24; bar++) {
            DrawRectangle(555 - 30, bar * 10, 22, 8, (Color) { 0, 22, (mc->bar_cur == bar) * 222, 255 });
        };
        if (IsKeyPressed(KEY_DOWN)) { mc->bar_cur += 1; }
        if (IsKeyPressed(KEY_UP)) { mc->bar_cur -= 1; }

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
            strcpy(g->log[g->log_cur], g->buffer); g->log_cur += 1;
            g->buff_cur = 0; g->buffer[0] = '\0';
        }

        DrawText(TextFormat("LV %i, %s", mc->lvl, g->map[mc->place - 1].name), 12, 12, 20, ORANGE);
        DrawText(TextFormat("%i", k), 26, 26, 20, ORANGE);
        DrawText(TextFormat("%i", c), 55, 26, 20, ORANGE);
        DrawText(TextFormat("%s", g->buffer), 12, 300, 20, ORANGE);

        for (int line = 1; line < 4; line++) {
            DrawRectangle(160, 280 - (line * 16),
                TextLength(TextFormat("%s", g->log[g->log_cur - line])) * 5, 16, BLACK);
            DrawText(TextFormat("%s", g->log[g->log_cur - line]), 160, 280 - (line * 16), 10, GRAY);

        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
