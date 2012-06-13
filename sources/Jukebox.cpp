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
#include "Jukebox.h"
#include "base/MathUtil.h"
#include "base/Log.h"
#include <algorithm>
#include <assert.h>

enum Theme {
    C=0, D, I
};

static std::string themes[] = {
    "audio/C.ogg",
    "audio/D.ogg",
    "audio/I.ogg"
};

enum Accomp {
    A=0, B, G, E
};


static std::string accomp[] = {
    "audio/A.ogg",
    "audio/B.ogg",
    "audio/G.ogg",
    "audio/E.ogg"
};

static void randomNumbersInRange(int fromIncl, int toIncl, int* out, int count, int incomp1, int incomp2) {
    bool i1Used = false, i2Used = false;
    out[0] = MathUtil::RandomIntInRange(fromIncl, toIncl + 1);
    if (out[0] == incomp1) i1Used = true;
    else if (out[0] == incomp2) i2Used = true;
    for (int i=1; i<count ;i++) {
        bool equalToPrevious;
        do {
            equalToPrevious = false;
            out[i] = MathUtil::RandomIntInRange(fromIncl, toIncl + 1);
            if (i1Used && out[i] == incomp2) {
	            equalToPrevious = true;
            } else if (i2Used && out[i] == incomp1) {
	            equalToPrevious = true;
	        } else {
	            for (int j=0; j<i; j++) {
	                if (out[i] == out[j]) {
	                    equalToPrevious = true;
	                    break;
	                }
	            }
	        }
        } while (equalToPrevious);
        if (out[i] == incomp1) i1Used = true;
    	else if (out[i] == incomp2) i2Used = true;
    }
}

static void build1SongComposition(std::vector<std::string>& selection) {
    selection.push_back(accomp[A]);
}

static void build2SongsComposition(std::vector<std::string>& selection) {
    // theme (excl. I)
    selection.push_back(themes[MathUtil::RandomInt(2)]);
    // accomp (excl. E)
    selection.push_back(accomp[MathUtil::RandomInt(3)]);
}

static void build3SongsComposition(std::vector<std::string>& selection) {
	if (MathUtil::RandomInt(2)) {
	    // 1 theme (excl. I)
	    selection.push_back(themes[MathUtil::RandomInt(2)]);
	    // 2 diff accomp
	    int a[2];
	    
	    randomNumbersInRange(A, E, a, 2, A, G);
	    for (int i=0; i<2; i++) {
	        selection.push_back(accomp[a[i]]);
	    }
	} else {
	    // 2 themes
	    int a[2];
	    randomNumbersInRange(0, 2, a, 2, -1, -1);
	    for (int i=0; i<2; i++)
	        selection.push_back(themes[a[i]]);
	    // 1 accomp
		selection.push_back(accomp[MathUtil::RandomInt(E)]);
	}
}

static void build4SongsComposition(std::vector<std::string>& selection) {
    if (MathUtil::RandomInt(2)) {
        // 3 theme
        int t[3];
        randomNumbersInRange(0, 2, t, 3, -1, -1);
        for (int i=0; i<3; i++) {
            selection.push_back(themes[t[i]]);
        }
        // 1 diff accomp (excl E)
        selection.push_back(accomp[MathUtil::RandomInt(E)]);
    } else {
        // 2 theme
        int t[2];
        randomNumbersInRange(0, 2, t, 2, -1, -1);
        for (int i=0; i<2; i++) {
            selection.push_back(themes[t[i]]);
        }

        // 2 diff accomp
        int a[2];
        randomNumbersInRange(A, E, a, 2, A, G);
        for (int i=0; i<2; i++) {
            selection.push_back(accomp[a[i]]);
        }
    }
}



// A ou H
// sinon 1+ thème et 1+ acc
// et E/I ne peuvent pas être le seul acc

static void initSelection(std::vector<std::string>& selection, int songCount) {
    switch (songCount) {
        case 2:
            build2SongsComposition(selection);
            break;
        case 3:
            build3SongsComposition(selection);
            break;
        case 4:
            build4SongsComposition(selection);
            break;
        case 1:
        default:
            build1SongComposition(selection);
            break;

    }
}

struct IsNotIn {
    std::vector<std::string>* other;
    IsNotIn(std::vector<std::string>* pO) : other(pO) {}

    bool operator()(const std::string& v) const {
    	assert (other->size() > 0);
        for (unsigned int i=0; i<other->size(); i++) {
            if ((*other)[i] == v) {
                return false;
            }
        }

        return true;
    }
};

const std::vector<std::string>& Jukebox::pickNextSongs(int maxSongCount) {
    if (currentSelection.empty()) {
	    int songCount  = MathUtil::RandomInt(maxSongCount) + 1;
        initSelection(currentSelection, songCount);
    } else {
	    int songCount = 0;
    	switch (currentSelection.size()) {
	    	case 1:
	    		songCount = 2;
	    		break;
	    	case 2: {
	    		// bias toward 2 or 3
	    		int r = MathUtil::RandomInt(5);
	    		if (r==0)
	    			songCount = 1;
	    		else if (r < 3)
		    		songCount = 2;
	    		else
	    			songCount = 3;
	    		break;
	    	}
	    	default:
	    		songCount = MathUtil::Min(maxSongCount, MathUtil::RandomIntInRange(currentSelection.size() - 1, currentSelection.size() + 2));
	    		break;
    	}
    
        std::vector<std::string> newSelection;
        int maxTries = 100;
        do {
            newSelection.clear();
            initSelection(newSelection, songCount);
			// count elements in A which are not in B
            int cA = std::count_if(newSelection.begin(), newSelection.end(), IsNotIn(&currentSelection));
            // count elements in B which are not in A
            int cB = std::count_if(currentSelection.begin(), currentSelection.end(), IsNotIn(&newSelection));

            if (cA == 1 && cB <= 1) {
                break;
            }
            if (cB == 1 && cA <= 1) {
                break;
            }
        } while (maxTries --);
        currentSelection = newSelection;
    }
    assert (currentSelection.size() <= (unsigned)maxSongCount);
    return currentSelection;
}
