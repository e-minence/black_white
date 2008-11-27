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
	BTL_SERVER_CMD_QUE_SIZE = 1024,
	BTL_SERVERCMD_ARG_MAX = 8,
};

//--------------------------------------------------------------
/**
 *		�T�[�o�R�}���h
 */
//--------------------------------------------------------------
typedef enum {

	SC_OP_HP_MINUS=0,		///< �y�v�Z�zHP�}�C�i�X  [ClientID, �}�C�i�X��]
	SC_OP_HP_PLUS,			///< �y�v�Z�zHP�v���X    [ClientID, �v���X��]
	SC_OP_PP_MINUS,			///< �y�v�Z�zPP�}�C�i�X  [ClientID, �}�C�i�X��]
	SC_OP_PP_PLUS,			///< �y�v�Z�zPP�v���X    [ClientID, �v���X��]
	SC_OP_RANK_UP,			///< �y�v�Z�z�X�e�[�^�X�����N�A�b�v  [ClientID, StatusType, �v���X��]
	SC_OP_RANK_DOWN,		///< �y�v�Z�z�X�e�[�^�X�����N�_�E��  [ClientID, StatusType, �}�C�i�X��]

	SC_DATA_WAZA_EXE,		///< �y�f�[�^�Z�b�g�z���U���� [ AtClientID, wazaIdx, DefPokeNum, DefClientID1, ... ]
	SC_DATA_MEMBER_OUT,		///< �y�f�[�^�Z�b�g�z�t�����g�����o�[�A�E�g[ CliendID ]
	SC_DATA_MEMBER_IN,		///< �y�f�[�^�Z�b�g�z�t�����g�����o�[�C��[ ClientID, pokeIdx ]

	SC_ACT_WAZA_DMG,		///< �y���U�����F�_���[�W�z[ AtClient, DefClient, wazaIdx, Affinity ]
	SC_ACT_RANKUP,			///< �y�����N�A�b�v���ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
	SC_ACT_RANKDOWN,		///< �y�����N�_�E�����ʁz �����́~�~�~�����������I[ ClientID, statusType, volume ]
	SC_ACT_DEAD,			///< �y�|�P�����Ђ񂵁z[ ClientID ]
	SC_ACT_MEMBER_IN,		///< �y�|�P�����C���z[ ClientID, pokeIdx ]

	SC_TOKWIN_IN,			///< �Ƃ������E�B���h�E�\���C�� [ClientID]
	SC_TOKWIN_OUT,			///< �Ƃ������E�B���h�E�\���A�E�g [ClientID]
	SC_MSG_STD,				///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]
	SC_MSG_SET,				///< ���b�Z�[�W�\�� [MsgID, ClientID, numArgs, arg1, arg2, ... ]
	SC_MSG_WAZA,			///< ���U���b�Z�[�W�\��[ ClientID, wazaIdx ]

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
static inline void scque_put2byte( BTL_SERVER_CMD_QUE* que, u16 data )
{
	GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-1));
	que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
	que->buffer[ que->writePtr++ ] = (data & 0xff);
}
static inline u8 scque_read1byte( BTL_SERVER_CMD_QUE* que )
{
	GF_ASSERT(que->readPtr < que->writePtr);
	return que->buffer[ que->readPtr++ ];
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

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
static inline BOOL SCQUE_IsFinishRead( const BTL_SERVER_CMD_QUE* que )
{
	return que->readPtr == que->writePtr;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------


//---------------------------------------------
static inline void SCQUE_PUT_OP_HpMinus( BTL_SERVER_CMD_QUE* que, u8 clientID, u16 value )
{
	scque_put2byte( que, SC_OP_HP_MINUS );
	scque_put1byte( que, clientID );
	scque_put2byte( que, value );
}
static inline void SCQUE_READ_OP_HpMinus( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read2byte( que );
}

static inline void SCQUE_PUT_OP_RankUp( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 statusType, u8 volume )
{
	scque_put2byte( que, SC_OP_RANK_UP );
	scque_put1byte( que, clientID );
	scque_put1byte( que, statusType );
	scque_put1byte( que, volume );
}
static inline void SCQUE_READ_OP_RankUp( BTL_SERVER_CMD_QUE* que, int *args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read1byte( que );
}

static inline void SCQUE_PUT_OP_RankDown( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 statusType, u8 volume )
{
	scque_put2byte( que, SC_OP_RANK_DOWN );
	scque_put1byte( que, clientID );
	scque_put1byte( que, statusType );
	scque_put1byte( que, volume );
}
static inline void SCQUE_READ_OP_RankDown( BTL_SERVER_CMD_QUE* que, int *args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read1byte( que );
}


static inline void SCQUE_PUT_DATA_WazaExe( BTL_SERVER_CMD_QUE* que, u8 atClientID, u8 wazaIdx, u8 numDefClients, u8 defClientID1, u8 defClientID2, u8 defClientID3 )
{
	scque_put2byte( que, SC_DATA_WAZA_EXE );
	scque_put1byte( que, atClientID );
	scque_put1byte( que, wazaIdx );
	scque_put1byte( que, numDefClients );
	scque_put1byte( que, defClientID1 );
	scque_put1byte( que, defClientID2 );
	scque_put1byte( que, defClientID3 );
}
static inline void SCQUE_READ_DATA_WazaExe( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read1byte( que );
	args[3] = scque_read1byte( que );
	args[4] = scque_read1byte( que );
	args[5] = scque_read1byte( que );
}
// �����o�[����ւ��f�[�^
static inline void SCQUE_PUT_DATA_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 memberIdx )
{
	scque_put2byte( que, SC_DATA_MEMBER_IN );
	scque_put1byte( que, clientID );
	scque_put1byte( que, memberIdx );
}
static inline void SCQUE_READ_DATA_MemberIn( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
}

//---------------------------------------------
static inline void SCQUE_PUT_ACT_WazaDamage( BTL_SERVER_CMD_QUE* que, u8 atClientID, u8 defClientID, u16 damage, u8 wazaIdx, u8 affinity )
{
	scque_put2byte( que, SC_ACT_WAZA_DMG );
	scque_put1byte( que, atClientID );
	scque_put1byte( que, defClientID );
	scque_put2byte( que, damage );
	scque_put1byte( que, wazaIdx );
	scque_put1byte( que, affinity );
}
static inline void SCQUE_READ_ACT_WazaDamage( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read2byte( que );
	args[3] = scque_read1byte( que );
	args[4] = scque_read1byte( que );
}
static inline void SCQUE_PUT_ACT_RankUp( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 statusType, u8 volume )
{
	scque_put2byte( que, SC_ACT_RANKUP );
	scque_put1byte( que, clientID );
	scque_put1byte( que, statusType );
	scque_put1byte( que, volume );
}
static inline void SCQUE_READ_ACT_RankUp( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read1byte( que );
}
static inline void SCQUE_PUT_ACT_RankDown( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 statusType, u8 volume )
{
	scque_put2byte( que, SC_ACT_RANKDOWN );
	scque_put1byte( que, clientID );
	scque_put1byte( que, statusType );
	scque_put1byte( que, volume );
}
static inline void SCQUE_READ_ACT_RankDown( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
	args[2] = scque_read1byte( que );
}

static inline void SCQUE_PUT_ACT_Dead( BTL_SERVER_CMD_QUE* que, u8 clientID )
{
	scque_put2byte( que, SC_ACT_DEAD );
	scque_put1byte( que, clientID );
}
static inline void SCQUE_READ_ACT_Dead( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
}

static inline void SCQUE_PUT_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 pokeIdx )
{
	scque_put2byte( que, SC_ACT_MEMBER_IN );
	scque_put1byte( que, clientID );
	scque_put1byte( que, pokeIdx );
}
static inline void SCQUE_READ_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
}



static inline void SCQUE_PUT_TOKWIN_IN( BTL_SERVER_CMD_QUE* que, u8 clientID )
{
	scque_put2byte( que, SC_TOKWIN_IN );
	scque_put1byte( que, clientID );
}
static inline void SCQUE_READ_TOKWIN( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
}
static inline void SCQUE_PUT_TOKWIN_OUT( BTL_SERVER_CMD_QUE* que, u8 clientID )
{
	scque_put2byte( que, SC_TOKWIN_OUT );
	scque_put1byte( que, clientID );
}
//=====================================================
typedef u8 ScMsgArg;
enum { 
	MSGARG_TERMINATOR = 0xff,
};

#include <stdarg.h>

// ��������͂��������
extern void SCQUE_PUT_MsgSpecial( BTL_SERVER_CMD_QUE* que, u16 strID, u8 arg1, u8 arg2 );
extern void SCQUE_PUT_MsgBody( BTL_SERVER_CMD_QUE* que, u16 strID, ... );
#define SCQUE_PUT_Msg(que, ...)	SCQUE_PUT_MsgBody(que, __VA_ARGS__, MSGARG_TERMINATOR)

// �� �ϕ����̍ŏ��̈����ɂ͕K��������ID��n���B
extern void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... );
#define SCQUE_PUT_MSG_STD(que, ...)	SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...)	SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )

extern void SCQUE_READ_Msg( BTL_SERVER_CMD_QUE* que, int* args );

static inline void SCQUE_PUT_MSG_WAZA( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 wazaIdx )
{
	scque_put2byte( que, SC_MSG_WAZA );
	scque_put1byte( que, clientID );
	scque_put1byte( que, wazaIdx );
}

static inline void SCQUE_READ_MSG_WAZA( BTL_SERVER_CMD_QUE* que, int* args )
{
	args[0] = scque_read1byte( que );
	args[1] = scque_read1byte( que );
}

//=====================================================

static inline ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args )
{
	ServerCmd cmd = scque_read2byte( que );

	switch( cmd ){
	case SC_OP_HP_MINUS:		SCQUE_READ_OP_HpMinus( que, args ); break;
	case SC_OP_RANK_UP:			SCQUE_READ_OP_RankUp( que, args ); break;
	case SC_OP_RANK_DOWN:		SCQUE_READ_OP_RankDown( que, args ); break;
	case SC_DATA_WAZA_EXE:		SCQUE_READ_DATA_WazaExe( que, args ); break;
	case SC_DATA_MEMBER_IN:		SCQUE_READ_DATA_MemberIn( que, args ); break;
	case SC_ACT_WAZA_DMG:		SCQUE_READ_ACT_WazaDamage( que, args ); break;
	case SC_ACT_RANKUP:			SCQUE_READ_ACT_RankUp( que, args ); break;
	case SC_ACT_RANKDOWN:		SCQUE_READ_ACT_RankDown( que, args ); break;
	case SC_ACT_DEAD:			SCQUE_READ_ACT_Dead( que, args ); break;
	case SC_ACT_MEMBER_IN:		SCQUE_READ_ACT_MemberIn( que, args ); break;
	case SC_TOKWIN_IN:			SCQUE_READ_TOKWIN( que, args ); break;
	case SC_TOKWIN_OUT:			SCQUE_READ_TOKWIN( que, args ); break;
	case SC_MSG_STD:			SCQUE_READ_Msg( que, args ); break;
	case SC_MSG_SET:			SCQUE_READ_Msg( que, args ); break;
	case SC_MSG_WAZA:			SCQUE_READ_MSG_WAZA( que, args ); break;
	default:
		args[0] = scque_read1byte( que );
		break;
	}

	return cmd;
}

#endif
