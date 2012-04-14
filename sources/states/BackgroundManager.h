#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "GameStateManager.h"
#include "DepthLayer.h"

class BackgroundManager : public GameStateManager {
	public:
		BackgroundManager();
		~BackgroundManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();

	private:
		struct Actor;
		struct AnimatedActor;
		void switchAnim(AnimatedActor* a);

		std::vector<Actor*> clouds;
		Actor* initCloud(Actor* c);

		std::vector<Actor*> landscapes;
		std::vector<Actor*> trees;
		Actor* initLandscape(Actor* c, bool isATree);

	public:
		//clouds
		Vector2 xCloudStartRange, yCloudRange, cloudScaleRange;
		//landscape

};

