//=============================================================================================
/**
 * @file  wazadata.h
 * @brief �|�P����WB  ���U�f�[�^�A�N�Z�T
 * @author  taya
 *
 * @date  2008.10.14  �쐬
 */
//=============================================================================================
#ifndef __WAZADATA_H__
#define __WAZADATA_H__

#include "poke_tool/poketype.h"
#include "battle/battle.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
typedef  u16  WazaID;


//------------------------------------------------------------------------------
/**
 *  ���U�̃J�e�S���ŏ�ʕ���
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZADATA_CATEGORY_SIMPLE_DAMAGE,
  WAZADATA_CATEGORY_SIMPLE_SICK,
  WAZADATA_CATEGORY_SIMPLE_EFFECT,
  WAZADATA_CATEGORY_SIMPLE_RECOVER,
  WAZADATA_CATEGORY_DAMAGE_SICK,
  WAZADATA_CATEGORY_EFFECT_SICK,
  WAZADATA_CATEGORY_DAMAGE_EFFECT,
  WAZADATA_CATEGORY_DAMAGE_EFFECT_USER,
  WAZADATA_CATEGORY_DRAIN,
  WAZADATA_CATEGORY_ICHIGEKI,
  WAZADATA_CATEGORY_GUARD,
  WAZADATA_CATEGORY_WEATHER,
  WAZADATA_CATEGORY_FIELD_EFFECT,
  WAZADATA_CATEGORY_SIDE_EFFECT,
  WAZADATA_CATEGORY_EXPLOSION,
  WAZADATA_CATEGORY_PUSHOUT,
  WAZADATA_CATEGORY_OTHERS,

  WAZADATA_CATEGORY_MAX,

}WazaCategory;

//------------------------------------------------------------------------------
/**
 *  ���U�̃_���[�W���
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZADATA_DMG_NONE = 0,
  WAZADATA_DMG_PHYSIC,  ///< �����_���[�W
  WAZADATA_DMG_SPECIAL, ///< ����_���[�W

}WazaDamageType;

//------------------------------------------------------------------------------
/**
 *  ���U�ɂ���Ĉ����N��������Ԉُ�B
 *  PokeSick���o�g���p�Ɋg���������́B
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZASICK_ORIGIN = POKESICK_ORIGIN,

  WAZASICK_DOKU = POKESICK_DOKU,
  WAZASICK_MAHI = POKESICK_MAHI,
  WAZASICK_NEMURI = POKESICK_NEMURI,
  WAZASICK_KOORI = POKESICK_KOORI,
  WAZASICK_YAKEDO = POKESICK_YAKEDO,

  WAZASICK_KONRAN = POKESICK_MAX,
  WAZASICK_MEROMERO,
  WAZASICK_BIND,
  WAZASICK_AKUMU,
  WAZASICK_NOROI,
  WAZASICK_TYOUHATSU,
  WAZASICK_ICHAMON,
  WAZASICK_KANASIBARI,
  WAZASICK_AKUBI,
  WAZASICK_KAIHUKUHUUJI,
  WAZASICK_IEKI,            ///< �Ƃ�����������
  WAZASICK_MIYABURU,        ///< �݂�Ԃ��đ����[���ł����{�ŉ����違��𗦏㏸������
  WAZASICK_YADORIGI,        ///< ��ǂ肬�̂���
  WAZASICK_SASIOSAE,        ///< ����������
  WAZASICK_HOROBINOUTA,     ///< �ق�т̂���
  WAZASICK_NEWOHARU,        ///< �˂��͂�
  WAZASICK_TOOSENBOU,       ///< �ɂ���E���ꂩ�����o���Ȃ�
  WAZASICK_ENCORE,          ///< ���O�Ɏg�������U�����o���Ȃ�
  WAZASICK_WAZALOCK,        ///< ���O�Ɏg�������U�����o���Ȃ���A�s�����I���ł��Ȃ�
  WAZASICK_MUSTHIT,         ///< ���U�K�����
  WAZASICK_MUSTHIT_TARGET,  ///< ���葊��ɑ΂��Ă̂ݕK�����
  WAZASICK_FLYING,          ///< �ł񂶂ӂ䂤
  WAZASICK_POWERTRICK,      ///< �p���[�g���b�N

  WAZASICK_MAX,
  WAZASICK_NULL = POKESICK_NULL,

}WazaSick;

//------------------------------------------------------------------------------
/**
 *  ��Ԉُ�̌p���^�C�v����
 */
//------------------------------------------------------------------------------
typedef enum {
  WAZASICK_CONT_NONE = 0,

  WAZASICK_CONT_PERMANENT,  ///< �����ƌp��
  WAZASICK_CONT_TURN,       ///< �^�[�����p��
  WAZASICK_CONT_POKE,       ///< ���U���������|�P����������ԁA�p��
  WAZASICK_CONT_POKETURN,   ///< �^�[�����o�߂����|�P�������ꂩ�������܂ł𗼕��`�F�b�N

}WazaSickCont;

//------------------------------------------------------
/**
 *  ��Ԉُ�̌p���p�����[�^
 */
//------------------------------------------------------
typedef struct {

  union {
    u16 raw;
    struct {
      u16  type    : 4;   ///< �p���^�C�v�i WazaSickCont �j
      u16  turnMin : 6;   ///< �p���^�[���ŏ�
      u16  turnMax : 6;   ///< �p���^�[���ő�
    };
  };

}WAZA_SICKCONT_PARAM;


//------------------------------------------------------------------------------
/**
 *  ���U�ɂ���Ĕ�������ǉ����ʁi�U�����̃����N�A�b�v�E�_�E�����ʁj
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
  WAZA_RANKEFF_NUMS = WAZA_RANKEFF_MAX - WAZA_RANKEFF_ORIGIN,

  WAZA_RANKEFF_CRITICAL_RATIO = WAZA_RANKEFF_AVOID+1,

}WazaRankEffect;

//------------------------------------------------------------------------------
/**
 *  ���U�̌��ʔ͈�
 */
//------------------------------------------------------------------------------
typedef enum {

  // �|�P�����Ώ�
  WAZA_TARGET_OTHER_SELECT,       ///< �����ȊO�P�́i�I���j
  WAZA_TARGET_OTHER_ALL,          ///< �����ȊO�S��
  WAZA_TARGET_ENEMY_SELECT,       ///< ���葤�P�́i�I���j
  WAZA_TARGET_ENEMY_ALL,          ///< ���葤�S��
  WAZA_TARGET_ENEMY_RANDOM,       ///< ���葤�P�̃����_��
  WAZA_TARGET_FRIEND_USER_SELECT, ///< �������܂ޖ����P�́i�I���j
  WAZA_TARGET_FRIEND_SELECT,      ///< �����ȊO�̖����P�́i�I���j
  WAZA_TARGET_USER,               ///< �����̂�
  WAZA_TARGET_ALL,                ///< ��ɏo�Ă���|�P�����S��
  // �|�P�����ȊO�Ώ�
  WAZA_TARGET_SIDE_FRIEND,        ///< �������w�c
  WAZA_TARGET_SIDE_ENEMY,         ///< �G���w�c
  WAZA_TARGET_FIELD,              ///< ��S�́i�V��Ȃǁj
  WAZA_TARGET_UNKNOWN,            ///< ��т��ӂ�ȂǓ���^

}WazaTarget;

//------------------------------------------------------------------------------
/**
 *  ���U�D��x
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZAPRI_MAX = 5,
  WAZAPRI_MIN = -7,

  WAZAPRI_RANGE = (WAZAPRI_MAX - WAZAPRI_MIN + 1),

}WazaPriority;

//------------------------------------------------------------------------------
/**
 *  ���U�C���[�W����
 */
//------------------------------------------------------------------------------
typedef enum {
  WAZA_IMG_NULL = 0,    ///< �C���[�W�Ȃ�

  WAZA_IMG_PUNCH,       ///< �p���`�i�Ă̂��Ԃ��Ώہj
  WAZA_IMG_SOUND,       ///< �T�E���h�i�ڂ�����Ώہj
  WAZA_IMG_HEAT,        ///< ���M�i����������j
  WAZA_IMG_FLY,         ///< ���ɔ�ԁi���イ��傭���A�g�p�s�j

  WAZA_IMG_MAX,
}WazaImage;

//------------------------------------------------------------------------------
/**
 *  �e��t���O
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZAFLAG_MAMORU = 0,    ///< �u�܂���v�ŃK�[�h�ΏۂƂȂ郏�U
  WAZAFLAG_TAME,          ///< �P�^�[�����߃��U

  WAZAFLAG_MAX,

}WazaFlag;

//------------------------------------------------------------------------------
/**
 *  ���U�֘A�萔
 */
//------------------------------------------------------------------------------
enum {
  WAZA_RANKEFF_NUM_MAX = 2,   ///< �P�̃��U�Ɋ��蓖�Ă��郉���N���ʂ̎�ސ��ő�l

  WAZANO_NULL = 0,  // @@@ �z���g�� wazano_def.h �ɓ��ꂽ��
};

extern u16  WAZADATA_GetPower( WazaID id );
extern s8   WAZADATA_GetPriority( WazaID id );


// MaxPP�l
extern u8 WAZADATA_GetMaxPP( WazaID id, u8 upCnt );

// �J�e�S��
extern WazaCategory  WAZADATA_GetCategory( WazaID id );

// �^�C�v
extern PokeType WAZADATA_GetType( WazaID id );

// ������
extern u8  WAZADATA_GetHitRatio( WazaID id );

// ������A���U�����U�̍ő�񐔁B
extern u16  WAZADATA_GetContiniusCount( WazaID id );

// �K���t���O�B�΂߂������Ȃǂ�TRUE��Ԃ��B
extern BOOL WAZADATA_IsAlwaysHit( WazaID id );

// �_���[�W���U���ǂ���
extern BOOL WAZADATA_IsDamage( WazaID id );

// �ڐG���U���ǂ���
extern BOOL WAZADATA_IsTouch( WazaID id );

// �_���[�W�^�C�v
extern WazaDamageType  WAZADATA_GetDamageType( WazaID id );

// �N���e�B�J�������N
extern u8 WAZADATA_GetCriticalRank( WazaID id );

// �ő�q�b�g��
extern u8 WAZADATA_GetMaxHitCount( WazaID id );

// ���ʔ͈�
extern WazaTarget WAZADATA_GetTarget( WazaID id );

// �ǉ����ʂłЂ�ފm��
extern u32 WAZADATA_GetShrinkPer( WazaID id );

// �V�����
extern BtlWeather WAZADATA_GetWeather( WazaID id );

// ��Ԉُ����
extern WazaSick WAZADATA_GetSick( WazaID id );

// ��Ԉُ�p���p�����[�^
extern WAZA_SICKCONT_PARAM WAZADATA_GetSickCont( WazaID id );

// �ǉ����ʂŏ�Ԉُ�ɂȂ�m��
extern u32 WAZADATA_GetSickPer( WazaID id );

// ���čs���s�ɂȂ郏�U���H
extern BOOL WAZADATA_IsTire( WazaID id );

// ���U�t���O�擾
extern BOOL WAZADATA_GetFlag( WazaID id, WazaFlag flag );

//=============================================================================================
/**
 * �����N���ʂ̎�ސ����擾
 *
 * @param   id        [in] ���UID
 * @param   volume    [out] �����N���ʂ̒��x�i+�Ȃ�A�b�v, -�Ȃ�_�E��, �߂�l==WAZA_RANKEFF_NULL�̎��̂�0�j
 *
 * @retval  WazaRankEffect    �����N����ID
 */
//=============================================================================================
extern u8 WAZADATA_GetRankEffectCount( WazaID id );

//=============================================================================================
/**
 * �����N���ʂ��擾
 *
 * @param   id        [in] ���UID
 * @param   idx       [in] ���Ԗڂ̃����N���ʂ��H�i0�`�j
 * @param   volume    [out] �����N���ʂ̒��x�i+�Ȃ�A�b�v, -�Ȃ�_�E��, �߂�l==WAZA_RANKEFF_NULL�̎��̂�0�j
 *
 * @retval  WazaRankEffect    �����N����ID
 */
//=============================================================================================
extern WazaRankEffect  WAZADATA_GetRankEffect( WazaID id, u32 idx, int* volume );

// �����N���ʂ̔����m���i0-100�j
extern u32 WAZADATA_GetRankEffPer( WazaID id );

//=============================================================================================
/**
 * ���U�C���[�W�`�F�b�N
 *
 * @param   id    ���UID
 * @param   img   �C���[�WID
 *
 * @retval  BOOL    ���U���w�肳�ꂽ�C���[�W�ɑ�����ꍇ��TRUE
 */
//=============================================================================================
extern BOOL WAZADATA_IsImage( WazaID id, WazaImage img );

//=============================================================================================
/**
 * �������i����ɗ^�����_���[�W�̉����𔽓��Ƃ��Ď������󂯂邩�j��Ԃ�
 *
 * @param   id
 *
 * @retval  u8    �������i�����Ȃ��Ȃ�0, max100�j
 */
//=============================================================================================
extern u8 WAZADATA_GetReactionRatio( WazaID id );

//=============================================================================================
/**
 * �h���C���Z�̃_���[�W�l->��HP�Ҍ���
 *
 * @param   id
 *
 * @retval  u8
 */
//=============================================================================================
extern u8 WAZADATA_GetDrainRatio( WazaID id );

extern u8 WAZADATA_GetRecoverHPRatio( WazaID id );


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
#define WAZADATA_PrintDebug() /* */
#endif


#endif
