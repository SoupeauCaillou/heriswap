#pragma once

//Menu 2: un titre, un bouton jouer, les 5 meilleurs scores du mode de jeu, un bouton openfeint, un bouton back, le score de la partie est affiché ici ?

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "Game.h"
#include "DepthLayer.h"
#include "PlacementHelper.h"
#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"

class ModeMenuStateManager : public GameStateManager {
	public:
		ModeMenuStateManager() { };
		~ModeMenuStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
		GameMode mode;
	private:
		Entity title, play, scores[5], back;
};
