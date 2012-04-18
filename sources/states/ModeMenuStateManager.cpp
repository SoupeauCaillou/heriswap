#include "ModeMenuStateManager.h"

//Menu 2: un titre, un bouton jouer, les 5 meilleurs scoresName du mode de jeu, un bouton openfeint, un bouton back, le score de la partie est affiché ici ?

ModeMenuStateManager::~ModeMenuStateManager() {
	for (int i=0; i<5; i++) {
		theEntityManager.DeleteEntity(scoresName[i]);
	}
	theEntityManager.DeleteEntity(title);
	theEntityManager.DeleteEntity(play);
	theEntityManager.DeleteEntity(back);
}

void ModeMenuStateManager::Setup() {
	//Creating text entities
	for (int i=0; i<5; i++) {
		scoresName[i] = theTextRenderingSystem.CreateLocalEntity(2);
		scoresPoints[i] = theTextRenderingSystem.CreateLocalEntity(2);
		TRANSFORM(scoresName[i])->z = DL_MainMenuUI;
		TRANSFORM(scoresPoints[i])->z = DL_MainMenuUI;
		TRANSFORM(scoresName[i])->position = Vector2(PlacementHelper::GimpXToScreen(350),PlacementHelper::GimpYToScreen(900))-Vector2(0,i/2.);
		TRANSFORM(scoresPoints[i])->position = Vector2(PlacementHelper::GimpXToScreen(300),PlacementHelper::GimpYToScreen(900))-Vector2(0,i/2.);
		TEXT_RENDERING(scoresName[i])->charSize = 0.3;
		TEXT_RENDERING(scoresPoints[i])->charSize = 0.3;
		TEXT_RENDERING(scoresPoints[i])->isANumber = true;
		TEXT_RENDERING(scoresName[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scoresPoints[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
	}
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	ADD_COMPONENT(play, Rendering);
	ADD_COMPONENT(play, Button);
	TRANSFORM(play)->size = Vector2(1,1);
	TRANSFORM(play)->position = Vector2(0,2);
	TRANSFORM(play)->z = DL_MainMenuUI;
	RENDERING(play)->color = Color(0.f,1.f,0.f);

	title = theTextRenderingSystem.CreateLocalEntity(2);
	TRANSFORM(title)->z = DL_MainMenuUI;
	TRANSFORM(title)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300));
	TEXT_RENDERING(title)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(title)->hide = true;

	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	TRANSFORM(back)->size = Vector2(1,1);
	TRANSFORM(back)->position = Vector2(0,0);
	TRANSFORM(back)->z = DL_MainMenuUI;
	RENDERING(back)->color = Color(1.f,0.f,0.f);
}
void ModeMenuStateManager::LoadScore(int mode) {
	std::vector<ScoreStorage::Score> entries = storage->getScore(mode);
	for (int i=0; i<5; i++) {
		TextRenderingComponent* trcN = TEXT_RENDERING(scoresName[i]);
		TextRenderingComponent* trcP = TEXT_RENDERING(scoresPoints[i]);
		if (i < entries.size()) {
			trcN->hide = false;
			trcP->hide = false;
			std::stringstream a;
			a.precision(0);
			if (mode==ScoreAttack) a << std::fixed << entries[i].time << "s";
			else a << std::fixed << entries[i].points;
			trcP->text = a.str();
			trcN->text = entries[i].name;
		} else {
			trcP->hide = true;
			trcN->hide = true;
		}
	}
}


void ModeMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	BUTTON(play)->clicked = false;
	BUTTON(back)->clicked = false;
	GameMode m = modeMgr->GetMode();
	LoadScore(m);
	switch (m) {
	case Normal :
		TEXT_RENDERING(title)->text = "Normal";
		break;
	case ScoreAttack :
		TEXT_RENDERING(title)->text = "Score Attack";
		break;
	case StaticTime :
		TEXT_RENDERING(title)->text = "Frozen time";
		break;
	default :
		LOGI("ce mode n'est pas connu");
		break;
	}
	TEXT_RENDERING(title)->hide = false;
	RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = false;
		TEXT_RENDERING(scoresPoints[i])->hide = false;
	}
}

GameState ModeMenuStateManager::Update(float dt) {
	if (BUTTON(play)->clicked) return ModeMenuToBlackState;
	if (BUTTON(back)->clicked) return MainMenu;
	return ModeMenu;
}

void ModeMenuStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(title)->hide = true;
	RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
	}
}
