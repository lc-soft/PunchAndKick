// LifeBar：生命条，血条

/** 创建一个生命条（血条） */
LCUI_Widget *LifeBar_New(void);

/** 为生命条设置一条生命槽能够容纳的生命值 */
void LifeBar_SetFullHP( LCUI_Widget *widget, int hp );

/** 为生命条设置当前生命值 */
void LifeBar_SetHP( LCUI_Widget *widget, int hp );

/** 为生命条设置最大生命值 */
void LifeBar_SetMaxHP( LCUI_Widget *widget, int hp );

/** 注册生命条部件 */
void LifeBar_Regiser(void);
