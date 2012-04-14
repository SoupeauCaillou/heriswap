#include "BackgroundManager.h"
#include "../DepthLayer.h"

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
	#define GIMP_Y_TO_GAME(x) -(windowHeight * (x - 640.0f) / 1280.0f)
	cloudY[0] = Interval<float>(GIMP_Y_TO_GAME(96.0), GIMP_Y_TO_GAME(252));
	cloudY[1] = Interval<float>(GIMP_Y_TO_GAME(310), GIMP_Y_TO_GAME(490));
	cloudY[2] = Interval<float>(GIMP_Y_TO_GAME(570), GIMP_Y_TO_GAME(640));

	#define GIMP_W_TO_GAME(x) (10.0 * x / 800.0f)
	cloudSize[0] = Interval<float>(GIMP_W_TO_GAME(230), GIMP_W_TO_GAME(410));
	cloudSize[1] = Interval<float>(GIMP_W_TO_GAME(290), GIMP_W_TO_GAME(190));
	cloudSize[2] = Interval<float>(GIMP_W_TO_GAME(100), GIMP_W_TO_GAME(200));
	
	cloudSpeed[0] = Interval<float>(-0.5, -0.3);
	cloudSpeed[1] = Interval<float>(-0.25, -0.13);
	cloudSpeed[2] = Interval<float>(-0.1, -0.03);
	
	textures[0].push_back("nuages/haut_0.png");
	textures[0].push_back("nuages/haut_1.png");
	textures[0].push_back("nuages/haut_2.png");
	textures[1].push_back("nuages/moyen_0.png");
	textures[1].push_back("nuages/moyen_1.png");
	textures[2].push_back("nuages/bas_0.png");
	textures[2].push_back("nuages/bas_1.png");
	textures[2].push_back("nuages/bas_2.png");
	textures[2].push_back("nuages/bas_3.png");
	textures[2].push_back("nuages/bas_4.png");
}


BackgroundManager::~BackgroundManager() {
	for(int i=0; i<clouds.size(); i++) {
		theEntityManager.DeleteEntity(clouds[i]->e);
		delete clouds[i];
	}
	clouds.clear();
}

void BackgroundManager::Setup() {
	for(int j=0; j<3; j++) {
		for(int i=0; i<3; i++) {
			Actor* c = new Actor();
			Entity e = theEntityManager.CreateEntity();
			theEntityManager.AddComponent(e, &theTransformationSystem);
			theEntityManager.AddComponent(e, &theADSRSystem);
			theEntityManager.AddComponent(e, &theRenderingSystem);
			c->e = e;
			clouds.push_back(initCloud(c, j));
		}
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
