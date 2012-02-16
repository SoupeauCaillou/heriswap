#include "GameStateManager.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

class BackgroundManager : public GameStateManager {
	public:
		BackgroundManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();

	private:
		struct Cloud;
		
		Cloud* initCloud(Cloud* c);
	
		std::list<Cloud*> clouds;
	public:
		Vector2 xStartRange;
		Vector2 yRange;
		Vector2 scaleRange;
};

