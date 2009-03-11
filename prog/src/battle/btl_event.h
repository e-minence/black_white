//=============================================================================================
/**
 * @file	btl_event.h
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g���n���h������
 * @author	taya
 *
 * @date	2008.10.23	�쐬
 */
//=============================================================================================
#ifndef __BTL_EVENT_H__
#define __BTL_EVENT_H__


#include "btl_server_flow.h"
#include "btl_server.h"

//--------------------------------------------------------------
/**
*	�C�x���g�^�C�v
*/
//--------------------------------------------------------------
typedef enum {
	BTL_EVENT_NULL = 0,

	BTL_EVENT_CHECK_CONF,				///< �����`�F�b�N
	BTL_EVENT_CALC_CONF_DAMAGE,	///< �����_���[�W�v�Z
	BTL_EVENT_WAZA_EXECUTE,			///< ���U���s�`�F�b�N

	BTL_EVENT_WAZA_HIT_RATIO,		///< ���U�̖������␳�n���h��
	BTL_EVENT_WAZA_HIT_COUNT,		///< �q�b�g�񐔌���i������q�b�g���U�̂݁j
	BTL_EVENT_CRITICAL_RANK,		///< �N���e�B�J�����␳�n���h��
	BTL_EVENT_WAZA_POWER,				///< ���U�З͕␳�n���h��
	BTL_EVENT_ATTACKER_POWER,		///< �U�����̔\�͒l�i��������or�Ƃ������j�␳�n���h��
	BTL_EVENT_DEFENDER_GUARD,		///< �h�䑤�̔\�͒l�i�ڂ�����or�Ƃ��ڂ��j�␳�n���h��
	BTL_EVENT_DAMAGE_DENOM,			///< �h�䕪��␳�n���h��
	BTL_EVENT_DAMAGE_PROC1,			///< �^�C�v�v�Z�O�_���[�W�␳�n���h��
	BTL_EVENT_ATTACKER_TYPE,		///< �U�����|�P�^�C�v
	BTL_EVENT_DEFENDER_TYPE,		///< �h�䑤�|�P�^�C�v
	BTL_EVENT_TYPEMATCH_RATIO,	///< �U�����^�C�v��v���{��
	BTL_EVENT_WAZA_TYPE,				///< ���U�^�C�v�`�F�b�N�n���h��
	BTL_EVENT_WAZA_ATK_TYPE,		///< �U�����|�P�^�C�v�`�F�b�N�n���h��
	BTL_EVENT_WAZA_DEF_TYPE,		///< �h�䑤�|�P�^�C�v�`�F�b�N�n���h��
	BTL_EVENT_WAZA_DMG_PROC2,		///< �^�C�v�v�Z��_���[�W�␳�n���h��
	BTL_EVENT_WAZA_DMG_LAST,		///< �ŏI�_���[�W�␳�n���h��
	BTL_EVENT_WAZA_DMG_AFTER,		///< �_���[�W������
	BTL_EVENT_DECREMENT_PP,			///< �g�p���U��PP�l������

	BTL_EVENT_WAZA_REFRECT,			///< �����v�Z�n���h��
	BTL_EVENT_WAZA_ADD_TARGET,	///< ���U���󂯂����ւ̒ǉ�����
	BTL_EVENT_WAZA_ADD_USER,		///< ���U���g�������ւ̒ǉ�����

	BTL_EVENT_MEMBER_OUT,				///< �ʃ|�P�ޏ꒼�O
	BTL_EVENT_MEMBER_IN,				///< �ʃ|�P���꒼��
	BTL_EVENT_MEMBER_COMP,			///< �S�Q���|�P�o���
	BTL_EVENT_BEFORE_RANKDOWN,	///< �\�͉�����i�O�j
	BTL_EVENT_AFTER_RANKDOWN,		///< �\�͉�����i��j
	BTL_EVENT_ADD_SICK,					///< ��Ԉُ�ɂ�������
	BTL_EVENT_SICK_DAMAGE,			///< ��Ԉُ�ɂ��_���[�W�v�Z
	BTL_EVENT_SHRINK_CHECK,			///< �Ђ�݌v�Z
	BTL_EVENT_SHRINK_FIX,				///< �Ђ�݊m���


}BtlEventType;


enum {
	BTL_EVARG_COMMON_POKEID = 0,
	BTL_EVARG_COMMON_POKEPOS,
	BTL_EVARG_COMMON_END,

	BTL_EVARG_RANKDOWN_STATUS_TYPE = BTL_EVARG_COMMON_END,
	BTL_EVARG_RANKDOWN_VOLUME,
	BTL_EVARG_RANKDOWN_FAIL_FLAG,


	BTL_EVARG_MAX = 16,
};
//--------------------------------------------------------------
/**
*	�C�x���g�ϐ����x��
*/
//--------------------------------------------------------------
typedef enum {
	BTL_EVAR_NULL = 0,
	BTL_EVAR_SYS_SEPARATE,
	//---���������̓V�X�e���\��̈�-------------------------
	BTL_EVAR_POKEID,
	BTL_EVAR_POKEID_USER,
	BTL_EVAR_WAZAID,
	BTL_EVAR_SICKID,
	BTL_EVAR_STATUS_TYPE,
	BTL_EVAR_POKE_TYPE,
	BTL_EVAR_POKE_TYPE_PAIR,
	BTL_EVAL_VOLUME,
	BTL_EVAR_FAIL_FLAG,
	BTL_EVAR_RANDOM_FLAG,
	BTL_EVAR_CONF_DMG,
	BTL_EVAR_FAIL_REASON,
	BTL_EVAR_TURN_COUNT,
	BTL_EVAR_ALMOST_FLAG,
	BTL_EVAR_ADD_PER,

	BTL_EVAR_MAX,

}BtlEvVarLabel;


//--------------------------------------------------------------
/**
*	�C�x���g�ϐ��X�^�b�N
*/
//--------------------------------------------------------------
extern void BTL_EVENTVAR_Push( void );
extern void BTL_EVENTVAR_Pop( void );
extern void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, u32 value );
extern u32 BTL_EVENTVAR_GetValue( BtlEvVarLabel label );


//--------------------------------------------------------------
/**
*	�C�x���g�t�@�N�^�[�^�C�v
*/
//--------------------------------------------------------------
typedef enum {

	BTL_EVENT_FACTOR_WAZA,
	BTL_EVENT_FACTOR_TOKUSEI,
	BTL_EVENT_FACTOR_ITEM,

}BtlEventFactor;


extern void BTL_EVENT_InitSystem( void );
extern void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* server, BtlEventType type );


#endif
