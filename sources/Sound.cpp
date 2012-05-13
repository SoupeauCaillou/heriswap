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

	int selected = MathUtil::RandomInt(10);
	std::vector<std::string> result;
	for (int i=0; i<4; i++) {
		const std::string& s = combis[selected][i];
		if (!s.empty()) {
			result.push_back(s);
		}
	}
	return result;
}
