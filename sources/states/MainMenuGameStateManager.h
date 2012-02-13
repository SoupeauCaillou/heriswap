#include "GameStateManager.h"
#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

class MainMenuGameStateManager : public GameStateManager {
	public:
		MainMenuGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity start, score, eScore, eStart;
};

