#include "InstructionsContext.h"
#include "Tileset.h"
#include "Utils.h"

namespace spaceshoot { namespace instructions {

    static const char STR_HEADER[] = "*** INSTRUCTIONS ***";

    static const char STR_SHIP[] = "Your space ship.";
    static const char STR_CONTROLS1[] = "Use the D-Pad to move up";
    static const char STR_CONTROLS2[] = "and down. Press [A] button";
    static const char STR_CONTROLS3[] = "to fire a missile.";
    static const char STR_BLOCKS1[] = "Debris to destroy. Don't";
    static const char STR_BLOCKS2[] = "let them reach the station.";
    static const char STR_SCORING1[] = "Each hit is worth 5 points.";
    static const char STR_BLOCKS3[] = "A bomb. Shoot it to be able";
    static const char STR_BLOCKS4[] = "to fire a salvo.";
    static const char STR_CONTROLS4[] = "Press [B] button to fire";
    static const char STR_CONTROLS5[] = "a salvo, if equipped.";
    static const char STR_BLOCKS5[] = "A bonus block. Shoot it to get";
    static const char STR_BLOCKS6[] = "extra points.";
    static const char STR_SCORING2[] = "125 points, to be precise.";

    static const char STR_HINT1[] = "Bombs and bonus blocks expire when they";
    static const char STR_HINT2[] = "approach too close. Hit them quickly!";

    static const char STR_HINT3[] = "Don't waste the salvos when the debris";
    static const char STR_HINT4[] = "are far away. Fire them when it gets";
    static const char STR_HINT5[] = "really hot!";

    static const char STR_HINT6[] = "Every salvo left is worth 100 points";
    static const char STR_HINT7[] = "of bonus added at the end of the game.";
    static const char STR_HINT8[] = "Maybe you should not use them when";
    static const char STR_HINT9[] = "the mission is lost anyway? Remember,";
    static const char STR_HINT10[] = "good accuracy is awarded...";
    static const char STR_SCORING3[] = "You can get up to 2000 points if you";
    static const char STR_SCORING4[] = "don't waste any shot.";
    static const char STR_SCORING5[] = "Clearing all the debris will bring";
    static const char STR_SCORING6[] = "extra 500 points... Go for it!";
    
    static const char STR_HIDDEN1[] = "You wanted to force the console";
    static const char STR_HIDDEN2[] = "to read outside the memory, didn't you?";

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

        const uint8_t COLOR_DESCRIPTION = 12;
        const uint8_t COLOR_SCORING = 6;
        const uint8_t COLOR_HINTS = 8;
        const uint8_t COLOR_CONTROLS = 15;
        gb.display.clear();
        processEvents();

        tileset::applyPalette(0, 0, 127);
        gb.tft.colorCells.enabled = true;
        uint32_t f = 0;
        int sy = -4 - 8*18;
        bool enteringAnimation = true;

        while (1) {
            gb.display.clear();
            
            gb.display.setColor(2);
            gb.display.print(0, -4 - sy, STR_HEADER);

            size_t ix = 0;
            tileset::draw(tileSet, 4, 12 - sy, elements[ix++]);
            tileset::draw(tileSet, 8, 12 - sy, elements[ix++]);
            tileset::draw(tileSet, 8,  7 - sy, elements[ix++]);
            tileset::draw(tileSet, 8, 17 - sy, elements[ix++]);

            gb.display.setColor(COLOR_DESCRIPTION);
            gb.display.print(28, 6 - sy, STR_SHIP);
            gb.display.setColor(COLOR_CONTROLS);
            gb.display.print(28, 12 - sy, STR_CONTROLS1);
            gb.display.print(28, 18 - sy, STR_CONTROLS2);
            gb.display.print(28, 24 - sy, STR_CONTROLS3);

            tileset::draw(tileSet, 4, 40 - sy, elements[ix++]);
            tileset::draw(tileSet, 10, 40 - sy, elements[ix++]);
            tileset::draw(tileSet, 16, 40 - sy, elements[ix++]);
            tileset::draw(tileSet, 4, 46 - sy, elements[ix++]);
            tileset::draw(tileSet, 10, 46 - sy, elements[ix++]);
            tileset::draw(tileSet, 16, 46 - sy, elements[ix++]);
            tileset::draw(tileSet, 7, 52 - sy, elements[ix++]);
            tileset::draw(tileSet, 13, 52 - sy, elements[ix++]);
            
            gb.display.setColor(COLOR_DESCRIPTION);
            gb.display.print(28, 40 - sy, STR_BLOCKS1);
            gb.display.print(28, 46 - sy, STR_BLOCKS2);
            
            gb.display.setColor(COLOR_SCORING);
            gb.display.print(28, 52 - sy, STR_SCORING1);

            tileset::draw(tileSet, 4, 72 - sy, elements[ix++]);
            
            gb.display.setColor(COLOR_DESCRIPTION);
            gb.display.print(28, 62 - sy, STR_BLOCKS3);
            gb.display.print(28, 68 - sy, STR_BLOCKS4);
            gb.display.setColor(COLOR_CONTROLS);
            gb.display.print(28, 74 - sy, STR_CONTROLS4);
            gb.display.print(28, 80 - sy, STR_CONTROLS5);

            tileset::draw(tileSet, 4, 98 - sy, elements[ix++]);
            
            gb.display.setColor(COLOR_DESCRIPTION);
            gb.display.print(28, 96 - sy, STR_BLOCKS5);
            gb.display.print(28, 102 - sy, STR_BLOCKS6);

            gb.display.setColor(COLOR_SCORING);
            gb.display.print(28, 108 - sy, STR_SCORING2);

            gb.display.setColor(COLOR_HINTS);
            gb.display.print(0, 122 - sy, STR_HINT1);
            gb.display.print(0, 128 - sy, STR_HINT2);

            gb.display.print(0, 138 - sy, STR_HINT3);
            gb.display.print(0, 144 - sy, STR_HINT4);
            gb.display.print(0, 150 - sy, STR_HINT5);

            gb.display.print(0, 160 - sy, STR_HINT6);
            gb.display.print(0, 166 - sy, STR_HINT7);
            gb.display.print(0, 172 - sy, STR_HINT8);
            gb.display.print(0, 178 - sy, STR_HINT9);
            gb.display.print(0, 184 - sy, STR_HINT10);

            gb.display.setColor(COLOR_SCORING);
            gb.display.print(0, 194 - sy, STR_SCORING3);
            gb.display.print(0, 200 - sy, STR_SCORING4);
            gb.display.print(0, 206 - sy, STR_SCORING5);
            gb.display.print(0, 212 - sy, STR_SCORING6);

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
            
            gb.display.setColor(COLOR_DESCRIPTION);
            gb.display.print(0, -256 - sy, STR_HIDDEN1);
            gb.display.print(0, -250 - sy, STR_HIDDEN2);

            processEvents();

            if (gb.buttons.pressed(BUTTON_B)) {
                return;
            }
            if (buttonDown(BUTTON_DOWN)) {
                sy+=2;
            }
            if (buttonDown(BUTTON_UP)) {
                sy-=2;
            }
            f++;
            if (enteringAnimation) {
                if (sy < -4) {
                    sy += 8;
                } else {
                    enteringAnimation = false;
                }
            }
        }
    }
}}