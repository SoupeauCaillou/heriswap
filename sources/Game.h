#pragma once

#include <string>
#include <vector>

#include <base/MathUtil.h>

#include "systems/RenderingSystem.h"

#include "GameState.h"

#include "api/LocalizeAPI.h"
#include "api/StorageAPI.h"

class NameInputAPI;

class ScoreStorage {
	public:
		struct Score {
			int points, level;
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
			LOGI("Success completed '%s': %d", description, successId);
		}
};

class PrivateData;
class Game {
	public:
		Game(NativeAssetLoader* loader, StorageAPI* storage, NameInputAPI* inputUI, SuccessAPI* successAPI, LocalizeAPI* localizeAPI);
		void sacInit(int windowW, int windowH);
		void init(const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);
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

	PrivateData* datas;
	NativeAssetLoader* loader;
};
