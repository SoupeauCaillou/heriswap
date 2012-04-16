#include "PlacementHelper.h"

int PlacementHelper::ScreenWidth = 0;
int PlacementHelper::ScreenHeight = 0;

int PlacementHelper::WindowWidth = 0;
int PlacementHelper::WindowHeight = 0;

#define GIMP_WIDTH  800.0f
#define GIMP_HEIGHT 1280.0f

#define WIDTH_RATIO_TO_SCREEN_WIDTH(r) ((r) * ScreenHeight * GIMP_WIDTH/GIMP_HEIGHT)


float PlacementHelper::GimpWidthToScreen(int width) {
    return WIDTH_RATIO_TO_SCREEN_WIDTH(width / GIMP_WIDTH);
}

float PlacementHelper::GimpHeightToScreen(int height) {
    return (ScreenHeight * height) / GIMP_HEIGHT;
}

float PlacementHelper::GimpYToScreen(int y) {
    return - ScreenHeight * (y - GIMP_HEIGHT * 0.5) / GIMP_HEIGHT;
}

float PlacementHelper::GimpXToScreen(int x) {
    return WIDTH_RATIO_TO_SCREEN_WIDTH(x - GIMP_WIDTH * 0.5) / GIMP_WIDTH);
}
