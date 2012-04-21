#include "ModeMenuStateManager.h"

//Menu 2: un titre, un bouton jouer, les 5 meilleurs scoresName du mode de jeu, un bouton openfeint, un bouton back, le score de la partie est affiché ici ?

ModeMenuStateManager::~ModeMenuStateManager() {
	for (int i=0; i<5; i++) {
		theEntityManager.DeleteEntity(scoresName[i]);
		theEntityManager.DeleteEntity(scoresPoints[i]);
	}
	theEntityManager.DeleteEntity(play);
	theEntityManager.DeleteEntity(back);
	theEntityManager.DeleteEntity(yourScore);
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
		TEXT_RENDERING(scoresName[i])->charHeight = 0.3;
		TEXT_RENDERING(scoresPoints[i])->charHeight = 0.3;
		TEXT_RENDERING(scoresPoints[i])->isANumber = true;
		TEXT_RENDERING(scoresName[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scoresPoints[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresName[i])->color = Color(0.f,0.f,0.f);
		TEXT_RENDERING(scoresPoints[i])->color = Color(0.f,0.f,0.f);
	}
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	ADD_COMPONENT(play, Rendering);
	ADD_COMPONENT(play, Button);
	ADD_COMPONENT(play, Sound);
	TRANSFORM(play)->size = Vector2(1,1);
	TRANSFORM(play)->position = Vector2(0,2);
	TRANSFORM(play)->z = DL_MainMenuUI;
	RENDERING(play)->color = Color(0.f,1.f,0.f);
	SOUND(play)->type = SoundComponent::EFFECT;
	BUTTON(play)->enabled = false;
	
	yourScore = theTextRenderingSystem.CreateLocalEntity(2);
	TRANSFORM(yourScore)->z = DL_MainMenuUI;
	TRANSFORM(yourScore)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(800));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(yourScore)->charHeight = 0.3;
	TEXT_RENDERING(yourScore)->color = Color(0.f,0.f,0.f);

	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = Vector2(1,1);
	TRANSFORM(back)->position = Vector2(0,0);
	TRANSFORM(back)->z = DL_MainMenuUI;
	RENDERING(back)->color = Color(1.f,0.f,0.f);
	SOUND(back)->type = SoundComponent::EFFECT;
	BUTTON(back)->enabled = false;

	playerName.clear();
	inputUI->getName(playerName);
}

void ModeMenuStateManager::LoadScore(int mode) {
	std::vector<ScoreStorage::Score> entries = storage->getScore(mode);
	bool alreadyGreen = false;
	for (int i=0; i<5; i++) {
		TextRenderingComponent* trcN = TEXT_RENDERING(scoresName[i]);
		TextRenderingComponent* trcP = TEXT_RENDERING(scoresPoints[i]);
		if (i < entries.size()) {
			trcN->hide = false;
			trcP->hide = false;
			std::stringstream a;
			a.precision(1);
			if (mode==ScoreAttack) {
				a << std::fixed << entries[i].time << "s";
				TEXT_RENDERING(scoresPoints[i])->isANumber = false;
			} else {
				a << std::fixed << entries[i].points;
				TEXT_RENDERING(scoresPoints[i])->isANumber = true;
			}
			trcP->text = a.str();
			trcN->text = entries[i].name;

			if (!alreadyGreen && ended && ((entries[i].points == modeMgr->points && mode!=ScoreAttack) || (mode==ScoreAttack && entries[i].time-modeMgr->time<0.01f)) && entries[i].name == playerName) {
				trcP->color = Color(0.f,1.f,0.f);
				trcN->color = Color(0.f,1.f,0.f);
				alreadyGreen = true;
			} else {
				trcP->color = Color(0.f,0.f,0.f);
				trcN->color = Color(0.f,0.f,0.f);			
			}
		} else {
			trcP->hide = true;
			trcN->hide = true;
		}
	}
}


void ModeMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	GameMode m = modeMgr->GetMode();
	BUTTON(back)->enabled = true;
	BUTTON(play)->enabled = true;
	if (ended) {
		if (playerName.length() == 0) {
			inputUI->show();
			inputUI->query(playerName);
		}
		ScoreStorage::Score entry;
		entry.points = modeMgr->points;
		entry.time = modeMgr->time;
		entry.name = playerName;
		entry.mode = (int)modeMgr->GetMode();
		storage->submitScore(entry);
		TEXT_RENDERING(yourScore)->hide = false;
		std::stringstream a;
		a.precision(1);
		if (m==ScoreAttack) a << std::fixed << "Votre temps : " << entry.time << "s";
		else a << "Votre score : " << entry.points;
		TEXT_RENDERING(yourScore)->text = a.str();
	}
	LoadScore(m);

	RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
}

GameState ModeMenuStateManager::Update(float dt) {
	if (BUTTON(play)->clicked) {
		SOUND(play)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		TEXT_RENDERING(title)->hide = true;
		return ModeMenuToBlackState;
	} if (BUTTON(back)->clicked) {
		SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return MainMenu;
	}
	return ModeMenu;
}

void ModeMenuStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(yourScore)->hide = true;
	RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
	}
	ended = false;
	BUTTON(back)->enabled = false;
	BUTTON(play)->enabled = false;

	modeMgr->Reset();
}
