#pragma once
#include <iostream>
#include <sstream>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

#include "PlayerSystem.h"




class HUDManager {
	public:
			
		void Setup();
		
		void Update(float dt);
		
		void ScoreCalc(int nb);

		void Hide(bool toHide);
		
	private:
		class HUDManagerData;
		HUDManagerData* datas;
};
