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
#include "gamesystem/game_data.h"

//  �l�b�g���[�N
#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_rapcommon.h"
#include "system/net_err.h"
#include "net/dwc_error.h"
#include "net/nhttp_rap.h"
#include "net/nhttp_rap_evilcheck.h"
#include "net/dwc_tool.h"

#include "msg/msg_wifi_system.h"

//  �Z�[�u�f�[�^
#include "savedata/system_data.h"
#include "savedata/wifilist.h"

//  �A�g���X���
#include "atlas_syachi2ds_v1.h"

//  �O�����J
#include "wifibattlematch_net.h"

//  �f�o�b�O�p�Ƀ^�C�~���O��m�点�邽��SE��炷�̂�
#ifdef PM_DEBUG
#include "sound/pm_sndsys.h"
#include "wifibattlematch_snd.h"
#endif

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_DEBUG_NET_Printf_ON //�S���҂݂̂̃v�����g�\����ON
#define DEBUGWIN_OWNER_NET_ONLY   //�S���҂͎����Ƃ����Ȃ���Ȃ��Ȃ�
#define DEBUG_SC_RELEASE_CHECK    //SC�̊J���`�F�b�N
#define DEBUG_GDB_RELEASE_CHECK   //GDB�̊J���`�F�b�N
//#define DEBUG_EVAL_FREE           //�]���l�v�Z�ŕK���q����悤�ɂ���
#endif //PM_DEBUG

//�S���҂͎��������Ȃ���Ȃ��Ȃ�
#ifdef DEBUGWIN_OWNER_NET_ONLY
#if defined(DEBUG_ONLY_FOR_shimoyamada)
#undef  MATCHINGKEY
#define MATCHINGKEY 0xFF
#elif defined(DEBUG_ONLY_FOR_toru_nagihashi)
#undef  MATCHINGKEY
#define MATCHINGKEY 0xFE
#endif  //defined
#endif // DEBUGWIN_OWNER_NET_ONLY

//�S���҂݂̂̃v�����g�\��
#ifdef DEBUG_DEBUG_NET_Printf_ON

#if defined(DEBUG_ONLY_FOR_toru_nagihashi)
#define DEBUG_NET_Printf(...)  OS_TFPrintf(1,__VA_ARGS__)
#elif defined(DEBUG_ONLY_FOR_shimoyamada)
#define DEBUG_NET_Printf(...)  OS_TPrintf(__VA_ARGS__)
#else //defined(DEBUG_ONLY_FOR_�`
#define DEBUG_NET_Printf(...)  /*  */
#endif  //defined(DEBUG_ONLY_FOR_�`

#else //DEBUG_DEBUG_NET_Printf_ON

#ifdef WIFIMATCH_RATE_AUTO
#undef DEBUGPRINT_ON
#define DEBUG_NET_Printf(...)  OS_TFPrintf(1,__VA_ARGS__)
#else
#define DEBUG_NET_Printf(...)  /*  */
#endif
#endif  //DEBUG_DEBUG_NET_Printf_ON

//SC�̊J���`�F�b�N�`�F�b�N
#ifdef DEBUG_SC_RELEASE_CHECK
static int sc_release_cnt   = 0;
#define DEBUG_SC_RELEASE_Increment {DEBUG_NET_Printf( "sc_release cnt=%d\n", ++sc_release_cnt );}  
#define DEBUG_SC_RELEASE_Decrement {DEBUG_NET_Printf( "sc_release cnt=%d\n", --sc_release_cnt );}  
#else //DEBUG_SC_RELEASE_CHECK
#define DEBUG_SC_RELEASE_Increment /*  */
#define DEBUG_SC_RELEASE_Decrement /*  */
#endif //DEBUG_SC_RELEASE_CHECK

//GDB�̊J���`�F�b�N
#ifdef DEBUG_GDB_RELEASE_CHECK
static int gdb_release_cnt   = 0;
#define DEBUG_GDB_RELEASE_Increment {DEBUG_NET_Printf( "gdb_release cnt=%d\n", ++gdb_release_cnt );}  
#define DEBUG_GDB_RELEASE_Decrement {DEBUG_NET_Printf( "gdb_release cnt=%d\n", --gdb_release_cnt );}  
#else //DEBUG_GDB_RELEASE_CHECK
#define DEBUG_GDB_RELEASE_Increment /*  */
#define DEBUG_GDB_RELEASE_Decrement /*  */
#endif //DEBUG_GDB_RELEASE_CHECK



//#define SC_DIVIDE_SESSION
//-------------------------------------
///	�}�N���X�C�b�`
//=====================================

//-------------------------------------
///	�l�b�g�萔
//=====================================
#define WIFIBATTLEMATCH_NET_SSL_TYPE DWC_SSL_TYPE_SERVER_AUTH

#define WIFIBATTLEMATCH_NET_TABLENAME "PlayerStats_v1" 

//�ȉ��AATLAS�o�R�ł͂Ȃ��ASAKE���ڃA�N�Z�X�̏ꍇ
#define SAKE_STAT_RECORDID              "recordid"
#define SAKE_STAT_INITIAL_PROFILE_ID    "INITIAL_PROFILE_ID"
#define SAKE_STAT_NOW_PROFILE_ID        "NOW_PROFILE_ID"
#define SAKE_STAT_LAST_LOGIN_DATETIME   "LAST_LOGIN_DATETIME"
#define SAKE_STAT_WIFICUP_POKEMON_PARTY "WIFICUP_POKEMON_PARTY"
#define SAKE_STAT_MYSTATUS              "MYSTATUS"
#define SAKE_STAT_RECORD_DATA_01        "RECORD_DATA_01"
#define SAKE_STAT_RECORD_DATA_02        "RECORD_DATA_02"
#define SAKE_STAT_RECORD_DATA_03        "RECORD_DATA_03"
#define SAKE_STAT_RECORD_DATA_04        "RECORD_DATA_04"
#define SAKE_STAT_RECORD_DATA_05        "RECORD_DATA_05"
#define SAKE_STAT_RECORD_DATA_06        "RECORD_DATA_06"
#define SAKE_STAT_RECORD_SAVE_IDX       "RECORD_SAVE_IDX"

static const char* scp_stat_record_data_tbl[] =
{ 
  SAKE_STAT_RECORD_DATA_01,
  SAKE_STAT_RECORD_DATA_02,
  SAKE_STAT_RECORD_DATA_03,
  SAKE_STAT_RECORD_DATA_04,
  SAKE_STAT_RECORD_DATA_05,
  SAKE_STAT_RECORD_DATA_06,
};

//-------------------------------------
///	SC�̃V�[�P���X
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_SC_SEQ_SEND_TIMING_INIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_TIMING_WAIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_INIT,
  WIFIBATTLEMATCH_SC_SEQ_SEND_WAIT,
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
  WIFIBATTLEMATCH_SC_SEQ_END_TIMING_START,
  WIFIBATTLEMATCH_SC_SEQ_END_TIMING_WAIT,
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

  WIFIBATTLEMATCH_GDB_SEQ_CANCEL_START,
  WIFIBATTLEMATCH_GDB_SEQ_CANCEL_WAIT,
  WIFIBATTLEMATCH_GDB_SEQ_CANCEL_END,
} WIFIBATTLEMATCH_GDB_SEQ;

//-------------------------------------
///	�^�C�~���O
//=====================================
#define WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING     (1)
#define WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING   (2)
#define WIFIBATTLEMATCH_SC_REPORT_TIMING              (3)
#define WIFIBATTLEMATCH_SC_DIRTY_TIMING               (4)
#define WIFIBATTLEMATCH_SC_END_TIMING                 (5)
#define WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT        (15)

//-------------------------------------
///	���̑�
//=====================================
#define TIMEOUT_MS            100       // HTTP�ʐM�̃^�C���A�E�g����
#define PLAYER_NUM            2          // �v���C���[��
#define TEAM_NUM              0          // �`�[����
#define CANCELSELECT_TIMEOUT (60*60)     //�L�����Z���Z���N�g�^�C���A�E�g
#define ASYNC_TIMEOUT (120*60)     //�񓯊��p�^�C���A�E�g
#define DISCONNECTS_WAIT_SYNC      (30)  //�ؒf�����p

#define RECV_BUFFER_SIZE  (0x1000)

#define WBM_GDB_FIELD_TABLE_MAX (38)

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
  MATCHMAKE_KEY_BTLCNT,
  MATCHMAKE_KEY_VERLANG,

  MATCHMAKE_KEY_MAX,
} MATCHMAKE_KEY;

//�}�b�`���C�N�^�C�~���O�V���N

//�}�b�`���C�N�]���v�Z���̒萔
#define MATCHMAKE_EVAL_CALC_RATE_STANDARD       (1000)  //�萔A
#define MATCHMAKE_EVAL_CALC_WEIGHT              (1)     //�萔B
#define MATCHMAKE_EVAL_CALC_DISCONNECT_WEIGHT   (32)    //�萔C
#define MATCHMAKE_EVAL_CALC_DISCONNECT_REVISE   (5)     //�萔D
#define MATCHMAKE_EVAL_CALC_DISCONNECT_RATE     (50)    //�萔E

#ifdef DEBUG_EVAL_FREE
#undef MATCHMAKE_EVAL_CALC_RATE_STANDARD
#define MATCHMAKE_EVAL_CALC_RATE_STANDARD (65535)
#endif //DEBUG_EVAL_FREE

//-------------------------------------
///	��M�t���O
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER,
  WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA,
  WIFIBATTLEMATCH_NET_RECVFLAG_CANCELREQUEST,
  WIFIBATTLEMATCH_NET_RECVFLAG_POKEPARTY,
  WIFIBATTLEMATCH_NET_RECVFLAG_DIRTYCNT,
  WIFIBATTLEMATCH_NET_RECVFLAG_FLAG,

  WIFIBATTLEMATCH_NET_RECVFLAG_MAX
} WIFIBATTLEMATCH_NET_RECVFLAG;

//-------------------------------------
///	�A�g���r���[�g
//=====================================
#define WIFI_FILE_ATTR1			"REGCARD_J"
//#define WIFI_FILE_ATTR2		//�����ɑ��ԍ��������Ă���
#define WIFI_FILE_ATTR3			""
#define REGULATION_CARD_DATA_SIZE (sizeof(REGULATION_CARDDATA))

//-------------------------------------
///	�f�o�b�O
//=====================================
#include "wifibattlematch_debug.h"

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�������ݏ��
//=====================================
typedef struct 
{
  WIFIBATTLEMATCH_BTLRULE   btl_rule;
  BtlResult btl_result;
  u32      is_dirty;
} DWC_SC_WRITE_DATA;
//SC�̃��|�[�g�쐬�R�A�֐�
typedef DWCScResult (*DWCRAP_SC_CREATE_REPORT_CORE_FUNC)( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );

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
///	�G���[
//=====================================
typedef struct 
{
  DWCScResult                 sc_err;
  DWCGdbError                 gdb_err;
  DWCGdbState                 gdb_state;
  DWCGdbAsyncResult           gdb_result;
  DWCNdError                  nd_err;
  NHTTPError                  nhttp_err;
  WIFIBATTLEMATCH_NET_SYSERROR  sys_err;
  WIFIBATTLEMATCH_NET_ERRORTYPE type;
} WIFIBATTLEMATCH_NETERR_WORK;


typedef void (*DWCGdbGetRecordsCallbackEx)(int record_num, DWCGdbField** records, void* user_param, int field_num);

//-------------------------------------
///	�l�b�g���W���[��
//=====================================
struct _WIFIBATTLEMATCH_NET_WORK
{ 
  const DWCUserData *cp_user_data;
  WIFI_LIST         *p_wifilist;
  GAMEDATA          *p_gamedata;
  s32               pid;
  DWCSvlResult      *p_svl_result;
  s32               timing_no;

  //�G���[
  WIFIBATTLEMATCH_NETERR_WORK error;
  
  //����
  WBM_NET_DATETIME  datetime;

  //�}�b�`���O
  u32 seq_matchmake;
  char filter[128];
  MATCHMAKE_KEY_WORK  key_wk[ MATCHMAKE_KEY_MAX ];


  HEAPID heapID;
  u32   init_seq;
  u32   seq;
  u32   next_seq;
  u32   wait_cnt;
  BOOL  is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_MAX];
  u32   recv_flag;
  u32   disconnect_seq;
  
  //SC
//  DWC_SC_PLAYERDATA player[2];  //�����͂O ����͂P
  DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc;
  DWCScResult       result;
  BOOL is_sc_error;
  BOOL is_session;
  BOOL is_sc_you_recv;
  BOOL is_send_report;
  BOOL is_sc_start;

  //�|�P�����s�������Ŏg��
  u32             poke_max;

  //gdb
  void *p_get_wk;
  DWCGdbGetRecordsCallbackEx  gdb_get_record_callback;
  DWC_SC_WRITE_DATA report;
  DWCGdbField *p_field_buff;
  const char **pp_table_name;
  int table_name_num;
  u32   magic_num;
  u32 async_timeout;
  u32 cancel_select_timeout;   //CANCELSELECT_TIMEOUT
  BOOL is_auth;
  WIFIBATTLEMATCH_SC_REPORT_TYPE report_type;
  int get_recordID;
  u16 playername_unicode[PERSON_NAME_SIZE + EOM_SIZE];
  WIFIBATTLEMATCH_GDB_RND_RECORD_DATA record_data;

  //SENDDATA�n
  u8 recv_buffer[RECV_BUFFER_SIZE];//��M�o�b�t�@

  //�}�b�`���C�N
  DWCEvalPlayerCallback matchmake_eval_callback;

  //GPF�T�[�o�[
  NHTTP_RAP_WORK* p_nhttp;
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA gpf_data;
  DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA gdb_write_data;

  //�ȉ��_�E�����[�h�p
  REGULATION_CARDDATA temp_buffer;
  DWCNdFileInfo fileInfo;
  u32               percent;
  u32               recived;
  u32               contentlen;
  char              nd_attr2[10];

  //�ȉ��o�b�N�A�b�v�p
  s32 init_profileID;
  s32 now_profileID;
  
  WIFIBATTLEMATCH_NET_DATA *p_data;

  //�ȉ��s�������`�F�b�N�p
  DWC_TOOL_BADWORD_WORK badword;
  MYSTATUS              *p_badword_mystatus;

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
static u8 s_cleanup_callback_flag   = FALSE;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void WifiBattleMatch_SetDateTime( WBM_NET_DATETIME *p_wk );
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
static void WifiBattleMatch_RecvDirtyCnt(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);
static void WifiBattleMatch_RecvFlag(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle);

//-------------------------------------
///	SC�֌W
//=====================================
//DWC�R�[���o�b�N
static void DwcRap_Sc_CreateSessionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SetReportIntentionCallback( DWCScResult theResult, void* theUserData );
static void DwcRap_Sc_SubmitReportCallback( DWCScResult theResult, void* theUserData );
//private
static void DwcRap_Sc_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data, BOOL is_auth, DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc, BOOL is_disconnect, WIFIBATTLEMATCH_SC_REPORT_TYPE report_type, BOOL is_sc_you_recv );
static DWCScResult DwcRap_Sc_CreateReportDebug( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const WIFIBATTLEMATCH_SC_DEBUG_DATA * cp_data, BOOL is_auth );
//���|�[�g�쐬�R�[���o�b�N
static DWCScResult DwcRap_Sc_CreateReportRndCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
static DWCScResult DwcRap_Sc_CreateReportWifiCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
static DWCScResult DwcRap_Sc_CreateReportRndCoreBtlAfter( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
static DWCScResult DwcRap_Sc_CreateReportWifiCoreBtlAfter( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
static DWCScResult DwcRap_Sc_CreateReportRndCoreBtlError( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
static DWCScResult DwcRap_Sc_CreateReportWifiCoreBtlError( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect );
//-------------------------------------
///	GDB�֌W
//=====================================
static void DwcRap_Gdb_GetCallback(int record_num, DWCGdbField** records, void* user_param);

static void DwcRap_Gdb_Rnd_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num );
static void DwcRap_Gdb_Wifi_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num );
static void DwcRap_Gdb_RecordID_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num);
static void DwcRap_Gdb_PokeParty_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num);
static void DwcRap_Gdb_LoginDate_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num );
static void DwcRap_Gdb_SakeAll_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num);
static void DwcRap_Gdb_Finalize( WIFIBATTLEMATCH_NET_WORK *p_wk );
static void DwcRap_Gdb_SetMyInfo( WIFIBATTLEMATCH_NET_WORK *p_wk );


#ifdef PM_DEBUG
static void print_field(DWCGdbField* field); // ���R�[�h���f�o�b�O�o�͂���B
#else
#define print_field(...) /*  */
#endif

//-------------------------------------
///	�_�E�����[�h�֌W
//=====================================
static void DwcRap_Nd_WaitNdCallback( WIFIBATTLEMATCH_NET_WORK *p_wk, int next_seq );
static void DwcRap_Nd_CleanUpNdCallback( WIFIBATTLEMATCH_NET_WORK *p_wk, int next_seq );
static void NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror);

//-------------------------------------
///	�G���[�֌W
//=====================================
static void WIFIBATTLEMATCH_NETERR_Clear( WIFIBATTLEMATCH_NETERR_WORK *p_wk );
static void WIFIBATTLEMATCH_NETERR_Main( WIFIBATTLEMATCH_NETERR_WORK *p_wk );
static void WIFIBATTLEMATCH_NETERR_SetSysError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, WIFIBATTLEMATCH_NET_SYSERROR error, int line );
static void WIFIBATTLEMATCH_NETERR_SetScError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCScResult sc_err, int line );
static void WIFIBATTLEMATCH_NETERR_SetGdbError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbError gdb_err, int line );
static void WIFIBATTLEMATCH_NETERR_SetGdbState( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbState state, int line );
static void WIFIBATTLEMATCH_NETERR_SetGdbResult( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbAsyncResult result, int line );
static void WIFIBATTLEMATCH_NETERR_SetNdError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCNdError nd_err, int line );
static void WIFIBATTLEMATCH_NETERR_SetNhttpError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, NHTTPError nhttp_err, int line );
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SC_GetErrorRepairType( DWCScResult error );
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_GDB_GetErrorRepairType( DWCGdbError error, DWCGdbState gdb_state, DWCGdbAsyncResult gdb_result );
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_ND_GetErrorRepairType( DWCNdError nd_err );
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SYS_GetErrorRepairType( WIFIBATTLEMATCH_NET_SYSERROR  sys_err );
static void WIFIBATTLEMATCH_NETERR_ClearError( WIFIBATTLEMATCH_NETERR_WORK *p_wk );


//-------------------------------------
///	���̑�
//=====================================
static BOOL UTIL_IsClear( void *p_adrs, u32 size );

static void WifiBattleMatch_ErrDisconnectCallback(void* p_wk_adrs, int code, int type, int ret );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
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
  "cnt",
  "ver",
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
  WIFIBATTLEMATCH_NETCMD_SEND_DIRTYCNT,
  WIFIBATTLEMATCH_NETCMD_SEND_FLAG,
  WIFIBATTLEMATCH_NETCMD_MAX,
};
static const NetRecvFuncTable sc_net_recv_tbl[] = 
{
  {DwcRap_Sc_RecvPlayerData,   NULL},    ///_NETCMD_INFOSEND
  {WifiBattleMatch_RecvEnemyData, WifiBattleMatch_RecvBuffAddr },
  {WifiBattleMatch_RecvMatchCancel, NULL},
  {WifiBattleMatch_RecvPokeParty, WifiBattleMatch_RecvBuffAddr },
  {WifiBattleMatch_RecvDirtyCnt, WifiBattleMatch_RecvBuffAddr },
  {WifiBattleMatch_RecvFlag, NULL },
};

//-------------------------------------
///	�T�P�S���
//=====================================
static const char *sc_get_debugall_tbl[]  =
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
  "NUM_SINGLE_WIN_COUNTER",
  "NUM_TRIPLE_LOSE_COUNTER",
  "NUM_TRIPLE_WIN_COUNTER",
  "ARENA_ELO_RATING_1V1_WIFICUP",
  "CHEATS_WIFICUP_COUNTER",
  "DISCONNECTS_WIFICUP_COUNTER",
  "NUM_WIFICUP_LOSE_COUNTER",
  "NUM_WIFICUP_WIN_COUNTER",
  "WIFICUP_POKEMON_PARTY",
  "LAST_LOGIN_DATETIME",
  "MYSTATUS",
  "RECORD_DATA_01",
  "RECORD_DATA_02",
  "RECORD_DATA_03",
  "RECORD_DATA_04",
  "RECORD_DATA_05",
  "RECORD_DATA_06",
  "RECORD_SAVE_IDX",
  "recordid",
  "ownerid",
  "INITIAL_PROFILE_ID",
  "NOW_PROFILE_ID",
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
 *  @param  GAMEDATA        WiFIList��Mystatus�擾�p
 *  @param  DWCSvlResult    WiFILogin����������SVL�B�s���T�[�o�[���g��Ȃ��Ȃ��NULL�ł��悢
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( WIFIBATTLEMATCH_NET_DATA *p_data, GAMEDATA *p_gamedata, DWCSvlResult *p_svl_result, HEAPID heapID )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NET_WORK) );

  p_wk->p_wifilist    = GAMEDATA_GetWiFiList( p_gamedata );
  p_wk->cp_user_data  = WifiList_GetMyUserInfo( p_wk->p_wifilist );
  p_wk->pid           = MyStatus_GetProfileID( GAMEDATA_GetMyStatus( p_gamedata ) );
  p_wk->p_gamedata    = p_gamedata;
  p_wk->p_svl_result  = p_svl_result;
  p_wk->p_data        = p_data;

  p_wk->magic_num = 0x573;

#ifdef DEBUG_ONLY_FOR_toru_nagihashi
  GFL_NET_DebugGetPrintOn();
#endif

  p_wk->p_field_buff  = GFL_HEAP_AllocMemory( heapID, WBM_GDB_FIELD_TABLE_MAX * sizeof(DWCGdbField) );
  GFL_STD_MemClear(p_wk->p_field_buff, WBM_GDB_FIELD_TABLE_MAX * sizeof(DWCGdbField) );

#ifdef PM_DEBUG
  GFL_NET_DebugPrintOn();
#endif //PM_DEBUG

  if(!GFL_NET_IsInit())
  {
    GF_ASSERT(0);
  }

  GFL_NET_AddCommandTable( GFL_NET_CMD_WIFIMATCH, sc_net_recv_tbl, NELEMS(sc_net_recv_tbl), p_wk );
  GFL_NET_SetWifiBothNet(FALSE);

  GFL_NET_DWC_SetErrDisconnectCallback( WifiBattleMatch_ErrDisconnectCallback, p_wk );

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
  GFL_NET_DWC_SetErrDisconnectCallback( NULL, NULL );

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
 *	@brief  �^�C�~���O�Ƃ�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	timing_no                         �^�C�~���ONO
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartTiming( WIFIBATTLEMATCH_NET_WORK *p_wk, int timing_no )
{ 
  p_wk->timing_no = timing_no;
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), p_wk->timing_no, WB_NET_WIFIMATCH);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����҂�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return TRUE�œ������Ƃ���  FALSE�łƂ��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitTiming( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  return GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),p_wk->timing_no,WB_NET_WIFIMATCH);
}

//----------------------------------------------------------------------------
/**
 *	@brief  DWC�̃G���[�C���^�C�v���擾
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return �C���^�C�v���擾
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_NET_CheckErrorRepairType( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_heavy, BOOL is_timeout )
{ 
  WIFIBATTLEMATCH_NETERR_WORK           *p_error  = &p_wk->error;
  WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE repair  = WIFIBATTLEMATCH_NET_ERROR_NONE;


  //���P�̓G���[��ʂ̌�ɂ��ł��܂��Ƃ܂������߃G���[��ʂ̑O�ŌĂ�
  //->�G���[��ʂŃG���[�����ɂ��遨������disconnect
  //  ��disconnect���ŃG���[�N���A


  //DWC�̃G���[
  if( GFL_NET_IsInit() )
  { 
    //���P
    if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
    {
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
    }

    //���L�֐���dev_wifilib�̃I�[�o�[���C�ɂ���̂ŁAGFL_NET����������Ƃ�΂�Ȃ��Ȃ�
    switch( GFL_NET_DWC_ERROR_ReqErrorDisp(is_heavy, is_timeout) )
    { 
    case GFL_NET_DWC_ERROR_RESULT_NONE:
      repair  = WIFIBATTLEMATCH_NET_ERROR_NONE;
      break;

    case GFL_NET_DWC_ERROR_RESULT_PRINT_MSG:
      repair  = WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
      break;

    case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC:
      repair  = WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
      break;

    case GFL_NET_DWC_ERROR_RESULT_FATAL:
      repair  = WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL;
      break;

    case GFL_NET_DWC_ERROR_RESULT_TIMEOUT:
      repair  = WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT;
      break;
    }
    if( repair != GFL_NET_DWC_ERROR_RESULT_NONE )
    {
      DEBUG_NET_Printf( "DWC�̃G���[���m %d\n",repair );
    }

    //�G���[��ʂƃG���[�̉��������łɂ��Ă���̂Ŕ�����
    return repair;
  }

  //�ʂ̃G���[
  if( repair == WIFIBATTLEMATCH_NET_ERROR_NONE )
  { 
    BOOL is_clear = FALSE;
    switch( p_error->type )
    { 
    case WIFIBATTLEMATCH_NET_ERRORTYPE_NONE:
      repair  = WIFIBATTLEMATCH_NET_ERROR_NONE;
      break;

    case WIFIBATTLEMATCH_NET_ERRORTYPE_SC:
      repair  = WIFIBATTLEMATCH_SC_GetErrorRepairType( p_error->sc_err );
      break;

    case WIFIBATTLEMATCH_NET_ERRORTYPE_GDB:
      repair  = WIFIBATTLEMATCH_GDB_GetErrorRepairType( p_error->gdb_err, p_error->gdb_state, p_error->gdb_result );
      break;

    case WIFIBATTLEMATCH_NET_ERRORTYPE_ND:
      repair  = WIFIBATTLEMATCH_NET_ERROR_NONE;
      break;

    case WIFIBATTLEMATCH_NET_ERRORTYPE_NHTTP:
      //�V�[�P���X�̖߂�l�ŃG���[��Ԃ��̂ł����ł͍s��Ȃ�
      repair  = WIFIBATTLEMATCH_NET_ERROR_NONE;//WIFIBATTLEMATCH_ND_GetErrorRepairType( p_error->nd_err );
      break;

    case WIFIBATTLEMATCH_NET_ERRORTYPE_SYS:
      repair  = WIFIBATTLEMATCH_SYS_GetErrorRepairType( p_error->sys_err );
      break;

    default:
      repair  =  WIFIBATTLEMATCH_NET_ERROR_NONE;
      break;
    }

    if( repair != GFL_NET_DWC_ERROR_RESULT_NONE )
    {
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
      DEBUG_NET_Printf( "���C�u�����ʂ̃G���[���m %d\n",repair );
    }

    switch( repair )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:      //���A�\�n�_�܂Ŗ߂�
      GFL_NET_StateResetError();
      GFL_NET_StateClearWifiError();
      NetErr_DispCallPushPop();
      NetErr_ErrWorkInit();
      break;
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��

      //  ����DWC�̃G���[���Ȃ������ꍇ�G���[�𗧂Ă�
      //  �V�X�e���G���[��GDB�̌��ʓ��ꕔ��DWC�̃G���[�������Ȃ��̂�
      if( NET_ERR_CHECK_NONE == NetErr_App_CheckError() )
      {
        GFL_NET_StateSetWifiError(0, 0, 0, ERRORCODE_SYSTEM );
        NetErr_ErrorSet();
      }

      NetErr_App_ReqErrorDisp();
      break;
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
      NetErr_DispCallFatal();
      break;
    }

    if( repair != WIFIBATTLEMATCH_NET_ERROR_NONE )
    {
      WIFIBATTLEMATCH_NETERR_ClearError( p_error );
    }
  }

  return repair;
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
  p_wk->seq       = 0;
  p_wk->init_seq  = 0;
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
WIFIBATTLEMATCH_GDB_RESULT WIFIBATTLEMATCH_NET_WaitInitialize( WIFIBATTLEMATCH_NET_WORK *p_wk )
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
    WIFIBATTLEMATCH_GDB_Start( p_wk, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_RECORDID, &p_wk->p_data->sake_recordID );
    p_wk->init_seq  = SEQ_WAIT_GET_TABLE_ID;
    break;

  case SEQ_WAIT_GET_TABLE_ID:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        if( p_wk->p_data->sake_recordID != 0 )
        { 
          p_wk->init_seq  = SEQ_END;
        }
        else
        { 
          DEBUG_NET_Printf( "�e�[�u��ID��0�������̂ŁA�V�K�쐬���܂�\n" );
          p_wk->init_seq  = SEQ_START_CREATE_RECORD;
        }
      }
      else if( res == WIFIBATTLEMATCH_GDB_RESULT_ERROR )
      {
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
    }
    break;

  case SEQ_START_CREATE_RECORD:
    WIFIBATTLEMATCH_GDB_StartCreateRecord( p_wk );
    p_wk->init_seq  = SEQ_WAIT_CREATE_RECORD;
    break;

  case SEQ_WAIT_CREATE_RECORD:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_ProcessCreateRecord( p_wk );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        p_wk->init_seq  = SEQ_END;
      }
      else if( res == WIFIBATTLEMATCH_GDB_RESULT_ERROR )
      {
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
    }
    break;


  case SEQ_END:
    DEBUG_NET_Printf( "sake �e�[�u��ID %d\n", p_wk->p_data->sake_recordID );
    return WIFIBATTLEMATCH_GDB_RESULT_SUCCESS;
  }

  return WIFIBATTLEMATCH_GDB_RESULT_UPDATE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_TYPE mode, BOOL is_rnd_rate, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_MATCH_KEY_DATA *cp_data )
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
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTLCNT, cp_data->btlcnt );
  MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_VERLANG, PM_VERSION | (PM_LANG << 16) ); //@todo forupper �����A�b�p�[�o�[�W�����Ńo�g���T�[�o�[�o�[�W�������オ�����炱�̒l�����Čq����Ȃ��悤�ɂ���
  STD_TSPrintf( p_wk->filter, "mod=%d And rul=%d And deb=%d", btl_mode, btl_rule, MATCHINGKEY );
  OS_TFPrintf( 3, "%s\n", p_wk->filter );
  p_wk->seq_matchmake = 0;
  p_wk->async_timeout = 0;
  p_wk->cancel_select_timeout = 0;

  //�ڑ��]���R�[���o�b�N�w��
  switch( mode )
  { 
  case WIFIBATTLEMATCH_TYPE_WIFICUP:    //WIFI���
    p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_WIFI_Eval_Callback;
    break;
  case WIFIBATTLEMATCH_TYPE_LIVECUP:    //���C�u���
    GF_ASSERT( 0 );
    break;
  case WIFIBATTLEMATCH_TYPE_RNDRATE:  //�����_�����[�g�ΐ�
    p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_RATE_Eval_Callback;
    break;
  case WIFIBATTLEMATCH_TYPE_RNDFREE:  //�����_���t���[�ΐ�
    p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_FREE_Eval_Callback;
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
  p_wk->seq_matchmake = 0;

  p_wk->matchmake_eval_callback = WIFIBATTLEMATCH_RND_FREE_Eval_Callback;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *  @ret  �I���R�[�h
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_MATCHMAKE_STATE WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  {
    WIFIBATTLEMATCH_NET_SEQ_MATCH_START,
    WIFIBATTLEMATCH_NET_SEQ_MATCH_START2,
    WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT,

    WIFIBATTLEMATCH_NET_SEQ_CONNECT_START,
    WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD,
    WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT,
    WIFIBATTLEMATCH_NET_SEQ_TIMING_END,

    WIFIBATTLEMATCH_NET_SEQ_CANCEL,

    WIFIBATTLEMATCH_NET_SEQ_MATCH_END,
  };

  //DEBUG_NET_Printf( "WiFiState %d \n", GFL_NET_StateGetWifiStatus() );
  switch( p_wk->seq_matchmake )
  { 
  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START:
    {
      if( GFL_NET_DWC_StartMatchFilter( p_wk->filter, 2, p_wk->matchmake_eval_callback, p_wk ) != 0 )
      {
        GFL_NET_StateStartWifiRandomMatch_RateMode();
        GFL_NET_DWC_SetVChat( FALSE );
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START2;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START2:
    if( 1 )
    { 
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT:
    {
      int ret;
#if 1
      if(GFL_NET_STATE_MATCHED == GFL_NET_StateGetWifiStatus())
      {
        p_wk->cancel_select_timeout = 0;
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_START;
        GFL_NET_SetAutoErrorCheck(TRUE);
        GFL_NET_SetNoChildErrorCheck(TRUE);
      }
#else
      ret = GFL_NET_DWC_GetStepMatchResult();
      switch(ret)
      {
      case STEPMATCH_CONNECT:
      case STEPMATCH_CONTINUE:
        /* �ڑ��� */
        break;

      case STEPMATCH_SUCCESS:
        p_wk->cancel_select_timeout = 0;
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_START;
        break;

      case STEPMATCH_CANCEL:
      case STEPMATCH_FAIL:
        return WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED;

      default:
        GF_ASSERT_MSG(0,"StepMatchResult=[%d]\n",ret);
      }
#endif
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_START:
    {
      if( GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE )
      {  // �q�@�Ƃ��Đڑ�����������
        if( GFL_NET_HANDLE_RequestNegotiation() )
        {
          p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD;
        }

        if( p_wk->cancel_select_timeout++ > CANCELSELECT_TIMEOUT )
        { 
          DEBUG_NET_Printf( "�}�b�`���O�l�S�V�G�[�V�����^�C���A�E�gline %d\n", __LINE__ );

          WIFIBATTLEMATCH_NET_StartDisConnect( p_wk );
          p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CANCEL;
        }
      }
      else
      {
        p_wk->cancel_select_timeout = 0;
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT;
      }
      DEBUG_NET_Printf( "�ڑ����� %d\n",GFL_NET_SystemGetCurrentID() );
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD:
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT, WB_NET_WIFIMATCH);
    p_wk->cancel_select_timeout = 0;
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT:
    if( p_wk->cancel_select_timeout++ > CANCELSELECT_TIMEOUT )
    { 
      DEBUG_NET_Printf( "�}�b�`���O�l�S�V�G�[�V�����^�C���A�E�gline %d\n", __LINE__ );
      WIFIBATTLEMATCH_NET_StartDisConnect( p_wk );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CANCEL;
    }
    else if( GFL_NET_HANDLE_GetNumNegotiation() != 0 )
    {
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT, WB_NET_WIFIMATCH);
        p_wk->cancel_select_timeout = 0;
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_TIMING_END:
    if( p_wk->cancel_select_timeout++ > CANCELSELECT_TIMEOUT )
    { 
      DEBUG_NET_Printf( "�}�b�`���O�l�S�V�G�[�V�����^�C���A�E�gline %d\n", __LINE__ );
      WIFIBATTLEMATCH_NET_StartDisConnect( p_wk );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CANCEL;
    }
    else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT,WB_NET_WIFIMATCH))
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_END;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CANCEL:
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk ) )
    {
      //�}�b�`���O�^�C���A�E�g�̂Ƃ�������̂�

      { 
        const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();
        if( cp_error->errorUser == ERRORCODE_TIMEOUT )
        {
          GFL_NET_StateClearWifiError();  //WIFI�G���[�ڍ׏��N���A
          GFL_NET_StateResetError();      //NET���G���[���N���A
          NetErr_ErrWorkInit();           //�G���[�V�X�e�����N���A
          GFL_NET_StateWifiMatchEnd(TRUE);
        }
      }
      return WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED;
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
    return WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS;
  }

  return WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_UPDATE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�������ؒf����  �i�G���[���N�����Ƃ��ȂǂɎg���j
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_SetDisConnectForce( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(FALSE);

  GFL_NET_StateWifiMatchEnd(TRUE);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���O�����ؒf�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  p_wk->disconnect_seq  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���O�����ؒf������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Őؒf�I��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  {
    SEQ_START_INIT_TIMEING,
    SEQ_WAIT_INIT_TIMEING,
    SEQ_SET_DISCONNECT_FLAG,
    SEQ_WAIT_CNT,
    SEQ_DISCONNECT,
    SEQ_END,
  };

  switch( p_wk->disconnect_seq )
  {
  case SEQ_START_INIT_TIMEING:
    WIFIBATTLEMATCH_NET_StartTiming( p_wk, WIFIBATTLEMATCH_NET_TIMINGSYNC_DISCONNECT_INIT );
    p_wk->disconnect_seq++;
    break;
  case SEQ_WAIT_INIT_TIMEING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk ) )
    {
      p_wk->disconnect_seq++;
    }
    break;
  case SEQ_SET_DISCONNECT_FLAG:
    GFL_NET_SetAutoErrorCheck(FALSE);
    GFL_NET_SetNoChildErrorCheck(FALSE);
    p_wk->wait_cnt  = 0;
    p_wk->disconnect_seq++;
    break;
  case SEQ_WAIT_CNT:
    if( p_wk->wait_cnt++> DISCONNECTS_WAIT_SYNC )
    {
      p_wk->disconnect_seq++;
    }
    break;
  case SEQ_DISCONNECT:
    GFL_NET_StateWifiMatchEnd(TRUE);
    p_wk->disconnect_seq++;
    break;
  case SEQ_END:
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������L�����Z���������Ƃ𑊎�֒ʒm����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *
 *	@return TRUE�ő��M����  FALSE�ő��M�p��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SendMatchCancel( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  static const int sc_temp  = 0;

  {
    if( GFL_NET_DWC_IsMatched() )
    {
      return GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), 
          WIFIBATTLEMATCH_NETCMD_SEND_CANCELMATCH, sizeof(int), &sc_temp );
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���肪�L�����Z���������`�F�b�N
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk ���[�N
 *
 *	@return TRUE�ő��肪�L�����Z������  FALSE�ł��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_RecvMatchCancel( WIFIBATTLEMATCH_NET_WORK *p_wk )
{
  BOOL  ret;
  ret = p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_CANCELREQUEST];

  if( ret )
  {
    p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_CANCELREQUEST] = FALSE;
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���O�C�����Ԃ���A���݂̓��������o��
 *
 *	@param	WBM_NET_DATETIME *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_SetDateTime( WBM_NET_DATETIME *p_wk )
{ 
  RTCDate date;
  RTCTime time;
  BOOL ret  = DWC_GetDateTime(&date, &time);
  GF_ASSERT( ret );

  GFL_STD_MemClear( p_wk, sizeof(WBM_NET_DATETIME) );

  p_wk->year  = date.year;
  p_wk->month = date.month;
  p_wk->day   = date.day;
  p_wk->hour  = time.hour;
  p_wk->minute= time.minute;
  p_wk->second= time.second;
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
  DEBUG_NET_Printf( "�t���[���[�h�]���R�[���o�b�N�I\n" );
  return 1000;

}
//WIFI���p
static int WIFIBATTLEMATCH_WIFI_Eval_Callback( int index, void* p_param_adrs )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_param_adrs;

  int value=0;
  int rate, disconnect, cup, btlcnt;

  int you_rate      = DWC_GetMatchIntValue( index, p_wk->key_wk[MATCHMAKE_KEY_RATE].keyStr, -1 );
  disconnect= DWC_GetMatchIntValue(index, p_wk->key_wk[MATCHMAKE_KEY_DISCONNECT].keyStr, -1 );
  btlcnt    = DWC_GetMatchIntValue(index, p_wk->key_wk[MATCHMAKE_KEY_BTLCNT].keyStr, -1 );
  cup       = DWC_GetMatchIntValue( index, p_wk->key_wk[MATCHMAKE_KEY_CUPNO].keyStr, -1 );

  //�w�W�L�[�����f����Ă��Ȃ��̂ŁA��������
  if( you_rate == -1 && disconnect == -1 && cup == -1 && btlcnt == -1 )
  { 
    DEBUG_NET_Printf( "���w�W�L�[�����f����Ă��Ȃ�\n" );
    return 0;
  }

  if( cup == p_wk->key_wk[MATCHMAKE_KEY_CUPNO].ivalue )
  {
    int my_disconnect_rate;
    int you_disconnect_rate;
    int disconnect_rate;

    //�ؒf�䗦�����߂�
    you_disconnect_rate = MATCHMAKE_EVAL_CALC_DISCONNECT_RATE *
      disconnect / (btlcnt+MATCHMAKE_EVAL_CALC_DISCONNECT_REVISE);

    my_disconnect_rate  = MATCHMAKE_EVAL_CALC_DISCONNECT_RATE * 
      p_wk->key_wk[MATCHMAKE_KEY_DISCONNECT].ivalue / 
      (p_wk->key_wk[MATCHMAKE_KEY_BTLCNT].ivalue + MATCHMAKE_EVAL_CALC_DISCONNECT_REVISE);

    //�������߂�
    rate        = you_rate;
    rate        -= p_wk->key_wk[MATCHMAKE_KEY_RATE].ivalue;
    rate        = MATH_ABS( rate );
    disconnect_rate  = you_disconnect_rate - my_disconnect_rate;
    disconnect_rate  = MATH_ABS( disconnect_rate );

    //�]���v�Z��
    //�}�b�`���O�]���l  ���@�i�萔A - ���[�e�B���O���j �� �萔B�@�[�@�ؒf�䗦�� �� �萔C
    value = (MATCHMAKE_EVAL_CALC_RATE_STANDARD - rate) * MATCHMAKE_EVAL_CALC_WEIGHT
      - disconnect_rate * MATCHMAKE_EVAL_CALC_DISCONNECT_WEIGHT;

    value = MATH_IMax( value, 0 );
    DEBUG_NET_Printf( "��---------------�]��--------------��\n" );
    DEBUG_NET_Printf( "�����̏�� ���[�g%d �ؒf�� %d ������%d �J�b�v%d\n", 
        p_wk->key_wk[MATCHMAKE_KEY_RATE].ivalue, p_wk->key_wk[MATCHMAKE_KEY_DISCONNECT].ivalue, 
        p_wk->key_wk[MATCHMAKE_KEY_BTLCNT].ivalue, p_wk->key_wk[MATCHMAKE_KEY_CUPNO].ivalue );
    DEBUG_NET_Printf( "����̏�� ���[�g%d �ؒf�� %d ������%d �J�b�v%d\n", 
        you_rate, disconnect, 
        btlcnt, cup );

    DEBUG_NET_Printf( "�v�Z���� �]���l%d ���[�g%d �ؒf�䗦(��) %d�ؒf�䗦�i���j%d \n", value, rate, my_disconnect_rate, you_disconnect_rate );
    DEBUG_NET_Printf( "��---------------------------------��\n" );
  }
  else
  { 
    value = 0;
    DEBUG_NET_Printf( "���]���l�I %d cup���قȂ��� %d<>%d\n", value, p_wk->key_wk[MATCHMAKE_KEY_CUPNO].ivalue, cup );
  }

  return value;

}

//=============================================================================
/**
 *    ���v�E�����֌W
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g���M�̊J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  WIFIBATTLEMATCH_BTLRULE rule              ���[��
 *	@param  BtlResult result          �ΐ팋��
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_SC_StartReport( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_SC_REPORT_TYPE type, WIFIBATTLEMATCH_TYPE mode, WIFIBATTLEMATCH_BTLRULE rule, const BATTLEMATCH_BATTLE_SCORE *cp_btl_score, BOOL is_error )
{ 
  p_wk->is_debug  = FALSE;
  p_wk->seq = 0;
  p_wk->is_auth   = TRUE;//GFL_NET_IsParentMachine();

  //�^�C�~���O�V���N������̂ł����ł̃t���O������OK
  p_wk->is_recv[ WIFIBATTLEMATCH_NET_RECVFLAG_FLAG ]  = FALSE;
  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = FALSE;
  p_wk->report_type = type;
  p_wk->is_sc_error = is_error;
  p_wk->is_session  = FALSE;
  p_wk->is_sc_you_recv  = FALSE;
  p_wk->is_send_report  = FALSE;

  if( type == WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER )
  { 
    switch( mode )
    { 
    case WIFIBATTLEMATCH_TYPE_WIFICUP:    //WIFI���
      p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportWifiCoreBtlAfter;
      break;
    case WIFIBATTLEMATCH_TYPE_RNDRATE:    //�����_���}�b�`���[�e�B���O
      p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportRndCoreBtlAfter;
      break;
    default:
      GF_ASSERT(0);
      break;
    }
  }
  else if( type == WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_SCORE )
  { 
    if( p_wk->is_sc_error )
    {
      //�퓬���ɃG���[�����Ƃ������ƂȂ̂ŁA�X�R�A���͂������A�����̐ؒf�J�E���^�[�̂ݑ���
      switch( mode )
      { 
      case WIFIBATTLEMATCH_TYPE_WIFICUP:    //WIFI���
        p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportWifiCoreBtlError;
        break;
      case WIFIBATTLEMATCH_TYPE_RNDRATE:    //�����_���}�b�`���[�e�B���O
        p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportRndCoreBtlError;
        break;
      default:
        GF_ASSERT(0);
        break;
      }
    }
    else
    {
      if( GFL_NET_IsParentMachine() )
      { 
        DEBUG_NET_Printf( "�I�e�@ ���[��%d ����%d\n", rule, cp_btl_score->result );
      }
      else
      { 
        DEBUG_NET_Printf( "�I�q�@ ���[��%d ����%d\n", rule, cp_btl_score->result );
      }

      p_wk->report.btl_rule   = rule;
      p_wk->report.btl_result = cp_btl_score->result;
      p_wk->report.is_dirty   = cp_btl_score->is_dirty;

      switch( mode )
      { 
      case WIFIBATTLEMATCH_TYPE_WIFICUP:    //WIFI���
        p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportWifiCore;
        break;
      case WIFIBATTLEMATCH_TYPE_RNDRATE:    //�����_���}�b�`���[�e�B���O
        p_wk->SC_CreateReportCoreFunc = DwcRap_Sc_CreateReportRndCore;
        break;
      default:
        GF_ASSERT(0);
        break;
      }
    }
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
  p_wk->seq           = 0;
  p_wk->is_debug      = TRUE;
  p_wk->is_auth = is_auth;
  p_wk->p_debug_data  = p_data;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g���M�̏�����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param  BOOL *is_send_report  ���|�[�g���M���s������
 *	@retval WIFIBATTLEMATCH_NET_SC_STATE�񋓂��Q��
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_SC_STATE WIFIBATTLEMATCH_SC_ProcessReport( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL *p_is_send_report )
{ 
  DWCScResult ret;

  if( p_is_send_report )
  {
    *p_is_send_report = p_wk->is_send_report;
  }
  { 
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();
    //�}�b�`���O����Ɛؒf�����ꍇ
    if( cp_error->errorUser == ERRORCODE_TIMEOUT
        || cp_error->errorUser == ERRORCODE_DISCONNECT )
    { 
      DEBUG_NET_Printf( "�^�C���A�E�g!!!!\n" );
      p_wk->is_sc_error = TRUE;
    }

    //DWC���x���Őؒf�G���[�������ꍇ
    if( NetErr_App_CheckError() == NET_ERR_CHECK_HEAVY ) 
    {
      if( cp_error->errorType == DWC_ETYPE_DISCONNECT
          || cp_error->errorType == DWC_ETYPE_SHUTDOWN_FM )
      {
        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      }
    }

    if( p_wk->is_sc_error )
    {
      //�����G���[�Ȃ̂Ɏ������Z�b�V�������쐬���Ă��Ȃ������ꍇ��
      //�Z�b�V�����쐬�ɖ߂�
      //(�o�g���O���|�[�g�ȊO)
      if( p_wk->report_type != WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER &&
          p_wk->is_session == FALSE && 
           WIFIBATTLEMATCH_SC_SEQ_SESSION_WAIT < p_wk->seq && p_wk->seq < WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA )
      {

        p_wk->is_sc_you_recv  = FALSE;
        DEBUG_NET_Printf( "�Z�b�V�������Ȃ��̂ō��Ȃ����܂�!\n" );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SESSION_START;
      }
    }

    switch( p_wk->seq )
    { 
    case WIFIBATTLEMATCH_SC_SEQ_SEND_TIMING_INIT:

      if( p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INIT;
      }
      else
      { 
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_SC_DIRTY_TIMING, WB_NET_WIFIMATCH);
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_TIMING_WAIT;
      }
      break;

    case WIFIBATTLEMATCH_SC_SEQ_SEND_TIMING_WAIT:
      if( p_wk->is_sc_error)
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INIT;
      }
      else
      { 
        if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_SC_DIRTY_TIMING,WB_NET_WIFIMATCH) || p_wk->is_sc_error )
        { 
          //�s���t���O�𑡂肠��
           
            p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_INIT;
        }
      }
      break;

    case WIFIBATTLEMATCH_SC_SEQ_SEND_INIT:
      if( p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INIT;
      }
      else
      { 
        if( WIFIBATTLEMATCH_NET_SendBtlDirtyFlag( p_wk, &p_wk->report.is_dirty )
            || p_wk->is_sc_error )
        { 
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_WAIT;
        }
      }
      break;

    case WIFIBATTLEMATCH_SC_SEQ_SEND_WAIT:
      if( p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INIT;
      }
      else
      { 
        if( WIFIBATTLEMATCH_NET_RecvBtlDirtyFlag( p_wk, &p_wk->report.is_dirty )
            || p_wk->is_sc_error )
        { 
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INIT;
        }
      }
      break;

    //������
    case WIFIBATTLEMATCH_SC_SEQ_INIT:
      p_wk->wait_cnt  = 0;
      p_wk->is_sc_start = TRUE;
      DEBUG_NET_Printf( "sc is_sc_start == true\n" );
      DEBUG_NET_Printf( "SC:sc Init\n" );
      ret = DWC_ScInitialize( GAME_ID,DWC_SSL_TYPE_SERVER_AUTH );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      }
      DEBUG_NET_Printf( "SC:Init end\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_LOGIN;
      break;

    //�F�؏��擾
    case WIFIBATTLEMATCH_SC_SEQ_LOGIN:
#ifdef SC_DIVIDE_SESSION
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START;
#else
      ret = DWC_ScGetLoginCertificate( &p_wk->p_data->sc_player[0].mCertificate );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      }
      DEBUG_NET_Printf( "SC:Login\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SESSION_START;
#endif 
      break;

    //�Z�b�V�����J�n  �i�z�X�g���s���A�N���C�A���g�ɃZ�b�V����ID��`�d����j
    case WIFIBATTLEMATCH_SC_SEQ_SESSION_START:
      if( GFL_NET_IsParentMachine() || p_wk->is_sc_error )
      { 
        p_wk->wait_cnt++;
        ret = DWC_ScCreateSessionAsync( DwcRap_Sc_CreateSessionCallback, TIMEOUT_MS, p_wk );
        p_wk->is_session  = TRUE;
        if( ret != DWC_SC_RESULT_NO_ERROR )
        { 
          WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }
        DEBUG_NET_Printf( "SC:Session parent\n" );
        p_wk->async_timeout = 0;
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
        GFL_STD_MemCopy( DWC_ScGetSessionId(), p_wk->p_data->sc_player[0].mSessionId, DWC_SC_SESSION_GUID_SIZE );
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START;
      }
      else
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ ); 
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }

        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, p_wk->result, __LINE__ );
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }
      }
      break;

    //�z�X�g���N���C�A���g�փZ�b�V����ID�𑗂邽�߂̃^�C�~���O�Ƃ�
   case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_START:
      if( p_wk->is_sc_error )
      { 
        //�؂�Ă���ꍇ�̓C���e���V�����܂Ŕ��
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_START;
      }
      else
      { 
        GFL_STD_MemCopy( DWC_ScGetConnectionId(), p_wk->p_data->sc_player[0].mConnectionId, DWC_SC_CONNECTION_GUID_SIZE );

        GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
        p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT;
      }
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_SEND_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) || p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA;
      }
      break;

    //�z�X�g���N���C�A���g�֎����̃f�[�^�𑗂�
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA:
      if( GFL_NET_IsParentMachine() || p_wk->is_sc_error )
      { 
        //�����𑗐M
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->p_data->sc_player[0]) || p_wk->is_sc_error )
        { 
          DEBUG_NET_Printf( "SC:Senddata parent\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_START;
        }
      }
      else
      { 
        //�󂯎��
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] || p_wk->is_sc_error )
        { 
          p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] = FALSE;
          //�z�X�g�ȊO�̐l�́A�z�X�g��CreateSession����sessionId��ݒ肷��
          ret = DWC_ScSetSessionId( p_wk->p_data->sc_player[1].mSessionId );
          GFL_STD_MemCopy( DWC_ScGetSessionId(), p_wk->p_data->sc_player[0].mSessionId, DWC_SC_SESSION_GUID_SIZE );
          if( ret != DWC_SC_RESULT_NO_ERROR )
          {
            WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
            DwcRap_Sc_Finalize( p_wk );
            return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
          }
          DEBUG_NET_Printf( "SC:Senddata child\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_INTENTION_START;
        }
      }
      break;

    //���M���郌�|�[�g�̎�ނ�ʒm
    case WIFIBATTLEMATCH_SC_SEQ_INTENTION_START:
      p_wk->wait_cnt++;
      ret = DWC_ScSetReportIntentionAsync( p_wk->is_auth, DwcRap_Sc_SetReportIntentionCallback, TIMEOUT_MS, p_wk );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );

        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      } 
      DEBUG_NET_Printf( "SC:intention start\n" );
      p_wk->async_timeout = 0;
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
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ ); 
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }

        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, p_wk->result, __LINE__ );
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }
      }
      break;

    //�N���C�A���g�̏����z�X�g�����炤���߂̃^�C�~���O�Ƃ�
    case WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_START:
      if( p_wk->is_sc_error )
      { 
        //�؂�Ă���ꍇ�̓��|�[�g�쐬�܂Ŕ��
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT;
      }
      else
      { 
        GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH );
        p_wk->seq  = WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT;
      }
      break;

    //�^�C�~���O�҂�
    case WIFIBATTLEMATCH_SC_SEQ_RETURN_PLAYERDATA_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_RETURN_PLAYERDATA_TIMING, WB_NET_WIFIMATCH ) || p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT;
      }
      break;

    //���݂���CCID�iIntention ConnectionID�j����������  �܂��͐e�f�[�^���q�ɑ��M
    case WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_PARENT:
      if( GFL_NET_IsParentMachine() )
      { 
        if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_PLAYERDATA, 
              sizeof(DWC_SC_PLAYERDATA), &p_wk->p_data->sc_player[0]) || p_wk->is_sc_error )
        { 
          DEBUG_NET_Printf( "SC:send data ccid parent\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SEND_PLAYERDATA_CCID_CHILD;
        }
      }
      else
      { 
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] || p_wk->is_sc_error )
        { 
          if( !p_wk->is_sc_error )
          {
            p_wk->is_sc_you_recv  = TRUE;
          }

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
              sizeof(DWC_SC_PLAYERDATA), &p_wk->p_data->sc_player[0]) || p_wk->is_sc_error )
        { 
          DEBUG_NET_Printf( "SC:send data ccid child\n" );
          p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT_TIMING_START;
        }
      }
      else
      { 
        if( p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_PLAYER] || p_wk->is_sc_error )
        { 
          if( !p_wk->is_sc_error )
          {
            p_wk->is_sc_you_recv  = TRUE;
          }

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
      if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_REPORT_TIMING, WB_NET_WIFIMATCH )  || p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT;
      }
      break;

    //���|�[�g�쐬
    case WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT:
      //���|�[�g�쐬
#ifdef PM_DEBUG
      if( !p_wk->is_debug )
#else
      if(1)
#endif
      { 
        ret = DwcRap_Sc_CreateReport( &p_wk->p_data->sc_player[0], &p_wk->p_data->sc_player[1], &p_wk->report, p_wk->is_auth, p_wk->SC_CreateReportCoreFunc, p_wk->is_sc_error, p_wk->report_type, p_wk->is_sc_you_recv );
      }
      else
      { 
        ret = DwcRap_Sc_CreateReportDebug( &p_wk->p_data->sc_player[0], &p_wk->p_data->sc_player[1], p_wk->p_debug_data, p_wk->is_auth );
      }

      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      }
      DEBUG_NET_Printf( "SC:create report\n" );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_START;
      break;

    //���|�[�g���M�J�n
    case WIFIBATTLEMATCH_SC_SEQ_SUBMIT_REPORT_START:
      p_wk->wait_cnt++;
      ret = DWC_ScSubmitReportAsync( p_wk->p_data->sc_player[0].mReport, p_wk->is_auth,
            DwcRap_Sc_SubmitReportCallback, TIMEOUT_MS, p_wk );
      if( ret != DWC_SC_RESULT_NO_ERROR )
      { 
        WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, ret, __LINE__ );
        DwcRap_Sc_Finalize( p_wk );
        return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
      }
      DEBUG_NET_Printf( "SC:submit report start\n" );
      p_wk->async_timeout = 0;
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
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ ); 
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }

        DWC_ScProcess();
        if( p_wk->result != DWC_SC_RESULT_NO_ERROR )
        {
          WIFIBATTLEMATCH_NETERR_SetScError( &p_wk->error, p_wk->result, __LINE__ );
          DwcRap_Sc_Finalize( p_wk );
          return WIFIBATTLEMATCH_NET_SC_STATE_FAILED;
        }
      }
      break;

    //�I���I
    case WIFIBATTLEMATCH_SC_SEQ_EXIT:
#ifdef PM_DEBUG
      if( *DEBUGWIN_REPORTMARK_GetFlag() )
      {
        PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );
      }
#endif //PM_DEBUG

      p_wk->is_send_report  = TRUE;
      DEBUG_NET_Printf( "SC:exit\n" );
      DwcRap_Sc_Finalize( p_wk );
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_END_TIMING_START;
      break;

    case WIFIBATTLEMATCH_SC_SEQ_END_TIMING_START:
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_SC_END_TIMING, WB_NET_WIFIMATCH);
      p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_END_TIMING_WAIT;
      break;
    case WIFIBATTLEMATCH_SC_SEQ_END_TIMING_WAIT:
      if( GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_SC_END_TIMING,WB_NET_WIFIMATCH) || p_wk->is_sc_error )
      { 
        p_wk->seq = WIFIBATTLEMATCH_SC_SEQ_END;
      }
      break;
    case WIFIBATTLEMATCH_SC_SEQ_END:
      return WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS;
    }
  }

  return WIFIBATTLEMATCH_NET_SC_STATE_UPDATE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g���Ɏ󂯎�����s���t���O���擾
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk   ���[�N
 *
 *	@return �s���t���O
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SC_GetDirtyFlag( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{
  return cp_wk->recv_flag;
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

#ifdef PM_DEBUG
#if 0
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    theResult = DWC_SC_RESULT_HTTP_ERROR;
  }
#endif
#endif 

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
  DWC_SC_PLAYERDATA   *p_player_data  = &p_wk->p_data->sc_player[0];

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
  if( p_wk->is_sc_start == TRUE )
  {
    if( p_wk->seq > WIFIBATTLEMATCH_SC_SEQ_CREATE_REPORT )
    {
      DEBUG_NET_Printf( "destroy report\n" );
      DWC_ScDestroyReport( p_wk->p_data->sc_player[0].mReport );
    }

    if( p_wk->seq >= WIFIBATTLEMATCH_SC_SEQ_INIT )
    {
      DEBUG_NET_Printf( "sc shutdown\n" );
      DWC_ScShutdown();
    }

    p_wk->is_sc_start = FALSE;
    DEBUG_NET_Printf( "sc is_sc_start == false\n" );
  }
  DEBUG_NET_Printf( "sc end shutdown\n" );
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
static DWCScResult DwcRap_Sc_CreateReport( DWC_SC_PLAYERDATA *p_my, DWC_SC_PLAYERDATA *p_other, const DWC_SC_WRITE_DATA *cp_data, BOOL is_auth, DWCRAP_SC_CREATE_REPORT_CORE_FUNC SC_CreateReportCoreFunc, BOOL is_disconnect, WIFIBATTLEMATCH_SC_REPORT_TYPE report_type, BOOL is_sc_you_recv )
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

    if( is_disconnect || report_type == WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER )
    {
      game_result = DWC_SC_GAME_RESULT_NONE;
    }
    else
    {
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

#ifdef PM_DEBUG
      if( *DEBUGWIN_ATLASDIRTY_GetFlag() )
      {
        if( game_result == DWC_SC_GAME_RESULT_LOSS )
        {
          game_result = DWC_SC_GAME_RESULT_WIN;
        }
        else if( game_result == DWC_SC_GAME_RESULT_WIN )
        {
          game_result = DWC_SC_GAME_RESULT_LOSS;
        }
      }
#endif //PM_DEBUG

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
  ret = SC_CreateReportCoreFunc( p_my, cp_data, TRUE, is_disconnect );
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

  if( is_sc_you_recv == TRUE )
  { 
    DWCScGameResult game_result;

    if( report_type == WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER )
    { 
      game_result = DWC_SC_GAME_RESULT_NONE;
    }
    else if( is_disconnect )
    {
      game_result = DWC_SC_GAME_RESULT_DISCONNECT;
    }
    else
    {
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
    }

#ifdef PM_DEBUG
      if( *DEBUGWIN_ATLASDIRTY_GetFlag() )
      {
        if( game_result == DWC_SC_GAME_RESULT_LOSS )
        {
          game_result = DWC_SC_GAME_RESULT_WIN;
        }
        else if( game_result == DWC_SC_GAME_RESULT_WIN )
        {
          game_result = DWC_SC_GAME_RESULT_LOSS;
        }
      }
#endif //PM_DEBUG

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

    //���я�������
    ret = SC_CreateReportCoreFunc( p_my, cp_data, FALSE, is_disconnect );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      DEBUG_NET_Printf( "Report:Core%d\n",ret );
      return ret;
    }
  }

  // ���|�[�g�Ƀ`�[���f�[�^����������
  ret = DWC_ScReportBeginTeamData( p_my->mReport );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    DEBUG_NET_Printf( "Report:Team%d\n",ret );
    return ret;
  }

  //�@���|�[�g�̏I�[��錾����
  if( is_disconnect )
  {
    ret = DWC_ScReportEnd( p_my->mReport, is_auth, DWC_SC_GAME_STATUS_PARTIAL );
  }
  else
  { 
    ret = DWC_ScReportEnd( p_my->mReport, is_auth, DWC_SC_GAME_STATUS_COMPLETE );
  }
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
static DWCScResult DwcRap_Sc_CreateReportRndCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{ 
  const BtlResult result  = cp_data->btl_result;
  BOOL is_win = FALSE;
  BOOL is_lose  = FALSE;
  BOOL is_inverse = FALSE;
  DWCScResult ret;

  switch( cp_data->btl_result )
  {
  case BTL_RESULT_RUN:         ///< ������
  case BTL_RESULT_LOSE:        ///< ������
    is_win  = FALSE;
    is_lose = TRUE;
    is_inverse  = TRUE;
    break;
  case BTL_RESULT_RUN_ENEMY:   ///< ���肪������
  case BTL_RESULT_WIN:         ///< ������
    is_win  = TRUE;
    is_lose = FALSE;
    is_inverse  = TRUE;
    break;
  case BTL_RESULT_DRAW:        ///< �Ђ��킯
    is_win  = FALSE;
    is_lose = FALSE;
    break;
  case BTL_RESULT_CAPTURE:     ///< �߂܂����i�쐶�̂݁j
  default:
    GF_ASSERT( 0 );
  case BTL_RESULT_COMM_ERROR:  ///< �ʐM�G���[�ɂ��
    is_win  = FALSE;
    is_lose = FALSE;
    break;
  }

#ifdef PM_DEBUG
  if( *DEBUGWIN_ATLASDIRTY_GetFlag() )
  {
    if( is_inverse )
    {
      is_win  ^= 1;
      is_lose ^= 1;
    }
  }
#endif //PM_DEBUG

  //��������̏����������ނȂ�΁A���ʂ𔽑΂ɂ���
  if( !is_my && is_inverse )
  {
    is_win  ^= 1;
    is_lose ^= 1;
  }

  DEBUGWIN_REPORT_SetData( is_my, is_win, is_lose, cp_data->is_dirty != 0, -1  );

  DEBUG_NET_Printf( "-----------Report_start-------------\n" );
  DEBUG_NET_Printf( "���|�[�g��%d\n", is_my );
  DEBUG_NET_Printf( "����%d\n", is_win );
  DEBUG_NET_Printf( "����%d\n", is_lose );
  DEBUG_NET_Printf( "�s��%d\n", cp_data->is_dirty );
  DEBUG_NET_Printf( "-----------Report_end-------------\n" );

  //���[���ɂ���đ�����̂��Ⴄ
  switch( cp_data->btl_rule )
  { 
  case WIFIBATTLEMATCH_BTLRULE_SINGLE:    ///< �V���O��
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SINGLE_LOSE_COUNTER, is_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case WIFIBATTLEMATCH_BTLRULE_DOUBLE:    ///< �_�u��
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_DOUBLE_LOSE_COUNTER, is_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case WIFIBATTLEMATCH_BTLRULE_TRIPLE:    ///< �g���v��
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_TRIPLE_LOSE_COUNTER, is_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case WIFIBATTLEMATCH_BTLRULE_ROTATE:  ///< ���[�e�[�V����
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
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_ROTATE_LOSE_COUNTER, is_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  case WIFIBATTLEMATCH_BTLRULE_SHOOTER:  ///< �V���[�^�[
    ret = DWC_ScReportAddByteValue( p_my->mReport, KEY_ARENA_GAMETYPE_1V1_SHOOTER, 1 );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_WIN_COUNTER, is_win );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_SHOOTER_LOSE_COUNTER, is_lose );
    if( ret != DWC_SC_RESULT_NO_ERROR )
    { 
      return ret;
    }
    break;

  default:
    GF_ASSERT(0);
  }

  //���ʂ̑������
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_CHEATS_COUNTER, cp_data->is_dirty != 0 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_ADD_DISCONNECTS_COUNTER, -1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
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
static DWCScResult DwcRap_Sc_CreateReportWifiCore( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{ 
  const BtlResult result  = cp_data->btl_result;
  BOOL is_win = FALSE;
  BOOL is_lose  = FALSE;
  BOOL is_inverse = FALSE;
  DWCScResult ret;

  switch( cp_data->btl_result )
  {
  case BTL_RESULT_RUN:         ///< ������
  case BTL_RESULT_LOSE:        ///< ������
    is_win  = FALSE;
    is_lose = TRUE;
    is_inverse = TRUE;
    break;
  case BTL_RESULT_RUN_ENEMY:   ///< ���肪������
  case BTL_RESULT_WIN:         ///< ������
    is_win  = TRUE;
    is_lose = FALSE;
    is_inverse = TRUE;
    break;
  case BTL_RESULT_DRAW:        ///< �Ђ��킯
    is_win  = FALSE;
    is_lose = FALSE;
    break;
  case BTL_RESULT_CAPTURE:     ///< �߂܂����i�쐶�̂݁j
  default:
    GF_ASSERT( 0 );
  case BTL_RESULT_COMM_ERROR:  ///< �ʐM�G���[�ɂ��
    is_win  = FALSE;
    is_lose = FALSE;
    break;
  }


#ifdef PM_DEBUG
  if( *DEBUGWIN_ATLASDIRTY_GetFlag() )
  {
    if( is_inverse )
    {
      is_win  ^= 1;
      is_lose ^= 1;
    }
  }
#endif //PM_DEBUG

  //��������̏��������������ނȂ�΁A���ʂ𔽑΂ɂ���
  if( !is_my && is_inverse )
  {
    is_win  ^= 1;
    is_lose ^= 1;
  }


  DEBUGWIN_REPORT_SetData( is_my, is_win, is_lose, cp_data->is_dirty != 0, -1  );

  DEBUG_NET_Printf( "-----------Report_start-------------\n" );
  DEBUG_NET_Printf( "���|�[�g��%d\n", is_my );
  DEBUG_NET_Printf( "����%d\n", is_win );
  DEBUG_NET_Printf( "����%d\n", is_lose );
  DEBUG_NET_Printf( "�s��%d\n", cp_data->is_dirty );
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
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_NUM_WIFICUP_LOSE_COUNTER, is_lose );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_CHEATS_WIFICUP_COUNTER, cp_data->is_dirty != 0 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  { 
    return ret;
  }
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_ADD_DISCONNECTS_WIFICUP_COUNTER, -1 );
  if( ret != DWC_SC_RESULT_NO_ERROR )
  {
    return ret;
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬  �����_���}�b�`�o�g���O��
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportRndCoreBtlAfter( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{ 
  DWCScResult ret;
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_DISCONNECTS_COUNTER, 1 );
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬  WIFI���o�g���O��
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportWifiCoreBtlAfter( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{ 
  DWCScResult ret;
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_DISCONNECTS_WIFICUP_COUNTER, 1 );
  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬  �����_���}�b�`�o�g���ؒf�G���[��
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportRndCoreBtlError( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{
  DWCScResult ret;

  DEBUGWIN_REPORT_SetData( is_my, 0, 0, 0, -1  );

  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_ADD_DISCONNECTS_COUNTER, -1 );
  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���|�[�g�쐬  WIFI���o�g���ؒf�G���[��
 *
 *	@param	DWC_SC_PLAYERDATA *p_my           �������ރ��|�[�g�̎�����
 *	@param  const DWC_SC_WRITE_DATA *cp_data  �������ރf�[�^
 *	@param	is_my                             TRUE�Ȃ�Ύ����AFALSE�Ȃ�Α���
 *
 *	@return �������݌���
 */
//-----------------------------------------------------------------------------
static DWCScResult DwcRap_Sc_CreateReportWifiCoreBtlError( DWC_SC_PLAYERDATA *p_my, const DWC_SC_WRITE_DATA *cp_data, BOOL is_my, BOOL is_disconnect )
{
  DWCScResult ret;

  DEBUGWIN_REPORT_SetData( is_my, 0, 0, 0, -1  );
  ret = DWC_ScReportAddIntValue( p_my->mReport, KEY_ADD_DISCONNECTS_WIFICUP_COUNTER, -1 );
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

  GFL_STD_MemCopy( cp_data_adrs, &p_wk->p_data->sc_player[1], sizeof(DWC_SC_PLAYERDATA));

  DEBUG_NET_Printf( "[Other]Session ID: %s\n", p_wk->p_data->sc_player[1].mSessionId );
  DEBUG_NET_Printf( "[Other]Connection ID: %s\n", p_wk->p_data->sc_player[1].mConnectionId );
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
  const WIFIBATTLEMATCH_ENEMYDATA *cp_recv = cp_data_adrs;

  //����Ǝ����̃}�C�X�e�[�^�X���Q�[���f�[�^�Ɋi�[����
  MYSTATUS *p_mystatus  = GAMEDATA_GetMyStatusPlayer( p_wk->p_gamedata, netID );
  GFL_STD_MemCopy( cp_recv->mystatus, p_mystatus, MyStatus_GetWorkSize() );
  DEBUG_NET_Printf( "MyStatus���Z�b�g�InetID=%d size=%d \n", netID, size );

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

//----------------------------------------------------------------------------
/**
 *	@brief  �s���J�E���^��M�R�[���o�b�N
 *
 *	@param	int netID     �l�b�gID
 *	@param	int size      �T�C�Y
 *	@param	void* pData   �f�[�^
 *	@param	pWk           �����ł��������A�h���X
 *	@param	pNetHandle    �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_RecvDirtyCnt(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
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

  DEBUG_NET_Printf( "�s���J�E���^��M�R�[���o�b�N�InetID=%d size=%d \n", netID, size );

  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_DIRTYCNT] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t���O��M�R�[���o�b�N
 *
 *	@param	int netID           �l�b�gID
 *	@param	int size            �T�C�Y
 *	@param	void* cp_data_adrs  �f�[�^
 *	@param	p_wk_adrs           �����ŗ^�����A�h���X
 *	@param	p_net_handle        �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_RecvFlag(const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE* p_net_handle)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;
  u32 data  = *(u32*)cp_data_adrs;

  if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  DEBUG_NET_Printf( "�t���O��M�R�[���o�b�N�InetID=%d size=%d \n", netID, size, data );

  p_wk->recv_flag = data;
  p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_FLAG] = TRUE;
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
void WIFIBATTLEMATCH_GDB_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, int recordID, WIFIBATTLEMATCH_GDB_GETTYPE type, void *p_wk_adrs )
{ 
  p_wk->seq           = 0;
  p_wk->p_get_wk      = p_wk_adrs;
  p_wk->get_recordID = recordID;
  DEBUG_NET_Printf( "GDB:request[%d]\n",type );


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

  case WIFIBATTLEMATCH_GDB_GET_PARTY:
    { 
      static const char *sc_get_pokemon_tbl[]  =
      { 
        SAKE_STAT_WIFICUP_POKEMON_PARTY
      };

      p_wk->pp_table_name  = sc_get_pokemon_tbl;
      p_wk->table_name_num= NELEMS( sc_get_pokemon_tbl );
      p_wk->gdb_get_record_callback = DwcRap_Gdb_PokeParty_GetRecordsCallback;
    }
    break;

  case WIFIBATTLEMATCH_GDB_GET_LOGIN_DATE:
    { 
      static const char *sc_get_datetime_tbl[]  =
      { 
        SAKE_STAT_LAST_LOGIN_DATETIME
      };

      p_wk->pp_table_name  = sc_get_datetime_tbl;
      p_wk->table_name_num= NELEMS( sc_get_datetime_tbl );
      p_wk->gdb_get_record_callback = DwcRap_Gdb_LoginDate_GetRecordsCallback;
    }
    break;
#ifdef PM_DEBUG
  case WIFIBATTLEMATCH_GDB_GET_DEBUGALL:
    {
      p_wk->pp_table_name  = sc_get_debugall_tbl;
      p_wk->table_name_num= NELEMS( sc_get_debugall_tbl );
      p_wk->gdb_get_record_callback = DwcRap_Gdb_SakeAll_GetRecordsCallback;
    }
    break;
#endif //PM_DEBUG
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
WIFIBATTLEMATCH_GDB_RESULT WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  DWCGdbError error;
  DWCGdbState state;

  { 
    switch( p_wk->seq )
    { 
    case WIFIBATTLEMATCH_GDB_SEQ_INIT:
      error = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
      if( error != DWC_GDB_ERROR_NONE )
      { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
        DwcRap_Gdb_Finalize( p_wk );
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
      DEBUG_NET_Printf( "GDB:Init\n" );
      p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_GET_START;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_START:
      GF_ASSERT( p_wk->table_name_num < WBM_GDB_FIELD_TABLE_MAX );
      if( p_wk->get_recordID == WIFIBATTLEMATCH_GDB_MYRECORD )
      { 
        //�����̃��R�[�h�擾�̏ꍇ
        error = DWC_GdbGetMyRecordsAsync( WIFIBATTLEMATCH_NET_TABLENAME, p_wk->pp_table_name, p_wk->table_name_num, DwcRap_Gdb_GetCallback, p_wk );
      }
      else
      { 
        //���R�[�hID�w�肵�Ď擾�̏ꍇ
        error = DWC_GdbGetRecordsAsync( WIFIBATTLEMATCH_NET_TABLENAME,
                                   &p_wk->get_recordID,
                                   1,
                                   p_wk->pp_table_name, 
                                   p_wk->table_name_num, 
                                   DwcRap_Gdb_GetCallback, p_wk);
      }
      if( error != DWC_GDB_ERROR_NONE )
      { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
        DwcRap_Gdb_Finalize( p_wk );
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
      DEBUG_NET_Printf( "GDB:Get start\n" );
      p_wk->seq= WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT;
      p_wk->async_timeout = 0;
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_WAIT:
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ ); 
          //�񓯊������͊J�n����Ă���̂ŁA
          //�L�����Z�����s�Ȃ�
          p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_CANCEL_START;
          break;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 
          //�񓯊������͊J�n����Ă���̂ŁA
          //�L�����Z�����s�Ȃ�
          WIFIBATTLEMATCH_NETERR_SetGdbState( &p_wk->error, state, __LINE__ );
          p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_CANCEL_START;
          break;
        }
        
        if( state == DWC_GDB_STATE_IDLE )
        { 
          DEBUG_NET_Printf( "GDB:Get wait\n" );
          p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_GET_END;
        }
      }
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_GET_END:
      { 
        DWCGdbAsyncResult ret;
        ret = DWC_GdbGetAsyncResult();
        if( ret == DWC_GDB_ASYNC_RESULT_NONE )
        {
          WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
          p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_CANCEL_START;
          break;
        }
        else if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
        { 
          WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
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
      return WIFIBATTLEMATCH_GDB_RESULT_SUCCESS;


    case WIFIBATTLEMATCH_GDB_SEQ_CANCEL_START:
      {

        error = DWC_GdbCancelRequest();
        DEBUG_NET_Printf( "GDB:Start Cancel\n" );
        if( error == DWC_GDB_ERROR_NONE )
        {
          p_wk->async_timeout = 0;
          p_wk->seq= WIFIBATTLEMATCH_GDB_SEQ_CANCEL_WAIT;
        }
        else
        {
          WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }
      }
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_CANCEL_WAIT:
      DEBUG_NET_Printf( "GDB:Wait Cancel\n" );
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ ); 
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }
        
        if( state == DWC_GDB_STATE_IDLE )
        { 
          p_wk->seq = WIFIBATTLEMATCH_GDB_SEQ_CANCEL_END;
        }
      }
      break;

    case WIFIBATTLEMATCH_GDB_SEQ_CANCEL_END:
      DwcRap_Gdb_Finalize( p_wk );
      return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
    }
  }

  return WIFIBATTLEMATCH_GDB_RESULT_UPDATE;
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
  DEBUG_NET_Printf( "GDB_ShotDown finalize !\n" );
  DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���ݒ�t�B�[���h�ɒǉ�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_SetMyInfo( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  MYSTATUS  *p_mystatus = GAMEDATA_GetMyStatus(p_wk->p_gamedata);

  p_wk->p_field_buff[0].name  = SAKE_STAT_MYSTATUS;
  p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
  p_wk->p_field_buff[0].value.binary_data.data = (u8*)p_mystatus;
  p_wk->p_field_buff[0].value.binary_data.size = MYSTATUS_SAVE_SIZE;

  WifiBattleMatch_SetDateTime( &p_wk->datetime );
  p_wk->p_field_buff[1].name  = SAKE_STAT_LAST_LOGIN_DATETIME;
  p_wk->p_field_buff[1].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
  p_wk->p_field_buff[1].value.binary_data.data = (u8*)&p_wk->datetime;
  p_wk->p_field_buff[1].value.binary_data.size = sizeof(WBM_NET_DATETIME);

  p_wk->p_field_buff[2].name  = SAKE_STAT_INITIAL_PROFILE_ID;
  p_wk->p_field_buff[2].type  = DWC_GDB_FIELD_TYPE_INT;
  p_wk->p_field_buff[2].value.int_s32 = p_wk->pid;

  p_wk->p_field_buff[3].name  = SAKE_STAT_NOW_PROFILE_ID;
  p_wk->p_field_buff[3].type  = DWC_GDB_FIELD_TYPE_INT;
  p_wk->p_field_buff[3].value.int_s32 = p_wk->cp_user_data->gs_profile_id;
  
  p_wk->table_name_num  = 4;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N  �匳
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_GetCallback(int record_num, DWCGdbField** records, void* user_param)
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = user_param;

  //�ݒ肵���R�[���o�b�N���Ă�
  p_wk->gdb_get_record_callback( record_num, records, p_wk->p_get_wk, p_wk->table_name_num );

  //�����̃��R�[�h����̎擾�̏ꍇ�ŁA���R�[�hID�̎�M������΁A
  //�i�[
  if( p_wk->get_recordID == WIFIBATTLEMATCH_GDB_MYRECORD )
  { 
    int i,j;

    for (i = 0; i < record_num; i++)
    {
      for (j = 0; j < p_wk->table_name_num; j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORDID ) )
        { 
          p_wk->p_data->sake_recordID  = field->value.int_s32;
          DEBUG_NET_Printf("recordID�擾 %d!\n", p_wk->p_data->sake_recordID );
        }
      }
    }
  }
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
static void DwcRap_Gdb_Rnd_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num )
{
    int i,j;
    WIFIBATTLEMATCH_GDB_RND_SCORE_DATA *p_data = user_param;
//    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_RND_SCORE_DATA) );
//    �G���[���R�[���o�b�N���܂���0�N���A���Ă��܂�

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n");
      for (j = 0; j < field_num; j++)   // user_param -> field_num
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
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_SAVE_IDX ) )
        { 
          p_data->record_save_idx  = field->value.int_s32;
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORDID ) )
        { 
          p_data->recordID  = field->value.int_s32;
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
static void DwcRap_Gdb_Wifi_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num )
{
    int i,j;
    WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *p_data = user_param;
//    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA) );
//    �G���[���R�[���o�b�N������0�N���A���Ă��܂�

    DEBUG_NET_Printf("!!!=====gdb_Print[%d]:======\n", record_num);
    for (i = 0; i < record_num; i++)
    {
  
      for (j = 0; j < field_num; j++)   // user_param -> field_num
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
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_SAVE_IDX ) )
        { 
          p_data->record_save_idx  = field->value.int_s32;
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
static void DwcRap_Gdb_RecordID_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num)
{ 
    int i,j;
    int *p_data = user_param;

      DEBUG_NET_Printf("!!!=====gdb_Print[%d]:======\n", record_num );
    for (i = 0; i < record_num; i++)
    {
      for (j = 0; j < field_num; j++)   // user_param -> field_num
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
//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N  �|�P�����p�[�e�B�̂�
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_PokeParty_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num )
{ 
    int i,j;
    void *p_data = user_param;

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n" );
      for (j = 0; j < field_num; j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_WIFICUP_POKEMON_PARTY ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, p_data, field->value.binary_data.size );
        }
        print_field( field );
        DEBUG_NET_Printf(" ");
      }
      DEBUG_NET_Printf("!!!====================\n");
    }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�擾�R�[���o�b�N  ���O�C�����Ԃ̂�
 *
 *	@param	record_num  ���R�[�h��
 *	@param	records     ���R�[�h�̃A�h���X
 *	@param	user_param  �����Őݒ肵�����[�N
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_LoginDate_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num)
{ 
    int i,j;
    WBM_NET_DATETIME *p_data = user_param;

    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n" );
      for (j = 0; j < field_num; j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];

        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_LAST_LOGIN_DATETIME ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, p_data, field->value.binary_data.size );
        }
        print_field( field );
        DEBUG_NET_Printf(" ");
      }
      DEBUG_NET_Printf("!!!====================\n");
    }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �T�P�̑S�f�[�^�擾�R�[���o�b�N
 *
 *	@param	record_num
 *	@param	records
 *	@param	user_param
 */
//-----------------------------------------------------------------------------
static void DwcRap_Gdb_SakeAll_GetRecordsCallback(int record_num, DWCGdbField** records, void* user_param, int field_num)
{
    int i,j;
    WIFIBATTLEMATCH_GDB_SAKE_ALL_DATA *p_data = user_param;
    GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_GDB_SAKE_ALL_DATA) );

    DwcRap_Gdb_LoginDate_GetRecordsCallback( record_num, records, &p_data->datetime, field_num );
    DwcRap_Gdb_Wifi_GetRecordsCallback( record_num, records, &p_data->wifi, field_num );
    DwcRap_Gdb_Rnd_GetRecordsCallback( record_num, records, &p_data->rnd, field_num );

    //�����擾
    for (i = 0; i < record_num; i++)
    {
      DEBUG_NET_Printf("!!!=====gdb_Print:======\n" );
      for (j = 0; j < field_num; j++)   // user_param -> field_num
      {
        DWCGdbField* field  = &records[i][j];
        
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_MYSTATUS ) )
        {
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->mystatus, field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_01 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[0], field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_02 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[1], field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_03 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[2], field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_04 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[3], field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_05 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[4], field->value.binary_data.size );
        }
        if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_DATA_06 ) )
        { 
          GFL_STD_MemCopy( field->value.binary_data.data, &p_data->record_data[5], field->value.binary_data.size );
        }
        else if( !GFL_STD_StrCmp( field->name, SAKE_STAT_RECORD_SAVE_IDX ) )
        { 
          p_data->record_save_idx  = field->value.int_s32;
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

  p_wk->seq             = 0;
  DEBUG_NET_Printf( "GDBW: request[%d]\n", type );
  GF_ASSERT( p_wk->p_data->sake_recordID != 0 );

  switch( type )
  { 
#ifdef PM_DEBUG
  case WIFIBATTLEMATCH_GDB_WRITE_DEBUGALL:
    { 
      int i;
      const WIFIBATTLEMATCH_GDB_SAKE_ALL_DATA *cp_data  = cp_wk_adrs;
      p_wk->table_name_num  = 33;

      //�_�u�����[�g
      p_wk->p_field_buff[0].name          = (char*)sc_get_debugall_tbl[0];
      p_wk->p_field_buff[0].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[0].value.int_s32 = cp_data->rnd.double_rate;
      //���[�e�[�V�������[�g
      p_wk->p_field_buff[1].name          = (char*)sc_get_debugall_tbl[1];
      p_wk->p_field_buff[1].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[1].value.int_s32 = cp_data->rnd.rot_rate;
      //�V���[�^�[���[�g
      p_wk->p_field_buff[2].name          = (char*)sc_get_debugall_tbl[2];
      p_wk->p_field_buff[2].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[2].value.int_s32 = cp_data->rnd.shooter_rate;
      //�V���O�����[�g
      p_wk->p_field_buff[3].name          = (char*)sc_get_debugall_tbl[3];
      p_wk->p_field_buff[3].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[3].value.int_s32 = cp_data->rnd.single_rate;
      //�g���v�����[�g
      p_wk->p_field_buff[4].name          = (char*)sc_get_debugall_tbl[4];
      p_wk->p_field_buff[4].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[4].value.int_s32 = cp_data->rnd.triple_rate;
      //�`�[�g
      p_wk->p_field_buff[5].name          = (char*)sc_get_debugall_tbl[5];
      p_wk->p_field_buff[5].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[5].value.int_s32 = cp_data->rnd.cheat;
      //�}�b�`
      p_wk->p_field_buff[6].name          = (char*)sc_get_debugall_tbl[6];
      p_wk->p_field_buff[6].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[6].value.int_s32 = cp_data->rnd.complete;
      //�ؒf
      p_wk->p_field_buff[7].name          = (char*)sc_get_debugall_tbl[7];
      p_wk->p_field_buff[7].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[7].value.int_s32 = cp_data->rnd.disconnect;
      //�_�u������
      p_wk->p_field_buff[8].name          = (char*)sc_get_debugall_tbl[8];
      p_wk->p_field_buff[8].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[8].value.int_s32 = cp_data->rnd.double_lose;
      //�_�u������
      p_wk->p_field_buff[9].name          = (char*)sc_get_debugall_tbl[9];
      p_wk->p_field_buff[9].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[9].value.int_s32 = cp_data->rnd.double_win;
      //���b�g����
      p_wk->p_field_buff[10].name          = (char*)sc_get_debugall_tbl[10];
      p_wk->p_field_buff[10].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[10].value.int_s32 = cp_data->rnd.rot_lose;
      //���b�g����
      p_wk->p_field_buff[11].name          = (char*)sc_get_debugall_tbl[11];
      p_wk->p_field_buff[11].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[11].value.int_s32 = cp_data->rnd.rot_win;
      //�V���[�^�[����
      p_wk->p_field_buff[12].name          = (char*)sc_get_debugall_tbl[12];
      p_wk->p_field_buff[12].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[12].value.int_s32 = cp_data->rnd.shooter_lose;
      //�V���[�^�[����
      p_wk->p_field_buff[13].name          = (char*)sc_get_debugall_tbl[13];
      p_wk->p_field_buff[13].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[13].value.int_s32 = cp_data->rnd.shooter_win;
      //�V���O������
      p_wk->p_field_buff[14].name          = (char*)sc_get_debugall_tbl[14];
      p_wk->p_field_buff[14].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[14].value.int_s32 = cp_data->rnd.single_lose;
      //�V���O������
      p_wk->p_field_buff[15].name          = (char*)sc_get_debugall_tbl[15];
      p_wk->p_field_buff[15].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[15].value.int_s32 = cp_data->rnd.single_win;
      //�g���v������
      p_wk->p_field_buff[16].name          = (char*)sc_get_debugall_tbl[16];
      p_wk->p_field_buff[16].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[16].value.int_s32 = cp_data->rnd.triple_lose;
      //�Ƃ�Ղ鏟��
      p_wk->p_field_buff[17].name          = (char*)sc_get_debugall_tbl[17];
      p_wk->p_field_buff[17].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[17].value.int_s32 = cp_data->rnd.triple_win;
      //WIFI���[�g
      p_wk->p_field_buff[18].name          = (char*)sc_get_debugall_tbl[18];
      p_wk->p_field_buff[18].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[18].value.int_s32 = cp_data->wifi.rate;
      //WIFI�`�[�g
      p_wk->p_field_buff[19].name          = (char*)sc_get_debugall_tbl[19];
      p_wk->p_field_buff[19].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[19].value.int_s32 = cp_data->wifi.cheat;
      //WIFI�ؒf
      p_wk->p_field_buff[20].name          = (char*)sc_get_debugall_tbl[20];
      p_wk->p_field_buff[20].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[20].value.int_s32 = cp_data->wifi.disconnect;
      //WIFI�܂�
      p_wk->p_field_buff[21].name          = (char*)sc_get_debugall_tbl[21];
      p_wk->p_field_buff[21].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[21].value.int_s32 = cp_data->wifi.lose;
      //WIFI����
      p_wk->p_field_buff[22].name          = (char*)sc_get_debugall_tbl[22];
      p_wk->p_field_buff[22].type          = DWC_GDB_FIELD_TYPE_INT;
      p_wk->p_field_buff[22].value.int_s32 = cp_data->wifi.win;
      //�p�[�e�B
      p_wk->p_field_buff[23].name          = (char*)sc_get_debugall_tbl[23];
      p_wk->p_field_buff[23].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[23].value.binary_data.data = (u8*)cp_data->wifi.pokeparty;
      p_wk->p_field_buff[23].value.binary_data.size = WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE;

      //���O�C������
      p_wk->p_field_buff[24].name          = (char*)sc_get_debugall_tbl[24];
      p_wk->p_field_buff[24].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[24].value.binary_data.data = (u8*)&cp_data->datetime;
      p_wk->p_field_buff[24].value.binary_data.size = sizeof(WBM_NET_DATETIME);
      //MYSTATUS
      p_wk->p_field_buff[25].name          = (char*)sc_get_debugall_tbl[25];
      p_wk->p_field_buff[25].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[25].value.binary_data.data = (u8*)cp_data->mystatus;
      p_wk->p_field_buff[25].value.binary_data.size = MYSTATUS_SAVE_SIZE;
      //��тO�P
      p_wk->p_field_buff[26].name          = (char*)sc_get_debugall_tbl[26];
      p_wk->p_field_buff[26].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[26].value.binary_data.data = (u8*)&cp_data->record_data[0];
      p_wk->p_field_buff[26].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��тO�Q
      p_wk->p_field_buff[27].name          = (char*)sc_get_debugall_tbl[27];
      p_wk->p_field_buff[27].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[27].value.binary_data.data = (u8*)&cp_data->record_data[1];
      p_wk->p_field_buff[27].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��тO�R
      p_wk->p_field_buff[28].name          = (char*)sc_get_debugall_tbl[28];
      p_wk->p_field_buff[28].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[28].value.binary_data.data = (u8*)&cp_data->record_data[2];
      p_wk->p_field_buff[28].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��тO�S
      p_wk->p_field_buff[29].name          = (char*)sc_get_debugall_tbl[29];
      p_wk->p_field_buff[29].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[29].value.binary_data.data = (u8*)&cp_data->record_data[3];
      p_wk->p_field_buff[29].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��тO�T
      p_wk->p_field_buff[30].name          = (char*)sc_get_debugall_tbl[30];
      p_wk->p_field_buff[30].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[30].value.binary_data.data = (u8*)&cp_data->record_data[4];
      p_wk->p_field_buff[30].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��тO�U
      p_wk->p_field_buff[31].name          = (char*)sc_get_debugall_tbl[31];
      p_wk->p_field_buff[31].type          = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[31].value.binary_data.data = (u8*)&cp_data->record_data[5];
      p_wk->p_field_buff[31].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //��уC���f�b�N�X
      p_wk->p_field_buff[32].name          = (char*)sc_get_debugall_tbl[32];
      p_wk->p_field_buff[32].type          = DWC_GDB_FIELD_TYPE_BYTE;
      p_wk->p_field_buff[32].value.int_u8  = cp_data->record_save_idx;
    }
    break;
#endif

  case WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY:

    p_wk->p_field_buff[0].name  = ATLAS_GET_STAT_NAME( ARENA_ELO_RATING_1V1_WIFICUP );
    p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_INT;
    p_wk->p_field_buff[0].value.int_s32 = WIFIBATTLEMATCH_GDB_DEFAULT_RATEING;

    p_wk->p_field_buff[1].name  = ATLAS_GET_STAT_NAME( CHEATS_WIFICUP_COUNTER );
    p_wk->p_field_buff[1].type  = DWC_GDB_FIELD_TYPE_INT;
    p_wk->p_field_buff[1].value.int_s32 = 0;

    p_wk->p_field_buff[2].name  = ATLAS_GET_STAT_NAME( DISCONNECTS_WIFICUP_COUNTER );
    p_wk->p_field_buff[2].type  = DWC_GDB_FIELD_TYPE_INT;
    p_wk->p_field_buff[2].value.int_s32 = 0;

    p_wk->p_field_buff[3].name  = ATLAS_GET_STAT_NAME( NUM_WIFICUP_LOSE_COUNTER );
    p_wk->p_field_buff[3].type  = DWC_GDB_FIELD_TYPE_INT;
    p_wk->p_field_buff[3].value.int_s32 = 0;

    p_wk->p_field_buff[4].name  = ATLAS_GET_STAT_NAME( NUM_WIFICUP_WIN_COUNTER );
    p_wk->p_field_buff[4].type  = DWC_GDB_FIELD_TYPE_INT;
    p_wk->p_field_buff[4].value.int_s32 = 0;

    p_wk->p_field_buff[5].name  = SAKE_STAT_WIFICUP_POKEMON_PARTY;
    p_wk->p_field_buff[5].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
    p_wk->p_field_buff[5].value.binary_data.data = (u8*)cp_wk_adrs;
    p_wk->p_field_buff[5].value.binary_data.size = WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE;
    p_wk->table_name_num  = 6;

    break;
  case WIFIBATTLEMATCH_GDB_WRITE_LOGIN_DATE:
    { 
      WifiBattleMatch_SetDateTime( &p_wk->datetime );

      p_wk->table_name_num  = 1;
      p_wk->p_field_buff[0].name  = SAKE_STAT_LAST_LOGIN_DATETIME;
      p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[0].value.binary_data.data = (u8*)&p_wk->datetime;
      p_wk->p_field_buff[0].value.binary_data.size = sizeof(WBM_NET_DATETIME);
    }
    break;
  case WIFIBATTLEMATCH_GDB_WRITE_MYINFO:
    DwcRap_Gdb_SetMyInfo( p_wk );
    break;
  
  case WIFIBATTLEMATCH_GDB_WRITE_RECORD:     //��уf�[�^����������
    { 
      const WIFIBATTLEMATCH_GDB_RND_RECORD_DATA  *cp_data  = cp_wk_adrs;
      p_wk->record_data = *cp_data;

      p_wk->table_name_num  = 2;
      //���
      p_wk->p_field_buff[0].name  = (char*)scp_stat_record_data_tbl[ p_wk->record_data.save_idx ];
      p_wk->p_field_buff[0].type  = DWC_GDB_FIELD_TYPE_BINARY_DATA;
      p_wk->p_field_buff[0].value.binary_data.data = (u8*)&p_wk->record_data.record_data;
      p_wk->p_field_buff[0].value.binary_data.size = sizeof(WIFIBATTLEMATCH_RECORD_DATA);
      //�C���f�b�N�X
      p_wk->p_field_buff[1].name          = SAKE_STAT_RECORD_SAVE_IDX;
      p_wk->p_field_buff[1].type          = DWC_GDB_FIELD_TYPE_BYTE;
      p_wk->p_field_buff[1].value.int_u8  = (p_wk->record_data.save_idx + 1 ) % NELEMS(scp_stat_record_data_tbl);

      DEBUG_NET_Printf( "��уf�[�^�𑗐M�J�n sake%d ��%d ��IDX%d\n", p_wk->p_data->sake_recordID, p_wk->record_data.save_idx,p_wk->p_field_buff[1].value.int_u8 );
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
 *
 *	@return TRUE�ŏI��  FALSE�Ŕj��
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_GDB_RESULT WIFIBATTLEMATCH_GDB_ProcessWrite( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START,
    SEQ_WAIT,
    SEQ_END,
    SEQ_EXIT,
    SEQ_NONE,

    SEQ_CANCEL_START,
    SEQ_CANCEL_WAIT,
    SEQ_CANCEL_END,
  };

  DWCGdbState state;
  DWCGdbError error;

  { 
    switch( p_wk->seq )
    { 
    case SEQ_INIT:
      error = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
      if( error != DWC_GDB_ERROR_NONE )
      { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
        DwcRap_Gdb_Finalize( p_wk );
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
      DEBUG_NET_Printf( "GDBW:Init\n" );
      p_wk->seq = SEQ_START;
      break;

    case SEQ_START:
      GF_ASSERT( p_wk->table_name_num < WBM_GDB_FIELD_TABLE_MAX );
      error = DWC_GdbUpdateRecordAsync( WIFIBATTLEMATCH_NET_TABLENAME, p_wk->p_data->sake_recordID, p_wk->p_field_buff, p_wk->table_name_num );
      if( error != DWC_GDB_ERROR_NONE )
      { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
        DwcRap_Gdb_Finalize( p_wk );
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }
      DEBUG_NET_Printf( "GDBW:Get start\n" );
      p_wk->async_timeout = 0;
      p_wk->seq= SEQ_WAIT;
      break;

    case SEQ_WAIT:
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ );
          p_wk->seq = SEQ_CANCEL_START;
          break;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 
          WIFIBATTLEMATCH_NETERR_SetGdbState( &p_wk->error, state, __LINE__ );
          p_wk->seq = SEQ_CANCEL_START;
          break;
        }

        if( state == DWC_GDB_STATE_IDLE )
        { 
          DEBUG_NET_Printf( "GDBW:Get wait end\n" );
          p_wk->seq = SEQ_END;
        }
      }
      break;

    case SEQ_END:
      { 
        DWCGdbAsyncResult ret;
        ret = DWC_GdbGetAsyncResult();
        if( ret == DWC_GDB_ASYNC_RESULT_NONE )
        {
          WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
          p_wk->seq = SEQ_CANCEL_START;
          break;
        }else 
        if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
        { 
          DwcRap_Gdb_Finalize( p_wk );
          WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }
      }
      DEBUG_NET_Printf( "GDBW:Get end\n" );
      p_wk->seq = SEQ_EXIT;
      break;

    case SEQ_EXIT:
      DwcRap_Gdb_Finalize( p_wk );
      DEBUG_NET_Printf( "GDBW:Get exit\n" );
      DEBUG_NET_Printf( "sake �e�[�u��ID %d\n", p_wk->p_data->sake_recordID );
      p_wk->seq = SEQ_NONE;
      /* fallthrough */

    case SEQ_NONE:
      return WIFIBATTLEMATCH_GDB_RESULT_SUCCESS;

    case SEQ_CANCEL_START:
      {

        error = DWC_GdbCancelRequest();
        DEBUG_NET_Printf( "GDB:Start Cancel\n" );
        if( error == DWC_GDB_ERROR_NONE )
        {
          p_wk->async_timeout = 0;
          p_wk->seq= SEQ_CANCEL_WAIT;
        }
        else
        {
          WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }
      }
      break;

    case SEQ_CANCEL_WAIT:
      DEBUG_NET_Printf( "GDB:Wait Cancel\n" );
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }

        if( state == DWC_GDB_STATE_IDLE )
        { 
          p_wk->seq = SEQ_CANCEL_END;
        }
      }
      break;

    case SEQ_CANCEL_END:
      DwcRap_Gdb_Finalize( p_wk );
      return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
    }
  }

  return WIFIBATTLEMATCH_GDB_RESULT_UPDATE;
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
  p_wk->seq           = 0;
  DwcRap_Gdb_SetMyInfo( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���R�[�h�e�[�u�����쐬
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_GDB_RESULT WIFIBATTLEMATCH_GDB_ProcessCreateRecord( WIFIBATTLEMATCH_NET_WORK *p_wk)
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_CREATE_START,
    SEQ_CREATE_WAIT,
    SEQ_EXIT,

    SEQ_CANCEL_START,
    SEQ_CANCEL_WAIT,
    SEQ_CANCEL_END,
  };
  DWCGdbState state;
  DWCGdbError error; 

  { 
    switch( p_wk->seq )
    { 
    case SEQ_INIT:
	    error = DWC_GdbInitialize( GAME_ID, p_wk->cp_user_data, WIFIBATTLEMATCH_NET_SSL_TYPE );
      if( error != DWC_GDB_ERROR_NONE )
      { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
      }

      DEBUG_NET_Printf( "INIT:Init\n" );
      p_wk->seq = SEQ_CREATE_START;
      break;

	  case SEQ_CREATE_START:
	    error = DWC_GdbCreateRecordAsync( WIFIBATTLEMATCH_NET_TABLENAME,
	                                      p_wk->p_field_buff, 
	                                      p_wk->table_name_num, &p_wk->p_data->sake_recordID );
	    if( error != DWC_GDB_ERROR_NONE )
	    { 
        WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ ); 
        return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
	    }
      DEBUG_NET_Printf( "INIT:Create Start\n" );
	    p_wk->async_timeout = 0;
	    p_wk->seq = SEQ_CREATE_WAIT;
      break;
	
	  case SEQ_CREATE_WAIT:
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ );
          p_wk->seq = SEQ_CANCEL_START;
          break;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 

          WIFIBATTLEMATCH_NETERR_SetGdbState( &p_wk->error, state, __LINE__ );
          p_wk->seq = SEQ_CANCEL_START;
          break;
        }

        if( state == DWC_GDB_STATE_IDLE )
        { 
          DEBUG_NET_Printf( "Init:Get wait end\n" );
          p_wk->seq = SEQ_EXIT;
        }
      }
	    break;

    case SEQ_EXIT:
      {
        { 
          DWCGdbAsyncResult ret;
          ret = DWC_GdbGetAsyncResult();
          if( ret == DWC_GDB_ASYNC_RESULT_NONE )
          {
            WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
            p_wk->seq = SEQ_CANCEL_START;
            break;
          }
          else if( ret != DWC_GDB_ASYNC_RESULT_SUCCESS )
          { 
            DwcRap_Gdb_Finalize( p_wk );
            WIFIBATTLEMATCH_NETERR_SetGdbResult( &p_wk->error, ret, __LINE__ );
            return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
          }
        }
        if( p_wk->p_data->sake_recordID == 0 )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_NO_RECORD, __LINE__ );  
        }
        DwcRap_Gdb_Finalize( p_wk );
        DEBUG_NET_Printf( "INIT:Back exit\n" );
        DEBUG_NET_Printf( "�쐬! sake �e�[�u��ID %d\n", p_wk->p_data->sake_recordID );
        return WIFIBATTLEMATCH_GDB_RESULT_SUCCESS;
      }

    case SEQ_CANCEL_START:
      {
        error = DWC_GdbCancelRequest();
        DEBUG_NET_Printf( "GDB:Start Cancel\n" );
        if( error == DWC_GDB_ERROR_NONE )
        {
          p_wk->async_timeout = 0;
          p_wk->seq= SEQ_CANCEL_WAIT;
        }
        else
        {
          WIFIBATTLEMATCH_NETERR_SetGdbError( &p_wk->error, error, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }
      }
      break;

    case SEQ_CANCEL_WAIT:
      DEBUG_NET_Printf( "GDB:Wait Cancel\n" );
      { 
        if( p_wk->async_timeout++ > ASYNC_TIMEOUT )
        { 
          WIFIBATTLEMATCH_NETERR_SetSysError( &p_wk->error, WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT, __LINE__ );
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }

        DWC_GdbProcess();

        state = DWC_GdbGetState();
        if( state == DWC_GDB_STATE_UNINITIALIZED || 
            state == DWC_GDB_STATE_ERROR_OCCURED )
        { 
          DwcRap_Gdb_Finalize( p_wk );
          return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
        }

        if( state == DWC_GDB_STATE_IDLE )
        { 
          p_wk->seq = SEQ_CANCEL_END;
        }
      }
      break;

    case SEQ_CANCEL_END:
      DwcRap_Gdb_Finalize( p_wk );
      return WIFIBATTLEMATCH_GDB_RESULT_ERROR;
    }
  }

  return WIFIBATTLEMATCH_GDB_RESULT_UPDATE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SAKE�̃��R�[�hID���擾
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk ���[�N
 *
 *	@return ���R�[�hID
 */
//-----------------------------------------------------------------------------
u32 WIFIBATTLEMATCH_GDB_GetRecordID( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return cp_wk->p_data->sake_recordID;
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

    p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_GAMEDATA]  = FALSE;
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
    p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_POKEPARTY] = FALSE;
    GFL_STD_MemCopy( p_wk->recv_buffer, p_party, PokeParty_GetWorkSize() );
  }
  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s���J�E���g�𑗂荇��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	dirty_cnt                       �s���J�E���^
 *
 *	@return TRUE�ő��M  FALSE�ő��M���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SendDirtyCnt( WIFIBATTLEMATCH_NET_WORK *p_wk, const u32 *cp_dirty_cnt )
{ 
  NetID netID;


  //����ɂ̂ݑ��M
  netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
  netID = netID == 0? 1: 0;
  return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), netID, WIFIBATTLEMATCH_NETCMD_SEND_DIRTYCNT, sizeof(u32), cp_dirty_cnt, TRUE, TRUE, TRUE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s���J�E���g����M����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_dirty_cnt                    ��M�����s���J�E���^
 *
 *	@return TRUE�Ŏ�M  FALSE�Ŏ�M��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_RecvDirtyCnt( WIFIBATTLEMATCH_NET_WORK *p_wk, u32 *p_dirty_cnt )
{ 
  BOOL ret;
  ret = p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_DIRTYCNT];

  if( ret )
  { 
    p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_DIRTYCNT]  = FALSE;
    GFL_STD_MemCopy( p_wk->recv_buffer, p_dirty_cnt, sizeof(u32) );
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���s���t���O�𑗐M����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	dirty_flag                      �s���t���O
 *
 *	@return TRUE�ő��M  FALSE�ő��M���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_SendBtlDirtyFlag( WIFIBATTLEMATCH_NET_WORK *p_wk, const u32 *cp_dirty_flag )
{
  return GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), WIFIBATTLEMATCH_NETCMD_SEND_FLAG, sizeof(u32), cp_dirty_flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���s���t���O����M����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	*p_dirty_flag                   ��M�����s���t���O
 *
 *	@return TRUE�Ŏ�M  FALSE�Ŏ�M��
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_RecvBtlDirtyFlag( WIFIBATTLEMATCH_NET_WORK *p_wk, u32 *p_dirty_flag )
{
  BOOL ret;
  ret = p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_FLAG];

  if( ret )
  { 
    p_wk->is_recv[WIFIBATTLEMATCH_NET_RECVFLAG_FLAG]  = FALSE;
    *p_dirty_flag = p_wk->recv_flag;
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
  p_wk->seq = 0;
  GFL_STD_MemClear( p_wk->nd_attr2, 10 );

  GFL_STD_MemClear( &p_wk->fileInfo, sizeof(DWCNdFileInfo) );

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
    SEQ_FILELIST,
    SEQ_GET_FILE,
    SEQ_GETTING_FILE,

    SEQ_DOWNLOAD_COMPLETE,
    SEQ_END,
    SEQ_EMPTY_END,
    SEQ_ERROR_END,

    SEQ_WAIT_CALLBACK         = 100,
    SEQ_WAIT_CLEANUP_CALLBACK = 200,
  };

  switch( p_wk->seq )
  { 
  case SEQ_INIT:


    if( DWC_NdInitAsync( NdCallback, GF_DWC_ND_LOGIN, WIFI_ND_LOGIN_PASSWD ) == FALSE )
    {
      DEBUG_NET_Printf( "DWC_NdInitAsync: Failed\n" );
      return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
    }
    else
    {
      DwcRap_Nd_WaitNdCallback( p_wk, SEQ_ATTR );
    }
    break;

  case SEQ_ATTR:
    // �t�@�C�������̐ݒ�
    if( DWC_NdSetAttr(WIFI_FILE_ATTR1, p_wk->nd_attr2, WIFI_FILE_ATTR3) == FALSE )
    {
      DEBUG_NET_Printf( "DWC_NdSetAttr: Failed\n." );
      DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
    }
    else
    {
      p_wk->seq = SEQ_FILELIST;
    }
    break;

//-------------------------------------
///	�T�[�o�[�̃t�@�C���`�F�b�N
//=====================================
  case SEQ_FILELIST:
    //�����I�ɂP���Ƃ�ɂ���
    { 
      if( DWC_NdGetFileListAsync( &p_wk->fileInfo, 0, 1 ) == FALSE)
      {
        DEBUG_NET_Printf( "DWC_NdGetFileListNumAsync: Failed.\n" );
        DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
      }
      else
      {
        DwcRap_Nd_WaitNdCallback( p_wk, SEQ_GET_FILE );
      }
    }
    break;

//-------------------------------------
///	�t�@�C���ǂݍ��݊J�n
//=====================================
  case SEQ_GET_FILE:
    if( !UTIL_IsClear( &p_wk->fileInfo, sizeof(DWCNdFileInfo) ) )
    { 
      //���݂��Ă����ꍇ�t�@�C���ǂݍ��݊J�n
      if(DWC_NdGetFileAsync( &p_wk->fileInfo, (char*)&p_wk->temp_buffer, REGULATION_CARD_DATA_SIZE) == FALSE){
        DEBUG_NET_Printf( "DWC_NdGetFileAsync: Failed.\n" );
        DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
      }
      else
      {
        s_callback_flag   = FALSE;
        s_callback_result = DWC_ND_ERROR_NONE;
        p_wk->seq = SEQ_GETTING_FILE;
      }
    }
    else
    {
      //���݂��Ă��Ȃ������ꍇ�A
      if( !DWC_NdCleanupAsync() ){  //FALSE�̏ꍇ�R�[���o�b�N���Ă΂�Ȃ�
        return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY;
      }
      else
      {
        DwcRap_Nd_WaitNdCallback( p_wk, SEQ_EMPTY_END );
      }

    }
    break;
    
  case SEQ_GETTING_FILE:
    // �t�@�C���ǂݍ��ݒ�
    DWC_NdProcess();

    if( p_wk->async_timeout++ > WIFI_ND_TIMEOUT_SYNC )
    {
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_USER_TIMEOUT );
      DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
    }

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
      OS_Printf("DWC_NdGetProgress�ŃG���[\n");
      DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
    }
    else
    {
      // �t�@�C���ǂݍ��ݏI��
      if( !DWC_NdCleanupAsync() ){  //FALSE�̏ꍇ�R�[���o�b�N���Ă΂�Ȃ�
        OS_Printf("DWC_NdCleanupAsync�Ɏ��s\n");
        return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;
      }
      else
      {
        DwcRap_Nd_WaitNdCallback( p_wk, SEQ_DOWNLOAD_COMPLETE );
      }
    }
    break;

//-------------------------------------
///	�I���E�L�����Z������
//=====================================
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


  case SEQ_EMPTY_END:
    return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY;

  case SEQ_ERROR_END:
    return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_ERROR;

//-------------------------------------
///	�R�[���o�b�N�҂�����  
//�@�@WIFI_DOWNLOAD_WaitNdCallback���g���Ă�������
//=====================================
  case SEQ_WAIT_CALLBACK:
    //�R�[���o�b�N������҂�
    DWC_NdProcess();
    if(s_callback_result != DWC_ND_ERROR_NONE)
    {
      DwcRap_Nd_CleanUpNdCallback( p_wk, SEQ_ERROR_END );
    }
    if( s_callback_flag )
    { 
      s_callback_flag = FALSE;
      if( s_callback_result == DWC_ND_ERROR_NONE)
      { 
        p_wk->seq  = p_wk->next_seq;
      }
    }
    break;

  case SEQ_WAIT_CLEANUP_CALLBACK:
    DWC_NdProcess();
    if( s_cleanup_callback_flag )
    { 
      s_cleanup_callback_flag = FALSE;
      p_wk->seq  = p_wk->next_seq;
    }
    break;
  }

  return WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_DOWNLOADING;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O�N���A����Ă��邩�ǂ���
 *
 *	@param	void *p_adrs  �A�h���X
 *	@param	size          �o�C�g�T�C�Y
 *
 *	@return TRUE�łO�N���A����Ă���  FALSE�ł���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_IsClear( void *p_adrs, u32 size )
{
  int i;
  u32 *p_ptr  = (u32*)p_adrs;

  for( i = 0; i < size/4; i++ )
  {
    if( *(p_ptr + i) != 0 )
    {
      return FALSE;
    }
  }

  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���ɐؒf�̑O�ɌĂ΂��R�[���o�b�N
 *
 *	@param	p_wk_adrs ���[�N�A�h���X
 *	@param	code      �G���[�R�[�h
 *	@param	type      �G���[�^�C�v
 *	@param	ret       �G���[���^�[��
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_ErrDisconnectCallback(void* p_wk_adrs, int code, int type, int ret )
{
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_wk_adrs;

  DEBUG_NET_Printf( "Call ErrDisconnectCallback code=%d type=%d ret=%d\n", code, type, ret );
  switch( type )
  {
    //�ؒf�G���[
  case DWC_ETYPE_SHUTDOWN_FM:
  case DWC_ETYPE_DISCONNECT:
    //NHTTP��������Ă��Ȃ��Ȃ�ΐ�ɌĂ�

    if(p_wk->p_nhttp)
    {
      NHTTP_RAP_ErrorClean(p_wk->p_nhttp);
      NHTTP_RAP_End(p_wk->p_nhttp);
      p_wk->p_nhttp  = NULL;
    }

    //SC���g���Ă���Ȃ�ΌĂ�
    DwcRap_Sc_Finalize( p_wk );
  }
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
//----------------------------------------------------------------------------
/**
 *	@brief  �N���[���A�b�v
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	next_seq  ���̃V�[�P���X
 *	@param  false_seq ���s�����Ƃ��̃V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void DwcRap_Nd_CleanUpNdCallback( WIFIBATTLEMATCH_NET_WORK *p_wk, int next_seq )
{
  s_callback_flag   = FALSE;
  s_callback_result = DWC_ND_ERROR_NONE;
  p_wk->next_seq    = next_seq;
  p_wk->seq         = 200;

  if( !DWC_NdCleanupAsync(  ) ){  //FALSE�̏ꍇ�R�[���o�b�N���Ă΂�Ȃ�
    OS_Printf("DWC_NdCleanupAsync�Ɏ��s\n");
    p_wk->next_seq    = next_seq;
  }
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
  s_cleanup_callback_flag = TRUE;

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
void WIFIBATTLEMATCH_NET_StartRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, HEAPID heapID )
{ 
  GF_ASSERT( p_wk->p_nhttp == NULL );
  p_wk->seq = 0;
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID, MyStatus_GetProfileID(GAMEDATA_GetMyStatus(p_wk->p_gamedata)), p_wk->p_svl_result);
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
WIFIBATTLEMATCH_RECV_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitRecvGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_CONNECT,
    SEQ_WAIT_CONNECT,

    SEQ_END,
    SEQ_DIRTY_END,
  };

  NHTTPError error;

  if( p_wk->p_nhttp == NULL )
  { 
    return WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR;
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
    if(NHTTP_RAP_StartConnect(p_wk->p_nhttp)==NHTTP_ERROR_NONE)
    {
      p_wk->seq  = SEQ_WAIT_CONNECT;
    }
    else
    {
      if(p_wk->p_nhttp)
      {
        NHTTP_RAP_End(p_wk->p_nhttp);
        p_wk->p_nhttp  = NULL;
      }
      return WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR;
    }
    break;

  case SEQ_WAIT_CONNECT:
    {
      int response = NHTTP_RAP_GetGetResultCode(p_wk->p_nhttp);

      switch( response )
      {
      case 502:
        p_wk->seq  = SEQ_DIRTY_END;

        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR011 );
        return WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE;

      case 503:
        p_wk->seq  = SEQ_DIRTY_END;
        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR012 );
        return WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE;
      }
    }

    error  = NHTTP_RAP_Process(p_wk->p_nhttp);
    if(NHTTP_ERROR_NONE == error)
    {
      u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(p_wk->p_nhttp);
      DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA* pDream = (DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA*)&pEvent[sizeof(gs_response)];


      NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
      DEBUG_NET_Printf("ID %x\n",pDream->WifiMatchUpID);
      DEBUG_NET_Printf("FLG %x\n",pDream->GPFEntryFlg);
      DEBUG_NET_Printf("ST %d\n",pDream->WifiMatchUpState);
      DEBUG_NET_Printf("Sign %d\n",pDream->signin);

      DEBUG_NET_Printf("�I��\n");

      p_wk->gpf_data  = *pDream;

      if( ((gs_response*)pEvent)->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE
        ||  ((gs_response*)pEvent)->ret_cd == DREAM_WORLD_SERVER_ALREADY_EXISTS )
      { 
        p_wk->seq  = SEQ_END;
      }
      else
      { 

        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR001 + ((gs_response*)pEvent)->ret_cd -1 );
        p_wk->seq  = SEQ_DIRTY_END;
      }
    }
    else if( NHTTP_ERROR_BUSY != error )
    { 
      if(p_wk->p_nhttp)
      {
        NHTTP_RAP_End(p_wk->p_nhttp);
        p_wk->p_nhttp  = NULL;
      }
      return WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR;
    }

    break;

  case SEQ_DIRTY_END:
    GFL_STD_MemClear( &p_wk->gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
    if(p_wk->p_nhttp)
    {
      NHTTP_RAP_ErrorClean( p_wk->p_nhttp );
      NHTTP_RAP_End(p_wk->p_nhttp);
      p_wk->p_nhttp  = NULL;
    }
    return WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY;

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
 *	@param	DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *cp_send    �������݃f�[�^
 *	@param	heapID                                                �q�[�vID
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk, const DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA *cp_send, HEAPID heapID )
{ 
  GF_ASSERT( p_wk->p_nhttp == NULL );
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID, MyStatus_GetProfileID(GAMEDATA_GetMyStatus(p_wk->p_gamedata)), p_wk->p_svl_result);
  p_wk->gdb_write_data  = *cp_send;
  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GPF�T�[�o�[�֏������� ���C������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_SEND_GPFDATA_RET WIFIBATTLEMATCH_NET_WaitSendGpfData( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_CONNECT,
    SEQ_WAIT_CONNECT,

    SEQ_DIRTY_END,  
    SEQ_END,
  };
  NHTTPError error;

  if( p_wk->p_nhttp == NULL )
  { 
    return WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR;
  }

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    if (NHTTP_RAP_ConectionCreate( NHTTPRAP_URL_BATTLE_UPLOAD, p_wk->p_nhttp ))
    {
      NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(p_wk->p_nhttp), &p_wk->gdb_write_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      p_wk->seq  = SEQ_START_CONNECT;
    }
    break;

  case SEQ_START_CONNECT:
    if(NHTTP_RAP_StartConnect(p_wk->p_nhttp)==NHTTP_ERROR_NONE)
    {
      p_wk->seq  = SEQ_WAIT_CONNECT;
    }
    else
    {
      if(p_wk->p_nhttp)
      {
        NHTTP_RAP_End(p_wk->p_nhttp);
        p_wk->p_nhttp  = NULL;
      }
      return WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR;
    }
    break;

  case SEQ_WAIT_CONNECT:
    {
      int response = NHTTP_RAP_GetGetResultCode(p_wk->p_nhttp);
      switch( response )
      {
      case 502:
        p_wk->seq  = SEQ_DIRTY_END;
        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR011 );
        return WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE;

      case 503:
        p_wk->seq  = SEQ_DIRTY_END;
        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR012 );
        return WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE;
      }
    }


    error  = NHTTP_RAP_Process(p_wk->p_nhttp);
    if(NHTTP_ERROR_NONE == error)
    {
      u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(p_wk->p_nhttp);
      NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
      DEBUG_NET_Printf("GPF�T�[�o�[�ւ̏������ݏI��\n");



      if( ((gs_response*)pEvent)->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE
          ||  ((gs_response*)pEvent)->ret_cd == DREAM_WORLD_SERVER_ALREADY_EXISTS )
      {  
        p_wk->seq  = SEQ_END;
      }
      else
      {
        NetErr_App_ReqErrorDispForce( WIFI_GSYNC_ERR001 + ((gs_response*)pEvent)->ret_cd -1 );
        p_wk->seq  = SEQ_DIRTY_END;
      }
    }
    else if( NHTTP_ERROR_BUSY != error )
    { 
      WIFIBATTLEMATCH_NETERR_SetNhttpError( &p_wk->error, error, __LINE__ );

      if(p_wk->p_nhttp)
      {
        NHTTP_RAP_End(p_wk->p_nhttp);
        p_wk->p_nhttp  = NULL;
      }

      return WIFIBATTLEMATCH_SEND_GPFDATA_RET_ERROR;
    }
    break;

  case SEQ_DIRTY_END:
    if(p_wk->p_nhttp)
    {
      NHTTP_RAP_ErrorClean( p_wk->p_nhttp );
      NHTTP_RAP_End(p_wk->p_nhttp);
      p_wk->p_nhttp  = NULL;
    }
    return WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY;

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
 *	@param	STRBUF *p_str                   �`�F�b�N����STRBUF
 *	@param	HEAPID                          �q�[�vID
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartBadWord( WIFIBATTLEMATCH_NET_WORK *p_wk, MYSTATUS *p_mystatus, HEAPID heapID )
{ 
  p_wk->p_badword_mystatus  = p_mystatus;
  {

    STRBUF *p_word_check  = MyStatus_CreateNameString( p_mystatus, p_wk->heapID );
    DWC_TOOL_BADWORD_Start( &p_wk->badword, p_word_check, heapID );

    GFL_STR_DeleteBuffer( p_word_check );
  }
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
  ret = DWC_TOOL_BADWORD_Wait( &p_wk->badword, p_is_bad_word );

  if( ret == TRUE && *p_is_bad_word == TRUE )
  { 
    STRBUF  *p_modifiname = DWC_TOOL_CreateBadNickName( p_wk->heapID );

    MyStatus_SetMyNameFromString( p_wk->p_badword_mystatus, p_modifiname);

    GFL_STR_DeleteBuffer(p_modifiname);
    NAGI_Printf( "�������ł���\n" );
  }

  return ret;
}

//=============================================================================
/**
 *    �|�P�����s���`�F�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s���`�F�b�N  �J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	POKEPARTY *cp_party �`�F�b�N����|�P�����p�[�e�B
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartEvilCheck( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_void, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE type )
{ 
  p_wk->seq = 0;

  p_wk->p_nhttp = NHTTP_RAP_Init( p_wk->heapID, MyStatus_GetProfileID(GAMEDATA_GetMyStatus(p_wk->p_gamedata)), p_wk->p_svl_result);

  switch( type )
  { 
  case WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PP:
    NHTTP_RAP_PokemonEvilCheckCreate( p_wk->p_nhttp, p_wk->heapID, POKETOOL_GetWorkSize(), NHTTP_POKECHK_RANDOMMATCH);
    NHTTP_RAP_PokemonEvilCheckAdd( p_wk->p_nhttp, (void*)cp_void, POKETOOL_GetWorkSize() );
    p_wk->poke_max  = 1;
    break;

  case WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY:
    { 
      int i;
      const POKEPARTY *cp_party = (const POKEPARTY*)cp_void;
      POKEMON_PARAM *p_pp;

      NHTTP_RAP_PokemonEvilCheckCreate( p_wk->p_nhttp, p_wk->heapID, POKETOOL_GetWorkSize() * PokeParty_GetPokeCount(cp_party), NHTTP_POKECHK_RANDOMMATCH);
      for( i = 0; i < PokeParty_GetPokeCount(cp_party); i++ )
      { 
        p_pp  = PokeParty_GetMemberPointer( cp_party, i );
        NHTTP_RAP_PokemonEvilCheckAdd( p_wk->p_nhttp, p_pp, POKETOOL_GetWorkSize() );

        DEBUG_NET_Printf( "monsno %d\n", PP_Get( p_pp, ID_PARA_monsno, NULL) );
        DEBUG_NET_Printf( "rnd %d\n", PP_Get( p_pp, ID_PARA_personal_rnd, NULL) );
        DEBUG_NET_Printf( "crc %d\n", GFL_STD_CrcCalc( p_pp, POKETOOL_GetWorkSize() ) );
      }
      p_wk->poke_max  = PokeParty_GetPokeCount(cp_party);
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s���`�F�b�N  �����҂�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 *	@param  WIFIBATTLEMATCH_NET_EVILCHECK_DATA  �f�[�^
 *
 *	@return WIFIBATTLEMATCH_NET_EVILCHECK_RET
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_EVILCHECK_RET WIFIBATTLEMATCH_NET_WaitEvilCheck( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_NET_EVILCHECK_DATA *p_data )
{ 
  enum
  { 
    SEQ_EVILCHECL_START,
    SEQ_EVILCHECL_WAIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  { 
  case SEQ_EVILCHECL_START:
    { 
      BOOL ret;
      ret = NHTTP_RAP_PokemonEvilCheckConectionCreate( p_wk->p_nhttp );

      if( ret )
      {
        if( NHTTP_RAP_StartConnect( p_wk->p_nhttp ) == NHTTP_ERROR_NONE )
        {
          p_wk->seq++;
        }
        else
        {
          if(p_wk->p_nhttp)
          {
            NHTTP_RAP_End(p_wk->p_nhttp);
            p_wk->p_nhttp  = NULL;
            p_wk->seq = 0;
          }
          return WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR;
        }
        DEBUG_NET_Printf( "�s���`�F�b�N�J�n\n" );
      }
      else
      {
        if(p_wk->p_nhttp)
        {
          NHTTP_RAP_End(p_wk->p_nhttp);
          p_wk->p_nhttp  = NULL;
          p_wk->seq = 0;
        }
        return WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR;
      }

    }
    break;

  case SEQ_EVILCHECL_WAIT:
    { 
      NHTTPError error;
      error = NHTTP_RAP_Process( p_wk->p_nhttp );
      if( NHTTP_ERROR_NONE == error )
      {
        p_wk->seq++;
      }
      else if( NHTTP_ERROR_BUSY != error )
      { 
        WIFIBATTLEMATCH_NETERR_SetNhttpError( &p_wk->error, error, __LINE__ );
        NHTTP_RAP_PokemonEvilCheckDelete(p_wk->p_nhttp);
        if(p_wk->p_nhttp)
        {
          NHTTP_RAP_End(p_wk->p_nhttp);
          p_wk->p_nhttp  = NULL;
          p_wk->seq = 0;
        }
        return WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR;
      }
    }
    break;

  case SEQ_END:
    { 
      int i;
      void *p_buff;
      const s8 *cp_sign;
      p_buff  = NHTTP_RAP_GetRecvBuffer(p_wk->p_nhttp);

      p_data->status_code  = NHTTP_RAP_EVILCHECK_GetStatusCode( p_buff );
      
      for( i = 0; i < p_wk->poke_max; i++ )
      { 
        p_data->poke_result[i] = NHTTP_RAP_EVILCHECK_GetPokeResult( p_buff, i );
      }

      if( p_data->status_code == 0 )
      { 
        NAGI_Printf( "�s���`�F�b�N�ʉ߂��ăT�C�����炢�܂����I\n" );
        cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_buff, p_wk->poke_max );
        GFL_STD_MemCopy( cp_sign, p_data->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
#ifdef PM_DEBUG
        { 
          int i;
          for( i = 0; i < NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN; i++ )
          { 
            DEBUG_NET_Printf( "%d ", p_data->sign[i] );
            if(i % 0x10 == 0xF )
            { 
              DEBUG_NET_Printf( "\n" );
            }
          }
        }
#endif
      }

      NHTTP_RAP_PokemonEvilCheckDelete(p_wk->p_nhttp);
      if(p_wk->p_nhttp)
      {
        NHTTP_RAP_End(p_wk->p_nhttp);
        p_wk->p_nhttp  = NULL;
      }
    }
    return WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS;
  }

  return WIFIBATTLEMATCH_NET_EVILCHECK_RET_UPDATE;
}
//=============================================================================
/**
 *    �G���[����
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���N���A
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_Clear( WIFIBATTLEMATCH_NETERR_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NETERR_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���C��
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_Main( WIFIBATTLEMATCH_NETERR_WORK *p_wk )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e���G���[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	type                              �V�X�e���G���[�̎��
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetSysError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, WIFIBATTLEMATCH_NET_SYSERROR error, int line )
{ 
  DEBUG_NET_Printf( "!!!Sys Error!!!=%d line=%d\n\n", error, line );
  p_wk->sys_err = error;
  p_wk->type    = WIFIBATTLEMATCH_NET_ERRORTYPE_SYS;
  
  GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_USER_TIMEOUT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SC�G���[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	sc_err                            SC�G���[�̎��
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetScError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCScResult sc_err, int line )
{ 
  DEBUG_NET_Printf( "!!!Sc Error!!!=%d line=%d\n", sc_err, line );
  p_wk->sc_err  = sc_err;
  p_wk->type    = WIFIBATTLEMATCH_NET_ERRORTYPE_SC;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GDB�G���[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	gdb_err                           GDB�G���[�̎��
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetGdbError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbError gdb_err, int line )
{ 
  DEBUG_NET_Printf( "!!!Gdb Error!!=%d line=%d\n", gdb_err, line );
  p_wk->gdb_err = gdb_err;
  p_wk->type    = WIFIBATTLEMATCH_NET_ERRORTYPE_GDB;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GDB�̏�Ԃ�ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	state                             GDB�̏��
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetGdbState( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbState state, int line )
{ 
  DEBUG_NET_Printf( "!!!Gdb State!!=%d line=%d\n", state, line );
  p_wk->gdb_state = state;
  p_wk->type      = WIFIBATTLEMATCH_NET_ERRORTYPE_GDB;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GDB�̌��ʂ�ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	state                             GDB�̌���
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetGdbResult( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCGdbAsyncResult result, int line )
{ 
  DEBUG_NET_Printf( "!!!Gdb Result!!=%d err=%d line=%d\n", result, DWC_GetLastError(NULL), line );
  p_wk->gdb_result  = result;
  p_wk->type        = WIFIBATTLEMATCH_NET_ERRORTYPE_GDB;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ND�G���[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 *	@param	nd_err                            �G���[
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetNdError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, DWCNdError nd_err, int line )
{ 
  DEBUG_NET_Printf( "!!!ND Error!!=%d line=%d\n", nd_err, line );
  p_wk->nd_err  = nd_err;
  p_wk->type    = WIFIBATTLEMATCH_NET_ERRORTYPE_ND;
}

//----------------------------------------------------------------------------
/**
 *	@brief  NHTTP�G���[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk   ���[�N
 *	@param	nhttp_err                           NHTTP�G���[
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_SetNhttpError( WIFIBATTLEMATCH_NETERR_WORK *p_wk, NHTTPError nhttp_err, int line )
{ 
  DEBUG_NET_Printf( "!!!NHTTP Error!!=%d line=%d\n", nhttp_err, line );
  p_wk->nhttp_err = nhttp_err;
  p_wk->type    = WIFIBATTLEMATCH_NET_ERRORTYPE_NHTTP;
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
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SC_GetErrorRepairType( DWCScResult error )
{ 
//  DEBUG_NET_Printf( "WIFIBATTLEMATCH_GDB_GetErrorRepairType err%d \n", error);

  switch( error )
  { 
  case DWC_SC_RESULT_NO_ERROR :              ///< �G���[�͔������܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_NONE;

  case DWC_SC_RESULT_NO_AVAILABILITY_CHECK:     ///< �������̑O�ɁA�W���� GameSpy �p���`�F�b�N�����s����܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_INVALID_PARAMETERS:        ///< �C���^�[�t�F�[�X�֐��ɓn���ꂽ�p�����[�^�������ł�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_NOT_INITIALIZED:           ///< SDK ������������܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_CORE_NOT_INITIALIZED:      ///< �R�A������������܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_OUT_OF_MEMORY:             ///< SDK �̓��\�[�X�Ƀ����������蓖�Ă��܂���ł���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_CALLBACK_PENDING:          ///< �A�v���P�[�V�����ɑ΂��A���삪�ۗ����ł��邱�Ƃ�`���܂�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_HTTP_ERROR:                ///< �o�b�N�G���h���K���� HTTP �ɂ�郌�X�|���X�Ɏ��s�����ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_UNKNOWN_RESPONSE:          ///< SDK �����ʂ𗝉��ł��Ȃ��ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_RESPONSE_INVALID:          ///< SDK ���o�b�N�G���h����̃��X�|���X��ǂݎ��Ȃ��ꍇ�̃G���[����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  case DWC_SC_RESULT_INVALID_DATATYPE:          ///< �����ȃf�[�^�^�C�v
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

  case DWC_SC_RESULT_REPORT_INCOMPLETE:         ///< ���|�[�g���s���S�ł�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

  case DWC_SC_RESULT_REPORT_INVALID:            ///< ���|�[�g�̈ꕔ�܂��͂��ׂĂ������ł�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

  case DWC_SC_RESULT_SUBMISSION_FAILED:         ///< ���|�[�g�̑��M�Ɏ��s���܂���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

  case DWC_SC_RESULT_UNKNOWN_ERROR:             ///< SDK ���F���ł��Ȃ��G���[
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

  default:
    GF_ASSERT(0);
    return 0;
  }
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
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_GDB_GetErrorRepairType( DWCGdbError error, DWCGdbState gdb_state, DWCGdbAsyncResult gdb_result )
{ 
 // DEBUG_NET_Printf( "WIFIBATTLEMATCH_GDB_GetErrorRepairType err%d state%d resul%d\n", error, gdb_state, gdb_result );

  //-------------------------------------
  ///	�G���[�̃`�F�b�N
  //=====================================
  switch( error )
  { 
    case DWC_GDB_ERROR_NONE :                 ///< ����ɏI�����܂����B
      //  resukt �� state�̃`�F�b�N
      break;

    case DWC_GDB_ERROR_OUT_OF_MEMORY :        ///< case DWC_Init�ŗ^�����A���P�[�^���烁�������m�ۂł��܂���ł����B
    case DWC_GDB_ERROR_NOT_AVAILABLE :        ///< ���p�\�ł͂Ȃ�
    case DWC_GDB_ERROR_CORE_SHUTDOWN :        ///< 
    case DWC_GDB_ERROR_NOT_LOGIN :            ///< WiFiConnection�ɐ��������O�C�����Ă��܂���B
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
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

    case DWC_GDB_ERROR_UNKNOWN :             ///<
    case DWC_GDB_ERROR_ALREADY_INITIALIZED : ///< ���ɏ���������Ă��܂��B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;

    case DWC_GDB_ERROR_NOT_INITIALIZED :     ///< ���������s���Ă��܂���B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;

    case DWC_GDB_ERROR_IN_ASYNC_PROCESS :    ///< �񓯊��������ł��B
      //  resukt �� state�̃`�F�b�N
      break;

    case DWC_GDB_ERROR_GHTTP :               ///< GHTTP�̃G���[���������܂����B
    case DWC_GDB_ERROR_IN_DWC_ERROR :         ///< DWC�̃G���[���N�����Ă��܂��B�G���[�̑Ώ����s���Ă��������B
      return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  }

  //-------------------------------------
  ///	��Ԃ̃`�F�b�N
  //=====================================
  switch( gdb_state )
  { 
  case DWC_GDB_STATE_UNINITIALIZED:    ///< �����������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN; 
  case DWC_GDB_STATE_IDLE:             ///< ���������I�����A�񓯊��������s���Ă��Ȃ����
      //  state�̃`�F�b�N
    break;
  case DWC_GDB_STATE_IN_ASYNC_PROCESS: ///< �񓯊��������s���Ă�����
      //  state�̃`�F�b�N
    break;
  case DWC_GDB_STATE_ERROR_OCCURED:     ///< �G���[�������������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT; 
  }

  //-------------------------------------
  ///	���ʂ̃`�F�b�N
  //=====================================
  switch( gdb_result )
  { 
  case DWC_GDB_ASYNC_RESULT_NONE:                  ///< �񓯊��������܂��I�����Ă��܂���B
    // ����
    break;
  case DWC_GDB_ASYNC_RESULT_SUCCESS:               ///< �������܂����B
    // ����
    break;
  case DWC_GDB_ASYNC_RESULT_SECRET_KEY_INVALID:   ///< �����Ȕ閧��
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_SERVICE_DISABLED:     ///< �T�[�r�X������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_CONNECTION_TIMEOUT:   ///< �^�C���A�E�g
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case DWC_GDB_ASYNC_RESULT_CONNECTION_ERROR:     ///< �ڑ��G���[
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_MALFORMED_RESPONSE:   ///< �Ԉ�����`���̉���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_OUT_OF_MEMORY:        ///< �������s��
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_DATABASE_UNAVAILABLE: ///< �f�[�^�x�[�X���g�p�ł��Ȃ�
  case DWC_GDB_ASYNC_RESULT_LOGIN_TICKET_INVALID: ///< ���O�C���`�P�b�g������
  case DWC_GDB_ASYNC_RESULT_LOGIN_TICKET_EXPIRED: ///< ���O�C���`�P�b�g�������؂�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_TABLE_NOT_FOUND:      ///< �e�[�u����������Ȃ�
  case DWC_GDB_ASYNC_RESULT_RECORD_NOT_FOUND:     ///< ���R�[�h��������Ȃ�
  case DWC_GDB_ASYNC_RESULT_FIELD_NOT_FOUND:      ///< �t�B�[���h����������Ȃ�
  case DWC_GDB_ASYNC_RESULT_FIELD_TYPE_INVALID:   ///< �t�B�[���h�^�C�v���s��
  case DWC_GDB_ASYNC_RESULT_NO_PERMISSION:        ///< �p�[�~�b�V�������قȂ�
  case DWC_GDB_ASYNC_RESULT_RECORD_LIMIT_REACHED: ///< ���R�[�h����ɒB����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case DWC_GDB_ASYNC_RESULT_ALREADY_RATED:        ///< ���ɍs����
  case DWC_GDB_ASYNC_RESULT_NOT_RATEABLE:         ///< �]���ł͂Ȃ�
  case DWC_GDB_ASYNC_RESULT_NOT_OWNED:            ///< ���L�҂ł͂Ȃ�
  case DWC_GDB_ASYNC_RESULT_FILTER_INVALID:       ///< �t�B���^������
  case DWC_GDB_ASYNC_RESULT_SORT_INVALID:         ///< �\�[�g������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case DWC_GDB_ASYNC_RESULT_UNKNOWN_ERROR:        ///< �s���ȃG���[
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_BAD_HTTP_METHOD:      ///< HTTP�̕���������
  case DWC_GDB_ASYNC_RESULT_BAD_FILE_COUNT:       ///< �t�@�C����������
  case DWC_GDB_ASYNC_RESULT_MISSING_PARAMETER:    ///< �p�����[�^�������Ă���
  case DWC_GDB_ASYNC_RESULT_FILE_NOT_FOUND:       ///< �t�@�C����������Ȃ�
  case DWC_GDB_ASYNC_RESULT_FILE_TOO_LARGE:       ///< �t�@�C��������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case DWC_GDB_ASYNC_RESULT_FILE_UNKNOWN_ERROR:   ///< �t�@�C���̕s���ȃG���[
  case DWC_GDB_ASYNC_RESULT_GHTTP_ERROR:          ///< GHTTP�̃G���[���������܂����B
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_GDB_ASYNC_RESULT_TARGET_FILTER_INVALID:///< �^�[�Q�b�g�t�B���^������
  case DWC_GDB_ASYNC_RESULT_SERVER_ERROR:         ///< �T�[�o�[�G���[
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  }

  return WIFIBATTLEMATCH_NET_ERROR_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�C���^�C�v���擾
 *
 *	@param	DWCNdError                  nd_err   �G���[
 *
 *	@return �G���[�C���^�C�v
 */
//-----------------------------------------------------------------------------
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_ND_GetErrorRepairType( DWCNdError nd_err )
{

  switch( nd_err )
  { 
  case DWC_ND_ERROR_NONE:     ///< ���s���̏�������������
    return WIFIBATTLEMATCH_NET_ERROR_NONE;
  case DWC_ND_ERROR_ALLOC:    ///< �������m�ۂɎ��s����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_ND_ERROR_BUSY:     ///< �{�����̊֐����Ăяo���Ȃ�STATE�ŌĂяo����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_ND_ERROR_HTTP:     ///< HTTP�ʐM�ɃG���[����������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_ND_ERROR_BUFFULL:  ///< (�����G���[)�_�E�����[�h�o�b�t�@������Ȃ�
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_ND_ERROR_PARAM:	   ///< �s���ȃp�����[�^�ɂ��G���[����������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case DWC_ND_ERROR_CANCELED: ///< �����̃L�����Z������������
    return WIFIBATTLEMATCH_NET_ERROR_NONE;
  case DWC_ND_ERROR_DLSERVER: ///< �T�[�o���G���[�R�[�h��Ԃ��Ă���
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  case DWC_ND_ERROR_FATAL:	   ///< ����`�̒v���I�ȃG���[����������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL;
  default:
    return WIFIBATTLEMATCH_NET_ERROR_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�C���^�C�v���擾
 *
 *	@param	DWCNdError                  nd_err   �G���[
 *
 *	@return �G���[�C���^�C�v
 */
//-----------------------------------------------------------------------------
static WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE WIFIBATTLEMATCH_SYS_GetErrorRepairType( WIFIBATTLEMATCH_NET_SYSERROR  sys_err )
{ 
  switch( sys_err )
  { 
  case WIFIBATTLEMATCH_NET_SYSERROR_NONE:    //�G���[�͔������Ă��Ȃ�
    return WIFIBATTLEMATCH_NET_ERROR_NONE;
  case WIFIBATTLEMATCH_NET_SYSERROR_TIMEOUT: //�A�v���P�[�V�����^�C���A�E�g����������
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN;
  case WIFIBATTLEMATCH_NET_SYSERROR_NO_RECORD:  //���R�[�hID���擾�ł��Ȃ�����
    return WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT;
  default:
    return WIFIBATTLEMATCH_NET_ERROR_NONE;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���N���A
 *
 *	@param	WIFIBATTLEMATCH_NETERR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WIFIBATTLEMATCH_NETERR_ClearError( WIFIBATTLEMATCH_NETERR_WORK *p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NETERR_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM�A�C�R����\��
 *
 *	@param	BOOL is_top  TRUE�Ȃ�Ώ���  FALSE�����
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NETICON_SetDraw( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_visible )
{ 
  if( is_visible )
  { 
    GFL_NET_ReloadIconTopOrBottom( TRUE, p_wk->heapID );
  }
  else
  { 
    GFL_NET_WirelessIconEasyEnd();
  }
}
