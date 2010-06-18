//=============================================================================================
/**
 * @file  btl_server_cmd.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h�����C���ߏ���
 * @author  taya
 *
 * @date  2008.10.30  �쐬
 */
//=============================================================================================
#include <stdarg.h>

#include "btl_common.h"
#include "btl_server_cmd.h"


//--------------------------------------------------------------
/**
 *    �T�[�o�R�}���h�����^�����}�N��
 */
//--------------------------------------------------------------
// �����̐�, �����̐����Ƃ̃C���f�b�N�X:����4bit
#define SC_ARGFMT(cnt, idx)   ((((idx)&0x0f)<<4)|(cnt&0x0f))

// �����^��������̐����擾
static inline SC_ARGFMT_GetArgCount( u8 fmt )
{
  return fmt & 0x0f;
}

//--------------------------------------------------------------
/**
 *    �T�[�o�R�}���h�����^��`�i�����̌����Ƃ�16�p�^�[���܂ő��₹��j
 */
//--------------------------------------------------------------
typedef enum {
  // �����P�̌^
  SC_ARGFMT_1byte = SC_ARGFMT(1,0),
  SC_ARGFMT_2byte = SC_ARGFMT(1,1),

  // �����Q�̌^
  SC_ARGFMT_11byte = SC_ARGFMT(2,0),
  SC_ARGFMT_12byte = SC_ARGFMT(2,1),
  SC_ARGFMT_14byte = SC_ARGFMT(2,2),
  SC_ARGFMT_44bit  = SC_ARGFMT(2,3),
  SC_ARGFMT_53bit  = SC_ARGFMT(2,4),


  // �����R�̌^
  SC_ARGFMT_53bit_1byte = SC_ARGFMT(3,0),
  SC_ARGFMT_53bit_2byte = SC_ARGFMT(3,1),
  SC_ARGFMT_555bit      = SC_ARGFMT(3,2),
  SC_ARGFMT_5_5_14bit   = SC_ARGFMT(3,3),
  SC_ARGFMT_112byte     = SC_ARGFMT(3,4),
  SC_ARGFMT_114byte     = SC_ARGFMT(3,5),

  // �����S�̌^
  SC_ARGFMT_53bit_12byte    = SC_ARGFMT(4,0),
  SC_ARGFMT_5353bit         = SC_ARGFMT(4,1),
  SC_ARGFMT_5_5_14bit_1byte = SC_ARGFMT(4,2),
  SC_ARGFMT_5_5_14bit_2byte = SC_ARGFMT(4,3),
  SC_ARGFMT_5_5_6_2byte     = SC_ARGFMT(4,4),
  SC_ARGFMT_1122byte        = SC_ARGFMT(4,5),

  // �����T�̌^
  SC_ARGFMT_555bit_22byte    = SC_ARGFMT(5,0),
  SC_ARGFMT_5_3_7_1bit_2byte = SC_ARGFMT(5,1),
  SC_ARGFMT_3311bit_2byte    = SC_ARGFMT(5,2),

  // �����U�̌^
  SC_ARGFMT_555555bit         = SC_ARGFMT(6,0),
  SC_ARGFMT_32111bit_2byte    = SC_ARGFMT(6,1),
  SC_ARGFMT_5555bit_22byte    = SC_ARGFMT(6,2),

  // �����W�̌^
  SC_ARGFMT_1x8byte = SC_ARGFMT(8,0),

  // ���b�Z�[�W�^�i�ψ����j
  SC_ARGFMT_MSG    = SC_ARGFMT(0,0),
  SC_ARGFMT_MSG_SE = SC_ARGFMT(0,1),
  SC_ARGFMT_POINT  = SC_ARGFMT(0,2),

}ScArgFormat;




//--------------------------------------------------------------
/**
 *    �T�[�o�R�}���h��������̌^���擾���邽�߂̃e�[�u��
 *    �i�T�[�o�R�}���h�Ə�������v������K�v������j
 */
//--------------------------------------------------------------
static const u8 ServerCmdToFmtTbl[] = {
  0,
  SC_ARGFMT_12byte,           // SC_OP_HP_MINUS
  SC_ARGFMT_12byte,           // SC_OP_HP_PLUS
  SC_ARGFMT_1byte,            // SC_OP_HP_ZERO
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_MINUS
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_MINUS_ORG
  SC_ARGFMT_53bit,            // SC_OP_WAZA_USED
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_PLUS
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_PLUS_ORG
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_UP
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_DOWN
  SC_ARGFMT_1x8byte,          // SC_OP_RANK_SET7
  SC_ARGFMT_1byte,            // SC_OP_RANK_RECOVER,
  SC_ARGFMT_1byte,            // SC_OP_RANK_RESET,
  SC_ARGFMT_14byte,           // SC_OP_ADD_CRITICAL
  SC_ARGFMT_114byte,          // SC_OP_SICK_SET
  SC_ARGFMT_1byte,            // SC_OP_CURE_POKESICK
  SC_ARGFMT_12byte,           // SC_OP_CURE_WAZASICK
  SC_ARGFMT_53bit_12byte,     // SC_OP_MEMBER_IN
  SC_ARGFMT_5_5_14bit,        // SC_OP_SET_STATUS
  SC_ARGFMT_12byte,           // SC_OP_SET_WEIGHT
  SC_ARGFMT_1byte,            // SC_OP_WAZASICK_TURNCHECK
  SC_ARGFMT_12byte,           // SC_OP_CHANGE_POKETYPE
  SC_ARGFMT_12byte,           // SC_OP_CONSUME_ITEM
  SC_ARGFMT_5555bit_22byte,   // SC_OP_UPDATE_USE_WAZA,
  SC_ARGFMT_11byte,           // SC_OP_SET_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_RESET_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_SET_TURNFLAG
  SC_ARGFMT_11byte,           // SC_OP_RESET_TURNFLAG
  SC_ARGFMT_12byte,           // SC_OP_CHANGE_TOKUSEI
  SC_ARGFMT_12byte,           // SC_OP_SET_ITEM
  SC_ARGFMT_5_3_7_1bit_2byte, // SC_OP_UPDATE_WAZANUMBER
  SC_ARGFMT_1byte,            // SC_OP_OUTCLEAR
  SC_ARGFMT_14byte,           // SC_OP_ADD_FLDEFF
  SC_ARGFMT_11byte,           // SC_OP_ADD_FLDEFF_DEPEND
  SC_ARGFMT_1byte,            // SC_OP_DEL_FLDEFF_DEPEND
  SC_ARGFMT_1byte,            // SC_OP_REMOVE_FLDEFF
  SC_ARGFMT_5_5_14bit,        // SC_OP_SET_POKE_COUNTER
  SC_ARGFMT_11byte,           // SC_OP_BATONTOUCH
  SC_ARGFMT_12byte,           // SC_OP_MIGAWARI_CREATE
  SC_ARGFMT_1byte,            // SC_OP_MIGAWARI_DELETE
  SC_ARGFMT_44bit,            // SC_OP_SHOOTER_CHARGE
  SC_ARGFMT_1byte,            // SC_OP_SET_ILLUSION
  SC_ARGFMT_1byte,            // SC_OP_CLEAR_CONSUMED_ITEM
  SC_ARGFMT_11byte,           // SC_OP_CURESICK_DEPEND_POKE
  SC_ARGFMT_5555bit_22byte,   // SC_OP_WAZADMG_REC
  SC_ARGFMT_1byte,            // SC_OP_TURN_CHECK
  SC_ARGFMT_5_5_14bit_1byte,  // SC_ACT_WAZA_EFFECT
  SC_ARGFMT_53bit,            // SC_ACT_TAMEWAZA_HIDE
  SC_ARGFMT_112byte,          // SC_ACT_WAZA_DMG
  SC_ARGFMT_112byte,          // SC_ACT_WAZA_DMG_PLURAL
  SC_ARGFMT_1byte,            // SC_ACT_WAZA_ICHIGEKI
  SC_ARGFMT_11byte,           // SC_ACT_SICK_ICON
  SC_ARGFMT_1byte,            // SC_ACT_CONF_DMG
  SC_ARGFMT_555bit,           // SC_ACT_RANKUP
  SC_ARGFMT_555bit,           // SC_ACT_RANKDOWN
  SC_ARGFMT_1byte,            // SC_ACT_DEAD
  SC_ARGFMT_1byte,            // SC_ACT_RELIVE
  SC_ARGFMT_11byte,           // SC_ACT_MEMBER_OUT_MSG
  SC_ARGFMT_12byte,           // SC_ACT_MEMBER_OUT
  SC_ARGFMT_5353bit,          // SC_ACT_MEMBER_IN
  SC_ARGFMT_44bit,            // SC_ACT_WEATHER_DMG,
  SC_ARGFMT_1byte,            // SC_ACT_WEATHER_START,
  SC_ARGFMT_1byte,            // SC_ACT_WEATHER_END,
  SC_ARGFMT_1byte,            // SC_ACT_SIMPLE_HP
  SC_ARGFMT_1byte,            // SC_ACT_KINOMI
  SC_ARGFMT_53bit,            // SC_ACT_KILL
  SC_ARGFMT_555bit,           // SC_ACTOP_MOVE
  SC_ARGFMT_14byte,           // SC_ACT_EXP
  SC_ARGFMT_32111bit_2byte,   // SC_ACT_BALL_THROW
  SC_ARGFMT_12byte,           // SC_ACT_BALL_THROW_TR
  SC_ARGFMT_44bit,            // SC_ACT_ROTATION
  SC_ARGFMT_12byte,           // SC_ACTOP_CHANGE_TOKUSEI
  SC_ARGFMT_1122byte,         // SC_ACTOP_SWAP_TOKUSEI
  SC_ARGFMT_1byte,            // SC_ACT_FAKE_DISABLE
  SC_ARGFMT_2byte,            // SC_ACT_EFFECT_SIMPLE
  SC_ARGFMT_12byte,           // SC_ACT_EFFECT_BYPOS
  SC_ARGFMT_112byte,          // SC_ACT_EFFECT_BYVECTOR
  SC_ARGFMT_11byte,           // SC_ACT_CHANGE_FORM
  SC_ARGFMT_5353bit,          // SC_ACT_RESET_MOVE
  SC_ARGFMT_1byte,            // SC_ACT_MIGAWARI_CREATE
  SC_ARGFMT_1byte,            // SC_ACT_MIGAWARI_DELETE
  SC_ARGFMT_11byte,           // SC_ACT_HENSIN,
  SC_ARGFMT_53bit_2byte,      // SC_ACT_MIGAWARI_DAMAGE
  SC_ARGFMT_2byte,            // SC_ACT_WIN_BGM
  SC_ARGFMT_1byte,            // SC_ACT_MSGWIN_HIDE
  SC_ARGFMT_1byte,            // SC_TOKWIN_IN
  SC_ARGFMT_1byte,            // SC_TOKWIN_OUT
  SC_ARGFMT_12byte, // SC_MSG_WAZA
  SC_ARGFMT_MSG,    // SC_MSG_STD
  SC_ARGFMT_MSG,    // SC_MSG_SET
  SC_ARGFMT_MSG_SE, // SC_MSG_STD_SE
};

//------------------------------------------------------------------------
// globals
//------------------------------------------------------------------------
static int ArgBuffer[ 8 ];  // �ό�����S�Ċi�[���邽�߂̈ꎞ�o�b�t�@

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline SC_ARGFMT_GetArgCount( u8 fmt );
static inline u8 pack1_2args( int arg1, int arg2, int bits1, int bits2 );
static inline void unpack1_2args( u8 pack, int bits1, int bits2, int* args, int idx_start );
static inline u32 pack_3args( int bytes, int arg1, int arg2, int arg3, int bits1, int bits2, int bits3 );
static inline void unpack_3args( int bytes, u32 pack, int bits1, int bits2, int bits3, int* args, int idx_start );
static void put_core( BTL_SERVER_CMD_QUE* que, ServerCmd cmd, ScArgFormat fmt, const int* args );
static void read_core( BTL_SERVER_CMD_QUE* que, ScArgFormat fmt, int* args );
static void read_core_msg( BTL_SERVER_CMD_QUE* que, u8 scType, int* args );



static inline u8 pack1_2args( int arg1, int arg2, int bits1, int bits2 )
{
  GF_ASSERT(bits1+bits2<=8);
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  {
    u8 mask1, mask2;

    mask1 = (1 << bits1) - 1;
    mask2 = (1 << bits2) - 1;

    GF_ASSERT( arg1 <= mask1 );
    GF_ASSERT( arg2 <= mask2 );

    return ((arg1 & mask1) << bits2) | (arg2 & mask2);
  }
}
static inline void unpack1_2args( u8 pack, int bits1, int bits2, int* args, int idx_start )
{
  GF_ASSERT(bits1+bits2<=8);
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);

  {
    u8 mask1 = (1 << bits1) - 1;
    u8 mask2 = (1 << bits2) - 1;

    args[ idx_start++ ] = (pack >> bits2) & mask1;
    args[ idx_start ] =  pack & mask2;
  }
}

static inline u32 pack_3args( int bytes, int arg1, int arg2, int arg3, int bits1, int bits2, int bits3 )
{
  GF_ASSERT(bits1+bits2+bits3<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;

    GF_ASSERT(arg1 <= mask1);
    GF_ASSERT(arg2 <= mask2);
    GF_ASSERT(arg3 <= mask3);

    return ( ((arg1&mask1)<<(bits2+bits3)) | ((arg2&mask2)<<bits3) | (arg3&mask3) );
  }
}
static inline void unpack_3args( int bytes, u32 pack, int bits1, int bits2, int bits3, int* args, int idx_start )
{
  GF_ASSERT(bits1+bits2+bits3<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3)) & mask1;
    args[ idx_start++ ] = (pack >> bits3) & mask2;
    args[ idx_start ] = pack & mask3;
  }
}

static inline u32 pack_4args( int bytes, int arg1, int arg2, int arg3, int arg4, int bits1, int bits2, int bits3, int bits4 )
{
  GF_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  GF_ASSERT(bits4!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;

    GF_ASSERT(arg1 <= mask1);
    GF_ASSERT(arg2 <= mask2);
    GF_ASSERT(arg3 <= mask3);
    GF_ASSERT(arg4 <= mask4);

    return ( ((arg1&mask1)<<(bits2+bits3+bits4)) | ((arg2&mask2)<<(bits3+bits4)) | ((arg3&mask3)<<bits4) | (arg4&mask4) );
  }
}
static inline void unpack_4args( int bytes, u32 pack, int bits1, int bits2, int bits3, int bits4, int* args, int idx_start )
{
  GF_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  GF_ASSERT(bits4!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3+bits4)) & mask1;
    args[ idx_start++ ] = (pack >> (bits3+bits4)) & mask2;
    args[ idx_start++ ] = (pack >> (bits4)) & mask3;
    args[ idx_start ] = pack & mask4;
  }
}

static inline u32 pack_5args( int bytes, int arg1, int arg2, int arg3, int arg4, int arg5, int bits1, int bits2, int bits3, int bits4, int bits5 )
{
  GF_ASSERT(bits1+bits2+bits3+bits4+bits5<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  GF_ASSERT(bits4!=0);
  GF_ASSERT(bits5!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;
    u16 mask5 = (1 << bits5) - 1;

    GF_ASSERT(arg1 <= mask1);
    GF_ASSERT(arg2 <= mask2);
    GF_ASSERT(arg3 <= mask3);
    GF_ASSERT(arg4 <= mask4);
    GF_ASSERT(arg5 <= mask5);

    return ( ((arg1&mask1)<<(bits2+bits3+bits4+bits5)) | ((arg2&mask2)<<(bits3+bits4+bits5)) |
              ((arg3&mask3)<<bits4+bits5) | ((arg4&mask4)<<bits5) | (arg5&mask5) );
  }
}
static inline void unpack_5args( int bytes, u32 pack, int bits1, int bits2, int bits3, int bits4, int bits5, int* args, int idx_start )
{
  GF_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GF_ASSERT(bits1!=0);
  GF_ASSERT(bits2!=0);
  GF_ASSERT(bits3!=0);
  GF_ASSERT(bits4!=0);
  GF_ASSERT(bits5!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;
    u16 mask5 = (1 << bits5) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3+bits4+bits5)) & mask1;
    args[ idx_start++ ] = (pack >> (bits3+bits4+bits5)) & mask2;
    args[ idx_start++ ] = (pack >> (bits4+bits5)) & mask3;
    args[ idx_start++ ] = (pack >> (bits5)) & mask4;
    args[ idx_start ] = pack & mask5;
  }
}

//---------------------------------------------------------------------------------------
/**
 * Que Read/Write
 */
//---------------------------------------------------------------------------------------

void scque_put1byte( BTL_SERVER_CMD_QUE* que, u8 data )
{
  GF_ASSERT(que->writePtr < BTL_SERVER_CMD_QUE_SIZE);
  que->buffer[ que->writePtr++ ] = data;
}
u8 scque_read1byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < que->writePtr);
  return que->buffer[ que->readPtr++ ];
}
void scque_put2byte( BTL_SERVER_CMD_QUE* que, u16 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-1));
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u16 scque_read2byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT_MSG(que->readPtr < (que->writePtr-1), "rp=%d, wp=%d", que->readPtr, que->writePtr);
  {
    u16 data = ( (que->buffer[que->readPtr] << 8) | que->buffer[que->readPtr+1] );
    que->readPtr += 2;
    return data;
  }
}
void scque_put3byte( BTL_SERVER_CMD_QUE* que, u32 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-2));
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u32 scque_read3byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < (que->writePtr-2));
  {
    u32 data = ( (que->buffer[que->readPtr]<<16) | (que->buffer[que->readPtr+1]<<8) | (que->buffer[que->readPtr+2]) );
    que->readPtr += 3;
    return data;
  }
}
void scque_put4byte( BTL_SERVER_CMD_QUE* que, u32 data )
{
  GF_ASSERT(que->writePtr < (BTL_SERVER_CMD_QUE_SIZE-3));
  que->buffer[ que->writePtr++ ] = (data >> 24)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u32 scque_read4byte( BTL_SERVER_CMD_QUE* que )
{
  GF_ASSERT(que->readPtr < (que->writePtr-3));
  {
    u32 data = ( (que->buffer[que->readPtr]<<24) | (que->buffer[que->readPtr+1]<<16)
               | (que->buffer[que->readPtr+2]<<8) | (que->buffer[que->readPtr+3]) );
    que->readPtr += 4;
    return data;
  }
}


//--------------------------------------------------------------------------
/**
 * �R�}���h�i���o�A�����^�ɉ����đS������QUE�Ɋi�[
 *
 * @param   que
 * @param   cmd
 * @param   fmt
 * @param   args
 *
 */
//--------------------------------------------------------------------------
static void put_core( BTL_SERVER_CMD_QUE* que, ServerCmd cmd, ScArgFormat fmt, const int* args )
{
  scque_put2byte( que, cmd );

  switch( fmt ) {
  case SC_ARGFMT_1byte:
    scque_put1byte( que, args[0] );
    break;
  case SC_ARGFMT_2byte:
    scque_put2byte( que, args[0] );
    break;
  case SC_ARGFMT_11byte:
    scque_put1byte( que, args[0] );
    scque_put1byte( que, args[1] );
    break;
  case SC_ARGFMT_12byte:
    scque_put1byte( que, args[0] );
    scque_put2byte( que, args[1] );
    break;
  case SC_ARGFMT_14byte:
    scque_put1byte( que, args[0] );
    scque_put4byte( que, args[1] );
    break;
  case SC_ARGFMT_44bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 4, 4) );
    break;
  case SC_ARGFMT_53bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    break;
  case SC_ARGFMT_53bit_1byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, args[2] );
    break;
  case SC_ARGFMT_53bit_2byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put2byte( que, args[2] );
    break;
  case SC_ARGFMT_555bit:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      scque_put2byte( que, pack );
    }
    break;
  case SC_ARGFMT_5_5_14bit:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
    }
    break;
  case SC_ARGFMT_112byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put2byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_114byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put4byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_5353bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, pack1_2args(args[2], args[3], 5, 3) );
    break;
  case SC_ARGFMT_53bit_12byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, args[2] );
    scque_put2byte( que, args[3] );
    break;
  case SC_ARGFMT_5_5_14bit_1byte:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
      scque_put1byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_5_5_14bit_2byte:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_5_5_6_2byte:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,6 );
      scque_put2byte( que, pack );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_1122byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put2byte( que, args[2] );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_555bit_22byte:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      scque_put2byte( que, pack );
      scque_put2byte( que, args[3] );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_5_3_7_1bit_2byte:
    {
      u16 pack1 = pack1_2args( args[0], args[1], 5, 3 );
      u16 pack2 = pack1_2args( args[2], args[3], 7, 1 );
      scque_put2byte( que, pack1 );
      scque_put2byte( que, pack2 );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_3311bit_2byte:
    {
      u8 pack = pack_4args( 1, args[0], args[1], args[2], args[3], 3, 3, 1, 1 );
      scque_put1byte( que, pack );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      u16 pack2 = pack_3args( 2, args[3],args[4],args[5], 5,5,5 );
      scque_put2byte( que, pack1 );
      scque_put2byte( que, pack2 );
    }
    break;
  case SC_ARGFMT_32111bit_2byte:
    {
      u8 pack = pack_5args( 1, args[0], args[1], args[2], args[3], args[4], 3, 2, 1, 1, 1 );
      scque_put1byte( que, pack );
      scque_put2byte( que, args[5] );
    }
    break;
  case SC_ARGFMT_5555bit_22byte:
    {
      u32 pack = pack_4args( 3, args[0],args[1],args[2],args[3], 5,5,5,5 );
      scque_put3byte( que, pack );
      scque_put2byte( que, args[4] );
      scque_put2byte( que, args[5] );
    }
    break;
  case SC_ARGFMT_1x8byte:
    {
      int i;
      for(i=0; i<8; ++i){
        scque_put1byte( que, args[i] );
      }
    }
    break;

  case SC_ARGFMT_POINT:
    break;
  default:
    GF_ASSERT(0);
    break;
  }/* switch(fmt) */
}
//--------------------------------------------------------------------------
/**
 * �����^�ɉ����đS������z��ɓǂݏo��
 *
 * @param   que
 * @param   fmt
 * @param   args
 *
 */
//--------------------------------------------------------------------------
static void read_core( BTL_SERVER_CMD_QUE* que, ScArgFormat fmt, int* args )
{
  switch( fmt ) {
  case SC_ARGFMT_1byte:
    args[0] = scque_read1byte( que );
    break;
  case SC_ARGFMT_2byte:
    args[0] = scque_read2byte( que );
    break;
  case SC_ARGFMT_11byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read1byte( que );
    break;
  case SC_ARGFMT_12byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read2byte( que );
    break;
  case SC_ARGFMT_14byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read4byte( que );
    break;
  case SC_ARGFMT_44bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 4, 4, args, 0 );
    }
    break;
  case SC_ARGFMT_53bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
    }
    break;
  case SC_ARGFMT_53bit_1byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_53bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_555bit:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5,5,5, args, 0 );
    }
    break;
  case SC_ARGFMT_5_5_14bit:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
    }
    break;
  case SC_ARGFMT_112byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_114byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read4byte( que );
    }
    break;
  case SC_ARGFMT_5353bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 2 );
    }
    break;
  case SC_ARGFMT_53bit_12byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read1byte( que );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_5_14bit_1byte:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
      args[3] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_5_5_14bit_2byte:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_5_6_2byte:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5,5,6, args, 0 );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_1122byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read2byte( que );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_555bit_22byte:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5, 5, 5, args, 0 );
      args[3] = scque_read2byte( que );
      args[4] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_3_7_1bit_2byte:
    {
      u16 pack1 = scque_read2byte( que );
      u16 pack2 = scque_read2byte( que );
      unpack1_2args( pack1, 5, 3, args, 0 );
      unpack1_2args( pack2, 7, 1, args, 2 );
      args[4] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_3311bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack_4args( 1, pack, 3, 3, 1, 1, args, 0 );
      args[4] = scque_read2byte( que );
    }
    break;

  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = scque_read2byte( que );
      u16 pack2 = scque_read2byte( que );
      unpack_3args( 2, pack1, 5, 5, 5, args, 0 );
      unpack_3args( 2, pack2, 5, 5, 5, args, 3 );
    }
    break;

  case SC_ARGFMT_32111bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack_5args( 1, pack, 3, 2, 1, 1, 1, args, 0 );
      args[5] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5555bit_22byte:
    {
      u32 pack = scque_read3byte( que );

      unpack_4args( 3, pack, 5,5,5,5, args, 0 );
      args[4] = scque_read2byte( que );
      args[5] = scque_read2byte( que );
    }
    break;

  case SC_ARGFMT_1x8byte:
    {
      int i;
      for(i=0; i<8; ++i){
        args[i] = scque_read1byte( que );
      }
    }
    break;

  case SC_ARGFMT_POINT:
    break;
  default:
    GF_ASSERT(0);
    break;
  }/* switch(fmt) */
}


void SCQUE_PUT_Common( BTL_SERVER_CMD_QUE* que, ServerCmd cmd, ... )
{
  GF_ASSERT( cmd < NELEMS(ServerCmdToFmtTbl) );

  {
    va_list   list;
    u32 i, arg_cnt;
    u8 fmt;

    fmt = ServerCmdToFmtTbl[ cmd ];
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );

    GF_ASSERT(arg_cnt <= NELEMS(ArgBuffer));

    va_start( list, cmd );
    for(i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    #if 1
    BTL_N_Printf( DBGSTR_SC_PutCmd, cmd, fmt, arg_cnt);
    for(i=0; i<arg_cnt; ++i){
      BTL_N_PrintfSimple( DBGSTR_csv, ArgBuffer[i]);
    }
    BTL_N_PrintfSimple(DBGSTR_LF);
    #endif

    put_core( que, cmd, fmt, ArgBuffer );
  }
}

//=============================================================================================
/**
 * �L���[�������݂̌��݈ʒu��\�񂷂�
 *
 * @param   que
 * @param   cmd   �������݃R�}���h
 *
 * @retval  u16   ���݈ʒu
 */
//=============================================================================================
u16 SCQUE_RESERVE_Pos( BTL_SERVER_CMD_QUE* que, ServerCmd cmd )
{
  GF_ASSERT_MSG( cmd < NELEMS(ServerCmdToFmtTbl), "cmd=%d\n", cmd );
  {
    u16 pos;
    u8  fmt, arg_cnt, reserve_size, i;

    fmt = ServerCmdToFmtTbl[ cmd ];
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );
    GF_ASSERT(arg_cnt <= NELEMS(ArgBuffer));

    for(i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = 0;
    }

    pos = que->writePtr;
    put_core( que, cmd, fmt, ArgBuffer );
    reserve_size = que->writePtr - pos;
    GF_ASSERT(reserve_size >= 3);

    que->writePtr = pos;
    scque_put2byte( que, SCEX_RESERVE );
    scque_put1byte( que, reserve_size-3 );

    que->writePtr = pos + reserve_size;

    BTL_N_Printf( DBGSTR_SC_ReservedPos, pos, que->writePtr);

    return pos;
  }
}

void SCQUE_PUT_ReservedPos( BTL_SERVER_CMD_QUE* que, u16 pos, ServerCmd cmd, ... )
{
  GF_ASSERT( cmd < NELEMS(ServerCmdToFmtTbl) );
  {
    va_list   list;
    u32 i, arg_cnt;
    u16 reserved_cmd, reserved_size;
    u8 fmt;

    fmt = ServerCmdToFmtTbl[ cmd ];
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );
    GF_ASSERT(arg_cnt <= NELEMS(ArgBuffer));

    va_start( list, cmd );
    for(i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    BTL_N_Printf( DBGSTR_SC_WriteReservedPos, pos, cmd );
    if( arg_cnt )
    {
      BTL_N_Printf( DBGSTR_SC_ArgsEqual );
      for( i=0; i<arg_cnt; ++i ){
        BTL_N_PrintfSimple(DBGSTR_csv, ArgBuffer[i]);
      }
    }
    BTL_N_PrintfSimple(DBGSTR_LF);

    {
      u16 default_read_pos = que->readPtr;
      que->readPtr = pos;
      reserved_cmd = scque_read2byte( que );
      reserved_size = scque_read1byte( que );
      que->readPtr = default_read_pos;
    }
    if( cmd != SCEX_RESERVE )
    {
      u8 wrote_size;
      u16 default_write_pos = que->writePtr;
      que->writePtr = pos;
      put_core( que, cmd, fmt, ArgBuffer );
      wrote_size = (que->writePtr - pos - 3);
      GF_ASSERT(wrote_size == reserved_size);
      que->writePtr = default_write_pos;
    }
  }
}

ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args )
{
  enum {
    PRINT_FLAG = FALSE,
  };

  ServerCmd cmd = scque_read2byte( que );

  BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ReadCmd, cmd);

  // �\��̈�ɉ����������܂�Ȃ������ꍇ�͒P���ɃX�L�b�v����
  while( cmd == SCEX_RESERVE )
  {
    u8 reserve_size = scque_read1byte( que );
    que->readPtr += reserve_size;
    cmd = scque_read2byte( que );
    BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ReserveCmd, cmd);
  }

  GF_ASSERT_MSG( cmd < NELEMS(ServerCmdToFmtTbl), "cmd=%d\n", cmd );

  {
    u8 fmt = ServerCmdToFmtTbl[ cmd ];

    if( (fmt != SC_ARGFMT_MSG) && (fmt != SC_ARGFMT_MSG_SE) )
    {
      read_core( que, fmt, args );
      #ifdef PM_DEBUG
      {
        u8 arg_cnt = SC_ARGFMT_GetArgCount( fmt );
        u8 i;
        BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ArgsEqual );

        for(i=0; i<arg_cnt; ++i){
          BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_csv, args[i]);
        }
        BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_LF );
      }
      #endif
    }
    else
    {
      read_core_msg( que, cmd, args );
    }
  }
  return cmd;
}

//=============================================================================================
/**
 * �P�o�C�g�P�ʂň������������݁i����R�}���h�ɉ����ĉψ����ɑΏ����邽�߂̑[�u�j
 *
 * @param   que
 * @param   arg
 *
 */
//=============================================================================================
void SCQUE_PUT_ArgOnly( BTL_SERVER_CMD_QUE* que, u8 arg )
{
  scque_put1byte( que, arg );
}

//=============================================================================================
/**
 * �P�o�C�g�P�ʂœn���ꂽ������ǂݏo��
 *
 * @param   que
 *
 */
//=============================================================================================
u8 SCQUE_READ_ArgOnly( BTL_SERVER_CMD_QUE* que )
{
  return scque_read1byte( que );
}



void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... )
{
  enum {
    PRINT_FLAG = FALSE,
  };

  {
    va_list   list;
    ScMsgArg   arg;
    u16 strID;

    va_start( list, scType );
    strID = va_arg( list, int );

    scque_put2byte( que, scType );
    scque_put2byte( que, strID );

    BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_PutMsgParam, scType, strID );

    if( scType == SC_MSG_STD_SE ){
      u16 seID = va_arg( list, int );
      scque_put2byte( que, seID );
      BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_SC_PutMsg_SE, seID );
    }
    BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_SC_ArgsEqual );

    do {
      arg = va_arg( list, int );
      if( arg != MSGARG_TERMINATOR ){
        BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_csv, arg );
      }
      scque_put4byte( que, arg );
    }while( arg != MSGARG_TERMINATOR );
    BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_LF );

    va_end( list );
  }
}

static void read_core_msg( BTL_SERVER_CMD_QUE* que, u8 scType, int* args )
{
  enum {
    PRINT_FLAG = FALSE,
  };

  int idx_begin = 1;

  args[0] = scque_read2byte( que );

  BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ReadMsgParam, scType, args[0] );

  if( scType == SC_MSG_STD_SE ){
    args[1] = scque_read2byte( que );
    BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_PutMsg_SE, args[1] );
    ++idx_begin;
  }
  BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_SC_ArgsEqual );

  {
    int i = idx_begin;
    for(i=idx_begin; i<BTL_SERVERCMD_ARG_MAX; ++i){
      args[i] = scque_read4byte( que );
      if( args[i] == MSGARG_TERMINATOR ){
        break;
      }
      BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_csv, args[i] );
    }
    BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_LF );

    if( i == BTL_SERVERCMD_ARG_MAX ){
      GF_ASSERT(0); // �����g������
    }
  }
}


