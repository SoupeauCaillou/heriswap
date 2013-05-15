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

#include "base/PlacementHelper.h"

#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <sstream>

struct CountDownScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;

	// State variables
	Entity counter;
	Entity vorhang;
	float timeRemaining;

	CountDownScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
	    this->game = game;
	}

	void setup() {
		counter = theEntityManager.CreateEntity("counter");
		ADD_COMPONENT(counter, Transformation);
		ADD_COMPONENT(counter, TextRendering);
		TRANSFORM(counter)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(650));
		TEXT_RENDERING(counter)->color = Color(3.0/255.0, 99.0/255, 71.0/255);
		TEXT_RENDERING(counter)->fontName = "gdtypo";
		TEXT_RENDERING(counter)->positioning = TextRenderingComponent::CENTER;
		TEXT_RENDERING(counter)->charHeight = PlacementHelper::GimpHeightToScreen(300);
		TRANSFORM(counter)->z = DL_MainMenuFg;

		vorhang = theEntityManager.CreateEntity("vorhang");
		ADD_COMPONENT(vorhang, Rendering);
		ADD_COMPONENT(vorhang, Transformation);
		TRANSFORM(vorhang)->z = DL_MainMenuBg;
		TRANSFORM(vorhang)->size = glm::vec2( (float)PlacementHelper::GimpWidth, 
											  (float)PlacementHelper::GimpHeight);
		RENDERING(vorhang)->color = Color(0.f, 0.f, 0.f, 0.2f);
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
	}

	void onEnter(Scene::Enum) override {
		LOGI("'" << __PRETTY_FUNCTION__ << "'");

		if (game->datas->mode != Normal) {
			TEXT_RENDERING(counter)->show = true;
			RENDERING(vorhang)->show = true;
		}
		timeRemaining = 3.f;
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float dt) override {
		if (game->datas->mode == Normal) {
			return Scene::UserInput;
		}

		timeRemaining -= dt;

		std::stringstream a;
		a << (int)timeRemaining+1;
		TEXT_RENDERING(counter)->text = a.str();

		if (timeRemaining <= 0.f) {
			return Scene::UserInput;
		}
		return Scene::CountDown;
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
		LOGI("'" << __PRETTY_FUNCTION__ << "'");

		TEXT_RENDERING(counter)->show = false;
		RENDERING(vorhang)->show = false;
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateCountDownSceneHandler(HeriswapGame* game) {
    	return new CountDownScene(game);
	}
}