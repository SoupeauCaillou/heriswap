#pragma once
#include <vector>
struct Actor {
	Entity e;
	float speed;
	bool visible;
};
struct AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct Actor actor;
};
void switchAnim(AnimatedActor* a);
void LoadHerissonTexture(int type, AnimatedActor* c);
