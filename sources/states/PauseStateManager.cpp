#include "PauseStateManager.h"

#include "Game.h"

PauseStateManager::PauseStateManager() {

}

void PauseStateManager::Setup() {		
	eRestart = theTextRenderingSystem.CreateLocalEntity(10);	
	eRestart2 = theEntityManager.CreateEntity();

	TRANSFORM(eRestart)->position = Vector2(3.5, 0);
		

	TEXT_RENDERING(eRestart)->text = "Reprendre";


	TEXT_RENDERING(eRestart)->hide = true;

	ADD_COMPONENT(eRestart, Button);
	ADD_COMPONENT(eRestart, Rendering);
	

}

void PauseStateManager::Enter() {
	
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eRestart)->hide = false;
	BUTTON(eRestart)->clicked = false;
}

GameState PauseStateManager::Update(float dt) {
	if (BUTTON(eRestart)->clicked)
		return Spawn;
	return Pause;
}

void PauseStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eRestart)->hide = true;
}



