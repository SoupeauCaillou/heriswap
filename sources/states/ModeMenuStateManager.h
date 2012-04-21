#pragma once

//Menu 2: un titre, un bouton jouer, les 5 meilleurs scores du mode de jeu, un bouton openfeint, un bouton back, le score de la partie est affiché ici ?

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
#include "PlacementHelper.h"
#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"

class ModeMenuStateManager : public GameStateManager {
	public:
		ModeMenuStateManager(ScoreStorage* storag, PlayerNameInputUI* inputUII) : storage(storag), ended(false), inputUI(inputUII) { };
		~ModeMenuStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

		void LoadScore(int mode);

		bool ended;

		Entity title, menufg, menubg, herissonActor;
	private:
		ScoreStorage* storage;
		PlayerNameInputUI* inputUI;
		Entity play, playButton, scoresPoints[5], scoresName[5], scoresLevel[5], back, openfeint, scoreTitle;
		Entity yourScore, fond;
		std::string playerName;
};
