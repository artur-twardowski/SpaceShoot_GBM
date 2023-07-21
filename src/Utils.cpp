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

#include "Utils.h"
#include <Gamebuino-Meta-ADTCRV.h>
#include "utility/Misc.h"

bool buttonPressed(Button button) {
    auto bstate = gb.buttons.states[(uint8_t)button];
    return bstate == 1 || (bstate >= 6 && bstate < 0xFFFE);
}

bool buttonDown(Button button) {
    auto bstate = gb.buttons.states[(uint8_t)button];
    return bstate >= 1 && bstate < 0xFFFE;
}

void paletteSyncFadeToBlack(uint8_t firstPalette, uint8_t lastPalette, uint8_t fadeTime) {
    for (int ix = 0; ix < fadeTime; ix++) {
        for (size_t pal = firstPalette; pal <= lastPalette; pal++) {
            paletteFadeFromBlack((uint16_t*)gb.tft.colorCells.palettes[pal], (uint16_t*)gb.tft.colorCells.palettes[pal], fadeTime - ix, fadeTime);
        }
        processEvents();
    }
}

void paletteFadeFromBlack(uint16_t* dest, const uint16_t* src, unsigned int val, unsigned int max) {
    for (size_t ix = 0; ix < 16; ix++) {
        unsigned int r, g, b;
        Gamebuino_Meta::RGB888 color = Gamebuino_Meta::rgb565Torgb888(src[ix]);
        r = color.r;
        g = color.g;
        b = color.b;
        r = r * val / max;
        g = g * val / max;
        b = b * val / max;
        color.r = r;
        color.g = g;
        color.b = b;
        dest[ix] = Gamebuino_Meta::rgb888Torgb565(color);
    }
}

