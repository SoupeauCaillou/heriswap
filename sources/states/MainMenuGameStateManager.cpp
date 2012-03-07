#include "MainMenuGameStateManager.h"

MainMenuGameStateManager::MainMenuGameStateManager() {

}

MainMenuGameStateManager::~MainMenuGameStateManager() {
	theEntityManager.DeleteEntity(start);
	theEntityManager.DeleteEntity(score);
	theTextRenderingSystem.DestroyLocalEntity(eScore);
	theTextRenderingSystem.DestroyLocalEntity(eStart);
}

void MainMenuGameStateManager::Setup() {
	start = theEntityManager.CreateEntity();
	score = theEntityManager.CreateEntity();

	ADD_COMPONENT(start, Transformation);
	ADD_COMPONENT(start, Rendering);
	ADD_COMPONENT(start, Button);

	RENDERING(start)->texture = theRenderingSystem.loadTextureFile("1.png");
	RENDERING(start)->size = Game::CellSize() * Game::CellContentScale();
	RENDERING(start)->hide = true;
	BUTTON(start)->clicked = false;

	ADD_COMPONENT(score, Transformation);
	ADD_COMPONENT(score, Rendering);
	ADD_COMPONENT(score, Button);

	BUTTON(score)->clicked = false;
	RENDERING(score)->texture = theRenderingSystem.loadTextureFile("2.png");
	RENDERING(score)->size = Game::CellSize() * Game::CellContentScale();
	RENDERING(score)->hide = true;

	eStart = theTextRenderingSystem.CreateLocalEntity(5);
	eScore = theTextRenderingSystem.CreateLocalEntity(6);

	TRANSFORM(start)->position = Vector2(0,3);
	TRANSFORM(eStart)->position = TRANSFORM(start)->position + Vector2(3.5, 0);

	TRANSFORM(score)->position = Vector2(0,1);
	TRANSFORM(eScore)->position = TRANSFORM(score)->position + Vector2(3.5, 0);


	RENDERING(start)->hide = true;
	RENDERING(score)->hide = true;

	RENDERING(start)->color = Color(0,0,0,0);
	RENDERING(score)->color = Color(0,0,0,0);

	TRANSFORM(start)->rotation = 0;
	TRANSFORM(score)->rotation = 0;

}




void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	//Pour les rotations et autres animations
	elapsedTime = 0;

	TEXT_RENDERING(eStart)->text = "Play!";
	TEXT_RENDERING(eScore)->text = "Score";

	TEXT_RENDERING(eStart)->hide = false;
	TEXT_RENDERING(eScore)->hide = false;
	RENDERING(start)->hide = false;
	RENDERING(score)->hide = false;

	BUTTON(score)->clicked = false;
	BUTTON(start)->clicked = false;

}

GameState MainMenuGameStateManager::Update(float dt) {
	elapsedTime += dt/4.;
	if (BUTTON(start)->clicked){
		return MainMenuToBlackState;
	} else if (BUTTON(score)->clicked) {
		return ScoreBoard;
	}

	if (elapsedTime>1)
		elapsedTime = 1;

	RENDERING(start)->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);
	RENDERING(score)->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);

	TRANSFORM(start)->rotation += dt/elapsedTime;
	TRANSFORM(score)->rotation -= dt/elapsedTime;
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eStart)->hide = true;
	TEXT_RENDERING(eScore)->hide = true;
	RENDERING(start)->hide = true;
	RENDERING(score)->hide = true;
}
