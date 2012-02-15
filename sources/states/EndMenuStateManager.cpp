#include "EndMenuStateManager.h"
#include "Game.h"

EndMenuStateManager::EndMenuStateManager() {

}

void EndMenuStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	
	theTransformationSystem.Add(startbtn);
	theRenderingSystem.Add(startbtn);
	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("5.png");
	RENDERING(startbtn)->hide = true;
	RENDERING(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	theButtonSystem.Add(startbtn);
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,3);
}
	



void EndMenuStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
		
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
}

GameState EndMenuStateManager::Update(float dt) {
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return EndMenu;
}
	
void EndMenuStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(startbtn)->hide = true;	
	BUTTON(startbtn)->clicked = true;
}

