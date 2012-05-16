#include "Game.h"

class SuccessManager {
	public :
	SuccessManager(SuccessAPI* sAPI) : successAPI(sAPI) {};
	~SuccessManager() {};
	
	void test();
	
	SuccessAPI* successAPI;
};
