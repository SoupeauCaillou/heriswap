#pragma once

#include "api/NameInputAPI.h"
#include "SuccessManager.h"
#include "states/GameStateManager.h"
#include "Game.h"
#include "AnimedEntity.h"

class ModeMenuStateManager : public GameStateManager {
	public:
		ModeMenuStateManager(StorageAPI* storag, NameInputAPI* nameInputAPI, SuccessManager* successMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();

		void LoadScore(int mode, int dif);

		Entity title, menufg, menubg;
		AnimatedActor* herisson;

        bool pleaseGoBack;
	private:
		StorageAPI* storage;
		NameInputAPI* nameInputAPI;
        SuccessAPI* successAPI;
		Entity play, playButton, scoresPoints[5], scoresName[5], scoresLevel[5], back, scoreTitle;
		Entity yourScore, fond;
		#ifdef ANDROID
		Entity openfeint;
		#endif
		std::string playerName;
		SuccessManager* successMgr;
		LocalizeAPI* localizeAPI;

		//variables for perso mode
		int difficulty;
		Entity eDifficulty, bDifficulty;

        void submitScore(const std::string& playerName);
        bool isCurrentScoreAHighOne();
    public:
        enum GameOverState {
            NoGame,
            GameEnded,
            AskingPlayerName
        } gameOverState;

};
