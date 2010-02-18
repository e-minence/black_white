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
#include "system/net_err.h"
#include "net/dwc_error.h"

//  �Z�[�u�f�[�^
#include "savedata/system_data.h"
#include "savedata/wifilist.h"

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
#define DEBUG_DEBUG_NET_Printf_ON
#endif //PM_DEBUG

#if defined(DEBUG_DEBUG_NET_Printf_ON) && defined(DEBUG_ONLY_FOR_toru_nagihashi)
#define DEBUG_NET_Printf(...)  OS_FPrintf(3,__VA_ARGS__)
#else
#define DEBUG_NET_Printf(...)  /*  */
#endif  //DEBUG_DEBUG_NET_Printf_ON

//-------------------------------------
///	�}�N���X�C�b�`
//=====================================

//-------------------------------------
///	�l�b�g�萔
//=====================================
#ifdef PM_DEBUG
#define WIFIBATTLEMATCH_NET_SSL_TYPE DWC_GDB_SSL_TYPE_NONE
#else
#define WIFIBATTLEMATCH_NET_SSL_TYPE DWC_GDB_SSL_TYPE_SERVER_AUTH
#endif

#define WIFIBATTLEMATCH_NET_TABLENAME "PlayerStats_v1" 

//�ȉ��AATLAS�o�R�ł͂Ȃ��ASAKE���ڃA�N�Z�X�̏ꍇ
#define SAKE_STAT_RECORDID              "recordid"
#define SAKE_STAT_INITIAL_PROFILE_ID    "INITIAL_PROFILE_ID"
#define SAKE_STAT_NOW_PROFILE_ID        "NOW_PROFILE_ID"
#define SAKE_STAT_LAST_LOGIN_DATETIME   "LAST_LOGIN_DATETIME"
#define SAKE_STAT_WIFICUP_POKEMON_PARTY "WIFICUP_POKEMON_PARTY"

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
#define CANCELSELECT_TIMEOUT (60*60)     //�L�����Z���Z���N�g�^�C���A�E�g
#define ASYNC_TIMEOUT (60*60)     //�L�����Z���Z���N�g�^�C���A�E�g

#define RECV_BUFFER_SIZE  (0x1000)

//-------------------------------------
///	�}�b�`���C�N
//=====================================
//�}�b�`���C�N�L�[
typedef enum
{
  MATCHMAKE_KEY_BTL_MODE,
  MATCHMAKE_KEY_BTL_RULE,
  MATCHMAKE_KEY_DEBUG,
  MATCHMAKE_KEY_RATE,
  MATCHMAKE_KEY_CUPNO,
  MATCHMAKE_KEY_DISCONNECT,

  MATCHMAKE_KEY_MAX,
} MATCHMAKE_KEY;

//�}�b�`���C�N�^�C�~���O�V���N
#define WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT  15

//�}�b�`���C�N�]���v�Z���̒萔
#define MATCHMAKE_EVAL_CALC_RATE_STANDARD       (1000)
#define MATCHMAKE_EVAL_CALC_WEIGHT              (1)
#define MATCHMAKE_EVAL_CALC_DISCONNECT_WEIGHT   (32)

//-------------------------------------
///	��M�t���O
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER,
  WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA,
  WIFIBATTLEMATCH_NET_RECVFLAG_CANCELREQUEST,
  WIFIBATTLEMATCH_NET_RECVFLAG_POKEPARTY,
  WIFIBATTLEMATCH_NET_RECVFLAG_MAX
} WIFIBATTLEMATCH_NET_RECVFLAG;

//-------------------------------------
///	�A�g���r���[�g
//=====================================
#define WIFI_FILE_ATTR1			"REGCARD"
//#define WIFI_FILE_ATTR2		//�����ɑ��ԍ��������Ă���
#define WIFI_FILE_ATTR3			""
#define REGULATION_CARD_DATA_SIZE (sizeof(REGULATION_CARDDATA))

//-------------------------------------
///	  �s�������`�F�b�N
//=====================================
#define WIFIBATTLEMATCH_BADWORD_STRL_MAX  (10)

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
  WIFIBATTLEMATCH_BTLRULE   btl_rule;
  BtlResult btl_result;
} DWC_SC_WRITE_DATA;
//SC�̃��|�[�g�쐬�R�A�֐�
typedef DWCScResult (*DWCRAP_SC_CREATE_REPORT_CORE_FUNC)( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my );

//-------------------------------------
///	�F�B���w��s�A�}�b�`���C�N�p�ǉ��L�[�f�[�^�\����
//=====================================
typedef struct
{
  int ivalue;       // �L�[�ɑΉ�����l�iint�^�j
  u8  keyID;        // �}�b�`���C�N�p�L�[ID
  u8 pad;           // �p�f�B���O
  char keyStr[4];   // �}�b�`���C�N�p�L�[������
} MATCHMAKE_KEY_WORK;

//-------------------------------------
///	�l�b�g���W���[��
//=====================================
struct _WIFIBATTLEMATCH_NET_WORK
{ 
  const DWCUserData *cp_user_data;
  WIFI_LIST *p_wifilist;

  //�}�b�`���O
  u32 seq_matchmake;
  char filter[128];
  MATCHMAKE_KEY_WORK  key_wk[ MATCHMAKE_KEY_MAX ];


  HEAPID heapID;
  u32   init_seq;
  u32   seq;
  u32   next_seq;
  BOOL  is_gdb_start;
  BOOL  is_gdb_w_start;
  BOOL  is_sc_start;
  BOOL  is_initialize;
  BOOL  is_nd_start;
  u32   wait_cnt;
  BOOL              is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_MAX];
  
  //SC
  DWC_SC_PLAYERDATA player[2];  //�����͂O ����͂P
  DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc;
  DWCScResult       result;



  //gdb
  void *p_get_wk;
  DWCGdbGetRecordsCallback  gdb_get_record_callback;
  DWC_SC_WRITE_DATA report;
  DWCGdbField *p_field_buff;
  const char **pp_table_name;
  int table_name_num;
  int sake_record_id;
  DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA gdb_write_data;
  u32   magic_num;
  u32 async_timeout;
  u32 cancel_select_timeout;   //CANCELSELECT_TIMEOUT
  BOOL is_stop_connect;
  BOOL is_auth;

  //SENDDATA�n
  u8 recv_buffer[RECV_BUFFER_SIZE];//��M�o�b�t�@

  //�}�b�`���C�N
  DWCEvalPlayerCallback matchmake_eval_callback;

  //GPF�T�[�o�[����̎�M
  NHTTP_RAP_WORK* p_nhttp;
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA gpf_data;

  //�ȉ��_�E�����[�h�p
  REGULATION_CARDDATA temp_buffer;
  DWCNdFileInfo fileInfo;
  u32               percent;
  u32               recived;
  u32               contentlen;
  int               server_filenum;
  char              nd_attr2[10];

  //�ȉ��o�b�N�A�b�v�p
  s32 init_profileID;
  s32 now_profileID;

  //�ȉ��s�������`�F�b�N�p
  STRCODE   badword_str[ WIFIBATTLEMATCH_BADWORD_STRL_MAX ];
  u16       *p_badword_arry[1];
  char      badword_result[1];
  int       badword_num;

  //�ȉ��f�o�b�O�p�f�[�^
  BOOL is_debug;
  WIFIBATTLEMATCH_SC_DEBUG_DATA * p_debug_data;
};

//=============================================================================
/**
 *          �O���[�o��
 */
//=============================================================================
//NdCallback���̏��ێ��i�O���烏�[�N�n���Ȃ��̂Łc�c�j
static u8 s_callback_flag   = FALSE;
static DWCNdError s_callback_result = 0;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�}�b�`���O�֌W
//=====================================
static void MATCHMAKE_KEY_Set( WIFIBATTLEMATCH_NET_WORK *p_wk, MATCHMAKE_KEY key, int value );
static int WIFIBATTLEMATCH_RND_FREE_Eval_Callback( int index, void* p_param_adrs );
static int WIFIBATTLEMATCH_RND_RATE_Eval_Callback( int index, void* p_param_adrs );
static int WIFIBATTLEMATCH_WIFI_Eval_Callback( int index, void* p_param_adrs );

//-------------------------------------
///	GFL_NET�R�[���o�b�N
//=====================================
static void DwcRap_Sc_RecvPlayerData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static void WifiBattleMatch_RecvEnemyData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static u8* WifiBattleMatch_RecvBuffAddr(int netID, void* p_wk_adrs, int size);
static void WifiBattleMatch_RecvMatchCancel(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static void WifiBattleMatch_RecvPokeParty(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);


//-------------------------------------
///	SC�֌W
//=====================================
//DWC�R�[���o�b�N
static void DwcRap_Sc_CreateSessionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SetReportIntentionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SubmitReportCallback( DWCScResult theResult, void* theUserData );
//private
static void DwcRap_Sc_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data, BOOL is_auth, DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc );
static DWCScResult DwcRap_Sc_CreateReportDebug( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const WIFIBATTLEMATCH_SC_DEBUG_DATA * cp_data, BOOL is_auth );
//���|�[�g�쐬�R�[���o�b�N
static DWCScResult DwcRap_Sc_CreateReportRndCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my );
static DWCScResult DwcRap_Sc_CreateReportWifiCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my );

//-------------------------------------
///	GDB�֌W
//=====================================
static void DwcRap_Gdb_Rnd_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param);
static void DwcRap_Gdb_Wifi_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param);
static void DwcRap_Gdb_RecordID_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param);
static void DwcRap_Gdb_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );


#ifdef PM_DEBUG
static void print_field(DWCGdbField* field); // ���R�[�h���f�o�b�O�o�͂���B
#endif

//-------------------------------------
///	�_�E�����[�h�֌W
//=====================================
static void DwcRap_Nd_WaitNdCallback( WIFIBATTLEMATCH_NET_WORK *p_wk, int next_seq );
static void NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror);

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�t�B�[���h�̌^
//=====================================
static const int sc_field_type[]  =
{ 
  DWC_GDB_FIELD_TYPE_INT,//"ARENA_ELO_RATING_1V1_DOUBLE",
  DWC_GDB_FIELD_TYPE_INT,//"ARENA_ELO_RATING_1V1_ROTATE",
  DWC_GDB_FIELD_TYPE_INT,//"ARENA_ELO_RATING_1V1_SHOOTER",
  DWC_GDB_FIELD_TYPE_INT,//"ARENA_ELO_RATING_1V1_SINGLE",
  DWC_GDB_FIELD_TYPE_INT,//"ARENA_ELO_RATING_1V1_TRIPLE",
  DWC_GDB_FIELD_TYPE_INT,//"CHEATS_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"COMPLETE_MATCHES_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"DISCONNECTS_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_DOUBLE_LOSE_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_DOUBLE_WIN_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_ROTATE_LOSE_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_ROTATE_WIN_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_SHOOTER_LOSE_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_SHOOTER_WIN_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_SINGLE_LOSE_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_SINGLE_WIN_COUNTER"  ,
  DWC_GDB_FIELD_TYPE_INT,//"NUM_TRIPLE_LOSE_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"NUM_TRIPLE_WIN_COUNTER",
  DWC_GDB_FIELD_TYPE_INT,//"INITIAL_PROFILE_ID",
  DWC_GDB_FIELD_TYPE_INT,//"NOW_PROFILE_ID",
};
//-------------------------------------
///	�}�b�`���C�N�L�[������
//    3�����Œ�
//=====================================
static const char *sc_matchmake_key_str[ MATCHMAKE_KEY_MAX ] =
{ 
  "mod",
  "rul",
  "deb",
  "rat",
  "cup",
  "dis",
};
//-------------------------------------
///	�ʐM�R�}���h
//=====================================
enum
{ 
  WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA  = GFL_NET_CMD_WIFIMATCH,
  WIFIBATTLEMATCH_NETCMD_SEND_ENEMYDATA,
  WIFIBATTLEMATCH_NETCMD_SEND_CANCELMATCH,
  WIFIBATTLEMATCH_NETCMD_SEND_POKEPARTY,
  WIFIBATTLEMATCH_NETCMD_MAX,
};
static const NetRecvFuncTable sc_net_recv_tbl[] = 
{
  {DwcRap_Sc_RecvPlayerData,   NULL},    ///_NETCMD_INFOSEND
  {WifiBattleMatch_RecvEnemyData, WifiBattleMatch_RecvBuffAddr },
  {WifiBattleMatch_RecvMatchCancel, NULL},
  {WifiBattleMatch_RecvPokeParty, WifiBattleMatch_RecvBuffAddr },
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
WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( const DWCUserData *cp_user_data, WIFI_LIST *p_wifilist, HEAPID heapID )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NET_WORK) );
  p_wk->cp_user_data  = cp_user_data;
  p_wk->p_wifilist    = p_wifilist;

  p_wk->magic_num = 0x573;


  p_wk->p_field_buff  = GFL_HEAP_AllocMemory( heapID, (ATLAS_RND_GetFieldNameNum() + ATLAS_WIFI_GetFieldNameNum()) * sizeof(DWCGdbField) );
  GFL_STD_MemClear(p_wk->p_field_buff, (ATLAS_RND_GetFieldNameNum() + ATLAS_WIFI_GetFieldNameNum()) * sizeof(DWCGdbField) );

  GFL_NET_DebugPrintOn();

  if(!GFL_NET_IsInit())
  {
    GF_ASSERT(0);
  }

  GFL_NET_AddCommandTable( GFL_NET_CMD_WIFIMATCH, sc_net_recv_tbl, NELEMS(sc_net_recv_tbl), p_wk );
  GFL_NET_SetWifiBothNet(FALSE);

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

  GFL_HEAP_FreeMemory( p_wk->p_field_buff );
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
 *	@brief  DWC�̃G���[�`�F�b�N
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return TRUE�G���[����  FALSE
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_CheckError( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  return GFL_NET_DWC_ERROR_ReqErrorDisp();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  p_wk->is_initialize = TRUE;
  p_wk->init_seq  = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���������Ă��邩�ǂ����𔻕�
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�ŏ�����  FALSE�Ŕj��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_IsInitialize( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return cp_wk->is_initialize;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �������҂�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return TRUE��������  FALSE������
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk, SAVE_CONTROL_WORK *p_save, DWCGdbError *p_result )
{ 
  enum
  { 
    SEQ_START_GET_TABLE_ID,
    SEQ_WAIT_GET_TABLE_ID,
    SEQ_START_CREATE_RECORD,
    SEQ_WAIT_CREATE_RECORD,
    SEQ_END,
  };


  switch( p_wk->init_seq )
  { 
  case SEQ_START_GET_TABLE_ID:
    *p_result = DWC_GDB_ERROR_NONE;
    WIFIBATTLEMATCH_GDB_Start( p_wk, WIFIBATTLEMATCH_GDB_GET_RECORDID, &p_wk->sake_record_id );
    p_wk->init_seq  = SEQ_WAIT_GET_TABLE_ID;
    break;

  case SEQ_WAIT_GET_TABLE_ID:
    if( WIFIBATTLEMATCH_GDB_Process( p_wk, p_result ) )
    { 
      if( p_wk->sake_record_id != 0 )
      { 
        p_wk->init_seq  = SEQ_END;
      }
      else
      { 
        p_wk->init_seq  = SEQ_START_CREATE_RECORD;
      }
    }
    break;

  case SEQ_START_CREATE_RECORD:
    WIFIBATTLEMATCH_GDB_StartCreateRecord( p_wk );
    p_wk->init_seq  = SEQ_WAIT_CREATE_RECORD;
    break;

  case SEQ_WAIT_CREATE_RECORD:
    if( WIFIBATTLEMATCH_GDB_ProcessCreateRecord( p_wk, p_result ) )
    { 
      p_wk->init_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    DEBUG_NET_Printf( "sake �e�[�u��ID %d\n", p_wk->sake_record_id );
    p_wk->is_initialize = FALSE;
    return TRUE;
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_MODE mode, BOOL is_rnd_rate, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_MATCH_KEY_DATA *cp_data )
{ 
  u32 btl_mode  = mode;
  if( mode == WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    btl_mode  += is_rnd_rate;
  }

  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL_MODE, btl_mode );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL_RULE, btl_rule );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DEBUG, MATCHINGKEY );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_RATE, cp_data->rate );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DISCONNECT, cp_data->disconnect );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_CUPNO, cp_data->cup_no );
  STD_TSPrintf( p_wk->filter, "mod=%d And rul=%d And deb=%d", btl_mode, btl_rule, MATCHINGKEY );
  OS_TFPrintf( 3, "%s\n", p_wk->filter );
  p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START;

  //�ڑ��]���R�[���o�b�N�w��
  switch( mode )
  { 
  case WIFIBATTLEMATCH_MODE_WIFI:    //WIFI���
    p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_WIFI_Eval_Callback;
    break;
  case WIFIBATTLEMATCH_MODE_LIVE:    //���C�u���
    GF_ASSERT( 0 );
    break;
  case WIFIBATTLEMATCH_MODE_RANDOM:  //�����_���ΐ�@�i�}�b�`���C�N�̂Ƃ���RANDOM�{�O���t���[�{�P�����[�e�B���O�ɂ��Ă���j
    if( is_rnd_rate )
    { 
      p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_RATE_Eval_Callback;
    }
    else
    { 
      p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_FREE_Eval_Callback;
    }
    break;
  default:
    GF_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�J�n  �f�o�b�O��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartMatchMakeDebug( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL_MODE, 0 );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL_RULE, 0 );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DEBUG, MATCHINGKEY );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_RATE, 0 );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DISCONNECT, 0 );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_CUPNO, 0 );
  STD_TSPrintf( p_wk->filter, "mod=%d And rul=%d And deb=%d", 0, 0, MATCHINGKEY );
  p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START;

  p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_FREE_Eval_Callback;
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
 // DEBUG_NET_Printf( "WiFiState %d \n", GFL_NET_StateGetWifiStatus() );
  switch( p_wk->seq_matchmake )
  { 
  case WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE:
    /* �������Ȃ�  */
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START:
    if( GFL_NET_DWC_StartMatchFilter( p_wk->filter, 2, p_wk->matchmake_eval_callback, p_wk ) != 0 )
    {
      GFL_NET_DWC_SetVChat( FALSE );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START2;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START2:
    if( GFL_NET_StateStartWifiRandomMatch_RateMode() )
    { 
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
        p_wk->cancel_select_timeout = 0;
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_START;
        break;

      case STEPMATCH_CANCEL:
        GF_ASSERT( 0 );
        break;

      case STEPMATCH_FAIL:
        GF_ASSERT( 0 );
        break;

      case STEPMATCH_CONNECT:
        break;

      default:
        GF_ASSERT_MSG(0,"StepMatchResult=[%d]\n",ret);
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_START:
    {
      u16 netID;
      netID = GFL_NET_SystemGetCurrentID();
      if( netID != GFL_NET_NO_PARENTMACHINE){  // �q�@�Ƃ��Đڑ�����������
        if( GFL_NET_HANDLE_RequestNegotiation() )
        {
          p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD;
        }
      }
      else
      {
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT;
      }
      DEBUG_NET_Printf( "�ڑ����� %d^\n",netID );

      //���肪�u��߂܂����͂�or�������v���ɕ��u���邱�Ƃ��l���āA
      //�^�C���A�E�g������A�ؒf���āA�}�b�`���C�N���Ȃ���
      if( p_wk->cancel_select_timeout++ > CANCELSELECT_TIMEOUT )
      { 
        p_wk->cancel_select_timeout = 0;
        DEBUG_NET_Printf( "�}�b�`���O�l�S�V�G�[�V�����^�C���A�E�g\n" );
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CANCEL;
      }
    }
    break;


  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD:
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT, WB_NET_WIFIMATCH);
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT:
    if( GFL_NET_HANDLE_GetNumNegotiation() != 0 )
    {
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT, WB_NET_WIFIMATCH);
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_TIMING_END:
    //���肪�u��߂܂����͂�or�������v���ɕ��u���邱�Ƃ��l���āA
    //�^�C���A�E�g������A�ؒf���āA�}�b�`���C�N���Ȃ���
    if( p_wk->cancel_select_timeout++ > CANCELSELECT_TIMEOUT )
    { 
      p_wk->cancel_select_timeout = 0;
      DEBUG_NET_Printf( "�}�b�`���O�l�S�V�G�[�V�����^�C���A�E�g\n" );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CANCEL;
    }

    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT,WB_NET_WIFIMATCH))
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_END;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CANCEL:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk, TRUE ) )
    { 
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_END:
    /* �������Ȃ� */
    if( GFL_NET_IsParentMachine() )
    { 
      DEBUG_NET_Printf( "�I�e�@ " );
    }
    else
    {
      DEBUG_NET_Printf( "�I�q�@ " );
    }
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
 *	@param	is_force  �����ؒf�̂Ƃ���TRUE�@�󋵂ɍ��킹��Ƃ���FALSE
 *
 *	@return TRUE�ŏ����I��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SetDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_force )
{ 
  GFL_NET_StateWifiMatchEnd(!is_force);
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ����ꎞ��~�ɂ���
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	is_stop                         TRUE�ňꎞ��~  FALSE�ŉ���
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StopConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_stop )
{ 
  p_wk->is_stop_connect = is_stop;
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
  GFL_STD_MemCopy( sc_matchmake_key_str[ key ], p_key_wk->keyStr, 3 );
  p_key_wk->keyStr[3] = '\0';
  p_key_wk->keyID  = DWC_AddMatchKeyInt( p_key_wk->keyID,
      p_key_wk->keyStr, &p_key_wk->ivalue );
  if( p_key_wk->keyID == 0 )
  {
      DEBUG_NET_Printf("AddMatchKey failed key=%d.value=%d\n",key, value);
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
static int WIFIBATTLEMATCH_RND_RATE_Eval_Callback( int index, void* p_param_adrs )
{ 
  //���̂Ƃ��냉���_���}�b�`���[�e�B���O��WIFI���͓����]���v�Z
  return WIFIBATTLEMATCH_WIFI_Eval_Callback( index, p_param_adrs );
}
//�t���[���[�h�p
static int WIFIBATTLEMATCH_RND_FREE_Eval_Callback( int index, void* p_param_adrs )
{ 
  //�t���[���[�h�͒N�Ƃł������x�q����
  OS_TPrintf( "�t���[���[�h�]���R�[���o�b�N�I\n" );
  return 1000;

}
//WIFI���p
static int WIFIBATTLEMATCH_WIFI_Eval_Callback( int index, void* p_param_adrs )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_param_adrs;

  int value=0;
  int rate, disconnect, cup;
  rate      = DWC_GetMatchIntValue( index, p_wk->key_wk[MATCHMAKE_KEY_RATE].keyStr, -1 );
  disconnect= DWC_GetMatchIntValue(index, p_wk->key_wk[MATCHMAKE_KEY_DISCONNECT].keyStr, -1 );
  cup       = DWC_GetMatchIntValue( index, p_wk->key_wk[MATCHMAKE_KEY_CUPNO].keyStr, -1 );

  //�w�W�L�[�����f����Ă��Ȃ��̂ŁA��������
  if( rate == -1 || disconnect == -1 || cup == -1 )
  { 
    return 0;
  }

  if( cup == p_wk->key_wk[MATCHMAKE_KEY_CUPNO].ivalue )
  {
    //�������߂�
    rate        -= p_wk->key_wk[MATCHMAKE_KEY_RATE].ivalue;
    rate        = MATH_ABS( rate );
    disconnect  -= p_wk->key_wk[MATCHMAKE_KEY_DISCONNECT].ivalue;
    disconnect  = MATH_ABS( disconnect );

    //�]���v�Z��
    //�}�b�`���O�]���l  ���@�i�萔A - ���[�e�B���O���j �� �萔B�@�[�@�ؒf���� �� �萔C
    value = (MATCHMAKE_EVAL_CALC_RATE_STANDARD - rate) * MATCHMAKE_EVAL_CALC_WEIGHT
      - disconnect * MATCHMAKE_EVAL_CALC_DISCONNECT_WEIGHT;

    value = MATH_IMax( value, 0 );
  }
  else
  { 
    value = 0;
  }

  OS_TPrintf( "�]���R�[���o�b�N %d \n", value );
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
 *	@param  WIFIBATTLEMATCH_BTLRULE rule              ���[��
 *	@param  BtlResult result          �ΐ팋��
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_SC_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_MODE mode, WIFIBATTLEMATCH_BTLRULE rule, BtlResult result )
{ 
  if( GFL_NET_IsParentMachine() )
  { 
    DEBUG_NET_Printf( "�I�e�@ ���[��%d ����%d\n", rule, result );
  }
  else
  { 
    DEBUG_NET_Printf( "�I�q�@ ���[��%d ����%d\n", rule, result );
  }

  p_wk->is_sc_start = TRUE;
  p_wk->seq = 0;
  p_wk->report.btl_rule   = rule;
  p_wk->report.btl_result = result;
  p_wk->is_debug  = FALSE;
  p_wk->is_auth   = GFL_NET_IsParentMachine();
  
  switch( mode )
  { 
  case WIFIBATTLEMATCH_MODE_WIFI:    //WIFI���
    p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportWifiCore;
    break;
  case WIFIBATTLEMATCH_MODE_LIVE:    //���C�u���
    GF_ASSERT(0);
    break;
  case WIFIBATTLEMATCH_MODE_RANDOM:  //�����_���ΐ�@�i�}�b�`���C�N�̂Ƃ���RANDOM�{�O���t���[�{�P�����[�e�B���O�ɂ��Ă���j
    p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportRndCore;
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  SC�̊J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  WIFIBATTLEMATCH_BTLRULE rule              ���[��
 *	@param  BtlResult result          �ΐ팋��
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_SC_StartDebug( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_SC_DEBUG_DATA * p_data, BOOL is_auth )
{ 

  p_wk->is_sc_start   = TRUE;
  p_wk->seq           = 0;
  p_wk->is_debug      = TRUE;
  p_wk->is_auth = is_auth;
  p_wk->p_debug_data  = p_data;
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
BOOL WIFIBATTLEMATCH_SC_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCScResult *p_result )
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
        ret = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, DWC_GDB_SSL_TYPE_SERVER_AUTH );
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

      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) )
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
      ret = DWC_ScSetReportIntentionAsync( p_wk->is_auth, DwcRap_Sc_SetReportIntentionCallback, TIMEOUT_MS, p_wk );
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
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT;
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) )
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
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_REPORT_TIMING , WB_NET_WIFIMATCH);
      p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_WAIT;
      break;

    //�^�C�~���O�܂�
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_REPORT_TIMING, WB_NET_WIFIMATCH ) )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT;
      }
      break;

    //���|�[�g�쐬
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT:
      //���|�[�g�쐬
      if( !p_wk->is_debug )
      { 
        ret = DwcRap_Sc_CreateReport( &p_wk->player[0], &p_wk->player[1], &p_wk->report, p_wk->is_auth, p_wk->SC_CreateReportCoreFunc );
      }
      else
      { 
        ret = DwcRap_Sc_CreateReportDebug( &p_wk->player[0], &p_wk->player[1], p_wk->p_debug_data, p_wk->is_auth );
      }

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
      ret = DWC_ScSubmitReportAsync( p_wk->player[0].mReport, p_wk->is_auth,
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
 *	@brief  �C���^�C�v
 *
 *	@param	DWCScResult error �G���[
 *
 *	@return �C���^�C�v
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SC_GetErrorRepairType( DWCScResult error )
{ 
  switch( error )
  { 
  case DWC_SC_RESULT_NO_ERROR :              ///< �G���[�͔������܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_NONE;

  case DWC_SC_RESULT_NO_AVAILABILITY_CHECK:     ///< �������̑O�ɁA�W���� GameSpy �p���`�F�b�N�����s����܂���ł���
    GF_ASSERT(0);
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_INVALID_PARAMETERS:        ///< �C���^�[�t�F�[�X�֐��ɓn���ꂽ�p�����[�^�������ł�
    GF_ASSERT(0);
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_NOT_INITIALIZED:           ///< SDK ������������܂���ł���
    GF_ASSERT(0);
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL;

  case DWC_SC_RESULT_CORE_NOT_INITIALIZED:      ///< �R�A������������܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_OUT_OF_MEMORY:             ///< SDK �̓��\�[�X�Ƀ����������蓖�Ă��܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_CALLBACK_PENDING:          ///< �A�v���P�[�V�����ɑ΂��A���삪�ۗ����ł��邱�Ƃ�`���܂�
    GF_ASSERT( 0 );
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_HTTP_ERROR:                ///< �o�b�N�G���h���K���� HTTP �ɂ�郌�X�|���X�Ɏ��s�����ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_UNKNOWN_RESPONSE:          ///< SDK �����ʂ𗝉��ł��Ȃ��ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_RESPONSE_INVALID:          ///< SDK ���o�b�N�G���h����̃��X�|���X��ǂݎ��Ȃ��ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_INVALID_DATATYPE:          ///< �����ȃf�[�^�^�C�v
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_REPORT_INCOMPLETE:         ///< ���|�[�g���s���S�ł�
    GF_ASSERT(0);
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_REPORT_INVALID:            ///< ���|�[�g�̈ꕔ�܂��͂��ׂĂ������ł�
    GF_ASSERT(0);
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_SUBMISSION_FAILED:         ///< ���|�[�g�̑��M�Ɏ��s���܂���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

  case DWC_SC_RESULT_UNKNOWN_ERROR:             ///< SDK ���F���ł��Ȃ��G���[
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  default:
    GF_ASSERT(0);
    return 0;
  }
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
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data, BOOL is_auth, DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc )
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
  ret = SC_CreateReportCoreFunc( p_my, cp_data, TRUE );
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
  ret = SC_CreateReportCoreFunc( p_my, cp_data, FALSE );
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
  ret = DWC_ScReportEnd( p_my->mReport, is_auth, DWC_SC_GAME_STATUS_COMPLETE );
  return ret;
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
static DWCScResult DwcRap_Sc_CreateReportDebug( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const WIFIBATTLEMATCH_SC_DEBUG_DATA * cp_data, BOOL is_auth )
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
    ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                0,
                p_my->mConnectionId,
                0,
                cp_data->my_result,
                p_my->mCertificate,
                p_my->mStatsAuthdata );

    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      DEBUG_NET_Printf( "Report:SetData%d\n",ret );
      return ret;
    }
  }

  //���я�������
  //�V���O��
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SINGLE, cp_data->my_single_rate );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_WIN_COUNTER, cp_data->my_single_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
      return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_LOSE_COUNTER, cp_data->my_single_lose );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  //�_�u��
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_DOUBLE, cp_data->my_double_rate );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_WIN_COUNTER, cp_data->my_double_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_LOSE_COUNTER, cp_data->my_double_lose );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  //�g���v��
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_TRIPLE, cp_data->my_triple_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_WIN_COUNTER, cp_data->my_triple_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_LOSE_COUNTER, cp_data->my_triple_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
  //���[�e
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_ROTATE, cp_data->my_rot_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_WIN_COUNTER, cp_data->my_rot_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_LOSE_COUNTER, cp_data->my_rot_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }

  //�V���[�^�[
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SHOOTER, cp_data->my_shooter_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_WIN_COUNTER, cp_data->my_shooter_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_LOSE_COUNTER, cp_data->my_shooter_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
  //���̑�
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_CHEATS_COUNTER, cp_data->my_cheat );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
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

    ret = DWC_ScReportSetPlayerData(
                p_my->mReport,
                1,
                p_other->mConnectionId,
                0,
                cp_data->you_result,
                p_other->mCertificate,
                p_other->mStatsAuthdata );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      DEBUG_NET_Printf( "Report:SetData%d\n",ret );
      return ret;
    }
  }

  //���я�������
  //���я�������
  //�V���O��
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SINGLE, cp_data->you_single_rate );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_WIN_COUNTER, cp_data->you_single_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
      return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_LOSE_COUNTER, cp_data->you_single_lose );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  //�_�u��
  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_DOUBLE, cp_data->you_double_rate );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_WIN_COUNTER, cp_data->you_double_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_LOSE_COUNTER, cp_data->you_double_lose );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  //�g���v��
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_TRIPLE, cp_data->you_triple_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_WIN_COUNTER, cp_data->you_triple_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_LOSE_COUNTER, cp_data->you_triple_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
  //���[�e
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_ROTATE, cp_data->you_rot_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_WIN_COUNTER, cp_data->you_rot_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_LOSE_COUNTER, cp_data->you_rot_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }

  //�V���[�^�[
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SHOOTER, cp_data->you_shooter_rate );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_WIN_COUNTER, cp_data->you_shooter_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_LOSE_COUNTER, cp_data->you_shooter_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
  //���̑�
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_CHEATS_COUNTER, cp_data->you_cheat );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
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
  ret = DWC_ScReportEnd( p_my->mReport, is_auth, DWC_SC_GAME_STATUS_COMPLETE );
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
static DWCScResult DwcRap_Sc_CreateReportRndCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my )
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_LOSE_COUNTER, !is_win );
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_LOSE_COUNTER, !is_win );
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_LOSE_COUNTER, !is_win );
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_LOSE_COUNTER, !is_win );
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

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬  WIFI��
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportWifiCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my )
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

  ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_WIFICUP, 1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_WIFICUP_WIN_COUNTER, is_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_WIFICUP_LOSE_COUNTER, !is_win );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
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
static void WifiBattleMatch_RecvEnemyData(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
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
static u8* WifiBattleMatch_RecvBuffAddr(int netID, void* p_wk_adrs, int size)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;

  GF_ASSERT_MSG( size < RECV_BUFFER_SIZE, "recv_buff error!! size = 0x%x <> buff= 0x%x\n", size, RECV_BUFFER_SIZE );

  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    GF_ASSERT( 0 );
    return NULL;//�����͎̂󂯎��Ȃ�
  }
  return p_wk->recv_buffer;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���O�L�����Z���t���O�R�[���o�b�N
 *
 *	@param	int netID     �l�b�gID
 *	@param	int size      �T�C�Y
 *	@param	void* pData   �f�[�^
 *	@param	pWk           �����ł��������A�h���X
 *	@param	pNetHandle    �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_RecvMatchCancel(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
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

  DEBUG_NET_Printf( "�L�����Z����M�R�[���o�b�N�InetID=%d size=%d \n", netID, size );

  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_CANCELREQUEST] = TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�p�[�e�B��M�R�[���o�b�N
 *
 *	@param	int netID     �l�b�gID
 *	@param	int size      �T�C�Y
 *	@param	void* pData   �f�[�^
 *	@param	pWk           �����ł��������A�h���X
 *	@param	pNetHandle    �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_RecvPokeParty(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
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

  DEBUG_NET_Printf( "�|�P�p�[�e�B��M�R�[���o�b�N�InetID=%d size=%d \n", netID, size );

  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_POKEPARTY] = TRUE;
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
  p_wk->seq           = 0;
  p_wk->p_get_wk      = p_wk_adrs;


  switch( type )
  { 
  case WIFIBATTLEMATCH_GDB_GET_RND_SCORE:
    p_wk->pp_table_name  = ATLAS_RND_GetFieldName();
    p_wk->table_name_num= ATLAS_RND_GetFieldNameNum();
    p_wk->gdb_get_record_callback = DwcRap_Gdb_Rnd_GetRecordsCallback;
    break;

  case WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE:
    p_wk->pp_table_name  = ATLAS_WIFI_GetFieldName();
    p_wk->table_name_num= ATLAS_WIFI_GetFieldNameNum();
    p_wk->gdb_get_record_callback = DwcRap_Gdb_Wifi_GetRecordsCallback;
    break;

  case WIFIBATTLEMATCH_GDB_GET_RECORDID:
    { 
      static const char *sc_get_record_id_tbl[]  =
      { 
        SAKE_STAT_RECORDID
      };

      p_wk->pp_table_name  = sc_get_record_id_tbl;
      p_wk->table_name_num= NELEMS( sc_get_record_id_tbl );
      p_wk->gdb_get_record_callback = DwcRap_Gdb_RecordID_GetRecordsCallback;
    }
    break;
  }
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
BOOL WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result )
{ 
  DWCError    error;
  DWCGdbState state;

  *p_result = DWC_GDB_ERROR_NONE;

  if( p_wk->is_gdb_start )  
  { 
    switch( p_wk->seq )
    { 
    case WIFIBATTLEMATCH_GDB_SEQ_INIT:
      *p_result = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
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
      *p_result = DWC_GdbGetMyRecordsAsync( WIFIBATTLEMATCH_NET_TABLENAME, p_wk->pp_table_name, p_wk->table_name_num, p_wk->gdb_get_record_callback, p_wk->p_get_wk );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return FALSE;
      }
      DEBUG_NET_Printf( "GDB:Get start\n" );
      p_wk->seq= WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT;
      p_wk->async_timeout = 0;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT:
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          GF_ASSERT(0);
          DwcRap_Gdb_Finalize( p_wk );
          return FALSE;
        }

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
 *	@brief  ���R�[�h�擾�R�[���o�b�N  �����_���}�b�`��
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_Rnd_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param)
{
    int i,j;
    WIFIBATTLEMATCH_GDB_RND_SCORE_DATA *p_data = user_param;
    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_RND_SCORE_DATA) );

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n");
      for (j = 0; j < ATLAS_RND_GetFieldNameNum(); j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SINGLE_WIN_COUNTER ) ) )
        { 
          p_data->single_win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SINGLE_LOSE_COUNTER) ) )
        { 
          p_data->single_lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_DOUBLE_WIN_COUNTER ) ) )
        { 
          p_data->double_win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_DOUBLE_LOSE_COUNTER) ) )
        { 
          p_data->double_lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_ROTATE_WIN_COUNTER ) ) )
        { 
          p_data->rot_win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_ROTATE_LOSE_COUNTER) ) )
        { 
          p_data->rot_lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_TRIPLE_WIN_COUNTER ) ) )
        { 
          p_data->triple_win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_TRIPLE_LOSE_COUNTER) ) )
        { 
          p_data->triple_lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SHOOTER_WIN_COUNTER ) ) )
        { 
          p_data->shooter_win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_SHOOTER_LOSE_COUNTER) ) )
        { 
          p_data->shooter_lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_SINGLE ) ) )
        { 
          p_data->single_rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_DOUBLE) ) )
        { 
          p_data->double_rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_ROTATE) ) )
        { 
          p_data->rot_rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_TRIPLE) ) )
        { 
          p_data->triple_rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_SHOOTER) ) )
        { 
          p_data->shooter_rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(DISCONNECTS_COUNTER) ) )
        { 
          p_data->disconnect  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(CHEATS_COUNTER) ) )
        { 
          p_data->cheat  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_INITIAL_PROFILE_ID ) )
        { 
          p_data->init_profileID  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_NOW_PROFILE_ID ) )
        { 
          p_data->now_profileID  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_LAST_LOGIN_DATETIME ) )
        { 
        }
        print_field( field );
        DEBUG_NET_Printf(" ");
      }
      DEBUG_NET_Printf("!!!====================\n");
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N  WiFI����
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_Wifi_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param)
{
    int i,j;
    WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *p_data = user_param;
    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_RND_SCORE_DATA) );

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n");
      for (j = 0; j < ATLAS_WIFI_GetFieldNameNum(); j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_WIFICUP_WIN_COUNTER ) ) )
        { 
          p_data->win  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(NUM_WIFICUP_LOSE_COUNTER) ) )
        { 
          p_data->lose  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(ARENA_ELO_RATING_1V1_WIFICUP ) ) )
        { 
          p_data->rate  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(DISCONNECTS_WIFICUP_COUNTER) ) )
        { 
          p_data->disconnect  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(CHEATS_WIFICUP_COUNTER) ) )
        { 
          p_data->cheat  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, ATLAS_GET_STAT_NAME(CHEATS_WIFICUP_COUNTER) ) )
        { 
          p_data->cheat  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_WIFICUP_POKEMON_PARTY ) )
        { 
          DWCGdbBinaryData  *p_binary = &field->value.binary_data;
          GF_ASSERT_MSG( p_binary->size <= WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE, "not %d <= %d" , p_binary->size, WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE );
          GFL_STD_MemCopy( p_binary->data, p_data->pokeparty, p_binary->size );
        }
        print_field( field );
        DEBUG_NET_Printf(" ");
      }
      DEBUG_NET_Printf("!!!====================\n");
    }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N  ���R�[�hID�̂�
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_RecordID_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param)
{ 
    int i,j;
    int *p_data = user_param;

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n" );
      for (j = 0; j < 1; j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORDID ) )
        { 
          *p_data  = field->value.int_s32;
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
 *	@brief  GDB�փf�[�^��������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	type                            �f�[�^�^�C�v
 *	@param	void *cp_wk_adrs                �������ރf�[�^
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_GDB_StartWrite( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_WRITETYPE type, const void *cp_wk_adrs )
{ 

  p_wk->is_gdb_w_start  = TRUE;
  p_wk->seq             = 0;
  switch( type )
  { 
  case WIFIBATTLEMATCH_GDB_WRITE_DEBUGALL:
    { 
      int i;
      const WIFIBATTLEMATCH_GDB_RND_SCORE_DATA *cp_data  = cp_wk_adrs;

      p_wk->table_name_num  = ATLAS_RND_GetFieldNameNum();
      for( i = 0; i < p_wk->table_name_num; i++ )
      { 
        p_wk->p_field_buff[i].name  = (char*)ATLAS_RND_GetFieldName()[i];
        p_wk->p_field_buff[i].type  = sc_field_type[i];
        p_wk->p_field_buff[i].value.int_s32 = cp_data->arry[i];
      }
    }
    break;

  case WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY:
    p_wk->table_name_num  = 1;
    p_wk->p_field_buff[0].name  = SAKE_STAT_WIFICUP_POKEMON_PARTY;
    p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
    p_wk->p_field_buff[0].value.binary_data.data = (u8*)cp_wk_adrs;
    p_wk->p_field_buff[0].value.binary_data.size = WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE;
    break;

  case WIFIBATTLEMATCH_GDB_WRITE_WIFI_SCORE:
    { 
      const WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *cp_data  = cp_wk_adrs;

      p_wk->p_field_buff[0].name  = ATLAS_GET_KEY_NAME( ARENA_ELO_RATING_1V1_WIFICUP );
      p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[0].value.int_s32 = cp_data->rate;
      p_wk->p_field_buff[1].name  = ATLAS_GET_KEY_NAME( CHEATS_WIFICUP_COUNTER );
      p_wk->p_field_buff[1].type  = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[1].value.int_s32 = cp_data->cheat;
      p_wk->p_field_buff[2].name  = ATLAS_GET_KEY_NAME( DISCONNECTS_WIFICUP_COUNTER );
      p_wk->p_field_buff[2].type  = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[2].value.int_s32 = cp_data->disconnect;
      p_wk->p_field_buff[3].name  = ATLAS_GET_KEY_NAME( NUM_WIFICUP_LOSE_COUNTER );
      p_wk->p_field_buff[3].type  = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[3].value.int_s32 = cp_data->lose;
      p_wk->p_field_buff[4].name  = ATLAS_GET_KEY_NAME( NUM_WIFICUP_WIN_COUNTER );
      p_wk->p_field_buff[4].type  = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[4].value.int_s32 = cp_data->win;

      p_wk->table_name_num  = 5;
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  GDB�փf�[�^�������݃��C��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	DWCUserData *cp_user_data       ���[�U�[�f�[�^
 *	@param	*p_result                       �G���[�l
 *
 *	@return TRUE�ŏI��  FALSE�Ŕj��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_GDB_ProcessWrite( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START,
    SEQ_WAIT,
    SEQ_END,
    SEQ_EXIT,
  };

  DWCError    error;
  DWCGdbState state;

  *p_result = DWC_GDB_ERROR_NONE;

  if( p_wk->is_gdb_w_start )  
  { 
    switch( p_wk->seq )
    { 
    case SEQ_INIT:
      *p_result = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_w_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return TRUE;
      }
      DEBUG_NET_Printf( "GDB:Init\n" );
      p_wk->seq = SEQ_START;
      break;

    case SEQ_START:
      *p_result = DWC_GdbUpdateRecordAsync( WIFIBATTLEMATCH_NET_TABLENAME, p_wk->sake_record_id, p_wk->p_field_buff, p_wk->table_name_num );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_w_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return TRUE;
      }
      DEBUG_NET_Printf( "GDB:Get start\n" );
      p_wk->async_timeout = 0;
      p_wk->seq= SEQ_WAIT;
      break;

    case SEQ_WAIT:
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          GF_ASSERT(0);
          DwcRap_Gdb_Finalize( p_wk );
          return TRUE;
        }

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
            return TRUE;
          }
          else
          { 
            DEBUG_NET_Printf( "GDB:Get wait\n" );
            p_wk->seq = SEQ_END;
          }
        }
      }
      break;

    case SEQ_END:
      { 
        DWCGdbAsyncResult ret;
        ret = DWC_GdbGetAsyncResult();
        if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
        { 
          GF_ASSERT_MSG(0, "GDB:AsyncResult %d\n", ret);
          p_wk->is_gdb_w_start  = FALSE;
          DwcRap_Gdb_Finalize( p_wk );
          return TRUE;
        }
      }
      DEBUG_NET_Printf( "GDB:Get end\n" );
      p_wk->seq = SEQ_EXIT;
      break;

    case SEQ_EXIT:
      DwcRap_Gdb_Finalize( p_wk );
      DEBUG_NET_Printf( "GDB:Get exit\n" );
      DEBUG_NET_Printf( "sake �e�[�u��ID %d\n", p_wk->sake_record_id );
      return TRUE;

    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�C���^�C�v���擾
 *
 *	@param	DWCGdbError error   �G���[
 *
 *	@return �G���[�C���^�C�v
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_GDB_GetErrorRepairType( DWCGdbError error )
{ 
  OS_TPrintf( "WIFIBATTLEMATCH_GDB_GetErrorRepairType %d\n", error );
  switch( error )
  { 
    case DWC_GDB_ERROR_NONE :                 ///< ����ɏI�����܂����B
      return WIFIBATTLEMATCH_NET_ERROR_NONE;

    case DWC_GDB_ERROR_OUT_OF_MEMORY :        ///< case DWC_Init�ŗ^�����A���P�[�^���烁�������m�ۂł��܂���ł����B
      GF_ASSERT(0);
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

    case DWC_GDB_ERROR_NOT_AVAILABLE :        ///< ���p�\�ł͂Ȃ�
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

    case DWC_GDB_ERROR_CORE_SHUTDOWN :        ///< 
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_NOT_LOGIN :            ///< WiFiConnection�ɐ��������O�C�����Ă��܂���B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_NOT_AUTHENTICATED :    ///< WiFiConnection�ɐ��������O�C�����Ă��܂���B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_BAD_INPUT :            ///<
    case DWC_GDB_ERROR_BAD_TABLENAME :        ///<
    case DWC_GDB_ERROR_BAD_FIELDS :           ///<
    case DWC_GDB_ERROR_BAD_NUM_FIELDS :       ///<
    case DWC_GDB_ERROR_BAD_FIELD_NAME :      ///<
    case DWC_GDB_ERROR_BAD_FIELD_TYPE :      ///<
    case DWC_GDB_ERROR_BAD_FIELD_VALUE :     ///<
    case DWC_GDB_ERROR_BAD_OFFSET :          ///<
    case DWC_GDB_ERROR_BAD_MAX :             ///<
    case DWC_GDB_ERROR_BAD_RECORDIDS :       ///<
    case DWC_GDB_ERROR_BAD_NUM_RECORDIDS :   ///<
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

    case DWC_GDB_ERROR_UNKNOWN :             ///<
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

    case DWC_GDB_ERROR_ALREADY_INITIALIZED : ///< ���ɏ���������Ă��܂��B
      GF_ASSERT(0);
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_NOT_INITIALIZED :     ///< ���������s���Ă��܂���B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY;

    case DWC_GDB_ERROR_IN_ASYNC_PROCESS :    ///< �񓯊��������ł��B
      return WIFIBATTLEMATCH_NET_ERROR_NONE;

    case DWC_GDB_ERROR_GHTTP :               ///< GHTTP�̃G���[���������܂����B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_IN_DWC_ERROR :         ///< DWC�̃G���[���N�����Ă��܂��B�G���[�̑Ώ����s���Ă��������B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    default:
      GF_ASSERT(0);
      return 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�e�[�u�����쐬
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_GDB_StartCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  p_wk->is_gdb_start  = TRUE;
  p_wk->seq           = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�e�[�u�����쐬
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_result   �G���[����
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_GDB_ProcessCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk, DWCGdbError *p_result )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_CREATE_START,
    SEQ_CREATE_WAIT,
    SEQ_CREATE_END,
    SEQ_EXIT,
  };
  DWCGdbState state;

  if( p_wk->is_gdb_start )  
  { 
    switch( p_wk->seq )
    { 
    case SEQ_INIT:
	    *p_result = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
      if( *p_result != DWC_GDB_ERROR_NONE )
      { 
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return TRUE;
      }
      DEBUG_NET_Printf( "INIT:Init\n" );
      p_wk->seq = SEQ_CREATE_START;
      break;


	  case SEQ_CREATE_START:
      //�V�K�J�n�̂Ƃ��́A�܂�����ȏ�Ԃō쐬
	    *p_result = DWC_GdbCreateRecordAsync( WIFIBATTLEMATCH_NET_TABLENAME,
	                                      NULL, 
	                                      0, &p_wk->sake_record_id );
	    if( *p_result != DWC_GDB_ERROR_NONE )
	    { 
        DEBUG_NET_Printf( "INIT:Create Start\n" );
        p_wk->is_gdb_start  = FALSE;
        DwcRap_Gdb_Finalize( p_wk );
        return TRUE;
	    }
	    p_wk->async_timeout = 0;
	    p_wk->seq = SEQ_CREATE_WAIT;
	    break;
	
	  case SEQ_CREATE_WAIT:
	    { 
	      state = DWC_GdbGetState();
	
	      if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
	      { 
	        GF_ASSERT(0);

          p_wk->is_gdb_start  = FALSE;
          DwcRap_Gdb_Finalize( p_wk );
          return TRUE;
	      }
	
	      if( DWC_GDB_STATE_IN_ASYNC_PROCESS == state )
	      { 
	        DWC_GdbProcess();
	      }
	      else
	      { 
	        if( state != DWC_GDB_STATE_IDLE )
	        { 
            DEBUG_NET_Printf( "INIT:Create Wait\n" );
	          GF_ASSERT(0);

            p_wk->is_gdb_start  = FALSE;
	          DwcRap_Gdb_Finalize( p_wk );
	          return TRUE;
	        }
	        else
	        { 
            DEBUG_NET_Printf( "INIT:Create Wait\n" );
	          p_wk->seq = SEQ_CREATE_END;
	        }
	      }
	    }
	
	    break;
	
	  case SEQ_CREATE_END:
	    { 
	      DWCGdbAsyncResult ret;
	      ret = DWC_GdbGetAsyncResult();
	      if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
	      { 
	        GF_ASSERT_MSG(0, "INIT:Create %d\n", ret);
          p_wk->is_gdb_start  = FALSE;
	        DwcRap_Gdb_Finalize( p_wk );
	        return TRUE;
	      }
	    }
	    DEBUG_NET_Printf( "INIT:Create end\n" );
      p_wk->seq     = SEQ_EXIT;
      break;

    case SEQ_EXIT:
      DwcRap_Gdb_Finalize( p_wk );
      p_wk->is_gdb_start  = FALSE;
	    DEBUG_NET_Printf( "ITNI:Back exit\n" );
      DEBUG_NET_Printf( "�쐬! sake �e�[�u��ID %d\n", p_wk->sake_record_id );
	    return TRUE;
    }
  }

  return FALSE;
}
//=============================================================================
/**
 *    ���݂��̏��𑗂�  SENDDATA�n
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �G���
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *	@return TRUE�ő��M  FALSE�Ŏ��s
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
  BOOL ret  = p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA];

  if( ret )
  { 
    *pp_data  = (WIFIBATTLEMATCH_ENEMYDATA *)p_wk->recv_buffer;
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃|�P�p�[�e�B�𑗂�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	POKEPARTY *cp_party   ���鎩���̃|�P�p�[�e�B
 *
 *	@return TRUE�ő��M  FALSE�Ŏ��s
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SendPokeParty( WIFIBATTLEMATCH_NET_WORK *p_wk, const POKEPARTY *cp_party )
{ 
  NetID netID;

  //����ɂ̂ݑ��M
  netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
  netID = netID == 0? 1: 0;
  return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), netID, WIFIBATTLEMATCH_NETCMD_SEND_POKEPARTY, PokeParty_GetWorkSize(), cp_party, TRUE, TRUE, TRUE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����̃|�P�p�[�e�B��M�҂�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_party                        ���肩���������|�P�p�[�e�B
 *
 *	@return TRUE��M  FALSE��M��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_RecvPokeParty( WIFIBATTLEMATCH_NET_WORK *p_wk, POKEPARTY *p_party )
{ 
  BOOL ret;
  ret = p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_POKEPARTY];

  if( ret )
  { 
    GFL_STD_MemCopy( p_wk->recv_buffer, p_party, PokeParty_GetWorkSize() );
  }
  return ret;
}

//=============================================================================
/**
 *    �_�E�����[�h�n  �iND�n�j
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �������J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
extern void WIFIBATTLEMATCH_NET_StartDownloadDigCard( WIFIBATTLEMATCH_NET_WORK *p_wk, int cup_no )
{ 
  p_wk->is_nd_start = TRUE;
  p_wk->seq = 0;
  GFL_STD_MemClear( p_wk->nd_attr2, 10 );
  STD_TSPrintf( p_wk->nd_attr2, "%d", cup_no );
}
//----------------------------------------------------------------------------
/**
 *	@brief  Wifi����_�E�����[�h
 *  @param  p_wk    ���[�N
 *  @retval WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET���Q��
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET WIFIBATTLEMATCH_NET_WaitDownloadDigCard( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_ATTR,
    SEQ_FILENUM,
    SEQ_FILELIST,
    SEQ_GET_FILE,
    SEQ_GETTING_FILE,

    SEQ_CANCEL,
    SEQ_DOWNLOAD_COMPLETE,
    SEQ_END,

    SEQ_WAIT_CALLBACK         = 100,
  };

  if( !p_wk->is_nd_start )
  { 
    return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS;
  }

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    if( DWC_NdInitAsync( NdCallback, GF_DWC_ND_LOGIN, WIFI_ND_LOGIN_PASSWD ) == FALSE )
    {
      OS_TPrintf( "DWC_NdInitAsync: Failed\n" );
      //@todo
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    DwcRap_Nd_WaitNdCallback( p_wk, SEQ_ATTR );
    break;

  case SEQ_ATTR:
    // �t�@�C�������̐ݒ�
    if( DWC_NdSetAttr(WIFI_FILE_ATTR1, p_wk->nd_attr2, WIFI_FILE_ATTR3) == FALSE )
    {
      OS_TPrintf( "DWC_NdSetAttr: Failed\n." );
      GF_ASSERT(0);//@todo
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    p_wk->seq = SEQ_FILENUM;
    break;

//-------------------------------------
///	�T�[�o�[�̃t�@�C���`�F�b�N
//=====================================
  case SEQ_FILENUM:
    // �T�[�o�[�ɂ�����Ă���t�@�C���̐��𓾂�
    if( DWC_NdGetFileListNumAsync( &p_wk->server_filenum ) == FALSE )
    {
      OS_TPrintf( "DWC_NdGetFileListNumAsync: Failed.\n" );
      GF_ASSERT( 0 ); //@todo
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    DwcRap_Nd_WaitNdCallback( p_wk, SEQ_FILELIST );
    break;

  case SEQ_FILELIST:
    //�t�@�C�����Ȃ������ꍇ
    DEBUG_NET_Printf( "�T�[�o�[�Ƀ��M�����[�V�������������� %d\n", p_wk->server_filenum );
    if( p_wk->server_filenum == 1 )
    { 
      //�t�@�C�����P�����̏ꍇ����
      OS_TPrintf( "server_filenum %d\n", p_wk->server_filenum );
      if( DWC_NdGetFileListAsync( &p_wk->fileInfo, 0, 1 ) == FALSE)
      {
        OS_TPrintf( "DWC_NdGetFileListNumAsync: Failed.\n" );
        GF_ASSERT(0);
        return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
      }
      DwcRap_Nd_WaitNdCallback( p_wk, SEQ_GET_FILE );
    }
    else
    { 
      DEBUG_NET_Printf( "�T�[�o�[�Ƀ��M�����[�V�������������� %d\n", p_wk->server_filenum );
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    break;

//-------------------------------------
///	�t�@�C���ǂݍ��݊J�n
//=====================================
  case SEQ_GET_FILE:
    // �t�@�C���ǂݍ��݊J�n
    if(DWC_NdGetFileAsync( &p_wk->fileInfo, (char*)&p_wk->temp_buffer, REGULATION_CARD_DATA_SIZE) == FALSE){
      OS_TPrintf( "DWC_NdGetFileAsync: Failed.\n" );
      GF_ASSERT(0);
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    s_callback_flag   = FALSE;
    s_callback_result = DWC_ND_ERROR_NONE;
    p_wk->seq = SEQ_GETTING_FILE;
    break;
    
  case SEQ_GETTING_FILE:
    // �t�@�C���ǂݍ��ݒ�
    DWC_NdProcess();
    if( s_callback_flag == FALSE )
    {
      // �i�s�x��\��
      if(DWC_NdGetProgress( &p_wk->recived, &p_wk->contentlen ) == TRUE)
      {
        if(p_wk->percent != (p_wk->recived*100)/p_wk->contentlen)
        {
          p_wk->percent = (p_wk->recived*100)/p_wk->contentlen;
          OS_Printf( "Download %d/100\n", p_wk->percent );
        }
      }
    }
    else if( s_callback_result != DWC_ND_ERROR_NONE)
    {
      GF_ASSERT(0);
      OS_Printf("DWC_NdGetProgress�ŃG���[\n");
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    else
    {
      // �t�@�C���ǂݍ��ݏI��
      if( !DWC_NdCleanupAsync() ){  //FALSE�̏ꍇ�R�[���o�b�N���Ă΂�Ȃ�
        OS_Printf("DWC_NdCleanupAsync�Ɏ��s\n");
        return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
      }
      DwcRap_Nd_WaitNdCallback( p_wk, SEQ_CANCEL );
    }
    break;

//-------------------------------------
///	�I���E�L�����Z������
//=====================================
  case SEQ_CANCEL:
    if( DWC_NdCancelAsync() == FALSE )
    {
      p_wk->seq = SEQ_DOWNLOAD_COMPLETE;
    }
    else 
    {
      OS_Printf("download cancel\n");
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    break;

  case SEQ_DOWNLOAD_COMPLETE:
    DEBUG_NET_Printf( "��regulation card data\n" );
    DEBUG_NET_Printf( "cup no %d\n", p_wk->temp_buffer.no );
    DEBUG_NET_Printf( "status %d\n", p_wk->temp_buffer.status );


    p_wk->seq  = SEQ_END;
    break;

  case SEQ_END:
    //CRC�`�F�b�N
    if( Regulation_CheckCrc( &p_wk->temp_buffer ) )
    { 
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS;
    }
    else
    { 
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY;
    }

//-------------------------------------
///	�R�[���o�b�N�҂�����  
//�@�@WIFI_DOWNLOAD_WaitNdCallback���g���Ă�������
//=====================================
  case SEQ_WAIT_CALLBACK:
    //�R�[���o�b�N������҂�
    DWC_NdProcess();
    if( s_callback_flag )
    { 
      s_callback_flag = FALSE;
      if( s_callback_result != DWC_ND_ERROR_NONE)
      { 
        return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
      }
      else
      { 
        p_wk->seq  = p_wk->next_seq;
      }
    }
    break;
  }

  return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_DOWNLOADING;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�����f�W�^���I��؂��擾
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_recv   �擾�o�b�t�@
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_GetDownloadDigCard( const WIFIBATTLEMATCH_NET_WORK *cp_wk, REGULATION_CARDDATA *p_recv )
{ 
  Regulation_PrintDebug( &cp_wk->temp_buffer );
  GFL_STD_MemCopy( &cp_wk->temp_buffer, p_recv, sizeof(REGULATION_CARDDATA) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �R�[���o�b�N�҂�
 *
 *	@param	*p_wk                     ���[�N
 *	@param	next_seq                  �R�[���o�b�N������ւ����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void DwcRap_Nd_WaitNdCallback( WIFIBATTLEMATCH_NET_WORK *p_wk, int next_seq )
{ 
  s_callback_flag   = FALSE;
  s_callback_result = DWC_ND_ERROR_NONE;
  p_wk->next_seq    = next_seq;
  p_wk->seq         = 100;
}

/*-------------------------------------------------------------------------*
 * Name        : NdCallback
 * Description : ND�p�R�[���o�b�N
 * Arguments   : None.
 * Returns     : None.
 *-------------------------------------------------------------------------*/
static void NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror)
{
  DEBUG_NET_Printf("NdCallback: Called\n");
  switch(reason) {
  case DWC_ND_CBREASON_GETFILELISTNUM:
    DEBUG_NET_Printf("DWC_ND_CBREASON_GETFILELISTNUM\n");
    break;
  case DWC_ND_CBREASON_GETFILELIST:
    DEBUG_NET_Printf("DWC_ND_CBREASON_GETFILELIST\n");
    break;
  case DWC_ND_CBREASON_GETFILE:
    DEBUG_NET_Printf("DWC_ND_CBREASON_GETFILE\n");
    break;
  case DWC_ND_CBREASON_INITIALIZE:
    DEBUG_NET_Printf("DWC_ND_CBREASON_INITIALIZE\n");
    break;
  }
	
  switch(error) {
  case DWC_ND_ERROR_NONE:
    DEBUG_NET_Printf("DWC_ND_NOERR\n");
    break;
  case DWC_ND_ERROR_ALLOC:
    DEBUG_NET_Printf("DWC_ND_MEMERR\n");
    break;
  case DWC_ND_ERROR_BUSY:
    DEBUG_NET_Printf("DWC_ND_BUSYERR\n");
    break;
  case DWC_ND_ERROR_HTTP:
    DEBUG_NET_Printf("DWC_ND_HTTPERR\n");
    // �t�@�C�����̎擾�łg�s�s�o�G���[�����������ꍇ�̓_�E�����[�h�T�[�o�Ɍq�����Ă��Ȃ��\��������
    if( reason == DWC_ND_CBREASON_GETFILELISTNUM )
      {
          DEBUG_NET_Printf( "It is not possible to connect download server.\n." );
          ///	OS_Terminate();
      }
    break;
  case DWC_ND_ERROR_BUFFULL:
    DEBUG_NET_Printf("DWC_ND_BUFFULLERR\n");
    break;
  case DWC_ND_ERROR_DLSERVER:
    DEBUG_NET_Printf("DWC_ND_SERVERERR\n");
    break;
  case DWC_ND_ERROR_CANCELED:
    DEBUG_NET_Printf("DWC_ND_CANCELERR\n");
    break;
  }
  OS_Printf("errorcode = %d\n", servererror);
  s_callback_flag   = TRUE;
  s_callback_result = error;

  //NdCleanupCallback();
}
//=============================================================================
/**
 *    GPF�T�[�o����̎�M
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o����̎�M �X�^�[�g
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, HEAPID heapID )
{ 
  GF_ASSERT( p_wk->p_nhttp == NULL );
  p_wk->seq = 0;
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID, MyStatus_GetProfileID(p_mystatus), NULL);
}
//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o�[����̎�M ���C������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *
 *	@return TRUE�Ŏ�M  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_RECV_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, NHTTPError *p_error )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_CONNECT,
    SEQ_WAIT_CONNECT,

    SEQ_END,
  };

  if( p_wk->p_nhttp == NULL )
  { 
    return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS;
  }

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    if (NHTTP_RAP_ConectionCreate( NHTTPRAP_URL_BATTLE_DOWNLOAD, p_wk->p_nhttp ))
    {
      p_wk->seq  = SEQ_START_CONNECT;
    }
    break;

  case SEQ_START_CONNECT:
    if(NHTTP_RAP_StartConnect(p_wk->p_nhttp))
    {
      p_wk->seq  = SEQ_WAIT_CONNECT;
    }
    break;

  case SEQ_WAIT_CONNECT:
    *p_error  = NHTTP_RAP_Process(p_wk->p_nhttp);
    if(NHTTP_ERROR_NONE == *p_error)
    {
      u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(p_wk->p_nhttp);
      DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA* pDream = (DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA*)&pEvent[sizeof(gs_response)];


      NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
      DEBUG_NET_Printf("ID %x\n",pDream->WifiMatchUpID);
      DEBUG_NET_Printf("FLG %x\n",pDream->GPFEntryFlg);
      DEBUG_NET_Printf("ST %d\n",pDream->WifiMatchUpState);

      DEBUG_NET_Printf("�I��\n");

      p_wk->gpf_data  = *pDream;
      p_wk->seq  = SEQ_END;
    }
    else if( NHTTP_ERROR_BUSY != *p_error )
    { 
      return WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR;
    }

    break;


  case SEQ_END :
    if(p_wk->p_nhttp)
    {
      NHTTP_RAP_End(p_wk->p_nhttp);
      p_wk->p_nhttp  = NULL;
    }
    return WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS;
  }

  return WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o�[����̎�M ��M�����f�[�^���󂯎��
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk ���[�N
 *	@param	*p_recv �󂯎��o�b�t�@
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_GetRecvGpfData( const WIFIBATTLEMATCH_NET_WORK *cp_wk, DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_recv )
{ 
  *p_recv = cp_wk->gpf_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o�[�֏������� ������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk                        ���[�N
 *	@param	*p_mystatus                                           �����̃X�e�[�^�X
 *	@param	DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *cp_send    �������݃f�[�^
 *	@param	heapID                                                �q�[�vID
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, const DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA *cp_send, HEAPID heapID )
{ 
  GF_ASSERT( p_wk->p_nhttp == NULL );
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID, MyStatus_GetProfileID(p_mystatus), NULL);
  p_wk->gdb_write_data  = *cp_send;
  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o�[�֏������� ���C������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_error                        �G���[�l
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_SEND_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, NHTTPError *p_error )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_CONNECT,
    SEQ_WAIT_CONNECT,

    SEQ_END,
  };

  if( p_wk->p_nhttp == NULL )
  { 
    return TRUE;
  }

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    if (NHTTP_RAP_ConectionCreate( NHTTPRAP_URL_BATTLE_DOWNLOAD, p_wk->p_nhttp ))
    {
      NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(p_wk->p_nhttp), &p_wk->gdb_write_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      p_wk->seq  = SEQ_START_CONNECT;
    }
    break;

  case SEQ_START_CONNECT:
    if(NHTTP_RAP_StartConnect(p_wk->p_nhttp))
    {
      p_wk->seq  = SEQ_WAIT_CONNECT;
    }
    break;

  case SEQ_WAIT_CONNECT:
    *p_error  = NHTTP_RAP_Process(p_wk->p_nhttp);
    if(NHTTP_ERROR_NONE == *p_error)
    {
      u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(p_wk->p_nhttp);
      NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
      DEBUG_NET_Printf("GPF�T�[�o�[�ւ̏������ݏI��\n");

      p_wk->seq  = SEQ_END;
    }
    else if( NHTTP_ERROR_BUSY != *p_error )
    { 
      return WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR;
    }

    break;


  case SEQ_END :
    if(p_wk->p_nhttp)
    {
      NHTTP_RAP_End(p_wk->p_nhttp);
      p_wk->p_nhttp  = NULL;
    }
    return WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS;
  }

  return WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE;
}
//=============================================================================
/**
 *    �s������
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���X�^�[�g
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	STRCODE *cp_str                 �`�F�b�N����STRCODE
 *	@param	str_len                         STRCODE�̒���
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, const STRCODE *cp_str, u32 str_len )
{ 
  BOOL ret;
/*
  # �����R�[�h��Unicode�i���g���G���f�B�A��UTF16�j���g�p���Ă��������B
  ����ȊO�̕����R�[�h���g�p���Ă���ꍇ�́AUnicode�ɕϊ����Ă��������B
  # �X�N���[���l�[����Unicode�y��IPL�t�H���g�ɂȂ��Ǝ��̕������g�p���Ă���ꍇ�́A�X�y�[�X�ɒu�������Ă��������B
  # �I�[��"\0\0"�iu16��0x0000�j�ł���K�v������܂��B
  # �z����̑S�Ă̕�����̍��v��501�����܂Łi�e������̏I�[���܂ށj�ɂ���K�v������܂��B
  # �z����̕������NULL���w�肷�邱�Ƃ͂ł��܂���B 
  */
  { 
    int i;
    GF_ASSERT( str_len < WIFIBATTLEMATCH_BADWORD_STRL_MAX );
    for( i = 0; i < str_len; i++ )
    { 
      if( GFL_STR_GetEOMCode() == cp_str[i] )
      {
        p_wk->badword_str[i]  = 0x0000;
      }
      else
      { 
        p_wk->badword_str[i]  = cp_str[i];
      }
    }
    p_wk->p_badword_arry[0] = p_wk->badword_str;
  }
  p_wk->badword_num = 0;


  ret = DWC_CheckProfanityExAsync( (const u16 **)p_wk->p_badword_arry,
                             1,
                             NULL,
                             0,
                             p_wk->badword_result,
                             &p_wk->badword_num,
                             DWC_PROF_REGION_ALL );
  GF_ASSERT( ret );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���C��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk    ���[�N
 *	@param	*p_is_bad_word                    TRUE�Ȃ�Εs������  FALSE�Ȃ�ΐ��핶��
 *
 *	@return TRUE�����I��  FALSE������
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL *p_is_bad_word )
{ 
  BOOL ret;
  ret = DWC_CheckProfanityProcess() == DWC_PROF_STATE_SUCCESS;

  if( ret == TRUE )
  { 
    *p_is_bad_word  = p_wk->badword_num;
  }
  return ret;
}
