#pragma once
#include <vector>

#include "systems/System.h"
#include "base/MathUtil.h"
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
	int i, j, type;
	bool checkedV, checkedH;

};

#define theGridSystem GridSystem::GetInstance()
#define GRID(e) theGridSystem.Get(e)

UPDATABLE_SYSTEM(Grid)

public:

/* Return the Entity in pos (i,j)*/
Entity GetOnPos(int i, int j);

/* Return the finale list  of combinaisons*/ 
std::vector<Combinais> LookForCombinaison(int nbmin);

/* Set Back all entity at "not checked"*/
void ResetTest();

/* Return merged combinaisons */
std::vector<Combinais> MergeCombinaison(std::vector<Combinais> combinaisons);

/* Return true if an element is in both vector */
bool Intersec(std::vector<Vector2> v1, std::vector<Vector2> v2);

/* Return true if v2 is in v1*/
bool InVect(std::vector<Vector2> v1, Vector2 v2);

/* Merge 2 vector*/
Combinais MergeVectors(Combinais c1, Combinais c2);

/* Leaves fall if nothing below them */
std::vector<CellFall> TileFall();

int GridSize;
};
