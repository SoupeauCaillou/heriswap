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

#include "HeriswapGame.h"

#include "CombinationMark.h"
#include "DepthLayer.h"
#include "Game_Private.h"
#include "systems/HeriswapGridSystem.h"
#include "systems/TwitchSystem.h"
#include "systems/BackgroundSystem.h"

#include "modes/NormalModeManager.h"
#include "modes/Go100SecondsModeManager.h"
#include "modes/TilesAttackModeManager.h"

#include "base/EntityManager.h"
#include "base/Log.h"
#include "base/PlacementHelper.h"
#include "base/TimeUtil.h"
#include "base/TouchInputManager.h"

#include "systems/ADSRSystem.h"
#include "systems/AnimationSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include "util/ScoreStorageProxy.h"

#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>

#include <sstream>

bool HeriswapGame::inGameState(Scene::Enum state) {
    switch (state) {
        case Scene::Spawn:
        case Scene::UserInput:
        case Scene::Delete:
        case Scene::Fall:
        case Scene::LevelChanged:
            return true;
        default:
            return false;
    }
}

bool HeriswapGame::pausableState(Scene::Enum state) {
    switch (state) {
        case Scene::Spawn:
        case Scene::UserInput:
        case Scene::Delete:
        case Scene::Fall:
        case Scene::LevelChanged:
        case Scene::Pause:
            return true;
        default:
            return false;
    }
}

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / 8;

// grid: [48, 302] -> [752, 1006]  in gimp
glm::vec2 HeriswapGame::GridCoordsToPosition(int i, int j, int gridSize) {
    float startX = PlacementHelper::GimpXToScreen(48);
    float startY = PlacementHelper::GimpYToScreen(1006);
    float size = PlacementHelper::GimpWidthToScreen((752 - 48) / gridSize);

    return glm::vec2(
        startX + (i + 0.5) * size,
        startY + (j + 0.5) * size);
}

glm::vec2 HeriswapGame::CellSize(int gridSize, int cellType) {
    float y = PlacementHelper::GimpWidthToScreen((752 - 48) / gridSize);
    float coeff = 1;
    switch (cellType) {
        case 0:
        case 1:
            coeff = 0.74 / 0.8;
            break;
        case 2:
        case 3:
            coeff = 0.62 / 0.8;
            break;
        case 4:
        case 5:
            coeff = 0.52 / 0.8;
            break;
        case 6:
        case 7:
            coeff = 0.38 / 0.8;
            break;
    }
    return glm::vec2(y * coeff, y);
}

float HeriswapGame::CellContentScale() {
    return scale;
}

HeriswapGame::HeriswapGame() : Game() {
}

HeriswapGame::~HeriswapGame() {
    HeriswapGridSystem::DestroyInstance();
    TwitchSystem::DestroyInstance();
    BackgroundSystem::DestroyInstance();
    delete datas;
}

bool HeriswapGame::wantsAPI(ContextAPI::Enum api) const {
    switch (api) {
        case ContextAPI::Ad:
        case ContextAPI::Asset:
        case ContextAPI::Communication:
        case ContextAPI::Localize:
        case ContextAPI::Music:
        case ContextAPI::KeyboardInputHandler:
        case ContextAPI::Sound:
        case ContextAPI::Storage:
        case ContextAPI::Success:
        case ContextAPI::Vibrate:
        case ContextAPI::StringInput:
            return true;
        default:
            return false;
    }
}

void HeriswapGame::quickInit(){
    sceneStateMachine.reEnterCurrentState();
}

void HeriswapGame::sacInit(int windowW, int windowH) {
    LOGI("SAC engine initialisation begins...");
    Game::sacInit(windowW, windowH);

    HeriswapGridSystem::CreateInstance();
    TwitchSystem::CreateInstance();
    BackgroundSystem::CreateInstance();

    sceneStateMachine.registerState(Scene::CountDown, Scene::CreateCountDownSceneHandler(this), "Scene::CountDown");
    sceneStateMachine.registerState(Scene::Spawn, Scene::CreateSpawnSceneHandler(this), "Scene::Spawn");
    sceneStateMachine.registerState(Scene::UserInput, Scene::CreateUserInputSceneHandler(this), "Scene::UserInput");
    sceneStateMachine.registerState(Scene::Delete, Scene::CreateDeleteSceneHandler(this), "Scene::Delete");
    sceneStateMachine.registerState(Scene::Fall, Scene::CreateFallSceneHandler(this), "Scene::Fall");
    sceneStateMachine.registerState(Scene::LevelChanged, Scene::CreateLevelChangedSceneHandler(this), "Scene::LevelChanged");
    sceneStateMachine.registerState(Scene::Pause, Scene::CreatePauseSceneHandler(this), "Scene::Pause");
    // sceneStateMachine.registerState(Scene::Unpause, Scene::CreateUnpauseSceneHandler(this), "Scene::Unpause");
    sceneStateMachine.registerState(Scene::MainMenu, Scene::CreateMainMenuSceneHandler(this), "Scene::MainMenu");
    sceneStateMachine.registerState(Scene::ModeMenu, Scene::CreateModeMenuSceneHandler(this), "Scene::ModeMenu");
    // sceneStateMachine.registerState(Scene::ScoreBoard, Scene::CreateScoreBoardSceneHandler(this), "Scene::ScoreBoard");
    // sceneStateMachine.registerState(Scene::EndMenu, Scene::CreateEndMenuSceneHandler(this), "Scene::EndMenu");
    // sceneStateMachine.registerState(Scene::Background, Scene::CreateBackgroundSceneHandler(this), "Scene::Background");
    sceneStateMachine.registerState(Scene::Logo, Scene::CreateLogoSceneHandler(this), "Scene::Logo");
    sceneStateMachine.registerState(Scene::Help, Scene::CreateHelpSceneHandler(this), "Scene::Help");
    sceneStateMachine.registerState(Scene::Ads, Scene::CreateAdsSceneHandler(this), "Scene::Ads");
    sceneStateMachine.registerState(Scene::RateIt, Scene::CreateRateItSceneHandler(this), "Scene::RateIt");
    // sceneStateMachine.registerState(Scene::ExitState, Scene::CreateExitStateSceneHandler(this), "Scene::ExitState");
    sceneStateMachine.registerState(Scene::ElitePopup, Scene::CreateElitePopupSceneHandler(this), "Scene::ElitePopup");

    sceneStateMachine.registerState(Scene::BlackToMainMenu, Scene::CreateFadeSceneHandler(this, FadingType::FadeIn, .5f, Scene::MainMenu), "Scene::BlackToMainMenu");
    sceneStateMachine.registerState(Scene::BlackToModeMenu, Scene::CreateFadeSceneHandler(this, FadingType::FadeIn, 0.5f, Scene::ModeMenu), "Scene::BlackToModeMenu");
    sceneStateMachine.registerState(Scene::BlackToSpawn, Scene::CreateFadeSceneHandler(this, FadingType::FadeIn, 0.5f, Scene::Spawn), "Scene::BlackToSpawn");
    sceneStateMachine.registerState(Scene::AdsToBlackState, Scene::CreateFadeSceneHandler(this, FadingType::FadeOut, 0.2f, Scene::BlackToSpawn), "Scene::AdsToBlackState");
    sceneStateMachine.registerState(Scene::GameToBlack, Scene::CreateFadeSceneHandler(this, FadingType::FadeOut, 0.4f, Scene::BlackToModeMenu), "Scene::GameToBlack");
    sceneStateMachine.registerState(Scene::ModeMenuToBlackState, Scene::CreateFadeSceneHandler(this, FadingType::FadeOut, 0.2f, Scene::Ads), "Scene::ModeMenuToBlackState");

    //init database
    gameThreadContext->storageAPI->init(gameThreadContext->assetAPI, "Heriswap");
    gameThreadContext->storageAPI->setOption("sound", std::string(), "on");
    gameThreadContext->storageAPI->setOption("gameCount", std::string(), "0");

    Color::nameColor(Color(3.0/255.0, 99.0/255, 71.0/255), "green");

    theRenderingSystem.effectLibrary.load("desaturate.fs");

    // Animations
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_1", "herisson_1");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_2", "herisson_2");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_3", "herisson_3");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_4", "herisson_4");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_5", "herisson_5");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_6", "herisson_6");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_7", "herisson_7");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_8", "herisson_8");

    theButtonSystem.vibrateAPI = gameThreadContext->vibrateAPI;

    // default camera
    camera = theEntityManager.CreateEntity("camera",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("camera"));

    LOGI("SAC engine initialisation done.");
}

void HeriswapGame::init(const uint8_t* in, int size) {
    LOGI("HeriswapGame initialisation begins...");
    if (in && size) {
        in = loadEntitySystemState(in, size);
    }
    ScoreStorageProxy ssp;
    gameThreadContext->storageAPI->createTable(&ssp);

    SuccessManager *sm = new SuccessManager(gameThreadContext->successAPI);
    datas = new PrivateData(this, gameThreadContext, sm);

    datas->Setup();

    theSoundSystem.mute = !datas->storageAPI->isOption("sound", "on");
    theMusicSystem.toggleMute(theSoundSystem.mute);

    datas->sky = theEntityManager.CreateEntity("sky",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/sky"));
    
    SCROLLING(datas->sky)->images.push_back("ciel0");
    SCROLLING(datas->sky)->images.push_back("ciel1");
    SCROLLING(datas->sky)->images.push_back("ciel2");
    SCROLLING(datas->sky)->images.push_back("ciel3");

    SCROLLING(datas->sky)->displaySize = glm::vec2(TRANSFORM(datas->sky)->size.x * 1.01,
                                                   TRANSFORM(datas->sky)->size.y);

    if (in && size) {
        // datas->state = Pause;
        // loadGameState(in, size);
    }

    sceneStateMachine.setup(Scene::Logo);

    quickInit();

    LOGI("HeriswapGame initialisation done.");
}

void HeriswapGame::setMode() {
    // datas->state2Manager[Delete]->modeMgr = datas->mode2Manager[datas->mode];
    // datas->state2Manager[ModeMenu]->modeMgr = datas->mode2Manager[datas->mode];
    // datas->state2Manager[Spawn]->modeMgr = datas->mode2Manager[datas->mode];
    // static_cast<CountDownStateManager*> (datas->state2Manager[CountDown])->mode = datas->mode;
    // if (datas->mode == Normal || datas->mode == Go100Seconds) {
    //  static_cast<FadeGameStateManager*> (datas->state2Manager[GameToBlack])->duration = 4.0f;
    // } else {
    //  static_cast<FadeGameStateManager*> (datas->state2Manager[GameToBlack])->duration = 0.5f;
    // }
}

void HeriswapGame::toggleShowCombi(bool enabled) {
    static bool activated;
    static std::vector<Entity> marks;
    //on switch le bool
    activated = !activated;
    if (!enabled || (sceneStateMachine.getCurrentState() != Scene::UserInput))
        activated = false;

    if (activated) {
        LOGI("Affiche magique de la triche !") ;
        //j=0 : vertical
        //j=1 : h
        for (int j=0;j<2;j++) {
            std::vector<glm::vec2> combinaisons;
            if (j) combinaisons = theHeriswapGridSystem.LookForCombinationsOnSwitchHorizontal();
            else combinaisons = theHeriswapGridSystem.LookForCombinationsOnSwitchVertical();
            if (!combinaisons.empty())
            {
                for ( std::vector<glm::vec2>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
                {
                    CombinationMark::markCellInCombination(theHeriswapGridSystem.GetOnPos(it->x, it->y));
                    marks.push_back(theHeriswapGridSystem.GetOnPos(it->x, it->y));
                    CombinationMark::markCellInCombination(theHeriswapGridSystem.GetOnPos(it->x+(j+1)/2, it->y+(j+1)%2));
                    marks.push_back(theHeriswapGridSystem.GetOnPos(it->x+(j+1)/2, it->y+(j+1)%2));
                }
            }
        }


    //rajout de 2 marques sur les elements a switch
    } else {
        if (marks.size()>0) {
            for (unsigned int i=0; i<marks.size(); i++) {
                CombinationMark::clearCellInCombination(marks[i]);
            }
            marks.clear();
            LOGI("Destruction des marquages et de la triche !");
        }
    }

}

void HeriswapGame::backPressed() {
    Game::backPressed();

    const Scene::Enum state = sceneStateMachine.getCurrentState();
    if (state == Scene::ModeMenu) {
        // go back to main menu
        // (static_cast<ModeMenuStateManager*>(datas->state2Manager[ModeMenu]))->pleaseGoBack = true;
    } else if (pausableState(state)) {
        #if SAC_DEBUG
        // datas->mode2Manager[datas->mode]->toggleDebugDisplay();
        #else
        togglePause(true);
        #endif
    }
}

void HeriswapGame::togglePause(bool activate) {
    LOGT("TODO: Must be called from Game thread");

    const Scene::Enum currentState = sceneStateMachine.getCurrentState();
    if (activate && currentState != Scene::Pause && pausableState(currentState)) {

        // pause
        sceneStateMachine.forceNewState(Scene::Pause);
    }
}

void HeriswapGame::tick(float dt) {
    PROFILE("Game", "Tick", BeginEvent);
    sceneStateMachine.update(dt);
    // update state
    //datas->newState = datas->state2Manager[datas->state]->Update(dt);

    //update only if game has really begun (after countdown)
    if (sceneStateMachine.getCurrentState() != Scene::CountDown && !datas->newGame) {
        //updating gamemode
        datas->mode2Manager[datas->mode]->GameUpdate(dt, sceneStateMachine.getCurrentState());
    }

    LOGT_EVERY_N(600, "TBD in Pause (prolly)");
#if 0
    //si on est passé de pause à quelque chose different de pause, on desactive la pause
    if (datas->state == Pause && datas->newState == Unpause) {
        togglePause(false);
    }
#endif

    LOGT_EVERY_N(600, "State change action must be done in states respective onEnter/onExit methods");
#if 0
    //si on a change d'etat
     else if (datas->newState != datas->state) {
        stateChanged(datas->state, datas->newState);

        if (datas->newState == ExitState)
            return;

        // datas->state2Manager[datas->state]->Exit();
        datas->state = datas->newState;
        // datas->state2Manager[datas->state]->Enter();

        #if SAC_ANDROID
        bool ofHidden = (datas->newState != MainMenu && datas->newState != ModeMenu);
        #else
        bool ofHidden = true;
        #endif
        RENDERING(datas->socialGamNet)->show = !ofHidden;
        BUTTON(datas->socialGamNet)->enabled = RENDERING(datas->socialGamNet)->show;
    }
#endif

    // background (unconditionnal) update of state managers
    // for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin();
    //  it!=datas->state2Manager.end();
    //  ++it) {
    //  it->second->BackgroundUpdate(dt);
    // }

    //si on appuye sur le bouton mute
    if (BUTTON(datas->soundButton)->clicked) {
        BUTTON(datas->soundButton)->clicked = false;

        bool isEnable = datas->storageAPI->isOption("sound", "on");
        datas->storageAPI->setOption("sound", isEnable ? "off" : "on", "on");

        theSoundSystem.mute = !theSoundSystem.mute;
        theMusicSystem.toggleMute(theSoundSystem.mute);
        if (!theSoundSystem.mute) {
            SOUND(datas->soundButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on");
        } else {
            RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off");
        }
    }

    //if socialGamNet is clicked
    if (BUTTON(datas->socialGamNet)->clicked){
        if (sceneStateMachine.getCurrentState() == Scene::ModeMenu) {
            LOGT("TBD");
            // Difficulty diff = (static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu]))->difficulty;
            // successAPI->openLeaderboard(datas->mode, diff);
        } else {
            successAPI->openDashboard();
        }
    }

    //updating HUD if playing
    if (inGameState(sceneStateMachine.getCurrentState()) && sceneStateMachine.getCurrentState() != Scene::LevelChanged) {
        datas->mode2Manager[datas->mode]->UiUpdate(dt);
    }

    if (theTouchInputManager.wasTouched(0) && theMusicSystem.isMuted() && !theSoundSystem.mute) {
        // restore music
        theMusicSystem.toggleMute(false);
    }

    //update music
    if (!theMusicSystem.isMuted()) {
        const Scene::Enum state = sceneStateMachine.getCurrentState();
        //si on est en jeu et/ou  fin de musiques, charger de nouvelles musiques
        if ((pausableState(state) &&
            state != Scene::LevelChanged &&
            state != Scene::Pause) || state == Scene::BlackToSpawn) {
            MUSIC(datas->inGameMusic.masterTrack)->control = MusicControl::Play;
            MUSIC(datas->inGameMusic.masterTrack)->volume = 1;
            MUSIC(datas->inGameMusic.stressTrack)->control = (datas->mode == Normal) ? MusicControl::Play : MusicControl::Stop;
            if (MUSIC(datas->inGameMusic.masterTrack)->music == InvalidMusicRef) {
                MUSIC(datas->inGameMusic.stressTrack)->music = (datas->mode == Normal) ? theMusicSystem.loadMusicFile("audio/F.ogg") : InvalidMusicRef;
                std::vector<std::string> musics = datas->jukebox.pickNextSongs(4);
                LOGW("New music picked for 'music' field '" << musics.size() <<"' :");
                for (unsigned i=0; i<musics.size(); i++) {
                    LOGW("(music)\t'" << musics[i].c_str() << "'");
                }

                MUSIC(datas->inGameMusic.masterTrack)->music = theMusicSystem.loadMusicFile(musics[0]);
                MUSIC(datas->inGameMusic.masterTrack)->fadeIn = 1;

                unsigned int i;
                for (i=0; i<musics.size() - 1; i++) {
                     MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
                     mc->music = theMusicSystem.loadMusicFile(musics[i+1]);
                     mc->fadeIn = 1;
                     mc->control = MusicControl::Play;
                     mc->volume = 1;
                }
            }
            // if master track has looped, choose next songs to play
            else if (MUSIC(datas->inGameMusic.masterTrack)->loopNext == InvalidMusicRef) {
                MUSIC(datas->inGameMusic.stressTrack)->loopNext = (datas->mode == Normal) ? theMusicSystem.loadMusicFile("audio/F.ogg") : InvalidMusicRef;
                std::vector<std::string> musics = datas->jukebox.pickNextSongs(4);
                LOGW("New music picked for 'loopNext' field '" << musics.size() << "' :");
                for (unsigned i=0; i<musics.size(); i++) {
                    LOGW("(music)\t'" << musics[i].c_str() << "'");
                }

                MUSIC(datas->inGameMusic.masterTrack)->loopNext = theMusicSystem.loadMusicFile(musics[0]);
                unsigned int i;
                for (i=0; i<musics.size() - 1; i++) {
                    MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
                    mc->loopNext = theMusicSystem.loadMusicFile(musics[i+1]);
                    mc->control = MusicControl::Play;
                }
            } else {
                static MusicRef a = MUSIC(datas->inGameMusic.masterTrack)->loopNext;
                if (MUSIC(datas->inGameMusic.masterTrack)->loopNext != a) {
                    LOGI("music: master loopnext: '" << MUSIC(datas->inGameMusic.masterTrack)->loopNext << "'");
                    a = MUSIC(datas->inGameMusic.masterTrack)->loopNext;
                }
            }
            MUSIC(datas->inGameMusic.stressTrack)->volume = (datas->mode == Normal) ? ADSR(datas->inGameMusic.stressTrack)->value : 0;
            MUSIC(datas->menu)->control = MusicControl::Stop;

        } else if (state == Scene::MainMenu || state == Scene::ModeMenu) { //dans les menus
            if (MUSIC(datas->menu)->music == InvalidMusicRef) {
                LOGW("Start Menu music");
                MUSIC(datas->menu)->music = theMusicSystem.loadMusicFile("audio/musique_menu.ogg");
                MUSIC(datas->menu)->control = MusicControl::Play;
                MUSIC(datas->menu)->volume = 1;
            }

            if (MUSIC(datas->menu)->loopNext == InvalidMusicRef) {
                MUSIC(datas->menu)->loopNext = theMusicSystem.loadMusicFile("audio/musique_menu.ogg");
            }
        }
    }

    PROFILE("Game", "Tick", EndEvent);

    // systems update
    theHeriswapGridSystem.Update(dt);
    theTwitchSystem.Update(dt);
    theBackgroundSystem.Update(dt);
}

int HeriswapGame::saveState(uint8_t** out) {
    LOGT("TODO");
    return 0;
#if 0
    if (datas->state == Help) {
        // datas->state2Manager[datas->state]->Exit();
        // datas->state = static_cast<HelpStateManager*>(datas->state2Manager[datas->state])->oldState;//Pause;
        if (datas->state == Scene::BlackToSpawn) {
            datas->state = Scene::MainMenu;
        }
        // datas->state2Manager[datas->state]->Enter();
    }
    bool pausable = pausableState(datas->state);
    if (!pausable) {
        LOGI("Current state is '" << datas->state << "' -> nothing to save");
        return 0;
    }

    if (datas->state == LevelChanged) {
        // datas->state2Manager[datas->state]->Exit();
        datas->state = Spawn;
        datas->mode2Manager[datas->mode]->generateLeaves(0, theHeriswapGridSystem.Types);
    }

    /* save all entities/components */
    uint8_t* entities = 0;
    int eSize = theEntityManager.serialize(&entities);

    /* save System with assets ? (texture name -> texture ref map of RenderingSystem ?) */
    uint8_t* systems = 0;
    int sSize = theRenderingSystem.saveInternalState(&systems);

    /* save Game mode */
    uint8_t* gamemode = 0;
    int gSize = datas->mode2Manager[datas->mode]->saveInternalState(&gamemode);

    int finalSize = sizeof(datas->state) + sizeof(datas->mode) + sizeof(theHeriswapGridSystem.GridSize) + sizeof(eSize) + sizeof(sSize) + eSize + sSize + gSize;
    *out = new uint8_t[finalSize];
    uint8_t* ptr = *out;

    /* save entity/system thingie */
    ptr = (uint8_t*)mempcpy(ptr, &eSize, sizeof(eSize));
    ptr = (uint8_t*)mempcpy(ptr, &sSize, sizeof(sSize));
    ptr = (uint8_t*)mempcpy(ptr, entities, eSize);
    ptr = (uint8_t*)mempcpy(ptr, systems, sSize);

    /* save Game fields */
    if (datas->state == Pause) {
        ptr = (uint8_t*)mempcpy(ptr, &datas->stateBeforePause, sizeof(datas->state));
    } else {
        ptr = (uint8_t*)mempcpy(ptr, &datas->state, sizeof(datas->state));
    }

    ptr = (uint8_t*)mempcpy(ptr, &datas->mode, sizeof(datas->mode));
    ptr = (uint8_t*)mempcpy(ptr, &theHeriswapGridSystem.GridSize, sizeof(theHeriswapGridSystem.GridSize));
    ptr = (uint8_t*)mempcpy(ptr, gamemode, gSize);

    LOGI("'" << sizeof(datas->stateBeforePause) << "' + '"<< sizeof(datas->mode) << "' + '" << sizeof(eSize) << "' + '" << sizeof(sSize) << "' + '" << eSize << "' + '" << sSize << "' + '" << gSize << "' -> '" << finalSize << "' ('" << *out << "')");

    return finalSize;
#endif
}

const uint8_t* HeriswapGame::loadEntitySystemState(const uint8_t* in, int) {
    /* restore Game fields */
    int eSize, sSize, index=0;
    memcpy(&eSize, &in[index], sizeof(eSize));
    index += sizeof(eSize);
    memcpy(&sSize, &in[index], sizeof(sSize));
    index += sizeof(sSize);
    /* restore entities */
    theEntityManager.deserialize(&in[index], eSize);
    index += eSize;
    /* restore systems */
    theRenderingSystem.restoreInternalState(&in[index], sSize);
    index += sSize;
    return &in[index];
}

void HeriswapGame::loadGameState(const uint8_t* in, int size) {
    LOGT("TODO");
#if 0
    /* restore Game fields */
    memcpy(&datas->stateBeforePause, in, sizeof(datas->stateBeforePause));
    datas->state = datas->stateBeforePause;
    in += sizeof(datas->stateBeforePause);
    memcpy(&datas->mode, in, sizeof(datas->mode));
    in += sizeof(datas->mode);
    memcpy(&theHeriswapGridSystem.GridSize, in, sizeof(theHeriswapGridSystem.GridSize));
    theHeriswapGridSystem.Types = theHeriswapGridSystem.GridSize; //utiliser gridParamFromDifficulty nn ?
    in += sizeof(theHeriswapGridSystem.GridSize);

    datas->mode2Manager[datas->mode]->Enter();
    datas->mode2Manager[datas->mode]->restoreInternalState(in, size);
    datas->mode2Manager[datas->mode]->UiUpdate(0);

    setMode();
    togglePause(true);
    datas->stateBeforePauseNeedEnter = true;

    // MainMenuGameStateManager* mgsm = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]);
    // static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title =
        // mgsm->modeTitleToReset = mgsm->eStart[datas->mode];

    setupGameProp();

    RENDERING(datas->soundButton)->show = true;
    SCROLLING(datas->sky)->show = true;
    LOGW("RESTORED STATE: '" << datas->stateBeforePause << "'");
#endif
}

static float rotations[] = {
    glm::quarter_pi<float>(),
    -glm::half_pi<float>(),
    0.0f,
    -3 * glm::quarter_pi<float>(),
    3 * glm::quarter_pi<float>(),
    glm::half_pi<float>(),
    -glm::pi<float>(),
    -glm::quarter_pi<float>()
};

std::string HeriswapGame::cellTypeToTextureNameAndRotation(int type, float* rotation) {
    if (rotation)
        *rotation = rotations[type];

    std::stringstream s;
    s << "feuille" << (type+1);
    return s.str();
}

float HeriswapGame::cellTypeToRotation(int type) {
    return rotations[type];
}

bool HeriswapGame::shouldPlayPiano() {
    // are we near to beat the next score ?
    if (datas->scoreboardRankInSight == 0 || datas->mode != Normal)
        return false;
    int target = datas->bestScores[datas->scoreboardRankInSight - 1];
    if (target == 0)
        return false;
    int score = datas->mode2Manager[datas->mode]->points;
    float v = (score / (float)target);

    LOGW("SCORE TARGET: '" << target << "' (current: '" << score << "', rank: '" << datas->scoreboardRankInSight << "') -> '" << v << "'");
    if (v >= 0.95) {

        // play piano
        datas->scoreboardRankInSight--;
        return true;
    }
    return false;
}

void HeriswapGame::stopInGameMusics() {
    MUSIC(datas->inGameMusic.masterTrack)->control = MusicControl::Stop;
    MUSIC(datas->inGameMusic.accessoryTrack)->control = MusicControl::Stop;
    MUSIC(datas->inGameMusic.stressTrack)->control = MusicControl::Stop;
    for(int i=0; i<3; i++) {
       MUSIC(datas->inGameMusic.secondaryTracks[i])->control = MusicControl::Stop;
    }
}

void HeriswapGame::setupGameProp() {
    if (datas->mode == Go100Seconds) {
        datas->timing.deletion = 0.2;
        datas->timing.swap = 0.03;
        datas->timing.fall = 0.1;
        datas->timing.haveToAddLeavesInGrid = 0.2;
        datas->timing.replaceGrid = 0.5;
    } else {
        //update anim times
        Difficulty difficulty = theHeriswapGridSystem.sizeToDifficulty();
        if (difficulty == DifficultyEasy) {
            datas->timing.deletion = 0.6;
            datas->timing.swap = 0.14;
            datas->timing.fall = 0.30;
            datas->timing.haveToAddLeavesInGrid = 0.40;
            datas->timing.replaceGrid = 1;
        } else {
            datas->timing.deletion = 0.3;
            datas->timing.swap = 0.07;
            datas->timing.fall = 0.15;
            datas->timing.haveToAddLeavesInGrid = 0.40;
            datas->timing.replaceGrid = 1;
        }
    }


    std::stringstream ss;
    ss << "where mode = " << datas->mode << " and difficulty = " << theHeriswapGridSystem.sizeToDifficulty();
    if (datas->mode == TilesAttack) ss << " order by time asc limit 5";
    else ss << " order by points desc limit 5";

    ScoreStorageProxy ssp;
    datas->storageAPI->loadEntries(&ssp, "*", ss.str());

    datas->bestScores.clear();
    datas->bestScores.reserve(ssp._queue.size());
    datas->scoreboardRankInSight = ssp._queue.size();

    for (unsigned i = 0; i < ssp._queue.size(); ++i) {
        datas->bestScores[i] = ssp._queue.back().points;
        ssp.popAnElement();
    }

}

void HeriswapGame::prepareNewGame() {
    //for count down in 2nd mode
    datas->newGame = true;
    // call Enter before starting fade-in
    datas->mode2Manager[datas->mode]->Enter();

    LOGT("UiUpdate first call missing");
    // datas->mode2Manager[datas->mode]->UiUpdate(0);
    MUSIC(datas->menu)->control = MusicControl::Stop;
}
