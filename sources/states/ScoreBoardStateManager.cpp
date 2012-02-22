#include "ScoreBoardStateManager.h"
#include "Game.h"
#include <fstream>
#include <sstream>
#include <string>

ScoreBoardStateManager::ScoreBoardStateManager() {

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
	TRANSFORM(startbtn)->position = Vector2(0,7);

	for (int i=0; i<10; i++) {
		eScore[i] = theTextRenderingSystem.CreateLocalEntity(40);
		TRANSFORM(eScore[i])->position = Vector2(0, 6-i);
		TEXT_RENDERING(eScore[i])->charSize = 0.3;
		TEXT_RENDERING(eScore[i])->hide = true;
		TEXT_RENDERING(eScore[i])->alignL = true;
		TEXT_RENDERING(eScore[i])->color = Color(0.f,0.f,0.f);
	}	
}
	
void ScoreBoardStateManager::LoadScore() {
	FILE* file = fopen("scores.txt", "r");
	if (file) {
		char nom[10][30];
		int nombre[10], j, i=0;
		while(i<10 && fscanf(file, "%d%s\n", &(nombre[i]),nom[i])!=EOF) {
			i++;
		}
		j=i;
		fclose(file);
		
		for (i=0;i<j;i++) {
			{
			std::stringstream a;
			a.precision(0);
			a << std::fixed << nom[i] << " : " << nombre[i];
			TEXT_RENDERING(eScore[i])->text = a.str();	
			}
			TEXT_RENDERING(eScore[i])->hide = false;
		}
	} else printf("impossible de lire les scores\n");	
}


void ScoreBoardStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
}

GameState ScoreBoardStateManager::Update(float dt) {
	LoadScore();
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return ScoreBoard;
}
	
void ScoreBoardStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	RENDERING(startbtn)->hide = true;	
	BUTTON(startbtn)->clicked = true;
	for (int i=0;i<10;i++) {
		TEXT_RENDERING(eScore[i])->text = "";
		TEXT_RENDERING(eScore[i])->hide = true;
	}
}
