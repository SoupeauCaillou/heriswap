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

bool updateMusic(Canal* canal, Canal* canalStress1, Canal* canalStress2, float percentDone, float dt) {
	bool end, endv[4];
	for (int i=0; i<4; i++) endv[i] = canal[i].update(dt);
	end=endv[0] && endv[1] && endv[2] && endv[3];
	if (end) {
		std::vector<std::string> nouv = newMusics();
		for (int i=0; i<4; i++) {
			if (!nouv.empty()) {
				canal[i].name=nouv[0];
				nouv.erase(nouv.begin());
			} else {
				canal[i].name="";
			}
		}
	}
	float pos = 0;
	int count = 0;
	for (int i=0; i<4; i++) {
		if (SOUND(canal[i].sounds[canal[i].indice])->sound != InvalidSoundRef) {
			pos += SOUND(canal[i].sounds[canal[i].indice])->position;
			count++;
		}
	}
	if (percentDone > 30./45 && count) {
		pos /= count;
		if (SOUND(canalStress1->sounds[0])->sound==InvalidSoundRef && SOUND(canalStress1->sounds[1])->sound==InvalidSoundRef) {
			LOGI("starting clochettes n° one!");
			std::cout << pos << " != " << SOUND(canal[0].sounds[canal[0].indice])->position << std::endl;
			SOUND(canalStress1->sounds[canalStress1->indice])->position= pos + 2*dt / 18.;
			SOUND(canalStress1->sounds[canalStress1->indice])->masterTrack = SOUND(canal[0].sounds[canal[0].indice]);
			SOUND(canalStress1->sounds[canalStress1->indice])->masterTrackOffsetMs = 0;
		}
		canalStress1->update(dt);
	}
	if (percentDone > 40./45 && count) {
		if (SOUND(canalStress2->sounds[0])->sound==InvalidSoundRef && SOUND(canalStress2->sounds[1])->sound==InvalidSoundRef) {
			LOGI("starting clochettes n° two!");
			std::cout << pos << " != " << SOUND(canal[0].sounds[canal[0].indice])->position << std::endl;
			SOUND(canalStress2->sounds[canalStress2->indice])->position=pos + 2*dt / 18.;
			SOUND(canalStress2->sounds[canalStress2->indice])->masterTrack = SOUND(canal[0].sounds[canal[0].indice]);
			SOUND(canalStress2->sounds[canalStress2->indice])->masterTrackOffsetMs = 0;
		}
		canalStress2->update(dt);
	}
}
