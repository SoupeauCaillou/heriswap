#include <UnitTest++.h>

#include "modes/NormalModeManager.h"
#include <sstream>
#include <iostream>
#include <iomanip>

TEST(SuppressionFeuilleNiveauNormalDifficulty) {
	char cleared[8];
	memset(cleared, 0, sizeof(cleared));

	for (int i=0; i<2500; i++) {
		int initialCount = 48;
		int level = 1 + MathUtil::RandomInt(20);
		std::stringstream log;
		// log.setw(2);
		char countLeft[8];
		memset(countLeft, level + 2, sizeof(countLeft));
		
		log << "Level: " << level << std::endl;
		while (memcmp(countLeft, cleared, sizeof(countLeft)) != 0) {
			int color = MathUtil::RandomInt(8);
			if (countLeft[color] == 0)
				continue;

			for (int k=0; k<8; k++) {
				log << std::setw(2) << (int)countLeft[k] << " ";
			}
			log << " => left: " << initialCount << std::endl;
			
			if (initialCount < 0) {
				CHECK(initialCount > 0);
				std::cerr << log.str() << std::endl;
				break;
			}
			int count = MathUtil::Min(1 + MathUtil::RandomIntInRange(2, 5), (int)countLeft[color]);
			
			int remove = NormalGameModeManager::levelToLeaveToDelete(count, level + 2, level + 2 - countLeft[color]);
			initialCount -= remove;
			
			for (int k=0; k<color; k++)
				log << "   ";
			log << " ^ -" << count << " => -" << remove << std::endl;

			countLeft[color] -= count;
			if (countLeft[color] < 0)
				countLeft[color] = 0;
				
			if (initialCount == 0) {
				CHECK_EQUAL(0, memcmp(countLeft, cleared, sizeof(countLeft)));
			}
		}
		log << "Left at the end : " << initialCount << std::endl;
		if (initialCount > 0) {
			std::cerr << log.str() << std::endl;
		}
		CHECK( initialCount <= 0 );
	}
}
