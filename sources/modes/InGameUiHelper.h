#pragma once

#include "base/EntityManager.h"

class InGameUiHelper {
	public:
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