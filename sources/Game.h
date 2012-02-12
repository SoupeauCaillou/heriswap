#pragma once
#include "GridSystem.h"

class Game {
	public:

		void init(int windowW, int windowH);
		void tick(float dt);
		void fillTheBlank();

		void handleCombinations(std::vector<Combinais>& combinaisons);

		void updateSpawn(float dt);
		void updateUserInput(float dt);
		void updateRevertSwap(float dt);
		void updateDelete(float dt);
		void updateFall(float dt);
	class Data;
	Data* datas;
};

