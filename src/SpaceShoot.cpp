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

#include "SpaceShoot.h"
#include "Gamebuino-Meta-ADTCRV.h"
#include "Tileset.h"
#include "GameContext.h"
#include "GameOverContext.h"
#include "MainMenuContext.h"
#include "TitleScreenContext.h"
#include "InstructionsContext.h"

namespace spaceshoot {

using MenuPosition = context::mainmenu::MenuPosition;
using GameState = context::game::GameState;

    const char VERSION[] = "v0.2-R721";

    Image tileSet;
    context::game::Context ctx;

    uint8_t paletteToCell[SCREEN_HEIGHT];

    Color* palettes[32] = {
        const_cast<Color*>(Gamebuino_Meta::defaultColorPalette)
    };

    void init() {
        memset(paletteToCell, 0, sizeof(paletteToCell));
        gb.tft.colorCells.enabled = false;
        gb.tft.colorCells.paletteToLine = paletteToCell;
        gb.tft.colorCells.palettes = palettes;

        tileset::load(tileSet);
        ctx.difficultyLevel = 2;
        ctx.flags = context::game::FLAG_SMOOTH_SCROLLING | context::game::FLAG_SHOW_BACKGROUND;
    }

    void main() {
        context::titlescreen::run();

        bool showMenu = true;
        while (1) {
            MenuPosition menuPosition;
            if (showMenu) {
                menuPosition = context::mainmenu::run(ctx);

                switch (menuPosition) {
                    case MenuPosition::Instructions:
                        context::instructions::run(tileSet);
                        continue;

                    case MenuPosition::NewGame:
                        break;

                    case MenuPosition::ReturnToBootloader:
                        gb.bootloader.loader();

                    default: continue;
                }
            }

            context::game::restart(ctx);
            GameState state = context::game::run(ctx, tileSet);

            if (state == GameState::GameOverTimeout) {
                showMenu = context::gameover::run(ctx, true);
            } else if (state == GameState::GameOverLost) {
                showMenu = context::gameover::run(ctx, false);
            }
        }
    }
}
