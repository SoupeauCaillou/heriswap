#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/PlayerSystem.h"
#include "GridSystem.h"

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

#include <sstream>

#define GRIDSIZE 8

class Game::Data {
	public:
		Data() {
			hud.Setup();

			state = MainMenu;

			state2Manager[MainMenu] = new MainMenuGameStateManager();
			state2Manager[Spawn] = new SpawnGameStateManager();
			state2Manager[UserInput] = new UserInputGameStateManager();
			state2Manager[Delete] = new DeleteGameStateManager();
			state2Manager[Fall] = new FallGameStateManager();
			state2Manager[ScoreBoard] = new ScoreBoardStateManager();
			state2Manager[EndMenu] = new EndMenuStateManager();
			
			for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it) {
				it->second->Setup();
			}
			time = TIMELIMIT;
		}

		GameState state;
		Entity background;
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

void Game::init(int windowW, int windowH) {
	theRenderingSystem.setWindowSize(windowW, windowH);

	theGridSystem.GridSize = GRIDSIZE;
	datas->background = theEntityManager.CreateEntity();
	theTransformationSystem.Add(datas->background);
	theRenderingSystem.Add(datas->background);
	RENDERING(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");

	datas = new Data();
	datas->state2Manager[datas->state]->Enter();
	
	Entity eHUD = theEntityManager.CreateEntity();
	thePlayerSystem.Add(eHUD);
}

void Game::tick(float dt) {

	theTouchInputManager.Update(dt);

	GameState newState;
	if (datas->time<0) {
		newState = EndMenu;
		datas->time = 0;
	} else {
		newState = datas->state2Manager[datas->state]->Update(dt);
	}
	
	if (newState != datas->state) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();
	}
	for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin(); it!=datas->state2Manager.end(); ++it) {
		it->second->BackgroundUpdate(dt);
	}

	theADSRSystem.Update(dt);
	theButtonSystem.Update(dt);

	if (newState != MainMenu && newState != ScoreBoard && newState != EndMenu) {
		datas->hud.Hide(false);
		datas->hud.Update(dt);
		if (datas->time > 0)
			datas->time-=dt;
		thePlayerSystem.Update(dt);
	}
	if (newState == EndMenu) {
		datas->hud.Hide(true);
		theGridSystem.DeleteAll();
		datas->time = TIMELIMIT;
	} else if (newState == MainMenu) {
		thePlayerSystem.Reset();
	}
	
	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theRenderingSystem.Update(dt);		
}
