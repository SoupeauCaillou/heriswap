#pragma once
#include <iostream>
#include <sstream>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

#include "modes/GameModeManager.h"




class HUDManager {
	public:
		~HUDManager();

		void Setup(GameModeManager* moding, GameMode modingType);

		void Update(float dt);

		void ScoreCalc(int nb);

		void Hide(bool toHide);

	private:
		class HUDManagerData;
		HUDManagerData* datas;
};
