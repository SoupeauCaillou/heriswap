#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/PlayerSystem.h"
#include "GridSystem.h"

#include "base/Log.h"
#include "systems/HUDManager.h"
#include "base/TouchInputManager.h"
#include "base/MathUtil.h"
#include "base/EntityManager.h"
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

#include <sstream>

#define GRIDSIZE 8

class Game::Data {
	public:
		Data(ScoreStorage* storage) {
			hud.Setup();

			state = MainMenu;

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
		GameState state;
		Entity background, sky;
		float time;
		// drag/drop
		HUDManager hud;
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

	datas->state2Manager[datas->state]->Enter();

	Entity eHUD = theEntityManager.CreateEntity();

	ADD_COMPONENT(eHUD, Player);
}
void Game::togglePause(bool activate) {

	static GameState currentState;
	static bool gameIsPaused = false;
	
	if (activate && !gameIsPaused) {
		gameIsPaused = true;
		currentState = datas->state;
		datas->state2Manager[datas->state]->Exit();
		datas->state = Pause;
		datas->state2Manager[datas->state]->Enter();
	} else if (!activate) {
		gameIsPaused = false;
		datas->state2Manager[datas->state]->Exit();
		datas->state = currentState;
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

	theADSRSystem.Update(dt);
	theButtonSystem.Update(dt);
	//si on est ingame, on affiche le HUD
	if (newState != MainMenu && newState != ScoreBoard && newState != EndMenu && newState != Pause) {
		datas->hud.Hide(false);
		datas->hud.Update(dt);
		thePlayerSystem.Update(dt);
		theGridSystem.HideAll(false);
	} else {
		datas->hud.Hide(true);
		theGridSystem.HideAll(true);
	}

	if (newState == EndMenu) {
		theGridSystem.DeleteAll();
		thePlayerSystem.SetTime(0, true);
	} else if (newState == MainMenu) {
		thePlayerSystem.Reset();
	}

	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theRenderingSystem.Update(dt);
}
