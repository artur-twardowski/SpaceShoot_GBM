#include "GameOver.h"
#include "Utils.h"
#include "GameContext.h"

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
      const unsigned int DISPLAY_X = SCREEN_WIDTH - 4 * 5 - 8;
      const unsigned int DISPLAY_X1 = DISPLAY_X - 6 * 5 - 8;

      uint32_t totalScoreY = 72;
        if (showPhase > 64) {
            if (showPhase < 104) {
                totalScoreY -= (showPhase - 64);
            } else {
                totalScoreY -= (104 - 64);
            }
        }
      
      gb.display.setColor(INDEX_LIGHTGREEN);
      if (showPhase >= 0 && showPhase < 80) {
          gb.display.setColor(INDEX_GRAY);
          gb.display.printf(4, 44, "SCORE");
          gb.display.setColor(INDEX_LIGHTGREEN);
          gb.display.printf(DISPLAY_X, 44, "%5d", score);
      }
      if (showPhase >= 16 && showPhase < 80) {
          gb.display.setColor(INDEX_GRAY);
          gb.display.printf(4, 52, "ACCURACY");
          gb.display.setColor(INDEX_RED);
          gb.display.printf(DISPLAY_X1, 52, "%3d.%1d%%", accuracy / 10, accuracy % 10);
          gb.display.setColor(INDEX_LIGHTGREEN);
          gb.display.printf(DISPLAY_X, 52, "%5d", accuracyPoints);
      }
      if (showPhase >= 32 && showPhase < 80) {
          gb.display.setColor(INDEX_GRAY);
          gb.display.printf(4, 60, "BONUS");
          gb.display.setColor(INDEX_LIGHTGREEN);
          gb.display.printf(DISPLAY_X, 60, "%5d", bonus);
      }
      if (showPhase >= 48) {
        gb.display.setColor(INDEX_BEIGE);
        gb.display.printf(4, totalScoreY, "FINAL SCORE");
        gb.display.setColor(INDEX_YELLOW);

        gb.display.printf(DISPLAY_X, totalScoreY, "%5d", totalScore);
      }

      if (showPhase >= 100) {
          gb.display.setColor(INDEX_GREEN);
          gb.display.printf(80, 48, "HIT");
          gb.display.setColor(INDEX_RED);
          gb.display.printf(96, 48, "MISS");

          gb.display.setColor(INDEX_GRAY);
          gb.display.printf(4, 56, "BOMB BLOCKS");
          gb.display.printf(4, 64, "BONUS BLOCKS");

          gb.display.setColor(INDEX_LIGHTGREEN);
          gb.display.printf(76, 56,  "%4d", ctx.bombsCollected);
          gb.display.printf(76, 64, "%4d", ctx.bonusBlocksCollected);
          gb.display.setColor(INDEX_ORANGE);
          gb.display.printf(96, 56,  "%4d", ctx.bombsMissed);
          gb.display.printf(96, 64, "%4d", ctx.bonusBlocksMissed);

          gb.display.setColor(INDEX_WHITE);
          gb.display.print(8, 112, "Press [A] to play again");
          gb.display.print(8, 120, "Press [B] to return to main menu");
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

      if (ctx.shoots != 0) {
        accuracy = ctx.hits * 1000 / ctx.shoots;
        accuracyPoints = accuracy * 2;
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
