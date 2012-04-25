#include "LogoStateManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"
#include "systems/SoundSystem.h"

LogoStateManager::LogoStateManager(GameState _following, Entity _logo, Entity _logobg) : following(_following), logo(_logo), logobg(_logobg) {}

void LogoStateManager::Setup() {

}

void LogoStateManager::Enter() {
	animLogo = theEntityManager.CreateEntity();
	ADD_COMPONENT(animLogo, Transformation);
	TRANSFORM(animLogo)->size = Vector2(PlacementHelper::GimpWidthToScreen(49), PlacementHelper::GimpWidthToScreen(40));
	TransformationSystem::setPosition(
		TRANSFORM(animLogo), 
		Vector2(PlacementHelper::GimpXToScreen(365), PlacementHelper::GimpYToScreen(331 + (1280-869)*0.5)), 
		TransformationSystem::NW);
	TRANSFORM(animLogo)->z = DL_LogoAnim;
	ADD_COMPONENT(animLogo, Rendering);
	RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo2_365_331.png");
	RENDERING(animLogo)->hide = true;
	duration = 0;
	ADD_COMPONENT(animLogo, Sound);
	SOUND(animLogo)->type = SoundComponent::EFFECT;
    RENDERING(logo)->hide = RENDERING(logobg)->hide = false;
}

GameState LogoStateManager::Update(float dt) {
	duration += dt;
	
	if (duration > 1.75) {
		return following;
	} else if (duration > 0.8+0.35) {
		RENDERING(animLogo)->hide = true;
	} else if (duration > 0.8+0.3) {
		RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo2_365_331.png");
	} else if (duration > 0.8+0.05) {
		RENDERING(animLogo)->texture = theRenderingSystem.loadTextureFile("soupe_logo3_365_331.png");
	} else if (duration > 0.8) {
		RENDERING(animLogo)->hide = false;
		SOUND(animLogo)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg", false);
	}
	return Logo;
}

void LogoStateManager::Exit() {

}

void LogoStateManager::LateExit() {
    theEntityManager.DeleteEntity(logo);
    theEntityManager.DeleteEntity(logobg);
	theEntityManager.DeleteEntity(animLogo);
}