#pragma once

enum GameState {
     Spawn,
     UserInput,
     Delete,
     Fall,
     LevelChanged,
     Pause,
     Unpause,
     Abort,
     MainMenu,
     ModeMenu,
     ScoreBoard,
     EndMenu,
     Background,
     BlackToLogoState,
     LogoToBlackState,
     BlackToMainMenu,
     ModeMenuToBlackState,
     BlackToSpawn,
     Logo
};