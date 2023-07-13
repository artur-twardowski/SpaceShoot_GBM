#include "SpaceShoot.h"
#include "Gamebuino-Meta-ADTCRV.h"
#include "Tileset.h"
#include "GameContext.h"
#include "GameOver.h"
#include "MainMenu.h"
#include "TitleScreen.h"

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
    }

    void main() {
        spaceshoot::titlescreen::run();

        bool showMenu = true;
        while (1) {
            if (showMenu) {
                spaceshoot::mainmenu::run(ctx);
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
