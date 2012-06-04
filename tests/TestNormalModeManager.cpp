#include <UnitTest++.h>

#include "modes/NormalModeManager.h"
#include <sstream>
#include <iostream>
#include <iomanip>

TEST(SuppressionFeuilleNiveauNormalDifficulty) {
	char cleared[8];
	memset(cleared, 0, sizeof(cleared));

	for (int i=0; i<2500; i++) {
		unsigned char initialCountByType[8];
		memset(initialCountByType, 6, 8);
		int level = 1 + MathUtil::RandomInt(20);
		std::stringstream log;
		// log.setw(2);
		char countLeft[8];
		memset(countLeft, level + 2, sizeof(countLeft));
		
		log << "Level: " << level << std::endl;
		
		// PARTIE FINIE ?
		while (memcmp(countLeft, cleared, sizeof(countLeft)) != 0) {
			int color = MathUtil::RandomInt(8);
			if (countLeft[color] == 0)
				continue;

			for (int k=0; k<8; k++) {
				log << std::setw(2) << (int)countLeft[k] << " ";
			}
			log << " => left: ";
			for (int k=0; k<8; k++) {
				log << std::setw(2) << (int)initialCountByType[k] << " ";
			}
			
			if (!memcmp(countLeft, cleared, sizeof(countLeft))) {
				CHECK(!memcmp(countLeft, cleared, sizeof(countLeft)));
				std::cerr << log.str() << std::endl;
				break;
			}
			int count = MathUtil::Min(1 + MathUtil::RandomIntInRange(2, 5), (int)countLeft[color]);
			
			int remove = NormalGameModeManager::levelToLeaveToDelete(count, level + 2, level + 2 - countLeft[color]);
			initialCountByType[color] -= remove;
			
			for (int k=0; k<color; k++)
				log << "   ";
			log << " ^ -" << count << " => -" << remove << std::endl;

			countLeft[color] -= count;
			if (countLeft[color] < 0)
				countLeft[color] = 0;

			for (int i=0; i<8; i++) {
				if (initialCountByType[color] == 0) {
					CHECK_EQUAL(0, countLeft[color]);
				} else if (initialCountByType[color] > 0) {
					CHECK(countLeft[color] > 0);
				} else {
					CHECK(false);
				}
			}
		}
		if (memcmp(initialCountByType, cleared, sizeof(countLeft))) {
			std::cerr << log.str() << std::endl;
		}
		CHECK_EQUAL(0, memcmp(initialCountByType, cleared, sizeof(countLeft)));
	}
}
