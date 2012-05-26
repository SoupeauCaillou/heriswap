#include "Jukebox.h"
#include "base/MathUtil.h"
#include "base/Log.h"
#include <algorithm>
#include <assert.h>

#define STR(tok) "audio/"#tok".ogg"
#define L(a) STR(a)

enum Theme {
    A=0, B, I
};

static std::string themes[] = {
    "audio/A.ogg",
    "audio/B.ogg",
    "audio/I.ogg"
};

enum Accomp {
    C=0, D, G, H, E
};


static std::string accomp[] = {
    "audio/C.ogg",
    "audio/D.ogg",
    "audio/G.ogg",
    "audio/H.ogg",
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
    if (MathUtil::RandomInt(2)) {
        selection.push_back(themes[A]);
    } else {
        selection.push_back(accomp[H]);
    }
}

static void build2SongsComposition(std::vector<std::string>& selection) {
    // theme (excl. I)
    selection.push_back(themes[MathUtil::RandomInt(2)]);
    // accomp (excl. E)
    selection.push_back(accomp[MathUtil::RandomInt(4)]);
}

static void build3SongsComposition(std::vector<std::string>& selection) {
    // 1 theme
    selection.push_back(themes[MathUtil::RandomInt(3)]);
    // 2 diff accomp
    int a[2];
    randomNumbersInRange(0, 4, a, 2);
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
        randomNumbersInRange(0, 4, a, 3);
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
        randomNumbersInRange(0, 4, a, 2);
        for (int i=0; i<2; i++) {
            selection.push_back(accomp[a[i]]);
        }
    }
}



// A ou H
// sinon 1+ thème et 1+ acc
// et E/I ne peuvent pas être le seul acc

static void initSelection(std::vector<std::string>& selection, int maxSongCount) {
    int count = MathUtil::RandomInt(maxSongCount) + 1;
    switch (count) {
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

    bool operator()(std::string v) {
        for (int i=0; i<other->size(); i++) {
            if ((*other)[i] == v)
                return false;
        }
        return true;
    }
};

const std::vector<std::string>& Jukebox::pickNextSongs(int maxSongCount) {
    currentSelection.clear();

    if (currentSelection.size() || true) {
        initSelection(currentSelection, maxSongCount);
    } else {
        std::vector<std::string> newSelection;
        int maxTries = 100;
        do {
            newSelection.clear();
            initSelection(newSelection, maxSongCount);

            int cA = std::count_if(newSelection.begin(), newSelection.end(), IsNotIn(&currentSelection));
            int cB = std::count_if(currentSelection.begin(), currentSelection.end(), IsNotIn(&newSelection));

            if (cA == 1 && cB <= 1)
                break;
            if (cB == 1 && cA <= 1)
                break;
        } while (maxTries --);
        LOGW("Mac tries: %d", maxTries);
        currentSelection = newSelection;
    }
    assert (currentSelection.size() <= maxSongCount);
    return currentSelection;
}
