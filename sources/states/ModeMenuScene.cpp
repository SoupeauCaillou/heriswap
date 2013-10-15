/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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

#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "systems/HeriswapGridSystem.h"

#include "api/StringInputAPI.h"
#include "api/StorageAPI.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"
#include "modes/Go100SecondsModeManager.h"

#include "DepthLayer.h"

#include "base/EntityManager.h"
#include "base/Log.h"
#include "base/ObjectSerializer.h"
#include "base/PlacementHelper.h"
#include "base/TouchInputManager.h"

#include "systems/AnimationSystem.h"
#include "systems/AutoDestroySystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include "util/ScoreStorageProxy.h"

#include <glm/glm.hpp>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

struct ModeMenuScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    Difficulty difficulty;

    bool pleaseGoBack;
    // HelpStateManager* helpMgr;

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
            scoresName[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/score_name"));
            a.str("");
            a << "scoresPoints_" << i;
            scoresPoints[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/score_points"));
            a.str("");
            a << "scoresLevel_" << i;
            scoresLevel[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/score_level"));

            TRANSFORM(scoresName[i])->position.y =
                TRANSFORM(scoresPoints[i])->position.y =
                    TRANSFORM(scoresLevel[i])->position.y = (float)PlacementHelper::GimpYToScreen(605 + i * 95);
        }
        // back button
        back = theEntityManager.CreateEntity("back",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/back_button"));

        // score title
        scoreTitle = theEntityManager.CreateEntity("scoreTitle",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/score_title"));
        TEXT(scoreTitle)->text = game->gameThreadContext->localizeAPI->text("score");

        // score title
        average = theEntityManager.CreateEntity("average",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/average"));

        // play text
        playText = theEntityManager.CreateEntity("playText",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/play_text"));
        TEXT(playText)->text = game->gameThreadContext->localizeAPI->text("play");

        // play button
        playContainer = theEntityManager.CreateEntity("playContainer",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/play_container"));
        CONTAINER(playContainer)->entities.push_back(playText);

        //difficulty text
        eDifficulty = theEntityManager.CreateEntity("eDifficulty",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/difficulty_text"));

        //difficulty container
        bDifficulty = theEntityManager.CreateEntity("bDifficulty",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/difficulty_button"));
        CONTAINER(bDifficulty)->entities.push_back(eDifficulty);

        // your score
        yourScore = theEntityManager.CreateEntity("yourScore",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/your_score"));

        // fond
        fond = theEntityManager.CreateEntity("background",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/background"));

        // enableSwarm text
        enableSwarm = theEntityManager.CreateEntity("enableSwarm",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/enable_swarm"));
        TEXT(enableSwarm)->text = game->gameThreadContext->localizeAPI->text("get_googleplus");

        // enableSwarm container
        enableSwarmContainer = theEntityManager.CreateEntity("enableSwarmContainer",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/swarm_container"));
        CONTAINER(enableSwarmContainer)->entities.push_back(enableSwarm);


    #if ! SAC_MOBILE
        // name input entities
        input_label = theEntityManager.CreateEntity("input_label",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/input_label"));
        TEXT(input_label)->text = game->gameThreadContext->localizeAPI->text("enter_name");

        input_textbox = theEntityManager.CreateEntity("input_textbox",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/input_textbox"));
        TEXT(input_textbox)->caret.speed = 0.5;

        input_background = theEntityManager.CreateEntity("input_background",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("modemenu/input_background"));
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
            TextComponent* trcN = TEXT(scoresName[i]);
            TextComponent* trcP = TEXT(scoresPoints[i]);
            TextComponent* trcL = TEXT(scoresLevel[i]);
            if (i < entries.size()) {
                trcN->show = true;
                trcP->show = true;
                std::stringstream a;
                a.precision(1);
                if (mode==Normal || mode==Go100Seconds) {
                    a << std::fixed << entries[i].points;
                    trcP->flags |= TextComponent::IsANumberBit;
                } else {
                    a << std::fixed << entries[i].time << " s";
                    trcP->flags &= ~TextComponent::IsANumberBit;
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
            TEXT(average)->text = a.str();
            TEXT(average)->show = true;
        } else {
            TEXT(average)->show = false;
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

        // This is the leaderboards order:
        //      EScoreRaceEasy = 0,
        //      EScoreRaceDifficult,
        //      EScoreRaceMedium,
        //      E100SecondsEasy,
        //      E100SecondsDifficult,
        //      E100SecondsMedium,
        //      ETimeAttackEasy,
        //      ETimeAttackDifficult,
        //      ETimeAttackMedium,

        int e = (game->datas->mode * 3 + difficulty);
        std::string scoreS = (game->datas->mode == TilesAttack) ? ssp.getValue("time") : ssp.getValue("points");
        game->gameThreadContext->gameCenterAPI->submitScore(e, scoreS);
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
    void onPreEnter(Scene::Enum from) override {
        // if coming from game
        if (from == Scene::EndGame) {
            // exit game
            game->datas->mode2Manager[game->datas->mode]->Exit();
            // setup fadeout
            game->datas->faderHelper.start(Fading::In, 1.5f);
        }

        pleaseGoBack = false;
        game->datas->successMgr->sHardScore(game->gameThreadContext->storageAPI);

        BUTTON(back)->enabled = true;
        BUTTON(playContainer)->enabled = true;

        switch (from) {
            case Scene::MainMenu:
            case Scene::RateIt:
                gameOverState = NoGame;
                break;
            default:
                gameOverState = GameEnded;
                break;
        }

        //first launch : set an easiest diff
        if (gameOverState == NoGame && from == Scene::MainMenu) {
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
            difficulty = theHeriswapGridSystem.sizeToDifficulty();
        }

        LoadScore(game->datas->mode, difficulty);

        RENDERING(back)->show =
            RENDERING(game->herisson)->show =
            RENDERING(game->menubg)->show =
            RENDERING(fond)->show =
            RENDERING(game->menufg)->show = true;

        TEXT(game->title)->show =
            TEXT(scoreTitle)->show =
            TEXT(eDifficulty)->show =
            TEXT(enableSwarm)->show =
            TEXT(playText)->show = true;

        BUTTON(bDifficulty)->enabled = true;

        CONTAINER(playContainer)->enable =
            CONTAINER(enableSwarmContainer)->enable =
            CONTAINER(bDifficulty)->enable = true;

        SCROLLING(game->datas->sky)->show = true;

        TEXT(playText)->text = (gameOverState != NoGame) ? game->gameThreadContext->localizeAPI->text("restart") : game->gameThreadContext->localizeAPI->text("play");

        if (difficulty == DifficultyEasy)
            TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_1") + " }";
        else if (difficulty == DifficultyMedium)
            TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_2") + " }";
        else
            TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_3") + " }";

        CONTAINER(playContainer)->enable = CONTAINER(bDifficulty)->enable = true;

        #if SAC_ANDROID
        if (gameOverState == NoGame) {
            if (! game->gameThreadContext->gameCenterAPI->isRegistered()) {
                BUTTON(enableSwarmContainer)->enabled = true;
                TEXT(enableSwarm)->show = true;
                CONTAINER(enableSwarmContainer)->enable = true;
            }
        }
        #endif
    }

    bool updatePreEnter(Scene::Enum from, float dt) {
        if (from == Scene::EndGame) {
            return game->datas->faderHelper.update(dt);
        } else {
            return true;
        }
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
                    TEXT(input_label)->show = TEXT(input_textbox)->show = RENDERING(input_background)->show = true;
    #endif
                    game->gameThreadContext->stringInputAPI->askUserInput("", 14);
                    gameOverState = AskingPlayerName;
                    game->datas->successMgr->sTheyGood(true);
                } else {
                    game->datas->successMgr->sTheyGood(false);
                    gameOverState = NoGame;
                    submitScore("rzehtrtyBg");
                }

                TEXT(yourScore)->show = true;
                std::stringstream a;
                a.precision(1);
                if (game->datas->mode==Normal) {
                    a << game->datas->mode2Manager[game->datas->mode]->points << " : "<< game->gameThreadContext->localizeAPI->text("lvl") << " " << static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->currentLevel();
                } else if (game->datas->mode==Go100Seconds) {
                    a << game->datas->mode2Manager[game->datas->mode]->points;
                } else {
                    a << std::fixed << ((int)(game->datas->mode2Manager[game->datas->mode]->time*10))/10.f << " s";
                }
                TEXT(yourScore)->text = a.str();
                game->datas->successMgr->sTestEverything(game->gameThreadContext->storageAPI);
                break;
            }
            case AskingPlayerName: {
#if SAC_MOBILE
                if (game->gameThreadContext->stringInputAPI->done(playerName)) {
#else
                if (game->gameThreadContext->stringInputAPI->done(TEXT(input_textbox)->text)) {
                    playerName = TEXT(input_textbox)->text;
                    TEXT(input_textbox)->text = "";

                    TEXT(input_label)->show = TEXT(input_textbox)->show = RENDERING(input_background)->show = false;
#endif
                    if (game->datas->mode==Normal)
                        game->datas->successMgr->sBTAC(game->gameThreadContext->storageAPI, difficulty, game->datas->mode2Manager[game->datas->mode]->points);
                    else if (game->datas->mode==TilesAttack)
                        game->datas->successMgr->sBTAM(game->gameThreadContext->storageAPI, difficulty, game->datas->mode2Manager[game->datas->mode]->time);

                    submitScore(playerName);
                    LoadScore(game->datas->mode, difficulty);
                    gameOverState = NoGame;

                    if (game->gameThreadContext->communicationAPI->mustShowRateDialog()) {
                        TRANSFORM(game->herisson)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(game->herisson)->size.x;
                        TEXT(game->title)->show = false;
                        return Scene::RateIt;
                    }

                }
                break;
            }
            default:
                break;
        }

        //herisson
        if (theRenderingSystem.isVisible(game->herisson)) {
            TRANSFORM(game->herisson)->position.x += ANIMATION(game->herisson)->playbackSpeed*dt;
        } else {
            RENDERING(game->herisson)->show = false;
        }

        if (gameOverState != AskingPlayerName) {
            //difficulty button
            if (BUTTON(bDifficulty)->clicked) {
                SOUND(bDifficulty)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                difficulty = theHeriswapGridSystem.nextDifficulty(difficulty);

                if (difficulty == DifficultyEasy)
                    TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_1") + " }";
                else if (difficulty == DifficultyMedium)
                    TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_2") + " }";
                else
                    TEXT(eDifficulty)->text = "{ " + game->gameThreadContext->localizeAPI->text("diff_3") + " }";

                TEXT(playText)->text = game->gameThreadContext->localizeAPI->text("play");
                LoadScore(game->datas->mode, difficulty);
            }

            //new game button
            else if (BUTTON(playContainer)->clicked) {
                SOUND(playContainer)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");

                std::stringstream ss;
                ss << "where mode = " << (int)game->datas->mode << " and difficulty = " << (int)difficulty;
                ScoreStorageProxy ssp;
                if (game->gameThreadContext->storageAPI->count(&ssp, "*", ss.str()) == 0) {
                    return Scene::Help;
                } else {
                    return Scene::CountDown;
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
                // communicationAPI->swarmRegistering(modeMgr->GetMode(), theHeriswapGridSystem.sizeToDifficulty());
            }
        }
        return Scene::ModeMenu;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum to) override {
        if (to != Scene::MainMenu && to != Scene::Help) {
            game->datas->faderHelper.start(Fading::Out, 0.5);
        }
    }

    bool updatePreExit(Scene::Enum to, float dt) override {
        switch (to) {
            case Scene::MainMenu:
            case Scene::Help:
                return true;
            default:
                return game->datas->faderHelper.update(dt);
        }
    }

    void onExit(Scene::Enum nextState) override {
        if (nextState != Scene::MainMenu) {
            theHeriswapGridSystem.setGridFromDifficulty(difficulty);
            game->datas->successMgr->NewGame(difficulty);
            TRANSFORM(game->herisson)->position.x = (float)PlacementHelper::GimpXToScreen(0)-TRANSFORM(game->herisson)->size.x;

            RENDERING(game->herisson)->show =
                RENDERING(game->menubg)->show =
                RENDERING(game->datas->socialGamNet)->show =
                RENDERING(game->menufg)->show = false;

            TEXT(game->title)->show = false;

            BUTTON(game->datas->socialGamNet)->enabled = false;

        #if SAC_ANDROID
            BUTTON(enableSwarmContainer)->enabled = false;
            TEXT(enableSwarm)->show = false;
            CONTAINER(enableSwarmContainer)->enable = false;
        #endif
        }

        for (int i = 0; i<5; ++i) {
            TEXT(scoresPoints[i])->show =
                TEXT(scoresName[i])->show =
                TEXT(scoresLevel[i])->show = false;
        }

        RENDERING(back)->show =
            RENDERING(fond)->show = false;

        TEXT(scoreTitle)->show =
            TEXT(eDifficulty)->show =
            TEXT(yourScore)->show =
            TEXT(average)->show =
            TEXT(enableSwarm)->show =
            TEXT(playText)->show = false;

        BUTTON(bDifficulty)->enabled = false;

        CONTAINER(playContainer)->enable =
            CONTAINER(enableSwarmContainer)->enable =
            CONTAINER(bDifficulty)->enable = false;


    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateModeMenuSceneHandler(HeriswapGame* game) {
        return new ModeMenuScene(game);
    }
}
