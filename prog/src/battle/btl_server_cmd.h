//=============================================================================================
/**
 * @file  btl_server_cmd.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h�����C���ߏ���
 * @author  taya
 *
 * @date  2008.10.30  �쐬
 */
//=============================================================================================
#ifndef __BTL_SERVER_CMD_H__
#define __BTL_SERVER_CMD_H__

#include "btl_string.h"

enum {
  BTL_SERVER_CMD_QUE_SIZE = 800,
  BTL_SERVERCMD_ARG_MAX = 16,
};




//--------------------------------------------------------------
/**
 *    �T�[�o�R�}���h
 */
//--------------------------------------------------------------
typedef enum {
  SC_NULL = 0,

  SC_OP_HP_MINUS,           ///< HP�}�C�i�X  [ClientID, �}�C�i�X��]
  SC_OP_HP_PLUS,            ///< HP�v���X    [ClientID, �v���X��]
  SC_OP_HP_ZERO,            ///< HP0 �ɂ��� [ pokeID ]
  SC_OP_PP_MINUS,           ///< PP�}�C�i�X  [ClientID, �}�C�i�X��]
  SC_OP_PP_PLUS,            ///< PP�v���X    [ClientID, �v���X��]
  SC_OP_RANK_UP,            ///< �X�e�[�^�X�����N�A�b�v  [ClientID, StatusType, �v���X��]
  SC_OP_RANK_DOWN,          ///< �X�e�[�^�X�����N�_�E��  [ClientID, StatusType, �}�C�i�X��]
  SC_OP_RANK_SET7,          ///< �X�e�[�^�X�����N�V��Z�b�g[ pokeID, atk, def, sp_atk, sp_def, agi ]
  SC_OP_RANK_RECOVER,       ///< �X�e�[�^�X�����N�i�V��j�������Ă�����̂̂݃t���b�g��
  SC_OP_RANK_RESET,         ///< �X�e�[�^�X�����N�i�V��j�S�Ă��t���b�g��
  SC_OP_ADD_CRITICAL,       ///< �N���e�B�J�������N���Z[ pokeID, (int)value ]
  SC_OP_SICK_SET,           ///< ��Ԉُ� [PokeID, Sick, contParam]
  SC_OP_CURE_POKESICK,      ///< �|�P�����n��Ԉُ���� [PokeID ]
  SC_OP_CURE_WAZASICK,      ///< ���U�n��Ԉُ���� [PokeID, SickID ]
  SC_OP_MEMBER_IN,          ///< �����o�[����
  SC_OP_SET_STATUS,         ///< �\�͒l�i�U���A�h�䓙�j����������
  SC_OP_SET_WEIGHT,         ///< �̏d�ݒ�
  SC_OP_WAZASICK_TURNCHECK, ///<
  SC_OP_CHANGE_POKETYPE,    ///< �y�v�Z�z�|�P�����̃^�C�v�ύX�i pokeID, type �j
  SC_OP_CONSUME_ITEM,       ///< ���L�A�C�e���폜
  SC_OP_UPDATE_USE_WAZA,    ///< ���U�v���Z�X�I�����
  SC_OP_SET_CONTFLAG,       ///< �i���t���O�Z�b�g
  SC_OP_RESET_CONTFLAG,     ///< �i���t���O���Z�b�g
  SC_OP_SET_TURNFLAG,       ///< �^�[���t���O�Z�b�g
  SC_OP_RESET_TURNFLAG,     ///< �^�[���t���O�������Z�b�g
  SC_OP_CHANGE_TOKUSEI,     ///< �Ƃ�������������
  SC_OP_SET_ITEM,           ///< �A�C�e����������
  SC_OP_UPDATE_WAZANUMBER,  ///< ���U��������
  SC_OP_HENSIN,             ///< �ւ񂵂�
  SC_OP_OUTCLEAR,           ///< �ޏꎞ�N���A
  SC_OP_ADD_FLDEFF,         ///< �t�B�[���h�G�t�F�N�g�ǉ�
  SC_OP_REMOVE_FLDEFF,      ///< �t�B�[���h�G�t�F�N�g�폜
  SC_OP_SET_POKE_COUNTER,   ///< �|�P�����J�E���^�l�Z�b�g
  SC_OP_BATONTOUCH,         ///< �o�g���^�b�`
  SC_OP_MIGAWARI_CREATE,    ///< �݂����쐬
  SC_OP_MIGAWARI_DELETE,    ///< �݂����폜
  SC_OP_SHOOTER_CHARGE,     ///< �V���[�^�[�G�l���M�[�`���[�W
  SC_OP_SET_FAKESRC,        ///< �C�����[�W�����p�Q�ƃ|�P�����ύX
  SC_OP_CLEAR_CONSUMED_ITEM,///< �A�C�e��������̃N���A
  SC_OP_WAZADMG_REC,        ///< ���U�_���[�W�L�^
  SC_OP_TURN_CHECK,         ///< �^�[���`�F�b�N
  SC_ACT_WAZA_EFFECT,
  SC_ACT_TAMEWAZA_HIDE,     ///< �y�A�N�V�����z������ƂԂȂǂŉ�ʂ��������E�����ݒ�
  SC_ACT_WAZA_DMG,          ///< �y�A�N�V�����z[ AtClient, DefClient, wazaIdx, Affinity ]
  SC_ACT_WAZA_DMG_PLURAL,   ///< �y�A�N�V�����z�����̓����_���[�W���� [ pokeCnt, affAbout, ... ]
  SC_ACT_WAZA_ICHIGEKI,     ///< �y�A�N�V�����z�ꌂ���U����
  SC_ACT_SICK_ICON,         ///< �y�A�N�V�����z�Q�[�W�ɏ�Ԉُ�A�C�R���\��
  SC_ACT_CONF_DMG,          ///< �y�A�N�V�����z�����񎩔��_���[�W [ pokeID ]
  SC_ACT_RANKUP,            ///< �y�����N�A�b�v���ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  SC_ACT_RANKDOWN,          ///< �y�����N�_�E�����ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  SC_ACT_DEAD,              ///< �y�|�P�����Ђ񂵁z
  SC_ACT_RELIVE,            ///< �y�|�P���������Ԃ�z
  SC_ACT_MEMBER_OUT_MSG,    ///< �y�|�P�����ޏꃁ�b�Z�[�W�z[ ClientID, pokeID ]
  SC_ACT_MEMBER_OUT,        ///< �y�|�P�����ޏ�z[ ClientID, memberIdx ]
  SC_ACT_MEMBER_IN,         ///< �y�|�P�����C���z[ ClientID, posIdx, memberIdx ]
  SC_ACT_WEATHER_DMG,       ///< �V��ɂ���ă_���[�W����[ weather, pokeCnt ]
  SC_ACT_WEATHER_START,     ///< �V��ω�
  SC_ACT_WEATHER_END,       ///< �^�[���`�F�b�N�œV��I��
  SC_ACT_SIMPLE_HP,         ///< �V���v����HP�Q�[�W��������
  SC_ACT_KINOMI,            ///< ���݂̂�H�ׂ�
  SC_ACT_KILL,              ///< �����m�����o�i�݂��Â�A�ꌂ���U�Ȃǁj
  SC_ACT_MOVE,              ///< ���[�u
  SC_ACT_EXP,               ///< �o���l�擾
  SC_ACT_BALL_THROW,        ///< �{�[������
  SC_ACT_ROTATION,          ///< ���[�e�[�V����
  SC_ACT_CHANGE_TOKUSEI,    ///< �Ƃ������ύX
  SC_ACT_FAKE_DISABLE,      ///< �C�����[�W��������
  SC_ACT_EFFECT_BYPOS,      ///< �w��ʒu�ɃG�t�F�N�g����
  SC_ACT_EFFECT_BYVECTOR,   ///< �w�蔭���ʒu���I�_�ʒu�ŃG�t�F�N�g����
  SC_ACT_CHANGE_FORM,       ///< �t�H�����i���o�[�`�F���W
  SC_ACT_RESET_MOVE,        ///< ���Z�b�g���[�u�J�b�g�C��
  SC_TOKWIN_IN,             ///< �Ƃ������E�B���h�E�\���C�� [ClientID]
  SC_TOKWIN_OUT,            ///< �Ƃ������E�B���h�E�\���A�E�g [ClientID]
  SC_MSG_WAZA,              ///< ���U���b�Z�[�W�\��[ ClientID, wazaIdx ]
  SC_MSG_STD,               ///< ���b�Z�[�W�\�� [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_SET,               ///< ���b�Z�[�W�\�� [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_STD_SE,            ///< ���b�Z�[�W�\����SE [MsgID, SENo, numArgs, arg1, arg2, ... ]

  SC_MAX,

  SCEX_RESERVE,

}ServerCmd;

//--------------------------------------------------------------
/**
 *    �T�[�o�L���[�\��
 */
//--------------------------------------------------------------
typedef struct {
  u32   writePtr;
  u32   readPtr;
  u8    buffer[BTL_SERVER_CMD_QUE_SIZE];
}BTL_SERVER_CMD_QUE;

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
static inline void SCQUE_Init( BTL_SERVER_CMD_QUE* que )
{
  que->writePtr = 0;
  que->readPtr = 0;
}
static inline void SCQUE_Setup( BTL_SERVER_CMD_QUE* que, const void* data, u16 dataLength )
{
  GF_ASSERT(dataLength<BTL_SERVER_CMD_QUE_SIZE);

  GFL_STD_MemCopy32( data, que->buffer, dataLength );
  que->writePtr = dataLength;
  que->readPtr = 0;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
extern void scque_put1byte( BTL_SERVER_CMD_QUE* que, u8 data );
extern u8 scque_read1byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put2byte( BTL_SERVER_CMD_QUE* que, u16 data );
extern u16 scque_read2byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put3byte( BTL_SERVER_CMD_QUE* que, u32 data );
extern u32 scque_read3byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put4byte( BTL_SERVER_CMD_QUE* que, u32 data );
extern u32 scque_read4byte( BTL_SERVER_CMD_QUE* que );

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
static inline BOOL SCQUE_IsFinishRead( const BTL_SERVER_CMD_QUE* que )
{
  return que->readPtr == que->writePtr;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
extern void SCQUE_PUT_Common( BTL_SERVER_CMD_QUE* que, ServerCmd cmd, ... );

//---------------------------------------------
static inline void SCQUE_PUT_OP_HpMinus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 value )
{
  SCQUE_PUT_Common( que, SC_OP_HP_MINUS, pokeID, value );
}

static inline void SCQUE_PUT_OP_HpPlus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 value )
{
  SCQUE_PUT_Common( que, SC_OP_HP_PLUS, pokeID, value );
}
static inline void SCQUE_PUT_OP_HpZero( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_HP_ZERO, pokeID );
}

static inline void SCQUE_PUT_OP_PPMinus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_MINUS, pokeID, wazaIdx, value );
}

static inline void SCQUE_PUT_OP_PPPlus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_PLUS, pokeID, wazaIdx, value );
}

static inline void SCQUE_PUT_OP_RankUp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_UP, pokeID, statusType, volume );
}

static inline void SCQUE_PUT_OP_RankDown( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_DOWN, pokeID, statusType, volume );
}

static inline void SCQUE_PUT_OP_RankSet7( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 atk, u8 def, u8 sp_atk, u8 sp_def, u8 agi, u8 hit, u8 avoid )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_SET7, pokeID, atk, def, sp_atk, sp_def, agi, hit, avoid );
}
static inline void SCQUE_PUT_OP_RankRecover( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_RECOVER, pokeID );
}
static inline void SCQUE_PUT_OP_RankReset( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_RESET, pokeID );
}

static inline void SCQUE_PUT_OP_AddCritical( BTL_SERVER_CMD_QUE* que, u8 pokeID, int value )
{
  SCQUE_PUT_Common( que, SC_OP_ADD_CRITICAL, pokeID, value );
}


static inline void SCQUE_PUT_OP_SetSick( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 sick, u32 contParam )
{
  SCQUE_PUT_Common( que, SC_OP_SICK_SET, pokeID, sick, contParam );
}

static inline void SCQUE_PUT_OP_CurePokeSick( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CURE_POKESICK, pokeID );
}
static inline void SCQUE_PUT_OP_CureWazaSick( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 sickID )
{
  SCQUE_PUT_Common( que, SC_OP_CURE_WAZASICK, pokeID, sickID );
}
static inline void SCQUE_PUT_OP_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u16 turnCount )
{
  SCQUE_PUT_Common( que, SC_OP_MEMBER_IN, clientID, posIdx, memberIdx, turnCount );
}
static inline void SCQUE_PUT_OP_SetStatus( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppValueID vid, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_SET_STATUS, pokeID, vid, value );
}
static inline void SCQUE_PUT_OP_SetWeight( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 weight )
{
  SCQUE_PUT_Common( que, SC_OP_SET_WEIGHT, pokeID, weight );
}



static inline void SCQUE_PUT_OP_WazaSickTurnCheck( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_WAZASICK_TURNCHECK, pokeID );
}

static inline void SCQUE_PUT_OP_ChangePokeType( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 typePair )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_POKETYPE, pokeID, typePair );
}
static inline void SCQUE_PUT_OP_ConsumeItem( BTL_SERVER_CMD_QUE* que, u8 pokeID  )
{
  SCQUE_PUT_Common( que, SC_OP_CONSUME_ITEM, pokeID );
}

static inline void SCQUE_PUT_OP_UpdateWazaProcResult( BTL_SERVER_CMD_QUE* que,
    u8 pokeID, BtlPokePos actTargetPos, BOOL fActEnable, WazaID actWaza, WazaID orgWaza )
{
  SCQUE_PUT_Common( que, SC_OP_UPDATE_USE_WAZA, pokeID, actTargetPos, fActEnable, actWaza, orgWaza );
}


static inline void SCQUE_PUT_OP_SetTurnFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppTurnFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_SET_TURNFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ResetTurnFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppTurnFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_RESET_TURNFLAG, pokeID, flag );
}



static inline void SCQUE_PUT_OP_SetContFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppContFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_SET_CONTFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ResetContFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppContFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_RESET_CONTFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ChangeTokusei( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 tokID )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_TOKUSEI, pokeID, tokID );
}
static inline void SCQUE_PUT_OP_SetItem( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 itemID )
{
  SCQUE_PUT_Common( que, SC_OP_SET_ITEM, pokeID, itemID );
}
static inline void SCQUE_PUT_OP_UpdateWaza( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u16 wazaID, u8 ppMax, u8 fPermanent )
{
  SCQUE_PUT_Common( que, SC_OP_UPDATE_WAZANUMBER, pokeID, wazaIdx, ppMax, fPermanent, wazaID );
}
static inline void SCQUE_PUT_OP_Hensin( BTL_SERVER_CMD_QUE* que, u8 atkPokeID, u8 tgtPokeID )
{
  SCQUE_PUT_Common( que, SC_OP_HENSIN, atkPokeID, tgtPokeID );
}
static inline void SCQUE_PUT_OP_OutClear( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_OUTCLEAR, pokeID );
}
static inline void SCQUE_PUT_OP_AddFieldEffect( BTL_SERVER_CMD_QUE* que, u8 eff, u16 cont )
{
  SCQUE_PUT_Common( que, SC_OP_ADD_FLDEFF, eff, cont );
}
static inline void SCQUE_PUT_OP_RemoveFieldEffect( BTL_SERVER_CMD_QUE* que, u8 eff )
{
  SCQUE_PUT_Common( que, SC_OP_REMOVE_FLDEFF, eff );
}
static inline void SCQUE_PUT_OP_SetPokeCounter( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 counterID, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_SET_POKE_COUNTER, pokeID, counterID, value );
}
static inline void SCQUE_PUT_OP_BatonTouch( BTL_SERVER_CMD_QUE* que, u8 userPokeID, u8 targetPokeID )
{
  SCQUE_PUT_Common( que, SC_OP_BATONTOUCH, userPokeID, targetPokeID );
}
static inline void SCQUE_PUT_OP_MigawariCreate( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 migawariHP )
{
  SCQUE_PUT_Common( que, SC_OP_MIGAWARI_CREATE, pokeID, migawariHP );
}
static inline void SCQUE_PUT_OP_MigawariDelete( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_MIGAWARI_DELETE, pokeID );
}
static inline void SCQUE_PUT_OP_ShooterCharge( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 increment )
{
  SCQUE_PUT_Common( que, SC_OP_SHOOTER_CHARGE, clientID, increment );
}
static inline void SCQUE_PUT_OP_SetFakeSrcMember( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 memberIdx )
{
  SCQUE_PUT_Common( que, SC_OP_SET_FAKESRC, clientID, memberIdx );
}
static inline void SCQUE_PUT_OP_ClearConsumedItem( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CLEAR_CONSUMED_ITEM, pokeID );
}
static inline void SCQUE_OP_AddWazaDmgRec( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 atkPokeID, BtlPokePos pokePos, PokeType wazaType, u16 wazaID, u16 damage )
{
  SCQUE_PUT_Common( que, SC_OP_WAZADMG_REC, defPokeID, atkPokeID, pokePos, wazaType, wazaID, damage );
}
static inline void SCQUE_PUT_OP_TurnCheck( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_TURN_CHECK, pokeID );
}





//---------------------------------------------
static inline void SCQUE_PUT_ACT_WazaEffect( BTL_SERVER_CMD_QUE* que, u8 atPokePos, u8 defPokePos, u16 waza, u8 arg )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT, atPokePos, defPokePos, waza, arg );
}
/*
static inline void SCQUE_PUT_ACT_WazaEffectEx( BTL_SERVER_CMD_QUE* que, u8 atPokePos, u8 defPokePos, u16 waza, u8 arg )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT_EX, atPokePos, defPokePos, waza, arg );
}
*/
// ���߃��U�i������Ƃԓ��j�̊J�n���Ɏp�������E�U����Ɏp������
static inline void SCQUE_PUT_ACT_TameWazaHide( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 hideFlag )
{
  SCQUE_PUT_Common( que, SC_ACT_TAMEWAZA_HIDE, pokeID, hideFlag );
}



// �y�A�N�V�����z�P�̃_���[�W����
static inline void SCQUE_PUT_ACT_WazaDamage( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 affAbout, u16 wazaID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG, defPokeID, affAbout, wazaID );
}
// �y�A�N�V�����z�����̓����_���[�W����
static inline void SCQUE_PUT_ACT_WazaDamagePlural( BTL_SERVER_CMD_QUE* que, u8 pokeCnt, u8 affAbout, u16 wazaID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG_PLURAL, pokeCnt, affAbout, wazaID );
}

// �y�A�N�V�����z�ꌂ�K�E���U����
static inline void SCQUE_PUT_ACT_WazaIchigeki( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_ICHIGEKI, pokeID );
}

// �y�A�N�V�����z�Q�[�W�ɏ�Ԉُ�A�C�R����\���iPOKESICK_NULL�ŕ\���I�t�j
static inline void SCQUE_PUT_ACT_SickIcon( BTL_SERVER_CMD_QUE* que, u8 pokeID, PokeSick sick )
{
  SCQUE_PUT_Common( que, SC_ACT_SICK_ICON, pokeID, sick );
}

// �y�A�N�V�����z�����񎩔��_���[�W
static inline void SCQUE_PUT_ACT_ConfDamage( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_CONF_DMG, pokeID );
}
// �y�A�N�V�����z�\�̓����N�A�b�v
static inline void SCQUE_PUT_ACT_RankUp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_ACT_RANKUP, pokeID, statusType, volume );
}
// �y�A�N�V�����z�\�̓����N�_�E��
static inline void SCQUE_PUT_ACT_RankDown( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_ACT_RANKDOWN, pokeID, statusType, volume );
}
// �y�A�N�V�����z�|�P�����Ђ�
static inline void SCQUE_PUT_ACT_Dead( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_DEAD, pokeID );
}
// �y�A�N�V�����z�|�P���������Ԃ�
static inline void SCQUE_PUT_ACT_RelivePoke( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_RELIVE, pokeID );
}
// �y�A�N�V�����z�|�P�����ޏꃁ�b�Z�[�W�\��
static inline void SCQUE_PUT_ACT_MemberOutMsg( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT_MSG, clientID, pokeID );
}
// �y�A�N�V�����z�|�P�����ޏ�
static inline void SCQUE_PUT_ACT_MemberOut( BTL_SERVER_CMD_QUE* que, BtlPokePos pos )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT, pos );
}
// �y�A�N�V�����z�|�P��������
static inline void SCQUE_PUT_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u8 fPutMsg )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_IN, clientID, posIdx, memberIdx, fPutMsg );
}
// �y�A�N�V�����z�V��ɂ���ă_���[�W   weather:�V��ID, pokeCnt:�_���[�W���󂯂�|�P������
static inline void SCQUE_PUT_ACT_WeatherDamage( BTL_SERVER_CMD_QUE* que, u8 weather, u8 pokeCnt )
{
  SCQUE_PUT_Common( que, SC_ACT_WEATHER_DMG, weather, pokeCnt );
}

static inline void SCQUE_PUT_ACT_WeatherStart( BTL_SERVER_CMD_QUE* que, u8 weather )
{
  SCQUE_PUT_Common( que, SC_ACT_WEATHER_START, weather );
}

static inline void SCQUE_PUT_ACT_WeatherEnd( BTL_SERVER_CMD_QUE* que, u8 weather )
{
  SCQUE_PUT_Common( que, SC_ACT_WEATHER_END, weather );
}

static inline void SCQUE_PUT_ACT_SimpleHP( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_SIMPLE_HP, pokeID );
}

static inline void SCQUE_PUT_ACT_KINOMI( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_KINOMI, pokeID );
}
static inline void SCQUE_PUT_ACT_Kill( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 effectType )
{
  SCQUE_PUT_Common( que, SC_ACT_KILL, pokeID, effectType );
}
static inline void SCQUE_PUT_ACT_MemberMove( BTL_SERVER_CMD_QUE* que, u8 clientID, BtlPokePos pos1, BtlPokePos pos2 )
{
  SCQUE_PUT_Common( que, SC_ACT_MOVE, clientID, pos1, pos2 );
}
static inline void SCQUE_PUT_ACT_AddExp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u32 exp )
{
  SCQUE_PUT_Common( que, SC_ACT_EXP, pokeID, exp );
}
static inline void SCQUE_PUT_ACT_BallThrow( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u8 yureCnt,
    u8 fSuccess, u8 fZukanRegister, u8 fCritical, u16 ballItemID )
{
  SCQUE_PUT_Common( que, SC_ACT_BALL_THROW, pos, yureCnt, fSuccess, fZukanRegister, fCritical, ballItemID );
}
static inline void SCQUE_PUT_ACT_Rotation( BTL_SERVER_CMD_QUE* que, u8 clientID, BtlRotateDir dir )
{
  SCQUE_PUT_Common( que, SC_ACT_ROTATION, clientID, dir );
}
static inline void SCQUE_PUT_ACT_ChangeTokusei( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 tokuseiID )
{
  SCQUE_PUT_Common( que, SC_ACT_CHANGE_TOKUSEI, pokeID, tokuseiID );
}
static inline void SCQUE_PUT_ACT_FakeDisable( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_FAKE_DISABLE, pokeID );
}
static inline void SCQUE_PUT_ACT_EffectByPos( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYPOS, pos, effectNo );
}
static inline void SCQUE_PUT_ACT_EffectByVector( BTL_SERVER_CMD_QUE* que, BtlPokePos pos_from, BtlPokePos pos_to, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYVECTOR, pos_from, pos_to, effectNo );
}
static inline void SCQUE_PUT_ACT_ChangeForm( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 formNo )
{
  SCQUE_PUT_Common( que, SC_ACT_CHANGE_FORM, pokeID, formNo );
}
static inline void SCQUE_PUT_ACT_TripleResetMove( BTL_SERVER_CMD_QUE* que, u8 clientID_1, u8 posIdx1, u8 clientID_2, u8 posIdx2 )
{
  SCQUE_PUT_Common( que, SC_ACT_RESET_MOVE, clientID_1, posIdx1, clientID_2, posIdx2 );
}




static inline void SCQUE_PUT_TOKWIN_IN( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_TOKWIN_IN, pokeID );
}
static inline void SCQUE_PUT_TOKWIN_OUT( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_TOKWIN_OUT, pokeID );
}

static inline void SCQUE_PUT_MSG_WAZA( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 waza )
{
  SCQUE_PUT_Common( que, SC_MSG_WAZA, pokeID, waza );
}

//=====================================================
typedef u32 ScMsgArg;
enum {
  MSGARG_TERMINATOR = 0xffff0000,
};

#include <stdarg.h>

// �� �ϕ����̍ŏ��̈����ɂ͕K��������ID��n���B
extern void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... );
#define SCQUE_PUT_MSG_STD(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_STD_SE(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_STD_SE, __VA_ARGS__, MSGARG_TERMINATOR )

//=====================================================

extern u16  SCQUE_RESERVE_Pos( BTL_SERVER_CMD_QUE* que, ServerCmd cmd );
extern void SCQUE_PUT_ReservedPos( BTL_SERVER_CMD_QUE* que, u16 pos, ServerCmd cmd, ... );

extern ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args );

extern void SCQUE_PUT_ArgOnly( BTL_SERVER_CMD_QUE* que, u8 arg );
extern u8 SCQUE_READ_ArgOnly( BTL_SERVER_CMD_QUE* que );

#endif
