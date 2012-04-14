#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/Interval.h"
#include "systems/TransformationSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "GameStateManager.h"
#include "DepthLayer.h"

class BackgroundManager : public GameStateManager {
	public:
		BackgroundManager(float windowHeight);
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
		Actor* initCloud(Actor* c, int group);

	public:
		float skySpeed;
		//clouds
<<<<<<< HEAD
		Interval<float> cloudStartX;
=======
		Vector2 xCloudStartRange, yCloudRange, cloudScaleRange;
		//animals
		Vector2 xPapillonStartRange, yPapillonRange, papillonScaleRange;
>>>>>>> 3a03e96... le hérisson indique le temps
		//landscape
		Interval<float> cloudY[3];
		Interval<float> cloudSize[3];
		Interval<float> cloudSpeed[3];
		std::vector<std::string> textures[3];

};

