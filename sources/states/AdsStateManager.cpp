#include "AdsStateManager.h"

#include "base/PlacementHelper.h"

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"
#include "Game.h"
#include "DepthLayer.h"


AdsStateManager::~AdsStateManager() {
	theEntityManager.DeleteEntity(eAds);
}

void AdsStateManager::Setup() {
	eAds = theEntityManager.CreateEntity();
	ADD_COMPONENT(eAds, Transformation);
	ADD_COMPONENT(eAds, Rendering);
	ADD_COMPONENT(eAds, Button);
	TRANSFORM(eAds)->z = DL_MainMenuFg;
	TRANSFORM(eAds)->size = Vector2(PlacementHelper::WindowWidth, PlacementHelper::WindowHeight);
	TRANSFORM(eAds)->position = Vector2(PlacementHelper::GimpXToScreen(0),PlacementHelper::GimpYToScreen(0));
	BUTTON(eAds)->enabled = false;
}

void AdsStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	RENDERING(eAds)->hide = false;
	BUTTON(eAds)->enabled = true;
}

GameState AdsStateManager::Update(float dt) {
	if (BUTTON(eAds)->clicked) {
		return AdsToBlackState;
	}
	return Ads;
}

void AdsStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(eAds)->hide = true;
	BUTTON(eAds)->enabled = false;
}
