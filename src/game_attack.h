// 处理游戏角色的攻击伤害

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			int attack_type,
			GamePlayer *victim,
			int victim_state );

/** 处理已经记录的攻击 */
void Game_ProcAttack(void);

/** 初始化攻击记录 */
void Game_InitAttackRecord(void);
