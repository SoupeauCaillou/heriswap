/*
	This file is part of RecursiveRunner.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	RecursiveRunner is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	RecursiveRunner is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "api/StorageAPI.h"
#include "api/KeyboardInputHandlerAPI.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "states/HelpStateManager.h"

#include "DepthLayer.h"

#include "base/EntityManager.h"
#include "base/Log.h"
#include "base/ObjectSerializer.h"
#include "base/PlacementHelper.h"
#include "base/TouchInputManager.h"

#include "systems/AnimationSystem.h"
#include <systems/AutoDestroySystem.h>
#include <systems/ButtonSystem.h>
#include "systems/ContainerSystem.h"
#include "systems/MorphingSystem.h"
#include <systems/PhysicsSystem.h>
#include <systems/RenderingSystem.h>
#include "systems/SoundSystem.h"
#include <systems/TextRenderingSystem.h>
#include <systems/TransformationSystem.h>

#include "util/ScoreStorageProxy.h"

#include <glm/glm.hpp>
	
#include <iomanip>
#include <sstream>
#include <vector>
#include <sstream>
#include <fstream>

struct ModeMenuScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;

	Difficulty difficulty;
	Entity title, menufg, menubg;
	Entity herisson;

	bool pleaseGoBack;
	HelpStateManager* helpMgr;

	enum GameOverState {
		NoGame,
		GameEnded,
		AskingPlayerName
	} gameOverState;

	Entity playText, playContainer, scoresPoints[5], scoresName[5], scoresLevel[5], back, scoreTitle, average;
	Entity yourScore, fond;
	std::string playerName;

	Entity eDifficulty, bDifficulty;

#if ! SAC_MOBILE
		Entity input_label, input_textbox, input_background;
#endif

	Entity enableSwarm, enableSwarmContainer;

	ModeMenuScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
		this->game = game;
	}

	void setup() {
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
		TEXT_RENDERING(scoreTitle)->text = game->gameThreadContext->localizeAPI->text("score");
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
		TEXT_RENDERING(playText)->text = game->gameThreadContext->localizeAPI->text("play");
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
		TEXT_RENDERING(enableSwarm)->text = game->gameThreadContext->localizeAPI->text("get_swarm");

		// enableSwarm container
		enableSwarmContainer = theEntityManager.CreateEntity("enableSwarmContainer");
		ADD_COMPONENT(enableSwarmContainer, Transformation);
		ADD_COMPONENT(enableSwarmContainer, Container);
		CONTAINER(enableSwarmContainer)->entities.push_back(enableSwarm);
		CONTAINER(enableSwarmContainer)->includeChildren = true;
		ADD_COMPONENT(enableSwarmContainer, Button);
		BUTTON(enableSwarmContainer)->enabled = false;

	#if ! SAC_MOBILE
		// name input entities
		input_label = theEntityManager.CreateEntity("input_label");
		ADD_COMPONENT(input_label, Transformation);
		TRANSFORM(input_label)->position = glm::vec2(0, PlacementHelper::GimpYToScreen(275));
		TRANSFORM(input_label)->z = DL_HelpText;
		ADD_COMPONENT(input_label, TextRendering);
		TEXT_RENDERING(input_label)->text = game->gameThreadContext->localizeAPI->text("enter_name");
		TEXT_RENDERING(input_label)->fontName = "typo";
		TEXT_RENDERING(input_label)->positioning = TextRenderingComponent::CENTER;
		TEXT_RENDERING(input_label)->color = green;
		TEXT_RENDERING(input_label)->charHeight = PlacementHelper::GimpHeightToScreen(54);

		input_textbox = theEntityManager.CreateEntity("input_textbox");
		ADD_COMPONENT(input_textbox, Transformation);
		TRANSFORM(input_textbox)->position = glm::vec2(0, PlacementHelper::GimpYToScreen(390));
		TRANSFORM(input_textbox)->z = DL_HelpText;
		ADD_COMPONENT(input_textbox, TextRendering);
		TEXT_RENDERING(input_textbox)->fontName = "typo";
		TEXT_RENDERING(input_textbox)->text = "";
		TEXT_RENDERING(input_textbox)->positioning = TextRenderingComponent::CENTER;
		TEXT_RENDERING(input_textbox)->color = green;
		TEXT_RENDERING(input_textbox)->charHeight = PlacementHelper::GimpHeightToScreen(54);
		TEXT_RENDERING(input_textbox)->caret.speed = 0.5;

		input_background = theEntityManager.CreateEntity("input_background");
		ADD_COMPONENT(input_background, Transformation);
		TRANSFORM(input_background)->size = glm::vec2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(256));
		TRANSFORM(input_background)->position = glm::vec2(0, PlacementHelper::GimpYToScreen(320));
		TRANSFORM(input_background)->z = DL_HelpTextBg;
		ADD_COMPONENT(input_background, Rendering);
		RENDERING(input_background)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
		RENDERING(input_background)->color.a = 1;
	#endif
	}

	void LoadScore(GameMode mode, Difficulty dif) {
		float avg = 0.f;

		std::stringstream ss;
		ss << "where mode = " << mode << " and difficulty = " << dif;
		if (mode == TilesAttack) ss << " order by time asc limit 5";
		else ss << " order by points desc limit 5";

		ScoreStorageProxy ssp;
		game->gameThreadContext->storageAPI->loadEntries(&ssp, "*", ss.str());


		//a bit heavy, but..
		std::vector<Score> entries;
		while (! ssp.isEmpty()) {
			entries.push_back(ssp._queue.front());
			avg += (mode == TilesAttack) ? ssp._queue.front().time : ssp._queue.front().points;
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

				a.str(""); a<< std::fixed << game->gameThreadContext->localizeAPI->text("lvl") << " " <<entries[i].level;
				trcL->text = a.str();
				//affichage lvl
				if (mode==Normal) {
					trcL->show = true;
				}
				//highlight the just-played score if it is in the top 5
				if (!alreadyRed && gameOverState == AskingPlayerName &&
				 ((mode==Normal && (unsigned int)entries[i].points == game->datas->mode2Manager[game->datas->mode]->points)
				  || (mode==TilesAttack && glm::abs(entries[i].time-game->datas->mode2Manager[game->datas->mode]->time)<0.01f)
				  || (mode==Go100Seconds && (unsigned int)entries[i].points == game->datas->mode2Manager[game->datas->mode]->points))
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

			a << game->gameThreadContext->localizeAPI->text("average_score") << ' ';
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

	void submitScore(const std::string& playerName) {
	    ScoreStorageProxy ssp;
	    ssp.pushAnElement();
	    ssp.setValue("points", ObjectSerializer<int>::object2string(game->datas->mode2Manager[game->datas->mode]->points));
	    ssp.setValue("time", ObjectSerializer<float>::object2string(game->datas->mode2Manager[game->datas->mode]->time));
	    ssp.setValue("name", playerName);
	    
	    if (game->datas->mode==Normal) {
	    	NormalGameModeManager* ng = static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode]);
		    ssp.setValue("level", ObjectSerializer<int>::object2string(ng->currentLevel()));
	    } else {
		    ssp.setValue("level", ObjectSerializer<int>::object2string(1));
	    }
	    ssp.setValue("mode", ObjectSerializer<int>::object2string(game->datas->mode));
	    ssp.setValue("difficulty", ObjectSerializer<int>::object2string(difficulty));

	    game->gameThreadContext->storageAPI->saveEntries(&ssp);
	}
	    
	bool isCurrentScoreAHighOne() {
		std::stringstream ss;
		ss << "where mode = " << game->datas->mode << " and difficulty = " << difficulty;

	    if (game->datas->mode == Normal || game->datas->mode == Go100Seconds) {
	        ss << " and points >= " << game->datas->mode2Manager[game->datas->mode]->points;
	    } else {
	        ss << " and time <= " << game->datas->mode2Manager[game->datas->mode]->time;
	    }

	    ScoreStorageProxy ssp;
	    return (game->gameThreadContext->storageAPI->count(&ssp, "*", ss.str()) < 5);
    }

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum pState) override {
	}
	
	void onEnter(Scene::Enum) override {
		LOGI("'"<< __PRETTY_FUNCTION__ << "'");
		pleaseGoBack = false;
		game->datas->successMgr->sHardScore(game->gameThreadContext->storageAPI);

		BUTTON(back)->enabled = true;
		BUTTON(playContainer)->enabled = true;

		//first launch : set an easiest diff
		if (gameOverState == NoGame) {
			std::stringstream ss;
			ss << "where mode = " << game->datas->mode2Manager[game->datas->mode]->GetMode();

			ScoreStorageProxy ssp;
			int count = game->gameThreadContext->storageAPI->count(&ssp, "*", ss.str());
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

		// LoadScore(game->datas->mode, difficulty);


		TEXT_RENDERING(playText)->show = true;
		RENDERING(back)->show = true;
		RENDERING(menubg)->show = true;
		TEXT_RENDERING(title)->show = true;
		RENDERING(menufg)->show = true;
		RENDERING(fond)->show = true;
		TEXT_RENDERING(playText)->text = (gameOverState != NoGame) ? game->gameThreadContext->localizeAPI->text("restart") : game->gameThreadContext->localizeAPI->text("play");
		TEXT_RENDERING(scoreTitle)->show = true;
		TEXT_RENDERING(eDifficulty)->show = true;
		BUTTON(bDifficulty)->enabled = true;

		if (difficulty == DifficultyEasy)
			TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_1") + " }";
		else if (difficulty == DifficultyMedium)
			TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_2") + " }";
		else
			TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_3") + " }";

		CONTAINER(playContainer)->enable = CONTAINER(bDifficulty)->enable = true;

		#ifdef ANDROID
		if (gameOverState == NoGame) {
			if (!game->gameThreadContext->communicationAPI->swarmInstalled()) {
				BUTTON(enableSwarmContainer)->enabled = true;
				TEXT_RENDERING(enableSwarm)->show = true;
				CONTAINER(enableSwarmContainer)->enable = true;
			}
		}
		#endif
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float dt) override {
		switch (gameOverState) {
			case NoGame: {
				break;
			}
			case GameEnded: {
				// ask player's name if needed
				if (isCurrentScoreAHighOne()) {
	#if ! SAC_MOBILE
					TEXT_RENDERING(input_label)->show = TEXT_RENDERING(input_textbox)->show = RENDERING(input_background)->show = true;
	#endif
					game->gameThreadContext->keyboardInputHandlerAPI->getUserInput(14);
					gameOverState = AskingPlayerName;
					game->datas->successMgr->sTheyGood(true);
				} else {
					game->datas->successMgr->sTheyGood(false);
					gameOverState = NoGame;
					submitScore("rzehtrtyBg");
				}

				TEXT_RENDERING(yourScore)->show = true;
				std::stringstream a;
				a.precision(1);
				if (game->datas->mode==Normal) {
					a << game->datas->mode2Manager[game->datas->mode]->points << " : "<< game->gameThreadContext->localizeAPI->text("lvl") << " " << static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->currentLevel();
				} else if (game->datas->mode==Go100Seconds) {
					a << game->datas->mode2Manager[game->datas->mode]->points;
				} else {
					a << std::fixed << ((int)(game->datas->mode2Manager[game->datas->mode]->time*10))/10.f << " s";
				}
				TEXT_RENDERING(yourScore)->text = a.str();
				game->datas->successMgr->sTestEverything(game->gameThreadContext->storageAPI);
				break;
			}
			case AskingPlayerName: {
				if (game->gameThreadContext->keyboardInputHandlerAPI->done(TEXT_RENDERING(input_textbox)->text)) {
					playerName = TEXT_RENDERING(input_textbox)->text;
					TEXT_RENDERING(input_textbox)->text = "";

					TEXT_RENDERING(input_label)->show = TEXT_RENDERING(input_textbox)->show = RENDERING(input_background)->show = false;

					if (game->datas->mode==Normal)
						game->datas->successMgr->sBTAC(game->gameThreadContext->storageAPI, difficulty, game->datas->mode2Manager[game->datas->mode]->points);
					else if (game->datas->mode==TilesAttack)
						game->datas->successMgr->sBTAM(game->gameThreadContext->storageAPI, difficulty, game->datas->mode2Manager[game->datas->mode]->time);
					
					submitScore(playerName);
					LoadScore(game->datas->mode, difficulty);
					gameOverState = NoGame;

					if (game->gameThreadContext->communicationAPI->mustShowRateDialog()) {
						TRANSFORM(herisson)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(herisson)->size.x;
						TEXT_RENDERING(title)->show = false;
						//this->LateExit();
						return Scene::RateIt;
					}

				}
				break;
			}
		}

		//herisson
		if (TRANSFORM(herisson)->position.x < PlacementHelper::ScreenWidth + TRANSFORM(herisson)->size.x) {
			TRANSFORM(herisson)->position.x += ANIMATION(herisson)->playbackSpeed*dt;
		} else {
			RENDERING(herisson)->show = false;
		}

		if (gameOverState != AskingPlayerName) {
			//difficulty button
			if (BUTTON(bDifficulty)->clicked) {
				SOUND(bDifficulty)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
				difficulty = theGridSystem.nextDifficulty(difficulty);

				if (difficulty == DifficultyEasy)
					TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_1") + " }";
				else if (difficulty == DifficultyMedium)
					TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_2") + " }";
				else
					TEXT_RENDERING(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_3") + " }";

				TEXT_RENDERING(playText)->text = game->gameThreadContext->localizeAPI->text("play");
				LoadScore(game->datas->mode, difficulty);
			}

			//new game button
			else if (BUTTON(playContainer)->clicked) {
				SOUND(playContainer)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
				RENDERING(herisson)->show = false;
				TRANSFORM(herisson)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(herisson)->size.x;
				TEXT_RENDERING(title)->show = false;

				std::stringstream ss;
				ss << "where mode = " << (int)game->datas->mode << " and difficulty = " << (int)difficulty;
				ScoreStorageProxy ssp;
				if (game->gameThreadContext->storageAPI->count(&ssp, "*", ss.str()) == 0) {
					// show help
					helpMgr->mode = game->datas->mode;
					helpMgr->oldState = BlackToSpawn;
					// this->LateExit();
					return Scene::Help;
				} else {
					return Scene::ModeMenuToBlackState;
				}
			}

			//back button
			else if (BUTTON(back)->clicked || pleaseGoBack) {
				pleaseGoBack = false;
				SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
				return Scene::MainMenu;
			}

			//enableSwarm button
			else if (BUTTON(enableSwarmContainer)->clicked) {
				LOGE("NOT HANDLED YET!");
				// communicationAPI->swarmRegistering(modeMgr->GetMode(), theGridSystem.sizeToDifficulty());
				TEXT_RENDERING(enableSwarm)->show = false;
				BUTTON(enableSwarmContainer)->enabled = false;
			}
		}
		return Scene::ModeMenu;
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
		theGridSystem.setGridFromDifficulty(difficulty);

		game->datas->successMgr->NewGame(difficulty);

		CONTAINER(playContainer)->enable =
			CONTAINER(bDifficulty)->enable =
			CONTAINER(enableSwarmContainer)->enable = false;
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateModeMenuSceneHandler(HeriswapGame* game) {
		return new ModeMenuScene(game);
	}
}