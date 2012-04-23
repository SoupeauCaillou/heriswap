#include "LevelStateManager.h"
#include "systems/ParticuleSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MorphingSystem.h"
#include "TwitchSystem.h"
#include "PlacementHelper.h"
#include "DepthLayer.h"
#include "CombinationMark.h"
#include <sstream>

LevelStateManager::LevelStateManager() {

}

LevelStateManager::~LevelStateManager() {

}

void LevelStateManager::Setup() {
	eGrid = theEntityManager.CreateEntity();
	ADD_COMPONENT(eGrid, ADSR);

	ADSR(eGrid)->idleValue = 0;
	ADSR(eGrid)->attackValue = 1;
	ADSR(eGrid)->attackTiming = 3;
	ADSR(eGrid)->decayTiming = 0.;
	ADSR(eGrid)->sustainValue = 1;
	ADSR(eGrid)->releaseTiming = 0;
	ADSR(eGrid)->active = false;

	eBigLevel = theEntityManager.CreateEntity();
	ADD_COMPONENT(eBigLevel, Transformation);
	TRANSFORM(eBigLevel)->position = Vector2(0, PlacementHelper::GimpYToScreen(846));
	TRANSFORM(eBigLevel)->z = DL_Score;
	ADD_COMPONENT(eBigLevel, TextRendering);
	TEXT_RENDERING(eBigLevel)->color = Color(1, 1, 1);
	TEXT_RENDERING(eBigLevel)->fontName = "typo";
	TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(288);
	TEXT_RENDERING(eBigLevel)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(eBigLevel)->isANumber = true;
	ADD_COMPONENT(eBigLevel, Sound);
	SOUND(eBigLevel)->type = SoundComponent::MUSIC;
	SOUND(eBigLevel)->repeat = false;
	SOUND(eBigLevel)->stop = true;

	eSnowEmitter = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowEmitter, Transformation);
	TRANSFORM(eSnowEmitter)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), 0.5);
	TransformationSystem::setPosition(TRANSFORM(eSnowEmitter), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::S);
	TRANSFORM(eSnowEmitter)->z = DL_Snow;
	ADD_COMPONENT(eSnowEmitter, Particule);
	PARTICULE(eSnowEmitter)->emissionRate = 0;
	PARTICULE(eSnowEmitter)->texture = theRenderingSystem.loadTextureFile("snow_flake0.png");
	PARTICULE(eSnowEmitter)->lifetime = Interval<float>(5.0f, 6.5f);
	PARTICULE(eSnowEmitter)->initialColor = Interval<Color> (Color(1.0, 1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0, 1.0));
	PARTICULE(eSnowEmitter)->finalColor  = Interval<Color> (Color(1.0, 1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0, 1.0));
	PARTICULE(eSnowEmitter)->initialSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(40));
	PARTICULE(eSnowEmitter)->finalSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(40));
	PARTICULE(eSnowEmitter)->forceDirection = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->forceAmplitude  = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->mass = 1;
	
	eSnowBranch = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowBranch, Transformation);
	TRANSFORM(eSnowBranch)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(218));
	TransformationSystem::setPosition(TRANSFORM(eSnowBranch), Vector2(PlacementHelper::GimpXToScreen(0), PlacementHelper::GimpYToScreen(0)), TransformationSystem::NW);
	TRANSFORM(eSnowBranch)->z = DL_SnowBackground;
	ADD_COMPONENT(eSnowBranch, Rendering);
	RENDERING(eSnowBranch)->texture = theRenderingSystem.loadTextureFile("snow_branch.png");

	eSnowGround = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowGround, Transformation);
	TRANSFORM(eSnowGround)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(300));
	TransformationSystem::setPosition(TRANSFORM(eSnowGround), Vector2(PlacementHelper::GimpXToScreen(0), PlacementHelper::GimpYToScreen(1280)), TransformationSystem::SW);
	TRANSFORM(eSnowGround)->z = DL_SnowBackground;
	ADD_COMPONENT(eSnowGround, Rendering);
	RENDERING(eSnowGround)->texture = theRenderingSystem.loadTextureFile("snow_ground.png");
}

void LevelStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	std::stringstream a;
	a << currentLevel;
	TEXT_RENDERING(eBigLevel)->text = a.str();
	TEXT_RENDERING(eBigLevel)->hide = false;
	PARTICULE(eSnowEmitter)->emissionRate = 50;
	RENDERING(eSnowBranch)->hide = false;
	RENDERING(eSnowGround)->hide = false;
	SOUND(eBigLevel)->sound = theSoundSystem.loadSoundFile("audio/level_up.ogg", true);
	SOUND(eBigLevel)->stop = false;

	ADD_COMPONENT(eBigLevel, Morphing);
	MORPHING(eBigLevel)->timing = 1;
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->color.a, 0, 1));
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(smallLevel)->color.a, 1, 0));
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&RENDERING(eSnowGround)->color.a, 0, 1));
	MORPHING(eBigLevel)->active = true;
	
	TRANSFORM(eBigLevel)->position = Vector2(0, PlacementHelper::GimpYToScreen(846));
	TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(288);
	
	duration = 0;
	
	// desaturate everyone except the branch
	TextureRef branchTexture = theRenderingSystem.loadTextureFile("branche.png");
	std::vector<Entity> entities = theRenderingSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<entities.size(); i++) {
		RenderingComponent* rc = RENDERING(entities[i]);
		rc->desaturate = (rc->texture != branchTexture);
	}
	
	entities = theGridSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<entities.size(); i++) {
		CombinationMark::markCellInCombination(entities[i]);
	}
}

GameState LevelStateManager::Update(float dt) {
	duration += dt;

	if (duration > 0.15) {
		ADSR(eGrid)->active = true;
		float alpha = 1 - ADSR(eGrid)->value;
		std::vector<Entity> entities = theGridSystem.RetrieveAllEntityWithComponent();
		for (std::vector<Entity>::iterator it = entities.begin(); it != entities.end(); ++it ) {
			RENDERING(*it)->color.a = alpha;
			TWITCH(*it)->speed = alpha * 9;
		}
	}
	
	if (duration > 6) {
		MorphingComponent* mc = MORPHING(eBigLevel);
		for (int i=0; i<mc->elements.size(); i++) {
			delete mc->elements[i];
		}
		mc->elements.clear();
		// move big score to small score
		mc->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->charHeight, TEXT_RENDERING(eBigLevel)->charHeight, TEXT_RENDERING(smallLevel)->charHeight));
		mc->elements.push_back(new TypedMorphElement<Vector2> (&TRANSFORM(eBigLevel)->position, TRANSFORM(eBigLevel)->position, TRANSFORM(smallLevel)->position));
		mc->active = true;
		mc->activationTime = 0;
		mc->timing = 0.5;
		
		if (duration > 6) {
			PARTICULE(eSnowEmitter)->emissionRate = 0;
		}
	}

	if (SOUND(eBigLevel)->sound == InvalidSoundRef || duration > 8) {
		theGridSystem.DeleteAll();
		return Spawn;
	}

	return LevelChanged;
}

void LevelStateManager::Exit() {
	ADSR(eGrid)->active = false;
	feuilles.clear();
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eBigLevel)->text = "14";
	TEXT_RENDERING(eBigLevel)->hide = true;
	PARTICULE(eSnowEmitter)->emissionRate = 0;
	// RENDERING(eDesaturate)->hide = false;
	RENDERING(eSnowBranch)->hide = true;
	RENDERING(eSnowGround)->hide = true;
	
	MorphingComponent* mc = MORPHING(eBigLevel);
	for (int i=0; i<mc->elements.size(); i++) {
		delete mc->elements[i];
	}
	mc->elements.clear();
	// hide big level
	TEXT_RENDERING(eBigLevel)->hide = true;
	// show small level
	TEXT_RENDERING(smallLevel)->color.a = 1;
	
	std::vector<Entity> ent = theRenderingSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<ent.size(); i++) {
		RENDERING(ent[i])->desaturate = false;
	}
}
