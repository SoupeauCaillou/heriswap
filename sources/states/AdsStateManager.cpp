#include "AdsStateManager.h"

#include <sstream>

#include <base/PlacementHelper.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"

#include "DepthLayer.h"


AdsStateManager::~AdsStateManager() {
	theEntityManager.DeleteEntity(eAds);
}

void AdsStateManager::Setup() {
	eAds = theEntityManager.CreateEntity();
	ADD_COMPONENT(eAds, Transformation);
	ADD_COMPONENT(eAds, Rendering);
	ADD_COMPONENT(eAds, Button);
	TRANSFORM(eAds)->z = DL_Ads;
	TRANSFORM(eAds)->size = Vector2(PlacementHelper::WindowWidth, PlacementHelper::WindowHeight);
	TRANSFORM(eAds)->position = Vector2(PlacementHelper::GimpXToScreen(0),PlacementHelper::GimpYToScreen(0));
	BUTTON(eAds)->enabled = false;
}

void AdsStateManager::Enter() {
	stateActiveDuration = 0;
	LOGI("%s", __PRETTY_FUNCTION__);

	gameb4Ads = storage->getGameCountBeforeNextAd();

	if (!gameb4Ads) {
		BUTTON(eAds)->enabled = true;
		RENDERING(eAds)->color = Color(1.f,1.f,1.f);
	} else {
		RENDERING(eAds)->color = Color(0.f,0.f,0.f);
	}
	RENDERING(eAds)->hide = false;
}

GameState AdsStateManager::Update(float dt) {
	stateActiveDuration += dt;
	if (gameb4Ads>0 || BUTTON(eAds)->clicked) {
		if (BUTTON(eAds)->clicked)
			successMgr->sDonator();
		return AdsToBlackState;
	}
	return Ads;
}

void AdsStateManager::Exit() {
	LOGI("%s : %.3f", __PRETTY_FUNCTION__, stateActiveDuration);
	BUTTON(eAds)->enabled = false;
	if (gameb4Ads==0)
		gameb4Ads=3;
    storage->setGameCountBeforeNextAd(gameb4Ads);
}

void AdsStateManager::LateExit() {
	LOGW("ici on cache pour de vrai");
	RENDERING(eAds)->hide = true;
}
