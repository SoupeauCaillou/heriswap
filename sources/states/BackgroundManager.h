#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "GameStateManager.h"

class BackgroundManager : public GameStateManager {
	public:
		BackgroundManager();
		~BackgroundManager();
		void Setup();
		void Enter();
		GameState Update(float dt, GameModeManager* modeMng);
		void BackgroundUpdate(float dt);
		void Exit();

	private:
		struct Actor;
		struct AnimatedActor;

		Actor* initCloud(Actor* c);
		AnimatedActor* initAnimal(AnimatedActor* a);

		std::vector<Actor*> clouds;
		std::vector<AnimatedActor*> animals;

		void switchAnim(AnimatedActor* a);

	public:
		Vector2 xCloudStartRange, yCloudRange, cloudScaleRange;
		Vector2 xHerissonStartRange, yHerissonRange, herissonScaleRange;
		Vector2 xPapillonStartRange, yPapillonRange, papillonScaleRange;
};

