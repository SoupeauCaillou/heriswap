#include "BackgroundManager.h"

#include <sstream>

#include <base/PlacementHelper.h>
#include <base/Interval.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "DepthLayer.h"

#define CAMERASPEED -1.f

struct BackgroundManager::Actor {
	Entity e;
	float speed;
	bool visible;
	int group;
};

struct BackgroundManager::AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct Actor actor;
};

BackgroundManager::BackgroundManager(float windowHeight) {
	cloudY[0] = Interval<float>(PlacementHelper::GimpYToScreen(70.0), PlacementHelper::GimpYToScreen(240));
	cloudY[1] = Interval<float>(PlacementHelper::GimpYToScreen(390), PlacementHelper::GimpYToScreen(490));
	cloudY[2] = Interval<float>(PlacementHelper::GimpYToScreen(560), PlacementHelper::GimpYToScreen(650));

	cloudSize[0] = Interval<float>(PlacementHelper::GimpWidthToScreen(230), PlacementHelper::GimpWidthToScreen(380));
	cloudSize[1] = Interval<float>(PlacementHelper::GimpWidthToScreen(190), PlacementHelper::GimpWidthToScreen(230));
	cloudSize[2] = Interval<float>(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpWidthToScreen(180));

	cloudSpeed[0] = Interval<float>(-0.4, -0.25);
	cloudSpeed[1] = Interval<float>(-0.25, -0.13);
	cloudSpeed[2] = Interval<float>(-0.1, -0.03);

	textures[0].push_back("haut_0.png");
	textures[0].push_back("haut_1.png");
	textures[0].push_back("haut_2.png");
	textures[1].push_back("moyen_0.png");
	textures[1].push_back("moyen_1.png");
	textures[2].push_back("bas_0.png");
	textures[2].push_back("bas_1.png");
	textures[2].push_back("bas_2.png");
	textures[2].push_back("bas_3.png");
	textures[2].push_back("bas_4.png");
}


BackgroundManager::~BackgroundManager() {
	clouds.clear();
}

void BackgroundManager::Setup() {
	for(int j=0; j<3; j++) {
		Actor* c = new Actor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->e = e;
		clouds.push_back(initCloud(c, j));
	}
}

BackgroundManager::Actor* BackgroundManager::initCloud(Actor* c, int group) {
	float ratio = 1.67;

	float width = cloudSize[group].random();
	TRANSFORM(c->e)->position.X = cloudStartX.random();
	TRANSFORM(c->e)->position.Y = cloudY[group].random();
	TRANSFORM(c->e)->z = DL_Cloud;

	int idx = MathUtil::RandomInt(textures[group].size());
	RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile(textures[group][idx]);
	RENDERING(c->e)->hide = false;
	RENDERING(c->e)->color = Color(1, 1, 1, MathUtil::RandomFloatInRange(0.6, 0.9));
	TRANSFORM(c->e)->size = Vector2(width, width / ratio);
	c->visible = false;
	c->speed = cloudSpeed[group].random();
	c->group = group;
	return c;
}

void BackgroundManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
}

GameState BackgroundManager::Update(float dt) {
}

void BackgroundManager::BackgroundUpdate(float dt) {
	for (std::vector<Actor*>::iterator it=clouds.begin(); it!=clouds.end(); ++it) {
		Actor* c = *it;

		TransformationComponent* tc = TRANSFORM(c->e);
		tc->position.X += (skySpeed + c->speed) * dt;

		if (!theRenderingSystem.isEntityVisible(c->e)) {
			if (c->visible)
				initCloud(c, c->group);
		} else {
			c->visible = true;
		}
	}
}

void BackgroundManager::switchAnim(AnimatedActor* a)
{
	a->frames++;
	if (a->frames>=30/(MathUtil::Abs(a->actor.speed)+MathUtil::Abs(CAMERASPEED))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

void BackgroundManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
}
