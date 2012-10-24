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
#include <UnitTest++.h>

#include "GridSystem.h"

static void initGrid(char* type, int size)
{
	GridSystem::CreateInstance();
	theGridSystem.GridSize = size;
	theGridSystem.nbmin = 3;
	for(int j=size-1; j>=0; j--) {
		for(int i=0; i<size; i++) {
			char t = *type++;
			if (t != '_') {
				Entity e =  i * size + j + 1;
				theGridSystem.Add(e);
				GRID(e)->i = i;
				GRID(e)->j = j;
				GRID(e)->type = t;
			}
		}
	}
}

TEST(RowCombination)
{
	char grid[] = {
		'A', 'B', 'B', 'B',
		'C', 'A', 'B', 'C',
		'A', 'B', 'A', 'C',
		'A', 'B', 'A', 'A'
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(true,true);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'B');
	GridSystem::DestroyInstance();
}

TEST(ColCombination)
{
	char grid[] = {
		'A', 'B', 'C', 'B',
		'C', 'A', 'B', 'C',
		'A', 'B', 'A', 'C',
		'A', 'B', 'A', 'C'
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(true,true);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'C');
	GridSystem::DestroyInstance();
}

TEST(MultipleCombination)
{
	char grid[] = {
		'A', 'A', 'A', 'A',
		'C', 'A', 'B', 'C',
		'C', 'B', 'B', 'B',
		'C', 'B', 'A', 'C'
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(true,true);
	CHECK_EQUAL(combinaisons.size(), 3);
	GridSystem::DestroyInstance();
}

TEST(CombinationMergeL)
{
	char grid[] = {
		'A', 'A', 'A', 'B',
		'C', 'A', 'A', 'C',
		'A', 'B', 'A', 'B',
		'A', 'B', 'A', 'C'
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(true,true);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'A');
	CHECK_EQUAL(combinaisons[0].points.size(), 6);
	GridSystem::DestroyInstance();
}

TEST(CombinationMergeN)
{
	char grid[] = {
		'X', 'X', 'X', 'B',
		'X', 'X', 'X', 'C',
		'A', 'B', 'A', 'B',
		'A', 'B', 'A', 'C',
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(true,true);
	CHECK_EQUAL(combinaisons.size(), 2);
	CHECK_EQUAL(combinaisons[0].type, 'X');
	CHECK_EQUAL(combinaisons[1].type, 'X');
	GridSystem::DestroyInstance();
}

TEST(GridFall)
{
	char grid[] = {
		'A', 'B', 'B',
		'_', '_', 'B',
		'_', 'B', 'A'
	};
	initGrid(grid, 3);
	std::vector<CellFall> falls = theGridSystem.TileFall();
	CHECK_EQUAL(2, falls.size());
	if (falls[0].x ==0){
		CHECK_EQUAL(0, falls[0].x);
		CHECK_EQUAL(2, falls[0].fromY);
		CHECK_EQUAL(0, falls[0].toY);
	} else {
		CHECK_EQUAL(0, falls[1].x);
		CHECK_EQUAL(2, falls[1].fromY);
		CHECK_EQUAL(0, falls[1].toY);
	}
	GridSystem::DestroyInstance();
}

TEST(MultipleFalls)
{
	char grid[] = {
		'A', 'B', 'B', 'C',
		'C', '_', 'B', 'D',
		'D', 'B', 'A', 'A',
		'D', '_', 'A', 'A'
	};
	initGrid(grid, 4);
	std::vector<CellFall> falls = theGridSystem.TileFall();
	CHECK_EQUAL(2, falls.size());
	if (falls[0].fromY == 1) {
		CHECK_EQUAL(0, falls[0].toY);
		CHECK_EQUAL(1, falls[1].toY);
	} else {
		CHECK_EQUAL(1, falls[0].toY);
		CHECK_EQUAL(0, falls[1].toY);
	}
	GridSystem::DestroyInstance();
}

TEST(MultipleNewCombiOnFalls)
{
	char grid[] = {
		'D', 'A', 'B', 'C',
		'C', 'B', 'A', 'A',
		'C', 'D', 'D', 'C',
		'A', 'C', 'A', 'A'
	};
	initGrid(grid, 4);

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,true);
	CHECK_EQUAL(combinaisons.size(), 0);
	// swap (0,0) and (0,1)
	Entity e1 = theGridSystem.GetOnPos(0,0);
	Entity e2 = theGridSystem.GetOnPos(1,0);
	GRID(e1)->i = 1;
	GRID(e2)->i = 0;


	combinaisons = theGridSystem.LookForCombination(false,true);
	CHECK_EQUAL(2, combinaisons.size());
	for (int i=0;i<combinaisons.size();i++)
		for (int j=0;j<combinaisons[i].points.size();j++)
			theGridSystem.Delete(theGridSystem.GetOnPos(combinaisons[i].points[j].X, combinaisons[i].points[j].Y));

	std::vector<CellFall> falls = theGridSystem.TileFall();
	for (int i=0;i<falls.size();i++)
		GRID(falls[i].e)->j = falls[i].toY;

	combinaisons = theGridSystem.LookForCombination(false,true);
	CHECK_EQUAL(1, combinaisons.size());
	GridSystem::DestroyInstance();

}

TEST(NoMoreCombinations)
{
	char grid[] = {
		'A', 'B', 'A', 'M',
		'C', 'L', 'B', 'C',
		'C', 'D', 'Y', 'C',
		'A', 'B', 'A', 'P'
	};
	initGrid(grid, 4);

	bool combinaisons = theGridSystem.StillCombinations();
	CHECK_EQUAL(combinaisons, false);
	GridSystem::DestroyInstance();
}
