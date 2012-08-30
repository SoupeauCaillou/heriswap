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
#include "api/CommunicationAPI.h"

class NameInputAPI;

class SuccessAPI {
	public:
		virtual void successCompleted(const char* description, unsigned long successId) {
			LOGI("Success completed '%s': %lu", description, successId);
		}
        virtual void openLeaderboard(int mode, int diff) {
			LOGI("Openleaderboard mode=%d, diff=%d", mode, diff);
        }
        virtual void openDashboard() {}
};

class PrivateData;
class Game {
	public:
		Game(AssetAPI* asset, StorageAPI* storage, NameInputAPI* inputUI, SuccessAPI* successAPI, LocalizeAPI* localizeAPI, AdAPI* ad, ExitAPI* exAPI, CommunicationAPI* comAPI);
        ~Game();
        void sacInit(int windowW, int windowH);
		void init(const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool enabled);
        void backPressed();
		void setMode();
		int saveState(uint8_t** out);

        void stateChanged(GameState from, GameState to);

		static Vector2 GridCoordsToPosition(int i, int j, int s);
		static float CellSize(int s);
		static float CellContentScale();
		static std::string cellTypeToTextureNameAndRotation(int type, float* rotation);
		static float cellTypeToRotation(int type);

		void setupGameProp();
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
