#include "MainMenuGameStateManager.h"

MainMenuGameStateManager::~MainMenuGameStateManager() {
	for (int i=0; i<3; i++) {
		theTextRenderingSystem.DestroyLocalEntity(eStart[i]);
		theEntityManager.DeleteEntity(bStart[i]);
	}
}

void MainMenuGameStateManager::Setup() {
	//Creating text entities
	for (int i=0; i<3; i++)
		eStart[i] = theTextRenderingSystem.CreateLocalEntity(7);
	//Settings
	for (int i=0; i<3; i++) {
		TRANSFORM(eStart[i])->z = DL_MainMenuUI;
		TRANSFORM(eStart[i])->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300))-Vector2(0,2*i);
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::LEFT;
	}
	TEXT_RENDERING(eStart[0])->text = "Normal";
	TEXT_RENDERING(eStart[1])->text = "Score Atk";
	TEXT_RENDERING(eStart[2])->text = "Frozen Time";

	//Adding containers
	for (int i=0; i<3; i++) {
		bStart[i] = theEntityManager.CreateEntity();
		ADD_COMPONENT(bStart[i], Transformation);
		ADD_COMPONENT(bStart[i], Container);
		ADD_COMPONENT(bStart[i], Sound);
		SOUND(bStart[i])->type = SoundComponent::EFFECT;
		ADD_COMPONENT(bStart[i], Button);
		ADD_COMPONENT(bStart[i], Rendering);
		RENDERING(bStart[i])->color = Color(1.0, .0, .0, .5);
		TRANSFORM(bStart[i])->z = DL_MainMenuUI;
	}
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/click.wav", false);

	for (int i=0; i<3; i++) {
		RENDERING(bStart[i])->hide = false;
		TEXT_RENDERING(eStart[i])->hide = false;
		BUTTON(bStart[i])->clicked = false;
		CONTAINER(bStart[i])->entities.push_back(eStart[i]);
		CONTAINER(bStart[i])->includeChildren = true;
	}
}

GameState MainMenuGameStateManager::Update(float dt) {
	if (BUTTON(bStart[0])->clicked) {
		choosenGameMode = Normal;
		SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	} else if (BUTTON(bStart[1])->clicked){
		choosenGameMode = ScoreAttack;
		SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	} else if (BUTTON(bStart[2])->clicked){
		choosenGameMode = StaticTime;
		SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->hide = true;
		RENDERING(bStart[i])->hide = true;
		CONTAINER(bStart[i])->entities.clear();
	}
}
