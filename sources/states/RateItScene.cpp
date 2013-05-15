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
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct RateItScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;

	// State variables
	Entity title, menufg, menubg;
	Entity textToRead, textToReadContainer;
	Entity boutonText[3], boutonContainer[3];

	RateItScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
	    this->game = game;
	}

	void setup() {
		//Creating text entities
	    textToReadContainer = theEntityManager.CreateEntity("textToReadContainer");
	    ADD_COMPONENT(textToReadContainer, Transformation);
	    TRANSFORM(textToReadContainer)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(712), 
	    												 (float)PlacementHelper::GimpHeightToScreen(500));
	    TRANSFORM(textToReadContainer)->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(textToReadContainer, Rendering);
	    RENDERING(textToReadContainer)->texture = theRenderingSystem.loadTextureFile("fond_menu_mode");
	    RENDERING(textToReadContainer)->color.a = 0.5;
	    TransformationSystem::setPosition(TRANSFORM(textToReadContainer), 
	    								  glm::vec2((float)PlacementHelper::GimpXToScreen(44), 
	    								  			(float)PlacementHelper::GimpYToScreen(40)), 
	    								  TransformationSystem::NW);

	    textToRead = theEntityManager.CreateEntity("textToRead");
	    ADD_COMPONENT(textToRead, Transformation);
	    ADD_COMPONENT(textToRead, TextRendering);
	    TRANSFORM(textToRead)->z = DL_MainMenuUITxt;
	    TRANSFORM(textToRead)->size = TRANSFORM(textToReadContainer)->size;
	    TRANSFORM(textToRead)->size.x *= 0.9;
	    TRANSFORM(textToRead)->size.y = PlacementHelper::GimpHeightToScreen(147);
	    TEXT_RENDERING(textToRead)->show = false;
	    TEXT_RENDERING(textToRead)->positioning = TextRenderingComponent::LEFT;
	    TEXT_RENDERING(textToRead)->color = Color("green");
	    TEXT_RENDERING(textToRead)->charHeight = PlacementHelper::GimpHeightToScreen(55);
	    TEXT_RENDERING(textToRead)->text = game->gameThreadContext->localizeAPI->text("please_rate_it");
	    TEXT_RENDERING(textToRead)->flags |= TextRenderingComponent::MultiLineBit;
	    TransformationSystem::setPosition(TRANSFORM(textToRead), 
	    								  glm::vec2((float)PlacementHelper::GimpXToScreen(44), 
	    								  			(float)PlacementHelper::GimpYToScreen(25)), 
	    								  TransformationSystem::NW);

	    TRANSFORM(textToRead)->position.x = TRANSFORM(textToReadContainer)->position.x = 0;

	    for (int i = 0; i < 3; i++) {
	    	std::stringstream a;
	    	a.str("");
	    	a << "boutonText_" << i;
			boutonText[i] = theEntityManager.CreateEntity(a.str());

			ADD_COMPONENT(boutonText[i], Transformation);
			ADD_COMPONENT(boutonText[i], TextRendering);

			TRANSFORM(boutonText[i])->z = DL_MainMenuUITxt;
			TEXT_RENDERING(boutonText[i])->show = false;
			TEXT_RENDERING(boutonText[i])->positioning = TextRenderingComponent::CENTER;
			TEXT_RENDERING(boutonText[i])->color = Color("green");
			TEXT_RENDERING(boutonText[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);

			a.str("");
	    	a << "boutonContainer_" << i;
			boutonContainer[i] = theEntityManager.CreateEntity(a.str());
			ADD_COMPONENT(boutonContainer[i], Transformation);
			TRANSFORM(boutonContainer[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708), 
															(float)PlacementHelper::GimpHeightToScreen(147));
			TRANSFORM(boutonContainer[i])->z = DL_MainMenuUIBg;
			ADD_COMPONENT(boutonContainer[i], Rendering);
			RENDERING(boutonContainer[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
			RENDERING(boutonContainer[i])->color.a = 0.5;
			ADD_COMPONENT(boutonContainer[i], Sound);
			ADD_COMPONENT(boutonContainer[i], Button);
			BUTTON(boutonContainer[i])->enabled = false;

			TRANSFORM(boutonText[i])->position.x = TRANSFORM(boutonContainer[i])->position.x = 0.f;
			TRANSFORM(boutonText[i])->position.y = TRANSFORM(boutonContainer[i])->position.y = (float)PlacementHelper::GimpYToScreen(650 + i*183);
	    }
	    TEXT_RENDERING(boutonText[0])->text = game->gameThreadContext->localizeAPI->text("rate_now");
	    TEXT_RENDERING(boutonText[1])->text = game->gameThreadContext->localizeAPI->text("rate_later");
	    TEXT_RENDERING(boutonText[2])->text = game->gameThreadContext->localizeAPI->text("rate_never");
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
	}

	void onEnter(Scene::Enum) override {
		LOGI(__PRETTY_FUNCTION__);
		RENDERING(textToReadContainer)->show = true;
		TEXT_RENDERING(textToRead)->show = true;
		for (int i=0; i<3; i++) {
			RENDERING(boutonContainer[i])->show = true;
			TEXT_RENDERING(boutonText[i])->show = true;
			BUTTON(boutonContainer[i])->enabled = true;
		}

		RENDERING(menubg)->show = true;
		RENDERING(menufg)->show = true;
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float) override {
		//want to rate
		if (BUTTON(boutonContainer[0])->clicked) {
			SOUND(boutonContainer[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			game->gameThreadContext->communicationAPI->rateItNow();
			return Scene::ModeMenu;
		//will rate later
		} else if(BUTTON(boutonContainer[1])->clicked){
			SOUND(boutonContainer[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			game->gameThreadContext->communicationAPI->rateItLater();
			return Scene::ModeMenu;
		//won't never rate
		} else if(BUTTON(boutonContainer[2])->clicked){
			SOUND(boutonContainer[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			game->gameThreadContext->communicationAPI->rateItNever();
			return Scene::ModeMenu;
		}
		return Scene::RateIt;
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
		LOGI(__PRETTY_FUNCTION__);
		RENDERING(textToReadContainer)->show = false;
		TEXT_RENDERING(textToRead)->show = false;
		for (int i=0; i<3; i++) {
			TEXT_RENDERING(boutonText[i])->show = false;
			RENDERING(boutonContainer[i])->show = false;
			BUTTON(boutonContainer[i])->enabled = false;
		}

		RENDERING(menubg)->show = false;
		RENDERING(menufg)->show = false;
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateRateItSceneHandler(HeriswapGame* game) {
    	return new RateItScene(game);
	}
}