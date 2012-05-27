#include <UnitTest++.h>

#include "modes/TilesAttackModeManager.h"
#include <sstream>
#include <iostream>
#include <iomanip>

TEST(TilesSuppressionFeuilleNiveauNormalDifficulty) {
	char cleared[8];
	memset(cleared, 0, sizeof(cleared));

	// 50 tests
	for (int i=0; i<250; i++) {
		int initialCount = 48;
		int limit = MathUtil::RandomInt(2) ? 30 : 100;
		int left = limit;
		
		std::stringstream log;
		
		while (left > 0) {
			log << left << std::endl;			
			if (initialCount < 0) {
				CHECK(initialCount > 0);
				std::cerr << log.str() << std::endl;
				break;
			}
			int count = 1 + MathUtil::RandomIntInRange(2, 6);
			
			
			int remove = TilesAttackGameModeManager::levelToLeaveToDelete(48, limit, count, limit - left);
			left -= count;
			initialCount -= remove;
			
			log << "remove: " << count << " => " << remove << std::endl;
		}
		log << "Left at the end : " << initialCount << std::endl;
		if (initialCount > 0) {
			std::cerr << log.str() << std::endl;
		}
		CHECK( initialCount <= 0 );
	}
}
