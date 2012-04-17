#include "ModeMenuStateManager.h"

//Menu 2: un titre, un bouton jouer, les 5 meilleurs scores du mode de jeu, un bouton openfeint, un bouton back, le score de la partie est affiché ici ?

ModeMenuStateManager::~ModeMenuStateManager() {
	for (int i=0; i<5; i++) {
		theEntityManager.DeleteEntity(scores[i]);
	}
	theEntityManager.DeleteEntity(title);
	theEntityManager.DeleteEntity(play);
	theEntityManager.DeleteEntity(back);
}

void ModeMenuStateManager::Setup() {
	//Creating text entities
	for (int i=0; i<5; i++) {
		scores[i] = theTextRenderingSystem.CreateLocalEntity(2);
		TRANSFORM(scores[i])->z = DL_MainMenuUI;
		TRANSFORM(scores[i])->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300))-Vector2(0,i);
		TEXT_RENDERING(scores[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scores[i])->text = "a";
		TEXT_RENDERING(scores[i])->hide = true;
	}
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	ADD_COMPONENT(play, Rendering);
	ADD_COMPONENT(play, Button);
	TRANSFORM(play)->size = Vector2(1,1);
	TRANSFORM(play)->position = Vector2(0,0);
	TRANSFORM(play)->z = DL_MainMenuUI;

	title = theTextRenderingSystem.CreateLocalEntity(2);
	TRANSFORM(title)->z = DL_MainMenuUI;
	TRANSFORM(title)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300));
	TEXT_RENDERING(title)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(title)->hide = true;
	TEXT_RENDERING(title)->text = "a";

	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	TRANSFORM(back)->size = Vector2(1,1);
	TRANSFORM(back)->position = Vector2(0,2);
	TRANSFORM(back)->z = DL_MainMenuUI;
}

void ModeMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	BUTTON(play)->clicked = false;
	TEXT_RENDERING(title)->hide = false;
	RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
}

GameState ModeMenuStateManager::Update(float dt) {
	if (BUTTON(play)->clicked) return ModeMenuToBlackState;
	return ModeMenu;
}

void ModeMenuStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(title)->hide = true;
	RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;

}
