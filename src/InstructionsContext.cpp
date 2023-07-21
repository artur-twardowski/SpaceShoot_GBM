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

#include "InstructionsContext.h"
#include "Tileset.h"
#include "Utils.h"
#include "font4x7.c"
#include "utility/Graphics/font3x5.c"

namespace spaceshoot { namespace context { namespace instructions {

    static const char STR_HEADER[] = "INSTRUCTIONS";

    static const char STR_PAGING_FIRST[] = "\x13:next page";
    static const char STR_PAGING_MID[] = "\x12:previous page \x13:next page";
    static const char STR_PAGING_LAST[] = "\x12:previous page";
    
    static const char STR_RET_MENU[] = "\x02\x09:return to menu";

    static const char STR_SHIP[] = "Your space ship.";
    static const char STR_SHIP_CTRL1[] = "Use the D-Pad(\x12\x13)to move up";
    static const char STR_SHIP_CTRL2[] = "and down. Press \x01\x08 button";
    static const char STR_SHIP_CTRL3[] = "to fire missiles. You can keep";
    static const char STR_SHIP_CTRL4[] = "these buttons pressed for conti-";
    static const char STR_SHIP_CTRL5[] = "nuous movement and/or firing.";
    static const char STR_BLOCKS1[] = "Debris to destroy. Don't";
    static const char STR_BLOCKS2[] = "let them reach the station.";
    static const char STR_SCORING1[] = "Each hit is worth 5 points.";
    static const char STR_BLOCKS3[] = "A bomb. Shoot it to be able";
    static const char STR_BLOCKS4[] = "to fire a salvo.";
                                       /* -------|-------|-------|-------| */
    static const char STR_BOMB_CTRL1[] = "You will see a red flash of the";
    static const char STR_BOMB_CTRL2[] = "console lights when you collect";
    static const char STR_BOMB_CTRL3[] = "one. Press \x02\x09 to use it. ";
    static const char STR_BONUS_HINT1[] = "Again, you will see a green";
    static const char STR_BONUS_HINT2[] = "flash on the right hand side if";
    static const char STR_BONUS_HINT3[] = "you hit this block.";
    static const char STR_LIGHTS_EN[] = "(unless you disabled the lights)";

    static const char STR_CONTROLS4[] = "Press \x02\x09 button to fire a salvo";
    static const char STR_CONTROLS5[] = "if equipped. ";
    static const char STR_BLOCKS5[] = "A bonus block. Shoot it";
    static const char STR_BLOCKS6[] = "to get extra points.";
    static const char STR_SCORING2[] = "125 points, to be precise.";

    static const char STR_HINT1A[] = "Bombs and bonus blocks turn into";
    static const char STR_HINT1B[] = "stone when they are too close";
    static const char STR_HINT1C[] = "(after passing halfway the dis-";
    static const char STR_HINT1D[] = "tance). Better hit them as soon";
    static const char STR_HINT1E[] = "as you can.";

                                   /* -------|-------|-------|-------| */
    static const char STR_HINT2A[] = "Don't waste your salvos while";
    static const char STR_HINT2B[] = "the debris are far away.";
    static const char STR_HINT2C[] = "Fire them when it gets really";
    static const char STR_HINT2D[] = "hot! You'll see, close to";
    static const char STR_HINT2E[] = "the end of the game you will";
    static const char STR_HINT2F[] = "desperately need them.";

                                   /* -------|-------|-------|-------| */
    static const char STR_HINT3A[] = "Every salvo remaining is worth";
    static const char STR_HINT3B[] = "100 points of bonus added"; 
    static const char STR_HINT3C[] = "at the end of the game.";
    static const char STR_HINT3D[] = "Maybe you should not use them";
    static const char STR_HINT3E[] = "when the mission is lost anyway?";
    static const char STR_HINT3F[] = "Keep in mind that good accuracy";
    static const char STR_HINT3G[] = "is awarded...";

    static const char STR_SCORING3A[] = "You can get up to 2000 points";
    static const char STR_SCORING3B[] = "if you don't waste any shot.";
    static const char STR_SCORING3C[] = "Clearing all the debris will";
    static const char STR_SCORING3D[] = "give you extra 500 points...";
    static const char STR_SCORING3E[] = "Go for it!";

    static const char STR_HIDDEN1A[] = "Who told you that \x04\x14 button";
    static const char STR_HIDDEN1B[] = "has any meaning here?";

    static void setupPalettesForPages(uint8_t page) {
        switch (page) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 128:
            case 129:
                memset(gb.tft.colorCells.paletteToLine + 0, 1, 20);
                memset(gb.tft.colorCells.paletteToLine + 20, 0, 108);
                break;
            case 127:
            case 130:
                memset(gb.tft.colorCells.paletteToLine, 1, 128);
                break;
        }
    }

    void run(Image& tileSet) {
        tileset::ElementID elements[] = {
            tileset::ElementID::ShipTail,
            tileset::ElementID::ShipFrontNormal,
            tileset::ElementID::None,
            tileset::ElementID::None,
            tileset::ElementID::Debris1,
            tileset::ElementID::Debris2,
            tileset::ElementID::Debris3,
            tileset::ElementID::Debris4,
            tileset::ElementID::Debris5,
            tileset::ElementID::Debris6,
            tileset::ElementID::Debris7,
            tileset::ElementID::Debris8,
            tileset::ElementID::Bomb1,
            tileset::ElementID::Bonus1
        };

        const ColorIndex COLOR_DESCRIPTION = (ColorIndex)12;
        const ColorIndex COLOR_SCORING = (ColorIndex)6;
        const ColorIndex COLOR_HINTS = (ColorIndex)8;
        const ColorIndex COLOR_CONTROLS = (ColorIndex)15;
        gb.display.clear();
        processEvents();

        gb.tft.setPalette(Gamebuino_Meta::defaultColorPalette);
        gb.tft.colorCells.palettes[0] = (Color*) tileset::palette;
        gb.tft.colorCells.palettes[1] = (Color*) Gamebuino_Meta::defaultColorPalette;
        gb.tft.colorCells.enabled = true;

        uint8_t page = 0;
        uint8_t f = 0;

        setupPalettesForPages(page);

        while (1) {
            gb.display.clear();

            if (page == 0) {
                gb.display.setColor(2);
                setTextFormat((ColorIndex)6, 2, 1, font4x7);
                gb.display.print(0, 20, STR_HEADER);
            
                tileset::draw(tileSet, 4, 40, elements[0]);
                tileset::draw(tileSet, 8, 40, elements[1]);
                tileset::draw(tileSet, 8, 35, elements[2]);
                tileset::draw(tileSet, 8, 40, elements[3]);

                setTextFormat(COLOR_DESCRIPTION, 1, 1, font4x7);
                gb.display.setColor(COLOR_DESCRIPTION);
                gb.display.print(28, 38, STR_SHIP);

                gb.display.setColor(COLOR_CONTROLS);
                gb.display.print(0, 54, STR_SHIP_CTRL1);
                gb.display.print(0, 64, STR_SHIP_CTRL2);
                gb.display.print(0, 74, STR_SHIP_CTRL3);
                gb.display.print(0, 84, STR_SHIP_CTRL4);
                gb.display.print(0, 94, STR_SHIP_CTRL5);
            }

            if (page == 1) {
                tileset::draw(tileSet, 4, 20, elements[4]);
                tileset::draw(tileSet, 10, 20, elements[5]);
                tileset::draw(tileSet, 16, 20, elements[6]);
                tileset::draw(tileSet, 4, 26, elements[7]);
                tileset::draw(tileSet, 10, 26, elements[8]);
                tileset::draw(tileSet, 16, 26, elements[9]);
                tileset::draw(tileSet, 7, 32, elements[10]);
                tileset::draw(tileSet, 13, 32, elements[11]);
                
                setTextFormat(COLOR_DESCRIPTION, 1, 1, font4x7);
                gb.display.print(24, 20, STR_BLOCKS1);
                gb.display.print(24, 30, STR_BLOCKS2);
                
                gb.display.setColor(COLOR_SCORING);
                gb.display.print(0, 40, STR_SCORING1);

                tileset::draw(tileSet, 4, 60, elements[12]);

                gb.display.setColor(COLOR_DESCRIPTION);
                gb.display.print(18, 55, STR_BLOCKS3);
                gb.display.print(18, 65, STR_BLOCKS4);
                gb.display.setColor(COLOR_CONTROLS);
                gb.display.print(0, 80, STR_BOMB_CTRL1);
                gb.display.print(0, 90, STR_BOMB_CTRL2);
                gb.display.print(0, 100, STR_BOMB_CTRL3);
                gb.display.setColor(COLOR_HINTS);
                gb.display.print(0, 110, STR_LIGHTS_EN);

            }
            if (page == 2) {
                setTextFormat(COLOR_DESCRIPTION, 1, 1, font4x7);
                tileset::draw(tileSet, 4, 25, elements[13]);
                
                gb.display.print(18, 20, STR_BLOCKS5);
                gb.display.print(18, 30, STR_BLOCKS6);

                gb.display.setColor(COLOR_SCORING);
                gb.display.print(0, 40, STR_SCORING2);

                gb.display.setColor(COLOR_HINTS);
                gb.display.print(0, 53, STR_BONUS_HINT1);
                gb.display.print(0, 62, STR_BONUS_HINT2);
                gb.display.print(0, 71, STR_BONUS_HINT3);

                gb.display.print(0, 84, STR_HINT1A);
                gb.display.print(0, 93, STR_HINT1B);
                gb.display.print(0, 102, STR_HINT1C);
                gb.display.print(0, 111, STR_HINT1D);
                gb.display.print(0, 120, STR_HINT1E);
            }

            if (page == 3) {
                setTextFormat(COLOR_HINTS, 1, 1, font4x7);
                gb.display.print(0, 20, STR_HINT2A);
                gb.display.print(0, 30, STR_HINT2B);
                gb.display.print(0, 40, STR_HINT2C);
                gb.display.print(0, 50, STR_HINT2D);
                gb.display.print(0, 60, STR_HINT2E);
                gb.display.print(0, 70, STR_HINT2F);

                gb.display.print(0, 85, STR_HINT3A);
                gb.display.print(0, 95, STR_HINT3B);
                gb.display.print(0, 105, STR_HINT3C);
                gb.display.print(0, 115, STR_HINT3D);
            }

            if (page == 4) {
                setTextFormat(COLOR_HINTS, 1, 1, font4x7);
                
                gb.display.print(0, 20, STR_HINT3E);
                gb.display.print(0, 30, STR_HINT3F);
                gb.display.print(0, 40, STR_HINT3G);

                setTextFormat(COLOR_SCORING, 1, 1, font4x7);

                gb.display.print(0, 55, STR_SCORING3A);
                gb.display.print(0, 65, STR_SCORING3B);
                gb.display.print(0, 75, STR_SCORING3C);
                gb.display.print(0, 85, STR_SCORING3D);
                gb.display.print(0, 95, STR_SCORING3E);
            }

            if (page == 127) {
                setTextFormat(INDEX_BEIGE, 1, 1, font4x7);
                gb.display.print(0, 60, STR_HIDDEN1A);
                gb.display.print(0, 68, STR_HIDDEN1B);

            }

            if (page == 128) {
                gb.display.setColor(2);
                setTextFormat((ColorIndex)6, 2, 1, font4x7);
                gb.display.print(0, 20, "CUSTOM 4x7 FONT");

                setTextFormat(COLOR_DESCRIPTION, 1, 1, font4x7);
                for (uint8_t y = 0; y < 8; y++) {
                    for (uint8_t x = 0; x < 16; x++) {
                        gb.display.drawChar(x * 9, 40 + y * 8, (char)(y * 16 + x), 1);
                    }
                }
            }

            if (page == 129 || page == 130) {
                gb.display.setColor(2);
                setTextFormat((ColorIndex)6, 2, 1, font4x7);
                if (page == 129) {
                    gb.display.print(0, 20, "TILESET PALETTE");
                } else {
                    gb.display.print(0, 20, "DEFAULT PALETTE");
                }

                setTextFormat(COLOR_DESCRIPTION, 1, 3, font4x7);
                for (uint8_t y = 0; y < 4; y++) {
                    for (uint8_t x = 0; x < 4; x++) {
                        gb.display.setColor(COLOR_DESCRIPTION);
                        gb.display.printf(40 * x + 5, 20 * y + 40, "%d", (y*4+x));
                        gb.display.setColor((ColorIndex)(y*4+x));
                        gb.display.fillRect(40 * x + 16, 20 * y + 40, 20, 24);
                    }
                }

            }
            
            setTextFormat((ColorIndex)1, 1, 1, font4x7);

            gb.display.setColor((ColorIndex)1);
            if (page == 0) {
                gb.display.print(0, 0, STR_PAGING_FIRST);
            } else if (page == 4) {
                gb.display.print(0, 0, STR_PAGING_LAST);
            } else {
                gb.display.print(0, 0, STR_PAGING_MID);
            }
            
            gb.display.print(0, 8, STR_RET_MENU);

            tileset::updateAnimation(elements, sizeof(elements) / sizeof(elements[0]));

            if ((f & 0x7F) == 0x20) {
                elements[1] = tileset::ElementID::ShipFiringLeft;
                elements[2] = tileset::ElementID::ShipFiringGlowLeft;
            }
            if ((f & 0x7F) == 0x60) {
                elements[1] = tileset::ElementID::ShipFiringRight;
                elements[3] = tileset::ElementID::ShipFiringGlowRight;
            }

            if ((f & 0xFF) == 0x70) {
                elements[12] = tileset::ElementID::BombStoned;
                elements[13] = tileset::ElementID::BonusStoned;
            }
            if ((f & 0xFF) == 0xA0) {
                elements[12] = tileset::ElementID::Bomb1;
                elements[13] = tileset::ElementID::Bonus1;
            }
            
            processEvents();

            if (gb.buttons.pressed(BUTTON_MENU)) {
                page = 127;
            }

            if (gb.buttons.pressed(BUTTON_B)) {
                return;
            }
            if (buttonPressed(BUTTON_DOWN)) {
                if (page < 4 || page >= 127) {
                    page++;
                    setupPalettesForPages(page);
                }
            }
            if (buttonPressed(BUTTON_UP)) {
                if (page == 128) {
                    page = 0;
                    setupPalettesForPages(page);
                } else if (page > 0) {
                    page--;
                    setupPalettesForPages(page);
                }
            }
            f++;
        }
    }
}}} // namespace spaceshoot::context::instructions
