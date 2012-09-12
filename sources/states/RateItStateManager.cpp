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
#include "RateItStateManager.h"

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void RateItStateManager::Setup() {
    //Creating text entities
    textToReadContainer = theEntityManager.CreateEntity();
    ADD_COMPONENT(textToReadContainer, Transformation);
    TRANSFORM(textToReadContainer)->size = Vector2(PlacementHelper::GimpWidthToScreen(712), PlacementHelper::GimpHeightToScreen(450));
    TRANSFORM(textToReadContainer)->z = DL_MainMenuUIBg;
    ADD_COMPONENT(textToReadContainer, Rendering);
    RENDERING(textToReadContainer)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
    RENDERING(textToReadContainer)->color.a = 0.5;
    TransformationSystem::setPosition(TRANSFORM(textToReadContainer), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(80)), TransformationSystem::NW);

    textToRead = theTextRenderingSystem.CreateEntity();
    TRANSFORM(textToRead)->z = DL_MainMenuUITxt;
    TRANSFORM(textToRead)->size = TRANSFORM(textToReadContainer)->size;
    TRANSFORM(textToRead)->size.X *= 0.9;
    TRANSFORM(textToRead)->size.Y = PlacementHelper::GimpHeightToScreen(147);
    TEXT_RENDERING(textToRead)->hide = true;
    TEXT_RENDERING(textToRead)->positioning = TextRenderingComponent::LEFT;
    TEXT_RENDERING(textToRead)->color = Color("green");
    TEXT_RENDERING(textToRead)->charHeight = PlacementHelper::GimpHeightToScreen(55);
    TEXT_RENDERING(textToRead)->text = localizeAPI->text("rate_it", "Si vous aimez Heriswap, pourriez vous prendre quelques secondes pour laisser une évaluation ou un commentaire ? Merci d\'avance !");
    TEXT_RENDERING(textToRead)->flags |= TextRenderingComponent::MultiLineBit;
    TransformationSystem::setPosition(TRANSFORM(textToRead), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(80)), TransformationSystem::NW);

    TRANSFORM(textToRead)->position.X = TRANSFORM(textToReadContainer)->position.X = 0;

    for (int i = 0; i < 3; i++) {
	boutonText[i] = theTextRenderingSystem.CreateEntity();

	TRANSFORM(boutonText[i])->z = DL_MainMenuUITxt;
	TEXT_RENDERING(boutonText[i])->hide = true;
	TEXT_RENDERING(boutonText[i])->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(boutonText[i])->color = Color("green");
	TEXT_RENDERING(boutonText[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);

	boutonContainer[i] = theEntityManager.CreateEntity();
	ADD_COMPONENT(boutonContainer[i], Transformation);
	TRANSFORM(boutonContainer[i])->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(boutonContainer[i])->z = DL_MainMenuUIBg;
	ADD_COMPONENT(boutonContainer[i], Rendering);
	RENDERING(boutonContainer[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	RENDERING(boutonContainer[i])->color.a = 0.5;
	ADD_COMPONENT(boutonContainer[i], Sound);
	ADD_COMPONENT(boutonContainer[i], Button);
	BUTTON(boutonContainer[i])->enabled = false;

	TRANSFORM(boutonText[i])->position.X = TRANSFORM(boutonContainer[i])->position.X = 0;
	TRANSFORM(boutonText[i])->position.Y = TRANSFORM(boutonContainer[i])->position.Y = PlacementHelper::GimpYToScreen(650 + i*183);
    }
    TEXT_RENDERING(boutonText[0])->text = localizeAPI->text("rate_now", "Right now");
    TEXT_RENDERING(boutonText[1])->text = localizeAPI->text("rate_later", "Later");
    TEXT_RENDERING(boutonText[2])->text = localizeAPI->text("rate_never", "No thanks");
}

void RateItStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(textToReadContainer)->hide = false;
	TEXT_RENDERING(textToRead)->hide = false;
	for (int i=0; i<3; i++) {
		RENDERING(boutonContainer[i])->hide = false;
		TEXT_RENDERING(boutonText[i])->hide = false;
		BUTTON(boutonContainer[i])->enabled = true;
	}

	RENDERING(menubg)->hide = false;
	RENDERING(menufg)->hide = false;
	//~RENDERING(fond)->hide = false;
}

GameState RateItStateManager::Update(float dt) {
	//want to rate
	if (BUTTON(boutonContainer[0])->clicked) {
		SOUND(boutonContainer[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		communicationAPI->rateItNow();
		return ModeMenu;
	//will rate later
	} else if(BUTTON(boutonContainer[1])->clicked){
		SOUND(boutonContainer[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		communicationAPI->rateItLater();
		return ModeMenu;
	//won't never rate
	} else if(BUTTON(boutonContainer[2])->clicked){
		SOUND(boutonContainer[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		communicationAPI->rateItNever();
		return ModeMenu;
	}
	return RateIt;
}

void RateItStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(textToReadContainer)->hide = true;
	TEXT_RENDERING(textToRead)->hide = true;
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(boutonText[i])->hide = true;
		RENDERING(boutonContainer[i])->hide = true;
		BUTTON(boutonContainer[i])->enabled = false;
	}

	RENDERING(menubg)->hide = true;
	RENDERING(menufg)->hide = true;
	//~RENDERING(fond)->hide = true;
}
