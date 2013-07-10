#include <LCUI_Build.h>
#include LC_LCUI_H

#include "game.h"

/** 载入指定角色的动作动画 */
ActionData* ActionRes_Load( int id, int action_type )
{
	switch(id) {
	case ROLE_RIKI: return ActionRes_LoadRiki( action_type );
	default:
		break;
	}
	return NULL;
}
