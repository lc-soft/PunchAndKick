
enum RoleID {
	ROLE_RIKI
};

#define GLOBAL_Y_WIDTH	12

ActionData* ActionRes_LoadRiki( int action_type );

/** 载入指定角色的动作动画 */
ActionData* ActionRes_Load( int id, int action_type );
