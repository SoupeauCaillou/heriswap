#include "PlayerSystem.h"

INSTANCE_IMPL(PlayerSystem);
	
PlayerSystem::PlayerSystem() : ComponentSystemImpl<PlayerComponent>("player_") { 
}

bool PlayerSystem::LeveledUp() {
		std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();

		bool bid = PLAYER(vec[0])->levelUp;
		PLAYER(vec[0])->levelUp = false;
		return bid;
}

int PlayerSystem::GetBonus() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->bonus;
	else return 0;
}	

int PlayerSystem::GetScore() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->score;
	else return 0;
}

float PlayerSystem::GetTime() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->time;
	else return 0;
}

void PlayerSystem::SetTime(float nouv, bool reset) {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) {
		if (reset) PLAYER(vec[0])->time=nouv;
		else PLAYER(vec[0])->time += nouv;
	}
}

int PlayerSystem::GetRemain(int type) {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->remain[type];
	else return 0;	
}

int PlayerSystem::GetObj() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->obj[PLAYER(vec[0])->level-1];
	else return 0;	
}

int PlayerSystem::GetLevel() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1) return PLAYER(vec[0])->level;
	else return 0;	
}	

void PlayerSystem::Reset() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()==1)
	PLAYER(vec[0])->time = 0;
	PLAYER(vec[0])->score = 0;
	
	PLAYER(vec[0])->isReadyToStart = false;
	PLAYER(vec[0])->level = 1;
	PLAYER(vec[0])->bonus = MathUtil::RandomInt(8)+1;

	for (int i=0;i<8;i++) PLAYER(vec[0])->remain[i]=PLAYER(vec[0])->obj[0];

}

void PlayerSystem::ScoreCalc(int nb, int type) {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	
	if (vec.size()==1) {
		if (type == PLAYER(vec[0])->bonus)
			PLAYER(vec[0])->score += 10*PLAYER(vec[0])->level*2*nb*nb*nb/6;
		else
			PLAYER(vec[0])->score += 10*PLAYER(vec[0])->level*nb*nb*nb/6;
	
		PLAYER(vec[0])->remain[type-1] -= nb;
		PLAYER(vec[0])->time -= 2;
		if (PLAYER(vec[0])->time < 0)
			PLAYER(vec[0])->time = 0;
			
		if (PLAYER(vec[0])->remain[type-1]<0)
			PLAYER(vec[0])->remain[type-1]=0;
	} else 	std::cout << "Il y n'y a pas 1 seul personne dans le playersystem\n";
}


void PlayerSystem::DoUpdate(float dt) {
	LevelUp();
}

void PlayerSystem::LevelUp() {
	std::vector<Entity> vec = thePlayerSystem.RetrieveAllActorWithComponent();
	if (vec.size()!=1) std::cout << "Il y n'y a pas 1 seul personne dans le playersystem\n";
	else {
		int match = 1, i=0;
		while (match && i<8) {
			if (PLAYER(vec[0])->remain[i] != 0)
				match=0;
			i++;
		}
		//si on a tous les objectifs
		if (match) {
			PLAYER(vec[0])->level++;
			PLAYER(vec[0])->levelUp = true;
			PLAYER(vec[0])->time -= 20;
			if (PLAYER(vec[0])->time < 0)
				PLAYER(vec[0])->time = 0; 
			std::cout << "Level up to level " << PLAYER(vec[0])->level << std::endl;
			PLAYER(vec[0])->bonus = MathUtil::RandomInt(8)+1;
			for (int i=0;i<8;i++) 
				PLAYER(vec[0])->remain[i] = PLAYER(vec[0])->obj[PLAYER(vec[0])->level-1];
		}
	}	
}
