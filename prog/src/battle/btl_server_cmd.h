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
  BTL_SERVER_CMD_QUE_SIZE = 384,
  BTL_SERVERCMD_ARG_MAX = 16,
};




//--------------------------------------------------------------
/**
 *    �T�[�o�R�}���h
 */
//--------------------------------------------------------------
typedef enum {
  SC_NULL = 0,

  SC_OP_HP_MINUS,           ///< �y�v�Z�zHP�}�C�i�X  [ClientID, �}�C�i�X��]
  SC_OP_HP_PLUS,            ///< �y�v�Z�zHP�v���X    [ClientID, �v���X��]
  SC_OP_HP_ZERO,            ///< �y�v�Z�zHP0 �ɂ��� [ pokeID ]
  SC_OP_PP_MINUS,           ///< �y�v�Z�zPP�}�C�i�X  [ClientID, �}�C�i�X��]
  SC_OP_PP_PLUS,            ///< �y�v�Z�zPP�v���X    [ClientID, �v���X��]
  SC_OP_RANK_UP,            ///< �y�v�Z�z�X�e�[�^�X�����N�A�b�v  [ClientID, StatusType, �v���X��]
  SC_OP_RANK_DOWN,          ///< �y�v�Z�z�X�e�[�^�X�����N�_�E��  [ClientID, StatusType, �}�C�i�X��]
  SC_OP_RANK_SET5,          ///< �y�v�Z�z�X�e�[�^�X�����N��v�T��Z�b�g[ pokeID, atk, def, sp_atk, sp_def, agi ]
  SC_OP_SICK_SET,           ///< �y�v�Z�z��Ԉُ� [PokeID, Sick, contParam]
  SC_OP_CURE_POKESICK,      ///< �y�v�Z�z�|�P�����n��Ԉُ���� [PokeID ]
  SC_OP_CURE_WAZASICK,      ///< �y�v�Z�z���U�n��Ԉُ���� [PokeID, SickID ]
  SC_OP_MEMBER_IN,          ///< �����o�[����
  SC_OP_WAZASICK_TURNCHECK, ///<
  SC_OP_CANTESCAPE_ADD,     ///< �ɂ��E�����֎~�R�[�h�̒ǉ���S�N���C�A���g�ɒʒm [ClientID, CantCode]
  SC_OP_CANTESCAPE_SUB,     ///< �ɂ��E�����֎~�R�[�h�̍폜��S�N���C�A���g�ɒʒm [ClientID, CantCode]
  SC_OP_CHANGE_POKETYPE,    ///< �y�v�Z�z�|�P�����̃^�C�v�ύX�i pokeID, type �j
  SC_OP_CHANGE_POKEFORM,    ///< �y�v�Z�z�|�P�����̃t�H�����ύX�i pokeID, type �j
  SC_OP_REMOVE_ITEM,        ///< ���L�A�C�e���폜
  SC_OP_UPDATE_USE_WAZA,    ///< ���O�g�p���U�X�V
  SC_OP_RESET_USED_WAZA,    ///< �A�����U�g�p�J�E���^���Z�b�g
  SC_OP_SET_CONTFLAG,       ///< �i���t���O�Z�b�g
  SC_OP_RESET_CONTFLAG,     ///< �i���t���O���Z�b�g
  SC_OP_SET_ACTFLAG,        ///< �A�N�V�������t���O�Z�b�g
  SC_OP_CLEAR_ACTFLAG,      ///< �A�N�V�������t���O�I�[���N���A
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
  SC_ACT_WAZA_EFFECT,
  SC_ACT_WAZA_EFFECT_EX,    ///< �y�A�N�V�����z���U�G�t�F�N�g�g���i���߃^�[���G�t�F�N�g�ȂǂɎg�p�j
  SC_ACT_WAZA_DMG,          ///< �y�A�N�V�����z[ AtClient, DefClient, wazaIdx, Affinity ]
  SC_ACT_WAZA_DMG_DBL,      ///< �y�A�N�V�����z�Q�̓����_���[�W���� [ pokeID ]
  SC_ACT_WAZA_DMG_PLURAL,   ///< �y�A�N�V�����z�����̓����_���[�W���� [ pokeCnt, affAbout, ... ]
  SC_ACT_WAZA_ICHIGEKI,     ///< �y�A�N�V�����z�ꌂ���U����
  SC_ACT_CONF_DMG,          ///< �y�A�N�V�����z�����񎩔��_���[�W [ pokeID ]
  SC_ACT_RANKUP,            ///< �y�����N�A�b�v���ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  SC_ACT_RANKDOWN,          ///< �y�����N�_�E�����ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
  SC_ACT_DEAD,              ///< �y�|�P�����Ђ񂵁z[ ClientID ]
  SC_ACT_MEMBER_OUT,        ///< �y�|�P�����ޏ�z[ ClientID, memberIdx ]
  SC_ACT_MEMBER_IN,         ///< �y�|�P�����C���z[ ClientID, posIdx, memberIdx ]
  SC_ACT_SICK_DMG,          ///<  �A�N�V�����^�^�[���`�F�b�N���̏�Ԉُ�_���[�W
  SC_ACT_WEATHER_DMG,       ///< �V��ɂ���ă_���[�W����[ weather, pokeCnt ]
  SC_ACT_WEATHER_START,     ///< �V��ω�
  SC_ACT_WEATHER_END,       ///< �^�[���`�F�b�N�œV��I��
  SC_ACT_SIMPLE_HP,         ///< �V���v����HP�Q�[�W��������
  SC_ACT_TRACE_TOKUSEI,     ///< �Ƃ������g���[�X[ pokeID, targetPokeID, tokusei ]
  SC_ACT_KINOMI,            ///< ���݂̂�H�ׂ�
  SC_ACT_KILL,              ///< �����m�����o�i�݂��Â�A�ꌂ���U�Ȃǁj
  SC_TOKWIN_IN,             ///< �Ƃ������E�B���h�E�\���C�� [ClientID]
  SC_TOKWIN_OUT,            ///< �Ƃ������E�B���h�E�\���A�E�g [ClientID]
  SC_MSG_WAZA,              ///< ���U���b�Z�[�W�\��[ ClientID, wazaIdx ]
  SC_MSG_STD,               ///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]
  SC_MSG_SET,               ///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]

  SC_MAX,

  SCEX_RESERVE,

}ServerCmd;

//--------------------------------------------------------------
/**
 *    �T�[�o�L���[�\��
 */
//--------------------------------------------------------------
typedef struct {
  u16   writePtr;
  u16   readPtr;
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
static inline void scque_put1byte( BTL_SERVER_CMD_QUE* que, u8 data )
{
  GF_ASSERT(que->writePtr < BTL_SERVER_CMD_QUE_SIZE);
  que->buffer[ que->writePtr++ ] = data;
}
static inline u8 scque_read1byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < que->writePtr);
  return que->buffer[ que->readPtr++ ];
}
static inline void scque_put2byte( BTL_SERVER_CMD_QUE* que, u16 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-1));
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
static inline u16 scque_read2byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT_MSG(que->readPtr < (que->writePtr-1), "rp=%d, wp=%d", que->readPtr, que->writePtr);
  {
    u16 data = ( (que->buffer[que->readPtr] << 8) | que->buffer[que->readPtr+1] );
    que->readPtr += 2;
    return data;
  }
}
static inline void scque_put3byte( BTL_SERVER_CMD_QUE* que, u32 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-2));
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
static inline u32 scque_read3byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < (que->writePtr-2));
  {
    u32 data = ( (que->buffer[que->readPtr]<<16) | (que->buffer[que->readPtr+1]<<8) | (que->buffer[que->readPtr+2]) );
    que->readPtr += 3;
    return data;
  }
}
static inline void scque_put4byte( BTL_SERVER_CMD_QUE* que, u32 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-3));
  que->buffer[ que->writePtr++ ] = (data >> 24)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
static inline u32 scque_read4byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < (que->writePtr-3));
  {
    u32 data = ( (que->buffer[que->readPtr]<<24) | (que->buffer[que->readPtr+1]<<16)
               | (que->buffer[que->readPtr+2]<<8) | (que->buffer[que->readPtr+3]) );
    que->readPtr += 4;
    return data;
  }
}

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

static inline void SCQUE_PUT_OP_RankSet5( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 atk, u8 def, u8 sp_atk, u8 sp_def, u8 agi )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_SET5, pokeID, atk, def, sp_atk, sp_def, agi );
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


static inline void SCQUE_PUT_OP_WazaSickTurnCheck( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_WAZASICK_TURNCHECK, pokeID );
}

static inline void SCQUE_PUT_OP_CantEscape_Add( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 cantCode )
{
  SCQUE_PUT_Common( que, SC_OP_CANTESCAPE_ADD, clientID, cantCode );
}
static inline void SCQUE_PUT_OP_CantEscape_Sub( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 cantCode )
{
  SCQUE_PUT_Common( que, SC_OP_CANTESCAPE_SUB, clientID, cantCode );
}
static inline void SCQUE_PUT_OP_ChangePokeType( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 typePair )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_POKETYPE, pokeID, typePair );
}
static inline void SCQUE_PUT_OP_ChangePokeForm( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 formNo )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_POKEFORM, pokeID, formNo );
}
static inline void SCQUE_PUT_OP_RemoveItem( BTL_SERVER_CMD_QUE* que, u8 pokeID  )
{
  SCQUE_PUT_Common( que, SC_OP_REMOVE_ITEM, pokeID );
}
static inline void SCQUE_PUT_OP_UpdateUseWaza( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 targetPos, WazaID waza  )
{
  SCQUE_PUT_Common( que, SC_OP_UPDATE_USE_WAZA, pokeID, targetPos, waza );
}
static inline void SCQUE_PUT_OP_ResetUsedWazaCounter( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_RESET_USED_WAZA, pokeID );
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
static inline void SCQUE_PUT_OP_SetActFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppActFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_SET_ACTFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ClearActFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CLEAR_ACTFLAG, pokeID );
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



//---------------------------------------------
static inline void SCQUE_PUT_ACT_WazaEffect( BTL_SERVER_CMD_QUE* que, u8 atPokeID, u8 defPokeID, u16 waza )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT, atPokeID, defPokeID, waza );
}
static inline void SCQUE_PUT_ACT_WazaEffectEx( BTL_SERVER_CMD_QUE* que, u8 atPokeID, u8 defPokeID, u16 waza, u8 arg )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT_EX, atPokeID, defPokeID, waza, arg );
}

// �y�A�N�V�����z�P�̃_���[�W����
static inline void SCQUE_PUT_ACT_WazaDamage( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 affinity, u16 damage )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG, defPokeID, affinity, damage );
}
// �y�A�N�V�����z�Q�̓����_���[�W����
static inline void SCQUE_PUT_ACT_WazaDamageDbl( BTL_SERVER_CMD_QUE* que, u8 defPokeID1, u8 defPokeID2, u16 damage1, u16 damage2, u8 aff  )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG_DBL, defPokeID1, defPokeID2, aff, damage1, damage2 );
}
// �y�A�N�V�����z�����̓����_���[�W����
static inline void SCQUE_PUT_ACT_WazaDamagePlural( BTL_SERVER_CMD_QUE* que, u8 pokeCnt, u8 affAbout )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG_PLURAL, pokeCnt, affAbout );
}

// �y�A�N�V�����z�ꌂ�K�E���U����
static inline void SCQUE_PUT_ACT_WazaIchigeki( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_ICHIGEKI, pokeID );
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
static inline void SCQUE_PUT_ACT_Kill( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 effectType )
{
  SCQUE_PUT_Common( que, SC_ACT_KILL, pokeID, effectType );
}

// �y�A�N�V�����z�|�P�����Ђ�
static inline void SCQUE_PUT_ACT_Dead( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_DEAD, pokeID );
}
// �y�A�N�V�����z�|�P�����ޏ�
static inline void SCQUE_PUT_ACT_MemberOut( BTL_SERVER_CMD_QUE* que, BtlPokePos pos )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT, pos );
}
// �y�A�N�V�����z�|�P��������
static inline void SCQUE_PUT_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_IN, clientID, posIdx, memberIdx );
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

static inline void SCQUE_PUT_ACT_TokTrace( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 targetPokeID, PokeTokusei tok )
{
  SCQUE_PUT_Common( que, SC_ACT_TRACE_TOKUSEI, pokeID, targetPokeID, tok );
}
static inline void SCQUE_PUT_ACT_KINOMI( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_KINOMI, pokeID );
}
static inline void SCQUE_PUT_SickDamage( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 sick, u8 damage )
{
  SCQUE_PUT_Common( que, SC_ACT_SICK_DMG, pokeID, sick, damage );
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
typedef u16 ScMsgArg;
enum {
  MSGARG_TERMINATOR = 0xffff,
};

#include <stdarg.h>

// �� �ϕ����̍ŏ��̈����ɂ͕K��������ID��n���B
extern void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... );
#define SCQUE_PUT_MSG_STD(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )


//=====================================================

extern u16  SCQUE_RESERVE_Pos( BTL_SERVER_CMD_QUE* que, ServerCmd cmd );
extern void SCQUE_PUT_ReservedPos( BTL_SERVER_CMD_QUE* que, u16 pos, ServerCmd cmd, ... );

extern ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args );

extern void SCQUE_PUT_ArgOnly( BTL_SERVER_CMD_QUE* que, u8 arg );
extern u8 SCQUE_READ_ArgOnly( BTL_SERVER_CMD_QUE* que );

#endif
