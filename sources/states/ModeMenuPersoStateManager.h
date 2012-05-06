#pragma once

#include <fstream>

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/MorphingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "Game.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"
#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"

class ModeMenuPersoStateManager : public GameStateManager {
	public:
		ModeMenuPersoStateManager(LocalizeAPI* lAPI);
		~ModeMenuPersoStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();

		Entity title, menufg, menubg;
		AnimatedActor* herisson;
	private:
		Entity play, playButton, back, scoreTitle;
		Entity fond;
		LocalizeAPI* localizeAPI;
		bool ended;
};
