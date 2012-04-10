#include "PauseStateManager.h"
#include "../DepthLayer.h"

PauseStateManager::PauseStateManager() {

}

PauseStateManager::~PauseStateManager() {
	theTextRenderingSystem.DestroyLocalEntity(eRestart);
	theTextRenderingSystem.DestroyLocalEntity(bRestart);
}

void PauseStateManager::Setup() {
	eRestart = theTextRenderingSystem.CreateLocalEntity(10);
	ADD_COMPONENT(eRestart, Rendering);
	TRANSFORM(eRestart)->position = Vector2(3.5, 0);
	TRANSFORM(eRestart)->z = DL_PauseUI;
	TEXT_RENDERING(eRestart)->text = "Reprendre";
	TEXT_RENDERING(eRestart)->hide = true;

	bRestart = theEntityManager.CreateEntity();
	ADD_COMPONENT(bRestart, Transformation);
	ADD_COMPONENT(bRestart, Container);
	ADD_COMPONENT(bRestart, Button);
	ADD_COMPONENT(bRestart, Rendering);
	RENDERING(bRestart)->color = Color(.0, 1.0, .0, .5);
	TRANSFORM(bRestart)->z = DL_PauseUI;
	BUTTON(bRestart)->clicked = false;
	CONTAINER(bRestart)->includeChildren = true;
	CONTAINER(bRestart)->entities.push_back(eRestart);

}

void PauseStateManager::Enter() {

	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = false;
	RENDERING(bRestart)->hide = false;
	BUTTON(bRestart)->clicked = false;

}

GameState PauseStateManager::Update(float dt) {
	if (BUTTON(bRestart)->clicked) {
		return ScoreBoard;
	}
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	RENDERING(bRestart)->hide = true;
	BUTTON(bRestart)->clicked = true;
}
