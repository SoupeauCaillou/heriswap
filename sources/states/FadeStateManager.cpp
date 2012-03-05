#include "FadeStateManager.h"

FadeGameStateManager::FadeGameStateManager() {

}

void FadeGameStateManager::Setup() {
	eFading = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFading, ADSR);
	ADD_COMPONENT(eFading, Transformation);
	ADD_COMPONENT(eFading, Rendering);

	ADSR(eFading)->idleValue = 0;
	ADSR(eFading)->attackValue = 0.5;
	ADSR(eFading)->attackTiming = 1;
	ADSR(eFading)->decayTiming = 2;
	ADSR(eFading)->sustainValue = 1.0;
	ADSR(eFading)->releaseTiming = 0;

	TRANSFORM(eFading)->z = 40;
	TRANSFORM(eFading)->position = Vector2(0,0);
	RENDERING(eFading)->size = Vector2(420,700);
	RENDERING(eFading)->hide = true;
	RENDERING(eFading)->texture = theRenderingSystem.loadTextureFile("background.png");
}
	



void FadeGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(eFading)->hide = false;
	ADSR(eFading)->active = true;
}

GameState FadeGameStateManager::Update(float dt) {
	RENDERING(eFading)->color = Color(1-ADSR(eFading)->value);
	if (ADSR(eFading)->value == ADSR(eFading)->sustainValue)
		return MainMenu;
	return Fade;
}
	
void FadeGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(eFading)->hide = true;
	ADSR(eFading)->active = false;
}

