
LCUI_BEGIN_HEADER
	
#define GAME_SCREEN_WIDTH	800
#define GAME_SCREEN_HEIGHT	600
#define STATUS_BAR_HEIGHT	90	/**< 游戏角色的状态信息区域的高度 */

#include "game_lifebar.h"
#include "game_object.h"
#include "game_control.h"
#include "game_resource.h"
#include "game_action.h"
#include "game_space.h"
#include "game_msg.h"
#include "game_statusbar.h"
#include "game_attack.h"
#include "game_battle.h"
	
wchar_t *GetPlayerName(void);

LCUI_END_HEADER
