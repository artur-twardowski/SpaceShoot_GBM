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

#include "GameOver.h"
#include "Utils.h"
#include "GameContext.h"
#include "utility/Graphics/font3x5.c"
#include "font4x7.c"

namespace spaceshoot { namespace gameover {

    static bool decreaseByInterval(uint32_t& measure, uint32_t interval, uint32_t& score) {
        if (measure >= interval) {
            measure -= interval;
            score += interval;
            return true;
        }
        return false;
    }

    static void drawStats(uint32_t score, uint16_t accuracy, uint32_t accuracyPoints, uint32_t bonus, uint32_t totalScore, uint16_t showPhase, GameContext& ctx) {
      const unsigned int DISPLAY_X = SCREEN_WIDTH - 2 * 4 * 5 - 8;
      const unsigned int DISPLAY_X1 = DISPLAY_X - 6 * 5 - 8;

      uint32_t totalScoreY = 100;
        if (showPhase > 64) {
            if (showPhase < 104) {
                totalScoreY -= (showPhase - 64) * 2;
            } else {
                totalScoreY -= (104 - 64) * 2;
            }
        }
      
      gb.display.setColor(INDEX_LIGHTGREEN);
      if (showPhase >= 0 && showPhase < 80) {
          setTextFormat(INDEX_GRAY, 1, 1, font4x7);
          gb.display.printf(4, 44, "SCORE");

          setTextFormat(INDEX_LIGHTGREEN, 2, 2, font3x5);
          gb.display.printf(DISPLAY_X, 44, "%5d", score);
      }
      if (showPhase >= 16 && showPhase < 80) {
          setTextFormat(INDEX_GRAY, 1, 1, font4x7);
          gb.display.printf(4, 56, "ACCURACY");

          setTextFormat(INDEX_RED, 1, 2, font3x5);
          gb.display.printf(DISPLAY_X1, 56, "%3d.%1d%%", accuracy / 10, accuracy % 10);

          setTextFormat(INDEX_LIGHTGREEN, 2, 2, font3x5);
          gb.display.printf(DISPLAY_X, 56, "%5d", accuracyPoints);
      }
      if (showPhase >= 32 && showPhase < 80) {
          setTextFormat(INDEX_GRAY, 1, 1, font4x7);
          gb.display.printf(4, 68, "BONUS");

          setTextFormat(INDEX_LIGHTGREEN, 2, 2, font3x5);
          gb.display.printf(DISPLAY_X, 68, "%5d", bonus);
      }
      if (showPhase >= 48) {
        setTextFormat(INDEX_BEIGE, 1, 1, font4x7);
        gb.display.printf(4, totalScoreY, "FINAL SCORE");

        setTextFormat(INDEX_YELLOW, 2, 2, font3x5);
        gb.display.printf(DISPLAY_X, totalScoreY, "%5d", totalScore);
      }

      gb.display.setFontSize(1, 1);
      if (showPhase >= 100) {
          gb.display.setColor(INDEX_GREEN);
          gb.display.printf(80, 48, "HIT");
          gb.display.setColor(INDEX_RED);
          gb.display.printf(116, 48, "MISS");

          gb.display.setColor(INDEX_GRAY);
          gb.display.printf(4, 56, "BOMB BLOCKS");
          gb.display.printf(4, 64, "BONUS BLOCKS");

          gb.display.setColor(INDEX_LIGHTGREEN);
          gb.display.setFontSize(2, 1);
          gb.display.printf(76, 56,  "%4d", ctx.bombsCollected);
          gb.display.printf(76, 64, "%4d", ctx.bonusBlocksCollected);
          gb.display.setColor(INDEX_ORANGE);
          gb.display.setFontSize(2, 1);
          gb.display.printf(116, 56,  "%4d", ctx.bombsMissed);
          gb.display.printf(116, 64, "%4d", ctx.bonusBlocksMissed);

          gb.display.setColor(INDEX_WHITE);
          gb.display.setFontSize(1, 1);
          gb.display.setFont(font4x7);
          gb.display.print(8, 112, "\x15\x16 to play again");
          gb.display.print(8, 120, "\x0F\x17 to return to main menu");
      }
    }

    bool run(GameContext& ctx, bool timedOut) {
        uint32_t accuracy;
        uint32_t accuracyPoints;
        uint32_t bonus;
        uint32_t gameScore = ctx.score;
        uint32_t showPhaseCounter = 0;
        uint32_t showPhase = 0;

        gb.tft.colorCells.enabled = false;
        gb.tft.setPalette(Gamebuino_Meta::defaultColorPalette);

        if (ctx.shoots > 0) {
            accuracy = ctx.hits * 1000 / ctx.shoots;
            accuracyPoints = accuracy * 2;
        } else {
            accuracy = 0;
            accuracyPoints = 0;
        }

        bonus = ctx.numBombs * 100;
        if (timedOut) {
            bonus += 500;
        }

        ctx.score = 0;

        while (1) {
            gb.display.clear();

            if (showPhaseCounter == 64) {
                bool updated = decreaseByInterval(gameScore, 1000, ctx.score);
                if (!updated) updated = decreaseByInterval(gameScore, 100, ctx.score);
                if (!updated) updated = decreaseByInterval(gameScore, 10, ctx.score);
                if (!updated) updated = decreaseByInterval(gameScore, 1, ctx.score);
                if (!updated) updated = decreaseByInterval(accuracyPoints, 100, ctx.score);
                if (!updated) updated = decreaseByInterval(accuracyPoints, 10, ctx.score);
                if (!updated) updated = decreaseByInterval(accuracyPoints, 1, ctx.score);
                if (!updated) updated = decreaseByInterval(bonus, 1000, ctx.score);
                if (!updated) updated = decreaseByInterval(bonus, 100, ctx.score);
                if (!updated) updated = decreaseByInterval(bonus, 10, ctx.score);
                if (!updated) updated = decreaseByInterval(bonus, 1, ctx.score);

                if (!updated) showPhaseCounter++;

                if (updated) {
                    gb.sound.tone(250, 40);
                }      
            } else {
                showPhaseCounter++;
            }

            drawStats(gameScore, accuracy, accuracyPoints, bonus, ctx.score, showPhaseCounter, ctx);

            processEvents();

            if (showPhaseCounter >= 100) {
                if (gb.buttons.pressed(BUTTON_A)) {
                    return false; /* Do not invoke the menu, jump right away to the game */
                }
                if (gb.buttons.pressed(BUTTON_B)) {
                    return true; /* Go back to menu */
                }
            }
        }

    }
}}
