#ifndef SST_TILESET_H
#define SST_TILESET_H

#include "Gamebuino-Meta-ADTCRV.h"
#include <stdint.h>

namespace spaceshoot { namespace tileset {

enum struct ElementID {
    None = 0,
    Stone,
    Bomb1, Bomb2, Bomb3, Bomb4,
    Bonus1, Bonus2, Bonus3, Bonus4,
    Destroyed1, Destroyed2, Destroyed3, Destroyed4, Destroyed5,
    BombStoned, BonusStoned,
    ShipTail, ShipFrontNormal, ShipFiringLeft, ShipFiringRight, ShipFiringBoth,
    ShipFiringGlowLeft, ShipFiringGlowRight, ShipTailFire,
    ShipTailExploding, ShipFrontExploding,

    Count
};


struct AnimationSequence {
    ElementID next;
    uint8_t speed;
};

extern const AnimationSequence animSequences[];

void load(Image& tileset);
void draw(Image& tileset, uint8_t x, uint8_t y, ElementID id);
void applyPalette(uint8_t paletteSlot, uint8_t firstRow, uint8_t lastRow);

}} // namespace spaceshoot::tileset

#endif // SST_TILESET_H

