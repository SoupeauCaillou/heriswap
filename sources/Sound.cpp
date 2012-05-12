#include "base/EntityManager.h"

#include "systems/SoundSystem.h"

#include "Sound.h"
#include <sstream>

std::vector<std::string> newMusics(std::vector<std::string> &olds) {
	static std::vector<char> ppal;
	if (ppal.size()==0) {
		ppal.push_back('A');
		ppal.push_back('B');
	}
	std::vector<std::string> res;
	
	int a = olds.size();
	int count = MathUtil::RandomInt(4);
	if (a) 
		count = MathUtil::RandomIntInRange(MathUtil::Max(1,a-1), a+1);

	if (count == 1) {
		res.push_back("audio/A.ogg");
		return res;
	}
	
	char c;
	std::vector<char> l; // all songs id
	for (int i=0; i<olds.size(); i++) {
		l.push_back(olds[i].size()-4);
		LOGI("%c", olds[i].size()-4);
	}
	//on copie l'ancien dans le nouveau et on va modifier seulement un element
	res = olds;
	
	//si on a gagné une piste(ou plusieurs si c'est la 1ere fois), on rajoute une lettre(s)
	if (count > olds.size()) {
		for (int i=0;i<count;i++) {
			do {
				c = MathUtil::RandomInt(4)+'A';
			} while (std::find(l.begin(), l.end(), c) != l.end());
			std::stringstream s;
			s << "audio/" << c << ".ogg";
			res.push_back(s.str());
		}
	//sinon si on en a perdu une, on supprime une qui n'est pas principale
	} else if (count < olds.size()) {
		int main = 0;
		if (std::find(l.begin(), l.end(), 'A') != l.end())
			main++;
		if (std::find(l.begin(), l.end(), 'B') != l.end())
			main++;
		
		int i = -1;
		//si on a 2 pistes ppales, on se permet d'en supprimer une des deux
		if (main==2) {
			i =	MathUtil::RandomInt(count);
		//sinon on tire une piste aléatoire secondaire
		} else {
			do {
				i = MathUtil::RandomInt(count);
			} while (res[i]=="audio/A.ogg"|| res[i]=="audio/B.ogg");
		}
		//puis on la supprime du resultat
		res.erase(res.begin()+count);
	//si on en a le même nombre, il faut en remplacer une
	} else {
		int main = 0;
		if (std::find(l.begin(), l.end(), 'A') != l.end())
			main++;
		if (std::find(l.begin(), l.end(), 'B') != l.end())
			main++;
		
		int i = MathUtil::RandomInt(res.size());		
		//si on a 1 ppale et qu'on tire une ppale, faut faire ppal->ppal
		if (main==1) {
			res[i] = (res[i]=="audio/A.ogg")? "audio/B.ogg" : "audio/A.ogg";
		//sinon on peut faire ppal->secondaire
		} else {
			do {
				c = MathUtil::RandomInt(4)+'A';
			} while (std::find(l.begin(), l.end(), c) != l.end());
			std::stringstream s;
			s << "audio/" << c << ".ogg";
			res.push_back(s.str());
		}

	}
	std::cout <<"starting " << count <<" musics : ";
	for (int i=0; i<res.size()-1; i++) std::cout << res[i] <<", ";
	std::cout << res[res.size()-1] << std::endl;
	return res;
}

bool updateMusic(Canal* canal, Canal* canalStress1, Canal* canalStress2, float percentDone, float dt) {
	//conserve olds musics
	static std::vector<std::string> current;
	bool end, endv[4];
	for (int i=0; i<4; i++)
		endv[i] = canal[i].update(dt);
	end=endv[0] && endv[1] && endv[2] && endv[3];
	//if all musics ended, generate new ones
	if (end) {
		std::vector<std::string> nouv = newMusics(current);
		current = nouv;
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
