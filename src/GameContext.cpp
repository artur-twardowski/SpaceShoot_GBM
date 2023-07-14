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

#include "GameContext.h"
#include "Configuration.h"
#include "Utils.h"
#include "Tileset.h"
#include "Gamebuino-Meta-ADTCRV.h"
#include "utility/Misc.h"

using spaceshoot::tileset::AnimationSequence;
using spaceshoot::tileset::animSequences;
using spaceshoot::tileset::ElementID;

namespace spaceshoot { namespace game {

    enum class DrawScene {
        Gameplay, Winning, Losing
    };

    struct DifficultyLevelParams {
        uint32_t maxRunTime;
        uint16_t bombProbability;
        uint16_t bonusProbability;
        uint8_t densityIncreaseFactor;
    };

    const DifficultyLevelParams DIFFICULTIES[] = {
        /* Very easy */          {TARGET_FPS * 50,  65, 37, 5},
        /* Easy */               {TARGET_FPS * 80,  55, 29, 6},
        /* Normal */             {TARGET_FPS * 120, 50, 23, 7},
        /* Hard */               {TARGET_FPS * 150, 40, 19, 7},
        /* Very hard */          {TARGET_FPS * 195, 40, 17, 7},
        /* You shall not pass */ {TARGET_FPS * 300, 20,  7, 7}
    };
        
    const ColorIndex COLOR_BAR_BACKGROUND = (ColorIndex)0;
    const ColorIndex COLOR_SCORE = (ColorIndex)1;
    const ColorIndex COLOR_BOMBS = (ColorIndex)2;
    const ColorIndex COLOR_TIME = (ColorIndex)3;

    const size_t PLAYER_TILE_TAIL = 0;
    const size_t PLAYER_TILE_FRONT = 1;
    const size_t PLAYER_TILE_FRONT_LEFT = 2;
    const size_t PLAYER_TILE_FRONT_RIGHT = 3;

    void restart(GameContext& ctx) {
        memset(reinterpret_cast<void*>(&ctx.playerPosition), 0, sizeof(ctx) - 2);
        ctx.playerPosition = NUM_ROWS / 2;
    }

    bool handleHit(GameContext& ctx, ElementID blockType) {
        switch (blockType) {
        case ElementID::Stone:
        case ElementID::Debris1:
        case ElementID::Debris2:
        case ElementID::Debris3:
        case ElementID::Debris4:
        case ElementID::Debris5:
        case ElementID::Debris6:
        case ElementID::Debris7:
        case ElementID::Debris8:
            ctx.score += 5;
            return true;

        case ElementID::Bomb1:
        case ElementID::Bomb2:
        case ElementID::Bomb3:
        case ElementID::Bomb4:
            ctx.score += 5;
            ctx.bombsCollected++;
            ctx.numBombs++;
           // gb.sound.tone(400, 200);
            return true;

        case ElementID::Bonus1:
        case ElementID::Bonus2:
        case ElementID::Bonus3:
        case ElementID::Bonus4:
            ctx.score += 125;
            ctx.bonusBlocksCollected++;
           // gb.sound.tone(1000, 4000);
            return true;

        default:
            return false;
        }
    }

    bool handleMiss(GameContext& ctx, ElementID blockType, uint8_t row, uint8_t col) {
        switch (blockType) {
        case ElementID::Bomb1:
        case ElementID::Bomb2:
        case ElementID::Bomb3:
        case ElementID::Bomb4:
            game::setBlock(ctx, row, col, ElementID::BombStoned);
            ctx.bombsMissed++;
            return true;

        case ElementID::Bonus1:
        case ElementID::Bonus2:
        case ElementID::Bonus3:
        case ElementID::Bonus4:
            game::setBlock(ctx, row, col, ElementID::BonusStoned);
            ctx.bonusBlocksMissed++;
            return true;

        default:
            return false;
        }
    }

    static bool checkCollisions(GameContext& ctx, uint8_t row) {
        bool hit = false;
        for (size_t col = 0; col < NUM_COLS; col++) {
            bool missile = game::getMissile(ctx, row, col);
            auto blk = game::getBlock(ctx, row, col);

            if (missile && handleHit(ctx, blk)) {
                ctx.hits++;
                ctx.blocksPresent--;
                game::setBlockClearMissile(ctx, row, col, ElementID::Destroyed1);
                hit = true;
            }
        }
        return hit;
    }


    static GameState updateGameField(GameContext& ctx, DrawScene drawScene) {
        const DifficultyLevelParams& params = DIFFICULTIES[ctx.difficultyLevel];

        bool hit = false;
        bool miss = false;

        for (size_t row = 0; row < NUM_ROWS; row++) {

            hit = checkCollisions(ctx, row);

            for (size_t col = NUM_COLS-1; col < NUM_COLS; col--) {
                auto blk = game::getBlock(ctx, row, col);

                /* Move missiles to the left */
                if (col > 0) {
                    bool missile = game::getMissile(ctx, row, col-1);

                    game::setMissile(ctx, row, col, missile);
                }

                /* Handle stoning of function blocks and update of animated tiles */
                const AnimationSequence& animSeq = animSequences[static_cast<size_t>(blk)];
                if (blk != ElementID::None) {
                    /* TODO: magic number */
                    if (game::isFunctionBlock(blk) && col == 19) {
                        miss = miss || handleMiss(ctx, blk, row, col);
                    } else if (animSeq.speed && (gb.frameCount % animSeq.speed == 0)) {
                        game::setBlock(ctx, row, col, animSeq.next);
                    }
                }
            }
            
            hit |= checkCollisions(ctx, row);

            game::setMissile(ctx, row, 0, false);
            /* Move the blocks left */
            if ((ctx.runTime & 0x07) == 0 && drawScene == DrawScene::Gameplay) {
                auto blk = game::getBlock(ctx, row, 0);
                if (blk != ElementID::None && !(blk >= ElementID::Destroyed1 && blk <= ElementID::Destroyed5)) {
                    return GameState::GameOverLost;
                }
                for (size_t col = 0; col < NUM_COLS-1; col++) {
                    game::setBlock(ctx, row, col,
                            game::getBlock(ctx, row, col + 1));
                }
                size_t col = NUM_COLS - 1;
                int randval = rand();

                if (ctx.runTime <= params.maxRunTime - 8 * NUM_COLS) {

                    int density = ctx.runTime >> params.densityIncreaseFactor;
                    if (density >= 18) {
                        density = 18;
                    }

                    game::setBlock(ctx, row, col, ElementID::None);
                    bool blockPlaced = false;
                    if (randval % 24 <= density) {
                        uint8_t b = (uint8_t)ElementID::Debris1 + ((randval + row) & 0x07);
                        game::setBlock(ctx, row, col, static_cast<ElementID>(b));
                        blockPlaced = true;
                    }
                    if ((randval & 0x0FFF) <=params.bombProbability) {
                        game::setBlock(ctx, row, col, ElementID::Bomb1);
                        blockPlaced = true;
                    }
                    if ((randval & 0x0FFF) <= params.bonusProbability) {
                        game::setBlock(ctx, row, col, ElementID::Bonus1);
                        blockPlaced = true;
                    } 

                    if (blockPlaced) ctx.blocksPresent++;
                } else {
                    game::setBlock(ctx, row, col, ElementID::None);
                }
            }


        }

        if (drawScene == DrawScene::Gameplay) {
            ctx.runTime++;
        }

        if (ctx.runTime > params.maxRunTime - 8 * NUM_COLS && ctx.blocksPresent == 0) {
            return GameState::GameOverTimeout;
        } else if (ctx.runTime >= params.maxRunTime) {
            return GameState::GameOverTimeout;
        }
        if (hit) {
            gb.sound.tone(240, 60);
        }
        if (miss) {
            gb.sound.tone(100, 60);
        }

        return GameState::Continue;

    }

    void shoot(GameContext& ctx) {
        //gb.sound.tone(100, 50);
        ctx.shoots++;
        setMissile(ctx, ctx.playerPosition, 0, true);
    }

    void salvo(GameContext& ctx) {
      if (ctx.numBombs > 0) {
        ctx.salvoCounter += 4;
        ctx.numBombs--;
      }
    }

    void continueSalvo(GameContext& ctx) {
        if (ctx.salvoCounter > 0) {
            gb.sound.tone(ctx.salvoCounter * 250, 50);
            for (size_t row = 0; row < NUM_ROWS; row++) {
                setMissile(ctx, row, 0, true);
            }
            ctx.salvoCounter--;
            ctx.shoots += NUM_ROWS;
        }
    }

    static inline void initColorCells(Color barsPalettes[16][8], uint8_t& paletteIndex) {
        /* Draw a blank frame to suppress artifacts */
        gb.display.clear();
        processEvents();

        for (size_t ix = 0; ix < 8; ix++) {
            memcpy(barsPalettes[ix], Gamebuino_Meta::defaultColorPalette, sizeof(barsPalettes[ix]));
            
            barsPalettes[ix][(int)COLOR_BAR_BACKGROUND] = (Color) Gamebuino_Meta::rgb888Torgb565({0, 0, (7 - ix) * 24});
            barsPalettes[ix][(int)COLOR_SCORE] = (Color) Gamebuino_Meta::rgb888Torgb565({(7 - ix) * 32, (7 - ix) * 32, 255});
            barsPalettes[ix][(int)COLOR_BOMBS] = (Color) Gamebuino_Meta::rgb888Torgb565({255, (7 - ix), 0});
            barsPalettes[ix][(int)COLOR_TIME] = (Color) Gamebuino_Meta::rgb888Torgb565({(7 - ix) * 32, (7 - ix/2) * 64, 0});

            gb.tft.colorCells.palettes[paletteIndex] = barsPalettes[ix];
            gb.tft.colorCells.paletteToLine[ix] = paletteIndex;
            gb.tft.colorCells.paletteToLine[SCREEN_HEIGHT - ix - 1] = paletteIndex;
            paletteIndex++;
        }

        tileset::applyPalette(paletteIndex++, 8, SCREEN_HEIGHT - 8);
        gb.tft.colorCells.enabled = true;
    }

    static inline void initPlayerTiles(tileset::ElementID playerTiles[4]) {
        playerTiles[PLAYER_TILE_TAIL] = tileset::ElementID::ShipTail;
        playerTiles[PLAYER_TILE_FRONT] = tileset::ElementID::ShipFrontNormal;
        playerTiles[PLAYER_TILE_FRONT_LEFT] = tileset::ElementID::None;
        playerTiles[PLAYER_TILE_FRONT_RIGHT] = tileset::ElementID::None;
    }

    static inline void drawGameField(GameContext& ctx, Image& tileSet) {
        size_t spriteDx = 0;
        if (ctx.flags & FLAG_SMOOTH_SCROLLING) {
            spriteDx = ((ctx.runTime - 1) >> 1) & 0x03;
        }

        size_t drawY = GAMEBOARD_Y;

        for (size_t y = 0; y < NUM_ROWS; y++) {
            size_t drawX = PLAYER_WIDTH + NUM_COLS * BLOCK_WIDTH;

            for (size_t x = NUM_COLS-1; x < NUM_COLS; x--) {
                auto blk = getBlock(ctx, y, x);

                if (blk != ElementID::None) {
                    tileset::draw(tileSet, drawX - spriteDx, drawY, blk);
                }

                if (getMissile(ctx, y, x)) {
                    gb.display.setColor(12);
                    gb.display.drawFastVLine(drawX + 1, drawY + 1, BLOCK_HEIGHT - 2);
                }

                drawX -= BLOCK_WIDTH;
            }

            drawY += BLOCK_HEIGHT;
        }
    }

    static inline void drawBorders(GameContext& ctx) {
        const DifficultyLevelParams& params = DIFFICULTIES[ctx.difficultyLevel];

        gb.display.setColor(COLOR_BAR_BACKGROUND);
        gb.display.fillRect(0,0,SCREEN_WIDTH, 8);
        gb.display.fillRect(0,SCREEN_HEIGHT-8,SCREEN_WIDTH, 8);

        gb.display.setColor(COLOR_SCORE);
        gb.display.printf(0, 1, "%5d", ctx.score);

        gb.display.setColor(COLOR_BOMBS);
        gb.display.printf(40, 1, "%2d", ctx.numBombs);

        unsigned int remainingTime = (params.maxRunTime - ctx.runTime) / TARGET_FPS;
        gb.display.setColor(COLOR_TIME);
        gb.display.printf(60, 1, "%d:%02d", remainingTime / 60, remainingTime % 60);

        if (ctx.flags & FLAG_SHOW_PROFILING_INFO) {
            /* Profiling information for nerds */
            gb.display.setColor(COLOR_SCORE);
            uint32_t fps = 10000000 / gb.frameDurationMicros;
            //if (fps > TARGET_FPS * 100) {
            //  fps = TARGET_FPS * 100;
            //}
            gb.display.printf(0, SCREEN_HEIGHT-7, "MFPS: %2d.%d, B: %3d, H/S: %4d/%4d",
                    fps / 10, fps % 10, ctx.blocksPresent, ctx.hits, ctx.shoots);
        }
    }

    static inline void drawPlayer(uint8_t playerPositionX, uint8_t playerPositionY, tileset::ElementID* playerTiles, Image& tileSet) {
        size_t drawY = GAMEBOARD_Y;
        for (unsigned int y = 0; y < NUM_ROWS; y++) {
            if (y == playerPositionY - 1) {
                tileset::draw(tileSet, playerPositionX + BLOCK_WIDTH, drawY, playerTiles[PLAYER_TILE_FRONT_LEFT]);
            } else if (y == playerPositionY) {
                tileset::draw(tileSet, playerPositionX, drawY, playerTiles[PLAYER_TILE_TAIL]);
                tileset::draw(tileSet, playerPositionX + BLOCK_WIDTH, drawY, playerTiles[PLAYER_TILE_FRONT]);
            } else if (y == playerPositionY + 1) {
                tileset::draw(tileSet, playerPositionX + BLOCK_WIDTH, drawY, playerTiles[PLAYER_TILE_FRONT_RIGHT]);
            } else {
                tileset::draw(tileSet, playerPositionX, drawY, ElementID::None);
                tileset::draw(tileSet, playerPositionX + BLOCK_WIDTH, drawY, ElementID::None);
            }
            drawY += PLAYER_HEIGHT;
        }
        if (playerPositionX >= BLOCK_WIDTH) {
            tileset::draw(tileSet, playerPositionX - BLOCK_WIDTH, playerPositionY * PLAYER_HEIGHT + GAMEBOARD_Y, ElementID::ShipTailFire);
        }
        /* Update animated tiles */
        updateAnimation(playerTiles, 4);
    }

    static inline void handleButtons(GameContext& ctx, tileset::ElementID* playerTiles, DrawScene& drawScene) {
        if (buttonPressed(BUTTON_UP)) {
            if (ctx.playerPosition > 0)
                ctx.playerPosition--;
        }
        if (buttonPressed(BUTTON_DOWN)) {
            if (ctx.playerPosition < NUM_ROWS - 1)
                ctx.playerPosition++;
        }
        if (buttonPressed(BUTTON_A)) {
            shoot(ctx);
            if (ctx.shoots & 0x01) {
                playerTiles[PLAYER_TILE_FRONT_LEFT] = tileset::ElementID::ShipFiringGlowLeft;
                playerTiles[PLAYER_TILE_FRONT] = tileset::ElementID::ShipFiringLeft;
            } else {
                playerTiles[PLAYER_TILE_FRONT_RIGHT] = tileset::ElementID::ShipFiringGlowRight;
                playerTiles[PLAYER_TILE_FRONT] = tileset::ElementID::ShipFiringRight;
            }
        }
        if (gb.buttons.pressed(BUTTON_B)) {
            salvo(ctx);
            playerTiles[PLAYER_TILE_FRONT_LEFT] = tileset::ElementID::ShipFiringGlowLeft;
            playerTiles[PLAYER_TILE_FRONT_RIGHT] = tileset::ElementID::ShipFiringGlowRight;
            playerTiles[PLAYER_TILE_FRONT] = tileset::ElementID::ShipFiringBoth;
        }
        if (gb.buttons.pressed(BUTTON_MENU)) {
            drawScene = DrawScene::Losing;
        }
    }

    GameState run(GameContext& ctx, Image& tileset) {
        Color barsPalettes[16][8];
        uint8_t paletteIndex = 1;
        tileset::ElementID playerTiles[4];

        initColorCells(barsPalettes, paletteIndex);
        initPlayerTiles(playerTiles);
        
      DrawScene drawScene = DrawScene::Gameplay;

      uint8_t drawSceneCounter = 0;

      while (1) {
        processEvents();

        switch (updateGameField(ctx, drawScene)) {
            case GameState::Continue:
                break;

            case GameState::GameOverTimeout:
                drawScene = DrawScene::Winning;
                break;

            case GameState::GameOverLost:
                drawScene = DrawScene::Losing;
                break;
        }


        size_t drawY = GAMEBOARD_Y;
        const size_t WARNING_X = NUM_COLS / 2 + 5;
        size_t shipX = 0;
        gb.display.setColor(INDEX_BLACK);
        gb.display.clear();

        if (drawScene != DrawScene::Gameplay) {
            drawSceneCounter++;

            if (drawScene == DrawScene::Losing) {
                if (drawSceneCounter == 1) {
                    gb.sound.tone(440, 800);
                    gb.sound.tone(523, 800);
                    gb.sound.tone(622, 800);
                    playerTiles[PLAYER_TILE_TAIL] = tileset::ElementID::ShipTailExploding;
                    playerTiles[PLAYER_TILE_FRONT] = tileset::ElementID::ShipFrontExploding;
                } else if (drawSceneCounter == 16) {
                    return GameState::GameOverLost;
                }
            } else if (drawScene == DrawScene::Winning) {
                if (drawSceneCounter == 1) {
                    gb.sound.tone(440, 800);
                    gb.sound.tone(554, 800);
                    gb.sound.tone(659, 800);
                }
                if (drawSceneCounter < 38) {
                    shipX = drawSceneCounter << 2;
                } else if (drawSceneCounter == 38) {
                    return GameState::GameOverTimeout;
                }
            }
        }

        drawBorders(ctx);
        drawGameField(ctx, tileset);
        drawPlayer(shipX, ctx.playerPosition, playerTiles, tileset);

        if (drawScene == DrawScene::Gameplay) {
            handleButtons(ctx, playerTiles, drawScene);
        }

        continueSalvo(ctx);
      }
    }

}} // namespace spaceshoot::game
