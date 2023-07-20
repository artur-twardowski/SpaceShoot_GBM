// MIT License
// 
// Copyright (c) 2023 Artur Twardowski
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//     SOFTWARE.

#ifndef SST_CONFIGURATION_H
#define SST_CONFIGURATION_H

#include "Gamebuino-Meta-ADTCRV.h"

const size_t NUM_ROWS=20;
const size_t NUM_COLS=39;
const unsigned int TARGET_FPS = 22;

#define HIGH_RESOLUTION_MODE
#define STORY_IMPLEMENTED
#define HIGHSCORES_IMPLEMENTED

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
