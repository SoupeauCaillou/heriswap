#include "PauseStateManager.h"
#include "DepthLayer.h"
#include "PlacementHelper.h"

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
	TRANSFORM(eRestart)->z = DL_PauseUI;
	TEXT_RENDERING(eRestart)->text = "Reprendre";
	TEXT_RENDERING(eRestart)->hide = true;
	TEXT_RENDERING(eRestart)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eRestart)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(450));

	bRestart = theEntityManager.CreateEntity();
	ADD_COMPONENT(bRestart, Transformation);
	ADD_COMPONENT(bRestart, Container);
	ADD_COMPONENT(bRestart, Button);
	ADD_COMPONENT(bRestart, Rendering);
	ADD_COMPONENT(bRestart, Sound);
	RENDERING(bRestart)->color = Color(.0, 1.0, .0, .5);
	TRANSFORM(bRestart)->z = DL_PauseUI;
	BUTTON(bRestart)->enabled = false;
	CONTAINER(bRestart)->includeChildren = true;
	CONTAINER(bRestart)->entities.push_back(eRestart);
	SOUND(bRestart)->type = SoundComponent::EFFECT;

	eAbort = theTextRenderingSystem.CreateLocalEntity(6);
	TRANSFORM(eAbort)->z = DL_CombinationMark;
	TEXT_RENDERING(eAbort)->hide = true;
	TEXT_RENDERING(eAbort)->text = "Abandonner";
	TEXT_RENDERING(eAbort)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eAbort)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(850));

	bAbort = theEntityManager.CreateEntity();
	ADD_COMPONENT(bAbort, Transformation);
	ADD_COMPONENT(bAbort, Container);
	ADD_COMPONENT(bAbort, Button);
	ADD_COMPONENT(bAbort, Rendering);
	ADD_COMPONENT(bAbort, Sound);
	BUTTON(bAbort)->enabled = false;
	RENDERING(bAbort)->color = Color(.0, 1.0, .0, .5);
	TRANSFORM(bAbort)->z = DL_CombinationMark;
	CONTAINER(bAbort)->includeChildren = true;
	CONTAINER(bAbort)->entities.push_back(eAbort);
	SOUND(bAbort)->type = SoundComponent::EFFECT;
}

void PauseStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
	TEXT_RENDERING(eRestart)->hide = false;
	RENDERING(bRestart)->hide = false;
	BUTTON(bRestart)->enabled = true;
	TEXT_RENDERING(eAbort)->hide = false;
	RENDERING(bAbort)->hide = false;
	BUTTON(bAbort)->enabled = true;
}

GameState PauseStateManager::Update(float dt) {
	if (BUTTON(bAbort)->clicked) {
		SOUND(bAbort)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return MainMenu;
	} if (BUTTON(bRestart)->clicked) {
		SOUND(bRestart)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return Unpause;
	}
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	RENDERING(bRestart)->hide = true;
	BUTTON(bRestart)->enabled = false;
	TEXT_RENDERING(eAbort)->hide = true;
	RENDERING(bAbort)->hide = true;
	BUTTON(bAbort)->enabled = false;
}
