/*
This file is part of RecursiveRunner.

@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
@author Soupe au Caillou - Gautier Pelloux-Prayer

RecursiveRunner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

RecursiveRunner is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "DepthLayer.h"

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>

#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct FadeScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;
	FadingType::Enum type;
	float duration;
	Scene::Enum nextState;

	// State variables	
	Entity eFading;
	float timeout, accum;

	FadeScene(HeriswapGame* game, FadingType::Enum pType, float pDuration, Scene::Enum pNextState)://, float d, FadeType fade, Scene::Enum whoAmI, Scene::Enum whoIsNext)://, StateHandler* enter, StateHandler* exitD) : 
	StateHandler<Scene::Enum>(), type(pType), duration(pDuration), nextState(pNextState) {//, duration(d), fading(fade), iAm(whoAmI), heIs(whoIsNext), timeout(0) {//, enterDelegate(enter), exitDelegate(exitD){
	    this->game = game;
	}

	void setup() {
		eFading = theEntityManager.CreateEntity("eFading");
		ADD_COMPONENT(eFading, Transformation);
		ADD_COMPONENT(eFading, Rendering);
		TRANSFORM(eFading)->position = glm::vec2(0.f);
		TRANSFORM(eFading)->size = glm::vec2(10.f, 20.f);
		RENDERING(eFading)->show = false;
		RENDERING(eFading)->color = Color(0,0,0);
		TRANSFORM(eFading)->z = DL_Fading;

		ADD_COMPONENT(eFading, ADSR);
		ADSR(eFading)->idleValue = 0;
		ADSR(eFading)->attackValue = 1.0;
		ADSR(eFading)->attackTiming = duration;
		ADSR(eFading)->decayTiming = 0;
		ADSR(eFading)->sustainValue = 1.0;
		ADSR(eFading)->releaseTiming = .2;
		ADSR(eFading)->attackMode = Quadratic;
	}

	static void updateColor(Entity eFading, FadingType::Enum type) {
		float value = ADSR(eFading)->value;
		switch (type) {
			case FadingType::FadeIn:
				RENDERING(eFading)->color.a = 1 - value;
				break;
			case FadingType::FadeOut:
				RENDERING(eFading)->color.a = value;
				break;
		}
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
		LOGW("Fade type: '" << type << "'");
		RENDERING(eFading)->show = true;
		//update duration (can be changed)
		ADSR(eFading)->attackTiming = duration;
		ADSR(eFading)->active = true;

		accum = 0;
		updateColor(eFading, type);
	}

	// Return false, until fading (in or out) is finished
	bool updatePreEnter(Scene::Enum , float dt) override {
		updateColor(eFading, type);

		if (theTouchInputManager.isTouched() && !theTouchInputManager.wasTouched()) {
			return true;
		}
		if (ADSR(eFading)->value == ADSR(eFading)->sustainValue) {
			accum += dt;
			if (accum >= timeout) {
				return true;
			}
		}
		return false;
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float ) override {
		return nextState;	
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
		RENDERING(eFading)->show = false;
		ADSR(eFading)->active = false;
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateFadeSceneHandler(HeriswapGame* game, FadingType::Enum type, float duration, Scene::Enum nextState) {
    	return new FadeScene(game, type, duration, nextState);
	}
}