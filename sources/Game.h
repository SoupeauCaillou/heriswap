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
	class Data;
	Data* datas;
};

