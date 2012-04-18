#include "ScoreBoardStateManager.h"
#include "../DepthLayer.h"

ScoreBoardStateManager::ScoreBoardStateManager(ScoreStorage* str): storage(str) {

}

ScoreBoardStateManager::~ScoreBoardStateManager() {
	theEntityManager.DeleteEntity(startbtn);
	for(int i=0; i<10; i++) {
		theTextRenderingSystem.DestroyLocalEntity(eScore[i]);
	}
}

void ScoreBoardStateManager::Setup() {
	startbtn = theEntityManager.CreateEntity();
	ADD_COMPONENT(startbtn, Transformation);
	ADD_COMPONENT(startbtn, Rendering);
	ADD_COMPONENT(startbtn, Button);
	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(5, 0));
	RENDERING(startbtn)->hide = true;
	TRANSFORM(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	TRANSFORM(startbtn)->position = Vector2(0,5);
	TRANSFORM(startbtn)->z = DL_Score;

	switchMode = theEntityManager.CreateEntity();
	ADD_COMPONENT(switchMode, Transformation);
	ADD_COMPONENT(switchMode, Rendering);
	ADD_COMPONENT(switchMode, Button);
	RENDERING(switchMode)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(3,0));
	TRANSFORM(switchMode)->size = Game::CellSize() * Game::CellContentScale();
	TRANSFORM(switchMode)->position = Vector2(4,5);
	TRANSFORM(switchMode)->z = DL_Score;

	for (int i=0; i<10; i++) {
		eScore[i] = theTextRenderingSystem.CreateLocalEntity(40);
		TRANSFORM(eScore[i])->position = Vector2(-3, 3-i);
		TRANSFORM(eScore[i])->z = DL_Score;
		TEXT_RENDERING(eScore[i])->charSize = 0.3;
		TEXT_RENDERING(eScore[i])->hide = true;
		TEXT_RENDERING(eScore[i])->color = Color(0.f,0.f,0.f);
	}
	mode = 1;
}
void ScoreBoardStateManager::LoadScore(int mode) {
	std::vector<ScoreStorage::Score> entries = storage->getScore(mode);
	for (int i=0; i<10; i++) {
		TextRenderingComponent* trc = TEXT_RENDERING(eScore[i]);
		if (i < entries.size()) {
			trc->hide = false;
			std::stringstream a;
			a.precision(0);
			if (mode==2)
				a << std::fixed << entries[i].time << "s : " << entries[i].name;
			else
				a << std::fixed << entries[i].points << ": " << entries[i].name;
			trc->text = a.str();
		} else {
			trc->hide = true;
		}
	}
}

void ScoreBoardStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(startbtn)->hide = false;
	BUTTON(startbtn)->clicked = false;
	RENDERING(switchMode)->hide = false;
	BUTTON(switchMode)->clicked = false;
}

GameState ScoreBoardStateManager::Update(float dt) {

	if (BUTTON(switchMode)->clicked) {
		BUTTON(switchMode)->clicked=false;
		mode++;
		if (mode==4) mode = 1;
		LoadScore(mode);
		RENDERING(switchMode)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(mode,0));
	}
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return ScoreBoard;
}

void ScoreBoardStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	RENDERING(startbtn)->hide = true;
	BUTTON(startbtn)->clicked = true;
	RENDERING(switchMode)->hide = true;
	BUTTON(switchMode)->clicked = true;
	for (int i=0;i<10;i++) {
		TEXT_RENDERING(eScore[i])->hide = true;
	}
}
