/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "AdsStateManager.h"

#include <sstream>

#include <glm/glm.hpp>

#include <base/PlacementHelper.h>
#include <base/EntityManager.h>
#include <base/ObjectSerializer.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "modes/GameModeManager.h"
#include "StateManager.h"
#include "base/TimeUtil.h"

#include "DepthLayer.h"

void AdsStateManager::Setup() {
	eAds = theEntityManager.CreateEntity("eAds");
	ADD_COMPONENT(eAds, Transformation);
	ADD_COMPONENT(eAds, Rendering);
	RENDERING(eAds)->color = Color(0.f,0.f,0.f);
	ADD_COMPONENT(eAds, Button);
	TRANSFORM(eAds)->z = DL_Ads;
	TRANSFORM(eAds)->size = glm::vec2((float)PlacementHelper::WindowWidth, 
 									  (float)PlacementHelper::WindowHeight);
	TRANSFORM(eAds)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(0),
										  (float)PlacementHelper::GimpYToScreen(0));
	BUTTON(eAds)->enabled = false;
    lastAdTime = -30;

    //init in db
    storageAPI->setOption("gameB4Ads", "2");
}

void AdsStateManager::Enter() {
	stateActiveDuration = 0;
	gameb4Ads = ObjectSerializer<int>::string2object(storageAPI->getOption("gameB4Ads"));
 	LOGI("GameB4Bds: " <<  gameb4Ads);
 	if (gameb4Ads > 3) {
	 	gameb4Ads = 3;
 	}

	float timeSinceLAstAd = TimeUtil::GetTime() - lastAdTime;

	// postpone ad if previous ad was shown less than 30sec ago
	if (gameb4Ads <= 0 && timeSinceLAstAd < 30) {
		gameb4Ads = 1;
	}

	if (gameb4Ads==0 || timeSinceLAstAd > 150) {
		if (adAPI->showAd()) {
			BUTTON(eAds)->enabled = true;
	        gameb4Ads = 0;
	        lastAdTime = TimeUtil::GetTime();
		} else {
			gameb4Ads = 1;
		}
	}
	RENDERING(eAds)->show = true;
}

GameState AdsStateManager::Update(float dt) {
	stateActiveDuration += dt;
	if (gameb4Ads>0 || BUTTON(eAds)->clicked || adAPI->done()) {
		return AdsToBlackState;
	}
	return Ads;
}

void AdsStateManager::Exit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "' : '" << stateActiveDuration << "'");
	BUTTON(eAds)->enabled = false;
	if (gameb4Ads==0)
		gameb4Ads=3;
    gameb4Ads--;
	storageAPI->setOption("gameB4Ads", ObjectSerializer<int>::object2string(gameb4Ads));
}

void AdsStateManager::LateExit() {
	LOGW("ici on cache pour de vrai");
	RENDERING(eAds)->show = false;
}
