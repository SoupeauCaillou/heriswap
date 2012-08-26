/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
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
#include "states/HelpStateManager.h"

#include "DepthLayer.h"
#include "GridSystem.h"

ModeMenuStateManager::ModeMenuStateManager(StorageAPI* storag, NameInputAPI* pNameInputAPI, SuccessManager* sMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI, CommunicationAPI* comAPI) {
	storage = storag;
    successAPI = sAPI;
	successMgr = sMgr;
	communicationAPI = comAPI;
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
	TransformationSystem::setPosition(TRANSFORM(back), Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(95)), TransformationSystem::W);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("back");
	BUTTON(back)->enabled = false;

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(520));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("score", "Highscores:");
	TEXT_RENDERING(scoreTitle)->fontName = "typo";
	TEXT_RENDERING(scoreTitle)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(scoreTitle)->color = green;
	TEXT_RENDERING(scoreTitle)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(scoreTitle)->hide = true;

	// play text
	playText = theEntityManager.CreateEntity();
	ADD_COMPONENT(playText, Transformation);
	TRANSFORM(playText)->position = Vector2(0, PlacementHelper::GimpYToScreen(275));
	TRANSFORM(playText)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(playText, TextRendering);
	TEXT_RENDERING(playText)->text = localizeAPI->text("jouer", "Start");
	TEXT_RENDERING(playText)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(playText)->color = green;
	TEXT_RENDERING(playText)->fontName = "typo";
	TEXT_RENDERING(playText)->charHeight = PlacementHelper::GimpHeightToScreen(100);
	TEXT_RENDERING(playText)->hide = true;
	// play button
	playContainer = theEntityManager.CreateEntity();
	ADD_COMPONENT(playContainer, Transformation);
	ADD_COMPONENT(playContainer, Container);
	CONTAINER(playContainer)->entities.push_back(playText);
	CONTAINER(playContainer)->includeChildren = true;
	ADD_COMPONENT(playContainer, Button);
	ADD_COMPONENT(playContainer, Sound);
	BUTTON(playContainer)->enabled = false;

	//difficulty text
	eDifficulty = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eDifficulty)->z = DL_MainMenuUITxt;
	TRANSFORM(eDifficulty)->position = Vector2(0, PlacementHelper::GimpYToScreen(375));
	TEXT_RENDERING(eDifficulty)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(eDifficulty)->hide = true;
	TEXT_RENDERING(eDifficulty)->charHeight = PlacementHelper::GimpHeightToScreen(45);
	TEXT_RENDERING(eDifficulty)->color = green;

	//difficulty container
	bDifficulty = theEntityManager.CreateEntity();
	ADD_COMPONENT(bDifficulty, Transformation);
	ADD_COMPONENT(bDifficulty, Container);
	CONTAINER(bDifficulty)->entities.push_back(eDifficulty);
	CONTAINER(bDifficulty)->includeChildren = true;
	ADD_COMPONENT(bDifficulty, Button);
	ADD_COMPONENT(bDifficulty, Sound);
	BUTTON(bDifficulty)->enabled = false;

	// your score
	yourScore = theTextRenderingSystem.CreateEntity();
	TRANSFORM(yourScore)->z = DL_MainMenuUITxt;
	TRANSFORM(yourScore)->position = Vector2(PlacementHelper::GimpXToScreen(250),PlacementHelper::GimpYToScreen(1242));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(yourScore)->charHeight = PlacementHelper::GimpHeightToScreen(56);
	TEXT_RENDERING(yourScore)->color = green;

	// facebook button
	facebook = theEntityManager.CreateEntity();
	ADD_COMPONENT(facebook, Transformation);
	ADD_COMPONENT(facebook, Rendering);
	ADD_COMPONENT(facebook, Button);
	BUTTON(facebook)->enabled = false;
	RENDERING(facebook)->texture = theRenderingSystem.loadTextureFile("facebook");
	TRANSFORM(facebook)->position = Vector2(PlacementHelper::GimpXToScreen(500),PlacementHelper::GimpYToScreen(1242));
	TRANSFORM(facebook)->size = Vector2(PlacementHelper::GimpWidthToScreen(80), PlacementHelper::GimpHeightToScreen(80));
	TRANSFORM(facebook)->z = DL_MainMenuUITxt;

	// twitter button
	twitter = theEntityManager.CreateEntity();
	ADD_COMPONENT(twitter, Transformation);
	ADD_COMPONENT(twitter, Rendering);
	ADD_COMPONENT(twitter, Button);
	BUTTON(twitter)->enabled = false;
	RENDERING(twitter)->texture = theRenderingSystem.loadTextureFile("twitter");
	TRANSFORM(twitter)->position = Vector2(PlacementHelper::GimpXToScreen(600),PlacementHelper::GimpYToScreen(1242));
	TRANSFORM(twitter)->size = Vector2(PlacementHelper::GimpWidthToScreen(80), PlacementHelper::GimpHeightToScreen(80));
	TRANSFORM(twitter)->z = DL_MainMenuUITxt;

	// enableSwarm text
	enableSwarm = theTextRenderingSystem.CreateEntity();
	TRANSFORM(enableSwarm)->z = DL_MainMenuUITxt;
	TRANSFORM(enableSwarm)->position = Vector2(0, PlacementHelper::GimpYToScreen(1100));
	TEXT_RENDERING(enableSwarm)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(enableSwarm)->hide = true;
	TEXT_RENDERING(enableSwarm)->charHeight = PlacementHelper::GimpHeightToScreen(45);
	TEXT_RENDERING(enableSwarm)->color = green;
	TEXT_RENDERING(enableSwarm)->text = "Enable swarm to see online scores";

	// enableSwarm container
	enableSwarmContainer = theEntityManager.CreateEntity();
	ADD_COMPONENT(enableSwarmContainer, Transformation);
	ADD_COMPONENT(enableSwarmContainer, Container);
	CONTAINER(enableSwarmContainer)->entities.push_back(enableSwarm);
	CONTAINER(enableSwarmContainer)->includeChildren = true;
	ADD_COMPONENT(enableSwarmContainer, Button);
	BUTTON(enableSwarmContainer)->enabled = false;

	//difficulty text
	eDifficulty = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eDifficulty)->z = DL_MainMenuUITxt;
	TRANSFORM(eDifficulty)->position = Vector2(0, PlacementHelper::GimpYToScreen(375));
	TEXT_RENDERING(eDifficulty)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(eDifficulty)->hide = true;
	TEXT_RENDERING(eDifficulty)->charHeight = PlacementHelper::GimpHeightToScreen(45);
	TEXT_RENDERING(eDifficulty)->color = green;

	//difficulty container
	bDifficulty = theEntityManager.CreateEntity();
	ADD_COMPONENT(bDifficulty, Transformation);
	ADD_COMPONENT(bDifficulty, Container);
	CONTAINER(bDifficulty)->entities.push_back(eDifficulty);
	CONTAINER(bDifficulty)->includeChildren = true;
	ADD_COMPONENT(bDifficulty, Button);
	ADD_COMPONENT(bDifficulty, Sound);
	BUTTON(bDifficulty)->enabled = false;



	// fond
	fond = theEntityManager.CreateEntity();
	ADD_COMPONENT(fond, Transformation);
	TRANSFORM(fond)->size = Vector2(PlacementHelper::GimpWidthToScreen(712), PlacementHelper::GimpHeightToScreen(1124));
	TRANSFORM(fond)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(fond), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(24)), TransformationSystem::NW);
	ADD_COMPONENT(fond, Rendering);
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("fond_menu_mode");
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

			a.str(""); a<< std::fixed <<localizeAPI->text("lvl", "lvl") << " " <<entries[i].level;
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
	BUTTON(playContainer)->enabled = true;

    difficulty = (theGridSystem.GridSize == 8) ? 1 : 0;

	LoadScore(modeMgr->GetMode(), difficulty);


	TEXT_RENDERING(playText)->hide = false;
	RENDERING(back)->hide = false;
	RENDERING(menubg)->hide = false;
	TEXT_RENDERING(title)->hide = false;
	RENDERING(menufg)->hide = false;
	RENDERING(fond)->hide = false;
	TEXT_RENDERING(playText)->text = (gameOverState != NoGame) ? localizeAPI->text("rejouer", "Restart") : localizeAPI->text("jouer", "Play");
	TEXT_RENDERING(scoreTitle)->hide = false;
	TEXT_RENDERING(eDifficulty)->hide=false;
	BUTTON(bDifficulty)->enabled = true;

    if (difficulty==0)
        TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1", "Easy") + " }";
    else
        TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2", "Hard") + " }";
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
			//show twitter, fb, und so weiter
			RENDERING(facebook)->hide = false;
			RENDERING(twitter)->hide = false;
			BUTTON(facebook)->enabled = true;
			BUTTON(twitter)->enabled = true;
			if (!communicationAPI->swarmInstalled()) {
				BUTTON(enableSwarmContainer)->enabled = true;
				TEXT_RENDERING(enableSwarm)->hide = false;
			}

            // ask player's name if needed
            if (isCurrentScoreAHighOne()) {
                nameInputAPI->show();
                gameOverState = AskingPlayerName;
                successMgr->sTheyGood(true);
            } else {
                successMgr->sTheyGood(false);
				gameOverState = NoGame;
                submitScore("rzehtrtyBg");
            }

            TEXT_RENDERING(yourScore)->hide = false;
            std::stringstream a;
            a.precision(1);
            if (modeMgr->GetMode()==Normal) {
                a << modeMgr->points << " : "<< localizeAPI->text("lvl", "lvl") << " " << static_cast<NormalGameModeManager*>(modeMgr)->currentLevel();
            } else {
                a << std::fixed << ((int)(modeMgr->time*10))/10.f << " s";
            }
            TEXT_RENDERING(yourScore)->text = a.str();
            successMgr->sTestEverything(storage);
            break;
        }
        case AskingPlayerName: {
            if (nameInputAPI->done(playerName)) {
                if (modeMgr->GetMode()==Normal)
					successMgr->sBTAC(storage, difficulty, modeMgr->points);
				else
					successMgr->sBTAM(storage, difficulty, modeMgr->time);
                nameInputAPI->hide();
                submitScore(playerName);
                LoadScore(modeMgr->GetMode(), difficulty);
                gameOverState = NoGame;
                if (MathUtil::RandomInt(3) == 0) {
					TRANSFORM(herisson->actor.e)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herisson->actor.e)->size.X;
					TEXT_RENDERING(title)->hide = true;
					this->LateExit();
					return RateIt;
				}
            }
            break;
        }
    }

	//herisson
	Entity herissonActor=  herisson->actor.e;
	if (TRANSFORM(herissonActor)->position.X < PlacementHelper::ScreenWidth+TRANSFORM(herissonActor)->size.X) {
		TRANSFORM(herissonActor)->position.X += herisson->actor.speed*dt;
		updateAnim(herisson, dt);
	} else {
		RENDERING(herissonActor)->hide = true;
	}

	if (gameOverState != AskingPlayerName) {
		//difficulty button
		if (BUTTON(bDifficulty)->clicked) {
			SOUND(bDifficulty)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			difficulty++;
			if (difficulty==2) difficulty=0;
			LoadScore(modeMgr->GetMode(), difficulty);
			if (difficulty==0)
				TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1", "Easy") + " }";
			else
				TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2", "Hard") + " }";

			TEXT_RENDERING(playText)->text = localizeAPI->text("jouer", "Play");
		}

		//new game button
		else if (BUTTON(playContainer)->clicked) {
			SOUND(playContainer)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			RENDERING(herisson->actor.e)->hide = true;
			TRANSFORM(herissonActor)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.X;
			TEXT_RENDERING(title)->hide = true;

			if (storage->savedScores(modeMgr->GetMode(), difficulty).size() == 0) {
				// show help
				helpMgr->mode = modeMgr->GetMode();
				helpMgr->oldState = BlackToSpawn;
				this->LateExit();
				return Help;
			} else {
				return ModeMenuToBlackState;
			}
		}

		//back button
		else if (BUTTON(back)->clicked || pleaseGoBack) {
			pleaseGoBack = false;
			SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return MainMenu;
		}

		//facebook button
		else if (BUTTON(facebook)->clicked) {
			communicationAPI->shareFacebook();
		}
		//twitter button
		else if (BUTTON(twitter)->clicked) {
			communicationAPI->shareTwitter();
		}
		//enableSwarm button
		else if (BUTTON(enableSwarmContainer)->clicked) {
			communicationAPI->swarmRegistering();
		}
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
	successMgr->NewGame(difficulty);
}

void ModeMenuStateManager::LateExit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(yourScore)->hide = true;
	TEXT_RENDERING(playText)->hide = true;
	RENDERING(back)->hide = true;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->hide = true;
		TEXT_RENDERING(scoresPoints[i])->hide = true;
		TEXT_RENDERING(scoresLevel[i])->hide = true;
	}
	BUTTON(back)->enabled = false;
	BUTTON(playContainer)->enabled = false;
	TEXT_RENDERING(eDifficulty)->hide=true;
	BUTTON(bDifficulty)->enabled = false;
	BUTTON(facebook)->enabled = false;
	BUTTON(twitter)->enabled = false;
	BUTTON(enableSwarmContainer)->enabled = false;
	RENDERING(facebook)->hide = true;
	RENDERING(twitter)->hide = true;
	TEXT_RENDERING(enableSwarm)->hide = true;

	gameOverState = NoGame;

	RENDERING(menubg)->hide = true;
	RENDERING(menufg)->hide = true;
	RENDERING(fond)->hide = true;
	TEXT_RENDERING(scoreTitle)->hide = true;
	RENDERING(herisson->actor.e)->hide = true;
}
