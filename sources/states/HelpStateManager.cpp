#include "HelpStateManager.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"

HelpStateManager::~HelpStateManager() {
	theEntityManager.DeleteEntity(eHelp);
}

void HelpStateManager::Setup() {
	eHelp = theEntityManager.CreateEntity();
	ADD_COMPONENT(eHelp, Transformation);
	ADD_COMPONENT(eHelp, Rendering);
	ADD_COMPONENT(eHelp, Button);
	TRANSFORM(eHelp)->z = DL_PauseUIBg;
	TRANSFORM(eHelp)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(eHelp)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(300));
	BUTTON(eHelp)->enabled = false;
}

void HelpStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	
	switch (mode) {
		case Normal :
			RENDERING(eHelp)->color = Color(1.f,0.f,0.f);
			break;
		case TilesAttack :
			RENDERING(eHelp)->color = Color(0.f,1.f,0.f);
			break;
	}
	RENDERING(eHelp)->hide = false;
	BUTTON(eHelp)->enabled = true;
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
	BUTTON(eHelp)->enabled = false;
}
