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
#include "SuccessManager.h"

class ModeMenuStateManager : public GameStateManager {
	public:
		ModeMenuStateManager(ScoreStorage* storag, PlayerNameInputUI* inputUII, SuccessManager* successMgr, LocalizeAPI* lAPI);
		~ModeMenuStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();

		void LoadScore(int mode, int dif);

		bool ended;

		Entity title, menufg, menubg;
		AnimatedActor* herisson;
	private:
		ScoreStorage* storage;
		PlayerNameInputUI* inputUI;
		Entity play, playButton, scoresPoints[5], scoresName[5], scoresLevel[5], back, openfeint, scoreTitle;
		Entity yourScore, fond;
		std::string playerName;
		SuccessManager* successMgr;
		LocalizeAPI* localizeAPI;

		//variables for perso mode
		int difficulty;
		Entity eDifficulty, bDifficulty;

};
