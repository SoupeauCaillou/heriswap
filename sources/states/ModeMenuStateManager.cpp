#include "ModeMenuStateManager.h"
#include "CallBack.cpp"

ModeMenuStateManager::ModeMenuStateManager(ScoreStorage* storag, PlayerNameInputUI* inputUII, SuccessAPI* successAP, LocalizeAPI* lAPI) {
	storage = storag;
	ended = false;
	inputUI = inputUII;
	successAPI = successAP;
	localizeAPI = lAPI;
	perso = false;
	numberTiles = 2;
	gridSize = 3;
}

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

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(476));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("Score");
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

	// your score
	yourScore = theTextRenderingSystem.CreateEntity();
	TRANSFORM(yourScore)->z = DL_MainMenuUITxt;
	TRANSFORM(yourScore)->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(1242));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(yourScore)->charHeight = PlacementHelper::GimpHeightToScreen(56);
	TEXT_RENDERING(yourScore)->color = green;

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

	// fond
	fond = theEntityManager.CreateEntity();
	ADD_COMPONENT(fond, Transformation);
	TRANSFORM(fond)->size = Vector2(PlacementHelper::GimpWidthToScreen(712), PlacementHelper::GimpHeightToScreen(1124));
	TRANSFORM(fond)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(fond), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(24)), TransformationSystem::NW);
	ADD_COMPONENT(fond, Rendering);
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("menu/fond_menu_mode.png");
	RENDERING(fond)->color.a = 0.5;

	//success test
	int sav=0;
	storage->request("select points from score", &sav, callbackSc);
	if (sav > 1000000) {
		successAPI->successCompleted("Hardscore gamer", 1653102);
	}


	//perso game buttons
	for (int i=0; i<2; i++) {
		persoTweakCont[i] = theEntityManager.CreateEntity();
		ADD_COMPONENT(persoTweakCont[i], Transformation);
		ADD_COMPONENT(persoTweakCont[i], Button);
		ADD_COMPONENT(persoTweakCont[i], Rendering);
		ADD_COMPONENT(persoTweakCont[i], Sound);
		SOUND(persoTweakCont[i])->type = SoundComponent::EFFECT;
		BUTTON(persoTweakCont[i])->enabled = false;
		TRANSFORM(persoTweakCont[i])->z = DL_MainMenuUITxt;
		TRANSFORM(persoTweakCont[i])->size = Vector2(2,.5);
		RENDERING(persoTweakCont[i])->color= Color(1.0f,0.f,0.f);
		persoTweakText[i] = theTextRenderingSystem.CreateEntity();
		TRANSFORM(persoTweakText[i])->z = DL_MainMenuUITxt;

		TRANSFORM(persoTweakText[i])->position.X = PlacementHelper::GimpXToScreen(350);
		TRANSFORM(persoTweakCont[i])->position.X = PlacementHelper::GimpXToScreen(200);
		TRANSFORM(persoTweakText[i])->position.Y = PlacementHelper::GimpYToScreen(675 + i * 95);
		TRANSFORM(persoTweakCont[i])->position.Y = PlacementHelper::GimpYToScreen(675 + i * 95);

		TEXT_RENDERING(persoTweakText[i])->charHeight = PlacementHelper::GimpHeightToScreen(45);
		TEXT_RENDERING(persoTweakText[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(persoTweakText[i])->hide = true;
		TEXT_RENDERING(persoTweakText[i])->color = green;
	}
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
			if (mode==ScoreAttack || mode==TilesAttack) {
				a << std::fixed << entries[i].time << " s";
				trcP->isANumber = false;
			} else {
				a << std::fixed << entries[i].points;
				trcP->isANumber = true;
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
			 ((entries[i].points == modeMgr->points && mode!=ScoreAttack && mode!=TilesAttack)
			  || ((mode==TilesAttack || mode==ScoreAttack) && entries[i].time-modeMgr->time<0.01f))
			   && entries[i].name == playerName) {
				alreadyGreen = true;
			} else {
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
			inputUI->query(playerName);
		}
	}

	if (!perso) {
		numberTiles = gridSize =8;
		LoadScore(m);
	}

	TEXT_RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
	RENDERING(menubg)->hide = false;
	//~ TEXT_RENDERING(title)->hide = false;
	RENDERING(menufg)->hide = false;
	RENDERING(fond)->hide = false;
	TEXT_RENDERING(play)->text = ended ? localizeAPI->text("Rejouer") : localizeAPI->text("Jouer");
	if (!perso) {
		RENDERING(openfeint)->hide = false;
		BUTTON(openfeint)->enabled = true;
		TEXT_RENDERING(scoreTitle)->hide = false;
	} else {
		TEXT_RENDERING(persoTweakText[0])->hide=false;
		TEXT_RENDERING(persoTweakText[1])->hide=false;
		RENDERING(persoTweakCont[0])->hide=false;
		RENDERING(persoTweakCont[1])->hide=false;
		BUTTON(persoTweakCont[0])->enabled = true;
		BUTTON(persoTweakCont[1])->enabled = true;
		
		std::stringstream s;
		s << "Grid size : " << gridSize;
		TEXT_RENDERING(persoTweakText[0])->text = localizeAPI->text(s.str().c_str());
		s.str("");
		s << "Number of tiles : " << numberTiles;
		TEXT_RENDERING(persoTweakText[1])->text = localizeAPI->text(s.str().c_str());
	}
}

GameState ModeMenuStateManager::Update(float dt) {
	if (ended && !perso) {
		GameMode m = modeMgr->GetMode();
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
        a << playerName << " ... ";
		a.precision(1);
		if (m==ScoreAttack || m==TilesAttack) a << std::fixed << entry.time << " s";
		else a << entry.points;
		if (m==Normal) a << "... "<< localizeAPI->text("niv") << " " << entry.level;
		TEXT_RENDERING(yourScore)->text = a.str();
		LoadScore(m);
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
	
	//perso buttons
	if (BUTTON(persoTweakCont[0])->clicked) {
		gridSize++;
		if (gridSize>12) gridSize=3;
		std::stringstream s;
		s << "Grid size : " << gridSize;
		TEXT_RENDERING(persoTweakText[0])->text = localizeAPI->text(s.str().c_str());
	} if (BUTTON(persoTweakCont[1])->clicked) {
		numberTiles++;
		if (numberTiles>8) numberTiles=2;
		std::stringstream s;
		s << "Number of tiles : " << numberTiles;
		TEXT_RENDERING(persoTweakText[1])->text = localizeAPI->text(s.str().c_str());
	} 

	if (BUTTON(playButton)->clicked) {
		SOUND(playButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		RENDERING(herisson->actor.e)->hide = true;
		TRANSFORM(herissonActor)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.X;
		if (!perso) TEXT_RENDERING(title)->hide = true;
		return ModeMenuToBlackState;
	} if (BUTTON(back)->clicked) {
		SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
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
	theGridSystem.GridSize = gridSize;
	theGridSystem.Types = numberTiles;
    // nothing to do here: will be done in LateExit after Fading
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
	
	TEXT_RENDERING(persoTweakText[0])->hide=true;
	TEXT_RENDERING(persoTweakText[1])->hide=true;
	RENDERING(persoTweakCont[0])->hide=true;
	RENDERING(persoTweakCont[1])->hide=true;
	BUTTON(persoTweakCont[0])->enabled = false;
	BUTTON(persoTweakCont[1])->enabled = false;
}
