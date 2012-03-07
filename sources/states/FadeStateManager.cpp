#include "FadeStateManager.h"

FadeGameStateManager::FadeGameStateManager(Entity eFade, FadeType fade, GameState whoAmI, GameState whoIsNext):
	eThing(eFade), fading(fade), iAm(whoAmI), heIs(whoIsNext) {
}

void FadeGameStateManager::Setup() {
	eFading = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFading, Transformation);
	ADD_COMPONENT(eFading, Rendering);
	TRANSFORM(eFading)->position = Vector2(0,0);
	RENDERING(eFading)->size = Vector2(10,20);
	RENDERING(eFading)->hide = true;
	RENDERING(eFading)->texture = theRenderingSystem.loadTextureFile("combinationMark1.png");
	RENDERING(eFading)->color = Color(0,0,0);
	TRANSFORM(eFading)->z = 40;

	ADD_COMPONENT(eFading, ADSR);
	ADSR(eFading)->idleValue = 0;
	ADSR(eFading)->attackValue = 0;
	ADSR(eFading)->attackTiming = 0.;
	ADSR(eFading)->decayTiming = 2.;
	ADSR(eFading)->sustainValue = 1.0;
	ADSR(eFading)->releaseTiming = 2.;
}

static void updateColor(Entity eFading, FadeType fading) {
	float value = ADSR(eFading)->value;
	if (fading == FadeIn)
		RENDERING(eFading)->color.a = 1 - value;
	else
		RENDERING(eFading)->color.a = value;
}

void FadeGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(eFading)->hide = false;
	if (eThing) RENDERING(eThing)->hide = false;
	ADSR(eFading)->active = true;

	updateColor(eFading, fading);
}

GameState FadeGameStateManager::Update(float dt) {
	updateColor(eFading, fading);

	if (ADSR(eFading)->value == ADSR(eFading)->sustainValue)
		return heIs;
	return iAm;
}

void FadeGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(eFading)->hide = true;
	if (eThing) RENDERING(eThing)->hide = true;
	ADSR(eFading)->active = false;
}
