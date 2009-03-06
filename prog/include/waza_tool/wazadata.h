//=============================================================================================
/**
 * @file	wazadata.h
 * @brief	�|�P����WB  ���U�f�[�^�A�N�Z�T
 * @author	taya
 *
 * @date	2008.10.14	�쐬
 */
//=============================================================================================
#ifndef __WAZADATA_H__
#define __WAZADATA_H__

#include "poke_tool/poketype.h"


typedef  unsigned short  WazaID;


//------------------------------------------------------------------------------
/**
 *	���U�̃J�e�S���ŏ�ʕ���
 */
//------------------------------------------------------------------------------
typedef enum {

	WAZADATA_CATEGORY_SIMPLE_DAMAGE,
	WAZADATA_CATEGORY_SIMPLE_SICK,
	WAZADATA_CATEGORY_SIMPLE_EFFECT,
	WAZADATA_CATEGORY_SIMPLE_RECOVER,
	WAZADATA_CATEGORY_DAMAGE_SICK,
	WAZADATA_CATEGORY_DAMAGE_EFFECT,
	WAZADATA_CATEGORY_DAMAGE_EFFECT_USER,
	WAZADATA_CATEGORY_ICHIGEKI,
	WAZADATA_CATEGORY_BIND,
	WAZADATA_CATEGORY_GUARD,
	WAZADATA_CATEGORY_FIELD_EFFECT,
	WAZADATA_CATEGORY_SIDE_EFFECT,
	WAZADATA_CATEGORY_OTHERS,

	WAZADATA_CATEGORY_MAX,

}WazaCategory;


typedef enum {

	WAZADATA_DMG_PHYSIC,	///< �����_���[�W
	WAZADATA_DMG_SPECIAL,	///< ����_���[�W
	WAZADATA_DMG_NONE,

}WazaDamageType;

//------------------------------------------------------------------------------
/**
 *	���U�ɂ���Ĉ����N��������Ԉُ�B
 *	PokeSick���o�g���p�Ɋg���������́B
 */
//------------------------------------------------------------------------------
typedef enum {

	WAZASICK_ORIGIN = POKESICK_ORIGIN,

	WAZASICK_DOKU = POKESICK_DOKU,
	WAZASICK_MAHI = POKESICK_MAHI,
	WAZASICK_NEMURI = POKESICK_NEMURI,
	WAZASICK_KOORI = POKESICK_KOORI,
	WAZASICK_YAKEDO = POKESICK_YAKEDO,

	WAZASICK_MOUDOKU = POKESICK_MAX,
	WAZASICK_KONRAN,
	WAZASICK_MEROMERO,
	WAZASICK_AKUMU,
	WAZASICK_TYOUHATSU,
	WAZASICK_SASIOSAE,
	WAZASICK_KAIHUKUHUUJI,
	WAZASICK_KANASIBARI,

	WAZASICK_MAX,
	WAZASICK_NULL = POKESICK_NULL,

}WazaSick;


//------------------------------------------------------------------------------
/**
 *	���U�ɂ���Ĕ�������ǉ����ʁi�U�����̃����N�A�b�v�E�_�E�����ʁj
 */
//------------------------------------------------------------------------------
typedef enum {

	WAZA_RANKEFF_NULL = 0,
	WAZA_RANKEFF_ORIGIN = 1,

	WAZA_RANKEFF_ATTACK = WAZA_RANKEFF_ORIGIN,
	WAZA_RANKEFF_DEFENCE,
	WAZA_RANKEFF_SP_ATTACK,
	WAZA_RANKEFF_SP_DEFENCE,
	WAZA_RANKEFF_AGILITY,
	WAZA_RANKEFF_HIT,
	WAZA_RANKEFF_AVOID,

	WAZA_RANKEFF_MAX,

}WazaRankEffect;


typedef enum {

	WAZA_TARGET_SINGLE,				///< �����ȊO�̂P�́i�I���j
	WAZA_TARGET_ENEMY2,				///< �G���Q��
	WAZA_TARGET_OTHER_ALL,		///< �����ȊO�S��

	WAZA_TARGET_ONLY_USER,		///< �����P�̂̂�
	WAZA_TARGET_SINGLE_FRIEND,///< �������܂ޖ����P�́i�I���j
	WAZA_TARGET_OTHER_FRIEND,	///< �����ȊO�̖����P��
	WAZA_TARGET_SINGLE_ENEMY,	///< �G�P�́i�I���j
	WAZA_TARGET_RANDOM,				///< �G�����_��

	WAZA_TARGET_TEAM_USER,		///< �������w�n
	WAZA_TARGET_TEAM_ENEMY,		///< �G���w�n
	WAZA_TARGET_FIELD,				///< ��Ɍ����i�V��n�Ȃǁj

	WAZA_TARGET_UNKNOWN,			///< ��т��ӂ�ȂǁA����̃��U�ɉ�����^�C�v

//	WAZA_TARGET_DOUBLE,		///< 


}WazaTarget;


//------------------------------------------------------------------------------
/**
 *	���U�D��x
 */
//------------------------------------------------------------------------------
typedef enum {

	WAZAPRI_MAX = 5,
	WAZAPRI_MIN = -7,

	WAZAPRI_RANGE = (WAZAPRI_MAX - WAZAPRI_MIN + 1),

}WazaPriority;



extern u16  WAZADATA_GetPower( WazaID id );
extern s8   WAZADATA_GetPriority( WazaID id );



// �J�e�S��
extern WazaCategory  WAZADATA_GetCategory( WazaID id );

// �^�C�v
extern PokeType WAZADATA_GetType( WazaID id );

// ������
extern u8  WAZADATA_GetHitRatio( WazaID id );

// ������A���U�����U�̍ő�񐔁B
extern u16  WAZADATA_GetContiniusCount( WazaID id );

// ���イ����ɓ����郉���N
extern u16  WAZADATA_GetCriticalRatio( WazaID id );

// �K���t���O�B�΂߂������Ȃǂ�TRUE��Ԃ��B
extern BOOL WAZADATA_IsAlwaysHit( WazaID id );

// �_���[�W���U���ǂ���
extern BOOL WAZADATA_IsDamage( WazaID id );

// �_���[�W�^�C�v
extern WazaDamageType  WAZADATA_GetDamageType( WazaID id );

// �N���e�B�J�������N
extern u8 WAZADATA_GetCriticalRank( WazaID id );

// �ő�q�b�g��
extern u8 WAZADATA_GetMaxHitCount( WazaID id );

// ���ʔ͈�
extern WazaTarget WAZADATA_GetTarget( WazaID id );

// ��Ԉُ���ʂ��擾
extern PokeSick WAZADATA_GetSick( WazaID id );

// ����B������͒ǉ����ʂ̊m�����Ԃ�B
extern u32 WAZADATA_GetSickPer( WazaID id );


/*-----------------------------------------------------------------


�g�p�ł��邩�H
�K�����s���邩�H
�З͂́H
�_���[�W�́H
�����������H
�񕜂́H
�ǉ����ʂ́H


-----------------------------------------------------------------*/

#ifdef PM_DEBUG
extern void WAZADATA_PrintDebug( void );
#else
#define WAZADATA_PrintDebug()	/* */
#endif


#endif
