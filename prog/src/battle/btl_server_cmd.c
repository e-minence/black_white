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
  SC_ARGFMT_4_4bit = SC_ARGFMT(2,2),
  SC_ARGFMT_5_3bit = SC_ARGFMT(2,3),

  // �����R�̌^
  SC_ARGFMT_53bit_1byte = SC_ARGFMT(3,0),
  SC_ARGFMT_5_5_5bit    = SC_ARGFMT(3,1),
  SC_ARGFMT_5_5_14bit   = SC_ARGFMT(3,2),
  SC_ARGFMT_112byte     = SC_ARGFMT(3,3),
  SC_ARGFMT_114byte     = SC_ARGFMT(3,4),

  // �����S�̌^
  SC_ARGFMT_53bit_12byte = SC_ARGFMT(4,0),
  SC_ARGFMT_5_5_14bit_1byte = SC_ARGFMT(4,1),

  // �����T�̌^
  SC_ARGFMT_5_5_5bit_22byte = SC_ARGFMT(5,0),
  SC_ARGFMT_5_3_7_1bit_2byte = SC_ARGFMT(5,1),

  // �����U�̌^
  SC_ARGFMT_555555bit = SC_ARGFMT(6,0),

  // ���b�Z�[�W�^�i�ψ����j
  SC_ARGFMT_MSG   = SC_ARGFMT(0,0),
  SC_ARGFMT_POINT = SC_ARGFMT(0,1),

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
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_PLUS
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_UP
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_DOWN
  SC_ARGFMT_555555bit,        // SC_OP_RANK_SET5
  SC_ARGFMT_114byte,          // SC_OP_SICK_SET
  SC_ARGFMT_1byte,            // SC_OP_CURE_POKESICK
  SC_ARGFMT_12byte,           // SC_OP_CURE_WAZASICK
  SC_ARGFMT_53bit_12byte,     // SC_OP_MEMBER_IN
  SC_ARGFMT_1byte,            // SC_OP_WAZASICK_TURNCHECK
  SC_ARGFMT_5_3bit,           // SC_OP_CANTESCAPE_ADD
  SC_ARGFMT_5_3bit,           // SC_OP_CANTESCAPE_SUB
  SC_ARGFMT_12byte,           // SC_OP_CHANGE_POKETYPE
  SC_ARGFMT_11byte,           // SC_OP_CHANGE_POKEFORM
  SC_ARGFMT_1byte,            // SC_OP_REMOVE_ITEM
  SC_ARGFMT_112byte,          // SC_OP_UPDATE_USE_WAZA,
  SC_ARGFMT_1byte,            // SC_OP_RESET_USED_WAZA
  SC_ARGFMT_11byte,           // SC_OP_SET_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_CLEAR_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_SET_ACTFLAG
  SC_ARGFMT_1byte,            // SC_OP_CLEAR_ACTFLAG
  SC_ARGFMT_11byte,           // SC_OP_SET_TURNFLAG
  SC_ARGFMT_11byte,           // SC_OP_RESET_TURNFLAG
  SC_ARGFMT_12byte,           // SC_OP_CHANGE_TOKUSEI
  SC_ARGFMT_12byte,           // SC_OP_SET_ITEM
  SC_ARGFMT_5_3_7_1bit_2byte, // SC_OP_UPDATE_WAZANUMBER
  SC_ARGFMT_11byte,           // SC_OP_HENSIN,
  SC_ARGFMT_1byte,            // SC_OP_OUTCLEAR
  SC_ARGFMT_12byte,           // SC_OP_ADD_FLDEFF
  SC_ARGFMT_1byte,            // SC_OP_REMOVE_FLDEFF
  SC_ARGFMT_5_5_14bit,        // SC_OP_SET_POKE_COUNTER
  SC_ARGFMT_11byte,           // SC_OP_BATONTOUCH
  SC_ARGFMT_12byte,           // SC_OP_MIGAWARI_CREATE
  SC_ARGFMT_1byte,            // SC_OP_MIGAWARI_DELETE
  SC_ARGFMT_5_5_14bit,        // SC_ACT_WAZA_EFFECT
  SC_ARGFMT_5_5_14bit_1byte,  // SC_ACT_WAZA_EFFECT_EX
  SC_ARGFMT_5_5_14bit,        // SC_ACT_WAZA_DMG
  SC_ARGFMT_5_5_5bit_22byte,  // SC_ACT_WAZA_DMG_DBL
  SC_ARGFMT_4_4bit,           // SC_ACT_WAZA_DMG_PLURAL
  SC_ARGFMT_1byte,            // 1C_ACT_WAZA_DMG_ICHIGEKI
  SC_ARGFMT_1byte,            // SC_ACT_CONF_DMG
  SC_ARGFMT_5_5_5bit,         // SC_ACT_RANKUP
  SC_ARGFMT_5_5_5bit,         // SC_ACT_RANKDOWN
  SC_ARGFMT_1byte,            // SC_ACT_DEAD
  SC_ARGFMT_1byte,            // SC_ACT_MEMBER_OUT
  SC_ARGFMT_53bit_1byte,      // SC_ACT_MEMBER_IN
  SC_ARGFMT_5_5_14bit,        // SC_ACT_SICK_DMG
  SC_ARGFMT_4_4bit,           // SC_ACT_WEATHER_DMG,
  SC_ARGFMT_1byte,            // SC_ACT_WEATHER_START,
  SC_ARGFMT_1byte,            // SC_ACT_WEATHER_END,
  SC_ARGFMT_1byte,            // SC_ACT_SIMPLE_HP
  SC_ARGFMT_5_5_14bit,        // SC_ACT_TRACE_TOKUSEI
  SC_ARGFMT_1byte,            // SC_ACT_KINOMI
  SC_ARGFMT_5_3bit,           // SC_ACT_KILL
  SC_ARGFMT_1byte,            // SC_TOKWIN_IN
  SC_ARGFMT_1byte,            // SC_TOKWIN_OUT

  SC_ARGFMT_12byte, // SC_MSG_WAZA
  SC_ARGFMT_MSG,    // SC_MSG_STD
  SC_ARGFMT_MSG,    // SC_MSG_SET
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
static void read_core_msg( BTL_SERVER_CMD_QUE* que, int* args );



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

    return ( ((arg1&mask2)<<(bits2+bits3)) | ((arg2&mask2)<<bits3) | (arg3&mask3) );
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
  case SC_ARGFMT_11byte:
    scque_put1byte( que, args[0] );
    scque_put1byte( que, args[1] );
    break;
  case SC_ARGFMT_12byte:
    scque_put1byte( que, args[0] );
    scque_put2byte( que, args[1] );
    break;
  case SC_ARGFMT_4_4bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 4, 4) );
    break;
  case SC_ARGFMT_5_3bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    break;
  case SC_ARGFMT_53bit_1byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, args[2] );
    break;
  case SC_ARGFMT_5_5_5bit:
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
  case SC_ARGFMT_5_5_5bit_22byte:
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
  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      u16 pack2 = pack_3args( 2, args[3],args[4],args[5], 5,5,5 );
      scque_put2byte( que, pack1 );
      scque_put2byte( que, pack2 );
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
  case SC_ARGFMT_11byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read1byte( que );
    break;
  case SC_ARGFMT_12byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read2byte( que );
    break;
  case SC_ARGFMT_4_4bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 4, 4, args, 0 );
    }
    break;
  case SC_ARGFMT_5_3bit:
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
  case SC_ARGFMT_5_5_5bit:
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
  case SC_ARGFMT_5_5_5bit_22byte:
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

  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = scque_read2byte( que );
      u16 pack2 = scque_read2byte( que );
      unpack_3args( 2, pack1, 5, 5, 5, args, 0 );
      unpack_3args( 2, pack2, 5, 5, 5, args, 3 );
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

    va_start( list, cmd );
    for(i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    #ifdef BTL_PRINT_SYSTEM_ENABLE
    BTL_Printf("PutCmd=%d, Format=%02x, argCnt=%d, args=", cmd, fmt, arg_cnt);
    for(i=0; i<arg_cnt; ++i)
    {
      BTL_PrintfSimple("%d,", ArgBuffer[i]);
    }
    BTL_PrintfSimple("\n");
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
  GF_ASSERT( cmd < NELEMS(ServerCmdToFmtTbl) );
  {
    u16 pos;
    u8  fmt, arg_cnt, reserve_size, i;

    fmt = ServerCmdToFmtTbl[ cmd ];
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );

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

    BTL_Printf(" reserved pos=%d, wp=%d\n", pos, que->writePtr);

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

    va_start( list, cmd );
    for(i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    BTL_Printf("Write Reserved Pos ... pos=%d, cmd=%d", pos, cmd );
    if( arg_cnt ){
      BTL_PrintfSimple(" args = ");
      for( i=0; i<arg_cnt; ++i ){
        BTL_PrintfSimple("%d,", ArgBuffer[i]);
      }
    }
    BTL_PrintfSimple("\n");

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
  ServerCmd cmd = scque_read2byte( que );

  // �\��̈�ɉ����������܂�Ȃ������ꍇ�͒P���ɃX�L�b�v����
  while( cmd == SCEX_RESERVE )
  {
    u8 reserve_size = scque_read1byte( que );
    BTL_Printf("Reserved Skip! size=%d\n", reserve_size);
    que->readPtr += reserve_size;
    cmd = scque_read2byte( que );
  }

  GF_ASSERT( cmd < NELEMS(ServerCmdToFmtTbl) );

  {
    u8 fmt = ServerCmdToFmtTbl[ cmd ];

    if( fmt != SC_ARGFMT_MSG )
    {
      read_core( que, fmt, args );

      #ifdef BTL_PRINT_SYSTEM_ENABLE
      {
        u8 arg_cnt = SC_ARGFMT_GetArgCount( fmt );
        u8 i;
        BTL_Printf("ReadCmd=%d, Format=%02x, argCnt=%d, args=", cmd, fmt, arg_cnt);
        for(i=0; i<arg_cnt; ++i)
        {
          BTL_PrintfSimple("%d,", args[i]);
        }
        BTL_PrintfSimple("\n");
      }
      #endif
    }
    else
    {
      read_core_msg( que, args );
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
  {
    va_list   list;
    ScMsgArg   arg;
    u16 strID;

    va_start( list, scType );
    strID = va_arg( list, int );

    scque_put2byte( que, scType );
    scque_put2byte( que, strID );

    BTL_Printf(" PUT MSG SC=%d, StrID=%d\n arg= ", scType, strID);
    do {
      arg = va_arg( list, int );
      BTL_PrintfSimple("%d ", arg);
      scque_put2byte( que, arg );
    }while( arg != MSGARG_TERMINATOR );
    BTL_PrintfSimple("\n");


    va_end( list );
  }
}

static void read_core_msg( BTL_SERVER_CMD_QUE* que, int* args )
{
  args[0] = scque_read2byte( que );

  BTL_Printf("READ MSG strID=%d\n", args[0]);

  {
    int i = 0;

    do {
      ++i;
      if( i < BTL_SERVERCMD_ARG_MAX )
      {
        args[i] = scque_read2byte( que );
        BTL_Printf(" msg arg[%d] = %d\n", i-1, args[i]);
      }
      else
      {
        GF_ASSERT(0);
        break;
      }
    }while( args[i] != MSGARG_TERMINATOR );
  }
}


