#include "MainMenuGameStateManager.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"
#include <sstream>
#include "../DepthLayer.h"

MainMenuGameStateManager::MainMenuGameStateManager() {

}

MainMenuGameStateManager::~MainMenuGameStateManager() {
	theEntityManager.DeleteEntity(score);
	theTextRenderingSystem.DestroyLocalEntity(eScore);
	for (int i=0; i<3; i++) {
		theEntityManager.DeleteEntity(start[i]);
		theTextRenderingSystem.DestroyLocalEntity(eStart[i]);
	}
}

void MainMenuGameStateManager::Setup() {
	//Creating Entities
	for(int i=0; i<3; i++)
		start[i] = theEntityManager.CreateEntity();
	score = theEntityManager.CreateEntity();

	//Adding components
	for(int i=0; i<3; i++) {
		ADD_COMPONENT(start[i], Transformation);
		ADD_COMPONENT(start[i], Rendering);
	}
	ADD_COMPONENT(score, Transformation);
	ADD_COMPONENT(score, Rendering);


	//Loading leaf texture
	for(int i=0; i<3; i++) {
		RENDERING(start[i])->texture = theRenderingSystem.loadTextureFile("feuilles.png");
		RENDERING(start[i])->bottomLeftUV = Vector2(i / 8.0, 0);
		RENDERING(start[i])->topRightUV = RENDERING(start[i])->bottomLeftUV + Vector2(1 / 8.0, 1);
		TRANSFORM(start[i])->size = Game::CellSize() * Game::CellContentScale();
		RENDERING(start[i])->hide = true;
	}
	RENDERING(score)->texture = theRenderingSystem.loadTextureFile("feuilles.png");
	RENDERING(score)->bottomLeftUV = Vector2(3/8.0, 0);
	RENDERING(score)->topRightUV = Vector2(4/8.0, 1);
	TRANSFORM(score)->size = Game::CellSize() * Game::CellContentScale();
	RENDERING(score)->hide = true;

	//Creating text containers
	for (int i=0; i<3; i++)
		eStart[i] = theTextRenderingSystem.CreateLocalEntity(7);
	eScore = theTextRenderingSystem.CreateLocalEntity(6);

	//Settings position and rotation on screen
	for (int i=0; i<3; i++) {
		TRANSFORM(start[i])->z = DL_MainMenuUI;
		TRANSFORM(start[i])->rotation = 0;
		TRANSFORM(start[i])->position = Vector2(-2,1 + i*2);
		TRANSFORM(eStart[i])->z = DL_MainMenuUI;
		TRANSFORM(eStart[i])->position = TRANSFORM(start[i])->position + Vector2(1.5, 0);
	}
	TRANSFORM(score)->z = DL_MainMenuUI;
	TRANSFORM(score)->rotation = 0;
	TRANSFORM(score)->position = Vector2(-2,-1);
	TRANSFORM(eScore)->z = DL_MainMenuUI;
	TRANSFORM(eScore)->position = TRANSFORM(score)->position + Vector2(1.5, 0);

	//Text settings
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->alignL = true;
		RENDERING(start[i])->color = Color(0,0,0,0);
	}
	TEXT_RENDERING(eScore)->hide = true;
	RENDERING(score)->color = Color(0,0,0,0);
	TEXT_RENDERING(eScore)->alignL = true;

	TEXT_RENDERING(eStart[0])->text = "Normal";
	TEXT_RENDERING(eStart[1])->text = "Score Atk";
	TEXT_RENDERING(eStart[2])->text = "Frozen Time";
	TEXT_RENDERING(eScore)->text = "Score";

	//Adding containers for clickable areas
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
	bScore = theEntityManager.CreateEntity();
	ADD_COMPONENT(bScore, Transformation);
	ADD_COMPONENT(bScore, Container);
	ADD_COMPONENT(bScore, Button);
	ADD_COMPONENT(bScore, Rendering);
	RENDERING(bScore)->color = Color(.0, 1.0, .0, .5);
	TRANSFORM(bScore)->z = DL_MainMenuUI;
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/click.wav", false);

	//Pour les rotations et autres animations
	elapsedTime = 0;

	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->hide = false;
		RENDERING(start[i])->hide = false;
		BUTTON(bStart[i])->clicked = false;
	}
	TEXT_RENDERING(eScore)->hide = false;
	RENDERING(score)->hide = false;
	BUTTON(bScore)->clicked = false;
	RENDERING(bScore)->hide = false;

	CONTAINER(bScore)->entities.push_back(score);
	CONTAINER(bScore)->entities.push_back(eScore);
	CONTAINER(bScore)->includeChildren = true;
	for (int i=0; i<3; i++) {
		CONTAINER(bStart[i])->entities.push_back(start[i]);
		CONTAINER(bStart[i])->entities.push_back(eStart[i]);
		CONTAINER(bStart[i])->includeChildren = true;
		RENDERING(bStart[i])->hide = false;
	}
}

GameState MainMenuGameStateManager::Update(float dt, GameModeManager* modeMng) {
	elapsedTime += dt/4.;
	if (BUTTON(bStart[0])->clicked) {
		choosenGameMode = Normal;
		SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return MainMenuToBlackState;
	} else if (BUTTON(bStart[1])->clicked){
		choosenGameMode = ScoreAttack;
		SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return MainMenuToBlackState;
	} else if (BUTTON(bStart[2])->clicked){
		choosenGameMode = StaticTime;
		SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return MainMenuToBlackState;
	} else if (BUTTON(bScore)->clicked) {
		return ScoreBoard;
	}

	if (elapsedTime>1)
		elapsedTime = 1;

	for (int i=0; i<3; i++) {
		RENDERING(start[i])->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);
		TRANSFORM(start[i])->rotation += dt/elapsedTime;
	}
	RENDERING(score)->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);
	TRANSFORM(score)->rotation -= dt/elapsedTime;
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(eStart[i])->hide = true;
		RENDERING(start[i])->hide = true;
		RENDERING(bStart[i])->hide = true;
		CONTAINER(bStart[i])->entities.clear();
	}

	TEXT_RENDERING(eScore)->hide = true;
	RENDERING(score)->hide = true;
	RENDERING(bScore)->hide = true;
	CONTAINER(bScore)->entities.clear();
}
