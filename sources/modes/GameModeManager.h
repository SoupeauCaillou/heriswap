#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

#include "DepthLayer.h"
#include "Game.h"
#include "InGameUiHelper.h"
#include "PlacementHelper.h"

//FCRR : FPS Calculation Refresh Rate
#define FCRR 1.


enum GameMode {
	Normal = 1,
	ScoreAttack,
	StaticTime
};


class GameModeManager {
	public:

		struct BranchLeaf {
			Entity e;
			int type;
		};
		struct Actor;
		struct AnimatedActor;
		void SetupCore();
		void UpdateCore(float dt);
		void HideUICore(bool ToHide);
		float position(float t, std::vector<Vector2> pts);
		Vector2 placeOnBranch();
		void generateLeaves(int nb);

		virtual ~GameModeManager() {}

		/* Do some initial setup if needed */
		virtual void Setup() = 0;
		/* Update gamemode, and returns true if end of the mode */
		virtual bool Update(float dt) = 0;

		virtual void UpdateUI(float dt) = 0;
		virtual void HideUI(bool toHide) = 0;

		virtual void ScoreCalc(int nb, int type) = 0;

		virtual GameMode GetMode() = 0;

		virtual void LevelUp() = 0;
		//permet de savoir si on a change de niveau
		virtual bool LeveledUp() = 0;

		virtual void Reset() = 0;

		float time, limit;
		int points;
		//feuilles de l'arbre
		std::vector<BranchLeaf> branchLeaves;
		//hérisson
		Entity herisson;
		AnimatedActor* c;
		std::vector<Vector2> pts;

		InGameUiHelper uiHelper;
};
