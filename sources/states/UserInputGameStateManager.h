#pragma once

#ifndef ANDROID
#include <GL/glew.h>
#include <GL/glfw.h>
#endif

#include "base/Log.h"
#include "base/TouchInputManager.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "GameStateManager.h"
#include "GridSystem.h"
#include "Game.h"
#include "SuccessManager.h"

class UserInputGameStateManager : public GameStateManager {
	public:

		UserInputGameStateManager(SuccessManager* smgr) : successMgr(smgr) {};
		~UserInputGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();
		
		void setAnimSpeed();
	private:
		// datas
		Entity eSwapper, dragged;
		bool dragStarted;
		int originI, originJ;
		int swapI, swapJ;
        std::vector<Entity> inCombinationCells;
        
        SuccessManager* successMgr;
};
