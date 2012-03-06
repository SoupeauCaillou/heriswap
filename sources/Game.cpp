#include <sstream>

#include "base/Log.h"
#include "base/TouchInputManager.h"
#include "base/MathUtil.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/SoundSystem.h"

#include "HUDManager.h"
#include "GridSystem.h"
#include "Game.h"
#include "PlayerSystem.h"
#include "CombinationMarkSystem.h"

#include "states/GameStateManager.h"
#include "states/SpawnGameStateManager.h"
#include "states/UserInputGameStateManager.h"
#include "states/DeleteGameStateManager.h"
#include "states/FallGameStateManager.h"
#include "states/MainMenuGameStateManager.h"
#include "states/ScoreBoardStateManager.h"
#include "states/EndMenuStateManager.h"
#include "states/BackgroundManager.h"
#include "states/LevelStateManager.h"
#include "states/PauseStateManager.h"
#include "states/FadeStateManager.h"


#define GRIDSIZE 8

class Game::Data {
	public:
		Data(ScoreStorage* storage) {
			hud = new HUDManager;
			hud->Setup();
			state = BlackToLogoState;
	


			Entity logo;
			logo = theEntityManager.CreateEntity();
			ADD_COMPONENT(logo, Rendering);
			ADD_COMPONENT(logo, Transformation);
			TRANSFORM(logo)->position = Vector2(0,0);
			RENDERING(logo)->size = Vector2(10,10*720/420);
			TRANSFORM(logo)->z = 39;
			RENDERING(logo)->texture = theRenderingSystem.loadTextureFile("logo.png");	
		
		
			state2Manager[BlackToLogoState] = new FadeGameStateManager(logo, FadeIn, BlackToLogoState, LogoToBlackState);
			state2Manager[LogoToBlackState] = new FadeGameStateManager(logo, FadeOut, LogoToBlackState, BlackToMainMenu);
			//to do : add entity || modif 0 du GameStateToBlack
			//hm, ça implique d'en faire un pour chaque state..
			state2Manager[BlackToMainMenu] = new FadeGameStateManager(0, FadeIn, BlackToMainMenu, MainMenu);	
			
			state2Manager[MainMenuToBlackState] = new FadeGameStateManager(0, FadeOut, MainMenuToBlackState, BlackToSpawn);	

			state2Manager[BlackToSpawn] = new FadeGameStateManager(0, FadeIn, BlackToSpawn, Spawn);	

			state2Manager[MainMenu] = new MainMenuGameStateManager();
			state2Manager[Spawn] = new SpawnGameStateManager();
			state2Manager[UserInput] = new UserInputGameStateManager();
			state2Manager[Delete] = new DeleteGameStateManager();
			state2Manager[Fall] = new FallGameStateManager();
			state2Manager[LevelChanged] = new LevelStateManager();
			state2Manager[ScoreBoard] = new ScoreBoardStateManager(storage);
			state2Manager[EndMenu] = new EndMenuStateManager(storage);
			state2Manager[Pause] = new PauseStateManager();


			BackgroundManager* bg = new BackgroundManager();
			bg->xStartRange = Vector2(6, 8);
			bg->yRange = Vector2(-2, 9);
			bg->scaleRange = Vector2(0.4, 1.5);
			state2Manager[Background] = bg;

			for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it) {
				it->second->Setup();
			}
			time = TIMELIMIT;
		}
		GameState state, stateBeforePause;
		Entity background, sky;
		float time;
		// drag/drop
		HUDManager* hud;
		std::map<GameState, GameStateManager*> state2Manager;
};

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / GRIDSIZE;

Vector2 Game::GridCoordsToPosition(int i, int j) {
	return Vector2(
		-5 + (i + 0.5) * size + offset,
		-5 + (j + 0.5)*size + offset);
}

float Game::CellSize() {
	return size;
}

float Game::CellContentScale() {
	return scale;
}

void Game::init(ScoreStorage* storage, int windowW, int windowH) {
	theRenderingSystem.init();

	LOGI("%s\n", __FUNCTION__);
	datas = new Data(storage);

	theRenderingSystem.setWindowSize(windowW, windowH);

	theGridSystem.GridSize = GRIDSIZE;

	datas->sky = theEntityManager.CreateEntity();
	theTransformationSystem.Add(datas->sky);
	TRANSFORM(datas->sky)->z = -1;
	theRenderingSystem.Add(datas->sky);
	RENDERING(datas->sky)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->sky)->texture = theRenderingSystem.loadTextureFile("sky.png");

	datas->background = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->background, Transformation);
	ADD_COMPONENT(datas->background, Rendering);
	TRANSFORM(datas->background)->z = 0;
	RENDERING(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");
	ADD_COMPONENT(datas->background, Sound);
	SOUND(datas->background)->sound = theSoundSystem.loadSoundFile("audio/A.ogg");
	SOUND(datas->background)->repeat = false;

	datas->state2Manager[datas->state]->Enter();


	Entity eHUD = theEntityManager.CreateEntity();

	ADD_COMPONENT(eHUD, Player);
	
}

void Game::toggleShowCombi(bool forcedesactivate) {
	static bool activated;
	//on switch le bool
	activated = !activated;
	if (forcedesactivate) activated = false;
	if (datas->state != UserInput) activated = false;
	static std::vector<Entity> combinationMark;
	if (activated) {
		std::cout << "Affiche magique de la triche ! \n" ;
		for (int j=0;j<2;j++) {
			std::vector<Vector2> combinaisons;
			if (j) combinaisons = theGridSystem.LookForCombinationsOnSwitchHorizontal();
			else combinaisons = theGridSystem.LookForCombinationsOnSwitchVertical();
			if (!combinaisons.empty())
			{
				for ( std::vector<Vector2>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
				{
					for (int i=0;i<2;i++) {
						combinationMark.push_back(theEntityManager.CreateEntity());
						theEntityManager.AddComponent(combinationMark.back(), &theTransformationSystem);
						theEntityManager.AddComponent(combinationMark.back(), &theADSRSystem);
						theEntityManager.AddComponent(combinationMark.back(), &theRenderingSystem);
						if (j) {
							TRANSFORM(combinationMark.back())->position = GridCoordsToPosition(it->X+i, it->Y);
							RENDERING(combinationMark.back())->texture = theRenderingSystem.loadTextureFile("combinationMark2.png");
						} else {
							TRANSFORM(combinationMark.back())->position = GridCoordsToPosition(it->X, it->Y+i);
							RENDERING(combinationMark.back())->texture = theRenderingSystem.loadTextureFile("combinationMark1.png");
						}
						TRANSFORM(combinationMark.back())->z = 5;
						RENDERING(combinationMark.back())->size = CellSize();
					}
				}
			}
		}
	} else {
		if (combinationMark.size() > 0) {
			std::cout << "Destruction des marquages et de la triche !\n";
			for (std::vector<Entity>::iterator it=combinationMark.begin(); it!=combinationMark.end(); it++) {
				theEntityManager.DeleteEntity(*it);
			}
			combinationMark.clear();
		}
	}
}
void Game::togglePause(bool activate) {
	static bool gameIsPaused = false;

	if (activate && !gameIsPaused) {
		gameIsPaused = true;
		datas->stateBeforePause = datas->state;
		datas->state2Manager[datas->state]->Exit();
		datas->state = Pause;
		datas->state2Manager[datas->state]->Enter();
	} else if (!activate) {
		gameIsPaused = false;
		datas->state2Manager[datas->state]->Exit();
		datas->state = datas->stateBeforePause;
		datas->state2Manager[datas->state]->Enter();
	}
}

void Game::tick(float dt) {
	GameState newState;

	theTouchInputManager.Update(dt);

	//si le chrono est fini, on passe au menu de fin
	if (TIMELIMIT - thePlayerSystem.GetTime()<0) {
		newState = EndMenu;
	} else {
		newState = datas->state2Manager[datas->state]->Update(dt);
	}
	if (newState != datas->state && datas->state == Pause) {
		togglePause(false);
	} else if (newState != datas->state) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();
	}

	for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin(); it!=datas->state2Manager.end(); ++it) {
		it->second->BackgroundUpdate(dt);
	}
	//si c'est pas à l'user de jouer, on cache de force les combi
	if (newState != UserInput)
		toggleShowCombi(true);



	theADSRSystem.Update(dt);
	theButtonSystem.Update(dt);
	//si on est ingame, on affiche le HUD
	if (newState == Spawn || newState == UserInput || newState == Delete || newState == Fall) {
		datas->hud->Hide(false);
		datas->hud->Update(dt);
		thePlayerSystem.Update(dt);
		theGridSystem.HideAll(false);
	} else {
		datas->hud->Hide(true);
		theGridSystem.HideAll(true);
	}


	if (newState == EndMenu) {
		theGridSystem.DeleteAll();
		thePlayerSystem.SetTime(0, true);
	} else if (newState == MainMenu) {
		thePlayerSystem.Reset();
	}

	SoundComponent* sc = SOUND(datas->background);
	if (sc->sound >= InvalidSoundRef) {
		char c = MathUtil::RandomInt('H' - 'A' + 1) + 'A';
		std::stringstream s;
		s << "audio/" << c << ".ogg";
		sc->sound = theSoundSystem.loadSoundFile(s.str());
		sc->position = 0;
	}
	theCombinationMarkSystem.Update(dt);
	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theRenderingSystem.Update(dt);
	theSoundSystem.Update(dt);
}

int Game::saveState(uint8_t** out) {
	/* delete game managers */
	for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin(); it!=datas->state2Manager.end(); ++it) {
		delete (*it).second;
	}
	datas->state2Manager.clear();
	delete datas->hud;

	/* special case... */
	std::vector<Entity> marks = theCombinationMarkSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<marks.size(); i++) {
		theEntityManager.DeleteEntity(marks[i]);
	}

	/* delte local entities */
	theEntityManager.DeleteEntity(datas->background);
	theEntityManager.DeleteEntity(datas->sky);

	/* save all entities/components */
	uint8_t* entities = 0;
	int eSize = theEntityManager.serialize(&entities);

	/* save System with assets ? (texture name -> texture ref map of RenderingSystem ?) */
	uint8_t* systems = 0;
	int sSize = theRenderingSystem.saveInternalState(&systems);

	/* save Game fields */
	int finalSize = sizeof(datas->stateBeforePause) + sizeof(eSize) + sizeof(sSize) + eSize + sSize;
	*out = new uint8_t[finalSize];
	uint8_t* ptr = *out;
	ptr = (uint8_t*)mempcpy(ptr, &datas->stateBeforePause, sizeof(datas->stateBeforePause));
	ptr = (uint8_t*)mempcpy(ptr, &eSize, sizeof(eSize));
	ptr = (uint8_t*)mempcpy(ptr, &sSize, sizeof(sSize));
	ptr = (uint8_t*)mempcpy(ptr, entities, eSize);
	ptr = (uint8_t*)mempcpy(ptr, systems, sSize);

	std::cout << sizeof(datas->stateBeforePause) << " + " << sizeof(eSize) << " + " << sizeof(sSize) << " + " << eSize << " + " << sSize << " -> " << finalSize << std::endl;
	return finalSize;
}

void Game::loadState(const uint8_t* in, int size) {
	/* restore Game fields */
	int index = 0;
	memcpy(&datas->stateBeforePause, &in[index], sizeof(datas->stateBeforePause));
	in += sizeof(datas->stateBeforePause);
	int eSize, sSize;
	memcpy(&eSize, &in[index], sizeof(eSize));
	index += sizeof(eSize);
	memcpy(&sSize, &in[index], sizeof(sSize));
	index += sizeof(sSize);
	/* restore entities */
	theEntityManager.deserialize(&in[index], eSize);
	index += eSize;
	/* restore systems */
	theRenderingSystem.restoreInternalState(&in[index], sSize);
}
