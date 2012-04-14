#include "BackgroundManager.h"
#include "../DepthLayer.h"

#define CAMERASPEED -1.f
struct BackgroundManager::Actor {
	Entity e;
	float speed;
	bool visible;
};

struct BackgroundManager::AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct Actor actor;
};



BackgroundManager::BackgroundManager() {
}


BackgroundManager::~BackgroundManager() {
	for(int i=0; i<clouds.size(); i++) {
		theEntityManager.DeleteEntity(clouds[i]->e);
		delete clouds[i];
	}
	clouds.clear();

	for(int i=0; i<landscapes.size(); i++) {
		theEntityManager.DeleteEntity(landscapes[i]->e);
		delete landscapes[i];
	}
	landscapes.clear();
}

void BackgroundManager::Setup() {
	Vector2 range = xCloudStartRange;
	xCloudStartRange.X = -5;
	for(int i=0; i<3; i++) {
		Actor* c = new Actor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->e = e;
		clouds.push_back(initCloud(c));
	}
	xCloudStartRange = range;
}
BackgroundManager::Actor* BackgroundManager::initLandscape(Actor* c, bool isATree) {
	float fullscreenWidth = 10.0;
	float fullscreenHeight = 10.0 * 700 / 420;


	if (isATree) {
		TRANSFORM(c->e)->z = DL_Tree;
		RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile("tree.png");
		TRANSFORM(c->e)->size = Vector2(fullscreenHeight * 250.0 / 800, fullscreenHeight);
		if (trees.empty()) {
			TRANSFORM(c->e)->position = Vector2(-5 + TRANSFORM(c->e)->size.X * 0.5, 0);
		} else {
			if (c->visible)
				TRANSFORM(c->e)->position = Vector2(2*fullscreenWidth -5 - TRANSFORM(c->e)->size.X*0.5, 0);
			else
				TRANSFORM(c->e)->position = Vector2(fullscreenWidth-5 + TRANSFORM(c->e)->size.X * 0.5, 0);
		}

	} else {
		TRANSFORM(c->e)->z = DL_Background;
		RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile("background.png");
		TRANSFORM(c->e)->size = Vector2(fullscreenWidth, fullscreenWidth * 428.0/480);
		if (landscapes.empty()) {
			TRANSFORM(c->e)->position = Vector2(0, (-fullscreenHeight + TRANSFORM(c->e)->size.Y ) * 0.5);
		} else {
			TRANSFORM(c->e)->position = Vector2(fullscreenWidth, (-fullscreenHeight + TRANSFORM(c->e)->size.Y ) * 0.5);
		}
	}
	RENDERING(c->e)->hide = false;
	c->visible = false;
	c->speed = CAMERASPEED;

	return c;
}

BackgroundManager::Actor* BackgroundManager::initCloud(Actor* c) {
	float t = MathUtil::RandomFloat();
	TRANSFORM(c->e)->position.X = xCloudStartRange.Length() * MathUtil::RandomFloat() + xCloudStartRange.X;
	TRANSFORM(c->e)->position.Y =  t * (yCloudRange.Y-yCloudRange.X) + yCloudRange.X;
	TRANSFORM(c->e)->z = DL_Cloud;
	std::stringstream tex;
	tex << "cloud" << 1+MathUtil::RandomInt(3) << ".png";
	RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile(tex.str());
	RENDERING(c->e)->hide = false;
	TRANSFORM(c->e)->size = Vector2(2, 1) * MathUtil::Lerp(cloudScaleRange.X, cloudScaleRange.Y, t);
	c->visible = false;
	c->speed = CAMERASPEED+MathUtil::Lerp(-0.2f, -1.5f, t);

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
		tc->position.X += c->speed * dt;

		if (!theRenderingSystem.isEntityVisible(c->e)) {
			if (c->visible)
				initCloud(c);
		} else {
			c->visible = true;
		}
	}
	for (std::vector<Actor*>::iterator it=landscapes.begin(); it!=landscapes.end(); ++it) {
		Actor* c = *it;

		TransformationComponent* tc = TRANSFORM(c->e);
		tc->position.X += c->speed * dt;

		if (!theRenderingSystem.isEntityVisible(c->e)) {
			if (c->visible)
				initLandscape(c, false);
		} else {
			c->visible = true;
		}
	}
	for (std::vector<Actor*>::iterator it=trees.begin(); it!=trees.end(); ++it) {
		Actor* c = *it;

		TransformationComponent* tc = TRANSFORM(c->e);
		tc->position.X += c->speed * dt;

		if (!theRenderingSystem.isEntityVisible(c->e)) {
			if (c->visible)
				initLandscape(c, true);
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
