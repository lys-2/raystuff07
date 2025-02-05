#include "raylib.h"

struct Place { char name[8]; float x; float y; char route[5]; };
struct Character {
    char name[8]; char place;
    char routes[4]; char routes_len;
    char stack; char age; char gen; float lvl;
};
struct Item { char name[8]; char place; char stack; };
struct G2 {
    int score; int frame; struct Character chars[64]; char log[111][111];
    struct Item items[64]; struct Place map[16];
};
struct G2 game = {
.chars = {{"Bob", 1, .lvl = 1}, {"Cat", 2}, {"Vermin", 7, 3}, {"NPC", 8, 100}},
.items = {{"Bottle", 9}, {"Stick", 3}, {"Clothes", 10}},
.map = {
{"Room", 44, 55, .route = {2}},
{"House", 44, 111, {3, 7, 11}},
{"Village", 123, 123, {4, 5, 6}},
{"Shop", 123, 55},
{"Woods", 222, 77 },
{"Town", 333, 177},
{"Basement", 33, 144},
{"City", 444, 222},
{"Backpack", 22, 222},
{"Equip", 22, 255},
{"Kitchen", 11, 77},
}
};

int main(void)
{
    InitWindow(555, 333, "sn08");
    SetTargetFPS(123);

    struct Character* mc; mc = &game.chars[0];
    struct G2* g; g = &game;

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
                DrawText(TextFormat("%s", game.map[place].name),
                    game.map[place].x, game.map[place].y, 15, BLUE);
            }
        }

        for (int character = 0; character < 64; character++) {
            DrawText(TextFormat("%c", game.chars[character].name[0]),
                game.map[game.chars[character].place - 1].x - 10,
                game.map[game.chars[character].place - 1].y - 10,
                20, RED);
        }

        for (int item = 0; item < 64; item++) {
            DrawText(TextFormat("%c", game.items[item].name[0]),
                game.map[game.items[item].place - 1].x + 10,
                game.map[game.items[item].place - 1].y + 10,
                20, BLACK);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
