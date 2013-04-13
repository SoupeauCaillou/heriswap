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

#include <glm/glm.hpp>

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
	eRestart = theEntityManager.CreateEntity("eRestart");
	ADD_COMPONENT(eRestart, Transformation);
	ADD_COMPONENT(eRestart, TextRendering);
	ADD_COMPONENT(eRestart, Rendering);
	TRANSFORM(eRestart)->z = DL_PauseUIFg;
	TEXT_RENDERING(eRestart)->color = green;
	TEXT_RENDERING(eRestart)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	// TODO !
	// TEXT_RENDERING(eRestart)->text = localizeAPI->text("continue_", "Continue");
	TEXT_RENDERING(eRestart)->text = localizeAPI->text("continue_");
	TEXT_RENDERING(eRestart)->show = false;
	TEXT_RENDERING(eRestart)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eRestart)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(150),
											  (float)PlacementHelper::GimpYToScreen(300));
	//Restart button
	bRestart = theEntityManager.CreateEntity("bRestart");
	ADD_COMPONENT(bRestart, Transformation);
	ADD_COMPONENT(bRestart, Button);
	ADD_COMPONENT(bRestart, Rendering);
	ADD_COMPONENT(bRestart, Sound);
	RENDERING(bRestart)->color.a = .5;
	RENDERING(bRestart)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bRestart)->z = DL_PauseUIBg;
	TRANSFORM(bRestart)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708),
										  (float)PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bRestart)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(300));
	BUTTON(bRestart)->enabled = false;

	//Help Text
	eHelp = theEntityManager.CreateEntity("eHelp");
	ADD_COMPONENT(eHelp, Transformation);
	ADD_COMPONENT(eHelp, TextRendering);
	ADD_COMPONENT(eHelp, Rendering);
	TRANSFORM(eHelp)->z = DL_PauseUIFg;
	TEXT_RENDERING(eHelp)->color = green;
	TEXT_RENDERING(eHelp)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eHelp)->text = localizeAPI->text("help");
	TEXT_RENDERING(eHelp)->show = false;
	TEXT_RENDERING(eHelp)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eHelp)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(150),
										   (float)PlacementHelper::GimpYToScreen(500));
	//Help button
	bHelp = theEntityManager.CreateEntity("bHelp");
	ADD_COMPONENT(bHelp, Transformation);
	ADD_COMPONENT(bHelp, Button);
	ADD_COMPONENT(bHelp, Rendering);
	ADD_COMPONENT(bHelp, Sound);
	RENDERING(bHelp)->color.a = .5;
	RENDERING(bHelp)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bHelp)->z = DL_PauseUIBg;
	TRANSFORM(bHelp)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708),
									   (float)PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bHelp)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(500));
	BUTTON(bHelp)->enabled = false;

	//Abort text
	eAbort = theEntityManager.CreateEntity("eAbort");
	ADD_COMPONENT(eAbort, Transformation);
	ADD_COMPONENT(eAbort, TextRendering);
	TRANSFORM(eAbort)->z = DL_PauseUIFg;
	TEXT_RENDERING(eAbort)->color = green;
	TEXT_RENDERING(eAbort)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eAbort)->text = localizeAPI->text("give_up");
	TEXT_RENDERING(eAbort)->show = false;
	TEXT_RENDERING(eAbort)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eAbort)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(150),
											(float)PlacementHelper::GimpYToScreen(900));
	//Abort button
	bAbort = theEntityManager.CreateEntity("bAbort");
	ADD_COMPONENT(bAbort, Transformation);
	ADD_COMPONENT(bAbort, Button);
	ADD_COMPONENT(bAbort, Rendering);
	ADD_COMPONENT(bAbort, Sound);
	RENDERING(bAbort)->color.a = .5;
	RENDERING(bAbort)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	TRANSFORM(bAbort)->z = DL_PauseUIBg;
	TRANSFORM(bAbort)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708), 
										(float)PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bAbort)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(900));
	BUTTON(bAbort)->enabled = false;

}

void PauseStateManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
	theSoundSystem.loadSoundFile("audio/son_menu.ogg");
	TEXT_RENDERING(eRestart)->show = true;
	RENDERING(bRestart)->show = true;
	TEXT_RENDERING(eAbort)->show = true;
	RENDERING(bAbort)->show = true;
	TEXT_RENDERING(eHelp)->show = true;
	RENDERING(bHelp)->show = true;
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
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
	TEXT_RENDERING(eRestart)->show = false;
	RENDERING(bRestart)->show = false;
	TEXT_RENDERING(eAbort)->show = false;
	RENDERING(bAbort)->show = false;
	TEXT_RENDERING(eHelp)->show = false;
	RENDERING(bHelp)->show = false;
	BUTTON(bRestart)->enabled = false;
	BUTTON(bAbort)->enabled = false;
    theMusicSystem.toggleMute(theSoundSystem.mute);

	BUTTON(bHelp)->enabled = false;
}
