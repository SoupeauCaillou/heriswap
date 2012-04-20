#include "base/Log.h"
#include <vector>

struct Compatible {
	std::vector<char> fri;
	char c;
	bool operator== (const Compatible & comp) const {
		return (c==comp.c);
	}
	bool operator>= (const Compatible & comp) const {
		return (c >= comp.c);
	}
	bool operator> (const Compatible & comp) const {
		return (c > comp.c);
	}
};

struct Canal {
	Canal() {
		timeLoop = 17.;
		time=0;
	}

	void update(float dt) {
		time-=dt;
		if (time<=0) {
			time=timeLoop;
			indice = (indice+1)%2;
			SOUND(sounds[indice])->sound = theSoundSystem.loadSoundFile(sound, true);
		}
	}

	float timeLoop, time;
	int indice;
	Entity* sounds;
	std::string sound;
};


int findCompatible(char c, std::vector<Compatible> &comp);
std::vector<char> intersec(const std::vector<char>& c1, std::vector<char> c2);
bool identic(const std::vector<char>& c1, std::vector<char> c2);
static void newMusics(Entity* music, int indiceSwitch);
static bool updateMusic(Entity* music, Entity* musicStress1, Entity* musicStress2, float percentLeft, float dt, int indiceMusic);
