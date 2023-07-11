#ifndef SST_CONFIGURATION_H
#define SST_CONFIGURATION_H

#include "Gamebuino-Meta-ADTCRV.h"

const size_t NUM_ROWS=20;
const size_t NUM_COLS=39;
const unsigned int TARGET_FPS = 21;
const unsigned int GAME_RUN_TIME_LIMIT = 120 * TARGET_FPS;

#define HIGH_RESOLUTION_MODE

#ifdef HIGH_RESOLUTION_MODE
    const size_t SCREEN_WIDTH = 160;
    const size_t SCREEN_HEIGHT = 128;
    const ColorMode SCREEN_MODE = ColorMode::index;

    const size_t BLOCK_WIDTH = 4;
    const size_t BLOCK_HEIGHT = 5;
    const size_t PLAYER_WIDTH = 8;
    const size_t PLAYER_HEIGHT = 5;
    const size_t GAMEBOARD_Y = 16;
#else
    const size_t SCREEN_WIDTH = 80;
    const size_t SCREEN_HEIGHT = 64;
    const ColorMode SCREEN_MODE = ColorMode::rgb565;

    const size_t BLOCK_WIDTH = 2;
    const size_t BLOCK_HEIGHT = 2;
    const size_t PLAYER_WIDTH = 4;
    const size_t PLAYER_HEIGHT = 2;
    const size_t GAMEBOARD_Y = 12;
#endif


#endif // SST_CONFIGURATION_H
