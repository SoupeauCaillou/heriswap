#pragma once
#include <iostream>

#include "base/MathUtil.h"
#include "base/EntityManager.h"

#include "systems/System.h"

struct PlayerComponent {
	PlayerComponent() {
		score=0;
		levelUp = false;
		isReadyToStart = false;
		time = 0.;
		level = 1;
		bonus = MathUtil::RandomInt(8)+1;
		for (int i=0;i<50;i++)
			obj[i]=3+i;
			
		for (int i=0; i<8;i++)
			remain[i]=obj[0];
	}
	int score, level, obj[50], remain[8], bonus;
	float time;
	bool isReadyToStart, levelUp;
};


#define thePlayerSystem PlayerSystem::GetInstance()
#define PLAYER(e) thePlayerSystem.Get(e)

UPDATABLE_SYSTEM(Player)

public : 
	void SetTime(float t, bool r);

	float GetTime();

	int GetScore();

	int GetBonus();
	
	void LevelUp();
	//permet de savoir si on a change de niveau
	bool LeveledUp();
	
	void ScoreCalc(int nb, int type);

	void Reset();

	int GetRemain(int type);

	int GetObj();


	int GetLevel();
};
