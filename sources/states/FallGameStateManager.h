#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

class FallGameStateManager : public GameStateManager {
	public:

	FallGameStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();
	public:
		// input/ouput
		struct Input {

		} input;
		struct Output {

		} output;
	private:
		// datas
		Entity eFall;
		std::vector<CellFall> falling;
};
