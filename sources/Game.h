#pragma once
#include "GridSystem.h"

class Game {
	public:

		void init(int windowW, int windowH);
		void tick(float dt);
		void fillTheBlank();

		void handleCombinations(std::vector<Combinais>& combinaisons);

	class Data;
	Data* datas;
};

