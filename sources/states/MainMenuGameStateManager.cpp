#include "MainMenuGameStateManager.h"
#include "Game.h"

MainMenuGameStateManager::MainMenuGameStateManager() {

}

void MainMenuGameStateManager::Setup() {
	start = theEntityManager.CreateEntity();
	score = theEntityManager.CreateEntity();
	
	theTransformationSystem.Add(start);
	theRenderingSystem.Add(start);
	RENDERING(start)->texture = theRenderingSystem.loadTextureFile("1.png");
	RENDERING(start)->size = Game::CellSize() * Game::CellContentScale();
	
	theTransformationSystem.Add(score);

	theRenderingSystem.Add(score);
	RENDERING(score)->texture = theRenderingSystem.loadTextureFile("2.png");
	RENDERING(score)->size = Game::CellSize() * Game::CellContentScale();	
	
	int count = 13;
	std::map<char, Vector2> char2UV;
	
	char2UV['0'] = Vector2(0 / (float)count, 0);
	char2UV['1'] = Vector2(1 / (float)count, 0);
	char2UV['2'] = Vector2(2 / (float)count, 0);
	char2UV['3'] = Vector2(3 / (float)count, 0);
	char2UV['4'] = Vector2(4 / (float)count, 0);
	char2UV['5'] = Vector2(5 / (float)count, 0);
	char2UV['6'] = Vector2(6 / (float)count, 0);
	char2UV['7'] = Vector2(7 / (float)count, 0);
	char2UV['8'] = Vector2(8 / (float)count, 0);
	char2UV['9'] = Vector2(9 / (float)count, 0);
	char2UV[':'] = Vector2(10 / (float)count, 0);
	char2UV['.'] = Vector2(11 / (float)count, 0);
	char2UV['s'] = Vector2(12 / (float)count, 0);

	eStart = theEntityManager.CreateEntity();
	theTransformationSystem.Add(eStart);
	theTextRenderingSystem.Add(eStart);
	TEXT_RENDERING(eStart)->fontBitmap = theRenderingSystem.loadTextureFile("figures.png");
	TEXT_RENDERING(eStart)->uvSize = Vector2(1.0 / count, 1);
	TEXT_RENDERING(eStart)->charSize = Vector2(0.5, 1);
	TEXT_RENDERING(eStart)->char2UV = char2UV;
	for(int i=0; i<5; i++) {
		Entity e = theEntityManager.CreateEntity();
		theTransformationSystem.Add(e);
		TRANSFORM(e)->parent = eStart;
		theRenderingSystem.Add(e);
		TEXT_RENDERING(eStart)->drawing.push_back(e);
	}	
	
	eScore = theEntityManager.CreateEntity();
	theTransformationSystem.Add(eScore);
	theTextRenderingSystem.Add(eScore);
	TEXT_RENDERING(eScore)->fontBitmap = theRenderingSystem.loadTextureFile("figures.png");
	TEXT_RENDERING(eScore)->uvSize = Vector2(1.0 / count, 1);
	TEXT_RENDERING(eScore)->charSize = Vector2(0.5, 1);
	TEXT_RENDERING(eScore)->char2UV = char2UV;
	for(int i=0; i<6; i++) {
		Entity e = theEntityManager.CreateEntity();
		theTransformationSystem.Add(e);
		TRANSFORM(e)->parent = eScore;
		theRenderingSystem.Add(e);
		TEXT_RENDERING(eScore)->drawing.push_back(e);
	}
	
			
	
	TRANSFORM(start)->position = Vector2(0,3);
	TRANSFORM(eStart)->position = Vector2(6, 3);
	
	TRANSFORM(score)->position = Vector2(0,1);
	TRANSFORM(eScore)->position = Vector2(7, 1);
}
	
void MainMenuGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eStart)->text = "s2345";
	TEXT_RENDERING(eScore)->text = "1337 :";

	TEXT_RENDERING(eStart)->hide = false;
	TEXT_RENDERING(eScore)->hide = false;
	RENDERING(start)->hide = false;
	RENDERING(score)->hide = false;
}

GameState MainMenuGameStateManager::Update(float dt) {
	if (theTouchInputManager.isTouched()){
		return Spawn;
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

