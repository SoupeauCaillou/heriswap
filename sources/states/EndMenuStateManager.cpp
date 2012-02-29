#include "EndMenuStateManager.h"


EndMenuStateManager::EndMenuStateManager(ScoreStorage* str): storage(str) {

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
	TRANSFORM(eScore)->position = Vector2(1, 6);
	
	eMsg = theTextRenderingSystem.CreateLocalEntity(28);
	TRANSFORM(eMsg)->position = Vector2(-4, 1);
	TEXT_RENDERING(eMsg)->charSize = 0.3;
	TEXT_RENDERING(eMsg)->hide = true;
	TEXT_RENDERING(eMsg)->alignL = true;
	TEXT_RENDERING(eMsg)->color = Color(0.f,0.f,0.f);
}

void EndMenuStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
		
	RENDERING(startbtn)->hide = false;	
	BUTTON(startbtn)->clicked = false;
	
	TEXT_RENDERING(eMsg)->hide = false;

	std::vector<ScoreStorage::ScoreEntry> entries = storage->loadFromStorage();
	ScoreStorage::ScoreEntry entry;
	entry.points = thePlayerSystem.GetScore();
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
	a << std::fixed << thePlayerSystem.GetScore();
	TEXT_RENDERING(eScore)->text = a.str();	
	TEXT_RENDERING(eScore)->hide = false;	
	}
	#if 0
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
	#endif
	
}

GameState EndMenuStateManager::Update(float dt) {
	if (BUTTON(startbtn)->clicked)
		return MainMenu;
	return EndMenu;
}
	
void EndMenuStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	TEXT_RENDERING(eMsg)->hide = true;
	RENDERING(startbtn)->hide = true;	
	BUTTON(startbtn)->clicked = true;
	TEXT_RENDERING(eScore)->hide = true;	

}

