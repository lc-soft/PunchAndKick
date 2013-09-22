#ifndef __GAME_ACTION_H__
#define __GAME_ACTION_H__
enum RoleID {
	ROLE_RIKI,
	ROLE_TORAJI
};

#define GLOBAL_Y_WIDTH	12

int ActionRes_Riki_GetActionTotalNum(void);
const ActionInfo *ActionRes_Riki_GetActionSet(void);

int ActionRes_Toraji_GetActionTotalNum(void);
const ActionInfo *ActionRes_Toraji_GetActionSet(void);

/** 载入指定角色的动作动画 */
ActionData* ActionRes_Load( int id, int action_type );
#endif