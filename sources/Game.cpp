#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
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

#include <sstream>

#define GRIDSIZE 8

class Game::Data {
	public:
		Data() {
			hud.Setup();

			state = Spawn;

			state2Manager[Spawn] = new SpawnGameStateManager();
			state2Manager[UserInput] = new UserInputGameStateManager();
			state2Manager[Delete] = new DeleteGameStateManager();
			state2Manager[Fall] = new FallGameStateManager();

			for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it) {
				it->second->Setup();
			}
		}

		GameState state;
		Entity background;

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
}

void Game::tick(float dt) {
	theTouchInputManager.Update(dt);

	GameState newState = datas->state2Manager[datas->state]->Update(dt);
	if (newState != datas->state) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();
	}

	theADSRSystem.Update(dt);

	theButtonSystem.Update(dt);

	//fillTheBlank();
	datas->hud.Update(dt);
	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theRenderingSystem.Update(dt);
}
