#pragma once

struct PlacementHelper {
    static float ScreenWidth, ScreenHeight;
    static int WindowWidth, WindowHeight;

    static float GimpWidthToScreen(int width);
    static float GimpHeightToScreen(int height);

    static float GimpYToScreen(int y);
    static float GimpXToScreen(int x);
};
