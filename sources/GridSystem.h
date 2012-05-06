#pragma once
#include <vector>
#include <iostream>

#include "base/MathUtil.h"
#include "base/EntityManager.h"

#include "systems/System.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

struct Feuille {
	int X, Y;
	Entity fe;
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

/* Return the finale list  of combinaisons*/
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

/* Clean the Grid from entities */
void DeleteAll();

/* Ces Fonctions ne servent plus en l'etat ... mais elles sont tjs en commentaire au cas où*/
///* Return true if v1 = v2 */
//bool EgalVec(std::vector<Combinais> v1, std::vector<Combinais> v2);

///* Return true if c1 = c2 */
//bool Egal(Combinais c1, Combinais c2);

///*Return all the elements who are in v1 and not in v2 */
//std::vector<Combinais> Diff(std::vector<Combinais> v1, std::vector<Combinais> v2);



void print();

int GridSize, Types;
int nbmin;
};
