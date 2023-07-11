#include "Utils.h"

bool buttonPressed(Button button) {
    auto bstate = gb.buttons.states[(uint8_t)button];
    return bstate == 1 || (bstate >= 6 && bstate < 0xFFFE);
}


