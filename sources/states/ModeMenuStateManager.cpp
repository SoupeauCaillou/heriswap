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
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<5; i++) {
		scoresName[i] = theTextRenderingSystem.CreateLocalEntity(2);
		scoresPoints[i] = theTextRenderingSystem.CreateLocalEntity(2);
		scoresLevel[i] = theTextRenderingSystem.CreateLocalEntity(2);
		TRANSFORM(scoresName[i])->z = TRANSFORM(scoresPoints[i])->z = TRANSFORM(scoresLevel[i])->z = DL_MainMenuUITxt;

		TRANSFORM(scoresName[i])->position.Y =
			TRANSFORM(scoresPoints[i])->position.Y =
				TRANSFORM(scoresLevel[i])->position.Y = PlacementHelper::GimpYToScreen(675 + i * 95);
		TRANSFORM(scoresName[i])->position.X = PlacementHelper::GimpXToScreen(92);
		TRANSFORM(scoresPoints[i])->position.X = PlacementHelper::GimpXToScreen(552);
		TRANSFORM(scoresLevel[i])->position.X = PlacementHelper::GimpXToScreen(620);

		TEXT_RENDERING(scoresName[i])->charHeight =
			TEXT_RENDERING(scoresPoints[i])->charHeight =
				TEXT_RENDERING(scoresLevel[i])->charHeight = PlacementHelper::GimpHeightToScreen(45);
		TEXT_RENDERING(scoresPoints[i])->isANumber = true;
		TEXT_RENDERING(scoresName[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scoresPoints[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(scoresLevel[i])->positioning = TextRenderingComponent::RIGHT;

		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresLevel[i])->hide = true;

		TEXT_RENDERING(scoresName[i])->color =
			TEXT_RENDERING(scoresPoints[i])->color =
				TEXT_RENDERING(scoresLevel[i])->color = green;
	}

	// play text
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	TRANSFORM(play)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(300));
	TRANSFORM(play)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(play, TextRendering);
	TEXT_RENDERING(play)->text = "Jouer";
	TEXT_RENDERING(play)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(play)->color = green;
	TEXT_RENDERING(play)->fontName = "typo";
	TEXT_RENDERING(play)->charHeight = PlacementHelper::GimpHeightToScreen(100);
	TEXT_RENDERING(play)->hide = true;

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(476));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = "Score";
	TEXT_RENDERING(scoreTitle)->fontName = "typo";
	TEXT_RENDERING(scoreTitle)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(scoreTitle)->color = green;
	TEXT_RENDERING(scoreTitle)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(scoreTitle)->hide = true;

	// play button
	playButton = theEntityManager.CreateEntity();
	ADD_COMPONENT(playButton, Transformation);
	ADD_COMPONENT(playButton, Container);
	CONTAINER(playButton)->entities.push_back(play);
	CONTAINER(playButton)->includeChildren = true;
	ADD_COMPONENT(playButton, Button);
	ADD_COMPONENT(playButton, Sound);
	SOUND(playButton)->type = SoundComponent::EFFECT;
	BUTTON(playButton)->enabled = false;

	yourScore = theTextRenderingSystem.CreateLocalEntity(2);
	TRANSFORM(yourScore)->z = DL_MainMenuUITxt;
	TRANSFORM(yourScore)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(800));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(yourScore)->charHeight = 0.3;
	TEXT_RENDERING(yourScore)->color = Color(0.f,0.f,0.f);

	// back button
	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = Vector2(PlacementHelper::GimpWidthToScreen(61), PlacementHelper::GimpHeightToScreen(37));
	TransformationSystem::setPosition(TRANSFORM(back), Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(82)), TransformationSystem::NW);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("menu/back.png");
	SOUND(back)->type = SoundComponent::EFFECT;
	BUTTON(back)->enabled = false;

	// openfeint button
	openfeint = theEntityManager.CreateEntity();
	ADD_COMPONENT(openfeint, Transformation);
	ADD_COMPONENT(openfeint, Rendering);
	ADD_COMPONENT(openfeint, Button);
	ADD_COMPONENT(openfeint, Sound);
	TRANSFORM(openfeint)->size = Vector2(PlacementHelper::GimpWidthToScreen(90), PlacementHelper::GimpHeightToScreen(91));
	TransformationSystem::setPosition(TRANSFORM(openfeint), Vector2(PlacementHelper::GimpXToScreen(605), PlacementHelper::GimpYToScreen(476)), TransformationSystem::NW);
	TRANSFORM(openfeint)->z = DL_MainMenuUITxt;
	RENDERING(openfeint)->texture = theRenderingSystem.loadTextureFile("menu/openfeint.png");
	SOUND(openfeint)->type = SoundComponent::EFFECT;
	BUTTON(openfeint)->enabled = false;

	playerName.clear();
	storage->getName(playerName);
}

void ModeMenuStateManager::LoadScore(int mode) {
	std::vector<ScoreStorage::Score> entries = storage->getScore(mode);
	bool alreadyGreen = false;
	for (int i=0; i<5; i++) {
		TextRenderingComponent* trcN = TEXT_RENDERING(scoresName[i]);
		TextRenderingComponent* trcP = TEXT_RENDERING(scoresPoints[i]);
		TextRenderingComponent* trcL = TEXT_RENDERING(scoresLevel[i]);
		if (i < entries.size()) {
			trcN->hide = false;
			trcP->hide = false;
			std::stringstream a;
			a.precision(1);
			if (mode==ScoreAttack) {
				a << std::fixed << entries[i].time << "s";
				trcP->isANumber = false;
			} else {
				a << std::fixed << entries[i].points;
				trcP->isANumber = true;
			}
			trcP->text = a.str();
			trcN->text = entries[i].name;
			trcL->text = entries[i].level;
			//affichage lvl
			if (mode==Normal) {
				trcL->hide = false;
			}

			if (!alreadyGreen && ended && ((entries[i].points == modeMgr->points && mode!=ScoreAttack) || (mode==ScoreAttack && entries[i].time-modeMgr->time<0.01f)) && entries[i].name == playerName) {
				// trcP->color = Color(0.f,1.f,0.f);
				// trcN->color = Color(0.f,1.f,0.f);
				alreadyGreen = true;
			} else {
				// trcP->color = Color(0.f,0.f,0.f);
				// trcN->color = Color(0.f,0.f,0.f);
			}
		} else {
			trcP->hide = true;
			trcN->hide = true;
			trcL->hide = true;
		}
	}
}


void ModeMenuStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	GameMode m = modeMgr->GetMode();
	BUTTON(back)->enabled = true;
	BUTTON(playButton)->enabled = true;
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
		if (modeMgr->GetMode()==Normal) {
			NormalGameModeManager* ng = static_cast<NormalGameModeManager*>(modeMgr);
			entry.level = ng->currentLevel();
		} else {
			entry.level = 1;
		}
		storage->submitScore(entry);
		TEXT_RENDERING(yourScore)->hide = false;
		std::stringstream a;
		a.precision(1);
		if (m==ScoreAttack) a << std::fixed << "Votre temps : " << entry.time << "s";
		else a << "Votre score : " << entry.points;
		TEXT_RENDERING(yourScore)->text = a.str();
	}
	LoadScore(m);

	TEXT_RENDERING(play)->hide = false;
	TEXT_RENDERING(scoreTitle)->hide = false;
	RENDERING(back)->hide = false;
	RENDERING(openfeint)->hide = false;
	TEXT_RENDERING(title)->hide = false;
	RENDERING(menubg)->hide = false;
	RENDERING(menufg)->hide = false;
}

GameState ModeMenuStateManager::Update(float dt) {
	if (BUTTON(playButton)->clicked) {
		SOUND(playButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
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
	// TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;
	RENDERING(openfeint)->hide = true;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresLevel[i])->hide = true;
	}
	RENDERING(menubg)->hide = true;
	RENDERING(menufg)->hide = true;
	ended = false;
	BUTTON(back)->enabled = false;
	BUTTON(playButton)->enabled = false;
	TEXT_RENDERING(scoreTitle)->hide = true;
	modeMgr->Reset();
}
