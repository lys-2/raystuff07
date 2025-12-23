#include <stdio.h>
#include <stdbool.h>

struct node { char name[16], at, leads; bool has_name, is_root, is_leads; };
struct scene { 
	struct node scene[16]; char count;
};
struct scene sn;

void sp(struct node n) {
	sn.scene[sn.count] = n;
	sn.count++;
	if (n.is_root) {
		printf("spawned %s[%i] at ROOT\n", n.name, sn.count-1);
		return;
	}
	printf("spawned %s[%i] at %s\n", n.name, sn.count-1, sn.scene[n.at].name );
}

void init2() {
	sp((struct node) { "a:Road", .is_root = 1 });
	sp((struct node) { "a:Plains", .is_root=1 });
	sp((struct node) { "a:Guy", 0, });
	sp((struct node) { "A:Spawn", });
	sp((struct node) { "a:Player", });
}