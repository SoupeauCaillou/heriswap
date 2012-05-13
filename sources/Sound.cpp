#include "base/EntityManager.h"

#include "systems/SoundSystem.h"

#include "Sound.h"
#include <sstream>

std::vector<std::string> newMusics() {
	#define STR(tok) "audio/"#tok".ogg"
	#define L2T(a) STR(a)

	static std::string combis[][4] = {
		{L2T(A)},
		{L2T(A), L2T(C)},
		{L2T(A), L2T(D)},
		{L2T(A), L2T(C), L2T(D)},
		{L2T(B), L2T(C)},
		{L2T(B), L2T(D)},
		{L2T(B), L2T(C), L2T(D)},
		{L2T(A), L2T(B), L2T(C)},
		{L2T(A), L2T(B), L2T(D)},
		{L2T(A), L2T(B), L2T(C), L2T(D)},
	};
	// ! attention : les indices sont décallées de +1
	static int newFromOld[10][6] = {
		{2, 3},
		{1, 3, 4, 5, 8},
		{1, 2, 4, 6, 9},
		{2, 3, 7, 8, 9, 10},
		{2, 6, 7, 8},
		{3, 5, 7, 9},
		{4, 5, 6, 8, 9, 10},
		{2, 4, 5, 7, 9, 10},
		{3, 4, 6, 7, 8, 10},
		{4, 7, 8, 9}
	};

	static int selected = -1; /* permet de savoir quelles etaient les anciennes musiques*/
	std::vector<std::string> result;
	//si c'est la 1ere fois, on en prend une au pif
	if (selected == -1) {
		selected = MathUtil::RandomInt(10);
	//Sinon on en prend une compatible (de la liste de newFromOld)
	} else {
		int r;
		do {
			r = MathUtil::RandomInt(6);
		} while (newFromOld[selected][r] == 0);

		selected = newFromOld[selected][r] - 1; // -1 parce que dans le tableau les indices commencent à 1
	}


	for (int i=0; i<4; i++) {
		const std::string& s = combis[selected][i];
		if (!s.empty()) {
			result.push_back(s);
		}
	}
	return result;
}
