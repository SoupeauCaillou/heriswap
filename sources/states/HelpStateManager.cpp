#include "HelpStateManager.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"

HelpStateManager::~HelpStateManager() {
	theTextRenderingSystem.DestroyLocalEntity(eHelp);
}

void HelpStateManager::Setup() {
	eHelp = theTextRenderingSystem.CreateEntity();
	ADD_COMPONENT(eHelp, Rendering);
	TRANSFORM(eHelp)->z = DL_PauseUIFg;
	TRANSFORM(eHelp)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(300));
}

void HelpStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(eHelp)->hide = false;
}

GameState HelpStateManager::Update(float dt) {
	if (BUTTON(eHelp)->clicked) {
		return oldState;
	}
	return Help;
}

void HelpStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(eHelp)->hide = true;
}
