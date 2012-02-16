#include "EndMenuStateManager.h"
#include "Game.h"
#include <sstream>

EndMenuStateManager::EndMenuStateManager() {

}

void EndMenuStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	
	ADD_COMPONENT(startbtn, Transformation);
	ADD_COMPONENT(startbtn, Rendering);
	ADD_COMPONENT(startbtn, Button);

	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("5.png");
	RENDERING(startbtn)->hide = true;
	RENDERING(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,3);
	
	eScore = theTextRenderingSystem.CreateLocalEntity(5);
	TRANSFORM(eScore)->position = Vector2(1, 5);
	


}
	



void EndMenuStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
		
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
	
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << thePlayerSystem.GetScore();
	TEXT_RENDERING(eScore)->text = a.str();	
	TEXT_RENDERING(eScore)->hide = false;	
	}
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
	TEXT_RENDERING(eScore)->hide = true;	

}

