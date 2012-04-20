#include "LevelStateManager.h"
#include "systems/ParticuleSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/SoundSystem.h"
#include "PlacementHelper.h"
#include "DepthLayer.h"

LevelStateManager::LevelStateManager() {

}

LevelStateManager::~LevelStateManager() {

}

void LevelStateManager::Setup() {
	eGrid = theEntityManager.CreateEntity();
	ADD_COMPONENT(eGrid, ADSR);

	ADSR(eGrid)->idleValue = 0;
	ADSR(eGrid)->attackValue = Game::GridCoordsToPosition(0,theGridSystem.GridSize).Y+8;
	ADSR(eGrid)->attackTiming = 8;
	ADSR(eGrid)->decayTiming = 0.;
	ADSR(eGrid)->sustainValue = ADSR(eGrid)->attackValue;
	ADSR(eGrid)->releaseTiming = 0;
	ADSR(eGrid)->active = false;

	eBigLevel = theEntityManager.CreateEntity();
	ADD_COMPONENT(eBigLevel, Transformation);
	TRANSFORM(eBigLevel)->position = Vector2(0, PlacementHelper::GimpYToScreen(846));
	TRANSFORM(eBigLevel)->z = DL_Score;
	ADD_COMPONENT(eBigLevel, TextRendering);
	TEXT_RENDERING(eBigLevel)->color = Color(1, 1, 1);
	TEXT_RENDERING(eBigLevel)->fontName = "typo";
	TEXT_RENDERING(eBigLevel)->charSize = Vector2(PlacementHelper::GimpWidthToScreen(200), PlacementHelper::GimpHeightToScreen(288));
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
	PARTICULE(eSnowEmitter)->initialSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(30));
	PARTICULE(eSnowEmitter)->finalSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(30));
	PARTICULE(eSnowEmitter)->forceDirection = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->forceAmplitude  = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->mass = 1;
	
	/*eDesaturate = theEntityManager.CreateEntity();
	ADD_COMPONENT(eDesaturate, Transformation);
	TRANSFORM(eDesaturate)->position = Vector2(0, 0);
	TRANSFORM(eDesaturate)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(1280));
	TRANSFORM(eDesaturate)->z = DL_SnowDesaturate;
	ADD_COMPONENT(eDesaturate, Rendering);
	RENDERING(eDesaturate)->color = Color(178.0/255, 203.0/255, 205.0/255, 1);*/
	
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
	ADSR(eGrid)->active = true;
	std::vector<Entity> ent = theGridSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<ent.size(); i++) {
		FeuilleOrientee truc = {ent[i], Game::GridCoordsToPosition(GRID(ent[i])->i, GRID(ent[i])->j), MathUtil::RandomIntInRange(-1,2)};
		feuilles.push_back(truc);
	}
	
	TEXT_RENDERING(eBigLevel)->text = "14";
	TEXT_RENDERING(eBigLevel)->hide = false;
	PARTICULE(eSnowEmitter)->emissionRate = 10;
	// RENDERING(eDesaturate)->hide = false;
	RENDERING(eSnowBranch)->hide = false;
	RENDERING(eSnowGround)->hide = false;
	SOUND(eBigLevel)->sound = theSoundSystem.loadSoundFile("audio/level_up.ogg", true);
	SOUND(eBigLevel)->stop = false;
}

GameState LevelStateManager::Update(float dt) {
	for ( std::vector<FeuilleOrientee>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
		TRANSFORM(it->e)->rotation = 0.3*it->sens*ADSR(eGrid)->value;
		TRANSFORM(it->e)->position = it->pos - Vector2(0,ADSR(eGrid)->value);
	}

	if (SOUND(eBigLevel)->sound == InvalidSoundRef) {
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
}
