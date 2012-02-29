#include "MainMenuGameStateManager.h"

MainMenuGameStateManager::MainMenuGameStateManager() {

}

void MainMenuGameStateManager::Setup() {
	start = theEntityManager.CreateEntity();
	score = theEntityManager.CreateEntity();
	
	ADD_COMPONENT(start, Transformation);
	ADD_COMPONENT(start, Rendering);
	ADD_COMPONENT(start, Button);

	RENDERING(start)->texture = theRenderingSystem.loadTextureFile("1.png");
	RENDERING(start)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(start)->clicked = false;

	ADD_COMPONENT(score, Transformation);
	ADD_COMPONENT(score, Rendering);
	ADD_COMPONENT(score, Button);
	
	BUTTON(score)->clicked = false;
	RENDERING(score)->texture = theRenderingSystem.loadTextureFile("2.png");
	RENDERING(score)->size = Game::CellSize() * Game::CellContentScale();	
	
	eStart = theTextRenderingSystem.CreateLocalEntity(5);
	eScore = theTextRenderingSystem.CreateLocalEntity(6);
	
	TRANSFORM(start)->position = Vector2(0,3);
	TRANSFORM(eStart)->position = TRANSFORM(start)->position + Vector2(3.5, 0);
	
	TRANSFORM(score)->position = Vector2(0,1);
	TRANSFORM(eScore)->position = TRANSFORM(score)->position + Vector2(3.5, 0);
}
	



void MainMenuGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
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
	if (BUTTON(start)->clicked){
		return Spawn;
	} else if (BUTTON(score)->clicked) {
		return ScoreBoard;
	}
	
	TRANSFORM(start)->rotation += dt;
	TRANSFORM(score)->rotation -= dt;
	return MainMenu;
}
	
void MainMenuGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eStart)->hide = true;
	TEXT_RENDERING(eScore)->hide = true;
	RENDERING(start)->hide = true;
	RENDERING(score)->hide = true;
}
