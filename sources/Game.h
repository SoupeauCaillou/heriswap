#pragma once
#include "GridSystem.h"

class ScoreStorage;

class Game {
	public:

		void init(ScoreStorage* score, int windowW, int windowH);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);

		static Vector2 GridCoordsToPosition(int i, int j);
		static float CellSize();
		static float CellContentScale();

		uint8_t* saveState();
		void loadState(const uint8_t* state);

	class Data;
	Data* datas;
};
