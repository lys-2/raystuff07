#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define wh 256;
#define ww 512;

struct color { char r, g, b, a; };
struct pixel { int x, y; struct color c; };
struct point { float x, y; struct color c; };
struct image { char name[8]; int w, h; struct color grid[512*256]; };
struct action { 
	char name[8];
	struct pixel p;
	int attr;
	long long time;
	char* text;
	char lib;
};
struct state { 
	struct image tex;
	struct action lib[8];
	struct action a[234567];
	int actions, libs, texts;
};
struct state s, s2;

void action_lib(char* name) {
	printf("Action ~%s is available!\n", name);
	s.lib[s.libs] = (struct action){ 0 };
	strcpy(s.lib[s.libs].name, name);
	s.libs++;
}

void pixel(struct pixel p) { s.tex.grid[p.x + p.y * s.tex.w] = p.c; }
void render(int cursor) {
	for (int i = 0; i < cursor; i++) {
		if (!strcmp("pixel", s.a[i].name)) pixel(s.a[i].p);
	}
}

void action(struct action a) {

	for (int i = 0; i < 8; i++) {
		if (!strcmp(s.lib[i].name, a.name)) a.lib = i;  
	}


	if (!strcmp("pixel", a.name)) {
		pixel(a.p);
		s.a[s.actions] = (struct action){ "pixel", a.p };
		printf("[%i] pixel r%hhu g%hhu b%hhu a%hhu at %s[%i,%i] %i %i\n",
			s.actions, a.p.c.r, a.p.c.g, a.p.c.b, a.p.c.a,
			s.tex.name, s.tex.w, s.tex.h, a.p.x, a.p.y);
		s.actions++;
	}
	if (!strcmp("render", a.name)) {
		render(a.attr);
		printf("Render %i/%i\n", a.attr, s.actions);
	}
}

void text(char* text, int id) {

	free((char*)s.lib[id].text);
	s.lib[id].text = (char*)malloc(strlen(text)+1);

	strcpy(s.lib[id].text, text);
	printf("~%s: %s\n", s.lib[id].name, s.lib[id].text);

}

void bulb() {}

void load() {}
void save() {}

void init2() {
	printf("Hi!\n");
	strcpy(&s.tex.name, "Screen");
	s.tex.w = ww;
	s.tex.h = wh;

	action_lib("pixel");
	text("Draw pixels on the screen", 0);

	action( (struct action) {"pixel",
		(struct pixel) { 11, 11, (struct color) {0,255,0,255}}
	});
	action( (struct action) {"pixel",
		(struct pixel) { 12, 12, (struct color) {0,255,222,255}}
	});

	action_lib("render");
	text("Render all the stuff", 1);
	
	action((struct action) { "render", .attr = 1 });

}

