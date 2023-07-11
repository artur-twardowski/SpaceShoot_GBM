#include "MainMenu.h"
#include "Utils.h"

namespace spaceshoot { namespace mainmenu {

    const char STR_NEW_GAME[] = "NEW GAME";
    const char STR_SETTINGS[] = "SETTINGS";

    const char STR_GAME_VARIANT[] = "GAME VARIANT";
    const char STR_GAME_VARIANT_CLASSIC[] = "CLASSIC";
    const char STR_GAME_VARIANT_META[] = "META";

    const char STR_SMOOTH_SCROLLING[] = "SMOOTH SCROLLING";
    const char STR_YES[] = "YES";
    const char STR_NO[] = "NO";

    const char STR_SMOOTH_SCROLLING_INFO_ROW0[] = "IMPROVES THE EXPERIENCE, BUT";
    const char STR_SMOOTH_SCROLLING_INFO_ROW1[] = "MAY AFFECT THE PERFORMANCE.";

    enum struct RootMenuPositions {
        NewGame = 0,
        Settings
    };

    enum struct VisibleScreen {
        Main = 0,
        Settings = 1
    };

    static void drawMenuPosition(uint8_t x, uint8_t y, const char* str, int selectedPosition, int assignedPosition) {
        if (selectedPosition == assignedPosition) {
            gb.display.setColor(INDEX_WHITE);
        } else {
            gb.display.setColor(INDEX_ORANGE);
        }

        gb.display.print(x, y, str);
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
                if (position < 1)
                    position++;
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
                drawMenuPosition(30, 30, STR_NEW_GAME, position, 0);
                drawMenuPosition(30, 50, STR_SETTINGS, position, 1);
            }

            if (screen == VisibleScreen::Settings) {
                drawMenuPosition(0, 30, STR_GAME_VARIANT, position, 0);
                drawMenuPosition(0, 50, STR_SMOOTH_SCROLLING, position, 1);

                dx = SCREEN_WIDTH - sizeof(STR_GAME_VARIANT_META) * 4;
                drawMenuPositionParam(dx, 30, STR_GAME_VARIANT_META, position, 0);

                dx = SCREEN_WIDTH - sizeof(STR_YES) * 4;
                drawMenuPositionParam(dx, 50, STR_YES, position, 1);

                if (position == 1) {
                    gb.display.setColor(INDEX_RED);
                    gb.display.print(0, 112, STR_SMOOTH_SCROLLING_INFO_ROW0);
                    gb.display.print(0, 120, STR_SMOOTH_SCROLLING_INFO_ROW1);
                }
            }

        }

    }

}}
