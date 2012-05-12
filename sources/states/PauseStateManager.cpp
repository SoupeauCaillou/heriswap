#include "PauseStateManager.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"
#include "systems/MusicSystem.h"

PauseStateManager::PauseStateManager() {

}

PauseStateManager::~PauseStateManager() {
	theTextRenderingSystem.DestroyLocalEntity(eRestart);
	theTextRenderingSystem.DestroyLocalEntity(eAbort);
	theTextRenderingSystem.DestroyLocalEntity(bRestart);
	theTextRenderingSystem.DestroyLocalEntity(bAbort);
}

void PauseStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	eRestart = theTextRenderingSystem.CreateEntity();
	ADD_COMPONENT(eRestart, Rendering);
	TRANSFORM(eRestart)->z = DL_PauseUIFg;
	TEXT_RENDERING(eRestart)->color = green;
	TEXT_RENDERING(eRestart)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eRestart)->text = "Reprendre";
	TEXT_RENDERING(eRestart)->hide = true;
	TEXT_RENDERING(eRestart)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eRestart)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(300));

	eAbort = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eAbort)->z = DL_PauseUIFg;
	TEXT_RENDERING(eAbort)->color = green;
	TEXT_RENDERING(eAbort)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eAbort)->text = "Abandonner";
	TEXT_RENDERING(eAbort)->hide = true;
	TEXT_RENDERING(eAbort)->positioning = TextRenderingComponent::LEFT;
	TRANSFORM(eAbort)->position = Vector2(PlacementHelper::GimpXToScreen(150),PlacementHelper::GimpYToScreen(500));


	bRestart = theEntityManager.CreateEntity();
	ADD_COMPONENT(bRestart, Transformation);
	ADD_COMPONENT(bRestart, Button);
	ADD_COMPONENT(bRestart, Rendering);
	ADD_COMPONENT(bRestart, Sound);
	RENDERING(bRestart)->color.a = .5;
	RENDERING(bRestart)->texture = theRenderingSystem.loadTextureFile("menu/fond_bouton.png");
	TRANSFORM(bRestart)->z = DL_PauseUIBg;
	TRANSFORM(bRestart)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bRestart)->position = Vector2(0, PlacementHelper::GimpYToScreen(300));
	BUTTON(bRestart)->enabled = false;
	SOUND(bRestart)->type = SoundComponent::EFFECT;

	bAbort = theEntityManager.CreateEntity();
	ADD_COMPONENT(bAbort, Transformation);
	ADD_COMPONENT(bAbort, Button);
	ADD_COMPONENT(bAbort, Rendering);
	ADD_COMPONENT(bAbort, Sound);
	RENDERING(bAbort)->color.a = .5;
	RENDERING(bAbort)->texture = theRenderingSystem.loadTextureFile("menu/fond_bouton.png");
	TRANSFORM(bAbort)->z = DL_PauseUIBg;
	TRANSFORM(bAbort)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	TRANSFORM(bAbort)->position = Vector2(0, PlacementHelper::GimpYToScreen(500));
	BUTTON(bAbort)->enabled = false;
	SOUND(bAbort)->type = SoundComponent::EFFECT;
}

void PauseStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
	TEXT_RENDERING(eRestart)->hide = false;
	RENDERING(bRestart)->hide = false;
	TEXT_RENDERING(eAbort)->hide = false;
	RENDERING(bAbort)->hide = false;
	BUTTON(bRestart)->enabled = true;
	BUTTON(bAbort)->enabled = true;

    theMusicSystem.toggleMute(true);
}

GameState PauseStateManager::Update(float dt) {
	if (BUTTON(bAbort)->clicked) {
		SOUND(bAbort)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return MainMenu;
	} if (BUTTON(bRestart)->clicked) {
		SOUND(bRestart)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return Unpause;
	}
	return Pause;
}

void PauseStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eRestart)->hide = true;
	RENDERING(bRestart)->hide = true;
	TEXT_RENDERING(eAbort)->hide = true;
	RENDERING(bAbort)->hide = true;
	BUTTON(bRestart)->enabled = false;
	BUTTON(bAbort)->enabled = false;

    theMusicSystem.toggleMute(theSoundSystem.mute);
}
