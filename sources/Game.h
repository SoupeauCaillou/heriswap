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

#include <string>
#include <vector>

#include <base/MathUtil.h>

#include "systems/RenderingSystem.h"

#include "GameState.h"

#include "api/LocalizeAPI.h"
#include "api/StorageAPI.h"
#include "api/AdAPI.h"
#include "api/ExitAPI.h"

class NameInputAPI;

class ScoreStorage {
	public:
		struct Score {
			unsigned int points, level;
			float time;
			std::string name;
		};

		virtual bool initTable() = 0;
		virtual void submitScore(Score scr, int mode, int diff) = 0;
		virtual	bool request(std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) = 0;
		virtual bool soundEnable(bool switchIt) = 0;
		virtual void saveOpt(std::string opt, std::string name) = 0;
		virtual std::vector<std::string> getName(std::string& result) = 0;

		#ifdef ANDROID
		virtual void openfeintLB(int mode) = 0;
		#endif
};

class SuccessAPI {
	public:
		virtual void successCompleted(const char* description, unsigned long successId) {
			LOGI("Success completed '%s': %lu", description, successId);
		}
        virtual void openfeintLB(int mode, int diff) {
	        mode=mode; diff=diff;
        }
        virtual void openfeintSuccess() {}
};

class PrivateData;
class Game {
	public:
		Game(AssetAPI* asset, StorageAPI* storage, NameInputAPI* inputUI, SuccessAPI* successAPI, LocalizeAPI* localizeAPI, AdAPI* ad, ExitAPI* exAPI);
        ~Game();
        void sacInit(int windowW, int windowH);
		void init(const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);
        void backPressed();
		void setMode();
		int saveState(uint8_t** out);

        void stateChanged(GameState from, GameState to);

		static Vector2 GridCoordsToPosition(int i, int j, int s);
		static float CellSize(int s);
		static float CellContentScale();
		static std::string cellTypeToTextureNameAndRotation(int type, float* rotation);
		static float cellTypeToRotation(int type);

	private:
		void loadFont(const std::string& name);
        const uint8_t* loadEntitySystemState(const uint8_t* in, int size);
        void loadGameState(const uint8_t* in, int size);
		void bench(bool active, float updateDuration, float dt);
		void stopInGameMusics();
		bool shouldPlayPiano();

	PrivateData* datas;
	AssetAPI* asset;
	SuccessAPI* successAPI;
	ExitAPI* exitAPI;
};
