#include <iostream>
#include "GridSystem.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
				
				
INSTANCE_IMPL(GridSystem);
	
GridSystem::GridSystem() : ComponentSystem<GridComponent>("Grid") { 
	GridSize=8;
}

Entity GridSystem::GetOnPos(int i, int j) {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;			
		GridComponent* bc = (*it).second;
		if (bc->i == i && bc->j == j)
			return a;
	}
	//std::cout << "Aucun element en position (" << i << ","<<j<<")\n";
	return 0;
}
	
void GridSystem::ResetTest() {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;			
		GridComponent* bc = (*it).second;
		bc->checkedH = false;
		bc->checkedV = false;
	}
}

bool GridSystem::Intersec(std::vector<Vector2> v1, std::vector<Vector2> v2){
	for ( size_t i = 0; i < v1.size(); ++i ) {
		for ( size_t j = 0; j < v2.size(); ++j ) {
			if (v1[i] == v2[j])
				return true;
		}
	}
	return false;
}

bool GridSystem::InVect(std::vector<Vector2> v1, Vector2 v2){
	for ( size_t i = 0; i < v1.size(); ++i ) {
		if (v1[i] == v2)
			return true;
	}
	return false;
}

Combinais GridSystem::MergeVectors(Combinais c1, Combinais c2) {
	Combinais merged;
	merged = c1;
	for (size_t i=0; i<c2.points.size();i++) {
		if (!InVect(c1.points,c2.points[i]))
			merged.points.push_back(c2.points[i]);
	}
	return merged;
}
	
std::vector<Combinais> GridSystem::MergeCombinaison(std::vector<Combinais> combinaisons) {
	std::vector<Combinais> combinmerged;
		
	for ( size_t i = 0; i < combinaisons.size(); ++i ) {
		int match = -1;
		for ( size_t j = i+1; j < combinaisons.size(); ++j ) {
			if (combinaisons[i].type != combinaisons[j].type || !Intersec(combinaisons[i].points, combinaisons[j].points)) {
				continue;
			} else {
				match = j;
				combinaisons[j] = MergeVectors(combinaisons[i],combinaisons[j]);
			}
		}
		if (match==-1)
			combinmerged.push_back(combinaisons[i]);
	}
	return combinmerged;
}

std::vector<Combinais> GridSystem::LookForCombinaison(int nbmin) { 
	std::vector<Combinais> combinaisons;

	/* do not look for combis if the grid is not full */
	if (components.size() != GridSize * GridSize)
		return combinaisons;

	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;			
		GridComponent* gc = (*it).second;
		int i=gc->i;
		int j=gc->j;
		Combinais potential;
		potential.type = gc->type;

		/*Check on j*/
		if (!gc->checkedV) {
			Combinais potential;
			potential.type = gc->type;
			/*Looking for twins on the bottom of the point*/
			int k=j;
			while (k>-1){
				Entity next = GetOnPos(i,k);
		
				if (GRID(next)->type != gc->type){
					k=-2;
				} else {
					/*Useless to check them later : we already did it now*/
					potential.points.push_back(Vector2(i,k));
					GRID(next)->checkedV = true;
					k--;
				}
			}
			
			/* Then on the top*/
			k = j+1;
			while (k<GridSize){
				Entity next = GetOnPos(i,k);

				if (GRID(next)->type != gc->type){
					k=GridSize;
				} else {
					GRID(next)->checkedV = true;
					potential.points.push_back(Vector2(i,k));
					k++;
				}
			}
			
				
			/*If there is at least 1 more cell
			 * We add it to the solutions*/
			 
			if (potential.points.size()>=nbmin){
				combinaisons.push_back(potential);
			}
			
			gc->checkedV = true;
		} 
		
		/*Check on i*/
		if (!gc->checkedH) {
			Combinais potential;
			potential.type = gc->type;
			
			/*Looking for twins on the left of the point*/
			int k=i;
			while (k>-1){
				Entity next = GetOnPos(k,j);
				
				if (GRID(next)->type != gc->type) {
					k=-2;
				} else {
					/*Useless to check them later : we already did it now*/
					GRID(next)->checkedH = true;
					potential.points.push_back(Vector2(k,j));
					k--;
				}
			}
			
			/* Then on the right*/
			k = i+1;
			while (k<GridSize){
				Entity next = GetOnPos(k,j);

				if (GRID(next)->type != gc->type) {
					k=(GridSize+1);
				} else {
					GRID(next)->checkedH = true;
					potential.points.push_back(Vector2(k,j));
					k++;
				}
			}			

	
			/*If there is at least 1 more cell
			 * We add it to the solutions
			 * longueurCombi < 0 <-> Horizontale */
			 
			if (potential.points.size()>=nbmin){
				combinaisons.push_back(potential);
			}
			
			gc->checkedH = true;
		} 
		
		
	}
	
	return MergeCombinaison(combinaisons);
}

std::vector<CellFall> GridSystem::TileFall() {
	std::vector<CellFall> result;

	for (int i=0; i<GridSize; i++) {
		for (int j=0; j<GridSize; j++) {
			/* if call is empty, find nearest non empty cell above*/
			if (!GetOnPos(i,j)){
				int k=j+1;
				while (k<GridSize){
					Entity e = GetOnPos(i, k);
					if (e) {
						int fallHeight = k - j;
						while (k < GridSize) {
							result.push_back(CellFall(GetOnPos(i, k), i, k, k - fallHeight));
							k++;
						}
						break;
					} else {
						k++;
					}
				}
				/* only one fall possible per column */
				break;
			}
		}
	}
	return result;
}
void GridSystem::DoUpdate(float dt) {
	std::vector<Combinais> combinaisons;
	combinaisons = LookForCombinaison(3);

	if (combinaisons.size()>0){
		for ( std::vector<Combinais>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
		{
			std::cout << it->points.size();
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV )
			{
				std::cout << "\t(" <<itV->X << ", "<< itV->Y << ")";
			}
			std::cout << std::endl;
			
		}
	}
	
	combinaisons.clear();	
}


