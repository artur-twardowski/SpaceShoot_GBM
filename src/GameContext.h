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

#ifndef SST_GAMECONTEXT_H
#define SST_GAMECONTEXT_H

#include "Configuration.h"
#include <stdint.h>
#include "Tileset.h"

struct GameContext {
    uint8_t difficultyLevel;
    uint8_t flags;

    uint8_t playerPosition;
    uint32_t score;
    uint8_t numBombs;
    uint16_t bonusBlocksCollected;
    uint16_t bonusBlocksMissed;
    uint16_t bombsCollected;
    uint16_t bombsMissed;
    uint16_t runTime;
    uint16_t shoots;
    uint16_t hits;
    uint8_t salvoCounter;
    uint16_t blocksPresent;
    uint8_t gameField[NUM_ROWS][NUM_COLS];

    GameContext() = default;
    GameContext(const GameContext&) = delete;
    GameContext& operator=(const GameContext&) = delete;
};

enum struct GameState {
  Continue, GameOverTimeout, GameOverLost
};

const uint8_t BLOCK_MASK = 0x3F;
const uint8_t BLOCK_MISSILE = 0x40;
const uint8_t FLAG_SMOOTH_SCROLLING = 0x01;
const uint8_t FLAG_SHOW_PROFILING_INFO = 0x02;

namespace spaceshoot { namespace game {
    void restart(GameContext& ctx);
    GameState run(GameContext& ctx, Image& tileset);

    static inline tileset::ElementID getBlock(GameContext& ctx, uint8_t row, uint8_t col) {
        return static_cast<tileset::ElementID>(ctx.gameField[row][col] & BLOCK_MASK);
    }

    static inline bool isFunctionBlock(tileset::ElementID elementID) {
        if (elementID >= tileset::ElementID::Bomb1 && elementID <= tileset::ElementID::Bomb4) {
            return true;
        }
        if (elementID >= tileset::ElementID::Bonus1 && elementID <= tileset::ElementID::Bonus4) {
            return true;
        }
        return false;
    }

    static inline void setBlock(GameContext& ctx, uint8_t row, uint8_t col, tileset::ElementID elementID) {
        ctx.gameField[row][col] &= ~BLOCK_MASK;
        ctx.gameField[row][col] |= static_cast<uint8_t>(elementID);
    }

    static inline void setBlockClearMissile(GameContext& ctx, uint8_t row, uint8_t col, tileset::ElementID elementID) {
        ctx.gameField[row][col] = static_cast<uint8_t>(elementID);
    }

    static inline bool getMissile(GameContext& ctx, uint8_t row, uint8_t col) {
        return ctx.gameField[row][col] & BLOCK_MISSILE;
    }

    static inline void setMissile(GameContext& ctx, uint8_t row, uint8_t col, bool present) {
        if (present) {
            ctx.gameField[row][col] |= BLOCK_MISSILE;
        } else {
            ctx.gameField[row][col] &= ~BLOCK_MISSILE;
        }
    }

}}

#endif // SST_GAMECONTEXT_H
