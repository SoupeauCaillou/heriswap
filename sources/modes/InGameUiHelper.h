#pragma once

#include "base/EntityManager.h"
#include "Game.h"

class InGameUiHelper {
	public:
		InGameUiHelper();

		void build();
		void show();
		void update(float dt);
		void hide();
		void destroy();

	Entity smallLevel;
	Entity pauseButton;
	Entity soundButton;
	Entity scoreProgress;

	private:
		bool built;
};
