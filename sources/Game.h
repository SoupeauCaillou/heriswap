#pragma once
#include "GridSystem.h"

class Game {
	public:

		void init(int windowW, int windowH);
		void tick(float dt);

		static Vector2 GridCoordsToPosition(int i, int j);
		static float CellSize();
		static float CellContentScale();
	class Data;
	Data* datas;
};

