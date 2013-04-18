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
#include "MainMenuStateManager.h"

#include <glm/glm.hpp>

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void MainMenuGameStateManager::Setup() {
	const Color green("green");

	//Creating text entities
	for (int i=0; i<3; i++) {
		std::stringstream a;
		a.str("");
		a << "eStart_" << i;
		eStart[i] = theEntityManager.CreateEntity(a.str());
		ADD_COMPONENT(eStart[i], Transformation);
		ADD_COMPONENT(eStart[i], TextRendering);

		TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eStart[i])->show = false;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(eStart[i])->color = green;
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		ADD_COMPONENT(eStart[i], Morphing);
	    TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
	    MORPHING(eStart[i])->elements.push_back(sizeMorph);
	    MORPHING(eStart[i])->timing = 0.2;

	    a.str("");
	    a << "bStart_" << i;
	    bStart[i] = theEntityManager.CreateEntity(a.str());
	    ADD_COMPONENT(bStart[i], Transformation);
	    TRANSFORM(bStart[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708), 
	    									   (float)PlacementHelper::GimpHeightToScreen(147));
	    TRANSFORM(bStart[i])->position.x = 0;
	    TRANSFORM(bStart[i])->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(bStart[i], Rendering);
	    RENDERING(bStart[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	    RENDERING(bStart[i])->color.a = 0.5;

		TRANSFORM(eStart[i])->position.x = (float)PlacementHelper::GimpXToScreen(75);
		TRANSFORM(eStart[i])->position.y = TRANSFORM(bStart[i])->position.y = (float)PlacementHelper::GimpYToScreen(156+i*183);
	}
	TEXT_RENDERING(eStart[0])->text = localizeAPI->text("mode_1");
	TEXT_RENDERING(eStart[1])->text = localizeAPI->text("mode_2");
	TEXT_RENDERING(eStart[2])->text = localizeAPI->text("mode_3");

	//Containers properties
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(54);
		float w = theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(eStart[i]));
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		glm::vec2 target = glm::vec2((float)(PlacementHelper::GimpXToScreen(700) - w) ,
									 (float)PlacementHelper::GimpYToScreen(100));
		TypedMorphElement<glm::vec2>* posMorph = new TypedMorphElement<glm::vec2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, target);
        MORPHING(eStart[i])->elements.push_back(posMorph);
		ADD_COMPONENT(bStart[i], Sound);
		ADD_COMPONENT(bStart[i], Button);
		BUTTON(bStart[i])->enabled = false;
	}

	//// TEMP TEST CODE
	//TEXT_RENDERING(eStart[0])->flags |= TextRenderingComponent::MultiLineBit;
	//TRANSFORM(eStart[0])->position.x = 0;
	//TRANSFORM(eStart[0])->size = TRANSFORM(bStart[0])->size;
	//TRANSFORM(eStart[0])->size.y *= 4;
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->flags |= TextRenderingComponent::AdjustHeightToFillWidthBit;
		TRANSFORM(eStart[i])->size = TRANSFORM(bStart[i])->size * 0.9f;
	}

	menubg = theEntityManager.CreateEntity("menubg");
	ADD_COMPONENT(menubg, Transformation);
	TRANSFORM(menubg)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 
										(float)PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menubg), 
									  glm::vec2(0.f, 
									  		    (float)PlacementHelper::GimpYToScreen(542)), 
									  TransformationSystem::N);
	TRANSFORM(menubg)->z = DL_MainMenuBg;
	ADD_COMPONENT(menubg, Rendering);
	RENDERING(menubg)->texture = theRenderingSystem.loadTextureFile("2emeplan");
	RENDERING(menubg)->show = false;
	// TODO !
	RENDERING(menubg)->opaqueType = RenderingComponent::NON_OPAQUE;
	// RENDERING(menubg)->opaqueSeparation = 0.26;

	menufg = theEntityManager.CreateEntity("menufg");
	ADD_COMPONENT(menufg, Transformation);
	TRANSFORM(menufg)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 
										(float)PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menufg), 
									  glm::vec2(0.f, PlacementHelper::GimpYToScreen(1280)), 
									  TransformationSystem::S);
	TRANSFORM(menufg)->z = DL_MainMenuFg;
	ADD_COMPONENT(menufg, Rendering);
	RENDERING(menufg)->texture = theRenderingSystem.loadTextureFile("1erplan");
	RENDERING(menufg)->show = false;
	// TODO !
	RENDERING(menufg)->opaqueType = RenderingComponent::NON_OPAQUE;
	// RENDERING(menufg)->opaqueSeparation = (PlacementHelper::GimpHeightToScreen(1092) - PlacementHelper::GimpHeightToScreen(1280 - 570)) / TRANSFORM(menufg)->size.y;

	herisson = new AnimatedActor();
	herisson->frames=0;
	herisson->actor.speed = 4.1;
	Entity a = theEntityManager.CreateEntity("herisson_actor");
	ADD_COMPONENT(a, Transformation);
	TRANSFORM(a)->z = DL_MainMenuHerisson;
	ADD_COMPONENT(a, Rendering);
	herisson->actor.e = a;
	herisson->anim.clear();
	loadHerissonTexture(glm::round(glm::linearRand(1.f, 8.f)), herisson);
	herisson->actor.speed = glm::linearRand(2.0f, 4.0f);
	TRANSFORM(a)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), 
								   (float)(PlacementHelper::GimpHeightToScreen(253)) * glm::linearRand(.3f, 1.f));
	TransformationSystem::setPosition(TRANSFORM(a), 
									  glm::vec2((float)(PlacementHelper::GimpXToScreen(-glm::round(glm::linearRand(0.f, 300.f))) - TRANSFORM(a)->size.x), 
									  			(float)PlacementHelper::GimpYToScreen(glm::round(glm::linearRand(830.f,1150.f)))), 
									  TransformationSystem::SW);

	quitButton[0] = theEntityManager.CreateEntity("quitButton_0");
	ADD_COMPONENT(quitButton[0], Transformation);
	TRANSFORM(quitButton[0])->z = DL_MainMenuUITxt;
	TRANSFORM(quitButton[0])->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(1215));
	ADD_COMPONENT(quitButton[0], TextRendering);
	// TEXT_RENDERING(quitButton[0])->text = " " + localizeAPI->text("quit", "Exit") + " ";
	TEXT_RENDERING(quitButton[0])->text = " " + localizeAPI->text("quit") + " ";
	TEXT_RENDERING(quitButton[0])->show = false;
	TEXT_RENDERING(quitButton[0])->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(quitButton[0])->color = green;
	TEXT_RENDERING(quitButton[0])->charHeight = PlacementHelper::GimpHeightToScreen(60);

	quitButton[1] = theEntityManager.CreateEntity("quitButton_1");
	ADD_COMPONENT(quitButton[1], Transformation);
	float hhh = PlacementHelper::GimpHeightToScreen(95);
	float www = hhh / 0.209; //theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(quitButton[0]));
	TRANSFORM(quitButton[1])->size = glm::vec2(www, www * 0.209);
	TRANSFORM(quitButton[1])->position = glm::vec2(0, PlacementHelper::GimpYToScreen(1215));
	TRANSFORM(quitButton[1])->z = DL_MainMenuUIBg;
	ADD_COMPONENT(quitButton[1], Rendering);
	RENDERING(quitButton[1])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	RENDERING(quitButton[1])->color.a = 0.5;
	ADD_COMPONENT(quitButton[1], Button);

	modeTitleToReset = 0;
}

void MainMenuGameStateManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/son_menu.ogg");

	RENDERING(herisson->actor.e)->show = true;

	for (int i = 0; i < 3; i++) {
		RENDERING(bStart[i])->show = true;
		TEXT_RENDERING(eStart[i])->show = true;
		BUTTON(bStart[i])->enabled = true;
	}
	RENDERING(menubg)->show = true;
	RENDERING(menufg)->show = true;

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
		MORPHING(modeTitleToReset)->activationTime = 0;
		MORPHING(modeTitleToReset)->active = true;
	}
	TEXT_RENDERING(quitButton[0])->show = true;
	RENDERING(quitButton[1])->show = true;
	BUTTON(quitButton[1])->enabled = true;
	choosenGameMode = Normal;
}

GameState MainMenuGameStateManager::Update(float dt) {
	Entity a = herisson->actor.e;
	updateAnim(herisson, dt);
	if (TRANSFORM(a)->position.x < PlacementHelper::GimpXToScreen(800)+TRANSFORM(a)->size.x) {
		TRANSFORM(a)->position.x += herisson->actor.speed/8.*dt;
	} else {
		herisson->anim.clear();
		loadHerissonTexture(glm::round(glm::linearRand(1.f, 8.f)), herisson);//random texture
		herisson->actor.speed = glm::linearRand(2.0f,4.0f);//speed
		TRANSFORM(a)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), 
									   (float)PlacementHelper::GimpHeightToScreen(253))*glm::linearRand(.3f,1.f);//size
		TransformationSystem::setPosition(TRANSFORM(a), 
										  glm::vec2((float)(PlacementHelper::GimpXToScreen(-glm::round(glm::linearRand(0.f, 300.f)))-TRANSFORM(a)->size.x), 
										  			(float)PlacementHelper::GimpYToScreen(glm::round(glm::linearRand(830.f, 1150.f)))), 
										  TransformationSystem::SW);//offset
	}

	if (!modeTitleToReset || (modeTitleToReset && !MORPHING(modeTitleToReset)->active)) {
		if (BUTTON(bStart[0])->clicked) {
			choosenGameMode = Normal;
			SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		}
		if(BUTTON(bStart[1])->clicked){
			choosenGameMode = TilesAttack;
			SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		}
		if(BUTTON(bStart[2])->clicked){
			choosenGameMode = TilesAttack;
			choosenGameMode = Go100Seconds;
			SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		}
		if (BUTTON(quitButton[1])->clicked) {
			return ExitState;
		}
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");

	for (int i = 0; i < 3; i++) {
		if (i!=choosenGameMode) TEXT_RENDERING(eStart[i])->show = false;
		RENDERING(bStart[i])->show = false;
		BUTTON(bStart[i])->enabled = false;
	}

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
	}
    MORPHING(eStart[choosenGameMode])->active = true;
    modeTitleToReset = eStart[choosenGameMode];

    herisson->actor.speed = 4.5f;

	TEXT_RENDERING(quitButton[0])->show = false;
	RENDERING(quitButton[1])->show = false;
	BUTTON(quitButton[1])->enabled = false;
}
