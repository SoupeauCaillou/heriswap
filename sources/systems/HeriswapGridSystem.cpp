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


#include "HeriswapGridSystem.h"

#include <iostream>

#include "systems/System.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "util/Serializer.h"
#include "util/Random.h"
INSTANCE_IMPL(HeriswapGridSystem);

HeriswapGridSystem::HeriswapGridSystem() : ComponentSystemImpl<HeriswapGridComponent>("HeriswapGrid") {
    GridSize = Types = 8;
    nbmin = 3;
    HeriswapGridComponent a;
    componentSerializer.add(new Property<int>(HASH("i", 0x87ea58bf), OFFSET(i, a)));
    componentSerializer.add(new Property<int>(HASH("j", 0xfe3dcbb), OFFSET(j, a)));
    componentSerializer.add(new Property<int>(HASH("type", 0xf3ebd1bf), OFFSET(type, a)));
}

Difficulty HeriswapGridSystem::sizeToDifficulty() {
    if (GridSize == 5)
        return DifficultyEasy;
    else if (GridSize == 6)
        return DifficultyMedium;
    else
        return DifficultyHard;
}

int HeriswapGridSystem::difficultyToSize(Difficulty diff) {
    if (diff == DifficultyEasy)
        return 5;
    else if (diff == DifficultyMedium)
        return 6;
    else
        return 8;
}

void HeriswapGridSystem::setGridFromDifficulty(Difficulty diff) {
    if (diff == DifficultyEasy)
        GridSize = Types = 5;
    else if (diff == DifficultyMedium)
        GridSize = Types = 6;
    else
        GridSize = Types = 8;
}

Difficulty HeriswapGridSystem::nextDifficulty(Difficulty diff) {
    switch (diff) {
        case DifficultyEasy :
            return DifficultyMedium;
        case DifficultyMedium :
            return DifficultyHard;
        case DifficultyHard :
            return DifficultyEasy;
        default:
            break;
    }
    //should never happen
    return DifficultyEasy;
}


void HeriswapGridSystem::print() {
    for(int j=GridSize-1; j>=0; j--) {
        for(int i=0; i<GridSize; i++) {
            Entity e = GetOnPos(i, j);
            if (e) {
                char t = HERISWAPGRID(e)->type;
                std::cerr << t << " ";
            } else
                std::cerr << "_ ";
        }
        std::cerr << std::endl;
    }
}

void HeriswapGridSystem::ShowAll(bool activate) {
    forEachEntityDo([activate] (Entity e) -> void {
        RENDERING(e)->show = activate;
    });
}

void HeriswapGridSystem::DeleteAll() {
    std::vector<Entity> all = this->RetrieveAllEntityWithComponent();
    for (unsigned int i=0; i<all.size(); i++) {
        theEntityManager.DeleteEntity(all[i]);
    }
}

Entity HeriswapGridSystem::GetOnPos(int i, int j) {
    Entity a = 0;
    forEachECDo([&a, i, j] (Entity e, HeriswapGridComponent* bc ) -> void {
        if (bc->i == i && bc->j == j) {
            a = e;
        }
    });
    return a;
}

void HeriswapGridSystem::ResetTest() {
    forEachECDo([] (Entity, HeriswapGridComponent* bc ) -> void {
        bc->checkedH = false;
        bc->checkedV = false;
    });
}

bool HeriswapGridSystem::Intersec(std::vector<glm::vec2> v1, std::vector<glm::vec2> v2){
    for ( size_t i = 0; i < v1.size(); ++i ) {
        for ( size_t j = 0; j < v2.size(); ++j ) {
            if (v1[i] == v2[j])
                return true;
        }
    }
    return false;
}

bool HeriswapGridSystem::InVect(std::vector<glm::vec2> v1, glm::vec2 v2){
    for ( size_t i = 0; i < v1.size(); ++i ) {
        if (v1[i] == v2)
            return true;
    }
    return false;
}

Combinais HeriswapGridSystem::MergeVectors(Combinais c1, Combinais c2) {
    Combinais merged;
    merged = c1;
    for (size_t i=0; i<c2.points.size();i++) {
        if (!InVect(c1.points,c2.points[i]))
            merged.points.push_back(c2.points[i]);
    }
    return merged;
}

std::vector<Combinais> HeriswapGridSystem::MergeCombination(std::vector<Combinais> combinaisons) {
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

std::vector<Combinais> HeriswapGridSystem::LookForCombination(bool markAsChecked, bool recheckEveryone) {
    std::vector<Combinais> combinaisons;

    forEachECDo([this, &combinaisons, markAsChecked, recheckEveryone] (Entity, HeriswapGridComponent* gc) -> void {
        int i=gc->i;
        int j=gc->j;
        Combinais potential;
        potential.type = gc->type;

        /*Check on j*/
        if (!recheckEveryone || !gc->checkedV) {
            Combinais potential;
            potential.type = gc->type;
            /*Looking for twins on the bottom of the point*/
            int k=j;
            while (k>-1){
                Entity next = GetOnPos(i,k);

                if (!next || HERISWAPGRID(next)->type != gc->type) {
                    k=-2;
                } else {
                    /*Useless to check them later : we already did it now*/
                    potential.points.push_back(glm::vec2(i,k));
                    if (markAsChecked) HERISWAPGRID(next)->checkedV = true;
                    k--;
                }
            }

            /* Then on the top*/
            k = j+1;
            while (k<GridSize){
                Entity next = GetOnPos(i,k);



                if (!next || HERISWAPGRID(next)->type != gc->type){
                    k=GridSize;
                } else {
                    if (markAsChecked) HERISWAPGRID(next)->checkedV = true;
                    potential.points.push_back(glm::vec2(i,k));
                    k++;
                }
            }


            /*If there is at least 1 more cell
             * We add it to the solutions*/

            if ((int)potential.points.size()>=nbmin){
                combinaisons.push_back(potential);
            }

            if (markAsChecked) gc->checkedV = true;
        }

        /*Check on i*/
        if (!recheckEveryone || !gc->checkedH) {
            Combinais potential;
            potential.type = gc->type;

            /*Looking for twins on the left of the point*/
            int k=i;
            while (k>-1){
                Entity next = GetOnPos(k,j);

                if (!next || HERISWAPGRID(next)->type != gc->type) {
                    k=-2;
                } else {
                    /*Useless to check them later : we already did it now*/
                    if (markAsChecked) HERISWAPGRID(next)->checkedH = true;
                    potential.points.push_back(glm::vec2(k,j));
                    k--;
                }
            }

            /* Then on the right*/
            k = i+1;
            while (k<GridSize){
                Entity next = GetOnPos(k,j);

                if (!next || HERISWAPGRID(next)->type != gc->type) {
                    k=(GridSize+1);
                } else {
                    if (markAsChecked) HERISWAPGRID(next)->checkedH = true;
                    potential.points.push_back(glm::vec2(k,j));
                    k++;
                }
            }


            /*If there is at least 1 more cell
             * We add it to the solutions
             * longueurCombi < 0 <-> Horizontale */

            if ((int)potential.points.size()>=nbmin){
                combinaisons.push_back(potential);
            }

            if (markAsChecked) gc->checkedH = true;
        }
    });

    return MergeCombination(combinaisons);
}

std::vector<CellFall> HeriswapGridSystem::TileFall() {
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
                            Entity e = GetOnPos(i, k);
                            if (e)
                                result.push_back(CellFall(e, i, k, k - fallHeight));
                            else fallHeight++;
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

void HeriswapGridSystem::DoUpdate(float) {
}

bool HeriswapGridSystem::NewCombiOnSwitch(Entity a, int i, int j) {
    //test right and top
    Entity e = GetOnPos(i+1,j);
    if (e) {
        HERISWAPGRID(e)->i--;
        HERISWAPGRID(a)->i++;
        HERISWAPGRID(e)->checkedH =
            HERISWAPGRID(a)->checkedH =
            HERISWAPGRID(e)->checkedV =
            HERISWAPGRID(a)->checkedV = false;
        std::vector<Combinais> combin = LookForCombination(true,true);
        HERISWAPGRID(e)->i++;
        HERISWAPGRID(a)->i--;
        if (combin.size()>0) return true;
    }
    e = GetOnPos(i,j+1);
    if (e) {
        HERISWAPGRID(e)->j--;
        HERISWAPGRID(a)->j++;
        HERISWAPGRID(e)->checkedH =
            HERISWAPGRID(a)->checkedH =
            HERISWAPGRID(e)->checkedV =
            HERISWAPGRID(a)->checkedV = false;
        std::vector<Combinais> combin = LookForCombination(true,true);
        HERISWAPGRID(e)->j++;
        HERISWAPGRID(a)->j--;
        if (combin.size()>0) return true;
    }
    return false;
}

void HeriswapGridSystem::SetCheckInCombi(std::vector<Combinais> c) {
    for (std::vector<Combinais>::reverse_iterator itc = c.rbegin(); itc != c.rend(); ++itc) {
        for (std::vector<glm::vec2>::reverse_iterator it = itc->points.rbegin(); it != itc->points.rend(); ++it) {
            HERISWAPGRID(GetOnPos(it->x, it->y))->checkedV =
                HERISWAPGRID(GetOnPos(it->x,it->y))->checkedH = false;
        }
    }
}

bool HeriswapGridSystem::StillCombinations() {
    //on utilise les checked pour pas recalculer toute la grille à chaque coup, apres on va juste en switch 2 à chaque fois donc les nouvelels combi
    //peuvent etre qu'au niveau du switch. A la fin, on remet la grille comme au debut.
    std::vector<Combinais> combin = LookForCombination(true,true);
    if (combin.size()>0) {
        SetCheckInCombi(combin);
        return true;
    }

#if SAC_USE_VECTOR_STORAGE
    for (auto e: entityWithComponent) {
        const auto* comp = &components[e];
#else
    FOR_EACH_ENTITY_COMPONENT(Background, e, comp)
#endif
        if (NewCombiOnSwitch(e, comp->i, comp->j)) {
            SetCheckInCombi(combin);
            return true;
        }
    }
    SetCheckInCombi(combin);
    return false;
}

std::vector<glm::vec2> HeriswapGridSystem::LookForCombinationsOnSwitchVertical() {
    std::vector<glm::vec2> combin;
    for (int i=0; i<GridSize; i++) {
        for (int j=0; j<GridSize-1; j++) {
            Entity a = GetOnPos(i,j);
            Entity e = GetOnPos(i,j+1);
            //si on a une nouvelle combi parmi les 8 possibles (1vert,3hori chacun pour e, et pour a)
            if ((j>=2 && GetOnPos(i,j-1) && GetOnPos(i,j-2) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j-1))->type && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j-2))->type)
            || (i>1 && GetOnPos(i-1,j) && GetOnPos(i-2,j) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i-1,j))->type && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i-2,j))->type)
            || (i>0 && GetOnPos(i+1,j) && GetOnPos(i-1,j) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i-1,j))->type && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i+1,j))->type)
            || (i<GridSize-2 && GetOnPos(i+1,j) && GetOnPos(i+2,j) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i+2,j))->type && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i+1,j))->type)
            || (j<GridSize-3 && GetOnPos(i,j+2) && GetOnPos(i,j+3) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i,j+2))->type && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i,j+3))->type)
            || (i>1 && GetOnPos(i-1,j+1) && GetOnPos(i-2,j+1) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i-1,j+1))->type && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i-2,j+1))->type)
            || (i>0 && GetOnPos(i+1,j+1) && GetOnPos(i-1,j+1) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i-1,j+1))->type && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j+1))->type)
            || (i<GridSize-2 && GetOnPos(i+1,j+1) && GetOnPos(i+2,j+1) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+2,j+1))->type && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j+1))->type))
                combin.push_back(glm::vec2(i, j));
        }
    }
    return combin;
}

std::vector<glm::vec2> HeriswapGridSystem::LookForCombinationsOnSwitchHorizontal() {
    std::vector<glm::vec2> combin;
    for (int i=0; i<GridSize-1; i++) {
        for (int j=0; j<GridSize; j++) {
            Entity a = GetOnPos(i,j);
            Entity e = GetOnPos(i+1,j);
            if ((i>=2 && GetOnPos(i-1,j) && GetOnPos(i-2,j) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i-1,j))->type &&  HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i-2,j))->type)
            || (j>1 && GetOnPos(i,j-1) && GetOnPos(i,j-2) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j-1))->type &&   HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j-2))->type)
            || (j>0 && GetOnPos(i,j+1) && GetOnPos(i,j-1) &&     HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j-1))->type &&   HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j+1))->type)
            || (j<GridSize-2 && GetOnPos(i,j+1) && GetOnPos(i,j+2) && HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j+2))->type &&   HERISWAPGRID(e)->type == HERISWAPGRID(GetOnPos(i,j+1))->type)
            || (i<GridSize-3 && GetOnPos(i+2,j) && GetOnPos(i+3,j) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+2,j))->type &&  HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+3,j))->type)
            || (j>1 && GetOnPos(i+1,j-1) && GetOnPos(i+1,j-2) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j-1))->type &&  HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j-2))->type)
            || (j>0 && GetOnPos(i+1,j+1) && GetOnPos(i+1,j-1) &&     HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j-1))->type && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j+1))->type)
            || (j<GridSize-2 && GetOnPos(i+1,j+1) && GetOnPos(i+1,j+2) && HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j+2))->type &&  HERISWAPGRID(a)->type == HERISWAPGRID(GetOnPos(i+1,j+1))->type))
                combin.push_back(glm::vec2(i, j));
        }
    }
    return combin;
}

std::vector< std::vector<Entity> > HeriswapGridSystem::GetSwapCombinations() {
    std::vector< std::vector<Entity> > res;
    for (int i = 0; i < GridSize; i++) {
        for (int j = 0; j < GridSize; j++) {
            Entity e = GetOnPos(i,j);
            //if it makes a combi on its left, search the combination
            std::vector<Entity> n = getCombiEntitiesInLine(e, i-1, j, 0);
            if (n.size() > 1)
                res.push_back(n);
            //if it makes a combi on its right, search the combination
            n = getCombiEntitiesInLine(e, i+1, j, 2);
            if (n.size() > 1)
                res.push_back(n);
            //or below its
            n = getCombiEntitiesInLine(e, i, j-1, 1);
            if (n.size() > 1)
                res.push_back(n);
            //or above its
            n = getCombiEntitiesInLine(e, i, j+1, 3);
            if (n.size() > 1)
                res.push_back(n);
        }
    }
    return res;
}

std::vector<Entity> HeriswapGridSystem::getCombiEntitiesInLine(Entity a, int i, int j, int move) {
    std::vector<Entity> res;
    if (i < 0 || i == GridSize || j < 0 || j == GridSize)
        return res;

    int type = HERISWAPGRID(a)->type;

    //adding itself
    res.push_back(a);


    //if horizontal switch
    if (move % 2 == 0) {
        int cpt = 0;
        int sens = move-1;
        int k = i + sens;
        //first look the line (NB : entity's type in (i,j) != type necessary)
        //so only right in right move, only left in left move
        Entity e = GetOnPos(k, j);
        while (k < GridSize && k >= 0 && HERISWAPGRID(e)->type == type) {
            cpt++;
            k += sens;
            res.push_back(e);
            e = GetOnPos(k, j);
        }

        //so if cpt < 2, there is no line combination and we need to remove entites in res vector
        if (cpt < 2) {
            res.clear();
            res.push_back(a);
        }
        cpt = 0;
        //now look on column
        //look below
        k = j-1;
        e = GetOnPos(i, k);
        while (k >= 0 && HERISWAPGRID(e)->type == type) {
            res.push_back(e);
            k--;
            cpt++;
            e = GetOnPos(i, k);
        }
        //look above
        k=j+1;
        e = GetOnPos(i, k);
        while (k<GridSize && HERISWAPGRID(e)->type == type) {
            res.push_back(e);
            k++;
            cpt++;
            e = GetOnPos(i, k);
        }
        //if cpt < 2, it was only horizontal combination, removes "cpt" last enties from res
        if (cpt < 2) {
            while (cpt > 0) {
                res.pop_back();
                cpt--;
            }
        }
    } else {
        int cpt = 0;
        int sens = move-2;
        int k = j + sens;
        //first look the column (NB : entity's type in (i,j) != type necessary)
        //so only top in top move, only bot in bot move
        Entity e = GetOnPos(i, k);
        while (k < GridSize && k >= 0 && HERISWAPGRID(e)->type == type) {
            cpt++;
            k += sens;
            res.push_back(e);
            e = GetOnPos(i, k);
        }

        //so if cpt < 2, there is no line combination and we need to remove entites in res vector
        if (cpt < 2) {
            res.clear();
            res.push_back(a);
        }
        cpt = 0;
        //now look on line
        //look left
        k = i-1;
        e = GetOnPos(k, j);
        while (k >= 0 && HERISWAPGRID(e)->type == type) {
            res.push_back(e);
            k--;
            cpt++;
            e = GetOnPos(k, j);
        }
        //look right
        k=i+1;
        e = GetOnPos(k, j);
        while (k<GridSize && HERISWAPGRID(e)->type == type) {
            res.push_back(e);
            k++;
            cpt++;
            e = GetOnPos(k, j);
        }
        //if cpt < 2, it was only horizontal combination, removes "cpt" last enties from res
        if (cpt < 2) {
            while (cpt > 0) {
                res.pop_back();
                cpt--;
            }
        }
    }
    return res;
}

bool HeriswapGridSystem::GridPosIsInCombination(int i, int j, int type, int* voisinsType) {
    if (type==-1)
        return false;

    int* vType = voisinsType;
    //if voisinsType is null, test with leaves currently in gridsystem
    if (!vType) {
        Entity voisins[8];

        voisins[0] = GetOnPos(i-2, j);
        voisins[1] = GetOnPos(i-1, j);
        voisins[2] = GetOnPos(i+1, j);
        voisins[3] = GetOnPos(i+2, j);
        voisins[4] = GetOnPos(i, j-2);
        voisins[5] = GetOnPos(i, j-1);
        voisins[6] = GetOnPos(i, j+1);
        voisins[7] = GetOnPos(i, j+2);

        vType = (int*)malloc(sizeof(int)*8);
        for (int i= 0; i<8; i++)
            vType[i] = voisins[i] ? HERISWAPGRID(voisins[i])->type : -1;
    }

    bool res =  (
    //horizontal combis ?
       (type == vType[0] && type == vType[1])
    || (type == vType[1] && type == vType[2])
    || (type == vType[2] && type == vType[3])
    //vertical combis ?
    || (type == vType[4] && type == vType[5])
    || (type == vType[5] && type == vType[6])
    || (type == vType[6] && type == vType[7])
    );

    if (!voisinsType)
        free(vType);

    return res;
}

std::vector<Entity> HeriswapGridSystem::ShowOneCombination() {
    LOGW("Show one 1 combi");
    std::vector<Entity> highLightedCombi;
    //desaturate everything
    std::vector<Entity> leaves = RetrieveAllEntityWithComponent();
    LOGW("Desaturate '"<< leaves.size() << "' leaves");
    for (unsigned int i = 0; i < leaves.size(); i++)
        RENDERING(leaves[i])->effectRef = theRenderingSystem.effectLibrary.load("desaturate.fs");

    //then resature one combi
    std::vector < std::vector<Entity> > c = GetSwapCombinations();
    int i = Random::Int(0, c.size()-1);
    for ( std::vector<Entity>::reverse_iterator it = c[i].rbegin(); it != c[i].rend(); ++it) {
        LOGW("Apply DefaultEffect to entity: '"<< *it << "'");
        RENDERING(*it)->effectRef = DefaultEffectRef;
        highLightedCombi.push_back(*it);
    }

    return highLightedCombi;
}
