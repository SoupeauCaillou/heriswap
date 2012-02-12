#include <UnitTest++.h>

#include "GridSystem.h"

static void initGrid(char* type, int size) 
{
	theGridSystem.Clear();
	theGridSystem.GridSize = size;

	for(int j=size-1; j>=0; j--) {
		for(int i=0; i<size; i++) {
			char t = *type++;
			if (t != '_') {
				Entity e =  i * size + j;
				theGridSystem.Add(e);
				GRID(e)->i = i;
				GRID(e)->j = j;
				GRID(e)->type = t;
			}
			// std::cout << "("<<i<<";"<<j<<") : "<<	t << "\t";
		}
		// std::cout << std::endl;
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

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'B');
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

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'C');
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

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	CHECK_EQUAL(combinaisons.size(), 3);
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

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	CHECK_EQUAL(combinaisons.size(), 1);
	CHECK_EQUAL(combinaisons[0].type, 'A');
	CHECK_EQUAL(combinaisons[0].points.size(), 6);
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

	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	CHECK_EQUAL(combinaisons.size(), 2);
	CHECK_EQUAL(combinaisons[0].type, 'X');
	CHECK_EQUAL(combinaisons[1].type, 'X');
}

TEST(GridFall)
{
	char grid[] = {
		'A', 'B', 'B',
		'_', 'A', 'B',
		'_', 'B', 'A'
	};
	initGrid(grid, 3);
	std::vector<CellFall> falls = theGridSystem.TileFall();
	CHECK_EQUAL(1, falls.size());
	CHECK_EQUAL(0, falls[0].x);
	CHECK_EQUAL(2, falls[0].fromY);
	CHECK_EQUAL(0, falls[0].toY);
}
