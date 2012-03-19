#pragma once
#include <iostream>
#include <sstream>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"



class HUDManager {
	public:
		~HUDManager();

		void Setup();

		void Update(float dt, GameModeManager* moding, GameMode modingType, GameState state);

		void ScoreCalc(int nb);

		void Hide(bool toHide, GameMode mode);

	private:
		class HUDManagerData;
		HUDManagerData* datas;
};
