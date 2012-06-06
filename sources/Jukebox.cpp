#include "Jukebox.h"
#include "base/MathUtil.h"
#include "base/Log.h"
#include <algorithm>
#include <assert.h>

enum Theme {
    A=0, B, I
};

static std::string themes[] = {
    "audio/A.ogg",
    "audio/B.ogg",
    "audio/I.ogg"
};

enum Accomp {
    C=0, D, G, E
};


static std::string accomp[] = {
    "audio/C.ogg",
    "audio/D.ogg",
    "audio/G.ogg",
    "audio/E.ogg"
};

static void randomNumbersInRange(int fromIncl, int toIncl, int* out, int count) {
    out[0] = MathUtil::RandomIntInRange(fromIncl, toIncl + 1);
    for (int i=1; i<count ;i++) {
        bool equalToPrevious;
        do {
            equalToPrevious = false;
            out[i] = MathUtil::RandomIntInRange(fromIncl, toIncl + 1);
            for (int j=0; j<i; j++) {
                if (out[i] == out[j]) {
                    equalToPrevious = true;
                    break;
                }
            }
        } while (equalToPrevious);
    }
}

static void build1SongComposition(std::vector<std::string>& selection) {
    selection.push_back(themes[A]);
}

static void build2SongsComposition(std::vector<std::string>& selection) {
    // theme (excl. I)
    selection.push_back(themes[MathUtil::RandomInt(2)]);
    // accomp (excl. E)
    selection.push_back(accomp[MathUtil::RandomInt(3)]);
}

static void build3SongsComposition(std::vector<std::string>& selection) {
    // 1 theme
    selection.push_back(themes[MathUtil::RandomInt(3)]);
    // 2 diff accomp
    int a[2];
    randomNumbersInRange(0, 3, a, 2);
    for (int i=0; i<2; i++) {
        selection.push_back(accomp[a[i]]);
    }
}

static void build4SongsComposition(std::vector<std::string>& selection) {
    if (MathUtil::RandomInt(2)) {
        // 1 theme
        selection.push_back(themes[MathUtil::RandomInt(3)]);
        // 3 diff accomp
        int a[3];
        randomNumbersInRange(0, 3, a, 3);
        for (int i=0; i<3; i++) {
            selection.push_back(accomp[a[i]]);
        }
    } else {
        // 2 theme
        int t[2];
        randomNumbersInRange(0, 2, t, 2);
        for (int i=0; i<2; i++) {
            selection.push_back(themes[t[i]]);
        }

        // 2 diff accomp
        int a[2];
        randomNumbersInRange(0, 3, a, 2);
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
