#include "PauseStateManager.h"
#include "../DepthLayer.h"

PauseStateManager::PauseStateManager() {

}

PauseStateManager::~PauseStateManager() {
	theTextRenderingSystem.DestroyLocalEntity(eRestart);
	theTextRenderingSystem.DestroyLocalEntity(eAbort);
	theTextRenderingSystem.DestroyLocalEntity(bRestart);
	theTextRenderingSystem.DestroyLocalEntity(bAbort);
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

	eAbort = theTextRenderingSystem.CreateLocalEntity(6);
	TRANSFORM(eAbort)->z = DL_CombinationMark;
	TRANSFORM(eAbort)->position = Vector2(4, -3);
	TEXT_RENDERING(eAbort)->hide = true;
	TEXT_RENDERING(eAbort)->text = "Abandonner";
	bAbort = theEntityManager.CreateEntity();
	ADD_COMPONENT(bAbort, Transformation);
	ADD_COMPONENT(bAbort, Container);
	ADD_COMPONENT(bAbort, Button);
	ADD_COMPONENT(bAbort, Rendering);
	BUTTON(bAbort)->clicked = false;
	RENDERING(bAbort)->color = Color(.0, 1.0, .0, .5);
	TRANSFORM(bAbort)->z = DL_CombinationMark;
	CONTAINER(bAbort)->includeChildren = true;
	CONTAINER(bAbort)->entities.push_back(eAbort);
}

void PauseStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = false;
	RENDERING(bRestart)->hide = false;
	BUTTON(bRestart)->clicked = false;
	TEXT_RENDERING(eAbort)->hide = false;
	RENDERING(bAbort)->hide = false;
	BUTTON(bAbort)->clicked = false;
}

GameState PauseStateManager::Update(float dt) {
	if (BUTTON(bAbort)->clicked)
		return EndMenu;
	if (BUTTON(bRestart)->clicked)
		return ScoreBoard;
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	RENDERING(bRestart)->hide = true;
	BUTTON(bRestart)->clicked = true;
	BUTTON(bAbort)->clicked = true;
	TEXT_RENDERING(eAbort)->hide = true;
	RENDERING(bAbort)->hide = true;
}
