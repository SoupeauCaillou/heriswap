#include "ModeMenuStateManager.h"
#include "Callback.cpp"
#include <sstream>

std::string diffic(int difficulty) {
	std::stringstream s;
	s << "Difficulty : ";
	switch (difficulty) {
		case 0:
			s << "easy 5x5";
			break;
		case 1:
			s << "medium 8x8";
			break;
	}
	return s.str().c_str();
}

ModeMenuStateManager::ModeMenuStateManager(ScoreStorage* storag, PlayerNameInputUI* inputUII, SuccessManager* sMgr, LocalizeAPI* lAPI) {
	storage = storag;
	successMgr = sMgr;
	ended = false;
	inputUI = inputUII;
	localizeAPI = lAPI;
	difficulty = 1;
}

ModeMenuStateManager::~ModeMenuStateManager() {
	for (int i=0; i<5; i++) {
		theEntityManager.DeleteEntity(scoresName[i]);
		theEntityManager.DeleteEntity(scoresPoints[i]);
		theEntityManager.DeleteEntity(scoresLevel[i]);
	}
	theEntityManager.DeleteEntity(play);
	theEntityManager.DeleteEntity(back);
	theEntityManager.DeleteEntity(yourScore);
	theEntityManager.DeleteEntity(eDifficulty);
	theEntityManager.DeleteEntity(bDifficulty);
}

void ModeMenuStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<5; i++) {
		scoresName[i] = theTextRenderingSystem.CreateEntity();
		scoresPoints[i] = theTextRenderingSystem.CreateEntity();
		scoresLevel[i] = theTextRenderingSystem.CreateEntity();
		TRANSFORM(scoresName[i])->z = TRANSFORM(scoresPoints[i])->z = TRANSFORM(scoresLevel[i])->z = DL_MainMenuUITxt;

		TRANSFORM(scoresName[i])->position.Y =
			TRANSFORM(scoresPoints[i])->position.Y =
				TRANSFORM(scoresLevel[i])->position.Y = PlacementHelper::GimpYToScreen(675 + i * 95);
		TRANSFORM(scoresName[i])->position.X = PlacementHelper::GimpXToScreen(92);
		TRANSFORM(scoresPoints[i])->position.X = PlacementHelper::GimpXToScreen(552);
		TRANSFORM(scoresLevel[i])->position.X = PlacementHelper::GimpXToScreen(590);

		TEXT_RENDERING(scoresName[i])->charHeight =
			TEXT_RENDERING(scoresPoints[i])->charHeight =
				TEXT_RENDERING(scoresLevel[i])->charHeight = PlacementHelper::GimpHeightToScreen(45);
		TEXT_RENDERING(scoresPoints[i])->isANumber = true;
		TEXT_RENDERING(scoresName[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scoresPoints[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(scoresLevel[i])->positioning = TextRenderingComponent::LEFT;

		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresLevel[i])->hide = true;

		TEXT_RENDERING(scoresName[i])->color =
			TEXT_RENDERING(scoresPoints[i])->color =
				TEXT_RENDERING(scoresLevel[i])->color = green;
	}
	// back button
	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(75));
	TransformationSystem::setPosition(TRANSFORM(back), Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(82)), TransformationSystem::NW);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("menu/back.png");
	BUTTON(back)->enabled = false;

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(476));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("Score :");
	TEXT_RENDERING(scoreTitle)->fontName = "typo";
	TEXT_RENDERING(scoreTitle)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(scoreTitle)->color = green;
	TEXT_RENDERING(scoreTitle)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(scoreTitle)->hide = true;

	// play text
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	TRANSFORM(play)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(300));
	TRANSFORM(play)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(play, TextRendering);
	TEXT_RENDERING(play)->text = localizeAPI->text("Jouer");
	TEXT_RENDERING(play)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(play)->color = green;
	TEXT_RENDERING(play)->fontName = "typo";
	TEXT_RENDERING(play)->charHeight = PlacementHelper::GimpHeightToScreen(100);
	TEXT_RENDERING(play)->hide = true;
	// play button
	playButton = theEntityManager.CreateEntity();
	ADD_COMPONENT(playButton, Transformation);
	ADD_COMPONENT(playButton, Container);
	CONTAINER(playButton)->entities.push_back(play);
	CONTAINER(playButton)->includeChildren = true;
	ADD_COMPONENT(playButton, Button);
	ADD_COMPONENT(playButton, Sound);
	BUTTON(playButton)->enabled = false;

	//difficulty text
	eDifficulty = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eDifficulty)->z = DL_MainMenuUITxt;
	TRANSFORM(eDifficulty)->position = Vector2(PlacementHelper::GimpXToScreen(85),PlacementHelper::GimpYToScreen(400));
	TEXT_RENDERING(eDifficulty)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(eDifficulty)->hide = true;
	TEXT_RENDERING(eDifficulty)->charHeight = PlacementHelper::GimpHeightToScreen(45);
	TEXT_RENDERING(eDifficulty)->color = green;
	TEXT_RENDERING(eDifficulty)->text = localizeAPI->text(diffic(difficulty));
	//difficulty container
	bDifficulty = theEntityManager.CreateEntity();
	ADD_COMPONENT(bDifficulty, Transformation);
	ADD_COMPONENT(bDifficulty, Container);
	CONTAINER(bDifficulty)->entities.push_back(eDifficulty);
	CONTAINER(bDifficulty)->includeChildren = true;
	ADD_COMPONENT(bDifficulty, Button);
	ADD_COMPONENT(bDifficulty, Sound);
	BUTTON(bDifficulty)->enabled = false;

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
	BUTTON(openfeint)->enabled = false;

	// your score
	yourScore = theTextRenderingSystem.CreateEntity();
	TRANSFORM(yourScore)->z = DL_MainMenuUITxt;
	TRANSFORM(yourScore)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(1242));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(yourScore)->charHeight = PlacementHelper::GimpHeightToScreen(56);
	TEXT_RENDERING(yourScore)->color = green;

	// fond
	fond = theEntityManager.CreateEntity();
	ADD_COMPONENT(fond, Transformation);
	TRANSFORM(fond)->size = Vector2(PlacementHelper::GimpWidthToScreen(712), PlacementHelper::GimpHeightToScreen(1124));
	TRANSFORM(fond)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(fond), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(24)), TransformationSystem::NW);
	ADD_COMPONENT(fond, Rendering);
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("menu/fond_menu_mode.png");
	RENDERING(fond)->color.a = 0.5;
}




void ModeMenuStateManager::LoadScore(int mode, int dif) {
	/*getting scores*/
	std::vector<ScoreStorage::Score> entries;
	std::stringstream tmp;
	tmp << "select * from score where mode= "<< mode << " and difficulty=" << dif;
	if (mode==Normal)
		 tmp << " order by points desc limit 5";
	else
		tmp << " order by time asc limit 5";

	storage->request(tmp.str().c_str(), &entries, callbackScore);

	/* treatment*/
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
			if (mode==Normal) {
				a << std::fixed << entries[i].points;
				trcP->isANumber = true;
			} else {
				a << std::fixed << entries[i].time << " s";
				trcP->isANumber = false;
			}
			trcP->text = a.str();
			trcN->text = entries[i].name;

			a.str(""); a<< std::fixed <<localizeAPI->text("niv") << " " <<entries[i].level;
			trcL->text = a.str();
			//affichage lvl
			if (mode==Normal) {
				trcL->hide = false;
			}
			if (!alreadyGreen && ended &&
			 ((mode==Normal && entries[i].points == modeMgr->points)
			  || (mode==TilesAttack && MathUtil::Abs(entries[i].time-modeMgr->time)<0.01f))
			   && entries[i].name == playerName) {
				trcN->color = Color(1.0f,0.f,0.f);
				trcP->color = Color(1.0f,0.f,0.f);
				trcL->color = Color(1.0f,0.f,0.f);
				alreadyGreen = true;
			} else {
				trcN->color = trcP->color = trcL->color = Color(3.0/255.0, 99.0/255, 71.0/255);
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

	if (!successMgr->bHardScore)
		successMgr->sHardScore(storage);

	BUTTON(back)->enabled = true;
	BUTTON(playButton)->enabled = true;
	if (ended) {
		if (playerName.length() == 0) {
			inputUI->query(playerName);
		}
	}

	LoadScore(modeMgr->GetMode(), difficulty);


	TEXT_RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
	RENDERING(menubg)->hide = false;
	//~ TEXT_RENDERING(title)->hide = false;
	RENDERING(menufg)->hide = false;
	RENDERING(fond)->hide = false;
	TEXT_RENDERING(play)->text = ended ? localizeAPI->text("Rejouer") : localizeAPI->text("Jouer");
	RENDERING(openfeint)->hide = false;
	BUTTON(openfeint)->enabled = true;
	TEXT_RENDERING(scoreTitle)->hide = false;
	TEXT_RENDERING(eDifficulty)->hide=false;
	BUTTON(bDifficulty)->enabled = true;

	std::stringstream s;
}

GameState ModeMenuStateManager::Update(float dt) {
	if (ended) {
		GameMode m = modeMgr->GetMode();
		ScoreStorage::Score entry;
		entry.points = modeMgr->points;
		entry.time = modeMgr->time;
		entry.name = playerName;
		if (m==Normal) {
			NormalGameModeManager* ng = static_cast<NormalGameModeManager*>(modeMgr);
			entry.level = ng->currentLevel();
		} else {
			entry.level = 1;
		}
		storage->submitScore(entry, m, difficulty);
		TEXT_RENDERING(yourScore)->hide = false;
		std::stringstream a;
        a << playerName << " ... ";
		a.precision(1);
		if (m==Normal)
			a << entry.points << "... "<< localizeAPI->text("niv") << " " << entry.level;
		else
			a << std::fixed << entry.time << " s";

		TEXT_RENDERING(yourScore)->text = a.str();
		LoadScore(m, difficulty);
		ended = false;
	}

	//herisson
	Entity herissonActor=  herisson->actor.e;
	if (TRANSFORM(herissonActor)->position.X < PlacementHelper::ScreenWidth+TRANSFORM(herissonActor)->size.X) {
		TRANSFORM(herissonActor)->position.X += herisson->actor.speed*dt;
		switchAnim(herisson);
	} else {
		RENDERING(herissonActor)->hide = true;
	}

	//difficulty button
	if (BUTTON(bDifficulty)->clicked) {
		difficulty++;
		if (difficulty==2) difficulty=0;
		LoadScore(modeMgr->GetMode(), difficulty);
		TEXT_RENDERING(eDifficulty)->text = localizeAPI->text(diffic(difficulty));
	}

	if (BUTTON(playButton)->clicked) {
		SOUND(playButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		RENDERING(herisson->actor.e)->hide = true;
		TRANSFORM(herissonActor)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.X;
		TEXT_RENDERING(title)->hide = true;
		return ModeMenuToBlackState;
	} if (BUTTON(back)->clicked) {
		SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		return MainMenu;
	}
	if (BUTTON(openfeint)->clicked) {
		#ifdef ANDROID
		storage->openfeintLB(modeMgr->GetMode());
		#else
		LOGW("Openfeint clicked");
		#endif
	}
	return ModeMenu;
}

void ModeMenuStateManager::Exit() {
	switch (difficulty) {
		case 0:
			theGridSystem.GridSize = 5;
			theGridSystem.Types = 5;
			break;
		case 1:
			theGridSystem.GridSize = 8;
			theGridSystem.Types = 8;
			break;
		}
}

void ModeMenuStateManager::LateExit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(yourScore)->hide = true;
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
	RENDERING(fond)->hide = true;
	ended = false;
	BUTTON(back)->enabled = false;
	BUTTON(playButton)->enabled = false;
	TEXT_RENDERING(scoreTitle)->hide = true;
	RENDERING(herisson->actor.e)->hide = true;
	BUTTON(openfeint)->enabled = false;

	TEXT_RENDERING(eDifficulty)->hide=true;
	BUTTON(bDifficulty)->enabled = false;
}
