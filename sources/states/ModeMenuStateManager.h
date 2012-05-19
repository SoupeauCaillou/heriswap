#pragma once

#include "SuccessManager.h"
#include "states/GameStateManager.h"
#include "Game.h"
#include "AnimedEntity.h"

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
