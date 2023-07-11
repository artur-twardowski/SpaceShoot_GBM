#ifndef SST_TILESET_H
#define SST_TILESET_H

#include "Gamebuino-Meta-ADTCRV.h"
#include <stdint.h>

namespace spaceshoot { namespace tileset {

enum struct ElementID {
    None = 0,
    Stone,  StoneDestroyed1,  StoneDestroyed2,  StoneDestroyed3,
    Bomb1,  Bomb2, Bomb3,
    Bonus1, Bonus2, Bonus3,

    Count
};


struct AnimationSequence {
    ElementID next;
    uint8_t speed;
};

extern const AnimationSequence animSequences[];

void load(Image& tileset);
void draw(Image& tileset, uint8_t x, uint8_t y, ElementID id);

}} // namespace spaceshoot::tileset

#endif // SST_TILESET_H

