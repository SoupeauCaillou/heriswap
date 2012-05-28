#include "ModeMenuStateManager.h"

#include <sstream>
#include <fstream>

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include "systems/MorphingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"

#include "DepthLayer.h"
#include "GridSystem.h"

ModeMenuStateManager::ModeMenuStateManager(StorageAPI* storag, NameInputAPI* pNameInputAPI, SuccessManager* sMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI) {
	storage = storag;
    successAPI = sAPI;
	successMgr = sMgr;
	nameInputAPI = pNameInputAPI;
	localizeAPI = lAPI;
	difficulty = 1;
    gameOverState = NoGame;
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
    BUTTON(back)->overSize = 1.3;
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(75));
	TransformationSystem::setPosition(TRANSFORM(back), Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(82)), TransformationSystem::NW);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("back.png");
	BUTTON(back)->enabled = false;

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(476));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("score", "Score :");
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
	TEXT_RENDERING(play)->text = localizeAPI->text("jouer", "Jouer");
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
	if (difficulty==0)
		TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_1", "easy 5x5");
	else
		TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_2", "medium 8x8");

	//difficulty container
	bDifficulty = theEntityManager.CreateEntity();
	ADD_COMPONENT(bDifficulty, Transformation);
	ADD_COMPONENT(bDifficulty, Container);
	CONTAINER(bDifficulty)->entities.push_back(eDifficulty);
	CONTAINER(bDifficulty)->includeChildren = true;
	ADD_COMPONENT(bDifficulty, Button);
	ADD_COMPONENT(bDifficulty, Sound);
	BUTTON(bDifficulty)->enabled = false;

	#ifdef ANDROID
	// openfeint button
	openfeint = theEntityManager.CreateEntity();
	ADD_COMPONENT(openfeint, Transformation);
	ADD_COMPONENT(openfeint, Rendering);
	ADD_COMPONENT(openfeint, Button);
    BUTTON(openfeint)->overSize = 1.3;
	ADD_COMPONENT(openfeint, Sound);
	TRANSFORM(openfeint)->size = Vector2(PlacementHelper::GimpWidthToScreen(90), PlacementHelper::GimpHeightToScreen(91));
	TransformationSystem::setPosition(TRANSFORM(openfeint), Vector2(PlacementHelper::GimpXToScreen(605), PlacementHelper::GimpYToScreen(476)), TransformationSystem::NW);
	TRANSFORM(openfeint)->z = DL_MainMenuUITxt;
	RENDERING(openfeint)->texture = theRenderingSystem.loadTextureFile("openfeint.png");
	BUTTON(openfeint)->enabled = false;
	#endif

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
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("fond_menu_mode.png");
	RENDERING(fond)->color.a = 0.5;
}

void ModeMenuStateManager::LoadScore(int mode, int dif) {
	/*getting scores*/
	std::vector<StorageAPI::Score> entries = storage->savedScores(mode, dif);

	/* treatment*/
	bool alreadyGreen = false;
	for (unsigned int i=0; i<5; i++) {
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

			a.str(""); a<< std::fixed <<localizeAPI->text("lvl", "niv") << " " <<entries[i].level;
			trcL->text = a.str();
			//affichage lvl
			if (mode==Normal) {
				trcL->hide = false;
			}
			if (!alreadyGreen && gameOverState == AskingPlayerName &&
			 ((mode==Normal && (unsigned int)entries[i].points == modeMgr->points)
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
    pleaseGoBack = false;
	successMgr->sHardScore(storage);

	BUTTON(back)->enabled = true;
	BUTTON(playButton)->enabled = true;

    difficulty = (theGridSystem.GridSize == 8) ? 1 : 0;

	LoadScore(modeMgr->GetMode(), difficulty);


	TEXT_RENDERING(play)->hide = false;
	RENDERING(back)->hide = false;
	RENDERING(menubg)->hide = false;
	TEXT_RENDERING(title)->hide = false;
	RENDERING(menufg)->hide = false;
	RENDERING(fond)->hide = false;
	TEXT_RENDERING(play)->text = (gameOverState != NoGame) ? localizeAPI->text("rejouer", "Rejouer") : localizeAPI->text("jouer", "Jouer");
	#ifdef ANDROID
	RENDERING(openfeint)->hide = false;
	BUTTON(openfeint)->enabled = true;
	#endif
	TEXT_RENDERING(scoreTitle)->hide = false;
	TEXT_RENDERING(eDifficulty)->hide=false;
	BUTTON(bDifficulty)->enabled = true;

    if (difficulty==0)
        TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_1", "easy");
    else
        TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_2", "medium");
}

void ModeMenuStateManager::submitScore(const std::string& playerName) {
    GameMode m = modeMgr->GetMode();
    StorageAPI::Score entry;
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
}

bool ModeMenuStateManager::isCurrentScoreAHighOne() {
    std::vector<StorageAPI::Score> entries = storage->savedScores(modeMgr->GetMode(), difficulty);

    int s = entries.size();
    if (s < 5)
        return true;

    if (modeMgr->GetMode() == Normal) {
        return modeMgr->points > (unsigned int)entries[s - 1].points;
    } else {
        return modeMgr->time < entries[s - 1].time;
    }
}

GameState ModeMenuStateManager::Update(float dt) {
    switch (gameOverState) {
        case NoGame: {
			break;
        }
        case GameEnded: {
            // ask player's name if needed
            if (isCurrentScoreAHighOne()) {
                nameInputAPI->show();
                gameOverState = AskingPlayerName;
            } else {
                gameOverState = NoGame;
                submitScore("rzehtrtyBg");
            }

            TEXT_RENDERING(yourScore)->hide = false;
            std::stringstream a;
            a.precision(1);
            if (modeMgr->GetMode()==Normal) {
                a << modeMgr->points << "... "<< localizeAPI->text("lvl", "niv") << " " << static_cast<NormalGameModeManager*>(modeMgr)->currentLevel();
            } else {
                a << std::fixed << modeMgr->time << " s";
            }
            TEXT_RENDERING(yourScore)->text = a.str();
            successMgr->sTestEverything(storage);
            break;
        }
        case AskingPlayerName: {
            if (nameInputAPI->done(playerName)) {
				successMgr->sTheyGood(isCurrentScoreAHighOne());
                if (modeMgr->GetMode()==Normal)
					successMgr->sBTAC(storage, difficulty, modeMgr->points);
				else
					successMgr->sBTAM(storage, difficulty, modeMgr->time);
                nameInputAPI->hide();
                submitScore(playerName);
                LoadScore(modeMgr->GetMode(), difficulty);
                gameOverState = NoGame;
            }
            break;
        }
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
	if (gameOverState != AskingPlayerName && BUTTON(bDifficulty)->clicked) {
		difficulty++;
		if (difficulty==2) difficulty=0;
		LoadScore(modeMgr->GetMode(), difficulty);
		if (difficulty==0)
			TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_1", "easy");
		else
			TEXT_RENDERING(eDifficulty)->text = localizeAPI->text("diff_2", "medium");

		TEXT_RENDERING(play)->text = localizeAPI->text("jouer", "Jouer");
	}

	if (gameOverState != AskingPlayerName && BUTTON(playButton)->clicked) {
		SOUND(playButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		RENDERING(herisson->actor.e)->hide = true;
		TRANSFORM(herissonActor)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.X;
		TEXT_RENDERING(title)->hide = true;
		return ModeMenuToBlackState;
	} if (gameOverState != AskingPlayerName && (BUTTON(back)->clicked || pleaseGoBack)) {
        pleaseGoBack = false;
		SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		return MainMenu;
	}
	#ifdef ANDROID
	if (gameOverState != AskingPlayerName && BUTTON(openfeint)->clicked) {
		successAPI->openfeintLB(modeMgr->GetMode(), difficulty);
	}
	#endif
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
	successMgr->NewGame(difficulty);
}

void ModeMenuStateManager::LateExit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;
	#ifdef ANDROID
	RENDERING(openfeint)->hide = true;
	BUTTON(openfeint)->enabled = false;
	#endif
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresLevel[i])->hide = true;
	}
	RENDERING(menubg)->hide = true;
	RENDERING(menufg)->hide = true;
	RENDERING(fond)->hide = true;
	gameOverState = NoGame;
	BUTTON(back)->enabled = false;
	BUTTON(playButton)->enabled = false;
	TEXT_RENDERING(scoreTitle)->hide = true;
	RENDERING(herisson->actor.e)->hide = true;

	TEXT_RENDERING(eDifficulty)->hide=true;
	BUTTON(bDifficulty)->enabled = false;
}
