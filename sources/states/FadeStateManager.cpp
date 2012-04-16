#include "FadeStateManager.h"
#include "base/TouchInputManager.h"
#include "../DepthLayer.h"

FadeGameStateManager::FadeGameStateManager(Entity eFade, FadeType fade, GameState whoAmI, GameState whoIsNext, float t):
	eThing(eFade), fading(fade), iAm(whoAmI), heIs(whoIsNext), timeout(t) {
}

void FadeGameStateManager::Setup() {
	eFading = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFading, Transformation);
	ADD_COMPONENT(eFading, Rendering);
	TRANSFORM(eFading)->position = Vector2(0,0);
	TRANSFORM(eFading)->size = Vector2(10,20);
	RENDERING(eFading)->hide = true;
	RENDERING(eFading)->color = Color(0,0,0);
	TRANSFORM(eFading)->z = DL_Fading;

	ADD_COMPONENT(eFading, ADSR);
	ADSR(eFading)->idleValue = 0;
	ADSR(eFading)->attackValue = 1.0;
	ADSR(eFading)->attackTiming = 0.2;
	ADSR(eFading)->decayTiming = 0;
	ADSR(eFading)->sustainValue = 1.0;
	ADSR(eFading)->releaseTiming = .2;
	ADSR(eFading)->attackMode = Quadratic;
	if (eThing) RENDERING(eThing)->hide = true;
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

	accum = 0;
	updateColor(eFading, fading);
}

GameState FadeGameStateManager::Update(float dt) {
	updateColor(eFading, fading);
	if (ADSR(eFading)->value == ADSR(eFading)->sustainValue) {
		accum += dt;
		if (accum >= timeout)
			return heIs;
		else
			return iAm;
	}
	if (theTouchInputManager.isTouched() && !theTouchInputManager.wasTouched()) {
		return heIs;
	}
	return iAm;
}

void FadeGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(eFading)->hide = true;
	if (eThing) RENDERING(eThing)->hide = true;
	ADSR(eFading)->active = false;
}
