#pragma once
#include "GridSystem.h"

#define FAST 0.2

class ScoreStorage;

class Game {
	public:
		Game(ScoreStorage* storage);
		void init(int windowW, int windowH, const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);

		static Vector2 GridCoordsToPosition(int i, int j);
		static float CellSize();
		static float CellContentScale();
		
		void setMode();

		int saveState(uint8_t** out);
	private:
		void loadState(const uint8_t* in, int size);
		void bench(bool active, float updateDuration, float dt);

	class Data;
	Data* datas;
};
