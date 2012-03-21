#include "PauseStateManager.h"
#include "../DepthLayer.h"

PauseStateManager::PauseStateManager() {

}

PauseStateManager::~PauseStateManager() {
	theTextRenderingSystem.DestroyLocalEntity(eRestart);
}

void PauseStateManager::Setup() {
	eRestart = theTextRenderingSystem.CreateLocalEntity(10);
	ADD_COMPONENT(eRestart, Button);
	ADD_COMPONENT(eRestart, Rendering);
	TRANSFORM(eRestart)->position = Vector2(3.5, 0);
	TRANSFORM(eRestart)->z = DL_PauseUI;
	TEXT_RENDERING(eRestart)->text = "Reprendre";
	TEXT_RENDERING(eRestart)->hide = true;
}

void PauseStateManager::Enter() {

	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = false;
}

GameState PauseStateManager::Update(float dt, GameModeManager* modeMng) {
	if (BUTTON(eRestart)->clicked) {
		return ScoreBoard;
	}
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	BUTTON(eRestart)->clicked = false;

}
