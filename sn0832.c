#include <stdio.h>
#include <stdbool.h>

enum type { character, location, item, monster, root, quest, assign, tag };
enum weather { clear, cloud, rain, snow };
char weather_names[8][8] = {
	 "clear", "cloud", "rain", "snow"
};
enum season { summer, autumn, winter, spring };
char season_names[8][8] = {
	 "summer", "autumn", "winter", "spring"
};
enum time { day, dusk, night, dawn, morning };
char time_names[8][8] = {
	 "day", "dusk", "night", "dawn", "morning"
};
enum char_slot { 
	inv, mh, lower, feet
};

struct req { char name[8], count; };
struct actor { 
	char name[16]; enum type type; char lvl, slot;
	int score, attached, stack, road;
	bool is_on, is_for_sale, is_important, is_gone, is_spawned,
		quest[16];
	char beast[16], explore[16];
	float weight;
	struct req req;
};
struct state { 
	struct actor scene[123];
	enum weather weather;
	enum time time;
	enum season season;
	char wind, temp;
	int actors, quest, quests[16], location, locations[16];
};
struct state s;

void time(enum time t) {
	s.time = t;
	printf("Time is %s ~~\n", time_names[t]);
}
void temp(char t) {
	s.temp = t;
	printf("Temperature is set to %i ~~\n", t);
}
void weather(enum time t) {
	s.weather = t;
	printf("Weather is %s ~~\n", weather_names[t]);
}
void season(enum time t) {
	s.season = t;
	printf("Season is %s ~~\n", season_names[t]);
}
void wait() {
	time((s.time + 1)%4);
}
void say(int id, char* m) {
	printf("%s: %s\n", s.scene[id].name, m);
}
void talk(int id) {}
void move(int a, int b, int slot) {
	s.scene[a].attached = b;
	printf("%s moved to %s\n", s.scene[a].name, s.scene[b].name);
}
void spawn(struct actor a) { 
	s.scene[s.actors] = a;
	if (a.stack == 0) s.scene[s.actors].stack = 1;
	s.scene[s.actors].lvl = 1;
	s.actors++;
	if (a.type!=root)
	printf("%s [t%i] has spawned at %s\n", a.name, a.type, s.scene[a.attached].name);
}
void take_quest(int a, int b) {
	spawn((struct actor) { "quest", assign, .attached = a, .road = b });
	printf("%s took quest %s\n", s.scene[a].name, s.scene[b].name);
}
void complete_quest(int a) {
	printf("%s completed quest %s\n",
		s.scene[s.scene[a].attached].name,
		s.scene[s.scene[a].road].name);
}

void init() {

	////////////////////////////// 0
	printf("Hello!\n");
	spawn((struct actor) { "Settlement", root });
	season(summer);
	time(day);
	weather(clear);
	spawn((struct actor) { "Street", location }); // 1
	spawn((struct actor) { "Mill", location }); // 2
	spawn((struct actor) { "Player", character , .attached =1});
	spawn((struct actor) { "Townperson", character, .attached = 1 });
	spawn((struct actor) { "Pants", item, .attached = 3, .slot = lower });
	spawn((struct actor) { "Key ~", item, .attached = 5 });
	spawn((struct actor) { "Stick", item, .attached =2 });
	spawn((struct actor) { "Backpack", item, .attached =2 });
	spawn((struct actor) { "Cat", monster, .attached = 1 });
	spawn((struct actor) { "Collar", item, .attached = 9 });
	spawn((struct actor) { "Sapphire", item, .attached = 10 });
	spawn((struct actor) { "Vendor", character, .attached = 1 });
	spawn((struct actor) { "Apple", item,
		.attached =12, .stack=20, .is_for_sale = 1 });
	spawn((struct actor) { "Beads", item, .attached = 12, .is_for_sale = 1 });
	spawn((struct actor) { "Shirt", item, .attached = 12, .is_for_sale=1 });
	spawn((struct actor) { "Feather", item, .attached = 12 });
	spawn((struct actor) { "Hatchet", item, .attached =12 });
	spawn((struct actor) { "Turnip", item, .attached =12 });
	say(12, "Come and get some goods!");

	move(9, 2, 0);
	move(3, 2, 0);
	say(9, "Meow!");

	////////////////////////////// 1
	printf("\n 1 \n\n");
	spawn((struct actor) { "Rats", quest, .attached = 4,
		.req.count=3, .req.name="Tail" }); // 19
	spawn((struct actor) { "Apples", quest, .attached = 4,
		.req.count=3, .req.name="Apple" }); 
	spawn((struct actor) { "Gear up", quest, .attached = 4 }); 
	say(4, "Look we have quests ");
	take_quest(3, 19);
	take_quest(3, 20);
	take_quest(3, 21); // 24
	complete_quest(22);
	complete_quest(23);
	complete_quest(24);
}
void main() {
	init(); 
	printf("\n 1 \n\n");

	init2();  while (1) {}; }