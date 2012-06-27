/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

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
#include <base/Vector2.h>


struct Feuille {
	int X, Y;
	Entity entity;
	int type;
};

struct Combinais {
	std::vector<Vector2> points;
	int type;
};

struct CellFall {
	CellFall(Entity _e, int _x=0, int fY=0, int tY=0) : e(_e), x(_x), fromY(fY), toY(tY) {}
	Entity e;
	int x;
	int fromY, toY;
};

struct GridComponent {
	GridComponent() {
		i = -1 ;
		j = -1;
		type = -1;
		checkedV = false;
		checkedH = false;
	}
	PERSISTENT_PROP int i;
	PERSISTENT_PROP int j;
	PERSISTENT_PROP int type;
	RUNTIME_PROP bool checkedV;
	RUNTIME_PROP bool checkedH;
};

#define theGridSystem GridSystem::GetInstance()
#define GRID(e) theGridSystem.Get(e)

UPDATABLE_SYSTEM(Grid)

public:

/* Return the Entity in pos (i,j)*/
Entity GetOnPos(int i, int j);

/* Return the finale list of actual combinations (no switch needed)*/
std::vector<Combinais> LookForCombination(bool markAsChecked, bool useChecked);

/* Set Back all entity at "not checked"*/
void ResetTest();

/* Return combinaisons without twice the same point*/
std::vector<Combinais> MergeCombination(std::vector<Combinais> combinaisons);

/* Return true if an element is in both vector */
bool Intersec(std::vector<Vector2> v1, std::vector<Vector2> v2);

/* Return true if e is in v*/
bool InVect(std::vector<Vector2> v, Vector2 e);

/* Merge 2 vector*/
Combinais MergeVectors(Combinais c1, Combinais c2);

/* Leaves fall if nothing below them */
std::vector<CellFall> TileFall();

/* Returns points (x,y) which generate new Combi */
std::vector<Vector2> LookForCombinationsOnSwitchVertical();
std::vector<Vector2> LookForCombinationsOnSwitchHorizontal();

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
void HideAll(bool activate);

bool IsValidGridPosition(int i, int j) {
	return (i>=0 && j>=0 && i<GridSize && j<GridSize);
}

/* Is leaf in position i,j in a combination (real grid or in voisinsType configuration) ? */
bool GridPosIsInCombination(int i, int j, int type, int* voisinsType);

/* Clean the Grid from entities */
void DeleteAll();

void print();

int GridSize, Types;
int nbmin;
};
