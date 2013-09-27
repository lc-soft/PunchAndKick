// 处理游戏角色的攻击伤害

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"

#define NAME_MAX_LEN	64

typedef struct AttackRecord_ {
	GamePlayer *attacker;			/**< 攻击者 */
	GamePlayer *victim;			/**< 受害者 */
	char attack_type_name[NAME_MAX_LEN];	/**< 攻击类型 */
	int victim_state;			/**< 受害者的状态 */
} AttackRecord;

typedef struct AttackInfo_ {
	int attack_type_id;				/**< 攻击标识符 */
	char attack_type_name[NAME_MAX_LEN];		/**< 攻击名称 */
	int (*get_damage)(GamePlayer*,GamePlayer*,int);	/**< 获取实际伤害值 */
	void (*effect)(GamePlayer*, GamePlayer*);	/**< 实现攻击带来的效果 */
} AttackInfo;

static LCUI_Queue attack_record;
static LCUI_Queue attack_library;

void AttackLibrary_Init(void)
{
	Queue_Init( &attack_library, sizeof(AttackInfo), NULL );
}

static AttackInfo *AttackLibrary_GetInfo( const char *attack_type_name )
{
	AttackInfo *p_info;
	int n, attack_type_id;
	attack_type_id = (int)BKDRHash( attack_type_name );
	n = Queue_GetTotal( &attack_library );
	while(n--) {
		p_info = (AttackInfo*)Queue_Get( &attack_library, n );
		if( !p_info ) {
			continue;
		}
		if( p_info->attack_type_id == attack_type_id ) {
			return p_info;
		}
	}
	return FALSE;
}

/**< 添加攻击数据 */
int AttackLibrary_AddAttack(	const char *attack_name, 
				int (*get_damage)(GamePlayer*,GamePlayer*,int),
				void (*effect)(GamePlayer*,GamePlayer*) )
{
	AttackInfo data;
	strncpy( data.attack_type_name, attack_name, NAME_MAX_LEN );
	data.attack_type_name[NAME_MAX_LEN-1] = 0;
	data.attack_type_id = (int)BKDRHash( data.attack_type_name );
	data.get_damage = get_damage;
	data.effect = effect;
	Queue_Add( &attack_library, &data );
	return data.attack_type_id;
}

/** 根据受害者的防御力及其它情况，获取减免后的实际伤害 */
int DamageReduce( GamePlayer *victim, int victim_state, int damage )
{
	double reduce;
	/* 根据受害者的防御力，计算伤害的减免比例 */
	reduce = victim->property.defense + 100.0;
	reduce = 1.0 - 100.0 / reduce;
	/* 根据受害者的状态，增加伤害减免比例 */
	switch( victim_state ) {
	case STATE_SOLID_DEFENSE:
		reduce += 0.40;
		break;
	case STATE_DEFENSE:
	case STATE_TUMMY:
	case STATE_TUMMY_HIT:
	case STATE_LYING:
	case STATE_LYING_HIT:
	case STATE_BE_LIFT_TUMMY:
	case STATE_BE_LIFT_TUMMY_HIT:
	case STATE_BE_LIFT_LYING:
	case STATE_BE_LIFT_LYING_HIT:
		/* 躺着或者趴着，减免20%的伤害 */
		reduce += 0.20;
		break;
	case STATE_F_ROLL:
	case STATE_B_ROLL:
		reduce += 0.10;
		break;
	default:
		break;
	}
	/* 限制最大伤害减免比例为95% */
	if( reduce > 0.95 ) {
		reduce = 0.95;
	}
	return damage - (int)(damage*reduce);
}

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			const char *attack_type_name,
			GamePlayer *victim,
			int victim_state )
{
	AttackRecord data;
	data.attacker = attacker;
	strncpy( data.attack_type_name, attack_type_name, NAME_MAX_LEN );
	data.attack_type_name[NAME_MAX_LEN-1] = 0;
	data.victim = victim;
	data.victim_state = victim_state;
	Queue_Lock( &attack_record );
	Queue_Add( &attack_record, &data );
	Queue_Unlock( &attack_record );
}

/** 处理已经记录的攻击 */
void Game_ProcAttack(void)
{
	int n, true_damage;
	AttackRecord *p_data;
	AttackInfo *p_info;

	Queue_Lock( &attack_record );
	n = Queue_GetTotal( &attack_record );
	for( ; n>=0; --n) {
		p_data = (AttackRecord*)Queue_Get( &attack_record, 0 );
		if( !p_data || !p_data->victim ) {
			continue;
		}
		/* 获取该类型攻击的信息 */
		p_info = AttackLibrary_GetInfo( p_data->attack_type_name );
		if( !p_info ) {
			Queue_Delete( &attack_record, 0 );
			continue;
		}
		if( p_info->effect ) {
			/* 实现攻击产生的效果 */
			p_info->effect( p_data->attacker, p_data->victim );
		}
		if( p_info->get_damage ) {
			/* 计算真实伤害 */
			true_damage = p_info->get_damage( p_data->attacker, p_data->victim, p_data->victim_state );
			DEBUG_MSG("attack type: %s, damage: %d\n", p_data->attack_type_name, true_damage);
		} else {
			true_damage = 0;
		}
		/* 计算现在的血量 */
		p_data->victim->property.cur_hp -= true_damage;
		if( p_data->victim->property.cur_hp < 0 ) {
			p_data->victim->property.cur_hp = 0;
		}
		/* 设置状态栏上显示的血量 */
		StatusBar_SetHealth(	p_data->victim->statusbar,
					p_data->victim->property.cur_hp );
		/* 删除该攻击记录 */
		Queue_Delete( &attack_record, 0 );
	}
	Queue_Unlock( &attack_record );
}

/** 初始化攻击记录 */
void Game_InitAttackRecord(void)
{
	AttackLibrary_Init();
	Queue_Init( &attack_record, sizeof(AttackRecord), NULL );
}
