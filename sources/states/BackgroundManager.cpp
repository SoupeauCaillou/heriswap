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

	for(int i=0; i<3; i++) {
		AnimatedActor* c = new AnimatedActor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->actor.e = e;
		animals.push_back(initAnimal(c));
	}


	for(int i=0; i<2; i++) {
		Actor* c = new Actor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->e = e;
		landscapes.push_back(initLandscape(c, false));


		Actor* tree = new Actor();
		Entity a = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(a, &theTransformationSystem);
		theEntityManager.AddComponent(a, &theADSRSystem);
		theEntityManager.AddComponent(a, &theRenderingSystem);
		tree->e = a;
		tree->visible = false; //nécessaire pour le replacement
		trees.push_back(initLandscape(tree, true));
	}
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

BackgroundManager::AnimatedActor* BackgroundManager::initAnimal(AnimatedActor* c) {
	c->anim.clear();
	c->frames=0;
	float t = MathUtil::RandomFloat();
	std::stringstream tex;
	if (MathUtil::RandomInt(2)) {
		int i = MathUtil::RandomInt(8);
		tex << "herisson0_" << i << ".png";
		c->anim.push_back(tex.str());
		tex.str("");
		tex << "herisson1_" << i << ".png";
		c->anim.push_back(tex.str());

		c->actor.speed = CAMERASPEED+MathUtil::Lerp(-0.2f, -1.5f, t);

		TRANSFORM(c->actor.e)->position.X = MathUtil::RandomFloat() * (xHerissonStartRange.Y-xHerissonStartRange.X) + xHerissonStartRange.X;
		TRANSFORM(c->actor.e)->position.Y = t * (yHerissonRange.Y-yHerissonRange.X) + yHerissonRange.X;
		TRANSFORM(c->actor.e)->size = Vector2(2, 1) * MathUtil::Lerp(herissonScaleRange.X, herissonScaleRange.Y, t);
	} else {
		int i = MathUtil::RandomInt(8);
		tex << "papillon0_" << i << ".png";
		c->anim.push_back(tex.str());
		tex.str("");
		tex << "papillon1_" << i << ".png";
		c->anim.push_back(tex.str());
		TRANSFORM(c->actor.e)->position.X = MathUtil::RandomFloat() * (xPapillonStartRange.Y-xPapillonStartRange.X) + xPapillonStartRange.X;
		TRANSFORM(c->actor.e)->position.Y = t * (yPapillonRange.Y-yPapillonRange.X) + yPapillonRange.X;
		TRANSFORM(c->actor.e)->size = Vector2(2, 1) * MathUtil::Lerp(papillonScaleRange.X, papillonScaleRange.Y, t);
		c->actor.speed = CAMERASPEED+MathUtil::Lerp(0.6f, 1.8f, t);
	}
	TRANSFORM(c->actor.e)->z = DL_Animal;
	RENDERING(c->actor.e)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
	RENDERING(c->actor.e)->hide = false;
	c->actor.visible = false;

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
