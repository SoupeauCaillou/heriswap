#include "AdsStateManager.h"

#include <sstream>

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
	TRANSFORM(eAds)->z = DL_Ads;
	TRANSFORM(eAds)->size = Vector2(PlacementHelper::WindowWidth, PlacementHelper::WindowHeight);
	TRANSFORM(eAds)->position = Vector2(PlacementHelper::GimpXToScreen(0),PlacementHelper::GimpYToScreen(0));
	BUTTON(eAds)->enabled = false;
}

void AdsStateManager::Enter() {
	stateActiveDuration = 0;
	LOGI("%s", __PRETTY_FUNCTION__);

	std::string s;
	storage->request("select value from info where opt='gameb4Ads'", &s, 0);
	gameb4Ads = std::atoi(s.c_str());
	
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
		return AdsToBlackState;
	}
	return Ads;
}

void AdsStateManager::Exit() {
	LOGI("%s : %.3f", __PRETTY_FUNCTION__, stateActiveDuration);
	BUTTON(eAds)->enabled = false;
	if (gameb4Ads==0)
		gameb4Ads=3;
	std::stringstream s;
	s << "update info set value='" << gameb4Ads-1 << "' where opt='gameb4Ads'";
	storage->request(s.str(),0, 0);
}

void AdsStateManager::LateExit() {
	RENDERING(eAds)->hide = true;
}
