#pragma once 

#include "Game.h"

class SuccessManager {
	public :
		
		SuccessManager(SuccessAPI* sAPI);
		~SuccessManager() {};
	
		void NewGame(int difficulty);
	
		//success only in "normal" difficulty
		bool successEnable;
		
		//success 6 in a row (Delete 6 or more leaves in one swap)
		void s6InARow(int nb);
		bool b6InARow;
		
		//success HardScore gamer (Reach 1M points in total !)
		void sHardScore(ScoreStorage* str);
		bool bHardScore;
		
		//success Fast And finish (Finish a tiles attack round within 35 secs)
		void sFastAndFinish(int leavesDone, int limit, float time);
		bool bFastAndFinish;
		
		//success Don't reset the grid (In tiles attack, if you finish without resetting the grid)
		void sResetGrid();
		bool bResetGrid;
		bool gridResetted;
		
		//success Take it slow (Played a game more than 15 min)
		void sTakeYourTime(float time);
		bool bTakeYourTime;
		
		//success Exterminascore (Start from scratch and skyrocket up to 100k in a row !)
		void sExterminaScore(int points);
		bool bExterminaScore;
			
		//success 1k points for level 1 (Get 1000 points without levelling in normal mode)
		void sLevel1For1K(int level, int points);
		bool bLevel1For1K;
		
		//success Level 10 (Reach level 10 in normal mode)
		void sLevel10(int level);
		bool bLevel10;
			
		//success Bonus to excess (Get 100 bonus leaves in one game)
		void sBonusToExcess(int type, int bonus, int nb);
		bool bBonusToExcess;	
		int bonusPoints;
	
		//success Rainbow combination (Make one combination from each type in a row)
		//success Double rainbow (Ohohoh, double rainbow)
		void sRainbow(int type);
		bool bRainbow;		
		bool bDoubleRainbow;
		int succEveryTypeInARow[8];

		//success Lucky Luke (Get more than 1 combi by 5 sec during 30 sec)
		void sLuckyLuke();
		float timeLL, timeLLloop; // timeLLloop used in userinput
		bool bLuckyLuke;
		
		//success Donator (Click on Ads)
		void sDonator();
		bool bDonator;
		
		SuccessAPI* successAPI;
};
