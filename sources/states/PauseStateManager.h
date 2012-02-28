#include "GameStateManager.h"
#include "base/EntityManager.h"
#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"

class PauseStateManager : public GameStateManager {
	public:

	PauseStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:
		Entity eRestart;
	
};


