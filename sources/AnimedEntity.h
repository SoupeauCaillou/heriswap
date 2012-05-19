#pragma once

#include <vector>
#include <string>

#include <base/EntityManager.h>

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
void loadHerissonTexture(int type, AnimatedActor* c);
