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

#include <heapsys.h>

#include "poke_tool/poketype.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
typedef  u16  WazaID;
typedef  struct _WAZA_DATA  WAZA_DATA;

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
  WAZADATA_CATEGORY_FIELD_EFFECT,
  WAZADATA_CATEGORY_SIDE_EFFECT,
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

  WAZASICK_MAHI = POKESICK_MAHI,
  WAZASICK_NEMURI = POKESICK_NEMURI,
  WAZASICK_KOORI = POKESICK_KOORI,
  WAZASICK_YAKEDO = POKESICK_YAKEDO,
  WAZASICK_DOKU = POKESICK_DOKU,

  WAZASICK_DOKUDOKU = POKESICK_MAX,
  WAZASICK_KONRAN,
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
  WAZASICK_FLYING_CANCEL,   ///< �ӂ䂤��ԂɂȂ�Ȃ�
  WAZASICK_TELEKINESIS,     ///< �e���L�l�V�X
  WAZASICK_FREEFALL,        ///< �t���[�t�H�[��
  WAZASICK_HITRATIO_UP,     ///< �������㏸

  WAZASICK_MAX,
  WAZASICK_NULL = POKESICK_NULL,
  WAZASICK_STD_MAX = WAZASICK_MEROMERO+1, ///< �W����Ԉُ�̐��i���������܂Łj

  WAZASICK_SPECIAL_CODE = 0xffff, ///< ���ꏈ���R�[�h

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

  WAZA_RANKEFF_SP = WAZA_RANKEFF_MAX,
  WAZA_RANKEFF_CRITICAL_RATIO,

}WazaRankEffect;

//------------------------------------------------------------------------------
/**
 *  ���U�̌��ʔ͈�
 */
//------------------------------------------------------------------------------
typedef enum {

  // �|�P�����Ώ�
  WAZA_TARGET_OTHER_SELECT,       ///< �ʏ�|�P�i�P�̑I���j
  WAZA_TARGET_FRIEND_USER_SELECT, ///< �������܂ޖ����|�P�i�P�̑I���j
  WAZA_TARGET_FRIEND_SELECT,      ///< �����ȊO�̖����|�P�i�P�̑I���j
  WAZA_TARGET_ENEMY_SELECT,       ///< ���葤�|�P�i�P�̑I���j
  WAZA_TARGET_OTHER_ALL,          ///< �����ȊO�̑S�|�P
  WAZA_TARGET_ENEMY_ALL,          ///< ���葤�S�|�P
  WAZA_TARGET_FRIEND_ALL,         ///< �������S�|�P
  WAZA_TARGET_USER,               ///< �����̂�
  WAZA_TARGET_ALL,                ///< ��ɏo�Ă���S�|�P
  WAZA_TARGET_ENEMY_RANDOM,       ///< ����|�P�P�̃����_��

  // �|�P�����ȊO�Ώ�
  WAZA_TARGET_FIELD,              ///< ��S�́i�V��Ȃǁj
  WAZA_TARGET_SIDE_ENEMY,         ///< �G���w�c
  WAZA_TARGET_SIDE_FRIEND,        ///< �������w�c

  // ����
  WAZA_TARGET_UNKNOWN,            ///< ��т��ӂ�ȂǓ���^

  WAZA_TARGET_MAX,
  WAZA_TARGET_LONG_SELECT = WAZA_TARGET_MAX,  ///<�ΏۑI��\���p�ɒ�`
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
 *  ���U�Ŕ�������V�����
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZA_WEATHER_NONE = 0,   ///< �V��Ȃ�

  WAZA_WEATHER_SHINE,    ///< �͂�
  WAZA_WEATHER_RAIN,     ///< ����
  WAZA_WEATHER_SNOW,     ///< �����
  WAZA_WEATHER_SAND,     ///< ���Ȃ��炵
  WAZA_WEATHER_MIST,     ///< ����

  WAZA_WEATHER_MAX,


}WazaWeather;

//------------------------------------------------------------------------------
/**
 *  �e��t���O
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZAFLAG_Touch = 0,     ///< �ڐG���U
  WAZAFLAG_Tame,          ///< �P�^�[�����߃��U
  WAZAFLAG_Tire,          ///< �P�^�[���������U
  WAZAFLAG_Mamoru,        ///< �u�܂���v�ŃK�[�h�ΏۂƂȂ郏�U
  WAZAFLAG_MagicCoat,     ///< �u�}�W�b�N�R�[�g�v�Ώ�
  WAZAFLAG_Yokodori,      ///< �u�悱�ǂ�v�Ώ�
  WAZAFLAG_Oumu,          ///< �u�����ނ������v�Ώ�
  WAZAFLAG_Punch,         ///< �u�Ă̂��Ԃ��v�Ώہi�p���`�̃C���[�W�j
  WAZAFLAG_Sound,         ///< �u�ڂ�����v�Ώہi���ɂ��U���E���ʂ̃C���[�W�j
  WAZAFLAG_Flying,        ///< �d�͎��Ɏ��s����i��񂾂蕂������C���[�W�j
  WAZAFLAG_KooriMelt,     ///< ����������ł��������ă��U���o���i�����C���[�W�j
  WAZAFLAG_TripleFar,     ///< �g���v���o�g���ŁA���ꂽ������I���o����

  WAZAFLAG_MAX,

}WazaFlag;

//------------------------------------------------------------------------------
/**
 *  ���U�֘A�萔
 */
//------------------------------------------------------------------------------
enum {
  WAZA_RANKEFF_NUM_MAX = 3,   ///< �P�̃��U�Ɋ��蓖�Ă��郉���N���ʂ̎�ސ��ő�l
};


//------------------------------------------------------------------------------
/**
 *  ���U�p�����[�^���
 */
//------------------------------------------------------------------------------
typedef enum {

  WAZAPARAM_TYPE,                ///< �^�C�v�i�݂��A�����A�ق̂��c�j
  WAZAPARAM_CATEGORY,            ///< �J�e�S���i enum WazaCategory )
  WAZAPARAM_DAMAGE_TYPE,         ///< �_���[�W�^�C�v�i enum WazaDamageType )
  WAZAPARAM_POWER,               ///< �З�
  WAZAPARAM_HITPER,              ///< ������
  WAZAPARAM_BASE_PP,             ///< ��{PPMax
  WAZAPARAM_PRIORITY,            ///< ���U�D��x
  WAZAPARAM_CRITICAL_RANK,       ///< �N���e�B�J�������N
  WAZAPARAM_HITCOUNT_MAX,        ///< �ő�q�b�g��
  WAZAPARAM_HITCOUNT_MIN,        ///< �ŏ��q�b�g��
  WAZAPARAM_SHRINK_PER,          ///< �Ђ�݊m��
  WAZAPARAM_SICK,                ///< ��Ԉُ�R�[�h�ienum WazaSick�j
  WAZAPARAM_SICK_PER,            ///< ��Ԉُ�̔�����
  WAZAPARAM_SICK_CONT,           ///< ��Ԉُ�p���p�^�[���ienum WazaSickCont�j
  WAZAPARAM_SICK_TURN_MIN,       ///< ��Ԉُ�̌p���^�[���ŏ�
  WAZAPARAM_SICK_TURN_MAX,       ///< ��Ԉُ�̌p���^�[���ő�
  WAZAPARAM_RANKTYPE_1,          ///< �X�e�[�^�X�����N���ʂP
  WAZAPARAM_RANKVALUE_1,         ///< �X�e�[�^�X�����N���ʂP�����l
  WAZAPARAM_RANKPER_1,           ///< �X�e�[�^�X�����N���ʂP�̔�����
  WAZAPARAM_RANKTYPE_2,          ///< �X�e�[�^�X�����N���ʂQ
  WAZAPARAM_RANKVALUE_2,         ///< �X�e�[�^�X�����N���ʂQ�����l
  WAZAPARAM_RANKPER_2,           ///< �X�e�[�^�X�����N���ʂQ�̔�����
  WAZAPARAM_RANKTYPE_3,          ///< �X�e�[�^�X�����N���ʂR
  WAZAPARAM_RANKVALUE_3,         ///< �X�e�[�^�X�����N���ʂR�����l
  WAZAPARAM_RANKPER_3,           ///< �X�e�[�^�X�����N���ʂR�̔�����
  WAZAPARAM_DAMAGE_RECOVER_RATIO,///< �_���[�W�񕜗�
  WAZAPARAM_HP_RECOVER_RATIO,    ///< HP�񕜗�
  WAZAPARAM_TARGET,              ///< ���U���ʔ͈�( enum WazaTarget )

  WAZAPARAM_WEATHER,             ///< �V��
  WAZAPARAM_DAMAGE_REACTION_RATIO,  ///< �_���[�W������
  WAZAPARAM_HP_REACTION_RATIO,      ///< HP������

  WAZAPARAM_MAX,

}WazaDataParam;



extern int WAZADATA_GetParam( WazaID id, WazaDataParam param );

extern const WAZA_DATA*  WAZADATA_Alloc( WazaID id, HEAPID heapID );
extern int WAZADATA_PTR_GetParam( const WAZA_DATA* wazaData, WazaDataParam param );

extern BOOL WAZADATA_GetFlag( WazaID id, WazaFlag flag );
extern BOOL WAZADATA_PTR_GetFlag( const WAZA_DATA* wazaData, WazaFlag flag );



// MaxPP�l
extern u8 WAZADATA_GetMaxPP( WazaID id, u8 upCnt );

// �З͒l
extern u16  WAZADATA_GetPower( WazaID id );

// �^�C�v
extern PokeType WAZADATA_GetType( WazaID id );

// �_���[�W�^�C�v
extern WazaDamageType  WAZADATA_GetDamageType( WazaID id );


// �J�e�S��
extern WazaCategory  WAZADATA_GetCategory( WazaID id );

// �K���t���O�B�΂߂������Ȃǂ�TRUE��Ԃ��B
extern BOOL WAZADATA_IsAlwaysHit( WazaID id );

// �K���N���e�B�J���t���O
extern BOOL WAZADATA_IsMustCritical( WazaID id );

// �_���[�W���U���ǂ���
extern BOOL WAZADATA_IsDamage( WazaID id );

// �N���e�B�J�������N
extern u8 WAZADATA_GetCriticalRank( WazaID id );

// �V�����
extern WazaWeather WAZADATA_GetWeather( WazaID id );

// ��Ԉُ����
extern WazaSick WAZADATA_GetSick( WazaID id );



//=============================================================================================
/**
 * ��Ԉُ�p���p�����[�^�擾
 *
 * @param   id
 *
 * @retval  extern WAZA_SICKCONT_PARAM
 */
//=============================================================================================
extern WAZA_SICKCONT_PARAM WAZADATA_GetSickCont( WazaID id );

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



#endif
