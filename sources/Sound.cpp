#include "base/EntityManager.h"

#include "systems/SoundSystem.h"

#include "Game.h"
#include "Sound.h"

int findCompatible(char c, std::vector<Compatible> &comp) {
	for (int i=0;i<comp.size();i++)
		if (comp[i].c==c) return i;
	return comp.size();
}
std::vector<char> intersec(const std::vector<char>& c1, std::vector<char> c2) {
	std::vector<char> res;
	for (int i=0;i<c1.size();i++) {
		for (int j=0;j<c2.size();j++) {
			if (c1[i]==c2[j]) {
				res.push_back(c1[i]);
				c2.erase(c2.begin()+j);
				j--;
				break;
			}
		}
	}
	return res;
}
bool identic(const std::vector<char>& c1, std::vector<char> c2) {
	for (int i=0;i<c1.size();i++) {
		int j;
		for (j=0;j<c2.size();j++) {
			if (c1[i]==c2[j]) {
				c2.erase(c2.begin()+j);
				j--;
				break;
			}
		}
		if (j==c2.size()) return false;
	}
	return true;
}

static void newMusics(Entity* music, int indiceSwitch) {
	static std::vector<char> ppal;
	if (ppal.size()==0) {
		ppal.push_back('A');
		ppal.push_back('C');
		ppal.push_back('E');
		ppal.push_back('F');
	}
	static std::vector<struct Compatible> compatible;
	if (compatible.size()==0) {
		for (int i=0;i<7;i++) {
			Compatible a;
			a.c = 'A'+i;
			for (int j=0;j<3;j++) {
				if (j!=i) a.fri.push_back('A'+j);
			}
			compatible.push_back(a);
		}
	}

		int count = MathUtil::RandomInt(4) + 1;
	LOGW("starting %d music", count);
	std::vector<char> l; // all songs id
	std::vector<char> canPickIn; // songs which can be picked

	int cr = MathUtil::RandomInt(ppal.size());
	char c = ppal[cr]; // letter from main music
	int indice = findCompatible(c, compatible); // get his id
	std::stringstream s1, s2;
	s1<<"audio/"<<c<<".ogg";
	s2<<"audio/"<<c<<"stress.ogg";
	l.push_back(c);
	SOUND(music[4])->stop = true;
	SOUND(music[0])->stop = false;
	SOUND(music[0])->sound = theSoundSystem.loadSoundFile(s1.str(), true);
	SOUND(music[4])->sound = theSoundSystem.loadSoundFile(s2.str(), true);
	SOUND(music[0])->mainMusic = true;
	SOUND(music[4])->mainMusic = true;

	for (int i=0; i<compatible[indice].fri.size(); i++)
		canPickIn.push_back(compatible[indice].fri[i]); //copying his friends into canPickIn

	for (int i=1; i<count; i++) {
		if (identic(canPickIn,l)) {
			LOGI("no more musics can be found; abort at %d musics inserted", i);
			break;
		}
		SoundComponent* sc = SOUND(music[i]);

		do {
			c = canPickIn[MathUtil::RandomInt(canPickIn.size())];
		} while (std::find(l.begin(), l.end(), c) != l.end());

		l.push_back(c);
		std::stringstream s;
		s << "audio/" << c << ".ogg";
		sc->sound = theSoundSystem.loadSoundFile(s.str(), true);
		sc->stop = false;
		if (std::find(ppal.begin(), ppal.end(), c) != ppal.end()) {
			sc->mainMusic = true;
			std::stringstream s2;
			s2 << "audio/" << c << "stress.ogg";
			SOUND(music[4+i])->stop = true;
			SOUND(music[4+i])->sound = theSoundSystem.loadSoundFile(s2.str(), true);
		} else {
			sc->mainMusic = false;
		}

		indice = findCompatible(c, compatible); // get the song id
		canPickIn = intersec(compatible[indice].fri, canPickIn); //pickIn = friends[him] U friends[old ones]
	}
}

static bool updateMusic(Entity* music, Entity* musicStress1, Entity* musicStress2, float percentLeft, float dt, int indiceMusic) {
/*	bool switchIndice = false;
	//init
	static float time = 0.f, timeEffect1=0.f, timeEffect2=0.f;

	time+=dt;
	//are all started entities done yet ?
	//(and percentLeft > 45%)
	if (time<=17.) {
		time=0;
		newMusics(music, indiceMusic);
		switchIndice = true;
	}
	if (percentLeft>.45) {
		for (int i=0; i<2; i++) {
			SOUND(musicStress1[i])->stop = true;
			SOUND(musicStress2[i])->stop = true;
		}
		timeEffect1=timeEffect2=0.f;
	} else if (percentLeft>.23) {

}




		timeEffect1+=dt;
		if (timeEffect1 >= 17.0) {
			timeEffect1=0;
			indice
		}
		for (int i=0; i<2; i++) {
			SOUND(musicStress1[i])->stop = false;
			SOUND(musicStress2[i])->stop = true;
		}
	} else {
		timeEffect1+=dt;
		timeEffect2+=dt;
		if (timeEffect1 >= 17.0) {

		}
		for (int i=0; i<2; i++) {
			SOUND(musicStress1[i])->stop = false;
			SOUND(musicStress2[i])->stop = false;
		}
	}

	for (int i=0; i<4; i++) {
		if (SOUND(music[i])->mainMusic && SOUND(music[i])->sound != InvalidSoundRef)
			return switchIndice;
	}

	newMusics(music, indiceMusic);
	return switchIndice;
	*/
}
