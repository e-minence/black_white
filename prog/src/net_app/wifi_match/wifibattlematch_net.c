//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_net.c
 *	@brief  WIFI�o�g���p�l�b�g���W���[��
 *	@author	Toru=Nagihashi
 *	@data		2009.11.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>
#include <dwc.h>

//	�V�X�e��
#include "system/main.h"

//  �l�b�g���[�N
#include "net/network_define.h"
#include "net/dwc_rap.h"

//�A�g���X���
#include "atlas_syachi2ds_v1.h"

//  �O�����J
#include "wifibattlematch_net.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_NET_PRINT_ON
#endif //PM_DEBUG

#if defined(DEBUG_NET_PRINT_ON) && defined(DEBUG_ONLY_FOR_toru_nagihashi)
#define DEBUG_NET_Printf(...)  OS_FPrintf(3,__VA_ARGS__)
#else
#define DEBUG_NET_Printf(...)  /*  */
#endif  //DEBUG_NET_PRINT_ON

//-------------------------------------
///	SC�̃V�[�P���X
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_SC_SEQ_INIT,
  WIFIBATTLEMATCH_SC_SEQ_LOGIN,
  WIFIBATTLEMATCH_SC_SEQ_SESSION_START,
  WIFIBATTLEMATCH_SC_SEQ_SESSION_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START,
  WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA,
  WIFIBATTLEMATCH_SC_SEQ_INTENTION_START,
  WIFIBATTLEMATCH_SC_SEQ_INTENTION_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_START,
  WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD,
  WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START,
  WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT,
  WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_START,
  WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_EXIT,
  WIFIBATTLEMATCH_SC_SEQ_END,
} WIFIBATTLEMATCH_SC_SEQ;

//-------------------------------------
///	GDB�̃V�[�P���X
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_GDB_SEQ_INIT,
  WIFIBATTLEMATCH_GDB_SEQ_GET_START,
  WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT,
  WIFIBATTLEMATCH_GDB_SEQ_GET_END,
  WIFIBATTLEMATCH_GDB_SEQ_EXIT,
  WIFIBATTLEMATCH_GDB_SEQ_END,
} WIFIBATTLEMATCH_GDB_SEQ;

//-------------------------------------
///	�^�C�~���O
//=====================================
#define WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING    (1)
#define WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING  (2)
#define WIFIBATTLEMATCH_SC_REPORT_TIMING    (3)

//-------------------------------------
///	���̑�
//=====================================
#define TIMEOUT_MS   100  // HTTP�ʐM�̃^�C���A�E�g����
#define PLAYER_NUM   2          // �v���C���[��
#define TEAM_NUM     0          // �`�[����


//-------------------------------------
///	�}�b�`���C�N�L�[
//=====================================
typedef enum
{
  MATCHMAKE_KEY_BTL,  //�_�~�[
  MATCHMAKE_KEY_DEBUG,  //�_�~�[

  MATCHMAKE_KEY_MAX,
} MATCHMAKE_KEY;

#define WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT  15

//-------------------------------------
///	��M�t���O
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER,
  WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA,
  WIFIBATTLEMATCH_NET_RECVFLAG_MAX
} WIFIBATTLEMATCH_NET_RECVFLAG;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�v���C���[�f�[�^
//=====================================
typedef struct
{
  DWCScLoginCertificatePtr mCertificate;
  DWCScReportPtr mReport;
  u8 mSessionId[DWC_SC_SESSION_GUID_SIZE];
  u8 mConnectionId[DWC_SC_CONNECTION_GUID_SIZE];
  u8 mStatsAuthdata[DWC_SC_AUTHDATA_SIZE];
} DWC_SC_PLAYERDATA;

//-------------------------------------
///	�������ݏ��
//=====================================
typedef struct 
{
  BtlRule   btl_rule;
  BtlResult btl_result;
} DWC_SC_WRITE_DATA;

//-------------------------------------
///	�F�B���w��s�A�}�b�`���C�N�p�ǉ��L�[�f�[�^�\����
//=====================================
typedef struct
{
  int ivalue;       // �L�[�ɑΉ�����l�iint�^�j
  u8  keyID;        // �}�b�`���C�N�p�L�[ID
  u8 pad;           // �p�f�B���O
  char keyStr[3];   // �}�b�`���C�N�p�L�[������
} MATCHMAKE_KEY_WORK;

//-------------------------------------
///	�l�b�g���W���[��
//=====================================
struct _WIFIBATTLEMATCH_NET_WORK
{ 
  u32 seq_matchmake;
  char filter[128];
  MATCHMAKE_KEY_WORK  key_wk[ MATCHMAKE_KEY_MAX ];
  HEAPID heapID;

  u32   seq;
  BOOL  is_gdb_start;
  BOOL  is_sc_start;
  u32   wait_cnt;
  
  DWC_SC_PLAYERDATA player[2];  //�����͂O ����͂P
  BOOL              is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_MAX];
  DWCScResult       result;

  void *p_get_wk;
  DWC_SC_WRITE_DATA report;
 
  WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data[2]; //0�����M�o�b�t�@  1����M�o�b�t�@
  BOOL is_recv_enemy_data;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void MATCHMAKE_KEY_Set( WIFIBATTLEMATCH_NET_WORK *p_wk, MATCHMAKE_KEY key, int value );
static int WIFIBATTLEMATCH_Eval_Callback( int index, void* p_param_adrs );

//-------------------------------------
///	SGFL_NET�R�[���o�b�N
//=====================================
static void DwcRap_Sc_RecvPlayerData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static void WifiBattleMaptch_RecvEnemyData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static u8* WifiBattleMaptch_RecvEnemyDataBuffAddr(int netID, void* p_wk_adrs, int size);
//-------------------------------------
///	SC�֌W
//=====================================
//DWC�R�[���o�b�N
static void DwcRap_Sc_CreateSessionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SetReportIntentionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SubmitReportCallback( DWCScResult theResult, void* theUserData );
//private
static void DwcRap_Sc_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data );
static DWCScResult DwcRap_Sc_CreateReportCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my );

//-------------------------------------
///	GDB�֌W
//=====================================
static void DwcRap_Gdb_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param);
static void DwcRap_Gdb_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );
#ifdef PM_DEBUG
static void print_field(DWCGdbField* field); // ���R�[�h���f�o�b�O�o�͂���B
#endif

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�}�b�`���C�N�L�[������
//=====================================
static const char *sc_matchmake_key_str[ MATCHMAKE_KEY_MAX ] =
{ 
  "bt",
  "db",
};
//-------------------------------------
///	�ʐM�R�}���h
//=====================================
enum
{ 
  WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA  = GFL_NET_CMD_WIFIMATCH,
  WIFIBATTLEMATCH_NETCMD_SEND_ENEMYDATA,
  WIFIBATTLEMATCH_NETCMD_MAX,
};
static const NetRecvFuncTable sc_net_recv_tbl[WIFIBATTLEMATCH_NETCMD_MAX] = 
{
  {DwcRap_Sc_RecvPlayerData,   NULL},    ///_NETCMD_INFOSEND
  {WifiBattleMaptch_RecvEnemyData, WifiBattleMaptch_RecvEnemyDataBuffAddr },
};

//-------------------------------------
///	SAKE�T�[�o�[�ɖ₢���킹�邽�߂̃L�[��
//=====================================
static const char* sc_field_names[] = 
{
  "ARENA_ELO_RATING_1V1_DOUBLE",
  "ARENA_ELO_RATING_1V1_ROTATE",
  "ARENA_ELO_RATING_1V1_SHOOTER",
  "ARENA_ELO_RATING_1V1_SINGLE",
  "ARENA_ELO_RATING_1V1_TRIPLE",
  "CHEATS_COUNTER",
  "COMPLETE_MATCHES_COUNTER",
  "DISCONNECTS_COUNTER",
  "NUM_DOUBLE_LOSE_COUNTER",
  "NUM_DOUBLE_WIN_COUNTER",
  "NUM_ROTATE_LOSE_COUNTER",
  "NUM_ROTATE_WIN_COUNTER",
  "NUM_SHOOTER_LOSE_COUNTER",
  "NUM_SHOOTER_WIN_COUNTER",
  "NUM_SINGLE_LOSE_COUNTER",
  "NUM_SINGLE_WIN_COUNTER"  ,
  "NUM_TRIPLE_LOSE_COUNTER",
  "NUM_TRIPLE_WIN_COUNTER",
};


//=============================================================================
/**
 *					�������E�j��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( HEAPID heapID )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NET_WORK) );

  { 
    p_wk->p_enemy_data[0]  = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
    GFL_STD_MemClear( p_wk->p_enemy_data[0], WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
    p_wk->p_enemy_data[1]  = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
    GFL_STD_MemClear( p_wk->p_enemy_data[1], WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  }

  if(!GFL_NET_IsInit())
  {
    GF_ASSERT(0);
  }

  GFL_NET_AddCommandTable( GFL_NET_CMD_WIFIMATCH, sc_net_recv_tbl, WIFIBATTLEMATCH_NETCMD_MAX, p_wk );


  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  GFL_NET_DelCommandTable( GFL_NET_CMD_WIFIMATCH );

  GFL_HEAP_FreeMemory( p_wk->p_enemy_data[1] );
  GFL_HEAP_FreeMemory( p_wk->p_enemy_data[0] );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  if( p_wk->seq_matchmake == WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE )
  { 
    GFL_NET_SetWifiBothNet(FALSE);

    MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DEBUG, MATCHINGKEY );
    MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL, 0 );
    STD_TSPrintf( p_wk->filter, "bt=%d And db=%d", 0, MATCHINGKEY );
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  switch( p_wk->seq_matchmake )
  { 
  case WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE:
    /* �������Ȃ�  */
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START:
    if( GFL_NET_DWC_StartMatchFilter( p_wk->filter, 2 ,&WIFIBATTLEMATCH_Eval_Callback, p_wk ) != 0 )
    {
      GFL_NET_DWC_SetVChat( VCHAT_NONE );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT:
    {
      int ret;

      ret = GFL_NET_DWC_GetStepMatchResult();
      switch(ret)
      {
      case STEPMATCH_CONTINUE:
        /* �ڑ��� */
        break;

      case STEPMATCH_SUCCESS:
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_START;
        break;

      case STEPMATCH_CANCEL:
        //GF_ASSERT( 0 );
        break;

      case STEPMATCH_FAIL:
        GF_ASSERT( 0 );
        break;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_START:
    if(GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE){  // �q�@�Ƃ��Đڑ�����������
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD;
      }
    }
    else
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT;
    }
    break;


  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD:
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT);
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT:
    if( GFL_NET_HANDLE_GetNumNegotiation() != 0 )
    {
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT);
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_TIMING_END:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT))
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_END;
    }
    break;


  case WIFIBATTLEMATCH_NET_SEQ_MATCH_END:
    /* �������Ȃ� */
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�̃V�[�P���X���擾
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk ���[�N
 *
 *	@return �V�[�P���X
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_SEQ WIFIBATTLEMATCH_NET_GetSeqMatchMake( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return cp_wk->seq_matchmake;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N��ؒf����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	is_return   ���O�C����ɖ߂�Ȃ��TRUE  ���S�ɐؒf����Ȃ��FALSE
 *
 *	@return TRUE�ŏ����I��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SetDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_return )
{ 
  if( GFL_NET_DWC_disconnect( GFL_NET_IsParentMachine() ) )
  { 
    if( GFL_NET_DWC_returnLobby() && is_return )
    { 
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�̏����L�����Z��
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�ŃL�����Z������  FALSE�ŃL�����Z����
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SetCancelState( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return GFL_NET_DWC_SetCancelState();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����Z���������s���ėǂ����ǂ���
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk 
 *
 *	@param	FALSE �L�����Z�����Ă悢 TRUE�L�����Z���֎~
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_CancelDisable( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return GFL_NET_DWC_CancelDisable();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[����
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�ŃG���[  FALSE�Œʏ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_IsError( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  //@todo
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �L�[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	key                             �L�[
 *	@param	value                           �l
 */
//-----------------------------------------------------------------------------
static void MATCHMAKE_KEY_Set( WIFIBATTLEMATCH_NET_WORK *p_wk, MATCHMAKE_KEY key, int value )
{ 
  MATCHMAKE_KEY_WORK *p_key_wk = &p_wk->key_wk[ key ];

  p_key_wk->ivalue  = value;
  GFL_STD_MemCopy( sc_matchmake_key_str[ key ], p_key_wk->keyStr, 2 );
  p_wk->key_wk[ key ].keyID  = DWC_AddMatchKeyInt( p_key_wk->keyID,
      p_key_wk->keyStr, &p_key_wk->ivalue );
  if( p_key_wk->keyID == 0 )
  {
      NAGI_Printf("AddMatchKey failed key=%d.value=%d\n",key, value);
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��R�[���o�b�N
 *
 *	@param	int index
 *	@param	p_param_adrs 
 *
 *	@return �]���l  0�ȉ��͂Ȃ���Ȃ��B�@1�ȏ�͐����������قǐڑ�����₷���Ȃ�l
 *	                                        �i���������v���C���[�̕]���l���j
 *	                                        ���C�u����������8�r�b�g���V�t�g������8�r�b�g�ɗ����������
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_Eval_Callback( int index, void* p_param_adrs )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_param_adrs;

  int value=0;
/*
  targetlv = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_LEVEL].keyStr,-1);
  targetfriend = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_FRIEND].keyStr,-1);
  targetmy = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_MY].keyStr,-1);
*/
 
  //@todo   �����ɕ]������������
  value = 100;


  OS_TPrintf("�]���R�[���o�b�N %d \n",value );
  return value;

}

//=============================================================================
/**
 *    ���v�E�����֌W
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  SC�̊J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  BtlRule rule              ���[��
 *	@param  BtlResult result          �ΐ팋��
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_SC_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, BtlRule rule, BtlResult result )
{ 
  if( GFL_NET_IsParentMachine() )
  { 
    NAGI_Printf( "�I�e�@ ���[��%d ����%d\n", rule, result );
  }
  else
  { 
    NAGI_Printf( "�I�q�@ ���[��%d ����%d\n", rule, result );
  }

  p_wk->is_sc_start = TRUE;
  p_wk->seq = 0;
  p_wk->report.btl_rule  = rule;
  p_wk->report.btl_result  = result;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SC�̏�����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  DWCScResult               ����
 *	@retval TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ����i�s��or�s���G���[
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_SC_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, const DWCUserData *cp_user_data, DWCScResult *p_result )
{ 
  DWCScResult ret;
  
  *p_result = DWC_SC_RESULT_NO_ERROR;

  //@todo ���s�����瑊��Ɏ��s�𑗂�K�v������H
  if( p_wk->is_sc_start )
  { 
    switch( p_wk->seq )
    { 
    //������
    case WIFIBATTLEMATCH_SC_SEQ_INIT:
      p_wk->wait_cnt  = 0;
      { 
        ret = DWC_GdbInitialize( GAME_ID, cp_user_data, DWC_GDB_SSL_TYPE_NONE );
        if( *p_result != DWC_GDB_ERROR_NONE )
        { 
          *p_result = ret;
          p_wk->is_gdb_start  = FALSE;
          DwcRap_Gdb_Finalize( p_wk );
          return FALSE;
        }
      }

      ret = DWC_ScInitialize( GAME_ID );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:Init\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_LOGIN;
      break;

    //�F�؏��擾
    case WIFIBATTLEMATCH_SC_SEQ_LOGIN:
      ret = DWC_ScGetLoginCertificate( &p_wk->player[0].mCertificate );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:Login\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SESSION_START;
      break;

    //�Z�b�V�����J�n  �i�z�X�g���s���A�N���C�A���g�ɃZ�b�V����ID��`�d����j
    case WIFIBATTLEMATCH_SC_SEQ_SESSION_START:
      if( GFL_NET_IsParentMachine() )
      { 
        p_wk->wait_cnt++;
        ret = DWC_ScCreateSessionAsync( DwcRap_Sc_CreateSessionCallback, TIMEOUT_MS, p_wk );
        if( ret != DWC_SC_RESULT_NO_ERROR )
        { 
          *p_result = ret;
          p_wk->is_sc_start  = FALSE;
          DwcRap_Sc_Finalize( p_wk );
          return FALSE;
        }
        DEBUG_NET_Printf( "SC:Session parent\n" );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SESSION_WAIT;
      }
      else
      { 
        //�N���C�A���g�̓Z�b�V�������s�킸����
        DEBUG_NET_Printf( "SC:Session child\n" );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START;
      }
      break;

    //�Z�b�V�����I���҂�
    case WIFIBATTLEMATCH_SC_SEQ_SESSION_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:Session process parent\n" );
        GFL_STD_MemCopy( DWC_ScGetSessionId(), p_wk->player[0].mSessionId, DWC_SC_SESSION_GUID_SIZE );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START;
      }
      else
      { 
        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          *p_result = p_wk->result;
          p_wk->is_sc_start  = FALSE;
          DwcRap_Sc_Finalize( p_wk );
          return FALSE;
        }
      }
      break;

    //�z�X�g���N���C�A���g�փZ�b�V����ID�𑗂邽�߂̃^�C�~���O�Ƃ�
   case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START:
      GFL_STD_MemCopy( DWC_ScGetConnectionId(), p_wk->player[0].mConnectionId, DWC_SC_CONNECTION_GUID_SIZE );

      GFL_NET_HANDLE_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING );
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING ) )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA;
      }
      break;

    //�z�X�g���N���C�A���g�֎����̃f�[�^�𑗂�
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA:
      if( GFL_NET_IsParentMachine() )
      { 
        //�����𑗐M
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:Senddata parent\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_START;
        }
      }
      else
      { 
        //�󂯎��
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] )
        { 
          p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = FALSE;
          //�z�X�g�ȊO�̐l�́A�z�X�g��CreateSession����sessionId��ݒ肷��
          ret = DWC_ScSetSessionId( p_wk->player[1].mSessionId );
          GFL_STD_MemCopy( DWC_ScGetSessionId(), p_wk->player[0].mSessionId, DWC_SC_SESSION_GUID_SIZE );
          if( ret != DWC_SC_RESULT_NO_ERROR )
          {
            *p_result = ret;
            p_wk->is_sc_start  = FALSE;
            DwcRap_Sc_Finalize( p_wk );
            return FALSE;
          }
          DEBUG_NET_Printf( "SC:Senddata child\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_START;
        }
      }
      break;

    //���M���郌�|�[�g�̎�ނ�ʒm
    case WIFIBATTLEMATCH_SC_SEQ_INTENTION_START:
      p_wk->wait_cnt++;
      //�������͐e�̂ݐ������Ƃ����l�Ȃ̂��H
      ret = DWC_ScSetReportIntentionAsync( GFL_NET_IsParentMachine(), DwcRap_Sc_SetReportIntentionCallback, TIMEOUT_MS, p_wk );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      } 
      DEBUG_NET_Printf( "SC:intention start\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_WAIT;
      break;

    //�ʒm�҂�
    case WIFIBATTLEMATCH_SC_SEQ_INTENTION_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:intention_wait\n" );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_START;
      }
      else
      { 
        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          *p_result = p_wk->result;
          p_wk->is_sc_start  = FALSE;
          DwcRap_Sc_Finalize( p_wk );
          return FALSE;
        }
      }
      break;

    //�N���C�A���g�̏����z�X�g�����炤���߂̃^�C�~���O�Ƃ�
    case WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_START:
      GFL_NET_HANDLE_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING );
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING ) )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT;
      }
      break;

    //���݂���CCID�iIntention ConnectionID�j����������  �܂��͐e�f�[�^���q�ɑ��M
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT:
      if( GFL_NET_IsParentMachine() )
      { 
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:send data ccid parent\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD;
        }
      }
      else
      { 
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] )
        { 
          p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = FALSE;
          DEBUG_NET_Printf( "SC:send data ccid parent\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD;
        }
      }
      break;

    //���͎q�̃f�[�^�𑗐M
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD:
      if( !GFL_NET_IsParentMachine() )
      { 
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:send data ccid child\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START;
        }
      }
      else
      { 
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] )
        { 
          p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = FALSE;
          DEBUG_NET_Printf( "SC:send data ccid child\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START;
        }
      }
      break;

    //��M��A���|�[�g�쐬�̂��߂̃^�C�~���O�Ƃ�
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START:
      GFL_NET_HANDLE_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_REPORT_TIMING );
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_WAIT;
      break;

    //�^�C�~���O�܂�
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_REPORT_TIMING ) )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT;
      }
      break;

    //���|�[�g�쐬
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT:
      //���|�[�g�쐬
      ret = DwcRap_Sc_CreateReport( &p_wk->player[0], &p_wk->player[1], &p_wk->report );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:create report\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_START;
      break;

    //���|�[�g���M�J�n
    case WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_START:
      p_wk->wait_cnt++;
      ret = DWC_ScSubmitReportAsync( p_wk->player[0].mReport, GFL_NET_IsParentMachine(),
            DwcRap_Sc_SubmitReportCallback, TIMEOUT_MS, p_wk );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:submit report start\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_WAIT;
      break;

    //���|�[�g���M��
    case WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:submit report wait\n" );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_EXIT;
      }
      else
      { 
        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          *p_result = p_wk->result;
          p_wk->is_sc_start  = FALSE;
          DwcRap_Sc_Finalize( p_wk );
          return FALSE;
        }
      }
      break;

    //�I���I
    case WIFIBATTLEMATCH_SC_SEQ_EXIT:
      DEBUG_NET_Printf( "SC:exit\n" );
      DwcRap_Sc_Finalize( p_wk );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_END;
      break;

    case WIFIBATTLEMATCH_SC_SEQ_END:
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�b�V�����쐬�R�[���o�b�N
 *
 *	@param	DWCScResult theResult ����
 *	@param	theUserData           �������n�����A�h���X
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_CreateSessionCallback( DWCScResult theResult, void* theUserData )
{
  WIFIBATTLEMATCH_NET_WORK  *p_wk = theUserData;

  if (theResult == DWC_SC_RESULT_NO_ERROR)
  {
    DEBUG_NET_Printf( "[CreateSessionCallback]Session ID: %s\n", DWC_ScGetSessionId() );
    DEBUG_NET_Printf( "[CreateSessionCallback]Connection ID: %s\n", DWC_ScGetConnectionId() );
  }
  else
  {
    p_wk->result = theResult;
  }

  p_wk->wait_cnt--;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g���M�ӎv�ʒm�R�[���o�b�N
 *
 *	@param	theResult   ����
 *	@param	theUserData �����ŗ^�����A�h���X
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_SetReportIntentionCallback(DWCScResult theResult, void* theUserData)
{
  WIFIBATTLEMATCH_NET_WORK  *p_wk = theUserData;
  DWC_SC_PLAYERDATA   *p_player_data  = &p_wk->player[0];

  if (theResult == DWC_SC_RESULT_NO_ERROR)
  {
    GFL_STD_MemCopy( DWC_ScGetConnectionId(),p_player_data->mConnectionId, DWC_SC_CONNECTION_GUID_SIZE );
    DEBUG_NET_Printf( "[SetReportIntentionCallback]Connection ID: %s\n", DWC_ScGetConnectionId() );
  }
  else
  {
    DEBUG_NET_Printf( "SetReportIntentionCallback�G���[ %d\n", theResult );
    p_wk->result = theResult;
  }
  p_wk->wait_cnt--;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g���M�R�[���o�b�N
 *
 *	@param	theResult   ����
 *	@param	theUserData �����ŗ^�����A�h���X
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_SubmitReportCallback(DWCScResult theResult, void* theUserData)
{
  WIFIBATTLEMATCH_NET_WORK  *p_wk = theUserData;

  if (theResult == DWC_SC_RESULT_NO_ERROR)
  {
    DEBUG_NET_Printf( "[SubmitReportCallback]success\n" );
  }
  else
  {
    p_wk->result = theResult;
  }

  p_wk->wait_cnt--;

}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�����̃V�X�e���j���֐�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  if( p_wk->seq > WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT )
  {
    DEBUG_NET_Printf( "destroy report\n" );
    DWC_ScDestroyReport( p_wk->player[0].mReport );
  }

  if( p_wk->seq >= WIFIBATTLEMATCH_SC_SEQ_INIT )
  {
    DEBUG_NET_Printf( "sc shutdown\n" );
    DWC_ScShutdown();
    DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�ɏ������ޏ���
 *
 *	@param	DWC_SC_PLAYERDATA *p_player �v���C���[
 *	@param	DWC_SC_WRITE_DATA *cp_data  �������ޏ��
 *
 *	@return ����
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data )
{ 
  DWCScResult ret;

  //���|�[�g�쐬
  ret = DWC_ScCreateReport( ATLAS_RULE_SET_VERSION, PLAYER_NUM, TEAM_NUM, &p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  {
    DEBUG_NET_Printf( "Report:Create%d\n",ret );
    return ret;
  }
  
  // ���|�[�g�ɃO���[�o���f�[�^����������
  ret = DWC_ScReportBeginGlobalData( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  {
    DEBUG_NET_Printf( "Report:Grobal%d\n",ret );
    return ret;
  }

  // ���|�[�g�Ƀv���C���[�f�[�^����������
  ret = DWC_ScReportBeginPlayerData( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:Player%d\n",ret );
    return ret;
  }

  //�����̃��|�[�g���쐬
  ret = DWC_ScReportBeginNewPlayer( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:New%d\n",ret );
    return ret;
  }

  { 
      
    DWCScGameResult game_result;

    switch( cp_data->btl_result )
    { 
    case BTL_RESULT_RUN_ENEMY:
      /* fallthrough */
    case BTL_RESULT_WIN:
      game_result = DWC_SC_GAME_RESULT_WIN;
      break;
    case BTL_RESULT_RUN:
      /* fallthrough */
    case BTL_RESULT_LOSE:
      game_result = DWC_SC_GAME_RESULT_LOSS;
      break;
    case BTL_RESULT_DRAW:
      game_result = DWC_SC_GAME_RESULT_DRAW;
      break;
    default:
      GF_ASSERT_MSG(0, "�o�g�����ʕs���l %d\n", cp_data->btl_result );
    }
    ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                0,
                p_my->mConnectionId,
                0,
                game_result,
                p_my->mCertificate,
                p_my->mStatsAuthdata );

    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      DEBUG_NET_Printf( "Report:SetData%d\n",ret );
      return ret;
    }
  }

  //���я�������
  ret = DwcRap_Sc_CreateReportCore( p_my, cp_data, TRUE );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:Core%d\n",ret );
    return ret;
  }

  //����̃��|�[�g���쐬    ����̂Ȃ̂Ńf�[�^�͋t�ɂȂ�
  ret = DWC_ScReportBeginNewPlayer( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:New%d\n",ret );
    return ret;
  }

  { 
    DWCScGameResult game_result;

    //����͂��Ⴍ�ɂȂ�
    switch( cp_data->btl_result )
    { 
    case BTL_RESULT_RUN_ENEMY:
      /* fallthrough */
    case BTL_RESULT_WIN:
      game_result = DWC_SC_GAME_RESULT_LOSS;
      break;
    case BTL_RESULT_RUN:
      /* fallthrough */
    case BTL_RESULT_LOSE:
      game_result = DWC_SC_GAME_RESULT_WIN;
      break;
    case BTL_RESULT_DRAW:
      game_result = DWC_SC_GAME_RESULT_DRAW;
      break;
    default:
      GF_ASSERT_MSG(0, "�o�g�����ʕs���l %d\n", cp_data->btl_result );
    }
    ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                1,
                p_other->mConnectionId,
                0,
                game_result,
                p_other->mCertificate,
                p_other->mStatsAuthdata );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      DEBUG_NET_Printf( "Report:SetData%d\n",ret );
      return ret;
    }
  }

  //���я�������
  ret = DwcRap_Sc_CreateReportCore( p_my, cp_data, FALSE );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:Core%d\n",ret );
    return ret;
  }

  // ���|�[�g�Ƀ`�[���f�[�^����������
  ret = DWC_ScReportBeginTeamData( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:Team%d\n",ret );
    return ret;
  }

  //�@���|�[�g�̏I�[��錾����
  ret = DWC_ScReportEnd( p_my->mReport, GFL_NET_IsParentMachine(), DWC_SC_GAME_STATUS_COMPLETE );
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my )
{ 
  const BtlResult result  = cp_data->btl_result;
  BOOL is_win;
  DWCScResult ret;

  //��������̏����������ނȂ�΁A���ʂ𔽑΂ɂ���
  is_win = (cp_data->btl_result == BTL_RESULT_WIN || cp_data->btl_result == BTL_RESULT_RUN_ENEMY);  
  if( !is_my )
  {
    is_win  ^= 1;
  }

  DEBUG_NET_Printf( "-----------Report_start-------------\n" );
  DEBUG_NET_Printf( "���|�[�g��%d\n", is_my );
  DEBUG_NET_Printf( "����%d\n", is_win );
  DEBUG_NET_Printf( "����%d\n", !is_win );
  DEBUG_NET_Printf( "-----------Report_end-------------\n" );

  //���[���ɂ���đ�����̂��Ⴄ
  switch( cp_data->btl_rule )
  { 
  case BTL_RULE_SINGLE:    ///< �V���O��
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SINGLE, 1 );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_SINGLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_SINGLE_LOSE_COUNTER, !is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case BTL_RULE_DOUBLE:    ///< �_�u��
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_DOUBLE, 1 );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_DOUBLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_DOUBLE_LOSE_COUNTER, !is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case BTL_RULE_TRIPLE:    ///< �g���v��
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_TRIPLE, 1 );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_TRIPLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_TRIPLE_LOSE_COUNTER, !is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case BTL_RULE_ROTATION:  ///< ���[�e�[�V����
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_ROTATE, 1 );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_ROTATE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddShortValue( p_my->mReport, KEY_NUM_ROTATE_LOSE_COUNTER, !is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  default:
    GF_ASSERT(0);
  }

  return ret;
}

//=============================================================================
/**
 *        �R�[���o�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �v���C���[�f�[�^��M�R�[���o�b�N
 *
 *	@param	int netID     �l�b�gID
 *	@param	int size      �T�C�Y
 *	@param	void* pData   �f�[�^
 *	@param	pWk           �����ł��������A�h���X
 *	@param	pNetHandle    �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_RecvPlayerData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;

  if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  DEBUG_NET_Printf( "�v���C���[�f�[�^��M�R�[���o�b�N�InetID=%d size=%d \n", netID, size );

  GFL_STD_MemCopy( cp_data_adrs, &p_wk->player[1], sizeof(DWC_SC_PLAYERDATA));

  DEBUG_NET_Printf( "[Other]Session ID: %s\n", p_wk->player[1].mSessionId );
  DEBUG_NET_Printf( "[Other]Connection ID: %s\n", p_wk->player[1].mConnectionId );
  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �G�f�[�^��M�R�[���o�b�N
 *
 *	@param	int netID     �l�b�gID
 *	@param	int size      �T�C�Y
 *	@param	void* pData   �f�[�^
 *	@param	pWk           �����ł��������A�h���X
 *	@param	pNetHandle    �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMaptch_RecvEnemyData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;

  if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  DEBUG_NET_Printf( "�G�f�[�^��M�R�[���o�b�N�InetID=%d size=%d \n", netID, size );

  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�A�h���X
 *
 *	@param	netID �l�b�gID
 *	@param	pWork ���[�N
 *	@param	size  �T�C�Y
 *
 *	@return �o�b�t�@�A�h���X
 */
//-----------------------------------------------------------------------------
static u8* WifiBattleMaptch_RecvEnemyDataBuffAddr(int netID, void* p_wk_adrs, int size)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;

  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    GF_ASSERT( 0 );
    return NULL;//�����͎̂󂯎��Ȃ�
  }
  return (u8 *)p_wk->p_enemy_data[1];
}

//=============================================================================
/**
 *    �f�[�^�x�[�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  DB�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_GDB_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_GETTYPE type, void *p_wk_adrs )
{ 
  p_wk->is_gdb_start  = TRUE;
  p_wk->seq       = 0;
  p_wk->p_get_wk  = p_wk_adrs;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DB�̏�����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  DWCScResult               ����
 *	@retval TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ����i�s��or�s���G���[
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, const DWCUserData *cp_user_data, DWCGdbError *p_result )
{ 
  DWCError    error;
  DWCGdbState state;

  *p_result = DWC_GDB_ERROR_NONE;

  if( p_wk->is_gdb_start )  
  { 
    switch( p_wk->seq )
    { 
    case WIFIBATTLEMATCH_GDB_SEQ_INIT:
      //@todo���i�ł� DWC_GDB_SSL_TYPE_SERVER_AUTH
      *p_result = DWC_GdbInitialize( GAME_ID, cp_user_data, DWC_GDB_SSL_TYPE_NONE );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "GDB:Init\n" );
      p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_GET_START;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_START:
      *p_result = DWC_GdbGetMyRecordsAsync( "PlayerStats_v1", sc_field_names, NELEMS(sc_field_names), DwcRap_Gdb_GetRecordsCallback, p_wk->p_get_wk );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "GDB:Get start\n" );
      p_wk->seq= WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT:
      { 
        state = DWC_GdbGetState();
        if( DWC_GDB_STATE_IN_ASYNC_PROCESS == state )
        { 
          DWC_GdbProcess();
        }
        else
        { 
          if( state != DWC_GDB_STATE_IDLE )
          { 
            GF_ASSERT(0);
            DwcRap_Gdb_Finalize( p_wk );
            return FALSE;
          }
          else
          { 
            DEBUG_NET_Printf( "GDB:Get wait\n" );
            p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_GET_END;
          }
        }
      }
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_END:
      { 
        DWCGdbAsyncResult ret;
        ret = DWC_GdbGetAsyncResult();
        if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
        { 
          GF_ASSERT_MSG(0, "GDB:AsyncResult %d\n", ret);
          p_wk->is_gdb_start  = FALSE;
          DwcRap_Gdb_Finalize( p_wk );
          return FALSE;
        }
      }
      DEBUG_NET_Printf( "GDB:Get end\n" );
      p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_EXIT;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_EXIT:
      DwcRap_Gdb_Finalize( p_wk );
      DEBUG_NET_Printf( "GDB:Get exit\n" );
      p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_END;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_END:
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�����̃V�X�e���j���֐�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  DEBUG_NET_Printf( "GDB_ShotDown!" );
  DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param)
{
    int i,j;
    WIFIBATTLEMATCH_GDB_RND_SCORE_DATA *p_data = user_param;
    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_RND_SCORE_DATA) );

    for (i = 0; i < record_num; i++)
    {
      p_data->is_get  = TRUE;
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n");
      for (j = 0; j < NELEMS(sc_field_names); j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SINGLE_WIN_COUNTER ) ) )
        { 
          p_data->single_win  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SINGLE_LOSE_COUNTER) ) )
        { 
          p_data->single_lose  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_DOUBLE_WIN_COUNTER ) ) )
        { 
          p_data->double_win  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_DOUBLE_LOSE_COUNTER) ) )
        { 
          p_data->double_lose  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_ROTATE_WIN_COUNTER ) ) )
        { 
          p_data->rot_win  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_ROTATE_LOSE_COUNTER) ) )
        { 
          p_data->rot_lose  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_TRIPLE_WIN_COUNTER ) ) )
        { 
          p_data->triple_win  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_TRIPLE_LOSE_COUNTER) ) )
        { 
          p_data->triple_lose  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SHOOTER_WIN_COUNTER ) ) )
        { 
          p_data->shooter_win  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SHOOTER_LOSE_COUNTER) ) )
        { 
          p_data->shooter_lose  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_SINGLE ) ) )
        { 
          p_data->single_rate  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_DOUBLE) ) )
        { 
          p_data->double_rate  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_ROTATE) ) )
        { 
          p_data->rot_rate  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_TRIPLE) ) )
        { 
          p_data->triple_rate  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_SHOOTER) ) )
        { 
          p_data->shooter_rate  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(COMPLETE_MATCHES_COUNTER) ) )
        { 
          p_data->disconnect  = field->value.int_s16;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(CHEATS_COUNTER) ) )
        { 
          p_data->cheat  = field->value.int_s16;
        }
        print_field( field );
        DEBUG_NET_Printf(" ");
      }
      DEBUG_NET_Printf("!!!====================\n");
    }
}

#ifdef PM_DEBUG
static void print_field(DWCGdbField* field) // ���R�[�h���f�o�b�O�o�͂���B
{
    DEBUG_NET_Printf("%s[", field->name);
    switch (field->type)
    {
    case DWC_GDB_FIELD_TYPE_BYTE:
        DEBUG_NET_Printf("BYTE]:%d\n", field->value.int_u8);
        break;
    case DWC_GDB_FIELD_TYPE_SHORT:
        DEBUG_NET_Printf("SHORT]:%d\n", field->value.int_s16);
        break;
    case DWC_GDB_FIELD_TYPE_INT:
        DEBUG_NET_Printf("INT]:%d\n", field->value.int_s32);
        break;
    case DWC_GDB_FIELD_TYPE_FLOAT:
        DEBUG_NET_Printf("FLOAT]:%f\n", field->value.float_f64);
        break;
    case DWC_GDB_FIELD_TYPE_ASCII_STRING:
        DEBUG_NET_Printf("ASCII]:%s\n", field->value.ascii_string);
        break;
    case DWC_GDB_FIELD_TYPE_UNICODE_STRING:
        DEBUG_NET_Printf("UNICODE]\n");
        break;
    case DWC_GDB_FIELD_TYPE_BOOLEAN:
        DEBUG_NET_Printf("BOOL]:%d\n", field->value.boolean);
        break;
    case DWC_GDB_FIELD_TYPE_DATE_AND_TIME:
        DEBUG_NET_Printf("TIME]:%lld\n", field->value.datetime);
        break;
    case DWC_GDB_FIELD_TYPE_BINARY_DATA:
        DEBUG_NET_Printf("BINARY]:%d\n", field->value.binary_data.size);
        break;
    }
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  �G���
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *	@return TRUE�ő��M  FALSE�Ŏ�M
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_StartEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_buff )
{ 
  NetID netID;

  //����ɂ̂ݑ��M
  netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
  netID = netID == 0? 1: 0;
  return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), netID, WIFIBATTLEMATCH_NETCMD_SEND_ENEMYDATA, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE, cp_buff, TRUE, TRUE, TRUE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �G����M
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_data                         �f�[�^�󂯎��
 *
 *	@return TRUE��M  FALSE��M��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data )
{ 
  *pp_data  = p_wk->p_enemy_data[1];
  return p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA];
}
