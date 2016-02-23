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



#include "HeriswapGame.h"

#include "CombinationMark.h"
#include "DepthLayer.h"
#include "Game_Private.h"
#include "util/ScoreStorageProxy.h"
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
#include "base/StateMachine.inl"

#include "systems/ADSRSystem.h"
#include "systems/AnimationSystem.h"
#include "systems/AutoDestroySystem.h"
#include "systems/AutonomousAgentSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/DebuggingSystem.h"
#include "systems/GraphSystem.h"
#include "systems/GridSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/ZSQDSystem.h"

#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>

#include <sstream>

#if SAC_INGAME_EDITORS && SAC_DEBUG
#include "util/HeriswapDebugConsole.h"
#endif

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
            return true;
        default:
            return false;
    }
}

static const float offset = 0.2f;
static const float scale = 0.95f;
static const float size = (10 - 2 * offset) / 8.f;

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
            coeff = 0.74f / 0.8f;
            break;
        case 2:
        case 3:
            coeff = 0.62f / 0.8f;
            break;
        case 4:
        case 5:
            coeff = 0.52f / 0.8f;
            break;
        case 6:
        case 7:
            coeff = 0.38f / 0.8f;
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
        case ContextAPI::Asset:
        case ContextAPI::Communication:
#if SAC_USE_PROPRIETARY_PLUGINS || SAC_DESKTOP
        case ContextAPI::GameCenter:
        case ContextAPI::InAppPurchase:
#endif
        case ContextAPI::KeyboardInputHandler:
        case ContextAPI::Localize:
        case ContextAPI::Music:
        case ContextAPI::OpenURL:
        case ContextAPI::Sound:
        case ContextAPI::Storage:
        case ContextAPI::StringInput:
        case ContextAPI::Vibrate:
            return true;
        default:
            return false;
    }
}

void HeriswapGame::quickInit(){
//    sceneStateMachine.reEnterCurrentState();
}

void HeriswapGame::sacInit(int windowW, int windowH) {
    LOGI("SAC engine initialisation begins:");
    Game::sacInit(windowW, windowH);

    LOGI("\t- Create Heriswap specific systems...");
    HeriswapGridSystem::CreateInstance();
    TwitchSystem::CreateInstance();
    BackgroundSystem::CreateInstance();

    LOGI("\t- Init sceneStateMachine...");
    sceneStateMachine.registerState(Scene::CountDown, Scene::CreateCountDownSceneHandler(this));
    sceneStateMachine.registerState(Scene::Spawn, Scene::CreateSpawnSceneHandler(this));
    sceneStateMachine.registerState(Scene::UserInput, Scene::CreateUserInputSceneHandler(this));
    sceneStateMachine.registerState(Scene::Delete, Scene::CreateDeleteSceneHandler(this));
    sceneStateMachine.registerState(Scene::Fall, Scene::CreateFallSceneHandler(this));
    sceneStateMachine.registerState(Scene::LevelChanged, Scene::CreateLevelChangedSceneHandler(this));
    sceneStateMachine.registerState(Scene::Pause, Scene::CreatePauseSceneHandler(this));
    sceneStateMachine.registerState(Scene::MainMenu, Scene::CreateMainMenuSceneHandler(this));
    sceneStateMachine.registerState(Scene::ModeMenu, Scene::CreateModeMenuSceneHandler(this));
    sceneStateMachine.registerState(Scene::EndGame, Scene::CreateEndGameSceneHandler(this));
    sceneStateMachine.registerState(Scene::Logo, Scene::CreateLogoSceneHandler(this));
    sceneStateMachine.registerState(Scene::Help, Scene::CreateHelpSceneHandler(this));
    sceneStateMachine.registerState(Scene::RateIt, Scene::CreateRateItSceneHandler(this));
    sceneStateMachine.registerState(Scene::ElitePopup, Scene::CreateElitePopupSceneHandler(this));
    sceneStateMachine.registerState(Scene::AboutUsPopup, Scene::CreateAboutUsPopupSceneHandler(this));
    sceneStateMachine.registerState(Scene::StartAt10, Scene::CreateStartAt10SceneHandler(this));

    Color::nameColor(Color(3.0f / 255.0f, 99.0f / 255.f, 71.0f / 255.f), HASH("green", 0x0));

    LOGI("\t- Load FX...");
    theRenderingSystem.effectLibrary.load("desaturate.fs");

    LOGI("\t- Load animations...");
    // Animations
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_1", "herisson_1");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_2", "herisson_2");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_3", "herisson_3");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_4", "herisson_4");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_5", "herisson_5");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_6", "herisson_6");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_7", "herisson_7");
    theAnimationSystem.loadAnim(renderThreadContext->assetAPI, "herisson_8", "herisson_8");

    LOGI("\t- Define vibrateAPI...");
    theButtonSystem.vibrateAPI = gameThreadContext->vibrateAPI;

    Game::buildOrderedSystemsToUpdateList();

    LOGI("SAC engine initialisation done.");
}

void HeriswapGame::init(const uint8_t* in, int size) {
    LOGI("HeriswapGame initialisation begins...");

    CAMERA(camera)->clearColor = Color(0.49,0.59,0,1);
    TRANSFORM(camera)->z = 0;

    ScoreStorageProxy ssp;
    //init database
    LOGI("\t- Init database...");
    gameThreadContext->storageAPI->init(gameThreadContext->assetAPI, "Heriswap");
    gameThreadContext->storageAPI->setOption("sound", std::string(), "on");
    gameThreadContext->storageAPI->setOption("gameCount", std::string(), "0");
    gameThreadContext->storageAPI->createTable(&ssp);

    SuccessManager *sm = new SuccessManager(gameThreadContext->gameCenterAPI);
    datas = new PrivateData(this, gameThreadContext, sm);

    datas->Setup();

    theSoundSystem.mute = !datas->storageAPI->isOption("sound", "on");
    theMusicSystem.toggleMute(theSoundSystem.mute);

    datas->sky = theEntityManager.CreateEntityFromTemplate("general/sky");

    SCROLLING(datas->sky)->images.push_back(HASH("ciel0", 0x89b9427));
    SCROLLING(datas->sky)->images.push_back(HASH("ciel1", 0xd296102e));
    SCROLLING(datas->sky)->images.push_back(HASH("ciel2", 0x22648ee8));
    SCROLLING(datas->sky)->images.push_back(HASH("ciel3", 0x7f710b96));

    SCROLLING(datas->sky)->displaySize = glm::vec2(TRANSFORM(datas->sky)->size.x * 1.01,
                                                   TRANSFORM(datas->sky)->size.y);

    datas->faderHelper.init(camera);

    sceneStateMachine.setup(gameThreadContext->assetAPI);
    if (in && size) {
        loadGameState(in, size);
        SCROLLING(datas->sky)->show = true;
        theBackgroundSystem.showAll();
    } else {
        sceneStateMachine.start(Scene::Logo);
    }

	if (gameThreadContext->gameCenterAPI) {
		datas->gamecenterAPIHelper.init(gameThreadContext->gameCenterAPI, true, true, true, [this] {
			if (sceneStateMachine.getCurrentState() == Scene::ModeMenu) {
				int id = datas->mode * difficulty;
				gameThreadContext->gameCenterAPI->openSpecificLeaderboard(id);
			} else {
				gameThreadContext->gameCenterAPI->openDashboard();
			}
		});
	}

    #if SAC_INGAME_EDITORS && SAC_DEBUG
    HeriswapDebugConsole::init(this);
    #endif

    modeMenuIsInNameInput = false;
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

bool HeriswapGame::willConsumeBackEvent() {
    const auto state = sceneStateMachine.getCurrentState();

    if (pausableState(state))
        return true;

    switch (state) {
        case Scene::Help:
        case Scene::Pause:
        case Scene::AboutUsPopup:
        case Scene::ModeMenu:
            return true;
        default:
            LOGI("HeriswapGame will not consume backEvent");
            return false;
    }
}

void HeriswapGame::backPressed() {
    const Scene::Enum state = sceneStateMachine.getCurrentState();
    switch (state) {
#if ! SAC_ANDROID
        case Scene::MainMenu:
            break;
#endif
        case Scene::ModeMenu:
            if (modeMenuIsInNameInput)
                break;
        case Scene::AboutUsPopup:
            sceneStateMachine.forceNewState(Scene::MainMenu);
            break;
        case Scene::Spawn:
        case Scene::UserInput:
        case Scene::Delete:
        case Scene::Fall:
        case Scene::LevelChanged:
            sceneStateMachine.forceNewState(Scene::Pause);
            break;
        case Scene::Pause:
            sceneStateMachine.forceNewState(Scene::UserInput);
            break;
        default:
            break;
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
    switch (sceneStateMachine.getCurrentState()) {
        case Scene::UserInput:
        case Scene::Delete:
        case Scene::Fall:
        case Scene::Spawn:
        case Scene::LevelChanged:
            //updating gamemode
            datas->mode2Manager[datas->mode]->GameUpdate(dt, sceneStateMachine.getCurrentState());
            break;
        default:
        break;
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

    //updating HUD if playing
    if (inGameState(sceneStateMachine.getCurrentState()) && sceneStateMachine.getCurrentState() != Scene::LevelChanged) {
        datas->mode2Manager[datas->mode]->UiUpdate(dt);
    }

    if (theTouchInputManager.wasTouched(0) && theMusicSystem.isMuted() && !theSoundSystem.mute) {
        // restore music
        theMusicSystem.toggleMute(false);
    }

    //update music
    LOGT_EVERY_N(30, "FIX ME");
    #if 0
    if (!theMusicSystem.isMuted()) {
        const Scene::Enum state = sceneStateMachine.getCurrentState();
        //si on est en jeu et/ou  fin de musiques, charger de nouvelles musiques
        if ((pausableState(state) &&
            state != Scene::LevelChanged &&
            state != Scene::Pause)) {

            // TODO || state == Scene::BlackToSpawn) {
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

                MUSIC(datas->inGameMusic.masterTrack)->music = theMusicSystem.loadMusicFile(musics[0].c_str());
                MUSIC(datas->inGameMusic.masterTrack)->fadeIn = 1;

                unsigned int i;
                for (i=0; i<musics.size() - 1; i++) {
                     MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
                     mc->music = theMusicSystem.loadMusicFile(musics[i+1].c_str());
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

                MUSIC(datas->inGameMusic.masterTrack)->loopNext = theMusicSystem.loadMusicFile(musics[0].c_str());
                unsigned int i;
                for (i=0; i<musics.size() - 1; i++) {
                    MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
                    mc->loopNext = theMusicSystem.loadMusicFile(musics[i+1].c_str());
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
    #endif

    PROFILE("Game", "Tick", EndEvent);

    // systems update
    theHeriswapGridSystem.Update(dt);
    theTwitchSystem.Update(dt);
    theBackgroundSystem.Update(dt);
}

struct SavedState {
    // hand saved variables
    GameMode mode;
    int gridSize;
    bool gameWasPaused;

    int stateMachineSize;
    int entitySize;
    int successSize;
    int gameStateSize;
};

void HeriswapGame::initSerializer(Serializer& s) const {
    SavedState ss;
    s.add(new Property<int>(HASH("mode", 0x0), OFFSET(mode, ss)));
    s.add(new Property<int>(HASH("grid_size", 0x0), OFFSET(gridSize, ss)));
    s.add(new Property<bool>(HASH("game_was_paused", 0x0), OFFSET(gameWasPaused, ss)));

    s.add(new Property<int>(HASH("state_machine_size", 0x0), OFFSET(stateMachineSize, ss)));
    s.add(new Property<int>(HASH("entity_size", 0x0), OFFSET(entitySize, ss)));
    s.add(new Property<int>(HASH("success_size", 0x0), OFFSET(successSize, ss)));
    s.add(new Property<int>(HASH("game_state_size", 0x0), OFFSET(gameStateSize, ss)));
}

int HeriswapGame::saveState(uint8_t** out) {
    switch (sceneStateMachine.getCurrentState()) {
        case Scene::Help:
        case Scene::Pause:
        case Scene::Delete:
        case Scene::ElitePopup:
        case Scene::Fall:
        case Scene::LevelChanged:
        case Scene::Spawn:
        case Scene::UserInput:
            break;
        default:
            // other states do not save anything
            return 0;
    }

    SavedState ss;
    ss.mode = datas->mode;
    ss.gridSize = theHeriswapGridSystem.GridSize;
    ss.gameWasPaused = (sceneStateMachine.getCurrentState() == Scene::Pause);

    /* save all entities/components */
    uint8_t* entities = 0;
    ss.entitySize = theEntityManager.serialize(&entities);

    /* scene state machine */
    uint8_t* ssm = 0;
    ss.stateMachineSize = sceneStateMachine.serialize(&ssm);

    /* save Game mode */
    uint8_t* gamemode = 0;
    ss.gameStateSize = datas->mode2Manager[datas->mode]->saveInternalState(&gamemode);

    uint8_t* success = 0;
    ss.successSize = datas->successMgr->saveState(&success);

    /* save Game */
    uint8_t* game = 0;
    Serializer sz;
    initSerializer(sz);
    int gmSize = sz.serializeObject(&game, &ss);

    int finalSize = sizeof(int) +
        gmSize +
        ss.entitySize +
        ss.gameStateSize +
        ss.successSize +
        ss.stateMachineSize;
    *out = new uint8_t[finalSize];
    uint8_t* ptr = *out;

    /* save entity/system thingie */
    MEMPCPY(uint8_t*, ptr, &gmSize, sizeof(int));

    MEMPCPY(uint8_t*, ptr, game, gmSize);
    MEMPCPY(uint8_t*, ptr, entities, ss.entitySize);
    MEMPCPY(uint8_t*, ptr, ssm, ss.stateMachineSize);
    MEMPCPY(uint8_t*, ptr, success, ss.successSize);
    MEMPCPY(uint8_t*, ptr, gamemode, ss.gameStateSize);

    return finalSize;
}

void HeriswapGame::loadGameState(const uint8_t* in, int ) {
    int gmSize = 0;
    memcpy(&gmSize, in, sizeof(int));
    in += sizeof(int);

    SavedState ss;
    Serializer sz;
    initSerializer(sz);

    /* restore Game fields */
    sz.deserializeObject(in, gmSize, &ss);
    datas->mode = ss.mode;
    theHeriswapGridSystem.GridSize = ss.gridSize;
    theHeriswapGridSystem.Types = theHeriswapGridSystem.GridSize; //utiliser gridParamFromDifficulty nn ?
    in += gmSize;

    /* restore entities */
    theEntityManager.deserialize(in, ss.entitySize);
    in += ss.entitySize;

    /* restore state machine */
    sceneStateMachine.deserialize(in, ss.stateMachineSize);
    in += ss.stateMachineSize;

    datas->successMgr->restoreState(in, ss.successSize);
    in += ss.successSize;

    /* restore game vars */
    datas->mode2Manager[datas->mode]->restoreInternalState(in, ss.gameStateSize);
    in += ss.gameStateSize;

    // if game wasn't pause before stopping, force pause
    if (!ss.gameWasPaused) {
        sceneStateMachine.update(0);
        sceneStateMachine.forceNewState(Scene::Pause);
    }

    setupGameProp();
}

static const float rotations[] = {
    glm::quarter_pi<float>(),
    -glm::half_pi<float>(),
    0.0f,
    -3 * glm::quarter_pi<float>(),
    3 * glm::quarter_pi<float>(),
    glm::half_pi<float>(),
    -glm::pi<float>(),
    -glm::quarter_pi<float>()
};
static const char* feuilles [] = {
    "feuille1",
    "feuille2",
    "feuille3",
    "feuille4",
    "feuille5",
    "feuille6",
    "feuille7",
    "feuille8",
};

const char* HeriswapGame::cellTypeToTextureNameAndRotation(int type, float* rotation) {
    if (rotation)
        *rotation = rotations[type];

    LOGF_IF(type >= 8, "Invalid type value:" << type);
    return feuilles[type];
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
        datas->timing.deletion = 0.2f;
        datas->timing.swap = 0.03f;
        datas->timing.fall = 0.1f;
        datas->timing.haveToAddLeavesInGrid = 0.2f;
        datas->timing.replaceGrid = 0.5f;
    } else {
        //update anim times
        Difficulty difficulty = theHeriswapGridSystem.sizeToDifficulty();
        if (difficulty == DifficultyEasy) {
            datas->timing.deletion = 0.6f;
            datas->timing.swap = 0.14f;
            datas->timing.fall = 0.30f;
            datas->timing.haveToAddLeavesInGrid = 0.40f;
            datas->timing.replaceGrid = 1.f;
        } else {
            datas->timing.deletion = 0.3f;
            datas->timing.swap = 0.07f;
            datas->timing.fall = 0.15f;
            datas->timing.haveToAddLeavesInGrid = 0.40f;
            datas->timing.replaceGrid = 1.f;
        }
    }


    std::stringstream ss;
    ss << "where mode = " << datas->mode << " and difficulty = " << theHeriswapGridSystem.sizeToDifficulty();
    if (datas->mode == TilesAttack) ss << " order by time asc limit 5";
    else ss << " order by points desc limit 5";

    ScoreStorageProxy ssp;
    datas->storageAPI->loadEntries(&ssp, "*", ss.str());

    datas->bestScores.clear();
    datas->bestScores.resize(ssp._queue.size());
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
    datas->mode2Manager[datas->mode]->UiUpdate(0);

    MUSIC(datas->menu)->control = MusicControl::Stop;
}
