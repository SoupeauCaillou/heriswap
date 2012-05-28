#include <sstream>

#include <base/Log.h>
#include <base/TouchInputManager.h>
#include <base/MathUtil.h>
#include <base/EntityManager.h>
#include <base/TimeUtil.h>
#include <base/PlacementHelper.h>

#include "api/NameInputAPI.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/MorphingSystem.h"

#include "states/GameStateManager.h"
#include "states/BackgroundManager.h"
#include "states/LevelStateManager.h"
#include "states/ModeMenuStateManager.h"
#include "states/PauseStateManager.h"

#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"

#include "DepthLayer.h"
#include "GridSystem.h"
#include "Game.h"
#include "TwitchSystem.h"
#include "CombinationMark.h"
#include "Game_Private.h"

static bool inGameState(GameState state) {
	switch (state) {
		case Spawn:
		case UserInput:
		case Delete:
		case Fall:
			return true;
		default:
			return false;
	}
}

static bool pausableState(GameState state) {
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

static void updateFps(float dt);

// grid: [48, 302] -> [752, 1006]  in gimp
Vector2 Game::GridCoordsToPosition(int i, int j, int gridSize) {
	float startX = PlacementHelper::GimpXToScreen(48);
	float startY = PlacementHelper::GimpYToScreen(1006);
	float size = PlacementHelper::GimpWidthToScreen((752 - 48) / gridSize);

	return Vector2(
		startX + (i + 0.5) * size,
		startY + (j + 0.5) * size);
}

float Game::CellSize(int gridSize) {
	return PlacementHelper::GimpWidthToScreen((752 - 48) / gridSize);
}

float Game::CellContentScale() {
	return scale;
}

Game::Game(NativeAssetLoader* ploader, StorageAPI* storage, NameInputAPI* inputUI, SuccessAPI* sAPI, LocalizeAPI* lAPI, AdAPI* ad) {
	this->loader = ploader;
	/* create EntityManager */
	EntityManager::CreateInstance();

	/* create before system so it cannot use any of them (use Setup instead) */
	datas = new PrivateData(this, storage, inputUI, new SuccessManager(sAPI), lAPI, sAPI, ad);

	/* create systems singleton */
	TransformationSystem::CreateInstance();
	RenderingSystem::CreateInstance();
	SoundSystem::CreateInstance();
    MusicSystem::CreateInstance();
	GridSystem::CreateInstance();
	ADSRSystem::CreateInstance();
	ButtonSystem::CreateInstance();
	TextRenderingSystem::CreateInstance();
	ContainerSystem::CreateInstance();
	PhysicsSystem::CreateInstance();
    ParticuleSystem::CreateInstance();
    ScrollingSystem::CreateInstance();
    MorphingSystem::CreateInstance();
    TwitchSystem::CreateInstance();
}

Game::~Game() {
    LOGW("Delete game instance %p", this);
    theEntityManager.deleteAllEntities();
    EntityManager::DestroyInstance();

    TransformationSystem::DestroyInstance();
    RenderingSystem::DestroyInstance();
    SoundSystem::DestroyInstance();
    MusicSystem::DestroyInstance();
    GridSystem::DestroyInstance();
    ADSRSystem::DestroyInstance();
    ButtonSystem::DestroyInstance();
    TextRenderingSystem::DestroyInstance();
    ContainerSystem::DestroyInstance();
    PhysicsSystem::DestroyInstance();
    ParticuleSystem::DestroyInstance();
    ScrollingSystem::DestroyInstance();
    MorphingSystem::DestroyInstance();
    TwitchSystem::DestroyInstance();

    delete datas;
}

void Game::loadFont(const std::string& name) {
	char* font = loader->loadShaderFile(name + ".desc");
	std::stringstream sfont;
	sfont << font;
	std::string line;
	std::map<unsigned char, float> h2wratio;
	while (getline(sfont, line)) {
		if (line[0] == '#')
			continue;
		int c, w, h;
		sscanf(line.c_str(), "%d,%d,%d", &c, &w, &h);
		h2wratio[c] = (float)w / h;
	}
	delete[] font;
	h2wratio[' '] = h2wratio['a'];
	theTextRenderingSystem.registerFont(name, h2wratio);
}

void Game::sacInit(int windowW, int windowH) {
	PlacementHelper::ScreenHeight = 10;
    PlacementHelper::ScreenWidth = PlacementHelper::ScreenHeight * windowW / (float)windowH;
    PlacementHelper::WindowWidth = windowW;
    PlacementHelper::WindowHeight = windowH;
    PlacementHelper::GimpWidth = 800.0f;
    PlacementHelper::GimpHeight = 1280.0f;

	theRenderingSystem.setWindowSize(windowW, windowH, PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight);
	theTouchInputManager.init(Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight), Vector2(windowW, windowH));

	theRenderingSystem.init();

	/*
	theRenderingSystem.loadAtlas("sprites");
	theRenderingSystem.loadAtlas("animals");
	*/
    theRenderingSystem.loadAtlas("sprites");
    theRenderingSystem.loadAtlas("logo");
	theRenderingSystem.loadAtlas("alphabet");
    theRenderingSystem.loadAtlas("animals");
    theRenderingSystem.loadAtlas("decor1");
    theRenderingSystem.loadAtlas("decor2");
    theRenderingSystem.loadAtlas("feuilles");
    theRenderingSystem.loadAtlas("snow");
    theRenderingSystem.loadAtlas("menu");
    theRenderingSystem.loadAtlas("nuages");

	// init font
	loadFont("typo");
	loadFont("gdtypo");
}

void Game::init(const uint8_t* in, int size) {
    if (in && size) {
        in = loadEntitySystemState(in, size);
    }

	datas->Setup();

	theSoundSystem.mute = !datas->storage->soundEnable(false);
    theMusicSystem.toggleMute(theSoundSystem.mute);

	float bgElementWidth = PlacementHelper::GimpWidthToScreen(800);
	datas->sky = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->sky, Transformation);
	TRANSFORM(datas->sky)->z = DL_Sky;
	TRANSFORM(datas->sky)->size = Vector2(bgElementWidth, (bgElementWidth * 833.0) / 808.0);
	TransformationSystem::setPosition(TRANSFORM(datas->sky), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::N);
	ADD_COMPONENT(datas->sky, Scrolling);
	SCROLLING(datas->sky)->images.push_back("ciel0.png");
	SCROLLING(datas->sky)->images.push_back("ciel1.png");
	SCROLLING(datas->sky)->images.push_back("ciel2.png");
	SCROLLING(datas->sky)->images.push_back("ciel3.png");
	SCROLLING(datas->sky)->speed = Vector2(-0.1, 0);
	SCROLLING(datas->sky)->displaySize = Vector2(TRANSFORM(datas->sky)->size.X * 1, TRANSFORM(datas->sky)->size.Y);
    SCROLLING(datas->sky)->hide = true;
	static_cast<BackgroundManager*> (datas->state2Manager[Background])->skySpeed = -0.3;

	datas->mode2Manager[Normal]->sky = datas->sky;

    if (in && size) {
        datas->state = Pause;
        loadGameState(in, size);
    }
    datas->state2Manager[datas->state]->Enter();

}

void Game::setMode() {
	datas->state2Manager[Delete]->modeMgr = datas->mode2Manager[datas->mode];
	datas->state2Manager[ModeMenu]->modeMgr = datas->mode2Manager[datas->mode];
}

void Game::toggleShowCombi(bool forcedesactivate) {
	static bool activated;
	static std::vector<Entity> marks;
	//on switch le bool
	activated = !activated;
	if (forcedesactivate) activated = false;
	if (datas->state != UserInput) activated = false;
	if (activated) {
		LOGI("Affiche magique de la triche !") ;
		//j=0 : vertical
		//j=1 : h
		for (int j=0;j<2;j++) {
			std::vector<Vector2> combinaisons;
			if (j) combinaisons = theGridSystem.LookForCombinationsOnSwitchHorizontal();
			else combinaisons = theGridSystem.LookForCombinationsOnSwitchVertical();
			if (!combinaisons.empty())
			{
				for ( std::vector<Vector2>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
				{
					//rajout de 2 marques sur les elements a swich
					for (int i=0;i<2;i++) {
						if (j) {
							CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->X, it->Y));
							CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->X+i, it->Y));
							marks.push_back(theGridSystem.GetOnPos(it->X, it->Y));
							marks.push_back(theGridSystem.GetOnPos(it->X+1, it->Y));
						} else {
							CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->X, it->Y));
							CombinationMark::markCellInCombination(theGridSystem.GetOnPos(it->X, it->Y+1));						}
							marks.push_back(theGridSystem.GetOnPos(it->X, it->Y));
							marks.push_back(theGridSystem.GetOnPos(it->X, it->Y+1));
					}
				}
			}
		}
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

void Game::backPressed() {
    if (datas->state == ModeMenu) {
        // go back to main menu
        (static_cast<ModeMenuStateManager*>(datas->state2Manager[ModeMenu]))->pleaseGoBack = true;
    } else if (pausableState(datas->state)) {
        togglePause(true);
    }
}

void Game::togglePause(bool activate) {
	if (activate && datas->state != Pause && pausableState(datas->state)) {
        // pause
		datas->stateBeforePause = datas->state;
		datas->stateBeforePauseNeedEnter = false;
		datas->state = Pause;
		static_cast<PauseStateManager*> (datas->state2Manager[Pause])->mode = datas->mode;
        datas->mode2Manager[datas->mode]->TogglePauseDisplay(true);
		datas->state2Manager[datas->state]->Enter();
	} else if (!activate) {
        // unpause
        datas->mode2Manager[datas->mode]->TogglePauseDisplay(false);
		datas->state2Manager[datas->state]->Exit();
        datas->state = datas->stateBeforePause;
		if (datas->stateBeforePauseNeedEnter)
			datas->state2Manager[datas->state]->Enter();
	}
}

void Game::tick(float dt) {
	float updateDuration = TimeUtil::getTime();
	GameState newState;

    updateFps(dt);

	theTouchInputManager.Update(dt);

    // update state
    newState = datas->state2Manager[datas->state]->Update(dt);

	//get the game progress
	float percentDone = datas->mode2Manager[datas->mode]->GameProgressPercent();

    //updating game if needed
    if (datas->state == UserInput) {
		datas->mode2Manager[datas->mode]->GameUpdate(dt);
        if (percentDone >= 1) {
            newState = GameToBlack;
        }
     } else {
		 toggleShowCombi(true);
	 }

	if ((datas->state == Delete && theGridSystem.entityCount() == theGridSystem.GridSize * theGridSystem.GridSize) || datas->state == UserInput) {
		// si on change de niveau (course au score), on remplit pas la grille avant l'anim
		if (datas->mode == Normal) {
			NormalGameModeManager* m = static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal]);
			if (m->LevelUp()) {
				static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->currentLevel = m->currentLevel();
				newState = LevelChanged;
			}
		}
		// si on a fini (contre la montre), on remplit pas la grille et on quitte direct
		if (datas->mode == TilesAttack) {
			TilesAttackGameModeManager* m = static_cast<TilesAttackGameModeManager*> (datas->mode2Manager[TilesAttack]);
			if (m->GameProgressPercent()==1) {
				newState = GameToBlack;
			}
		}
	}
	//si on est passé de pause à quelque chose different de pause, on desactive la pause
	if (datas->state == Pause && newState == Unpause) {
		togglePause(false);

	}
    //sinon si on a change d'etat
    else if (newState != datas->state) {
		stateChanged(datas->state, newState);

		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();
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
		datas->storage->soundEnable(true); //on met a jour la table sql
		theSoundSystem.mute = !theSoundSystem.mute;
        theMusicSystem.toggleMute(theSoundSystem.mute);
		if (!theSoundSystem.mute) {
            SOUND(datas->soundButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on.png");
        } else {
            RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off.png");
        }
	}

    //updating HUD
	if (inGameState(newState)) {
		datas->mode2Manager[datas->mode]->UiUpdate(dt);
	}

    if (theTouchInputManager.wasTouched() && theMusicSystem.isMuted() && !theSoundSystem.mute) {
        // restore music
        theMusicSystem.toggleMute(false);
    }

    //update music
    if (!theMusicSystem.isMuted()) {
	    if ((pausableState(datas->state) && datas->state != LevelChanged && datas->state != Pause) || datas->state == BlackToSpawn) { //si on joue
	    	MUSIC(datas->inGameMusic.masterTrack)->control = MusicComponent::Start;
	    	MUSIC(datas->inGameMusic.masterTrack)->volume = 1;
	    	MUSIC(datas->inGameMusic.stressTrack)->control = MusicComponent::Start;
	        if (MUSIC(datas->inGameMusic.masterTrack)->music == InvalidMusicRef) {
	            std::vector<std::string> musics = datas->jukebox.pickNextSongs(4);
	            LOGW("New music picked for 'music' field (%lu):", musics.size());
	            for (unsigned i=0; i<musics.size(); i++) {
		            LOGW("\t%s", musics[i].c_str());
	            }
	            MUSIC(datas->inGameMusic.masterTrack)->music = theMusicSystem.loadMusicFile(musics[0]);
	            MUSIC(datas->inGameMusic.stressTrack)->music = theMusicSystem.loadMusicFile("audio/F.ogg");
	            unsigned int i;
	            for (i=0; i<musics.size() - 1; i++) {
	                 MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
	                 mc->music = theMusicSystem.loadMusicFile(musics[i+1]);
	                 mc->control = MusicComponent::Start;
	                 mc->volume = 1;
	            }
	        }

	        // if master track has looped, choose next songs to play
	        if (MUSIC(datas->inGameMusic.masterTrack)->loopNext == InvalidMusicRef) {
		        std::vector<std::string> musics = datas->jukebox.pickNextSongs(4);
		        LOGW("New music picked for 'loopNext' field (%lu):", musics.size());
	            for (unsigned i=0; i<musics.size(); i++) {
		            LOGW("\t%s", musics[i].c_str());
	            }

		        MUSIC(datas->inGameMusic.masterTrack)->loopNext = theMusicSystem.loadMusicFile(musics[0]);
		        unsigned int i;
		        for (i=0; i<musics.size() - 1; i++) {
			        MusicComponent* mc = MUSIC(datas->inGameMusic.secondaryTracks[i]);
			        mc->loopNext = theMusicSystem.loadMusicFile(musics[i+1]);
			        mc->control = MusicComponent::Start;
		        }
		        MUSIC(datas->inGameMusic.stressTrack)->loopNext = theMusicSystem.loadMusicFile("audio/F.ogg");
	        }
	        MUSIC(datas->inGameMusic.stressTrack)->volume = ADSR(datas->inGameMusic.stressTrack)->value;
	        MUSIC(datas->menu)->control = MusicComponent::Stop;

	    } else if (datas->state == MainMenu || datas->state == ModeMenu) { //dans les menus
	        if (MUSIC(datas->menu)->music == InvalidMusicRef) {
	         	LOGW("Start Menu music");
	            MUSIC(datas->menu)->music = theMusicSystem.loadMusicFile("audio/musique_menu.ogg");
	            MUSIC(datas->menu)->control = MusicComponent::Start;
	            MUSIC(datas->menu)->volume = 1;
	        }

	        if (MUSIC(datas->menu)->loopNext == InvalidMusicRef) {
	            MUSIC(datas->menu)->loopNext = theMusicSystem.loadMusicFile("audio/musique_menu.ogg");
	        }
	    }
    }

    // systems update
	theADSRSystem.Update(dt);
	theGridSystem.Update(dt);
	theButtonSystem.Update(dt);
    theParticuleSystem.Update(dt);
	theMorphingSystem.Update(dt);
    theTwitchSystem.Update(dt);
	thePhysicsSystem.Update(dt);
	theTransformationSystem.Update(dt);
	theScrollingSystem.Update(dt);
	theContainerSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theSoundSystem.Update(dt);
    theMusicSystem.Update(dt);
	theRenderingSystem.Update(dt);

	//bench settings
	updateDuration = TimeUtil::getTime()-updateDuration;
	bench(true, updateDuration, dt);
}

void Game::bench(bool active, float updateDuration, float dt) {
	if (active) {
		static float benchAccum = 0;
		benchAccum += dt;
		if (benchAccum>=1 && (updateDuration > 0) && !RENDERING(datas->benchTotalTime)->hide) {
			// draw update duration
			if (updateDuration > 1.0/60) {
				RENDERING(datas->benchTotalTime)->color = Color(1.0, 0,0, 1);
			} else {
				RENDERING(datas->benchTotalTime)->color = Color(0.0, 1.0,0,1);
			}
			float frameWidth = MathUtil::Min(updateDuration / (1.f/60), 1.0f) * 10;
			TRANSFORM(datas->benchTotalTime)->size.X = frameWidth;
			TRANSFORM(datas->benchTotalTime)->position.X = -5 + frameWidth * 0.5;

			// for each system adjust rectangle size/position to time spent
			float timeSpentInSystems = 0;
			float x = -5;
			for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
					it != datas->benchTimeSystem.end(); ++it) {
				float timeSpent = ComponentSystem::Named(it->first)->timeSpent;
				timeSpentInSystems += timeSpent;
				float width = 10 * (timeSpent / updateDuration);
				TRANSFORM(it->second)->size.X = width;
				TRANSFORM(it->second)->position.X = x + width * 0.5;
				RENDERING(it->second)->hide = false;
				x += width;

				// LOGI("%s: %.3f s", it->first.c_str(), timeSpent);
			}

			// LOGI("temps passe dans les systemes : %f sur %f total (%f %) (théorique : dt=%f)\n", timeSpentInSystems, updateDuration, 100*timeSpentInSystems/updateDuration, dt);
			benchAccum = 0;
		}
	}
}
int Game::saveState(uint8_t** out) {
	bool pausable = pausableState(datas->state);
	if (!pausable) {
		LOGI("Current state is '%d' -> nothing to save", datas->state);
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

	LOGI("%lu + %lu + %lu + %lu + %d + %d + %d -> %d (%p)",
		sizeof(datas->stateBeforePause), sizeof(datas->mode), sizeof(eSize), sizeof(sSize), eSize, sSize, gSize, finalSize, *out);
	return finalSize;
}

const uint8_t* Game::loadEntitySystemState(const uint8_t* in, int size) {
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

void Game::loadGameState(const uint8_t* in, int size) {
    /* restore Game fields */
    memcpy(&datas->stateBeforePause, in, sizeof(datas->stateBeforePause));
    datas->state = datas->stateBeforePause;
    in += sizeof(datas->stateBeforePause);
    memcpy(&datas->mode, in, sizeof(datas->mode));
    in += sizeof(datas->mode);
    memcpy(&theGridSystem.GridSize, in, sizeof(theGridSystem.GridSize));
    theGridSystem.Types = theGridSystem.GridSize;
    in += sizeof(theGridSystem.GridSize);

    datas->mode2Manager[datas->mode]->Enter();
    datas->mode2Manager[datas->mode]->restoreInternalState(in, size);
    datas->mode2Manager[datas->mode]->UiUpdate(0);

    setMode();
    togglePause(true);
    datas->stateBeforePauseNeedEnter = true;
    RENDERING(datas->soundButton)->hide = false;
    SCROLLING(datas->sky)->hide = false;
    LOGW("RESTORED STATE: %d", datas->stateBeforePause);
}

static float rotations[] = {
	MathUtil::PiOver4,
	-MathUtil::PiOver2,
	0,
	-3*MathUtil::PiOver4,
	3*MathUtil::PiOver4,
	MathUtil::PiOver2,
	-MathUtil::Pi,
	-MathUtil::PiOver4
};

std::string Game::cellTypeToTextureNameAndRotation(int type, float* rotation) {
	if (rotation)
		*rotation = rotations[type];

	std::stringstream s;
	s << "feuille" << (type+1) << ".png";
	return s.str();
}

float Game::cellTypeToRotation(int type) {
	return rotations[type];
}

void updateFps(float dt) {
    #define COUNT 250
    static int frameCount = 0;
    static float accum = 0, t = 0;
    frameCount++;
    accum += dt;
    if (frameCount == COUNT) {
         LOGI("%d frames: %.3f s - diff: %.3f s - ms per frame: %.3f", COUNT, accum, TimeUtil::getTime() - t, accum / COUNT);
         t = TimeUtil::getTime();
         accum = 0;
         frameCount = 0;
     }
}
