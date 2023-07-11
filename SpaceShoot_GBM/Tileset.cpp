#include "Tileset.h"
#include "GameContext.h"

namespace spaceshoot { namespace tileset {

const AnimationSequence animSequences[] = {
  /* None */ {ElementID::None, 0},
  /* Stone */ {ElementID::Stone, 0},
  /* StoneDestroyed1 */ {ElementID::StoneDestroyed2, 1},
  /* StoneDestroyed2 */ {ElementID::StoneDestroyed3, 1},
  /* StoneDestroyed3 */ {ElementID::None, 1},
  /* Bomb1 */ {ElementID::Bomb2, 2},
  /* Bomb2 */ {ElementID::Bomb3, 2},
  /* Bomb3 */ {ElementID::Bomb1, 2},
  /* Bonus1 */ {ElementID::Bonus2, 2},
  /* Bonus2 */ {ElementID::Bonus3, 2},
  /* Bonus3 */ {ElementID::Bonus1, 2},
};

static_assert(sizeof(animSequences) / sizeof(animSequences[0]) == static_cast<size_t>(ElementID::Count));
static_assert(static_cast<size_t>(ElementID::Count) <= BLOCK_MASK);

void draw(Image& tileset, uint8_t x, uint8_t y, ElementID elementID) {
    gb.display.drawImage(x, y,
            tileset,
            static_cast<size_t>(elementID) * BLOCK_WIDTH, 0,
            BLOCK_WIDTH, BLOCK_HEIGHT);
}

void load(Image& tileset) {
  tileset.init(BLOCK_WIDTH * static_cast<size_t>(ElementID::Count), BLOCK_HEIGHT, SCREEN_MODE);

  for (size_t y = 0; y < BLOCK_HEIGHT; y++) {
    for (size_t tileNo = 0; tileNo < static_cast<size_t>(ElementID::Count); tileNo++) {
      for (size_t x = 0; x < BLOCK_WIDTH; x++) {
        size_t drawX = tileNo * BLOCK_WIDTH;
        switch (static_cast<ElementID>(tileNo)) {
        case ElementID::None:
          tileset.drawPixel(drawX + x, y, INDEX_BLACK);
          break;
        case ElementID::Stone:
          tileset.drawPixel(drawX + x, y, ((x + y) % 2) ? INDEX_YELLOW : INDEX_GRAY);
          break;
        case ElementID::StoneDestroyed1:
          tileset.drawPixel(drawX + x, y, INDEX_WHITE);
          break;
        case ElementID::StoneDestroyed2:
          tileset.drawPixel(drawX + x, y, ((x + y) % 2) ? INDEX_BLACK : INDEX_WHITE);
          break;
        case ElementID::StoneDestroyed3:
          tileset.drawPixel(drawX + x, y, ((x + y) % 2) ? INDEX_BLACK : INDEX_GRAY);
          break;
        case ElementID::Bomb1:
          tileset.drawPixel(drawX + x, y, INDEX_RED);
          break;
        case ElementID::Bomb2:
          tileset.drawPixel(drawX + x, y, ((x + y) % 2) ? INDEX_RED : INDEX_PURPLE);
          break;
        case ElementID::Bomb3:
          tileset.drawPixel(drawX + x, y, ((x + y) % 2) ? INDEX_PURPLE : INDEX_RED);
          break;
        case ElementID::Bonus1:
          tileset.drawPixel(drawX + x, y, INDEX_GREEN);
          break;
        }
      }
    }
  }
}


}}
