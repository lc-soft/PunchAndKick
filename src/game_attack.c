// 处理游戏角色的攻击伤害

#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include "game.h"

typedef struct AttackRecord_ {
	GamePlayer *attacker;	/**< 攻击者 */
	int attack_type;	/**< 攻击类型 */
	GamePlayer *victim;	/**< 受害者 */
	int victim_state;	/**< 受害者的状态 */
} AttackRecord;

static LCUI_Queue attack_record;

/** 记录攻击者对受害者的攻击 */
void Game_RecordAttack( GamePlayer *attacker,
			int attack_type,
			GamePlayer *victim,
			int victim_state )
{
	AttackRecord data;
	data.attacker = attacker;
	data.attack_type = attack_type;
	data.victim = victim;
	data.victim_state = victim_state;
	Queue_Lock( &attack_record );
	Queue_Add( &attack_record, &data );
	Queue_Unlock( &attack_record );
}

/** 处理已经记录的攻击 */
void Game_ProcAttack(void)
{
	int n;
	double damage, true_damage, reduce;
	AttackRecord *p_data;

	Queue_Lock( &attack_record );
	n = Queue_GetTotal( &attack_record );
	for( ; n>=0; --n) {
		p_data = (AttackRecord*)Queue_Get( &attack_record, 0 );
		if( p_data == NULL ) {
			continue;
		}
		/* 根据攻击者的攻击类型，计算输出伤害 */
		switch(p_data->attack_type) {
		case ATTACK_TYPE_JUMP_PUNCH:
		case ATTACK_TYPE_PUNCH:
			damage = p_data->attacker->property.punch/10.0;
			break;
		case ATTACK_TYPE_JUMP_KICK:
		case ATTACK_TYPE_KICK:
			damage = p_data->attacker->property.kick/10.0;
			break;
		case ATTACK_TYPE_MACH_STOMP:
		case ATTACK_TYPE_JUMP_TREAD:
			damage = p_data->attacker->property.kick/6.0;
			break;
		case ATTACK_TYPE_JUMP_ELBOW:
			damage = p_data->attacker->property.punch/6.0;
			break;
		case ATTACK_TYPE_S_PUNCH:
			damage = p_data->attacker->property.punch/3.0;
			break;
		case ATTACK_TYPE_S_KICK:
			damage = p_data->attacker->property.kick/3.0;
			break;
		case ATTACK_TYPE_SJUMP_KICK:
			damage = p_data->attacker->property.kick/2.0;
			break;
		case ATTACK_TYPE_SJUMP_PUNCH:
			damage = p_data->attacker->property.punch/2.0;
			break;
		case ATTACK_TYPE_RIDE_ATTACK:
			damage = p_data->attacker->property.punch/4.0;
			break;
		case ATTACK_TYPE_RIDE_JUMP_ATTACK:
			damage = 10+p_data->attacker->property.kick/5.0;
			break;
		case ATTACK_TYPE_BIG_ELBOW:
			damage = 20+p_data->attacker->property.punch/3.0;
			break;
		case ATTACK_TYPE_GUILLOTINE:
			damage = 20+p_data->attacker->property.kick/3.0;
			break;
		case ATTACK_TYPE_SPIN_HIT:
		case ATTACK_TYPE_SPIN_HIT2:
			damage = 30 + p_data->attacker->property.kick/4.0;
			damage += p_data->attacker->property.punch/4.0;
			break;
		case ATTACK_TYPE_BOMB_KICK:
			damage = 50 + p_data->attacker->property.kick/4.0;
			break;
		case ATTACK_TYPE_JUMP_SPIN_KICK:
			damage = 40 + p_data->attacker->property.kick/4.0;
		case ATTACK_TYPE_FINAL_BLOW:
			damage = p_data->attacker->property.punch/4.0;
			damage += p_data->attacker->property.kick/4.0;
			damage *= 2;
			break;
		case ATTACK_TYPE_THROW:
			damage = 20 + p_data->attacker->property.throw/5.0;
			break;
		case ATTACK_TYPE_BUMPED:
			damage = 20 + p_data->victim->property.max_hp*0.05;
			break;
		case ATTACK_TYPE_ELBOW1:
			damage = 20 + p_data->attacker->property.punch/5.0;
			break;
		case ATTACK_TYPE_ELBOW2:
			damage = 40 + p_data->attacker->property.punch/3.0;
			break;
		case ATTACK_TYPE_KNEE_HIT1:
			damage = 20 + p_data->attacker->property.punch/5.0;
			damage += p_data->attacker->property.kick/5.0;
			break;
		case ATTACK_TYPE_KNEE_HIT2:
			damage = 20 + p_data->victim->property.max_hp*0.03;
			break;
		case ATTACK_TYPE_NONE:
		default:
			damage = 0;
			break;
		}
		/* 根据受害者的防御力，计算伤害的减免比例 */
		reduce = p_data->victim->property.defense + 100;
		reduce = 1 - 100 / reduce;
		/* 根据受害者的状态，增加伤害减免比例 */
		switch( p_data->victim_state ) {
		case STATE_TUMMY:
		case STATE_TUMMY_HIT:
		case STATE_LYING:
		case STATE_LYING_HIT:
		case STATE_BE_LIFT_TUMMY:
		case STATE_BE_LIFT_TUMMY_HIT:
		case STATE_BE_LIFT_LYING:
		case STATE_BE_LIFT_LYING_HIT:
			/* 躺着或者趴着，减免20%的伤害 */
			reduce += 0.30;
			break;
		case STATE_F_ROLL:
		case STATE_B_ROLL:
			reduce += 0.20;
			break;
		default:
			break;
		}
		/* 限制最大伤害减免比例为95% */
		if( reduce > 0.95 ) {
			reduce = 0.95;
		}
		/* 计算真实伤害 */
		true_damage = damage * (1.00 - reduce) + 1;
		_DEBUG_MSG("true damage: %.2lf, damage: %.2lf, reduce: %.2lf\n", true_damage, damage, reduce*100 );
		/* 计算现在的血量 */
		p_data->victim->property.cur_hp -= (int)true_damage;
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
	Queue_Init( &attack_record, sizeof(AttackRecord), NULL );
}
