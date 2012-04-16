#include "EndMenuStateManager.h"
#include <iostream>

EndMenuStateManager::EndMenuStateManager(ScoreStorage* str, PlayerNameInputUI* ui) : storage(str), inputUI(ui) {
	modeMgr=0;
}

EndMenuStateManager::~EndMenuStateManager(){
	theEntityManager.DeleteEntity(startbtn);
	theTextRenderingSystem.DestroyLocalEntity(eScore);
	theTextRenderingSystem.DestroyLocalEntity(eMsg);
}

void EndMenuStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();

	ADD_COMPONENT(startbtn, Transformation);
	ADD_COMPONENT(startbtn, Rendering);
	ADD_COMPONENT(startbtn, Button);

	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(6, 0));
	RENDERING(startbtn)->hide = true;
	TRANSFORM(startbtn)->z = DL_MainMenuUI;
	TRANSFORM(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,5);

	eScore = theTextRenderingSystem.CreateLocalEntity(5);
	TRANSFORM(eScore)->position = Vector2(1, 6);
	TEXT_RENDERING(eScore)->hide = true;

	/*eMsg = theTextRenderingSystem.CreateLocalEntity(28);
	TRANSFORM(eMsg)->position = Vector2(-4, 1);
	TEXT_RENDERING(eMsg)->charSize = 0.3;
	TEXT_RENDERING(eMsg)->hide = true;
	TEXT_RENDERING(eMsg)->alignL = true;
	TEXT_RENDERING(eMsg)->color = Color(0.f,0.f,0.f);*/
}

void EndMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(startbtn)->hide = true;
	BUTTON(startbtn)->clicked = false;

	playerName.clear();
	if (!inputUI->nameNeeded()) {
		std::ifstream file( "player_name.txt" );
		file >> playerName;
	}
	/*
	{
	std::stringstream a;
	a.precision(0);
	if (place>0) {
		if (place>1) a << std::fixed << "Vous etes en "<<place<<"eme position";
		else  a << std::fixed << "Vous etes en "<<place<<"ere position";
	} else {
		a << std::fixed << "Noob. T'es meme pas 10eme";
	}
	TEXT_RENDERING(eMsg)->text = a.str();
	TEXT_RENDERING(eMsg)->hide = false;
	}
	*/

}

GameState EndMenuStateManager::Update(float dt) {
	if (playerName.length() == 0) {
		inputUI->show();
		if (inputUI->query(playerName))
			RENDERING(startbtn)->hide = false;
	}

	if (BUTTON(startbtn)->clicked) {
		ScoreStorage::Score entry;
		entry.points = modeMgr->points;
		entry.time = modeMgr->time;
		entry.name = playerName;
		entry.mode = (int)modeMgr->GetMode();

		storage->submitScore(entry);
		{
		std::stringstream a;
		a.precision(0);
		if (modeMgr->GetMode()==ScoreAttack)
			a << std::fixed << modeMgr->time << "s";
		else
			a << std::fixed << modeMgr->points;


		TEXT_RENDERING(eScore)->text = a.str();
		TEXT_RENDERING(eScore)->hide = false;
		}
		return MainMenu;
	}
	return EndMenu;
}

void EndMenuStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	//TEXT_RENDERING(eMsg)->hide = true;
	RENDERING(startbtn)->hide = true;
	BUTTON(startbtn)->clicked = true;
	TEXT_RENDERING(eScore)->hide = true;

}
