#include "BackgroundManager.h"


struct BackgroundManager::Cloud {
	Entity e;
	float speed;
	bool visible;
};

BackgroundManager::BackgroundManager() {

}

BackgroundManager::~BackgroundManager() {
	for(int i=0; i<clouds.size(); i++) {
		theEntityManager.DeleteEntity(clouds[i]->e);
		delete clouds[i];
	}
	clouds.clear();
}

BackgroundManager::Cloud* BackgroundManager::initCloud(Cloud* c) {
	float t = MathUtil::RandomFloat();
	TRANSFORM(c->e)->position.X = xStartRange.Length() * MathUtil::RandomFloat() + xStartRange.X;
	TRANSFORM(c->e)->position.Y = t * yRange.Length() + yRange.X;
	TRANSFORM(c->e)->z = -0.5;
	std::stringstream tex;
	tex << "cloud" << 1+MathUtil::RandomInt(3) << ".png";
	RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile(tex.str());
	RENDERING(c->e)->size = Vector2(2, 1) * MathUtil::Lerp(scaleRange.X, scaleRange.Y, t);
	c->visible = false;
	c->speed = MathUtil::Lerp(-0.2f, -1.5f, t);

	return c;
}

void BackgroundManager::Setup() {
	Vector2 range = xStartRange;
	xStartRange.X = -5;
	for(int i=0; i<3; i++) {
		Cloud* c = new Cloud();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->e = e;
		clouds.push_back(initCloud(c));
	}
	xStartRange = range;
}

void BackgroundManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
}

GameState BackgroundManager::Update(float dt) {
}

void BackgroundManager::BackgroundUpdate(float dt) {
	for (std::vector<Cloud*>::iterator it=clouds.begin(); it!=clouds.end(); ++it) {
		Cloud* c = *it;

		TransformationComponent* tc = TRANSFORM(c->e);
		tc->position.X += c->speed * dt;

		if (!theRenderingSystem.isEntityVisible(c->e)) {
			if (c->visible)
				initCloud(c);
		} else {
			c->visible = true;
		}
	}
}

void BackgroundManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
}
