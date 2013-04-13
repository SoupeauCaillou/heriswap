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
#include "LogoStateManager.h"

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include <glm/glm.hpp>

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

LogoStateManager::LogoStateManager(GameState _following) : following(_following) {}

void LogoStateManager::Setup() {
     logo = theEntityManager.CreateEntity();
     logobg = theEntityManager.CreateEntity();

    ADD_COMPONENT(logo, Rendering);
     ADD_COMPONENT(logo, Transformation);
     TRANSFORM(logo)->position = glm::vec2(0.f, 0.f);
     TRANSFORM(logo)->size = glm::vec2((float)PlacementHelper::GimpHeightToScreen(800),
     								   (float)PlacementHelper::GimpHeightToScreen(869));
     TRANSFORM(logo)->z = DL_Logo;
     RENDERING(logo)->texture = theRenderingSystem.loadTextureFile("soupe_logo");

     ADD_COMPONENT(logobg, Rendering);
     ADD_COMPONENT(logobg, Transformation);
     TRANSFORM(logobg)->position = glm::vec2(0.f, 0.f);
     TRANSFORM(logobg)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 
     									 (float)PlacementHelper::ScreenHeight);
     RENDERING(logobg)->color = Color(0,0,0);
     TRANSFORM(logobg)->z = DL_BehindLogo;
}

void LogoStateManager::Enter() {
	animLogo = theEntityManager.CreateEntity();
	ADD_COMPONENT(animLogo, Transformation);
	TRANSFORM(animLogo)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(49), 
										  (float)PlacementHelper::GimpWidthToScreen(40));
	TransformationSystem::setPosition(
		TRANSFORM(animLogo), 
		glm::vec2((float)PlacementHelper::GimpXToScreen(365), (float)(PlacementHelper::GimpYToScreen(331 + (1280-869)*0.5))), 
		TransformationSystem::NW);
	TRANSFORM(animLogo)->z = DL_LogoAnim;
	ADD_COMPONENT(animLogo, Rendering);
	RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo2_365_331");
	RENDERING(animLogo)->show = false;
	duration = 0;
	ADD_COMPONENT(animLogo, Sound);
    RENDERING(logo)->show = RENDERING(logobg)->show = true;
    // preload sound
    theSoundSystem.loadSoundFile("audio/son_monte.ogg");
    step = LogoStep1;
}

GameState LogoStateManager::Update(float dt) {
	duration += dt;
	
	if (duration > 1.75 && step == LogoStep5) {
		return following;
	} else if (duration > 0.8+0.35 && step == LogoStep4) {
		RENDERING(animLogo)->show = false;
		step = LogoStep5;
	} else if (duration > 0.8+0.3 && step == LogoStep3) {
		RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo2_365_331");
		step = LogoStep4;
	} else if (duration > 0.8+0.05 && step == LogoStep2) {
		RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo3_365_331");
		step = LogoStep3;
	} else if (duration > 0.8 && step == LogoStep1) {
		RENDERING(animLogo)->show = true;
		SOUND(animLogo)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg");
		step = LogoStep2;
	}
	return Logo;
}

void LogoStateManager::Exit() {

}

void LogoStateManager::LateExit() {
    // ou unloadLogo
    // theRenderingSystem.unloadAtlas("logo");
    /*
    theRenderingSystem.unloadTexture(RENDERING(logo)->texture);
    theRenderingSystem.unloadTexture(theRenderingSystem.loadTextureFile("soupe_logo2_365_331"));
    theRenderingSystem.unloadTexture(theRenderingSystem.loadTextureFile("soupe_logo3_365_331"));
    */
    theEntityManager.DeleteEntity(logo);
    theEntityManager.DeleteEntity(logobg);
	theEntityManager.DeleteEntity(animLogo);
}
