#pragma once

#include <vector>
#include <string>

#include <base/Interval.h>

#include "GameStateManager.h"

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
		Interval<float> cloudStartX;
		Interval<float> cloudY[3];
		Interval<float> cloudSize[3];
		Interval<float> cloudSpeed[3];
		std::vector<std::string> textures[3];

};

