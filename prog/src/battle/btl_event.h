//=============================================================================================
/**
 * @file    btl_event.h
 * @brief   �|�P����WB �o�g���V�X�e�� �C�x���g���n���h������
 * @author  taya
 *
 * @date  2008.10.23  �쐬
 */
//=============================================================================================
#ifndef __BTL_EVENT_H__
#define __BTL_EVENT_H__

#include "waza_tool/wazadata.h"

#include "btl_server.h"
#include "btl_server_flow.h"






//--------------------------------------------------------------
/**
* �C�x���g�^�C�v
*/
//--------------------------------------------------------------
typedef enum {
  BTL_EVENT_NULL = 0,

  BTL_EVENT_CHECK_CHANGE,           ///< ���ꂩ���`�F�b�N
  BTL_EVENT_CHECK_ESCAPE,           ///< �ɂ���`�F�b�N
  BTL_EVENT_CHECK_SP_PRIORITY,      ///< �Ƃ�����D��x�`�F�b�N
  BTL_EVENT_CALC_AGILITY,           ///< ���΂₳�v�Z
  BTL_EVENT_CHECK_CONF,             ///< �����`�F�b�N
  BTL_EVENT_CALC_CONF_DAMAGE,       ///< �����_���[�W�v�Z
  BTL_EVENT_WAZA_EXECUTE_CHECK,     ///< ���U���s�`�F�b�N
  BTL_EVENT_WAZA_EXECUTE_FAIL,      ///< ���U�o�����s
  BTL_EVENT_WAZA_EXECUTE_FIX,       ///< ���U�o�������m��
  BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, ///< ���U�o���������������ʂȂ�
  BTL_EVENT_WAZA_PARAM,             ///< ���U�p�����[�^�`�F�b�N
  BTL_EVENT_DECIDE_TARGET,          ///< ���U�Ώی���
  BTL_EVENT_NOEFFECT_TYPE_CHECK,    ///< ���U�������i�^�C�v�ɂ��j�`�F�b�N�n���h��
  BTL_EVENT_NOEFFECT_CHECK_L1,      ///< ���U�������`�F�b�N�ilv1 = �K���ɂ͕�����j�n���h��
  BTL_EVENT_NOEFFECT_CHECK_L2,      ///< ���U�������`�F�b�N�ilv2 = �K���ɂ����j�n���h��
  BTL_EVENT_CHECK_MAMORU_BREAK,     ///< �܂��閳�����`�F�b�N
  BTL_EVENT_WAZA_AVOID,             ///< ���U�O�ꂽ
  BTL_EVENT_DMG_TO_RECOVER_CHECK,   ///< ���U�_���[�W->�񕜉��`�F�b�N�n���h��
  BTL_EVENT_DMG_TO_RECOVER_FIX,     ///< ���U�_���[�W->�񕜉�����n���h��
  BTL_EVENT_EXCUSE_CALC_HIT,        ///< �������v�Z���΂����`�F�b�N
  BTL_EVENT_WAZA_HIT_RANK,          ///< �������E��𗦂̌���n���h��
  BTL_EVENT_WAZA_HIT_RATIO,         ///< ���U�̖������␳�n���h��
  BTL_EVENT_WAZA_HIT_COUNT,         ///< �q�b�g�񐔌���i������q�b�g���U�̂݁j
  BTL_EVENT_CRITICAL_CHECK,         ///< �N���e�B�J���`�F�b�N
  BTL_EVENT_WAZA_POWER,             ///< ���U�З͕␳�n���h��
  BTL_EVENT_ATTACKER_POWER_PREV,    ///< �U�����̔\�͒l�i��������or�Ƃ������j�擾�O�n���h��
  BTL_EVENT_DEFENDER_GUARD_PREV,    ///< �h�䑤�̔\�͒l�i�ڂ�����or�Ƃ��ڂ��j�擾�O�n���h��
  BTL_EVENT_ATTACKER_POWER,         ///< �U�����̔\�͒l�i��������or�Ƃ������j�␳�n���h��
  BTL_EVENT_DEFENDER_GUARD,         ///< �h�䑤�̔\�͒l�i�ڂ�����or�Ƃ��ڂ��j�␳�n���h��
  BTL_EVENT_CHECK_AFFINITY,         ///< �����v�Z
  BTL_EVENT_ATTACKER_TYPE,          ///< �U�����|�P�^�C�v
  BTL_EVENT_DEFENDER_TYPE,          ///< �h�䑤�|�P�^�C�v
  BTL_EVENT_TYPEMATCH_RATIO,        ///< �U�����^�C�v��v���{��
  BTL_EVENT_WAZA_ATK_TYPE,          ///< �U�����|�P�^�C�v�`�F�b�N�n���h��
  BTL_EVENT_WAZA_DEF_TYPE,          ///< �h�䑤�|�P�^�C�v�`�F�b�N�n���h��
  BTL_EVENT_WAZA_DMG_DETERMINE,     ///< �_���[�W���U�������邱�Ɗm�肵���i�v�Z�O�j
  BTL_EVENT_WAZA_DMG_PROC1,         ///< �^�C�v�v�Z�O�_���[�W�␳�n���h��
  BTL_EVENT_WAZA_DMG_PROC2,         ///< �^�C�v�v�Z��_���[�W�␳�n���h��
  BTL_EVENT_WAZA_DMG_LAST,          ///< �ŏI�_���[�W�␳�n���h��
  BTL_EVENT_KORAERU_CHECK,          ///< �u���炦��v�`�F�b�N
  BTL_EVENT_WAZA_DMG_AFTER,         ///< �P�̃_���[�W������
  BTL_EVENT_WAZA_DMG_AFTER_ATONCE,  ///< ��ă_���[�W������
  BTL_EVENT_DECREMENT_PP_VOLUME,    ///< �g�p���U�̌���PP�l���擾
  BTL_EVENT_DECREMENT_PP_DONE,      ///< �g�p���U��PP�l��������
  BTL_EVENT_CALC_KICKBACK,          ///< �����v�Z�n���h��
  BTL_EVENT_ADD_RANK_TARGET,        ///< ���U���󂯂����ւ̒ǉ�����
  BTL_EVENT_ADD_RANK_USER,          ///< ���U���g�������ւ̒ǉ�����
  BTL_EVENT_MEMBER_OUT,             ///< �ʃ|�P�ޏ꒼�O
  BTL_EVENT_MEMBER_IN,              ///< �ʃ|�P���꒼��
  BTL_EVENT_MEMBER_COMP,            ///< �S�Q���|�P�o���
  BTL_EVENT_GET_RANKEFF_VALUE,      ///< ���U�ɂ��\�̓����N�����l�`�F�b�N
  BTL_EVENT_RANKEFF_LAST_CHECK,     ///< �\�̓����N�����ŏI�`�F�b�N
  BTL_EVENT_RANKEFF_FAILED,         ///< �\�̓����N�������s�m��
  BTL_EVENT_RANKEFF_FIXED,          ///< �\�̓����N����������
  BTL_EVENT_ADD_SICK,               ///< ���U�̒ǉ����ʂɂ���Ԉُ�̔����`�F�b�N
  BTL_EVENT_ADDSICK_CHECKFAIL,      ///< ��Ԉُ�̎��s�`�F�b�N
  BTL_EVENT_ADDSICK_FIX,            ///< ��Ԉُ�m��
  BTL_EVENT_ADDSICK_FAILED,         ///< ��Ԉُ편�s
  BTL_EVENT_MAKE_POKESICK,          ///< �|�P�����n�̏�Ԉُ�ɂ���
  BTL_EVENT_MAKE_WAZASICK,          ///< ���U�n�̏�Ԉُ�ɂ���
  BTL_EVENT_SICK_DAMAGE,            ///< ��Ԉُ�ɂ��_���[�W�v�Z
  BTL_EVENT_SHRINK_CHECK,           ///< �Ђ�݌v�Z
  BTL_EVENT_SHRINK_FIX,             ///< �Ђ�݊m���
  BTL_EVENT_ICHIGEKI_CHECK,         ///< �ꌂ�K�E�`�F�b�N
  BTL_EVENT_NOT_WAZA_DAMAGE,        ///< ���U�ȊO�̃_���[�W�`�F�b�N
  BTL_EVENT_USE_ITEM_ENABLE,        ///< �A�C�e���g�p�ۃ`�F�b�N
  BTL_EVENT_USE_ITEM,               ///< �A�C�e���g�p
  BTL_EVENT_TURNCHECK_BEGIN,        ///< �^�[���`�F�b�N�i�擪�j
  BTL_EVENT_TURNCHECK_END,          ///< �^�[���`�F�b�N�i�I�[�j
  BTL_EVENT_WEATHER_CHANGE,         ///< �V��̕ω��i���O�j
  BTL_EVENT_WEATHER_CHANGE_AFTER,   ///< �V��̕ω��i����j
  BTL_EVENT_CALC_WEATHER_DAMAGE,    ///< �V��_���[�W�v�Z
  BTL_EVENT_SIMPLE_DAMAGE_ENABLE,   ///< ���U�ȊO�_���[�W�L������
  BTL_EVENT_SIMPLE_DAMAGE_REACTION, ///< ���U�ȊO�_���[�W��̔����`�F�b�N
  BTL_EVENT_AFTER_DAMAGE,           ///< �_���[�W���U��������
  BTL_EVENT_SKILL_SWAP,             ///< �Ƃ���������ւ�
  BTL_EVENT_CHECK_PUSHOUT,          ///< �ӂ��Ƃ΂��n���U�`�F�b�N
  BTL_EVENT_CALC_DRAIN,             ///< �h���C���n���U�񕜗ʌv�Z
  BTL_EVENT_CALC_SPECIAL_DRAIN,     ///< �h���C���n���U�񕜗ʌv�Z
  BTL_EVENT_RECOVER_HP_RATIO,       ///< HP�񕜃��U�̉񕜗��v�Z
  BTL_EVENT_CHECK_TAMETURN_SKIP,    ///< ���߃^�[���X�L�b�v����
  BTL_EVENT_TAME_START,             ///< ���ߊJ�n
  BTL_EVENT_TAME_RELEASE,           ///< ���߉��
  BTL_EVENT_CHECK_POKE_HIDE,        ///< �|�P�����������Ă����Ԃ̃��U�����`�F�b�N
  BTL_EVENT_SET_ITEM_BEFORE,        ///< �A�C�e�����������O�̐��ۃ`�F�b�N
  BTL_EVENT_SET_ITEM_AFTER,         ///< �A�C�e������������̃��A�N�V����
  BTL_EVENT_UNCATEGORIZE_WAZA,            ///< �����ރ��U����
  BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  ///< �����ރ��U�����i�^�[�Q�b�g���Ȃ����U�j


}BtlEventType;













//--------------------------------------------------------------
/**
* �C�x���g�ϐ����x��
*/
//--------------------------------------------------------------
typedef enum {
  BTL_EVAR_NULL = 0,
  BTL_EVAR_SYS_SEPARATE,
  //---���������̓V�X�e���\��̈�-------------------------
  BTL_EVAR_POKEID,
  BTL_EVAR_POKEID_ATK,
  BTL_EVAR_POKEID_DEF,
  BTL_EVAR_TARGET_POKECNT,
  BTL_EVAR_POKEID_TARGET1,
  BTL_EVAR_POKEID_TARGET2,
  BTL_EVAR_POKEID_TARGET3,
  BTL_EVAR_POKEID_TARGET4,
  BTL_EVAR_POKEID_TARGET5,
  BTL_EVAR_POKEID_TARGET6,
  BTL_EVAR_SP_PRIORITY_A,
  BTL_EVAR_SP_PRIORITY_B,
  BTL_EVAR_WAZAID,
  BTL_EVAR_WAZA_TYPE,
  BTL_EVAR_WAZA_IDX,
  BTL_EVAR_DAMAGE_TYPE,
  BTL_EVAR_USER_TYPE,
  BTL_EVAR_SICKID,
  BTL_EVAR_SICK_CONT,
  BTL_EVAR_STATUS_TYPE,
  BTL_EVAR_POKE_TYPE,
  BTL_EVAR_NOEFFECT_FLAG,
  BTL_EVAR_VOLUME,
  BTL_EVAR_GEN_FLAG,
  BTL_EVAR_FAIL_FLAG,
  BTL_EVAR_AVOID_FLAG,
  BTL_EVAR_POKE_HIDE,
  BTL_EVAR_FAIL_CAUSE,
  BTL_EVAR_CONF_DMG,
  BTL_EVAR_TURN_COUNT,
  BTL_EVAR_ALMOST_FLAG,
  BTL_EVAR_ADD_PER,
  BTL_EVAR_HIT_RANK,
  BTL_EVAR_AVOID_RANK,
  BTL_EVAR_HITCOUNT_MAX,
  BTL_EVAR_HITCOUNT,
  BTL_EVAR_HIT_PER,
  BTL_EVAR_CRITICAL_RANK,
  BTL_EVAR_CRITICAL_FLAG,
  BTL_EVAR_ITEM,
  BTL_EVAR_ITEMUSE_FLAG,
  BTL_EVAR_AGILITY,
  BTL_EVAR_WAZA_POWER,
  BTL_EVAR_WAZA_POWER_RATIO,
  BTL_EVAR_DAMAGE,
  BTL_EVAR_POWER,
  BTL_EVAR_GUARD,
  BTL_EVAR_RATIO,
  BTL_EVAR_RATIO_EX,
  BTL_EVAR_TYPEAFF,
  BTL_EVAR_WEATHER,
  BTL_EVAR_KORAERU_CAUSE,
  BTL_EVAR_WORK_ADRS,

  BTL_EVAR_MAX,

}BtlEvVarLabel;


//--------------------------------------------------------------
/**
* �C�x���g�ϐ��X�^�b�N
*/
//--------------------------------------------------------------
extern void BTL_EVENTVAR_Push( void );
extern void BTL_EVENTVAR_Pop( void );
extern void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax );
extern BOOL BTL_EVENTVAR_RewriteValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value );
extern int  BTL_EVENTVAR_GetValue( BtlEvVarLabel label );



//------------------------------------------------------------------------------------------
/**
* �C�x���g�n���h���p���[�N�^���萔
*/
//------------------------------------------------------------------------------------------

/**
 *  ���U�o�����s�n���h���p���[�N
 */
typedef struct {

  /** �e����ʂ̗L��bit�t���O */
  union {
    u8 bitFlags;
    struct {
      u8    flag_rankEffect : 1;  ///< �����N��������
      u8    flag_padding    : 7;
    };
  };

  WazaRankEffect  rankType;   ///< �����N���ʎ��
  s16             rankVolume; ///< �����N���ʑ����l

  u8              tokWinFlag; ///< �Ƃ������E�B���h�E�o���t���O

}BTL_EVWK_WAZAEXE_FAIL;

/**
 *  ���U�_���[�W->�񕜉��`�F�b�N�n���h���p���[�N
 */
typedef struct {
  u8  pokeID;                 ///< �Ώۃ|�P����ID
  u8  tokFlag;                ///< �Ƃ������E�C���h�E�\��
  u16 recoverHP;              ///< �񕜗�

  WazaRankEffect  rankEffect; ///< �񕜂ɉ��������N���ʂ���������ꍇ�ɐݒ�
  s16             rankVolume; ///< �����N���ʑ����l

}BTL_EVWK_DMG_TO_RECOVER;


/**
 *  ���U�_���[�W�󂯂���̃n���h�������^�C�v
 */
typedef enum {
  BTL_EV_AFTER_DAMAGED_REACTION_NONE,

  BTL_EV_AFTER_DAMAGED_REACTION_SICK,       ///< �Ώۃ|�P��������Ԉُ�ɂ���
  BTL_EV_AFTER_DAMAGED_REACTION_RANK,       ///< �Ώۃ|�P�����̃����N��������
  BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE,     ///< �Ώۃ|�P�����Ƀ_���[�W��^����
  BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE,   ///< �Ώۃ|�P�����̃^�C�v�ύX
  BTL_EV_AFTER_DAMAGED_REACTION_MAX,

}BtlEvAfterDamagedReactionType;

/**
 *  ���U�_���[�W�󂯂���̃n���h�����[�N
 */
typedef struct {

  BtlEvAfterDamagedReactionType   reaction; ///< �����^�C�v

  u8  damagedPokeID;    ///< �_���[�W���󂯂��|�P����ID
  u8  targetPokeID;     ///< ���ʑΏۃ|�P����ID
  u8  tokFlag;          ///< �Ƃ������E�C���h�E�\��
  u8  itemUseFlag;      ///< �A�C�e���g�p�t���O

  WazaSick        sick;       ///< �Ώۃ|�P�����ɗ^�����Ԉُ����
  BPP_SICK_CONT   sickCont;   ///< �Ώۃ|�P�����ɗ^�����Ԉُ�̌p���p�^�[��
  PokeType        pokeType;   ///< �Ώۃ|�P������ω�������^�C�v
  WazaRankEffect  rankType;   ///< �Ώۃ|�P�����ɗ^���郉���N���ʃ^�C�v
  int             rankVolume; ///< �Ώۃ|�P�����ɗ^���郉���N���ʒl
  u16             damage;     ///< �Ώۃ|�P�����ɗ^����_���[�W��

}BTL_EVWK_DAMAGE_REACTION;


/**
 *  ��Ԉُ�`�F�b�N�n���h���̔����^�C�v
 */
typedef enum {

  BTL_EV_SICK_REACTION_NONE = 0,  ///< �����Ȃ�
  BTL_EV_SICK_REACTION_DISCARD,   ///< ���ʂ�ł�����
  BTL_EV_SICK_REACTION_REFRECT,   ///< ����ɂ�����

}BtlEvSickReaction;

/**
 *  ��Ԉُ�`�F�b�N�n���h���p���[�N
 */
typedef struct {

  BtlEvSickReaction   reaction;           ///< �����^�C�v
  WazaSick            discardSickType;    ///< ����̏�Ԉُ�̂ݑł������ꍇ�Ɏw��
  u8                  fDiscardByTokusei;  ///< �Ƃ������ɂ��ł������Ȃ�TRUE�ɂ���
  u8                  discardPokeID;      ///< �ł��������|�P����ID
  u8                  fItemResponce;      ///< �A�C�e���g�p����ꍇTRUE�ɂ���

}BTL_EVWK_ADDSICK;

/**
 *  �����N���ʃ`�F�b�N�n���h���p���[�N
 */
typedef struct {

  u8              failFlag;           ///< �Ƃɂ������s
  u8              failTokuseiFlag;    ///< �Ώۃ|�P�����̂Ƃ������ɂ�鎸�s
  u8              useItemFlag;        ///< �A�C�e�������t���O
  WazaRankEffect  failSpecificType;   ///< ����̌��ʂ̂ݎ��s������Ȃ�w��

}BTL_EVWK_CHECK_RANKEFF;

/**
 *  �����`�F�b�N�n���h���p���[�N
 */
typedef struct {

  BtlTypeAff      aff;              ///< ����ID
  u8              weakedByItem;     ///< �A�C�e���ɂ���đ�������߂��Ă���
  u8              weakReserveByItem;///< �A�C�e���ɂ���ă_���[�W�l�𔼌�������
  u16             weakedItemID;     ///< ��߂邽�߂Ɏg��ꂽ�A�C�e��

}BTL_EVWK_CHECK_AFFINITY;


//--------------------------------------------------------------
/**
* �C�x���g�t�@�N�^�[�^�C�v
*/
//--------------------------------------------------------------
typedef enum {

  BTL_EVENT_FACTOR_WAZA,
  BTL_EVENT_FACTOR_POS,
  BTL_EVENT_FACTOR_SIDE,
  BTL_EVENT_FACTOR_FIELD,
  BTL_EVENT_FACTOR_TOKUSEI,
  BTL_EVENT_FACTOR_ITEM,

}BtlEventFactor;


extern void BTL_EVENT_InitSystem( void );
extern void BTL_EVENT_StartTurn( void );
extern void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* server, BtlEventType type );


#endif
