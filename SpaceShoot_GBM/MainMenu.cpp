#include "MainMenu.h"
#include "GameContext.h"
#include "Utils.h"

namespace spaceshoot { namespace mainmenu {

    const char STR_NEW_GAME[] = "NEW GAME";
    const char STR_STORY[] = "STORY";
    const char STR_INSTRUCTIONS[] = "INSTRUCTIONS";
    const char STR_SETTINGS[] = "SETTINGS";
    const char STR_RETURN_TO_BOOTLOADER[] = "RETURN TO BOOTLOADER";

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

    enum struct RootMenuPositions {
        NewGame = 0,
        Story,
        Instructions,
        Settings,
        ReturnToBootloader
    };

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

    static void drawMenuPositionParam(uint8_t x, uint8_t y, const char* str, int selectedPosition, int assignedPosition) {
        if (selectedPosition == assignedPosition) {
            gb.display.setColor(INDEX_LIGHTGREEN);
        } else {
            gb.display.setColor(INDEX_GREEN);
        }

        gb.display.print(x, y, str);
    }

    void run(GameContext& ctx) {
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
                size_t lastPosition;
                if (screen == VisibleScreen::Main) {
                    lastPosition = 4;
                } else {
                    lastPosition = 1;
                }
                if (position < lastPosition)
                    position++;
            }
            if (buttonPressed(BUTTON_LEFT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel > 0) ctx.difficultyLevel--; break;
                    case 1: ctx.flags &= ~FLAG_SMOOTH_SCROLLING; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_RIGHT)) {
                if (screen == VisibleScreen::Settings) {
                    switch (position) {
                    case 0: if (ctx.difficultyLevel < 5) ctx.difficultyLevel++; break;
                    case 1: ctx.flags |= FLAG_SMOOTH_SCROLLING; break;
                    }
                }
            }
            if (buttonPressed(BUTTON_A)) {
                if (screen == VisibleScreen::Main) {
                    if (static_cast<RootMenuPositions>(position) == RootMenuPositions::NewGame) {
                        break;
                    }
                    if (static_cast<RootMenuPositions>(position) == RootMenuPositions::Settings) {
                        screen = VisibleScreen::Settings;
                        position = 0;
                    }
                }
            }
            if (buttonPressed(BUTTON_B)) {
                if (screen == VisibleScreen::Settings) {
                    screen = VisibleScreen::Main;
                }
            }

            if (screen == VisibleScreen::Main) {
                drawMenuPositionHCentered(20, STR_NEW_GAME, position == 0);
                drawMenuPositionHCentered(40, STR_STORY, position == 1);
                drawMenuPositionHCentered(60, STR_INSTRUCTIONS, position == 2);
                drawMenuPositionHCentered(80, STR_SETTINGS, position == 3);
                drawMenuPositionHCentered(100, STR_RETURN_TO_BOOTLOADER, position == 4);
            }

            if (screen == VisibleScreen::Settings) {
                drawMenuPosition(0, 30, STR_GAME_DIFFICULTY, position == 0);
                drawMenuPosition(0, 50, STR_SMOOTH_SCROLLING, position == 1);

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
                drawMenuPositionParam(dx, 30, s, position, 0);

                if (ctx.flags & FLAG_SMOOTH_SCROLLING) {
                    s = STR_YES;
                } else {
                    s = STR_NO;
                }

                dx = SCREEN_WIDTH - 4 * strlen(s);
                drawMenuPositionParam(dx, 50, s, position, 1);
            }

        }

    }

}}
