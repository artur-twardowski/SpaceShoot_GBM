#ifndef SST_TILESET_H
#define SST_TILESET_H

#include "Gamebuino-Meta-ADTCRV.h"
#include <stdint.h>

namespace spaceshoot { namespace tileset {

enum struct ElementID: uint8_t {
    None = 0,
    Stone,
    Bomb1, Bomb2, Bomb3, Bomb4,
    Bonus1, Bonus2, Bonus3, Bonus4,
    Destroyed1, Destroyed2, Destroyed3, Destroyed4, Destroyed5,
    BombStoned, BonusStoned,
    ShipTail, ShipFrontNormal, ShipFiringLeft, ShipFiringRight, ShipFiringBoth,
    ShipFiringGlowLeft, ShipFiringGlowRight, ShipTailFire,
    ShipTailExploding, ShipFrontExploding,
    Debris1, Debris2, Debris3, Debris4, Debris5, Debris6, Debris7, Debris8,

    Count
};


struct AnimationSequence {
    ElementID next;
    uint8_t speed;
};

extern const AnimationSequence animSequences[];

void load(Image& tileset);
void draw(Image& tileset, uint16_t x, uint16_t y, ElementID id);
void applyPalette(uint8_t paletteSlot, uint8_t firstRow, uint8_t lastRow);

static inline void updateAnimation(ElementID& element) {
    const AnimationSequence& animSeq = animSequences[static_cast<int>(element)];
    if (animSeq.speed && (gb.frameCount % animSeq.speed == 0)) {
        element = animSeq.next;
    }
}

static inline void updateAnimation(ElementID* memArea, size_t elementCount) {
    ElementID* end = memArea + elementCount;
    while (memArea < end) {
        updateAnimation(*memArea++);
    }
}

//static inline void updateAnimation

}} // namespace spaceshoot::tileset

#endif // SST_TILESET_H

