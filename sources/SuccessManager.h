#pragma once 

#include "Game.h"

class SuccessManager {
	public :
		
		SuccessManager(SuccessAPI* sAPI);
		~SuccessManager() {};
	
		void NewGame();
	
		//success only in "normal" difficulty
		bool successEnable;
		
		//success 6 in a row (6 leaves in one combi)
		void s6InARow(int nb);
		bool b6InARow;
		
		//success HardScore gamer (100K points in one game)
		void sHardScore(ScoreStorage* str);
		bool bHardScore;
		
		//success Fast And finish (100K points in one game)
		void sFastAndFinish(int leavesDone, int limit, float time);
		bool bFastAndFinish;
		
		//success Don't reset the grid (100K points in one game)
		void sResetGrid(int leavesDone, int limit);
		bool bResetGrid;
		
		//success Don't reset the grid (100K points in one game)
		void sTakeYourTime(float time);
		bool bTakeYourTime;
		
		//success Don't reset the grid (100K points in one game)
		void sExterminaScore(int points);
		bool bExterminaScore;
			
		//success Don't reset the grid (100K points in one game)
		void sLevel1For1K(int level, int points);
		bool bLevel1For1K;
		
		//success Don't reset the grid (100K points in one game)
		void sLevel10(int level);
		bool bLevel10;
			
		//success Don't reset the grid (100K points in one game)
		void sBonusToExcess(int type, int bonus, int nb);
		bool bBonusToExcess;	
		int bonusPoints;
	
		//success Don't reset the grid (100K points in one game)
		void sRainbow(int type);
		bool bRainbow;		
		int succEveryTypeInARow[8];
			
		SuccessAPI* successAPI;
};
