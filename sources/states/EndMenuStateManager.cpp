#include "EndMenuStateManager.h"


EndMenuStateManager::EndMenuStateManager(ScoreStorage* str) : storage(str) {
	modeMng = 0;
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

	RENDERING(startbtn)->texture = theRenderingSystem.loadTextureFile("feuilles.png");
	RENDERING(startbtn)->hide = true;
	TRANSFORM(startbtn)->size = Game::CellSize() * Game::CellContentScale();
	BUTTON(startbtn)->clicked = false;
	TRANSFORM(startbtn)->position = Vector2(0,3);

	eScore = theTextRenderingSystem.CreateLocalEntity(5);
	TRANSFORM(eScore)->position = Vector2(1, 6);
	TEXT_RENDERING(eScore)->hide = true;

	eMsg = theTextRenderingSystem.CreateLocalEntity(28);
	TRANSFORM(eMsg)->position = Vector2(-4, 1);
	TEXT_RENDERING(eMsg)->charSize = 0.3;
	TEXT_RENDERING(eMsg)->hide = true;
	TEXT_RENDERING(eMsg)->alignL = true;
	TEXT_RENDERING(eMsg)->color = Color(0.f,0.f,0.f);

}

void EndMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	if (modeMng) {
	RENDERING(startbtn)->hide = false;
	BUTTON(startbtn)->clicked = false;

	std::vector<ScoreStorage::ScoreEntry> entries = storage->loadFromStorage();
	ScoreStorage::ScoreEntry entry;
	entry.points = modeMng->score;
	entry.name = "plop";
	entries.push_back(entry);
	std::sort(entries.begin(), entries.end(), ScoreStorage::ScoreEntryComp);
	if (entries.size() > 10) {
		entries.resize(10);
	}
	storage->saveToStorage(entries);

	{
	std::stringstream a;
	a.precision(0);
	if (mode == Normal || mode == StaticTime) a << std::fixed << modeMng->score;
	else a << (int)modeMng->time<<"s";
	TEXT_RENDERING(eScore)->text = a.str();
	TEXT_RENDERING(eScore)->hide = false;
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
}

GameState EndMenuStateManager::Update(float dt) {
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return EndMenu;
}

void EndMenuStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eMsg)->hide = true;
	RENDERING(startbtn)->hide = true;
	BUTTON(startbtn)->clicked = true;
	TEXT_RENDERING(eScore)->hide = true;

}
