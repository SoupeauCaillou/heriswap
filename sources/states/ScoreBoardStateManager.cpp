#include "ScoreBoardStateManager.h"
#include "Game.h"
#include <fstream>
#include <sstream>
#include <string>

ScoreBoardStateManager::ScoreBoardStateManager(ScoreStorage* str): storage(str) {

}

void ScoreBoardStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	ADD_COMPONENT(startbtn, Transformation);
	ADD_COMPONENT(startbtn, Rendering);
	ADD_COMPONENT(startbtn, Button);

	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("4.png");
	RENDERING(startbtn)->hide = true;
	RENDERING(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,5);

	for (int i=0; i<10; i++) {
		eScore[i] = theTextRenderingSystem.CreateLocalEntity(40);
		TRANSFORM(eScore[i])->position = Vector2(-3, 3-i);
		TEXT_RENDERING(eScore[i])->charSize = 0.3;
		TEXT_RENDERING(eScore[i])->hide = true;
		TEXT_RENDERING(eScore[i])->alignL = true;
		TEXT_RENDERING(eScore[i])->color = Color(0.f,0.f,0.f);
	}	
}

void ScoreBoardStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
	
	std::vector<ScoreStorage::ScoreEntry> entries = storage->loadFromStorage();
	for (int i=0; i<10; i++) {
		TextRenderingComponent* trc = TEXT_RENDERING(eScore[i]);
		if (i < entries.size()) {
			trc->hide = false;
			std::stringstream a;
			a.precision(0);
			a << std::fixed << entries[i].points << ": " << entries[i].name;
			trc->text = a.str();
		} else {
			trc->hide = true;
		}
	}
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
	for (int i=0;i<10;i++) {
		TEXT_RENDERING(eScore[i])->hide = true;
	}
}