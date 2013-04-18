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

#include <glm/glm.hpp>

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"
#include "base/PlacementHelper.h"
#include "base/ObjectSerializer.h"
#include "base/Log.h"

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

#include "util/ScoreStorageProxy.h"

#include "DepthLayer.h"

ModeMenuStateManager::ModeMenuStateManager(StorageAPI* storag, NameInputAPI* pNameInputAPI, SuccessManager* sMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI, CommunicationAPI* comAPI) {
	storageAPI = storag;
    successAPI = sAPI;
	successMgr = sMgr;
	communicationAPI = comAPI;
	nameInputAPI = pNameInputAPI;
	localizeAPI = lAPI;
	difficulty = DifficultyEasy;
    gameOverState = NoGame;
}

void ModeMenuStateManager::Setup() {
	const Color green("green");

	//Creating text entities
	for (int i=0; i<5; i++) {
		std::stringstream a;
		a.str("");
		a << "scoresName_" << i;
		scoresName[i] = theEntityManager.CreateEntity(a.str());
		a.str("");
		a << "scoresPoints_" << i;
		scoresPoints[i] = theEntityManager.CreateEntity(a.str());
		a.str("");
		a << "scoresLevel_" << i;
		scoresLevel[i] = theEntityManager.CreateEntity(a.str());

		ADD_COMPONENT(scoresName[i], Transformation);
		ADD_COMPONENT(scoresName[i], TextRendering);
		ADD_COMPONENT(scoresPoints[i], Transformation);
		ADD_COMPONENT(scoresPoints[i], TextRendering);
		ADD_COMPONENT(scoresLevel[i], Transformation);
		ADD_COMPONENT(scoresLevel[i], TextRendering);

		TRANSFORM(scoresName[i])->z = TRANSFORM(scoresPoints[i])->z = TRANSFORM(scoresLevel[i])->z = DL_MainMenuUITxt;

		TRANSFORM(scoresName[i])->position.y =
			TRANSFORM(scoresPoints[i])->position.y =
				TRANSFORM(scoresLevel[i])->position.y = (float)PlacementHelper::GimpYToScreen(605 + i * 95);
		TRANSFORM(scoresName[i])->position.x = (float)PlacementHelper::GimpXToScreen(92);
		TRANSFORM(scoresPoints[i])->position.x = (float)PlacementHelper::GimpXToScreen(552);
		TRANSFORM(scoresLevel[i])->position.x = (float)PlacementHelper::GimpXToScreen(590);

		TEXT_RENDERING(scoresName[i])->charHeight =
			TEXT_RENDERING(scoresPoints[i])->charHeight =
				TEXT_RENDERING(scoresLevel[i])->charHeight = PlacementHelper::GimpHeightToScreen(45);
		TEXT_RENDERING(scoresPoints[i])->flags |= TextRenderingComponent::IsANumberBit;
		TEXT_RENDERING(scoresName[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(scoresPoints[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(scoresLevel[i])->positioning = TextRenderingComponent::LEFT;

		TEXT_RENDERING(scoresName[i])->show = false;
		TEXT_RENDERING(scoresPoints[i])->show = false;
		TEXT_RENDERING(scoresLevel[i])->show = false;

		TEXT_RENDERING(scoresName[i])->color =
			TEXT_RENDERING(scoresPoints[i])->color =
				TEXT_RENDERING(scoresLevel[i])->color = green;
	}
	// back button
	back = theEntityManager.CreateEntity("back");
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
    BUTTON(back)->overSize = 1.3;
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(100), 
									  (float)PlacementHelper::GimpHeightToScreen(75));
	TransformationSystem::setPosition(TRANSFORM(back), 
									  glm::vec2((float)PlacementHelper::GimpXToScreen(92),
									  			(float)PlacementHelper::GimpYToScreen(95)), 
									  TransformationSystem::W);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("back");
	BUTTON(back)->enabled = false;

	// score title
	scoreTitle = theEntityManager.CreateEntity("scoreTitle");
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(92), 
												(float)PlacementHelper::GimpYToScreen(520));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("score");
	TEXT_RENDERING(scoreTitle)->fontName = "typo";
	TEXT_RENDERING(scoreTitle)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(scoreTitle)->color = green;
	TEXT_RENDERING(scoreTitle)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(scoreTitle)->show = false;

	// score title
	average = theEntityManager.CreateEntity("average");
	ADD_COMPONENT(average, Transformation);
	TRANSFORM(average)->size.x = (float)PlacementHelper::GimpWidthToScreen(620);
	TRANSFORM(average)->position = glm::vec2((float)PlacementHelper::GimpXToScreen(92), 
											 (float)PlacementHelper::GimpYToScreen(675 + 400));
	TRANSFORM(average)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(average, TextRendering);
	TEXT_RENDERING(average)->text = "";
	TEXT_RENDERING(average)->fontName = "typo";
	TEXT_RENDERING(average)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(average)->color = green;
	TEXT_RENDERING(average)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(average)->show = false;
	TEXT_RENDERING(average)->flags |= TextRenderingComponent::AdjustHeightToFillWidthBit;

	// play text
	playText = theEntityManager.CreateEntity("playText");
	ADD_COMPONENT(playText, Transformation);
	TRANSFORM(playText)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(275));
	TRANSFORM(playText)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(playText, TextRendering);
	TEXT_RENDERING(playText)->text = localizeAPI->text("play");
	TEXT_RENDERING(playText)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(playText)->color = green;
	TEXT_RENDERING(playText)->fontName = "typo";
	TEXT_RENDERING(playText)->charHeight = PlacementHelper::GimpHeightToScreen(100);
	TEXT_RENDERING(playText)->show = false;
	// play button
	playContainer = theEntityManager.CreateEntity("playContainer");
	ADD_COMPONENT(playContainer, Transformation);
	ADD_COMPONENT(playContainer, Container);
	CONTAINER(playContainer)->entities.push_back(playText);
	CONTAINER(playContainer)->includeChildren = true;
	ADD_COMPONENT(playContainer, Button);
	ADD_COMPONENT(playContainer, Sound);
	BUTTON(playContainer)->enabled = false;

	//difficulty text
	eDifficulty = theEntityManager.CreateEntity("eDifficulty");
	ADD_COMPONENT(eDifficulty, Transformation);
	ADD_COMPONENT(eDifficulty, TextRendering);
	TRANSFORM(eDifficulty)->z = DL_MainMenuUITxt;
	TRANSFORM(eDifficulty)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(375));
	TEXT_RENDERING(eDifficulty)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(eDifficulty)->show = false;
	TEXT_RENDERING(eDifficulty)->charHeight = PlacementHelper::GimpHeightToScreen(45);
	TEXT_RENDERING(eDifficulty)->color = green;

	//difficulty container
	bDifficulty = theEntityManager.CreateEntity("bDifficulty");
	ADD_COMPONENT(bDifficulty, Transformation);
	ADD_COMPONENT(bDifficulty, Container);
	CONTAINER(bDifficulty)->entities.push_back(eDifficulty);
	CONTAINER(bDifficulty)->includeChildren = true;
	ADD_COMPONENT(bDifficulty, Button);
	ADD_COMPONENT(bDifficulty, Sound);

	// your score
	yourScore = theEntityManager.CreateEntity("yourScore");
	ADD_COMPONENT(yourScore, Transformation);
	ADD_COMPONENT(yourScore, TextRendering);
	TRANSFORM(yourScore)->z = DL_MainMenuUITxt;
	TRANSFORM(yourScore)->position = glm::vec2(0.f,(float)PlacementHelper::GimpYToScreen(1215));
	TEXT_RENDERING(yourScore)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(yourScore)->show = false;
	TEXT_RENDERING(yourScore)->charHeight = PlacementHelper::GimpHeightToScreen(56);
	TEXT_RENDERING(yourScore)->color = green;

	// fond
	fond = theEntityManager.CreateEntity("fond");
	ADD_COMPONENT(fond, Transformation);
	TRANSFORM(fond)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(712), 
									  (float)PlacementHelper::GimpHeightToScreen(1124));
	TRANSFORM(fond)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(fond), 
									  glm::vec2((float)PlacementHelper::GimpXToScreen(44), 
									  			(float)PlacementHelper::GimpYToScreen(24)), 
									  TransformationSystem::NW);
	ADD_COMPONENT(fond, Rendering);
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("fond_menu_mode");
	RENDERING(fond)->color.a = 0.5;

	// enableSwarm text
	enableSwarm = theEntityManager.CreateEntity("enableSwarm");
	ADD_COMPONENT(enableSwarm, Transformation);
	ADD_COMPONENT(enableSwarm, TextRendering);
	TRANSFORM(enableSwarm)->z = DL_MainMenuUITxt;
	TRANSFORM(enableSwarm)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(1160));
	TRANSFORM(enableSwarm)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(460), 0.f);
	TEXT_RENDERING(enableSwarm)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(enableSwarm)->show = false;
	TEXT_RENDERING(enableSwarm)->charHeight = PlacementHelper::GimpHeightToScreen(40);
	TEXT_RENDERING(enableSwarm)->color = green;
	TEXT_RENDERING(enableSwarm)->flags |= TextRenderingComponent::MultiLineBit;
	TEXT_RENDERING(enableSwarm)->text = localizeAPI->text("get_swarm");

	// enableSwarm container
	enableSwarmContainer = theEntityManager.CreateEntity("enableSwarmContainer");
	ADD_COMPONENT(enableSwarmContainer, Transformation);
	ADD_COMPONENT(enableSwarmContainer, Container);
	CONTAINER(enableSwarmContainer)->entities.push_back(enableSwarm);
	CONTAINER(enableSwarmContainer)->includeChildren = true;
	ADD_COMPONENT(enableSwarmContainer, Button);
	BUTTON(enableSwarmContainer)->enabled = false;
}

void ModeMenuStateManager::LoadScore(int mode, Difficulty dif) {
	float avg = 0.f;

	std::stringstream ss;
	ss << "where mode = " << mode << " and difficulty = " << dif;
	if (mode == 2) ss << " order by time asc limit 5";
	else ss << " order by points desc limit 5";

	ScoreStorageProxy ssp;
	storageAPI->loadEntries(&ssp, "*", ss.str());


	//a bit heavy, but..
	std::vector<Score> entries;
	while (! ssp.isEmpty()) {
		entries.push_back(ssp._queue.back());
		avg += (mode == TilesAttack) ? ssp._queue.back().time : ssp._queue.back().points;
		ssp.popAnElement();		
	}

	if (entries.size() > 0)
		avg /= entries.size();

	bool alreadyRed = false;
	for (unsigned int i=0; i<5; i++) {
		TextRenderingComponent* trcN = TEXT_RENDERING(scoresName[i]);
		TextRenderingComponent* trcP = TEXT_RENDERING(scoresPoints[i]);
		TextRenderingComponent* trcL = TEXT_RENDERING(scoresLevel[i]);
		if (i < entries.size()) {
			trcN->show = true;
			trcP->show = true;
			std::stringstream a;
			a.precision(1);
			if (mode==Normal || mode==Go100Seconds) {
				a << std::fixed << entries[i].points;
				trcP->flags |= TextRenderingComponent::IsANumberBit;
			} else {
				a << std::fixed << entries[i].time << " s";
				trcP->flags &= ~TextRenderingComponent::IsANumberBit;
			}
			trcP->text = a.str();
			trcN->text = entries[i].name;

			// a.str(""); a<< std::fixed <<localizeAPI->text("lvl", "lvl") << " " <<entries[i].level;
			a.str(""); a<< std::fixed <<localizeAPI->text("lvl") << " " <<entries[i].level;
			trcL->text = a.str();
			//affichage lvl
			if (mode==Normal) {
				trcL->show = true;
			}
			//highlight the just-played score if it is in the top 5
			if (!alreadyRed && gameOverState == AskingPlayerName &&
			 ((mode==Normal && (unsigned int)entries[i].points == modeMgr->points)
			  || (mode==TilesAttack && glm::abs(entries[i].time-modeMgr->time)<0.01f)
			  || (mode==Go100Seconds && (unsigned int)entries[i].points == modeMgr->points))
			   && entries[i].name == playerName) {
				trcN->color = Color(1.0f,0.f,0.f);
				trcP->color = Color(1.0f,0.f,0.f);
				trcL->color = Color(1.0f,0.f,0.f);
				alreadyRed = true;
			} else {
				trcN->color = trcP->color = trcL->color = Color(3.0/255.0, 99.0/255, 71.0/255);
			}
		} else {
			trcP->show = false;
			trcN->show = false;
			trcL->show = false;
		}
	}

	if (avg > 0) {
		std::stringstream a;
		a.precision(1);

		a << localizeAPI->text("average_score") << ' ';
		if (mode==Normal || mode==Go100Seconds) {
			a << (int)avg;
		} else {
			a << std::fixed << ((int)(avg*10))/10.f << " s";
		}
		TEXT_RENDERING(average)->text = a.str();
		TEXT_RENDERING(average)->show = true;
	} else {
		TEXT_RENDERING(average)->show = false;
	}
}


void ModeMenuStateManager::Enter() {
	LOGI("'"<< __PRETTY_FUNCTION__ << "'");
    pleaseGoBack = false;
	successMgr->sHardScore(storageAPI);

	BUTTON(back)->enabled = true;
	BUTTON(playContainer)->enabled = true;

	//first launch : set an easiest diff
    if (gameOverState == NoGame) {
   		std::stringstream ss;
		ss << "where mode = " << modeMgr->GetMode();

	    ScoreStorageProxy ssp;
	    int count = storageAPI->count(&ssp, "*", ss.str());
	    LOGV(LogVerbosity::VERBOSE1, "There are currently " << count << " scores in database");
		if  (count <= 1)
			difficulty = DifficultyEasy;
		else if  (count < 5)
			difficulty = DifficultyMedium;
		else
			difficulty = DifficultyHard;
	} else {
		difficulty = theGridSystem.sizeToDifficulty();
	}

	LoadScore(modeMgr->GetMode(), difficulty);


	TEXT_RENDERING(playText)->show = true;
	RENDERING(back)->show = true;
	RENDERING(menubg)->show = true;
	TEXT_RENDERING(title)->show = true;
	RENDERING(menufg)->show = true;
	RENDERING(fond)->show = true;
	// TEXT_RENDERING(playText)->text = (gameOverState != NoGame) ? localizeAPI->text("restart", "Restart") : localizeAPI->text("play", "Play");
	TEXT_RENDERING(playText)->text = (gameOverState != NoGame) ? localizeAPI->text("restart") : localizeAPI->text("play");
	TEXT_RENDERING(scoreTitle)->show = true;
	TEXT_RENDERING(eDifficulty)->show = true;
	BUTTON(bDifficulty)->enabled = true;

    // if (difficulty == DifficultyEasy)
    //     TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1", "Easy") + " }";
    // else if (difficulty == DifficultyMedium)
    //     TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2", "Medium") + " }";
    // else
    //     TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_3", "Hard") + " }";

	if (difficulty == DifficultyEasy)
        TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1") + " }";
    else if (difficulty == DifficultyMedium)
        TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2") + " }";
    else
        TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_3") + " }";

    CONTAINER(playContainer)->enable = CONTAINER(bDifficulty)->enable = true;

    #ifdef ANDROID
    if (gameOverState == NoGame) {
		if (!communicationAPI->swarmInstalled()) {
			BUTTON(enableSwarmContainer)->enabled = true;
			TEXT_RENDERING(enableSwarm)->show = true;
			CONTAINER(enableSwarmContainer)->enable = true;
    	}
    }
    #endif
}

void ModeMenuStateManager::submitScore(const std::string& playerName) {
    GameMode m = modeMgr->GetMode();
    ScoreStorageProxy ssp;
    ssp.pushAnElement();
    ssp.setValue("points", ObjectSerializer<int>::object2string(modeMgr->points));
    ssp.setValue("time", ObjectSerializer<float>::object2string(modeMgr->time));
    ssp.setValue("name", playerName);
    
    if (m==Normal) {
    	NormalGameModeManager* ng = static_cast<NormalGameModeManager*>(modeMgr);
	    ssp.setValue("level", ObjectSerializer<int>::object2string(ng->currentLevel()));
    } else {
	    ssp.setValue("level", ObjectSerializer<int>::object2string(1));
    }
    storageAPI->saveEntries(&ssp);
}

bool ModeMenuStateManager::isCurrentScoreAHighOne() {
	std::stringstream ss;
	ss << "where mode = " << modeMgr->GetMode() << " and difficulty = " << difficulty;

    if (modeMgr->GetMode() == Normal || modeMgr->GetMode() == Go100Seconds) {
        ss << " and points >= " << modeMgr->points;
    } else {
        ss << " and time <= " << modeMgr->time;
    }

    ScoreStorageProxy ssp;
    return (storageAPI->count(&ssp, "*", ss.str()) < 5);
}

GameState ModeMenuStateManager::Update(float dt) {
    switch (gameOverState) {
        case NoGame: {
			break;
        }
        case GameEnded: {
			#if 0
			//show twitter, fb, und so weiter
			RENDERING(facebook)->hide = false;
			RENDERING(twitter)->hide = false;
			BUTTON(facebook)->enabled = true;
			BUTTON(twitter)->enabled = true;
			#endif
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

            TEXT_RENDERING(yourScore)->show = true;
            std::stringstream a;
            a.precision(1);
            if (modeMgr->GetMode()==Normal) {
                // a << modeMgr->points << " : "<< localizeAPI->text("lvl", "lvl") << " " << static_cast<NormalGameModeManager*>(modeMgr)->currentLevel();
                a << modeMgr->points << " : "<< localizeAPI->text("lvl") << " " << static_cast<NormalGameModeManager*>(modeMgr)->currentLevel();
            } else if (modeMgr->GetMode()==Go100Seconds) {
                a << modeMgr->points;
			} else {
                a << std::fixed << ((int)(modeMgr->time*10))/10.f << " s";
            }
            TEXT_RENDERING(yourScore)->text = a.str();
            successMgr->sTestEverything(storageAPI);
            break;
        }
        case AskingPlayerName: {
            if (nameInputAPI->done(playerName)) {
                if (modeMgr->GetMode()==Normal)
					successMgr->sBTAC(storageAPI, difficulty, modeMgr->points);
				else if (modeMgr->GetMode()==TilesAttack)
					successMgr->sBTAM(storageAPI, difficulty, modeMgr->time);
                nameInputAPI->hide();
                submitScore(playerName);
                LoadScore(modeMgr->GetMode(), difficulty);
                gameOverState = NoGame;

				if (communicationAPI->mustShowRateDialog()) {
					TRANSFORM(herisson->actor.e)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(herisson->actor.e)->size.x;
					TEXT_RENDERING(title)->show = false;
					this->LateExit();
					return RateIt;
				}
            }
            break;
        }
    }

	//herisson
	Entity herissonActor=  herisson->actor.e;
	if (TRANSFORM(herissonActor)->position.x < PlacementHelper::ScreenWidth+TRANSFORM(herissonActor)->size.x) {
		TRANSFORM(herissonActor)->position.x += herisson->actor.speed*dt;
		updateAnim(herisson, dt);
	} else {
		RENDERING(herissonActor)->show = false;
	}

	if (gameOverState != AskingPlayerName) {
		//difficulty button
		if (BUTTON(bDifficulty)->clicked) {
			SOUND(bDifficulty)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			difficulty = theGridSystem.nextDifficulty(difficulty);

			if (difficulty == DifficultyEasy)
				// TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1", "Easy") + " }";
				TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_1") + " }";
			else if (difficulty == DifficultyMedium)
				// TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2", "Medium") + " }";
				TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_2") + " }";
			else
				// TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_3", "Hard") + " }";
				TEXT_RENDERING(eDifficulty)->text = "{ " + localizeAPI->text("diff_3") + " }";

			// TEXT_RENDERING(playText)->text = localizeAPI->text("play", "Play");
			TEXT_RENDERING(playText)->text = localizeAPI->text("play");
			LoadScore(modeMgr->GetMode(), difficulty);
		}

		//new game button
		else if (BUTTON(playContainer)->clicked) {
			SOUND(playContainer)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			RENDERING(herisson->actor.e)->show = false;
			TRANSFORM(herissonActor)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.x;
			TEXT_RENDERING(title)->show = false;

			std::stringstream ss;
			ss << "where mode = " << (int)modeMgr->GetMode() << " and difficulty = " << (int)difficulty;
        	ScoreStorageProxy ssp;
			if (storageAPI->count(&ssp, "*", ss.str()) == 0) {
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
#if 0
		//facebook button
		else if (BUTTON(facebook)->clicked) {
			communicationAPI->shareFacebook();
		}
		//twitter button
		else if (BUTTON(twitter)->clicked) {
			communicationAPI->shareTwitter();
		}
#endif
		//enableSwarm button
		else if (BUTTON(enableSwarmContainer)->clicked) {
			LOGE("NOT HANDLED YET!");
			// communicationAPI->swarmRegistering(modeMgr->GetMode(), theGridSystem.sizeToDifficulty());
			TEXT_RENDERING(enableSwarm)->show = false;
			BUTTON(enableSwarmContainer)->enabled = false;
		}
	}
	return ModeMenu;
}

void ModeMenuStateManager::Exit() {
	theGridSystem.setGridFromDifficulty(difficulty);

	successMgr->NewGame(difficulty);

    CONTAINER(playContainer)->enable =
    	CONTAINER(bDifficulty)->enable =
    	CONTAINER(enableSwarmContainer)->enable = false;
}

void ModeMenuStateManager::LateExit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
	TEXT_RENDERING(yourScore)->show = false;
	TEXT_RENDERING(playText)->show = false;
	RENDERING(back)->show = false;
	for (int i=0;i<5;i++) {
		TEXT_RENDERING(scoresName[i])->show = false;
		TEXT_RENDERING(scoresPoints[i])->show = false;
		TEXT_RENDERING(scoresLevel[i])->show = false;
	}
	BUTTON(back)->enabled = false;
	BUTTON(playContainer)->enabled = false;
	TEXT_RENDERING(eDifficulty)->show = false;
	BUTTON(bDifficulty)->enabled = false;
#if 0
	BUTTON(facebook)->enabled = false;
	BUTTON(twitter)->enabled = false;
	RENDERING(facebook)->hide = true;
	RENDERING(twitter)->hide = true;
#endif
	BUTTON(enableSwarmContainer)->enabled = false;
	TEXT_RENDERING(enableSwarm)->show = false;

	gameOverState = NoGame;

	RENDERING(menubg)->show = false;
	RENDERING(menufg)->show = false;
	RENDERING(fond)->show = false;
	TEXT_RENDERING(scoreTitle)->show = false;
	TEXT_RENDERING(average)->show = false;
	RENDERING(herisson->actor.e)->show = false;
}
