/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "api/NameInputAPI.h"
#include "api/CommunicationAPI.h"
#include "SuccessManager.h"
#include "states/GameStateManager.h"
#include "Game.h"
#include "AnimedActor.h"

class HelpStateManager;

class ModeMenuStateManager : public GameStateManager {
	public:
		ModeMenuStateManager(StorageAPI* storag, NameInputAPI* nameInputAPI, SuccessManager* successMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI, CommunicationAPI* comAPI);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();

		void LoadScore(int mode, int dif);

		int getDifficulty() const { return difficulty; }

		Entity title, menufg, menubg;
		AnimatedActor* herisson;

        bool pleaseGoBack;
        HelpStateManager* helpMgr;

        enum GameOverState {
            NoGame,
            GameEnded,
            AskingPlayerName
        } gameOverState;

	private:
		StorageAPI* storage;
		CommunicationAPI* communicationAPI;
		NameInputAPI* nameInputAPI;
        SuccessAPI* successAPI;
		Entity playText, playContainer, scoresPoints[5], scoresName[5], scoresLevel[5], back, scoreTitle;
		Entity yourScore, fond;
		std::string playerName;
		SuccessManager* successMgr;
		LocalizeAPI* localizeAPI;

		int difficulty;
		Entity eDifficulty, bDifficulty;

		Entity facebook, twitter, enableSwarm, enableSwarmContainer;

        void submitScore(const std::string& playerName);
        bool isCurrentScoreAHighOne();

};
