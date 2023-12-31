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

#ifndef SST_UTILS_H
#define SST_UTILS_H

#include "Gamebuino-Meta-ADTCRV.h"

static inline void processEvents() {
    while (!gb.update()) ;
}

bool buttonPressed(Button button);
bool buttonDown(Button button);

static inline void setTextFormat(ColorIndex color, uint8_t w, uint8_t h, const uint8_t* font) {
    gb.display.setColor(color);
    gb.display.setFont(font);
    gb.display.setFontSize(w, h);
}

void paletteFadeFromBlack(uint16_t* dest, const uint16_t* src, unsigned int val, unsigned int max);
void paletteSyncFadeToBlack(uint8_t firstPalette, uint8_t lastPalette, uint8_t fadeTime);

#endif // SST_UTILS_H
