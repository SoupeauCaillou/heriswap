#include "ScoreBoardStateManager.h"
#include "Game.h"

ScoreBoardStateManager::ScoreBoardStateManager() {

}

void ScoreBoardStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	theTransformationSystem.Add(startbtn);
	theRenderingSystem.Add(startbtn);
	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("4.png");
	RENDERING(startbtn)->hide = true;
	RENDERING(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	theButtonSystem.Add(startbtn);
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,3);

}
	



void ScoreBoardStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
}

GameState ScoreBoardStateManager::Update(float dt) {
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return ScoreBoard;
}
	
void ScoreBoardStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(startbtn)->hide = true;	
	BUTTON(startbtn)->clicked = true;
}
