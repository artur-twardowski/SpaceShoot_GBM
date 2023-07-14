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

#include "MainMenu.h"
#include "GameContext.h"
#include "Utils.h"

namespace spaceshoot { namespace mainmenu {

    const char STR_NEW_GAME[] = "NEW GAME";
    const char STR_STORY[] = "STORY";
    const char STR_INSTRUCTIONS[] = "INSTRUCTIONS";
    const char STR_SETTINGS[] = "SETTINGS";
    const char STR_RETURN_TO_BOOTLOADER[] = "RETURN TO LOADER";

    const char STR_GAME_DIFFICULTY[] = "DIFFICULTY";
    const char STR_GAME_DIFFICULTY_L1[] = "  VERY EASY >";
    const char STR_GAME_DIFFICULTY_L2[] = "< EASY >";
    const char STR_GAME_DIFFICULTY_L3[] = "< NORMAL >";
    const char STR_GAME_DIFFICULTY_L4[] = "< HARD >";
    const char STR_GAME_DIFFICULTY_L5[] = "< VERY HARD >";
    const char STR_GAME_DIFFICULTY_L6[] = "< YOU SHALL NOT PASS  ";

    const char STR_SMOOTH_SCROLLING[] = "SMOOTH SCROLLING";
    const char STR_YES[] = "< YES  ";
    const char STR_NO[] = "  NO >";
    
    const char STR_SHOW_PROFILING_INFO[] = "SHOW PROFILING STATS";

    enum struct VisibleScreen {
        Main = 0,
        Settings = 1
    };

    static void drawMenuPosition(uint8_t x, uint8_t y, const char* str, bool selected) {
        gb.display.setColor(selected ? INDEX_WHITE : INDEX_ORANGE);

        gb.display.print(x, y, str);
    }

    static void drawMenuPositionHCentered(uint8_t y, const char* str, bool selected) {
        drawMenuPosition(SCREEN_WIDTH / 2 - 4 / 2 * strlen(str), y, str, selected);
    }

    static void drawMenuPositionParam(uint8_t x, uint8_t y, const char* str, bool selected) {
        if (selected) {
            gb.display.setColor(INDEX_LIGHTGREEN);
        } else {
            gb.display.setColor(INDEX_GREEN);
        }

        gb.display.print(x, y, str);
    }

    MenuPosition run(GameContext& ctx) {
        int position = 0;
        size_t dx;
        VisibleScreen screen = VisibleScreen::Main;
        gb.tft.colorCells.enabled = false;
        gb.tft.setPalette(Gamebuino_Meta::defaultColorPalette);

        while (1) {
            processEvents();
            
            gb.display.clear();


            if (buttonPressed(BUTTON_UP)) {
                if (position > 0)
                    position--;
            }
            if (buttonPressed(BUTTON_DOWN)) {
                uint16_t lastPosition;
                if (screen == VisibleScreen::Main) {
                    lastPosition = static_cast<uint16_t>(MenuPosition::Count) - 1;
                } else {
                    lastPosition = 2;
                }
                if (position < lastPosition)
                    position++;
            }
            if (buttonPressed(BUTTON_LEFT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel > 0) ctx.difficultyLevel--; break;
                    case 1: ctx.flags &= ~FLAG_SMOOTH_SCROLLING; break;
                    case 2: ctx.flags &= ~FLAG_SHOW_PROFILING_INFO; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_RIGHT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel < 5) ctx.difficultyLevel++; break;
                    case 1: ctx.flags |= FLAG_SMOOTH_SCROLLING; break;
                    case 2: ctx.flags |= FLAG_SHOW_PROFILING_INFO; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_A)) {
                if (screen == VisibleScreen::Main) {
                    if (static_cast<MenuPosition>(position) == MenuPosition::Settings) {
                        screen = VisibleScreen::Settings;
                        position = 0;
                    } else {
                        return static_cast<MenuPosition>(position);
                    }
                }
            }
            if (buttonPressed(BUTTON_B)) {
                if (screen == VisibleScreen::Settings) {
                    screen = VisibleScreen::Main;
                }
            }

            if (screen == VisibleScreen::Main) {
                uint8_t y = 0;
                drawMenuPositionHCentered((y+=20), STR_NEW_GAME, static_cast<MenuPosition>(position) == MenuPosition::NewGame);
#ifdef STORY_IMPLEMENTED
                drawMenuPositionHCentered((y+=20), STR_STORY, static_cast<MenuPosition>(position) == MenuPosition::Story);
#endif
                drawMenuPositionHCentered((y+=20), STR_INSTRUCTIONS, static_cast<MenuPosition>(position) == MenuPosition::Instructions);
                drawMenuPositionHCentered((y+=20), STR_SETTINGS, static_cast<MenuPosition>(position) == MenuPosition::Settings);
                drawMenuPositionHCentered((y+=20), STR_RETURN_TO_BOOTLOADER, static_cast<MenuPosition>(position) == MenuPosition::ReturnToBootloader);
            }

            if (screen == VisibleScreen::Settings) {
                drawMenuPosition(0, 30, STR_GAME_DIFFICULTY, position == 0);
                drawMenuPosition(0, 50, STR_SMOOTH_SCROLLING, position == 1);
                drawMenuPosition(0, 70, STR_SHOW_PROFILING_INFO, position == 2);

                const char* s;
                switch (ctx.difficultyLevel) {
                    case 0: s = STR_GAME_DIFFICULTY_L1; break;
                    case 1: s = STR_GAME_DIFFICULTY_L2; break;
                    case 2: s = STR_GAME_DIFFICULTY_L3; break;
                    case 3: s = STR_GAME_DIFFICULTY_L4; break;
                    case 4: s = STR_GAME_DIFFICULTY_L5; break;
                    case 5: s = STR_GAME_DIFFICULTY_L6; break;
                }

                dx = SCREEN_WIDTH - 4 * strlen(s);
                drawMenuPositionParam(dx, 30, s, position == 0);

                if (ctx.flags & FLAG_SMOOTH_SCROLLING) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                dx = SCREEN_WIDTH - 4 * strlen(s);
                drawMenuPositionParam(dx, 50, s, position == 1);

                if (ctx.flags & FLAG_SHOW_PROFILING_INFO) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                dx = SCREEN_WIDTH - 4 * strlen(s);
                drawMenuPositionParam(dx, 70, s, position == 2);
            }

        }

    }

}}
