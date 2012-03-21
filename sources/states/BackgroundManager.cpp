#include "BackgroundManager.h"
#include "../DepthLayer.h"

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
	xHerissonStartRange.X = 6;
	xHerissonStartRange.Y = 8;
	yHerissonRange.X = -6.3;
	yHerissonRange.Y = -8;
	herissonScaleRange.X = 0.4;
	herissonScaleRange.Y = 1.5;
	xPapillonStartRange.X = -5;
	xPapillonStartRange.Y = -8;
	yPapillonRange.X = 0;
	yPapillonRange.Y = 8.;
	papillonScaleRange.X = 0.9;
	papillonScaleRange.Y = 1.5;
}


BackgroundManager::~BackgroundManager() {
	for(int i=0; i<clouds.size(); i++) {
		theEntityManager.DeleteEntity(clouds[i]->e);
		delete clouds[i];
	}
	clouds.clear();

	for(int i=0; i<animals.size(); i++) {
		theEntityManager.DeleteEntity(animals[i]->actor.e);
		delete animals[i];
	}
	animals.clear();
}

BackgroundManager::Actor* BackgroundManager::initCloud(Actor* c) {
	float t = MathUtil::RandomFloat();
	TRANSFORM(c->e)->position.X = xCloudStartRange.Length() * MathUtil::RandomFloat() + xCloudStartRange.X;
	TRANSFORM(c->e)->position.Y = t * (yCloudRange.Y-yCloudRange.X) + yCloudRange.X;
	TRANSFORM(c->e)->z = DL_Cloud;
	std::stringstream tex;
	tex << "cloud" << 1+MathUtil::RandomInt(3) << ".png";
	RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile(tex.str());
	RENDERING(c->e)->hide = false;
	TRANSFORM(c->e)->size = Vector2(2, 1) * MathUtil::Lerp(cloudScaleRange.X, cloudScaleRange.Y, t);
	c->visible = false;
	c->speed = MathUtil::Lerp(-0.2f, -1.5f, t);

	return c;
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

	for(int i=0; i<3; i++) {
		AnimatedActor* c = new AnimatedActor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->actor.e = e;
		animals.push_back(initAnimal(c));
	}
}

BackgroundManager::AnimatedActor* BackgroundManager::initAnimal(AnimatedActor* c) {
	c->anim.clear();
	c->frames=0;
	float t = MathUtil::RandomFloat();
	std::stringstream tex;
	if (MathUtil::RandomInt(2)) {
		int i = MathUtil::RandomInt(8);
		tex << "animals/herisson0_" << i << ".png";
		c->anim.push_back(tex.str());
		tex.str("");
		tex << "animals/herisson1_" << i << ".png";
		c->anim.push_back(tex.str());

		c->actor.speed = MathUtil::Lerp(-0.2f, -1.5f, t);

		TRANSFORM(c->actor.e)->position.X = MathUtil::RandomFloat() * (xHerissonStartRange.Y-xHerissonStartRange.X) + xHerissonStartRange.X;
		TRANSFORM(c->actor.e)->position.Y = t * (yHerissonRange.Y-yHerissonRange.X) + yHerissonRange.X;
		TRANSFORM(c->actor.e)->z = .5;
		TRANSFORM(c->actor.e)->size = Vector2(2, 1) * MathUtil::Lerp(herissonScaleRange.X, herissonScaleRange.Y, t);
	} else {
		int i = MathUtil::RandomInt(8);
		tex << "animals/papillon0_" << i << ".png";
		c->anim.push_back(tex.str());
		tex.str("");
		tex << "animals/papillon1_" << i << ".png";
		c->anim.push_back(tex.str());
		TRANSFORM(c->actor.e)->position.X = MathUtil::RandomFloat() * (xPapillonStartRange.Y-xPapillonStartRange.X) + xPapillonStartRange.X;
		TRANSFORM(c->actor.e)->position.Y = t * (yPapillonRange.Y-yPapillonRange.X) + yPapillonRange.X;
		TRANSFORM(c->actor.e)->z = .5;
		TRANSFORM(c->actor.e)->size = Vector2(2, 1) * MathUtil::Lerp(papillonScaleRange.X, papillonScaleRange.Y, t);
		c->actor.speed = MathUtil::Lerp(0.6f, 1.8f, t);
	}
	RENDERING(c->actor.e)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
	RENDERING(c->actor.e)->hide = false;
	c->actor.visible = false;

	return c;
}

void BackgroundManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
}

GameState BackgroundManager::Update(float dt, GameModeManager* modeMng) {
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
	for (std::vector<AnimatedActor*>::iterator it=animals.begin(); it!=animals.end(); ++it) {
		AnimatedActor* a = *it;

		TransformationComponent* tc = TRANSFORM(a->actor.e);
		tc->position.X += a->actor.speed * dt;

		if (!theRenderingSystem.isEntityVisible(a->actor.e)) {
			if (a->actor.visible)
				initAnimal(a);
		} else {
			a->actor.visible = true;
			switchAnim(a);
		}
	}
}

void BackgroundManager::switchAnim(AnimatedActor* a)
{
	a->frames++;
	if (a->frames>=MathUtil::Abs(30/a->actor.speed)) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

void BackgroundManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
}
