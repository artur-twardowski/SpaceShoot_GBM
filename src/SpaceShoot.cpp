#include "SpaceShoot.h"
#include "Gamebuino-Meta-ADTCRV.h"
#include "Tileset.h"
#include "GameContext.h"
#include "GameOver.h"
#include "MainMenu.h"
#include "TitleScreen.h"
#include "InstructionsContext.h"

const char VERSION[] = "v0.1-R714";

namespace spaceshoot {

    Image tileSet;
    GameContext ctx;

    uint8_t paletteToCell[20 * 16];

    Color* palettes[32] = {
        const_cast<Color*>(Gamebuino_Meta::defaultColorPalette)
    };

    void init() {
        memset(paletteToCell, 0, sizeof(paletteToCell));
        gb.tft.colorCells.enabled = false;
        gb.tft.colorCells.paletteToLine = paletteToCell;
        gb.tft.colorCells.palettes = palettes;

        spaceshoot::tileset::load(tileSet);
        ctx.difficultyLevel = 2;
        ctx.flags = FLAG_SMOOTH_SCROLLING;
    }

    void main() {
        spaceshoot::titlescreen::run();

        bool showMenu = true;
        while (1) {
            mainmenu::MenuPosition menuPosition;
            if (showMenu) {
                menuPosition = spaceshoot::mainmenu::run(ctx);

                switch (menuPosition) {
                    case mainmenu::MenuPosition::Instructions:
                        instructions::run(tileSet);
                        continue;

                    case mainmenu::MenuPosition::NewGame:
                        break;

                    case mainmenu::MenuPosition::ReturnToBootloader:
                        gb.bootloader.loader();

                    default: continue;
                }
            }

            spaceshoot::game::restart(ctx);
            GameState state = spaceshoot::game::run(ctx, tileSet);

            if (state == GameState::GameOverTimeout) {
                showMenu = spaceshoot::gameover::run(ctx, true);
            } else if (state == GameState::GameOverLost) {
                showMenu = spaceshoot::gameover::run(ctx, false);
            }
        }
    }
}
