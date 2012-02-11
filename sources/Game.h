#pragma once

class Game {
	public:
		void init(int windowW, int windowH);
		void tick(float dt);
		void fillTheBlank();

	class Data;
	Data* datas;
};

