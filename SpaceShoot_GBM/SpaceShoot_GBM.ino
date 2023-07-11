  #include "SpaceShoot.h"
#include "Configuration.h"

void setup() {
  spaceshoot::init();
  gb.begin();
  gb.display.init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_MODE);
  gb.setFrameRate(TARGET_FPS);
}

void loop() {
  spaceshoot::main();
}
