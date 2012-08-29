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

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void MainMenuGameStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<3; i++) {
		eStart[i] = theTextRenderingSystem.CreateEntity();

		TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(eStart[i])->color = green;
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		ADD_COMPONENT(eStart[i], Morphing);
	    TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
	    MORPHING(eStart[i])->elements.push_back(sizeMorph);
	    MORPHING(eStart[i])->timing = 0.2;

	    bStart[i] = theEntityManager.CreateEntity();
	    ADD_COMPONENT(bStart[i], Transformation);
	    TRANSFORM(bStart[i])->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	    TRANSFORM(bStart[i])->position.X = 0;
	    TRANSFORM(bStart[i])->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(bStart[i], Rendering);
	    RENDERING(bStart[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	    RENDERING(bStart[i])->color.a = 0.5;

		TRANSFORM(eStart[i])->position.X = PlacementHelper::GimpXToScreen(75);
		TRANSFORM(eStart[i])->position.Y = TRANSFORM(bStart[i])->position.Y = PlacementHelper::GimpYToScreen(156+i*183);
	}
	TEXT_RENDERING(eStart[0])->text = localizeAPI->text("mode_1", "Score race is a very long text we'll try to fit in a box");
	TEXT_RENDERING(eStart[1])->text = localizeAPI->text("mode_2", "Time attack");
	TEXT_RENDERING(eStart[2])->text = localizeAPI->text("mode_3", "RandomNameToBeChanged");


	//Containers properties
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(54);
		float w = theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(eStart[i]));
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		Vector2 target = Vector2(PlacementHelper::GimpXToScreen(700) - w ,PlacementHelper::GimpYToScreen(100));
		TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, target);
        MORPHING(eStart[i])->elements.push_back(posMorph);
		ADD_COMPONENT(bStart[i], Sound);
		ADD_COMPONENT(bStart[i], Button);
		BUTTON(bStart[i])->enabled = false;
	}

	//// TEMP TEST CODE
	//TEXT_RENDERING(eStart[0])->flags |= TextRenderingComponent::MultiLineBit;
	//TRANSFORM(eStart[0])->position.X = 0;
	//TRANSFORM(eStart[0])->size = TRANSFORM(bStart[0])->size;
	//TRANSFORM(eStart[0])->size.Y *= 4;

	//TEXT_RENDERING(eStart[2])->flags |= TextRenderingComponent::AdjustHeightToFillWidthBit;
	//TRANSFORM(eStart[2])->size = TRANSFORM(bStart[2])->size * 0.9;
	////

	menubg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menubg, Transformation);
	TRANSFORM(menubg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menubg), Vector2(0, PlacementHelper::GimpYToScreen(542)), TransformationSystem::N);
	TRANSFORM(menubg)->z = DL_MainMenuBg;
	ADD_COMPONENT(menubg, Rendering);
	RENDERING(menubg)->texture = theRenderingSystem.loadTextureFile("2emeplan");
	RENDERING(menubg)->hide = true;
	RENDERING(menubg)->opaqueType = RenderingComponent::OPAQUE_UNDER;
	RENDERING(menubg)->opaqueSeparation = 0.26;

	menufg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menufg, Transformation);
	TRANSFORM(menufg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menufg), Vector2(0, PlacementHelper::GimpYToScreen(1280)), TransformationSystem::S);
	TRANSFORM(menufg)->z = DL_MainMenuFg;
	ADD_COMPONENT(menufg, Rendering);
	RENDERING(menufg)->texture = theRenderingSystem.loadTextureFile("1erplan");
	RENDERING(menufg)->hide = true;
	RENDERING(menufg)->opaqueType = RenderingComponent::OPAQUE_UNDER;
	RENDERING(menufg)->opaqueSeparation = (PlacementHelper::GimpHeightToScreen(1092) - PlacementHelper::GimpHeightToScreen(1280 - 570)) / TRANSFORM(menufg)->size.Y;

	herisson = new AnimatedActor();
	herisson->frames=0;
	herisson->actor.speed = 4.1;
	Entity a = theEntityManager.CreateEntity();
	ADD_COMPONENT(a, Transformation);
	TRANSFORM(a)->z = DL_MainMenuHerisson;
	ADD_COMPONENT(a, Rendering);
	herisson->actor.e = a;
	herisson->anim.clear();
	loadHerissonTexture(MathUtil::RandomInt(8)+1, herisson);
	herisson->actor.speed = MathUtil::RandomFloatInRange(2.0f,4.0f);
	TRANSFORM(a)->size = Vector2(PlacementHelper::GimpWidthToScreen(310), PlacementHelper::GimpHeightToScreen(253))*MathUtil::RandomFloatInRange(.3f,1.f);
	TransformationSystem::setPosition(TRANSFORM(a), Vector2(PlacementHelper::GimpXToScreen(-MathUtil::RandomInt(300))-TRANSFORM(a)->size.X, PlacementHelper::GimpYToScreen(MathUtil::RandomIntInRange(830,1150))), TransformationSystem::SW);

	quitButton[0] = theEntityManager.CreateEntity();
	ADD_COMPONENT(quitButton[0], Transformation);
	TRANSFORM(quitButton[0])->z = DL_MainMenuUITxt;
	TRANSFORM(quitButton[0])->position = Vector2(0, PlacementHelper::GimpYToScreen(1215));
	ADD_COMPONENT(quitButton[0], TextRendering);
	TEXT_RENDERING(quitButton[0])->text = " " + localizeAPI->text("quitter", "Exit") + " ";
	TEXT_RENDERING(quitButton[0])->hide = true;
	TEXT_RENDERING(quitButton[0])->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(quitButton[0])->color = green;
	TEXT_RENDERING(quitButton[0])->charHeight = PlacementHelper::GimpHeightToScreen(60);

	quitButton[1] = theEntityManager.CreateEntity();
	ADD_COMPONENT(quitButton[1], Transformation);
	float hhh = PlacementHelper::GimpHeightToScreen(95);
	float www = hhh / 0.209; //theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(quitButton[0]));
	TRANSFORM(quitButton[1])->size = Vector2(www, www * 0.209);
	TRANSFORM(quitButton[1])->position = Vector2(0, PlacementHelper::GimpYToScreen(1215));
	TRANSFORM(quitButton[1])->z = DL_MainMenuUIBg;
	ADD_COMPONENT(quitButton[1], Rendering);
	RENDERING(quitButton[1])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	RENDERING(quitButton[1])->color.a = 0.5;
	ADD_COMPONENT(quitButton[1], Button);

	modeTitleToReset = 0;
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/son_menu.ogg");

	RENDERING(herisson->actor.e)->hide = false;

	for (int i = 0; i < 3; i++) {
		RENDERING(bStart[i])->hide = false;
		TEXT_RENDERING(eStart[i])->hide = false;
		BUTTON(bStart[i])->enabled = true;
	}
	RENDERING(menubg)->hide = false;
	RENDERING(menufg)->hide = false;

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
		MORPHING(modeTitleToReset)->activationTime = 0;
		MORPHING(modeTitleToReset)->active = true;
	}
	TEXT_RENDERING(quitButton[0])->hide = false;
	RENDERING(quitButton[1])->hide = false;
	BUTTON(quitButton[1])->enabled = true;
	choosenGameMode = Normal;
}

GameState MainMenuGameStateManager::Update(float dt) {
	Entity a = herisson->actor.e;
	updateAnim(herisson, dt);
	if (TRANSFORM(a)->position.X < PlacementHelper::GimpXToScreen(800)+TRANSFORM(a)->size.X) {
		TRANSFORM(a)->position.X += herisson->actor.speed/8.*dt;
	} else {
		herisson->anim.clear();
		loadHerissonTexture(MathUtil::RandomInt(8)+1, herisson);//random texture
		herisson->actor.speed = MathUtil::RandomFloatInRange(2.0f,4.0f);//speed
		TRANSFORM(a)->size = Vector2(PlacementHelper::GimpWidthToScreen(310), PlacementHelper::GimpHeightToScreen(253))*MathUtil::RandomFloatInRange(.3f,1.f);//size
		TransformationSystem::setPosition(TRANSFORM(a), Vector2(PlacementHelper::GimpXToScreen(-MathUtil::RandomInt(300))-TRANSFORM(a)->size.X, PlacementHelper::GimpYToScreen(MathUtil::RandomIntInRange(830,1150))), TransformationSystem::SW);//offset
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
			choosenGameMode = RandomNameToBeChanged;
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
	LOGI("%s", __PRETTY_FUNCTION__);

	for (int i = 0; i < 3; i++) {
		if (i!=choosenGameMode-1) TEXT_RENDERING(eStart[i])->hide = true;
		RENDERING(bStart[i])->hide = true;
		BUTTON(bStart[i])->enabled = false;
	}

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
	}
    MORPHING(eStart[choosenGameMode-1])->active = true;
    modeTitleToReset = eStart[choosenGameMode-1];

    herisson->actor.speed = 4.5f;

	TEXT_RENDERING(quitButton[0])->hide = true;
	RENDERING(quitButton[1])->hide = true;
	BUTTON(quitButton[1])->enabled = false;
}
