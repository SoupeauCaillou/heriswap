SUBDIRS = 

bin_PROGRAMS = heriswap

AM_CXXFLAGS = $(GLFW_CFLAGS) \
	$(GLEW_CFLAGS) \
	$(OPENAL_CFLAGS) \
	$(LIBPNG_CFLAGS) \
	$(VORBIS_CFLAGS) \
	$(VORBISFILE_CFLAGS) \
	$(SQLITE_CFLAGS) \
	-I$(top_srcdir)/sac \
	-DDATADIR=\"$(pkgdatadir)/\"

heriswap_SOURCES = tilematch-linux.cpp \
	AnimedEntity.cpp \
	AnimedEntity.h \
	CombinationMark.cpp \
	CombinationMark.h \
	DepthLayer.h \
	Game.cpp \
	Game.h \
	Game_Private.cpp \
	Game_Private.h \
	Game_State.cpp \
	GameState.h \
	GridSystem.cpp \
	GridSystem.h \
	Jukebox.cpp \
	Jukebox.h \
	SuccessManager.cpp \
	SuccessManager.h \
	TwitchSystem.cpp \
	TwitchSystem.h \
	states/AdsStateManager.h \
	states/AdsStateManager.cpp \
	states/BackgroundManager.h \
	states/BackgroundManager.cpp \
	states/DeleteGameStateManager.h \
	states/DeleteGameStateManager.cpp \
	states/FadeStateManager.h \
	states/FadeStateManager.cpp \
	states/FallGameStateManager.h \
	states/FallGameStateManager.cpp \
	states/GameStateManager.h \
	states/HelpStateManager.h \
	states/HelpStateManager.cpp \
	states/LevelStateManager.h \
	states/LevelStateManager.cpp \
	states/LogoStateManager.h \
	states/LogoStateManager.cpp \
	states/MainMenuGameStateManager.h \
	states/MainMenuGameStateManager.cpp \
	states/ModeMenuStateManager.h \
	states/ModeMenuStateManager.cpp \
	states/PauseStateManager.h \
	states/PauseStateManager.cpp \
	states/SpawnGameStateManager.h \
	states/SpawnGameStateManager.cpp \
	states/UserInputGameStateManager.h \
	states/UserInputGameStateManager.cpp \
	modes/GameModeManager.h \
	modes/GameModeManager.cpp \
	modes/InGameUiHelper.h \
	modes/InGameUiHelper.cpp \
	modes/NormalModeManager.h \
	modes/NormalModeManager.cpp \
	modes/TilesAttackModeManager.h \
	modes/TilesAttackModeManager.cpp \
	modes/PositionFeuilles.h \
	api/StorageAPI.h \
	api/linux/Callback.h \
	api/linux/Callback.cpp \
	api/linux/StorageAPILinuxImpl.h \
	api/linux/StorageAPILinuxImpl.cpp

heriswap_LDADD = ../sac/libsac.a \
	$(GLFW_LIBS) \
	$(LIBPNG_LIBS) \
	$(GLEW_LIBS) \
	$(OPENAL_LIBS) \
	$(VORBIS_LIBS) \
	$(VORBISFILE_LIBS) \
	$(SQLITE_LIBS)
