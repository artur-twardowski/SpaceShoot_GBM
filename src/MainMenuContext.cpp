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

#include "MainMenuContext.h"
#include "GameContext.h"
#include "TitleScreenContext.h"
#include "Configuration.h"
#include "Utils.h"
#include "utility/Misc.h"
#include "font4x7.c"

namespace spaceshoot { namespace context { namespace mainmenu {

    const char STR_NEW_GAME[] = "New game";
    const char STR_HIGHSCORES[] = "Highscores";
    const char STR_STORY[] = "Story";
    const char STR_INSTRUCTIONS[] = "Instructions";
    const char STR_SETTINGS[] = "Settings";
    const char STR_RETURN_TO_BOOTLOADER[] = "Return to loader";

    const char STR_GAME_DIFFICULTY[] = "Difficulty";
    const char STR_GAME_DIFFICULTY_L1[] = "  Very easy \x10";
    const char STR_GAME_DIFFICULTY_L2[] = "\x11 Easy \x10";
    const char STR_GAME_DIFFICULTY_L3[] = "\x11 Normal \x10";
    const char STR_GAME_DIFFICULTY_L4[] = "\x11 Hard \x10";
    const char STR_GAME_DIFFICULTY_L5[] = "\x11 Very hard \x10";
    const char STR_GAME_DIFFICULTY_L6[] = "\x11 You shall not pass  ";

    const char STR_SMOOTH_SCROLLING[] = "Smooth scrolling";
    const char STR_YES[] = "\x11 Yes  ";
    const char STR_NO[] = "  No \x10";
    
    const char STR_SHOW_PROFILING_INFO[] = "Show profiling statistics";
    const char STR_SHOW_BACKGROUND[] = "Draw background stars";

    const char KEYS_HELP_M1[] = "\x13\x12:navigate menu, \x01\x08:confirm";
    const char KEYS_HELP_O1[] = "\x13\x12:navigate menu, \x02\x09:return";
    const char KEYS_HELP_O2[] = "\x11\x10:adjust selected setting";

    const uint8_t PAL_IDX_MENUPOS = 1;
    const ColorIndex COLOR_MENUPOS_BG_INACTIVE = (ColorIndex)0;
    const ColorIndex COLOR_MENUPOS_BG_ACTIVE = (ColorIndex)1;
    const ColorIndex COLOR_MENUPOS_BG_ACTIVE_PARAM = (ColorIndex)2;
    const ColorIndex COLOR_MENUPOS_FG_INACTIVE = (ColorIndex)3;
    const ColorIndex COLOR_MENUPOS_FG_ACTIVE = (ColorIndex)4;
    const ColorIndex COLOR_MENUPOS_FG_ACTIVE_PARAM = (ColorIndex)5;

    enum struct VisibleScreen {
        Main = 0,
        Settings = 1
    };

    static void drawMenuPositionGeneric(uint8_t x, uint8_t y, const char* str, ColorIndex bgColor, ColorIndex fgColor, bool invertedPalette) {
        for (size_t py = y - 2; py < y + 9; py++) {
            int k = invertedPalette ? (7 - (py - y)) : py - y;
            uint8_t palIndex = k < 0 ? 0 : (k > 7 ? 7 : k);

            gb.tft.colorCells.paletteToLine[py] = PAL_IDX_MENUPOS + palIndex;
        }

        gb.display.setColor(bgColor);
        gb.display.fillRect(0, y-2, SCREEN_WIDTH, 11);
        
        gb.display.setColor(fgColor);
        gb.display.print(x, y, str);
    }

    static void drawMenuPosition(uint8_t x, uint8_t y, const char* str, bool selected) {
        drawMenuPositionGeneric(x, y, str,
                selected ? COLOR_MENUPOS_BG_ACTIVE : COLOR_MENUPOS_BG_INACTIVE,
                selected ? COLOR_MENUPOS_FG_ACTIVE : COLOR_MENUPOS_FG_INACTIVE, false);

    }

    static void drawMenuPositionParam(uint8_t y, const char* str, bool selected) {
        drawMenuPositionGeneric(SCREEN_WIDTH - 5 * strlen(str), y, str,
                selected ? COLOR_MENUPOS_BG_ACTIVE_PARAM : COLOR_MENUPOS_BG_INACTIVE,
                selected ? COLOR_MENUPOS_FG_ACTIVE_PARAM : COLOR_MENUPOS_FG_INACTIVE, false);

    }

    static void drawMenuPositionHCentered(uint8_t y, const char* str, bool selected) {
        drawMenuPosition(SCREEN_WIDTH / 2 - 5 * strlen(str) / 2, y, str, selected);
    }

    static void setUpPalettes(uint16_t* palBg, uint16_t barsPalettes[8][16]) {
        paletteFadeFromBlack(palBg, titlescreen::gameLogoPalette, 7, 10);
        gb.tft.colorCells.enabled = true;
        gb.tft.colorCells.palettes[0] = (Color*)palBg;

        for (size_t ix = 0; ix < 8; ix++) {
            memcpy(barsPalettes[ix], Gamebuino_Meta::defaultColorPalette, sizeof(barsPalettes[ix]));
            
            barsPalettes[ix][(int)COLOR_MENUPOS_BG_INACTIVE] = Gamebuino_Meta::rgb888Torgb565({0, ix * 6, ix * 9});
            barsPalettes[ix][(int)COLOR_MENUPOS_FG_INACTIVE] = Gamebuino_Meta::rgb888Torgb565({ix * 16, ix * 16 + 127, ix * 16 + 127});

            barsPalettes[ix][(int)COLOR_MENUPOS_BG_ACTIVE] = Gamebuino_Meta::rgb888Torgb565({255, ix * 16 + 127, ix * 4});
            barsPalettes[ix][(int)COLOR_MENUPOS_FG_ACTIVE] = Gamebuino_Meta::rgb888Torgb565({ix * 2, ix * 4, 0});

            barsPalettes[ix][(int)COLOR_MENUPOS_BG_ACTIVE_PARAM] = Gamebuino_Meta::rgb888Torgb565({0, ix * 12 + 144, ix * 6 + 16});
            barsPalettes[ix][(int)COLOR_MENUPOS_FG_ACTIVE_PARAM] = Gamebuino_Meta::rgb888Torgb565({ix * 4, ix * 16 , ix * 8});

            gb.tft.colorCells.palettes[PAL_IDX_MENUPOS + ix] = (Color*) barsPalettes[ix];
        }
    }

    MenuPosition run(game::Context& ctx) {
        int position = 0;
        size_t dx;
        uint16_t palBg[16];
        uint16_t barsPalettes[8][16];
        setUpPalettes(palBg, barsPalettes);

        Image backgroundImage(titlescreen::gameLogoData);

        VisibleScreen screen = VisibleScreen::Main;

        bool fullRepaint = true;

        while (1) {
            processEvents();
            
            if (fullRepaint) {
                gb.display.clear();
                gb.display.drawImage(0, 0, backgroundImage);
                memset(gb.tft.colorCells.paletteToLine, 0, SCREEN_HEIGHT);
                gb.display.setFont(font4x7);

                fullRepaint = false;
            }

            if (screen == VisibleScreen::Main) {
                uint8_t y = 20;
                drawMenuPositionHCentered((y+=10), STR_NEW_GAME, static_cast<MenuPosition>(position) == MenuPosition::NewGame);
#ifdef HIGHSCORES_IMPLEMENTED
                drawMenuPositionHCentered((y+=10), STR_HIGHSCORES, static_cast<MenuPosition>(position) == MenuPosition::Highscores);
#endif
#ifdef STORY_IMPLEMENTED
                drawMenuPositionHCentered((y+=10), STR_STORY, static_cast<MenuPosition>(position) == MenuPosition::Story);
#endif
                drawMenuPositionHCentered((y+=10), STR_INSTRUCTIONS, static_cast<MenuPosition>(position) == MenuPosition::Instructions);
                drawMenuPositionHCentered((y+=10), STR_SETTINGS, static_cast<MenuPosition>(position) == MenuPosition::Settings);
                drawMenuPositionHCentered((y+=10), STR_RETURN_TO_BOOTLOADER, static_cast<MenuPosition>(position) == MenuPosition::ReturnToBootloader);

                gb.display.setColor(12);
                gb.display.print(10, 0, KEYS_HELP_M1);
            }

            if (screen == VisibleScreen::Settings) {
                drawMenuPosition(0, 18, STR_GAME_DIFFICULTY, position == 0);
                drawMenuPosition(0, 41, STR_SMOOTH_SCROLLING, position == 1);
                drawMenuPosition(0, 64, STR_SHOW_BACKGROUND, position == 2);
                drawMenuPosition(0, 87, STR_SHOW_PROFILING_INFO, position == 3);

                const char* s;
                switch (ctx.difficultyLevel) {
                    case 0: s = STR_GAME_DIFFICULTY_L1; break;
                    case 1: s = STR_GAME_DIFFICULTY_L2; break;
                    case 2: s = STR_GAME_DIFFICULTY_L3; break;
                    case 3: s = STR_GAME_DIFFICULTY_L4; break;
                    case 4: s = STR_GAME_DIFFICULTY_L5; break;
                    case 5: s = STR_GAME_DIFFICULTY_L6; break;
                }

                // TODO: refactor

                drawMenuPositionParam(29, s, position == 0);

                if (ctx.flags & game::FLAG_SMOOTH_SCROLLING) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                drawMenuPositionParam(52, s, position == 1);

                if (ctx.flags & game::FLAG_SHOW_BACKGROUND) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                drawMenuPositionParam(75, s, position == 2);

                if (ctx.flags & game::FLAG_SHOW_PROFILING_INFO) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                drawMenuPositionParam(98, s, position == 3);

                gb.display.setColor(12);
                gb.display.print(10, 0, KEYS_HELP_O1);
                gb.display.print(10, 8, KEYS_HELP_O2);
            }

            if (buttonPressed(BUTTON_UP)) {
                if (position > 0)
                    position--;
            }
            if (buttonPressed(BUTTON_DOWN)) {
                uint16_t lastPosition;
                if (screen == VisibleScreen::Main) {
                    lastPosition = static_cast<uint16_t>(MenuPosition::Count) - 1;
                } else {
                    lastPosition = 3;
                }
                if (position < lastPosition)
                    position++;
            }
            if (buttonPressed(BUTTON_LEFT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel > 0) ctx.difficultyLevel--; break;
                    case 1: ctx.flags &= ~game::FLAG_SMOOTH_SCROLLING; break;
                    case 2: ctx.flags &= ~game::FLAG_SHOW_BACKGROUND; break;
                    case 3: ctx.flags &= ~game::FLAG_SHOW_PROFILING_INFO; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_RIGHT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel < 5) ctx.difficultyLevel++; break;
                    case 1: ctx.flags |= game::FLAG_SMOOTH_SCROLLING; break;
                    case 2: ctx.flags |= game::FLAG_SHOW_BACKGROUND; break;
                    case 3: ctx.flags |= game::FLAG_SHOW_PROFILING_INFO; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_A)) {
                if (screen == VisibleScreen::Main) {
                    if (static_cast<MenuPosition>(position) == MenuPosition::Settings) {
                        screen = VisibleScreen::Settings;
                        position = 0;
                        fullRepaint = true;
                    } else {
                        paletteSyncFadeToBlack(0, 8, 12);
                        return static_cast<MenuPosition>(position);
                    }
                }
            }
            if (buttonPressed(BUTTON_B)) {
                if (screen == VisibleScreen::Settings) {
                    screen = VisibleScreen::Main;
                    fullRepaint = true;
                }
            }

        }

    }

}}} // namespace spaceshoot::context::mainmenu
