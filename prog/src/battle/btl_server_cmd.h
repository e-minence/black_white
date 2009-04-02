//=============================================================================================
/**
 * @file	btl_server_cmd.h
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o�R�}���h�����C���ߏ���
 * @author	taya
 *
 * @date	2008.10.30	�쐬
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
 *		�T�[�o�R�}���h
 */
//--------------------------------------------------------------
typedef enum {
	SC_NULL = 0,

	SC_OP_HP_MINUS,			///< �y�v�Z�zHP�}�C�i�X  [ClientID, �}�C�i�X��]
	SC_OP_HP_PLUS,			///< �y�v�Z�zHP�v���X    [ClientID, �v���X��]
	SC_OP_HP_ZERO,			///< �y�v�Z�zHP0 �ɂ��� [ pokeID ]
	SC_OP_PP_MINUS,			///< �y�v�Z�zPP�}�C�i�X  [ClientID, �}�C�i�X��]
	SC_OP_PP_PLUS,			///< �y�v�Z�zPP�v���X    [ClientID, �v���X��]
	SC_OP_RANK_UP,			///< �y�v�Z�z�X�e�[�^�X�����N�A�b�v  [ClientID, StatusType, �v���X��]
	SC_OP_RANK_DOWN,		///< �y�v�Z�z�X�e�[�^�X�����N�_�E��  [ClientID, StatusType, �}�C�i�X��]
	SC_OP_SICK_SET,			///< �y�v�Z�z��Ԉُ� [PokeID, Sick, contParam]
	SC_OP_CURE_POKESICK,			///< �y�v�Z�z�|�P�����n��Ԉُ���� [PokeID ]
	SC_OP_CURE_WAZASICK,			///< �y�v�Z�z���U�n��Ԉُ���� [PokeID, SickID ]
	SC_OP_WAZASICK_TURNCHECK,	///< 
	SC_OP_CANTESCAPE_ADD,		///< �ɂ��E�����֎~�R�[�h�̒ǉ���S�N���C�A���g�ɒʒm [ClientID, CantCode]
	SC_OP_CANTESCAPE_SUB,		///< �ɂ��E�����֎~�R�[�h�̍폜��S�N���C�A���g�ɒʒm [ClientID, CantCode]
	SC_OP_CHANGE_POKETYPE,	///< �y�v�Z�z�|�P�����̃^�C�v�ύX�i pokeID, type �j
	SC_OP_CHANGE_POKEFORM,	///< �y�v�Z�z�|�P�����̃t�H�����ύX�i pokeID, type �j
	SC_ACT_WAZA_EFFECT,
	SC_ACT_WAZA_DMG,		///< �y�A�N�V�����z[ AtClient, DefClient, wazaIdx, Affinity ]
	SC_ACT_WAZA_DMG_DBL,///< �y�A�N�V�����z�Q�̓����_���[�W���� [ pokeID ]
	SC_ACT_WAZA_DMG_PLURAL,	///< �y�A�N�V�����z�����̓����_���[�W���� [ pokeCnt, affAbout, ... ]
	SC_ACT_WAZA_ICHIGEKI,///< �y�A�N�V�����z�ꌂ���U����
	SC_ACT_CONF_DMG,		///< �y�A�N�V�����z�����񎩔��_���[�W [ pokeID ]
	SC_ACT_RANKUP,			///< �y�����N�A�b�v���ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
	SC_ACT_RANKDOWN,		///< �y�����N�_�E�����ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
	SC_ACT_DEAD,				///< �y�|�P�����Ђ񂵁z[ ClientID ]
	SC_ACT_MEMBER_OUT,	///< �y�|�P�����ޏ�z[ ClientID, memberIdx ]
	SC_ACT_MEMBER_IN,		///< �y�|�P�����C���z[ ClientID, posIdx, memberIdx ]
	SC_ACT_SICK_SET,		///<  ��Ԉُ���������
	SC_ACT_SICK_DMG,		///<  �A�N�V�����^�^�[���`�F�b�N���̏�Ԉُ�_���[�W
	SC_ACT_WEATHER_DMG,	///< �V��ɂ���ă_���[�W����[ weather, pokeCnt ]
	SC_ACT_WEATHER_START,///< �V��ω�
	SC_ACT_WEATHER_END,	///< �^�[���`�F�b�N�œV��I��
	SC_ACT_SIMPLE_HP,		///< �V���v����HP�Q�[�W��������
	SC_ACT_TRACE_TOKUSEI,	///< �Ƃ������g���[�X[ pokeID, targetPokeID, tokusei ]
	SC_TOKWIN_IN,				///< �Ƃ������E�B���h�E�\���C�� [ClientID]
	SC_TOKWIN_OUT,			///< �Ƃ������E�B���h�E�\���A�E�g [ClientID]

	SC_MSG_WAZA,				///< ���U���b�Z�[�W�\��[ ClientID, wazaIdx ]
	SC_MSG_STD,					///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]
	SC_MSG_SET,					///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]

	SC_MAX,
}ServerCmd;

//--------------------------------------------------------------
/**
 *		�T�[�o�L���[�\��
 */
//--------------------------------------------------------------
typedef struct {
	u16		writePtr;
	u16		readPtr;
	u8		buffer[BTL_SERVER_CMD_QUE_SIZE];
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
	GF_ASSERT(que->readPtr < (que->writePtr-1));
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

static inline void SCQUE_PUT_OP_SetSick( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 sick, u16 contParam )
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
	SCQUE_PUT_Common( que, SC_OP_CHANGE_POKETYPE, pokeID, formNo );
}



//---------------------------------------------
static inline void SCQUE_PUT_ACT_WazaEffect( BTL_SERVER_CMD_QUE* que, u8 atPokeID, u8 defPokeID, u16 waza )
{
	SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT, atPokeID, defPokeID, waza );
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
// �y�A�N�V�����z�|�P�����Ђ�
static inline void SCQUE_PUT_ACT_Dead( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
	SCQUE_PUT_Common( que, SC_ACT_DEAD, pokeID );
}
// �y�A�N�V�����z�|�P�����ޏ�
static inline void SCQUE_PUT_ACT_MemberOut( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 memberIdx )
{
	SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT, clientID, memberIdx );
}
// �y�A�N�V�����z�|�P��������
static inline void SCQUE_PUT_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u16 turnCount )
{
	SCQUE_PUT_Common( que, SC_ACT_MEMBER_IN, clientID, posIdx, memberIdx, turnCount );
}
// �y�A�N�V�����z�V��ɂ���ă_���[�W		weather:�V��ID, pokeCnt:�_���[�W���󂯂�|�P������
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

static inline void SCQUE_PUT_ACT_SickSet( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 sick )
{
	SCQUE_PUT_Common( que, SC_ACT_SICK_SET, pokeID, sick );
}

static inline void SCQUE_PUT_ACT_SimpleHP( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
	SCQUE_PUT_Common( que, SC_ACT_SIMPLE_HP, pokeID );
}

static inline void SCQUE_PUT_ACT_TokTrace( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 targetPokeID, PokeTokusei tok )
{
	SCQUE_PUT_Common( que, SC_ACT_TRACE_TOKUSEI, pokeID, targetPokeID, tok );
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

static inline void SCQUE_PUT_MSG_WAZA( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx )
{
	SCQUE_PUT_Common( que, SC_MSG_WAZA, pokeID, wazaIdx );
}

//=====================================================
typedef u8 ScMsgArg;
enum { 
	MSGARG_TERMINATOR = 0xff,
};

#include <stdarg.h>

// �� �ϕ����̍ŏ��̈����ɂ͕K��������ID��n���B
extern void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... );
#define SCQUE_PUT_MSG_STD(que, ...)	SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...)	SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )


//=====================================================

extern ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args );

extern void SCQUE_PUT_ArgOnly( BTL_SERVER_CMD_QUE* que, u8 arg );
extern u8 SCQUE_READ_ArgOnly( BTL_SERVER_CMD_QUE* que );

#endif
