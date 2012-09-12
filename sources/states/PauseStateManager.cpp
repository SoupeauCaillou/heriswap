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
#include "PauseStateManager.h"

#include "base/EntityManager.h"
#include "base/PlacementHelper.h"

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"

#include "DepthLayer.h"


void PauseStateManager::Setup() {
	const Color green("green");

	//Restart Text
	eRestart = theTextRenderingSystem.CreateEntity();
	ADD_COMPONENT(eRestart, Rendering);
	TRANSFORM(eRestart)->z = DL_PauseUIFg;
	TEXT_RENDERING(eRestart)->color = green;
	TEXT_RENDERING(eRestart)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eRestart)->text = localizeAPI->text("continue_", "Continue");
	TEXT_RENDERING(eRestart)->hide = true;
	TEXT_RENDERING(eRestart)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eRestart)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(300));
	//Restart button
	bRestart = theEntityManager.CreateEntity();
	ADD_COMPONENT(bRestart, Transformation);
	ADD_COMPONENT(bRestart, Button);
	ADD_COMPONENT(bRestart, Rendering);
	ADD_COMPONENT(bRestart, Sound);
	RENDERING(bRestart)->color.a = .5;
	RENDERING(bRestart)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bRestart)->z = DL_PauseUIBg;
	TRANSFORM(bRestart)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bRestart)->position = Vector2(0, PlacementHelper::GimpYToScreen(300));
	BUTTON(bRestart)->enabled = false;

	//Help Text
	eHelp = theTextRenderingSystem.CreateEntity();
	ADD_COMPONENT(eHelp, Rendering);
	TRANSFORM(eHelp)->z = DL_PauseUIFg;
	TEXT_RENDERING(eHelp)->color = green;
	TEXT_RENDERING(eHelp)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eHelp)->text = localizeAPI->text("help", "Help");
	TEXT_RENDERING(eHelp)->hide = true;
	TEXT_RENDERING(eHelp)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eHelp)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(500));
	//Help button
	bHelp = theEntityManager.CreateEntity();
	ADD_COMPONENT(bHelp, Transformation);
	ADD_COMPONENT(bHelp, Button);
	ADD_COMPONENT(bHelp, Rendering);
	ADD_COMPONENT(bHelp, Sound);
	RENDERING(bHelp)->color.a = .5;
	RENDERING(bHelp)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bHelp)->z = DL_PauseUIBg;
	TRANSFORM(bHelp)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bHelp)->position = Vector2(0, PlacementHelper::GimpYToScreen(500));
	BUTTON(bHelp)->enabled = false;

	//Abort text
	eAbort = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eAbort)->z = DL_PauseUIFg;
	TEXT_RENDERING(eAbort)->color = green;
	TEXT_RENDERING(eAbort)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eAbort)->text = localizeAPI->text("give_up", "Quit");
	TEXT_RENDERING(eAbort)->hide = true;
	TEXT_RENDERING(eAbort)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eAbort)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(900));
	//Abort button
	bAbort = theEntityManager.CreateEntity();
	ADD_COMPONENT(bAbort, Transformation);
	ADD_COMPONENT(bAbort, Button);
	ADD_COMPONENT(bAbort, Rendering);
	ADD_COMPONENT(bAbort, Sound);
	RENDERING(bAbort)->color.a = .5;
	RENDERING(bAbort)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bAbort)->z = DL_PauseUIBg;
	TRANSFORM(bAbort)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bAbort)->position = Vector2(0, PlacementHelper::GimpYToScreen(900));
	BUTTON(bAbort)->enabled = false;

}

void PauseStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	theSoundSystem.loadSoundFile("audio/son_menu.ogg");
	TEXT_RENDERING(eRestart)->hide = false;
	RENDERING(bRestart)->hide = false;
	TEXT_RENDERING(eAbort)->hide = false;
	RENDERING(bAbort)->hide = false;
	TEXT_RENDERING(eHelp)->hide = false;
	RENDERING(bHelp)->hide = false;
	BUTTON(bRestart)->enabled = true;
	BUTTON(bAbort)->enabled = true;

    // theMusicSystem.toggleMute(true);
	BUTTON(bHelp)->enabled = true;
	
	helpMgr->oldState = Pause;
	helpMgr->mode = mode;
}

GameState PauseStateManager::Update(float dt __attribute__((unused))) {
	if (BUTTON(bAbort)->clicked) {
		SOUND(bAbort)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		return MainMenu;
	} if (BUTTON(bRestart)->clicked) {
		SOUND(bRestart)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		return Unpause;
	} if (BUTTON(bHelp)->clicked) {
		SOUND(bHelp)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		return Help;
	}
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	RENDERING(bRestart)->hide = true;
	TEXT_RENDERING(eAbort)->hide = true;
	RENDERING(bAbort)->hide = true;
	TEXT_RENDERING(eHelp)->hide = true;
	RENDERING(bHelp)->hide = true;
	BUTTON(bRestart)->enabled = false;
	BUTTON(bAbort)->enabled = false;
    theMusicSystem.toggleMute(theSoundSystem.mute);

	BUTTON(bHelp)->enabled = false;
}
