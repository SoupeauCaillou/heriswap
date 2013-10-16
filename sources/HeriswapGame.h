/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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

#include "base/StateMachine.h"
#include "states/Scenes.h"

#include "api/AdAPI.h"
#include "api/CommunicationAPI.h"
#include "api/ExitAPI.h"
#include "api/LocalizeAPI.h"
#include "api/StorageAPI.h"
#include "api/VibrateAPI.h"

#include "base/Game.h"
#include "base/StateMachine.h"

#include "systems/RenderingSystem.h"
#include "systems/HeriswapGridSystem.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class NameInputAPI;

class PrivateData;
struct SavedState;

class HeriswapGame : public Game {
	public:
		HeriswapGame();
        ~HeriswapGame();

        bool wantsAPI(ContextAPI::Enum api) const;

		void sacInit(int windowW, int windowH);
		void init(const uint8_t* in = 0, int size = 0);
		void quickInit();
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool enabled);
		void setMode();
		int saveState(uint8_t** out);

        void backPressed();
        bool willConsumeBackEvent();

		static bool inGameState(Scene::Enum state);
		static bool pausableState(Scene::Enum state);
		static glm::vec2 GridCoordsToPosition(int i, int j, int s);
		static glm::vec2 CellSize(int gridSize, int cellType);
		static float CellContentScale();
		static std::string cellTypeToTextureNameAndRotation(int type, float* rotation);
		static float cellTypeToRotation(int type);

        void prepareNewGame();
		void setupGameProp();
    private:
        const uint8_t* loadEntitySystemState(const uint8_t* in, int size);
        void loadGameState(const uint8_t* in, int size);
		bool shouldPlayPiano();
        void onLeaderboardClick();


    public:
        PrivateData* datas;
        Entity herisson;
        Difficulty difficulty;

        void stopInGameMusics();

    private:
	    StateMachine<Scene::Enum> sceneStateMachine;

        void initSerializer(Serializer& s) const;
};
