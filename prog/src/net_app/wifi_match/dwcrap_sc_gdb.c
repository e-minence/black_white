//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwcrap_sc_gdb.c
 *	@brief  DWC���v�E�������f�[�^�x�[�X���b�v���[�`��
 *	@author	Toru=Nagihashi
 *	@data		2009.11.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include <dwc.h>

//�l�b�g���[�N
#include "net/network_define.h"

//�A�g���X���
#include "atlas_syachi2ds_v1.h"

//�O�����J
#include "dwcrap_sc_gdb.h"

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

#if defined(DEBUG_NET_PRINT_ON) | defined(DEBUG_ONLY_FOR_toru_nagihashi)
#define DEBUG_NET_Printf(...)  OS_FPrintf(3,__VA_ARGS__)
#else
#define DEBUG_NET_Printf(...)  /*  */
#endif  //DEBUG_NET_PRINT_ON

//-------------------------------------
///	SC�̃V�[�P���X
//=====================================
typedef enum 
{
  DWCRAP_SC_SEQ_INIT,
  DWCRAP_SC_SEQ_LOGIN,
  DWCRAP_SC_SEQ_SESSION_START,
  DWCRAP_SC_SEQ_SESSION_WAIT,
  DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_START,
  DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT,
  DWCRAP_SC_SEQ_SEND_PLAYERDATA,
  DWCRAP_SC_SEQ_INTENTION_START,
  DWCRAP_SC_SEQ_INTENTION_WAIT,
  DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_START,
  DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT,
  DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT,
  DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD,
  DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_START,
  DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_WAIT,
  DWCRAP_SC_SEQ_CREATE_REPORT,
  DWCRAP_SC_SEQ_SUBMIT_REPORT_START,
  DWCRAP_SC_SEQ_SUBMIT_REPORT_WAIT,
  DWCRAP_SC_SEQ_EXIT,
  DWCRAP_SC_SEQ_END,
} DWCRAP_SC_SEQ;

//-------------------------------------
///	GDB�̃V�[�P���X
//=====================================
typedef enum 
{
  DWCRAP_GDB_SEQ_INIT,
  DWCRAP_GDB_SEQ_GET_START,
  DWCRAP_GDB_SEQ_GET_WAIT,
  DWCRAP_GDB_SEQ_GET_END,
  DWCRAP_GDB_SEQ_EXIT,
  DWCRAP_GDB_SEQ_END,
} DWCRAP_GDB_SEQ;

//-------------------------------------
///	�^�C�~���O
//=====================================
#define DWCRAP_SC_SEND_PLAYERDATA_TIMING    (1)
#define DWCRAP_SC_RETURN_PLAYERDATA_TIMING  (2)
#define DWCRAP_SC_REPORT_TIMING    (3)


//-------------------------------------
///	���̑�
//=====================================
#define TIMEOUT_MS   100  // HTTP�ʐM�̃^�C���A�E�g����
#define PLAYER_NUM   2          // �v���C���[��
#define TEAM_NUM     0          // �`�[����

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
  int dummy;
} DWC_SC_WRITE_DATA;


//-------------------------------------
///	���[�N
//=====================================
struct _DWCRAP_SC_GDB_WORK
{ 
  u32   seq;
  BOOL  is_gdb_start;
  BOOL  is_sc_start;
  u32   wait_cnt;
  
  DWC_SC_PLAYERDATA player[2];  //�����͂O ����͂P
  BOOL              is_recv;
  DWCScResult       result;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	SC�֌W
//=====================================
//DWC�R�[���o�b�N
static void DwcRap_Sc_CreateSessionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SetReportIntentionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SubmitReportCallback( DWCScResult theResult, void* theUserData );
//private
static void DwcRap_Sc_Finalize( DWCRAP_SC_GDB_WORK *p_wk );
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data );
//GFL_NET�R�[���o�b�N
static void DwcRap_Sc_RecvPlayerData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
//-------------------------------------
///	GDB�֌W
//=====================================
static void DwcRap_Gdb_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param);
static void DwcRap_Gdb_Finalize( DWCRAP_SC_GDB_WORK *p_wk );
#ifdef PM_DEBUG
static void print_field(DWCGdbField* field); // ���R�[�h���f�o�b�O�o�͂���B
#endif

//=============================================================================
/**
 *          �f�[�^
 */
//=============================================================================
//-------------------------------------
///	�ʐM�R�}���h
//=====================================
enum
{ 
  DWCRAP_SC_NETCMD_SEND_PLAYERDATA  = GFL_NET_CMD_WIFIMATCH,
  DWCRAP_SC_NETCMD_MAX,
};
static const NetRecvFuncTable sc_net_recv_tbl[DWCRAP_SC_NETCMD_MAX] = 
{
  {DwcRap_Sc_RecvPlayerData,   NULL},    ///_NETCMD_INFOSEND
};

//-------------------------------------
///	SAKE�T�[�o�[�ɖ₢���킹�邽�߂̃L�[��
//=====================================
static const char* sc_field_names[] = 
{
  "ARENA_ELO_RATING_1V1",
  "TOTAL_MATCHES_COUNTER",
  "DISCONNECTS_COUNTER",
  "COMPLETE_MATCHES_COUNTER",
  "DISCONNECT_RATE",
  "NUM_SINGLE_MATCH_COUNTER",
  "ARENA_ELO_RATING_1V1_DOUBLE",
};

//=============================================================================
/**
 *    ���[�N�쐬�E�j��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �쐬
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
DWCRAP_SC_GDB_WORK * DWCRAP_SC_GDB_Init( HEAPID heapID )
{ 
  DWCRAP_SC_GDB_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DWCRAP_SC_GDB_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DWCRAP_SC_GDB_WORK) );

  if(!GFL_NET_IsInit())
  {
    GF_ASSERT(0);
  }

  GFL_NET_AddCommandTable( GFL_NET_CMD_WIFIMATCH, sc_net_recv_tbl, DWCRAP_SC_NETCMD_MAX, p_wk );
  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	DWCRAP_SC_GDB_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DWCRAP_SC_GDB_Exit( DWCRAP_SC_GDB_WORK *p_wk )
{ 
  GFL_NET_DelCommandTable( GFL_NET_CMD_WIFIMATCH );
  GFL_HEAP_FreeMemory( p_wk );
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
 *	@param	DWCRAP_SC_GDB_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void DWCRAP_SC_Start( DWCRAP_SC_GDB_WORK *p_wk )
{ 
  if( GFL_NET_IsParentMachine() )
  { 
    NAGI_Printf( "�I�e�@\n" );
  }
  else
  { 
    NAGI_Printf( "�I�q�@\n" );
  }

  p_wk->is_sc_start = TRUE;
  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SC�̏�����
 *
 *	@param	DWCRAP_SC_GDB_WORK *p_wk  ���[�N
 *	@param  DWCScResult               ����
 *	@retval TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ����i�s��or�s���G���[
 */
//-----------------------------------------------------------------------------
BOOL DWCRAP_SC_Process( DWCRAP_SC_GDB_WORK *p_wk, const DWCUserData *cp_user_data, DWCScResult *p_result )
{ 
  DWCScResult ret;
  
  *p_result = DWC_SC_RESULT_NO_ERROR;

  if( p_wk->is_sc_start )
  { 
    switch( p_wk->seq )
    { 
    //������
    case DWCRAP_SC_SEQ_INIT:
      p_wk->wait_cnt  = 0;
      { 
        ret = DWC_GdbInitialize( GAME_ID, cp_user_data, GF_DWC_GDB_AUTH_TYPE );
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
      p_wk->seq = DWCRAP_SC_SEQ_LOGIN;
      break;

    //�F�؏��擾
    case DWCRAP_SC_SEQ_LOGIN:
      ret = DWC_ScGetLoginCertificate( &p_wk->player[0].mCertificate );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:Login\n" );
      p_wk->seq = DWCRAP_SC_SEQ_SESSION_START;
      break;

    //�Z�b�V�����J�n  �i�z�X�g���s���A�N���C�A���g�ɃZ�b�V����ID��`�d����j
    case DWCRAP_SC_SEQ_SESSION_START:
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
        p_wk->seq = DWCRAP_SC_SEQ_SESSION_WAIT;
      }
      else
      { 
        //�N���C�A���g�̓Z�b�V�������s�킸����
        DEBUG_NET_Printf( "SC:Session child\n" );
        p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_START;
      }
      break;

    //�Z�b�V�����I���҂�
    case DWCRAP_SC_SEQ_SESSION_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:Session process parent\n" );
        GFL_STD_MemCopy( DWC_ScGetSessionId(), p_wk->player[0].mSessionId, DWC_SC_SESSION_GUID_SIZE );
        p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_START;
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
   case DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_START:
      GFL_STD_MemCopy( DWC_ScGetConnectionId(), p_wk->player[0].mConnectionId, DWC_SC_CONNECTION_GUID_SIZE );

      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
      p_wk->seq  = DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case DWCRAP_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) )
      { 
        p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA;
      }
      break;

    //�z�X�g���N���C�A���g�֎����̃f�[�^�𑗂�
    case DWCRAP_SC_SEQ_SEND_PLAYERDATA:
      if( GFL_NET_IsParentMachine() )
      { 
        //�����𑗐M
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:Senddata parent\n" );
          p_wk->seq = DWCRAP_SC_SEQ_INTENTION_START;
        }
      }
      else
      { 
        //�󂯎��
        if( p_wk->is_recv )
        { 
          p_wk->is_recv = FALSE;
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
          p_wk->seq = DWCRAP_SC_SEQ_INTENTION_START;
        }
      }
      break;

    //���M���郌�|�[�g�̎�ނ�ʒm
    case DWCRAP_SC_SEQ_INTENTION_START:
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
      p_wk->seq = DWCRAP_SC_SEQ_INTENTION_WAIT;
      break;

    //�ʒm�҂�
    case DWCRAP_SC_SEQ_INTENTION_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:intention_wait\n" );
        p_wk->seq = DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_START;
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
    case DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_START:
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
      p_wk->seq  = DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case DWCRAP_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) )
      { 
        p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT;
      }
      break;

    //���݂���CCID�iIntention ConnectionID�j����������  �܂��͐e�f�[�^���q�ɑ��M
    case DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT:
      if( GFL_NET_IsParentMachine() )
      { 
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:send data ccid parent\n" );
          p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD;
        }
      }
      else
      { 
        if( p_wk->is_recv )
        { 
          p_wk->is_recv = FALSE;
          DEBUG_NET_Printf( "SC:send data ccid parent\n" );
          p_wk->seq = DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD;
        }
      }
      break;

    //���͎q�̃f�[�^�𑗐M
    case DWCRAP_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD:
      if( !GFL_NET_IsParentMachine() )
      { 
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->player[0]) )
        { 
          DEBUG_NET_Printf( "SC:send data ccid child\n" );
          p_wk->seq = DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_START;
        }
      }
      else
      { 
        if( p_wk->is_recv )
        { 
          p_wk->is_recv = FALSE;
          DEBUG_NET_Printf( "SC:send data ccid child\n" );
          p_wk->seq = DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_START;
        }
      }
      break;

    //��M��A���|�[�g�쐬�̂��߂̃^�C�~���O�Ƃ�
    case DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_START:
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_REPORT_TIMING, WB_NET_WIFIMATCH );
      p_wk->seq  = DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_WAIT;
      break;

    //�^�C�~���O�܂�
    case DWCRAP_SC_SEQ_CREATE_REPORT_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), DWCRAP_SC_REPORT_TIMING, WB_NET_WIFIMATCH ) )
      { 
        p_wk->seq = DWCRAP_SC_SEQ_CREATE_REPORT;
      }
      break;

    //���|�[�g�쐬
    case DWCRAP_SC_SEQ_CREATE_REPORT:
      //���|�[�g�쐬
      ret = DwcRap_Sc_CreateReport( &p_wk->player[0], &p_wk->player[1], NULL );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        *p_result = ret;
        p_wk->is_sc_start  = FALSE;
        DwcRap_Sc_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "SC:create report\n" );
      p_wk->seq = DWCRAP_SC_SEQ_SUBMIT_REPORT_START;
      break;

    //���|�[�g���M�J�n
    case DWCRAP_SC_SEQ_SUBMIT_REPORT_START:
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
      p_wk->seq = DWCRAP_SC_SEQ_SUBMIT_REPORT_WAIT;
      break;

    //���|�[�g���M��
    case DWCRAP_SC_SEQ_SUBMIT_REPORT_WAIT:
      if( p_wk->wait_cnt == 0 )
      { 
        DEBUG_NET_Printf( "SC:submit report wait\n" );
        p_wk->seq = DWCRAP_SC_SEQ_EXIT;
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
    case DWCRAP_SC_SEQ_EXIT:
      DEBUG_NET_Printf( "SC:exit\n" );
      DwcRap_Sc_Finalize( p_wk );
      p_wk->seq = DWCRAP_SC_SEQ_END;
      break;

    case DWCRAP_SC_SEQ_END:
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
  DWCRAP_SC_GDB_WORK  *p_wk = theUserData;

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
  DWCRAP_SC_GDB_WORK  *p_wk = theUserData;
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
  DWCRAP_SC_GDB_WORK  *p_wk = theUserData;

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
 *	@param	DWCRAP_SC_GDB_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Sc_Finalize( DWCRAP_SC_GDB_WORK *p_wk )
{
  if( p_wk->seq > DWCRAP_SC_SEQ_CREATE_REPORT )
  {
    DEBUG_NET_Printf( "destroy report\n" );
    DWC_ScDestroyReport( p_wk->player[0].mReport );
  }

  if( p_wk->seq >= DWCRAP_SC_SEQ_INIT )
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

  ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                0,
                p_my->mConnectionId,
                0,
                GFL_NET_IsParentMachine() ? DWC_SC_GAME_RESULT_WIN : DWC_SC_GAME_RESULT_LOSS,
                p_my->mCertificate,
                p_my->mStatsAuthdata );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:SetData%d\n",ret );
    return ret;
  }

  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_DOUBLE, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_MATCH_COUNTER, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddStringValue( p_my->mReport,  ARENA_FFA_PLACE, GFL_NET_IsParentMachine() ? "1" : "2" );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }

  //����̃��|�[�g���쐬    ����̂Ȃ̂Ńf�[�^�͋t�ɂȂ�
  ret = DWC_ScReportBeginNewPlayer( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:New%d\n",ret );
    return ret;
  }

  ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                1,
                p_other->mConnectionId,
                0,
                GFL_NET_IsParentMachine() ? DWC_SC_GAME_RESULT_LOSS : DWC_SC_GAME_RESULT_WIN,
                p_other->mCertificate,
                p_other->mStatsAuthdata );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:SetData%d\n",ret );
    return ret;
  }

  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_DOUBLE, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_MATCH_COUNTER, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
    return ret;
  }
  ret = DWC_ScReportAddStringValue( p_my->mReport, ARENA_FFA_PLACE, GFL_NET_IsParentMachine() ? "2" : "1" );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:AddInt%d\n",ret );
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
  DWCRAP_SC_GDB_WORK *p_wk  = p_wk_adrs;

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
  p_wk->is_recv = TRUE;
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
 *	@param	DWCRAP_SC_GDB_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DWCRAP_GDB_Start( DWCRAP_SC_GDB_WORK *p_wk )
{ 
  p_wk->is_gdb_start  = TRUE;
  p_wk->seq = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DB�̏�����
 *
 *	@param	DWCRAP_SC_GDB_WORK *p_wk  ���[�N
 *	@param  DWCScResult               ����
 *	@retval TRUE�Ȃ�Ώ����I��  FALSE�Ȃ�Ώ����i�s��or�s���G���[
 */
//-----------------------------------------------------------------------------
BOOL DWCRAP_GDB_Process( DWCRAP_SC_GDB_WORK *p_wk, const DWCUserData *cp_user_data, DWCGdbError *p_result )
{ 
  DWCError    error;
  DWCGdbState state;

  *p_result = DWC_GDB_ERROR_NONE;

  if( p_wk->is_gdb_start )  
  { 
    switch( p_wk->seq )
    { 
    case DWCRAP_GDB_SEQ_INIT:
      *p_result = DWC_GdbInitialize( GAME_ID, cp_user_data, GF_DWC_GDB_AUTH_TYPE );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "GDB:Init\n" );
      p_wk->seq = DWCRAP_GDB_SEQ_GET_START;
      break;

    case DWCRAP_GDB_SEQ_GET_START:
      *p_result = DWC_GdbGetMyRecordsAsync( "PlayerStats_v1", sc_field_names, NELEMS(sc_field_names), DwcRap_Gdb_GetRecordsCallback, p_wk );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "GDB:Get start\n" );
      p_wk->seq= DWCRAP_GDB_SEQ_GET_WAIT;
      break;

    case DWCRAP_GDB_SEQ_GET_WAIT:
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
            p_wk->seq = DWCRAP_GDB_SEQ_GET_END;
          }
        }
      }
      break;

    case DWCRAP_GDB_SEQ_GET_END:
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
      p_wk->seq = DWCRAP_GDB_SEQ_EXIT;
      break;

    case DWCRAP_GDB_SEQ_EXIT:
      DwcRap_Gdb_Finalize( p_wk );
      DEBUG_NET_Printf( "GDB:Get exit\n" );
      p_wk->seq = DWCRAP_GDB_SEQ_END;
      break;

    case DWCRAP_GDB_SEQ_END:
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�����̃V�X�e���j���֐�
 *
 *	@param	DWCRAP_SC_GDB_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_Finalize( DWCRAP_SC_GDB_WORK *p_wk )
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

    for (i = 0; i < record_num; i++)
    {
        DEBUG_NET_Printf("!!!=====gdb_Print:======\n");
        for (j = 0; j < NELEMS(sc_field_names); j++)   // user_param -> field_num
        {
            print_field(&records[i][j]);
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
