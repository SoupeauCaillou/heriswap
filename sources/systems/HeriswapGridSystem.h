/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

    Heriswap is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    Heriswap is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <vector>
#include <base/EntityManager.h>
#include <glm/glm.hpp>
#include <systems/System.h>

//medium is after hard because it would have ruined ppl's score using the game before adding the medium difficulty on android
enum Difficulty {
	SelectAllDifficulty = -1,
	DifficultyEasy = 0,
	DifficultyMedium = 2,
	DifficultyHard = 1
};

struct Feuille {
	int X, Y;
	Entity entity;
	int type;
};

struct Combinais {
	std::vector<glm::vec2> points;
	int type;
};

struct CellFall {
	CellFall(Entity _e, int _x=0, int fY=0, int tY=0) : e(_e), x(_x), fromY(fY), toY(tY) {}
	Entity e;
	int x;
	int fromY, toY;
};

struct HeriswapGridComponent {
	HeriswapGridComponent() {
		i = -1 ;
		j = -1;
		type = -1;
		checkedV = false;
		checkedH = false;
	}
	int i;
	int j;
	int type;
	bool checkedV;
	bool checkedH;
};

#define theHeriswapGridSystem HeriswapGridSystem::GetInstance()
#define HERISWAPGRID(e) theHeriswapGridSystem.Get(e)

UPDATABLE_SYSTEM(HeriswapGrid)

public:

/* Return the Entity in pos (i,j)*/
Entity GetOnPos(int i, int j);

/* Return the finale list of actual combinations (no switch needed)*/
std::vector<Combinais> LookForCombination(bool markAsChecked, bool recheckEveryone);

/* Set Back all entity at "not checked"*/
void ResetTest();

/* Return combinaisons without twice the same point*/
std::vector<Combinais> MergeCombination(std::vector<Combinais> combinaisons);

/* Return true if an element is in both vector */
bool Intersec(std::vector<glm::vec2> v1, std::vector<glm::vec2> v2);

/* Return true if e is in v*/
bool InVect(std::vector<glm::vec2> v, glm::vec2 e);

/* Merge 2 vector*/
Combinais MergeVectors(Combinais c1, Combinais c2);

/* Leaves fall if nothing below them */
std::vector<CellFall> TileFall();

/* Returns points (x,y) which generate new Combi */
std::vector<glm::vec2> LookForCombinationsOnSwitchVertical();
std::vector<glm::vec2> LookForCombinationsOnSwitchHorizontal();

/* Returns all potentials combinations */
std::vector< std::vector<Entity> > GetSwapCombinations();

/* Returns entities in the combination with a moved in (i,j)*/
/* move :
 *      T=3
 * L=0		R=2
 * 		B=1
*/
std::vector<Entity> getCombiEntitiesInLine(Entity a, int i, int j, int move);

/* return true if there is still at least 1 combi by switching 2 entites */
bool StillCombinations();

/* return true if a in (i,j) generates a new combination */
bool NewCombiOnSwitch(Entity a, int i, int j);

/* Reset checkH and checkV to true for entities in c */
void SetCheckInCombi(std::vector<Combinais> c);

/* Hide the grid's entities (pause state) */
void ShowAll(bool activate);

bool IsValidGridPosition(int i, int j) {
	return (i>=0 && j>=0 && i<GridSize && j<GridSize);
}

/* Is leaf in position i,j in a combination (real grid or in voisinsType configuration) ? */
bool GridPosIsInCombination(int i, int j, int type, int* voisinsType);

/* Clean the Grid from entities */
void DeleteAll();

/*Highlight a combination*/
std::vector<Entity> ShowOneCombination();

void print();

Difficulty sizeToDifficulty();

int difficultyToSize(Difficulty diff);

void setGridFromDifficulty(Difficulty diff);

Difficulty nextDifficulty(Difficulty diff);

int GridSize, Types;
int nbmin;
};
