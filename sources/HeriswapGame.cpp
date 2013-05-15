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
#include "GridSystem.h"
#include "TwitchSystem.h"

#include "modes/NormalModeManager.h"
#include "modes/Go100SecondsModeManager.h"
#include "modes/TilesAttackModeManager.h"

#include "states/BackgroundManager.h"
#include "states/CountDownStateManager.h"
#include "states/FadeStateManager.h"
#include "states/LevelStateManager.h"
#include "states/MainMenuStateManager.h"
#include "states/ModeMenuStateManager.h"
#include "states/PauseStateManager.h"
#include "states/StateManager.h"
#include "states/UserInputStateManager.h"

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

bool HeriswapGame::inGameState(GameState state) {
	switch (state) {
		case Spawn:
		case UserInput:
		case Delete:
		case Fall:
		case LevelChanged:
			return true;
		default:
			return false;
	}
}

bool HeriswapGame::pausableState(GameState state) {
	switch (state) {
		case Spawn:
		case UserInput:
		case Delete:
		case Fall:
		case LevelChanged:
		case Pause:
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
	GridSystem::CreateInstance();
	TwitchSystem::CreateInstance();

	sceneStateMachine.registerState(Scene::CountDown, Scene::CreateLogoSceneHandler(this), "Scene::CountDown");
	sceneStateMachine.registerState(Scene::Spawn, Scene::CreateLogoSceneHandler(this), "Scene::Spawn");
	sceneStateMachine.registerState(Scene::UserInput, Scene::CreateLogoSceneHandler(this), "Scene::UserInput");
	sceneStateMachine.registerState(Scene::Delete, Scene::CreateLogoSceneHandler(this), "Scene::Delete");
	sceneStateMachine.registerState(Scene::Fall, Scene::CreateLogoSceneHandler(this), "Scene::Fall");
	sceneStateMachine.registerState(Scene::LevelChanged, Scene::CreateLogoSceneHandler(this), "Scene::LevelChanged");
	sceneStateMachine.registerState(Scene::Pause, Scene::CreateLogoSceneHandler(this), "Scene::Pause");
	sceneStateMachine.registerState(Scene::Unpause, Scene::CreateLogoSceneHandler(this), "Scene::Unpause");
	sceneStateMachine.registerState(Scene::MainMenu, Scene::CreateLogoSceneHandler(this), "Scene::MainMenu");
	sceneStateMachine.registerState(Scene::ModeMenu, Scene::CreateLogoSceneHandler(this), "Scene::ModeMenu");
	sceneStateMachine.registerState(Scene::ScoreBoard, Scene::CreateLogoSceneHandler(this), "Scene::ScoreBoard");
	sceneStateMachine.registerState(Scene::EndMenu, Scene::CreateLogoSceneHandler(this), "Scene::EndMenu");
	sceneStateMachine.registerState(Scene::Background, Scene::CreateLogoSceneHandler(this), "Scene::Background");
	sceneStateMachine.registerState(Scene::LogoToBlackState, Scene::CreateLogoSceneHandler(this), "Scene::LogoToBlackState");
	sceneStateMachine.registerState(Scene::ModeMenuToBlackState, Scene::CreateLogoSceneHandler(this), "Scene::ModeMenuToBlackState");
	sceneStateMachine.registerState(Scene::AdsToBlackState, Scene::CreateLogoSceneHandler(this), "Scene::AdsToBlackState");
	sceneStateMachine.registerState(Scene::BlackToLogoState, Scene::CreateLogoSceneHandler(this), "Scene::BlackToLogoState");
	sceneStateMachine.registerState(Scene::BlackToMainMenu, Scene::CreateLogoSceneHandler(this), "Scene::BlackToMainMenu");
	sceneStateMachine.registerState(Scene::BlackToSpawn, Scene::CreateLogoSceneHandler(this), "Scene::BlackToSpawn");
	sceneStateMachine.registerState(Scene::GameToBlack, Scene::CreateLogoSceneHandler(this), "Scene::GameToBlack");
	sceneStateMachine.registerState(Scene::BlackToModeMenu, Scene::CreateLogoSceneHandler(this), "Scene::BlackToModeMenu");
	sceneStateMachine.registerState(Scene::Logo, Scene::CreateLogoSceneHandler(this), "Scene::Logo");
	sceneStateMachine.registerState(Scene::Help, Scene::CreateLogoSceneHandler(this), "Scene::Help");
	sceneStateMachine.registerState(Scene::Ads, Scene::CreateLogoSceneHandler(this), "Scene::Ads");
	sceneStateMachine.registerState(Scene::RateIt, Scene::CreateLogoSceneHandler(this), "Scene::RateIt");
	sceneStateMachine.registerState(Scene::ExitState, Scene::CreateLogoSceneHandler(this), "Scene::ExitState");
	sceneStateMachine.registerState(Scene::ElitePopup, Scene::CreateLogoSceneHandler(this), "Scene::ElitePopup");
}

HeriswapGame::~HeriswapGame() {
	GridSystem::DestroyInstance();
	TwitchSystem::DestroyInstance();

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
            return true;
        default:
            return false;
    }
}

void HeriswapGame::quickInit(){
	
}

void HeriswapGame::sacInit(int windowW, int windowH) {
	LOGI("SAC engine initialisation begins...")
	Game::sacInit(windowW, windowH);
	PlacementHelper::GimpWidth = 800;
    PlacementHelper::GimpHeight = 1280;

	//init database
    gameThreadContext->storageAPI->init(gameThreadContext->assetAPI, "Heriswap");
    gameThreadContext->storageAPI->setOption("sound", std::string(), "on");
    gameThreadContext->storageAPI->setOption("gameCount", std::string(), "0");

	Color::nameColor(Color(3.0/255.0, 99.0/255, 71.0/255), "green");

	theRenderingSystem.effectLibrary.load("desaturate.fs");
	theRenderingSystem.loadAtlas("sprites", true);
	theRenderingSystem.loadAtlas("logo");
	theRenderingSystem.loadAtlas("alphabet", true);
    theRenderingSystem.loadAtlas("decor1");
    theRenderingSystem.loadAtlas("decor2");
    theRenderingSystem.loadAtlas("feuilles");
    theRenderingSystem.loadAtlas("snow");
    theRenderingSystem.loadAtlas("menu", true);
    theRenderingSystem.loadAtlas("nuages");
    theRenderingSystem.loadAtlas("help");

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

    // init font
	loadFont(renderThreadContext->assetAPI, "typo");
	loadFont(renderThreadContext->assetAPI, "gdtypo");

	// default camera
    camera = theEntityManager.CreateEntity("camera1");
    ADD_COMPONENT(camera, Transformation);
    TRANSFORM(camera)->size = glm::vec2(theRenderingSystem.screenW, theRenderingSystem.screenH);
    TRANSFORM(camera)->position = glm::vec2(0, 0);
    TRANSFORM(camera)->z = 1;
    ADD_COMPONENT(camera, Camera);
    CAMERA(camera)->enable = true;
    CAMERA(camera)->order = 2;
    CAMERA(camera)->id = 0;
    CAMERA(camera)->clearColor = Color(125.0/255, 150./255.0, 0.);

	LOGI("SAC engine initialisation done.")
}

void HeriswapGame::init(const uint8_t* in, int size) {
	LOGI("HeriswapGame initialisation begins...")
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

	datas->sky = theEntityManager.CreateEntity("sky");
	ADD_COMPONENT(datas->sky, Transformation);
	TRANSFORM(datas->sky)->z = DL_Sky;
	TRANSFORM(datas->sky)->size = glm::vec2(PlacementHelper::ScreenWidth, 
		                                    PlacementHelper::GimpWidthToScreen(800) * 833.0 / 808.0);
	TransformationSystem::setPosition(TRANSFORM(datas->sky), 
									  glm::vec2(0, PlacementHelper::GimpYToScreen(0)), 
									  TransformationSystem::N);
	ADD_COMPONENT(datas->sky, Scrolling);
	SCROLLING(datas->sky)->images.push_back("ciel0");
	SCROLLING(datas->sky)->images.push_back("ciel1");
	SCROLLING(datas->sky)->images.push_back("ciel2");
	SCROLLING(datas->sky)->images.push_back("ciel3");
	SCROLLING(datas->sky)->direction = -glm::vec2(1.f, 0.f);
	SCROLLING(datas->sky)->speed = 0.1;
	SCROLLING(datas->sky)->displaySize = glm::vec2(TRANSFORM(datas->sky)->size.x * 1.01, 
												   TRANSFORM(datas->sky)->size.y);
    SCROLLING(datas->sky)->show = false;
    SCROLLING(datas->sky)->opaqueType = RenderingComponent::FULL_OPAQUE;
	static_cast<BackgroundManager*> (datas->state2Manager[Background])->skySpeed = -0.3;

	datas->mode2Manager[Normal]->sky = datas->sky;

    if (in && size) {
        datas->state = Pause;
        loadGameState(in, size);
    }
    datas->state2Manager[datas->state]->Enter();

    quickInit();

    LOGI("HeriswapGame initialisation done.")
}

void HeriswapGame::setMode() {
	datas->state2Manager[Delete]->modeMgr = datas->mode2Manager[datas->mode];
	datas->state2Manager[ModeMenu]->modeMgr = datas->mode2Manager[datas->mode];
	datas->state2Manager[Spawn]->modeMgr = datas->mode2Manager[datas->mode];
	static_cast<CountDownStateManager*> (datas->state2Manager[CountDown])->mode = datas->mode;
	if (datas->mode == Normal || datas->mode == Go100Seconds) {
		static_cast<FadeGameStateManager*> (datas->state2Manager[GameToBlack])->duration = 4.0f;
	} else {
		static_cast<FadeGameStateManager*> (datas->state2Manager[GameToBlack])->duration = 0.5f;
	}
}

void HeriswapGame::toggleShowCombi(bool enabled) {
	static bool activated;
	static std::vector<Entity> marks;
	//on switch le bool
	activated = !activated;
	if (!enabled || (datas->state != UserInput))
		activated = false;

	if (activated) {
		LOGI("Affiche magique de la triche !") ;
		//j=0 : vertical
		//j=1 : h
		for (int j=0;j<2;j++) {
			std::vector<glm::vec2> combinaisons;
			if (j) combinaisons = theGridSystem.LookForCombinationsOnSwitchHorizontal();
			else combinaisons = theGridSystem.LookForCombinationsOnSwitchVertical();
			if (!combinaisons.empty())
			{
				for ( std::vector<glm::vec2>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
				{
					CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->x, it->y));
					marks.push_back(theGridSystem.GetOnPos(it->x, it->y));
					CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->x+(j+1)/2, it->y+(j+1)%2));
					marks.push_back(theGridSystem.GetOnPos(it->x+(j+1)/2, it->y+(j+1)%2));
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

    if (datas->state == ModeMenu) {
        // go back to main menu
        (static_cast<ModeMenuStateManager*>(datas->state2Manager[ModeMenu]))->pleaseGoBack = true;
    } else if (pausableState(datas->state)) {
	    #if defined(DEBUG)
	    datas->mode2Manager[datas->mode]->toggleDebugDisplay();
	    #else
        togglePause(true);
        #endif
    }
}

void HeriswapGame::togglePause(bool activate) {
	if (activate && datas->state != Pause && pausableState(datas->state)) {
		stopInGameMusics();
		// pause
		datas->stateBeforePause = datas->state;
		datas->stateBeforePauseNeedEnter = false;
		datas->state = datas->newState = Pause;
		static_cast<PauseStateManager*> (datas->state2Manager[Pause])->mode = datas->mode;
		TEXT_RENDERING(static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->eBigLevel)->show = false;
        datas->mode2Manager[datas->mode]->TogglePauseDisplay(true);
		datas->state2Manager[datas->state]->Enter();

	} else if (!activate) {
        // unpause
        if (datas->stateBeforePause == LevelChanged)
		    TEXT_RENDERING(static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->eBigLevel)->show = true;
        datas->mode2Manager[datas->mode]->TogglePauseDisplay(false);
		datas->state2Manager[datas->state]->Exit();
        datas->state = datas->stateBeforePause;
		if (datas->stateBeforePauseNeedEnter)
			datas->state2Manager[datas->state]->Enter();
	}
}

void HeriswapGame::tick(float dt) {
	PROFILE("Game", "Tick", BeginEvent);

	// update state
    datas->newState = datas->state2Manager[datas->state]->Update(dt);

    //update only if game has really begun (after countdown)
    if (datas->state != CountDown && static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput])->newGame == false) {
		//updating gamemode
		datas->mode2Manager[datas->mode]->GameUpdate(dt, datas->state);
    }

	//quand c'est plus au joueur de jouer, on supprime les marquages sur les feuilles
	if (datas->state != UserInput) {
		toggleShowCombi(false);
		if (datas->mode == Normal) {
			std::vector<Entity>& leavesInHelpCombination = static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal])->leavesInHelpCombination;
			if (!leavesInHelpCombination.empty()) {
				std::vector<Entity> leaves = theGridSystem.RetrieveAllEntityWithComponent();
				for ( std::vector<Entity>::reverse_iterator it = leaves.rbegin(); it != leaves.rend(); ++it) {
					RENDERING(*it)->effectRef = DefaultEffectRef;
				}

				leavesInHelpCombination.clear();
			}
		}
	}
	//get the game progress
	float percentDone = 0;
	if (inGameState(datas->state)) {
		percentDone = datas->mode2Manager[datas->mode]->GameProgressPercent();
	}

	//game ended
	if (datas->state == UserInput && percentDone >= 1) {
		datas->newState = GameToBlack;
		//show one combination which remain
		if (datas->mode != TilesAttack) {
			theGridSystem.ShowOneCombination();
		}
	}

	//ne pas changer la grille si fin de niveau/jeu
	if (datas->state == UserInput && datas->mode == Normal) {
		NormalGameModeManager* m = static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal]);
		if (m->LevelUp()) {
			datas->newState = LevelChanged;
		}
	}

	//si on est passé de pause à quelque chose different de pause, on desactive la pause
	if (datas->state == Pause && datas->newState == Unpause) {
		togglePause(false);

	}
    //si on a change d'etat
     else if (datas->newState != datas->state) {
		stateChanged(datas->state, datas->newState);

		if (datas->newState == ExitState)
			return;

		datas->state2Manager[datas->state]->Exit();
		datas->state = datas->newState;
		datas->state2Manager[datas->state]->Enter();

		#ifdef ANDROID
		bool ofHidden = (datas->newState != MainMenu && datas->newState != ModeMenu);
		#else
		bool ofHidden = true;
    	#endif
		RENDERING(datas->socialGamNet)->show = !ofHidden;
		BUTTON(datas->socialGamNet)->enabled = RENDERING(datas->socialGamNet)->show;
	}

    // background (unconditionnal) update of state managers
    for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin();
        it!=datas->state2Manager.end();
        ++it) {
        it->second->BackgroundUpdate(dt);
    }

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
		if (datas->state == ModeMenu) {
			Difficulty diff = (static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu]))->difficulty;
			successAPI->openLeaderboard(datas->mode, diff);
		} else {
			successAPI->openDashboard();
		}
	}

    //updating HUD if playing
	if (inGameState(datas->newState) && datas->newState != LevelChanged) {
		datas->mode2Manager[datas->mode]->UiUpdate(dt);
	}

    if (theTouchInputManager.wasTouched(0) && theMusicSystem.isMuted() && !theSoundSystem.mute) {
        // restore music
        theMusicSystem.toggleMute(false);
    }

    //update music
    if (!theMusicSystem.isMuted()) {
	    //si on est en jeu et/ou  fin de musiques, charger de nouvelles musiques
	    if ((pausableState(datas->state) && datas->state != LevelChanged && datas->state != Pause) || datas->state == BlackToSpawn) {
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

	    } else if (datas->state == MainMenu || datas->state == ModeMenu) { //dans les menus
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
	theGridSystem.Update(dt);
	theTwitchSystem.Update(dt);
}

int HeriswapGame::saveState(uint8_t** out) {
	if (datas->state == Help) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = static_cast<HelpStateManager*>(datas->state2Manager[datas->state])->oldState;//Pause;
		if (datas->state == BlackToSpawn) {
			datas->state = MainMenu;
		}
		datas->state2Manager[datas->state]->Enter();
	}
	bool pausable = pausableState(datas->state);
	if (!pausable) {
		LOGI("Current state is '" << datas->state << "' -> nothing to save");
		return 0;
	}

	if (datas->state == LevelChanged) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = Spawn;
		datas->mode2Manager[datas->mode]->generateLeaves(0, theGridSystem.Types);
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

    int finalSize = sizeof(datas->state) + sizeof(datas->mode) + sizeof(theGridSystem.GridSize) + sizeof(eSize) + sizeof(sSize) + eSize + sSize + gSize;
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
    ptr = (uint8_t*)mempcpy(ptr, &theGridSystem.GridSize, sizeof(theGridSystem.GridSize));
    ptr = (uint8_t*)mempcpy(ptr, gamemode, gSize);

	LOGI("'" << sizeof(datas->stateBeforePause) << "' + '"<< sizeof(datas->mode) << "' + '" << sizeof(eSize) << "' + '" << sizeof(sSize) << "' + '" << eSize << "' + '" << sSize << "' + '" << gSize << "' -> '" << finalSize << "' ('" << *out << "')");

	return finalSize;
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
    /* restore Game fields */
    memcpy(&datas->stateBeforePause, in, sizeof(datas->stateBeforePause));
    datas->state = datas->stateBeforePause;
    in += sizeof(datas->stateBeforePause);
    memcpy(&datas->mode, in, sizeof(datas->mode));
    in += sizeof(datas->mode);
    memcpy(&theGridSystem.GridSize, in, sizeof(theGridSystem.GridSize));
    theGridSystem.Types = theGridSystem.GridSize; //utiliser gridParamFromDifficulty nn ?
    in += sizeof(theGridSystem.GridSize);

    datas->mode2Manager[datas->mode]->Enter();
    datas->mode2Manager[datas->mode]->restoreInternalState(in, size);
    datas->mode2Manager[datas->mode]->UiUpdate(0);

    setMode();
    togglePause(true);
    datas->stateBeforePauseNeedEnter = true;

    MainMenuGameStateManager* mgsm = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]);
    static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title =
    	mgsm->modeTitleToReset = mgsm->eStart[datas->mode];

	setupGameProp();

    RENDERING(datas->soundButton)->show = true;
    SCROLLING(datas->sky)->show = true;
    LOGW("RESTORED STATE: '" << datas->stateBeforePause << "'");
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
