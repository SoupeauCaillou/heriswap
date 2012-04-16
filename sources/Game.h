#pragma once
#include "GridSystem.h"

#define FAST 1.

class ScoreStorage {
	public:
		struct Score {
			int mode, points;
			float time;
			std::string name;
		};
		virtual void initTable() = 0;
		virtual void submitScore(Score scr) = 0;
		virtual std::vector<Score> getScore(int mode) = 0;
};

class PlayerNameInputUI {
	public:
		virtual void show() = 0;
		virtual bool query(std::string& result) = 0;
		virtual bool nameNeeded() = 0;
};

class Game {
	public:
		Game(ScoreStorage* storage, PlayerNameInputUI* inputUI);
		void init(int windowW, int windowH, const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);

		static Vector2 GridCoordsToPosition(int i, int j);
		static float CellSize();
		static float CellContentScale();
		static std::string cellTypeToTextureNameAndRotation(int type, float* rotation);
		static float cellTypeToRotation(int type);
		void hideEveryThing(bool toHide, bool blacktospawn);

		void setMode();

		int saveState(uint8_t** out);
	private:
		void loadState(const uint8_t* in, int size);
		void bench(bool active, float updateDuration, float dt);

	class Data;
	Data* datas;
};
