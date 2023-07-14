#ifndef SST_UTILS_H
#define SST_UTILS_H

#include "Gamebuino-Meta-ADTCRV.h"

static inline void processEvents() {
    while (!gb.update()) ;
}

bool buttonPressed(Button button);
bool buttonDown(Button button);
#endif // SST_UTILS_H
