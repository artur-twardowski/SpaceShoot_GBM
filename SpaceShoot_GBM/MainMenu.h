#ifndef SST_MAINMENU_H
#define SST_MAINMENU_H

#include "GameContext.h"

namespace spaceshoot { namespace mainmenu {

    enum struct MenuPosition {
        NewGame = 0,
#ifdef STORY_IMPLEMENTED
        Story,
#endif
        Instructions,
        Settings,
        ReturnToBootloader,

        Count
    };

    MenuPosition run(GameContext& ctx);

}}

#endif // SST_MAINMENU_H
