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
#include "AnimedEntity.h"

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
		struct Render {
			Vector2 v;
			float rot;
		};

		void SetupCore(int bonus);
		void ResetCore(int bonus);
		void UpdateCore(float dt, float obj, float herissonSpeed = 0);
		void HideUICore(bool ToHide);
		float position(float t);
		void generateLeaves(int nb);

		virtual ~GameModeManager() {}

		/* Do some initial setup if needed */
		virtual void Setup() = 0;
		/* Update gamemode, and returns true if end of the mode */
		virtual float Update(float dt) = 0;

		virtual void UpdateUI(float dt) = 0;
		virtual void HideUI(bool toHide) = 0;

        virtual void WillScore(int nb, int type, std::vector<Entity>& out) {}
		virtual void ScoreCalc(int nb, int type) = 0;

		virtual GameMode GetMode() = 0;

		virtual void LevelUp() = 0;
		virtual bool LeveledUp() = 0;

		virtual void Reset() = 0;

		float time, limit;
		int points, bonus;
		//feuilles de l'arbre
		std::vector<BranchLeaf> branchLeaves;
		//hérisson
		Entity herisson;
		AnimatedActor* c;
		std::vector<Vector2> pts;
		InGameUiHelper uiHelper;

		Entity sky, branch;
		Entity  decor1er, decor2nd;
	private :
		std::vector<Render> posBranch;
		void fillVec();
		float distance;
};
