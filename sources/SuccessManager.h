#pragma once

#include <vector>

#include "GridSystem.h"
#include "Game.h"

class SuccessManager {
	public :

		SuccessManager(SuccessAPI* sAPI);
		~SuccessManager() {};

		void NewGame(int difficulty);


		//success only in "hard" difficulty
		bool hardMode;
		SuccessAPI* successAPI;

		//success 6 in a row (Delete 6 or more leaves in one swap)
		void s6InARow(int nb);
		bool b6InARow;

		//success HardScore gamer (Reach 1M points in total !)
		void sHardScore(StorageAPI* str);
		bool bHardScore;

		//success Fast And finish (Finish a tiles attack round within 35 secs)
		void sFastAndFinish(float time);
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
		int bonusTilesNumber;

		//success Rainbow combination (Make one combination from each type in a row)
		//success Double rainbow (Ohohoh, double rainbow)
		void sRainbow(int type);
		bool bRainbow;
		bool bDoubleRainbow;
		int succEveryTypeInARow[8];

		//success Lucky Luke (Get more than 1 combi by 5 sec during 30 sec)
		void sLuckyLuke();
		bool bLuckyLuke;
		float timeTotalPlayed, timeUserInputloop; // timeLLloop used in userinput

		//success Donator (Click on Ads)
		void sDonator();
		bool bDonator;

		//success Test everything (Get one score in each mode and difficulty)
		void sTestEverything(StorageAPI* str);
		bool bTestEverything;

		//success Beat them all (classic) (Get a better score than 5 current top scores in classic gamemode)
		void sBTAC(StorageAPI* storage, int difficulty, unsigned int points);
		bool bBTAC;

		//success Beat them all (MODE2) (Get a better score than 5 current top scores in MODE2 gamemode)
		void sBTAM(StorageAPI* storage, int difficulty, float time);
		bool bBTAM;

		//success 666 Loser ! (Lose 3 classic game at level 6 in a row)
		void s666Loser(int level);
		bool b666Loser;
		int l666numberLose;

		//success They're too good (Don't get into the top 5 during 3 consecutive games)
		void sTheyGood(bool imgood);
		bool bTheyGood;
		int lTheyGood;

		//success What I gonna do ? (Keep a leaf in hand during 5 sec)
		void sWhatToDo(bool swapInPreparation, float dt);
		bool bWhatToDo;
		float timeInSwappingPreparation;

		//success Bim Bam Boum (Get 3 differents combination by moving only one leaf)
		void sBimBamBoum(int count);
		bool bBimBamBoum;
		int numberCombinationInARow;

		//success Double in one (Make a double combination by switching 2 leaves)
		void sDoubleInOne(std::vector<Combinais> &s);
		bool bDoubleInOne;
};
