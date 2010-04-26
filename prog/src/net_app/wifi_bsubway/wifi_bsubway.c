//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   wifi_bsubway.c
 *  @brief  Wi-Fi�o�g���T�u�E�F�C
 *  @author tomoya takahashi
 *  @date   2010.02.17
 *
 *  ���W���[�����FWIFI_BSUBWAY
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include <dpw_bt.h>

#include "arc_def.h"


#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_error.h"
#include "net/dpw_rap.h"
#include "net/nhttp_rap.h"
#include "net/nhttp_rap_evilcheck.h"

#include "system/net_err.h"
#include "system/main.h"
#include "system/time_icon.h"


#include "sound/pm_sndsys.h"


#include "savedata/wifilist.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "savedata/system_data.h"
#include "savedata/bsubway_savedata.h"

#include "app/codein.h"


#include "net_app/wifi_bsubway.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net_app/connect_anm.h"


#include "wifi_bsubway_graphic.h"


#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"

#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"


#include "message.naix"
#include "msg/msg_wifi_bsubway.h"


#include "wifi_login.naix"


FS_EXTERN_OVERLAY( dpw_common );

#ifdef PM_DEBUG

#define WIFI_BSUBWAY_Printf( ... )  OS_TPrintf( __VA_ARGS__ )

#include "battle/bsubway_battle_data.h"


//#define DOWNLOAD_PRINT  // �������[�Ǐ����o��

#else

#define WIFI_BSUBWAY_Printf( ... )  ((void)0)

#endif

//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// ���C���V�[�P���X
//=====================================
enum {

  BSUBWAY_SEQ_BGMOUT,        // BGM�ύX����
  BSUBWAY_SEQ_BGMCHANGE,        // BGM�ύX����
  
  BSUBWAY_SEQ_LOGIN,        // Login����
  BSUBWAY_SEQ_LOGIN_WAIT,   // Login����
  
  BSUBWAY_SEQ_FADEIN,       // �t�F�[�h�C��
  BSUBWAY_SEQ_FADEINWAIT,   // �t�F�[�h�C���E�G�C�g

  BSUBWAY_SEQ_INIT,       // ������
  BSUBWAY_SEQ_INIT_WAIT,  // �������E�G�C�g
  BSUBWAY_SEQ_PERSON_SETUP,// Email�F��
  BSUBWAY_SEQ_PERSON_SETUP_WAIT,// Email�F��
  BSUBWAY_SEQ_MAIN,       // ���C������
  BSUBWAY_SEQ_END,        // �I���@���@�t�F�[�h�A�E�g
  BSUBWAY_SEQ_END_WAIT,   // �I���E�G�C�g

  BSUBWAY_SEQ_LOGOUT,     // Logout����

  BSUBWAY_SEQ_BGMFADE,      // BGMFADE
  BSUBWAY_SEQ_BGMFADE_WAIT, // BGMFADE WAIT

  BSUBWAY_SEQ_ALL_END,   // �I��

  BSUBWAY_SEQ_ERROR,      // �G���[����
  BSUBWAY_SEQ_ERROR_WAIT, // �G���[�E�G�C�g
} ;


//-------------------------------------
/// �T�u�V�[�P���X
//=====================================
enum{
  // �L�^�̃A�b�v���[�h
  SCORE_UPLOAD_SEQ_PERSON_MSG_00 = 0,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE_WAIT,
  SCORE_UPLOAD_SEQ_SAVE,
  SCORE_UPLOAD_SEQ_SAVE_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_MSG_01,
  SCORE_UPLOAD_SEQ_PERSON_END,

  // �Q�[�����̃_�E�����[�h
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK = 0,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_SAVE,
  SCORE_UPLOAD_SEQ_GAMEDATA_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_02,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END,

  SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG,
  SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_WAIT,

  // �����̃_�E�����[�h
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00 = 0,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_01,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_SAVE,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_03,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END,

  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_WAIT,
};

//-------------------------------------
/// ���C�������@�߂�l
//=====================================
typedef enum {
  BSUBWAY_MAIN_RESULT_CONTINUE,
  BSUBWAY_MAIN_RESULT_OK,
  BSUBWAY_MAIN_RESULT_CANCEL,

  BSUBWAY_MAIN_RESULT_MAX,
} BSUBWAY_MAIN_RESULT;



//-------------------------------------
/// NHTTP�̃G���[����
//=====================================
typedef enum{
  BSUBWAY_NHTTP_ERROR_NONE,
  BSUBWAY_NHTTP_ERROR_POKE_ERROR,   // �|�P�������s��
  BSUBWAY_NHTTP_ERROR_DISCONNECTED, // ����I�����Ȃ�����

  BSUBWAY_NHTTP_ERROR_MAX,
} BSUBWAY_NHTTP_ERROR;


///�����^�C���A�E�g�܂ł̎���
#define BSUBWAY_TIMEOUT_TIME      (30*60*2) //2��



//-------------------------------------
/// �`��֌W
//=====================================
// �`��t���[��
enum
{
  VIEW_FRAME_SUB_BACKGROUND = GFL_BG_FRAME0_S,
  VIEW_FRAME_MAIN_BACKGROUND = GFL_BG_FRAME0_M,
  VIEW_FRAME_MAIN_BACKGROUND_BASE = GFL_BG_FRAME3_M,
  VIEW_FRAME_MAIN_WIN = GFL_BG_FRAME1_M,
  VIEW_FRAME_MAIN_YESNO = GFL_BG_FRAME2_M,
};

// ���l��
enum
{
  VIEW_NUMBER_KETA_ROOM_NO  = 3,
  VIEW_NUMBER_KETA_RANK     = 2,
};


// BG�p���b�g
enum
{
  VIEW_PAL_MSG = 12,
  VIEW_PAL_WIN_FRAME = 13,
};

//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// PersonalData
//=====================================
typedef struct 
{
  BOOL setup;
  Dpw_Common_Profile        dc_profile;         // �����̏��o�^�p�\����
  Dpw_Common_ProfileResult  dc_profile_result;  // �����̏��o�^���X�|���X�p�\����
  Dpw_Bt_Player bt_player;              // ���������������̃f�[�^
  const MYSTATUS* cp_mystatus;
  const BSUBWAY_WIFI_DATA* cp_bsubway_wifi; // �o�g���T�u�E�F�C�@WiFi�Z�[�u���
  const BSUBWAY_SCOREDATA* cp_bsubway_score; // �o�g���T�u�E�F�C�@WiFi�Z�[�u���


  // �|�P�����F�؁@����
  NHTTP_RAP_WORK* p_nhttp;  // �|�P�����F�؃��[�N
  u16 poke_num;
  u8 sign[NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN];

} WIFI_BSUBWAY_PERSONAL;


//-------------------------------------
/// RoomData
//=====================================
typedef struct 
{
  BOOL setup;
  BOOL in_roomdata;
  
  u16 rank;
  u16 room_no;
  u16 room_num;

  Dpw_Bt_Room   bt_roomdata;            // �_�E�����[�h�������


} WIFI_BSUBWAY_ROOM;

//-------------------------------------
/// dpw_bt Error
//=====================================
typedef struct {
  DpwBtError          error_code;
  DpwBtServerStatus   server_status;

  u16 timeout_flag;
  s16 timeout;

  BSUBWAY_NHTTP_ERROR nhttp_error;
} WIFI_BSUBWAY_ERROR;

//-------------------------------------
/// �`��Ǘ����[�N
//=====================================
typedef struct {
  BOOL setup;

  HEAPID heapID;
  
  // �`��V�X�e��
  WIFI_BSUBWAY_GRAPHIC_WORK * p_graphic;

  // �w�i
  CONNECT_BG_PALANM bg_palanm;

  // �e�L�X�g�\��
  GFL_BMPWIN* p_win;
  BMPMENU_WORK* p_yesno;
  u32 frame_bgchar;
  u32 frame_bgchar_sys;
  TIMEICON_WORK* p_timeicon;
  
  // ��������
  GFL_MSGDATA* p_msgdata;
  WORDSET* p_wordset;
  GFL_FONT* p_font;
  STRBUF* p_strtmp;
  STRBUF* p_str;

  // ������`��
  PRINT_STREAM* p_msg_stream;
  GFL_TCBLSYS*  p_msg_tcbsys;

} WIFI_BSUBWAY_VIEW;


//-------------------------------------
/// �o�g���T�u�E�F�C�@WiFi�������[�N
//=====================================
typedef struct 
{
  // �p�����[�^
  WIFI_BSUBWAY_PARAM* p_param;

  // �����ڊ֌W
  WIFI_BSUBWAY_VIEW view;

  // Login Logout
  WIFILOGIN_PARAM   login;
  WIFILOGOUT_PARAM  logout;

  // �F�ؗp���[�N
  DWCSvlResult svl_result;

  // DpwMain���܂킷�t���O
  u16 dpw_main_do;

  // �Z�[�u���t���O
  u16 save_do;

  // �T�u�V�[�P���X
  s32 seq;
  u32 code_input;

  // �������
  WIFI_BSUBWAY_ROOM roomdata;

  // �l�����
  WIFI_BSUBWAY_PERSONAL personaldata;

  // DPW BT �G���[���
  WIFI_BSUBWAY_ERROR  bt_error;

  // ���l���͏��
  CODEIN_PARAM* p_codein;

  // �e����
  GAMEDATA*     p_gamedata;
  SYSTEMDATA*   p_systemdata;   // �V�X�e���Z�[�u�f�[�^�iDPW���C�u�����p�Ƃ������R�[�h��ۑ��j
  WIFI_LIST*    p_wifilist;     // �Ƃ������蒠
  MYSTATUS*     p_mystatus;     // �g���[�i�[���
  BSUBWAY_WIFI_DATA* p_bsubway_wifi; // �o�g���T�u�E�F�C�@WiFi�Z�[�u���
  BSUBWAY_SCOREDATA* p_bsubway_score; // �o�g���T�u�E�F�C�@�X�R�A�Z�[�u���
  
} WIFI_BSUBWAY;

//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// process
//=====================================
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcEnd( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );

static BOOL WiFiBsubway_IsErrorCheck( const WIFI_BSUBWAY* cp_wk, int seq );


//-------------------------------------
/// RoomData 
//=====================================
// ������
static void ROOM_DATA_Init( WIFI_BSUBWAY_ROOM* p_wk );
// ���[�����̎擾����
static void ROOM_DATA_SetUp( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 rank );
static BOOL ROOM_DATA_SetUpWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// ���[�����̎擾����
static void ROOM_DATA_LoadRoomData( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 room_no );
static BOOL ROOM_DATA_LoadRoomDataWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// ���̎擾
static u16 ROOM_DATA_GetRank( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomNo( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomNum( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomKeta( const WIFI_BSUBWAY_ROOM* cp_wk );
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk );

// �Z�[�u����
static void ROOM_DATA_SavePlayerData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save );
static void ROOM_DATA_SaveLeaderData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save );


//-------------------------------------
/// PersonalData
//=====================================
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk );
static void PERSONAL_DATA_Exit( WIFI_BSUBWAY_PERSONAL* p_wk );
// �l���F��
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error );
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );
static void PERSONAL_DATA_InitDpwPlayerData( Dpw_Bt_Player* p_player, GAMEDATA* p_gamedata );

// �|�P�����F��
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, DWCSvlResult* p_svl_result, HEAPID heapID );
static BOOL PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// �l���A�b�v�f�[�g
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );

//-------------------------------------
/// ErrorData
//=====================================
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk );
static BOOL ERROR_DATA_GetAsyncServerResult( WIFI_BSUBWAY_ERROR* p_wk );
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result );
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk );
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk );
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error );
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_OnlyTimeOutCount( WIFI_BSUBWAY_ERROR* p_wk );


//-------------------------------------
/// CodeIn
//=====================================
static void CODEIN_StartRoomNoProc( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static void CODEIN_StartRankProc( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk );
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str );
static int CODEIN_GetCode( const WIFI_BSUBWAY* cp_wk );

//-------------------------------------
/// Login
//=====================================
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk );
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk );

//-------------------------------------
/// Logout
//=====================================
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk );

//-------------------------------------
/// ���C����������
//=====================================
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );


//-------------------------------------
/// �ڑ��E�ؒf�E�G���[
//=====================================
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk );
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk );



//-------------------------------------
/// View
//=====================================
static void VIEW_Init( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID );
static void VIEW_Exit( WIFI_BSUBWAY_VIEW* p_wk );
static void VIEW_Main( WIFI_BSUBWAY_VIEW* p_wk );

// �O���t�B�b�N���\�[�X�Z�b�g�A�b�v
static void VIEW_InitResource( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID );
static void VIEW_ExitResource( WIFI_BSUBWAY_VIEW* p_wk );

// ���b�Z�[�W�`��
static void VIEW_PrintStream( WIFI_BSUBWAY_VIEW* p_wk, u32 msg_id );
static BOOL VIEW_PrintMain( WIFI_BSUBWAY_VIEW* p_wk );

// ���[�h�Z�b�g
static void VIEW_SetWordNumber( WIFI_BSUBWAY_VIEW* p_wk, u32 idx, u32 number, u32 keta );

// YesNo
static void VIEW_StartYesNo( WIFI_BSUBWAY_VIEW* p_wk );
static u32 VIEW_MainYesNo( WIFI_BSUBWAY_VIEW* p_wk );

// TimeIcon
static void VIEW_StartTimeIcon( WIFI_BSUBWAY_VIEW* p_wk );
static void VIEW_EndTimeIcon( WIFI_BSUBWAY_VIEW* p_wk );


//-------------------------------------
/// Save
//=====================================
static void SAVE_Start( WIFI_BSUBWAY* p_wk );
static BOOL SAVE_Main( WIFI_BSUBWAY* p_wk );

//-----------------------------------------------------------------------------
/**
 *      process���
 */
//-----------------------------------------------------------------------------
const GFL_PROC_DATA WIFI_BSUBWAY_Proc = {
  WiFiBsubway_ProcInit,
  WiFiBsubway_ProcMain,
  WiFiBsubway_ProcEnd,
};









//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C  process ������
 *
 *  @param  p_proc      process���[�N
 *  @param  p_seq       �V�[�P���X
 *  @param  p_param     �p�����[�^
 *  @param  p_work      ���[�N
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval GFL_PROC_RES_FINISH           ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk;

  // DPW_COMMON
  GFL_OVERLAY_Load( FS_OVERLAY_ID( dpw_common ) );


  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI_BSUBWAY, 0x18000 );

  p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WIFI_BSUBWAY), HEAPID_WIFI_BSUBWAY );
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY) );

  // Wi-FiLogin���ĂȂ���Ԃ��`�F�b�N
  GF_ASSERT( GFL_NET_IsInit() == FALSE );


  // ���[�N������
  p_wk->p_param = p_param;
  p_wk->p_param->result = WIFI_BSUBWAY_RESULT_NG; // �߂�l��NG�ŏ�����

  // �e����̎擾
  {
    GAMEDATA* p_gamedata = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesystem );
    SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
    
    p_wk->p_gamedata    = p_gamedata;
    p_wk->p_systemdata  = SaveData_GetSystemData( p_savedata );
    p_wk->p_wifilist    = GAMEDATA_GetWiFiList( p_gamedata );
    p_wk->p_mystatus    = GAMEDATA_GetMyStatus( p_gamedata );
    p_wk->p_bsubway_wifi  = GAMEDATA_GetBSubwayWifiData( p_gamedata );
    p_wk->p_bsubway_score = GAMEDATA_GetBSubwayScoreData( p_gamedata );
  }

  // �V�X�e�����[�N������
  ROOM_DATA_Init( &p_wk->roomdata );
  PERSONAL_DATA_Init( &p_wk->personaldata );
  ERROR_DATA_Init( &p_wk->bt_error );
  
  // DEBUG�o��ON
#ifdef PM_DEBUG
  GFL_NET_DebugPrintOn();
#endif

  return GFL_PROC_RES_FINISH;  
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C  process ���C��
 *
 *  @param  p_proc      process���[�N
 *  @param  p_seq       �V�[�P���X
 *  @param  p_param     �p�����[�^
 *  @param  p_work      ���[�N
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval GFL_PROC_RES_FINISH           ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk = p_work;

  // �ʐM�G���[�`�F�b�N
  if( WiFiBsubway_IsErrorCheck( p_wk, (*p_seq) ) ){
    if( ERROR_DATA_IsError( &p_wk->bt_error ) || (NetErr_App_CheckError() != NET_ERR_CHECK_NONE) )
    {
      WIFI_BSUBWAY_Printf( "ProcMain Error Hit\n" );
      WIFI_BSUBWAY_Printf( "BtError %d\n", ERROR_DATA_IsError( &p_wk->bt_error ) );
      WIFI_BSUBWAY_Printf( "NetError %d\n", NetErr_App_CheckError() );
      (*p_seq) = BSUBWAY_SEQ_ERROR;
    }
  }
  
  // ���C������
  switch( (*p_seq) ){
  case BSUBWAY_SEQ_BGMOUT:        // BGM�ύX����
    // BGM
    PMSND_FadeOutBGM( PMSND_FADE_FAST );
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_BGMCHANGE:        // BGM�ύX����
    if( PMSND_CheckFadeOnBGM() == FALSE ){
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      PMSND_PlayBGM( SEQ_BGM_WIFI_ACCESS );
      PMSND_FadeInBGM( PMSND_FADE_FAST );
      (*p_seq)++;
    }
    break;
    
  // Login����
  case BSUBWAY_SEQ_LOGIN:        
    LOGIN_StartProc( p_wk );
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_LOGIN_WAIT:
    {
      WIFILOGIN_RESULT result;
      result = LOGIN_GetResult( p_wk );
      if( result == WIFILOGIN_RESULT_LOGIN ){
        (*p_seq)++;
      }else{
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_CANCEL; //CANCEL�I
        (*p_seq) = BSUBWAY_SEQ_BGMFADE;
      }
    }
    break;

  case BSUBWAY_SEQ_FADEIN:       // �t�F�[�h�C��
    VIEW_Init( &p_wk->view, HEAPID_WIFI_BSUBWAY );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
    (*p_seq) ++;
    break;

  case BSUBWAY_SEQ_FADEINWAIT:   // �t�F�[�h�C���E�G�C�g
    if( GFL_FADE_CheckFade() == FALSE ){
      (*p_seq) ++;
    }
    break;

  // ������
  case BSUBWAY_SEQ_INIT:       
    WiFiBsubway_Connect( p_wk );
    
    VIEW_StartTimeIcon( &p_wk->view );

    (*p_seq) ++;
    break;
 
  // �������E�G�C�g
  case BSUBWAY_SEQ_INIT_WAIT:  
    if( WiFiBsubway_ConnectWait( p_wk ) )
    {
      (*p_seq) ++;
    }
    break;

  //Email�F��
  case BSUBWAY_SEQ_PERSON_SETUP:
    PERSONAL_DATA_SetUpProfile( &p_wk->personaldata, p_wk->p_gamedata, &p_wk->bt_error );
    (*p_seq) ++;
    break;

  //Email�F�؃E�G�C�g
  case BSUBWAY_SEQ_PERSON_SETUP_WAIT:
    if( PERSONAL_DATA_SetUpProfileWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      (*p_seq) ++;
    }
    break;

  // ���C������
  case BSUBWAY_SEQ_MAIN:
    {
      BSUBWAY_MAIN_RESULT result;

      result = WiFiBsubway_Main( p_wk, HEAPID_WIFI_BSUBWAY );
      
      if( result == BSUBWAY_MAIN_RESULT_OK ){
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_OK; //���������I
        (*p_seq) ++;
        
      }else if( result == BSUBWAY_MAIN_RESULT_CANCEL ){
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_CANCEL; //CANCEL�I���I
        (*p_seq) ++;
      }
    }
    break;

  // �I��
  case BSUBWAY_SEQ_END:        
    WiFiBsubway_Disconnect( p_wk );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    (*p_seq) ++;
    break;

  // �I���E�G�C�g
  case BSUBWAY_SEQ_END_WAIT:   
    if( WiFiBsubway_DisconnectWait( p_wk ) && (GFL_FADE_CheckFade() == FALSE) )
    {
      VIEW_Exit( &p_wk->view );
      (*p_seq) ++;
    }
    break;

  // Logout����
  case BSUBWAY_SEQ_LOGOUT:     
    LOGOUT_StartProc( p_wk );
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_BGMFADE:      // BGMFADE
    PMSND_FadeOutBGM( PMSND_FADE_FAST );
    (*p_seq)++;
    break;
    
  case BSUBWAY_SEQ_BGMFADE_WAIT: // BGMFADE WAIT
    if( PMSND_CheckFadeOnBGM() == FALSE ){
      (*p_seq)++;
    }
    break;

  // �I��
  case BSUBWAY_SEQ_ALL_END:   
    return GFL_PROC_RES_FINISH;

  // �G���[����
  case BSUBWAY_SEQ_ERROR:      
    if( WiFiBsubway_Error( p_wk ) ){
      (*p_seq) = BSUBWAY_SEQ_ERROR_WAIT;
    }else{
      (*p_seq) = BSUBWAY_SEQ_END;
    }
    break;

  // �G���[�E�G�C�g
  case BSUBWAY_SEQ_ERROR_WAIT: 
    if( WiFiBsubway_ErrorWait(p_wk) ){
      (*p_seq) = BSUBWAY_SEQ_END;
    }
    break;
    
  default:
    GF_ASSERT( 0 );
    break;
  }

  // Main dpw_main_do��TRUE
  // �̂Ƃ��̂ݓ���
  if( p_wk->dpw_main_do ){
    Dpw_Bt_Main();
  }
  VIEW_Main( &p_wk->view );


  return GFL_PROC_RES_CONTINUE;  
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C  process �j��
 *
 *  @param  p_proc      process���[�N
 *  @param  p_seq       �V�[�P���X
 *  @param  p_param     �p�����[�^
 *  @param  p_work      ���[�N
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval GFL_PROC_RES_FINISH           ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcEnd( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk = p_work;
  
  // ���l���̓��[�N�j��
  CODEIN_EndProc( p_wk );

  LOGIN_EndProc( p_wk );
  LOGOUT_EndProc( p_wk );

  // �V�X�e�����[�N�j��
  PERSONAL_DATA_Exit( &p_wk->personaldata );
  VIEW_Exit( &p_wk->view );
  
  // proc���[�N�j��
  GFL_PROC_FreeWork( p_proc );

  // HEAPID�j��
  GFL_HEAP_DeleteHeap( HEAPID_WIFI_BSUBWAY );

  // DPW_COMMON
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( dpw_common ) );

  //BGM����
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( PMSND_FADE_NORMAL );
  
  // DEBUG�o��ON
#ifdef PM_DEBUG
  GFL_NET_DebugPrintOff();
#endif

  return GFL_PROC_RES_FINISH;
}


//-------------------------------------
/// Error�`�F�b�N�@���s�����H
//=====================================
static BOOL WiFiBsubway_IsErrorCheck( const WIFI_BSUBWAY* cp_wk, int seq )
{
  if( (seq <= BSUBWAY_SEQ_MAIN) && (GFL_FADE_CheckFade() == FALSE) && (cp_wk->dpw_main_do) && (cp_wk->save_do == FALSE) ){
    return TRUE;
  }
  return FALSE;
}


//-------------------------------------
/// RoomData 
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief�@RoomData������
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_Init( WIFI_BSUBWAY_ROOM* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ROOM) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData  Rank�̕��������Z�b�g�A�b�v
 *
 *  @param  p_wk    ���[�N
 *  @param  rank    �����N
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SetUp( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 rank )
{
  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  p_wk->rank = rank;

  WIFI_BSUBWAY_Printf( "RoomNum Get\n" );

  // ���[�������擾
  Dpw_Bt_GetRoomNumAsync( rank );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData Rank�̕������Z�b�g�A�b�v�҂�
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL ROOM_DATA_SetUpWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;
  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  // ��M�����҂�
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( !ERROR_DATA_IsError( p_error ) ){
      // ����
      p_wk->room_num  = result;
      p_wk->setup     = TRUE;
      return TRUE;
    }
  }

  return FALSE;
}


// ���[�����̎擾����
//----------------------------------------------------------------------------
/**
 *  @brief  RoomData �������̎擾���� 
 *
 *  @param  p_wk      ���[�N
 *  @param  room_no   �����i���o�[
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_LoadRoomData( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 room_no )
{
  GF_ASSERT( p_wk->setup );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  WIFI_BSUBWAY_Printf( "Room Load \n" );

  p_wk->room_no = room_no;
  GF_ASSERT( p_wk->room_no <= p_wk->room_num );
  GF_ASSERT_MSG(0,"���C�u�����Ή��̈ו��� 100426\n");
  Dpw_Bt_DownloadRoomAsync( p_wk->rank, p_wk->room_no, &p_wk->bt_roomdata );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData �������@�擾�E�G�C�g
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL ROOM_DATA_LoadRoomDataWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup );
  if( p_wk->in_roomdata )
  {
    return TRUE;
  }

  // ��M�����҂�
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( !ERROR_DATA_IsError( p_error ) ){
      // ����
      p_wk->in_roomdata = TRUE;

#ifdef DOWNLOAD_PRINT
      {
        int i;
        int size;
        u8* p_buff;

        
        size = sizeof(Dpw_Bt_Room);

        p_buff = (u8*)&p_wk->bt_roomdata;
        for( i=0; i<size; i++ ){
          if( (i % 4) == 0 ){
            OS_Printf( "\n" );
          }
          // binary�f�[�^�̏o��
          OS_Printf( "%02x ", p_buff[i] );
        }
      }
#endif  // DOWNLOAD_PRINT
      
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData  rank�̎擾
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRank( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->rank;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RoomData roomNo�̎擾
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomNo( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->room_no;
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData roomNum�̎擾
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomNum( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  return cp_wk->room_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData  �����擾
 *
 *	@param	cp_wk 
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomKeta( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  if( cp_wk->room_num < 100 ){
    return VIEW_NUMBER_KETA_ROOM_NO-1;
  }
  return VIEW_NUMBER_KETA_ROOM_NO;
}


//----------------------------------------------------------------------------
/**
 *  @brief  RoomData �������̎擾
 */
//-----------------------------------------------------------------------------
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  GF_ASSERT( cp_wk->in_roomdata );
  return &cp_wk->bt_roomdata;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[���̃Z�[�u
 *
 *  @param  cp_wk
 *  @param  p_save 
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SavePlayerData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save )
{
  RTCTime time;
  RTCDate date;

  // GameSpy�T�[�o�[���ԁE���t���擾
  DWC_GetDateTime( &date, &time);

  // Player�f�[�^ 
  BSUBWAY_WIFIDATA_SetPlayerData( p_save, (const BSUBWAY_WIFI_PLAYER*)cp_wk->bt_roomdata.player, cp_wk->rank, cp_wk->room_no );

  // ����ςݐݒ�
  BSUBWAY_WIFIDATA_SetRoomDataFlag( p_save, cp_wk->rank, cp_wk->room_no, &date );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�_�[�����̃Z�[�u
 *
 *  @param  cp_wk
 *  @param  p_save 
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SaveLeaderData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save )
{
  // Leader�f�[�^ 
  BSUBWAY_WIFIDATA_SetLeaderData( p_save, (const BSUBWAY_LEADER_DATA*)cp_wk->bt_roomdata.leader, cp_wk->rank, cp_wk->room_no );
}





//-------------------------------------
/// PersonalData
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  ������
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_PERSONAL) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  �j��
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_Exit( WIFI_BSUBWAY_PERSONAL* p_wk )
{
  if(p_wk->p_nhttp){
    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  �v���t�B�[���Z�b�g�A�b�v
 *
 *  @param  p_wk        ���[�N
 *  @param  p_gamedata  �Q�[��Data
 *  @param  p_error     �G���[���[�N
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error )
{
  SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
  MYSTATUS* p_mystatus          = GAMEDATA_GetMyStatus( p_gamedata );
  BSUBWAY_WIFI_DATA* p_bsubway_wifi  = GAMEDATA_GetBSubwayWifiData( p_gamedata );
  BSUBWAY_SCOREDATA* p_bsubway_score  = GAMEDATA_GetBSubwayScoreData( p_gamedata );

  GF_ASSERT( p_wk->setup == FALSE );
  
  // DC_PROFILE�쐬
  DPW_RAP_CreateProfile( &p_wk->dc_profile, p_mystatus );
  
  // �F��
  Dpw_Bt_SetProfileAsync( &p_wk->dc_profile, &p_wk->dc_profile_result );
  ERROR_DATA_GetAsyncStart( p_error );

  
  // ���[�U�[�̃Q�[�����𐶐�
  PERSONAL_DATA_InitDpwPlayerData( &p_wk->bt_player, p_gamedata );

  // Mystatus�ۑ�
  p_wk->cp_mystatus       = p_mystatus;
  p_wk->cp_bsubway_wifi   = p_bsubway_wifi;
  p_wk->cp_bsubway_score  = p_bsubway_score;


  WIFI_BSUBWAY_Printf("Dpw Bsubway Email �F��\n");
}

//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  �v���t�B�[���Z�b�g�A�b�v�����҂�
 *
 *  @param  p_wk      ���[�N
 *  @param  p_error   �G���[���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup == FALSE );

  // ��M�����҂�
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( !ERROR_DATA_IsError( p_error ) ){
      // ����
      p_wk->setup = TRUE;
      return TRUE;
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Dpw_Bt_Player���̐���
 *
 *  @param  p_player  ���i�[��
 *  @param  p_gamedata  �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_InitDpwPlayerData( Dpw_Bt_Player* p_player, GAMEDATA* p_gamedata )
{
  SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
  MYSTATUS* p_mystatus          = GAMEDATA_GetMyStatus( p_gamedata );
  WIFI_HISTORY* p_history       = SaveData_GetWifiHistory( p_savedata );
  WIFI_LIST* p_wifilist         = GAMEDATA_GetWiFiList( p_gamedata );
  BSUBWAY_SCOREDATA* p_score    = GAMEDATA_GetBSubwayScoreData( p_gamedata );

  GFL_STD_MemClear( p_player, sizeof(Dpw_Bt_Player) );

  //name
  GFL_STD_MemCopy( MyStatus_GetMyName( p_mystatus ), p_player->playerName, 8*2 );
  //playerid
  *((u32*)p_player->playerId) = MyStatus_GetID( p_mystatus );
  //version
  p_player->versionCode = CasetteVersion;
  //language
  p_player->langCode = CasetteLanguage;
  //countryCode
  p_player->countryCode = (u8)MyStatus_GetMyNation( p_mystatus );
  //localCode
  p_player->localCode = (u8)MyStatus_GetMyArea( p_mystatus );
  //gender
  p_player->gender = MyStatus_GetMySex( p_mystatus );
  //tr_type
  p_player->trainerType = MyStatus_GetTrainerView( p_mystatus );
 
  //���b�Z�[�W�f�[�^�擾
  //@TODO ���b�Z�[�W���i�[
  /*
  for(i = 0;i < 3;i++){
    GFL_STD_MemCopy(TowerPlayerMsg_Get(sv,BTWR_MSG_PLAYER_READY+i),&(p_player->message[8*i]),8);
  }
  GFL_STD_MemCopy(TowerPlayerMsg_Get(sv,BTWR_MSG_LEADER),p_player->leaderMessage,8);
  */

  //�^���[�Z�[�u�f�[�^�擾
  p_player->result =  BSUBWAY_SCOREDATA_GetWifiScore( p_score );
  //�|�P�����f�[�^�擾
  BSUBWAY_SCOREDATA_GetUsePokeData( p_score, BSWAY_SCORE_POKE_WIFI, (BSUBWAY_POKEMON*)p_player->pokemon );
}



// �|�P�����F��
//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����F�؁@
 *
 *  @param  p_wk      ���[�N
 *  @param  p_error
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, DWCSvlResult* p_svl_result, HEAPID heapID )
{
  BOOL result;
  
  WIFI_BSUBWAY_Printf( "Pokemon�@Upload\n" );
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID,MyStatus_GetProfileID( p_wk->cp_mystatus ), p_svl_result );
 
  
  GF_ASSERT( p_wk->p_nhttp );

  NHTTP_RAP_PokemonEvilCheckCreate( p_wk->p_nhttp, heapID, sizeof(Dpw_Bt_PokemonData)*BSUBWAY_STOCK_WIFI_MEMBER_MAX, NHTTP_POKECHK_SUBWAY);

  // �|�P������ݒ肷��
  NHTTP_RAP_PokemonEvilCheckAdd( p_wk->p_nhttp, (BSUBWAY_POKEMON*)p_wk->bt_player.pokemon, sizeof(Dpw_Bt_PokemonData)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );

  // �f�o�b�N�o��
#ifdef PM_DEBUG
  {
    int i;
    u16* p_bin = (u16*)p_wk->bt_player.pokemon;

    OS_TPrintf( "size %d\n", sizeof(BSUBWAY_POKEMON) );
    for( i=0; i<(sizeof(Dpw_Bt_PokemonData)/2); i++ ){
      OS_TPrintf( "%04x ", p_bin[i] );
    }
    OS_TPrintf( "\n" );
  }
#endif

  // �ݒ�|�P������
  p_wk->poke_num = BSUBWAY_STOCK_WIFI_MEMBER_MAX;

  result = NHTTP_RAP_PokemonEvilCheckConectionCreate( p_wk->p_nhttp );
  GF_ASSERT( result );

  result = NHTTP_RAP_StartConnect( p_wk->p_nhttp );  
  GF_ASSERT( result );


  // �^�C���A�E�g�`�F�b�N�J�n
  ERROR_DATA_StartOnlyTimeOut( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����F�؁@�����҂�
 *
 *  @param  p_wk      ���[�N
 *  @param  p_error
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  NHTTPError error;

  GF_ASSERT( p_wk->p_nhttp );

  WIFI_BSUBWAY_Printf( "." );
  
  error = NHTTP_RAP_Process( p_wk->p_nhttp );
  if( NHTTP_ERROR_NONE != error )
  { 
    // �^�C���A�E�g�J�E���g
    ERROR_DATA_OnlyTimeOutCount( p_error );
  }
  else
  {
    void *p_data;
    int i;
    int poke_result;
    p_data  = NHTTP_RAP_GetRecvBuffer( p_wk->p_nhttp );


    //�����Ă�����Ԃ͐��킩
    if( NHTTP_RAP_EVILCHECK_GetStatusCode( p_data ) == 0 )
    {
      // ����

      // �������擾
      { 
        const s8 *cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_data, p_wk->poke_num );
        GFL_STD_MemCopy( cp_sign, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
      }

      for( i=0; i<p_wk->poke_num; i++ ){
        poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
        if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
          ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_POKE_ERROR );
          break;
        }
      }
      if( i==p_wk->poke_num ){
        WIFI_BSUBWAY_Printf( "�F�؁@OK\n" );
      }else{
        WIFI_BSUBWAY_Printf( "�F�؁@Error %d\n", i );
      }
    }
    else
    {
      // �F�؊m�F���s
      ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_DISCONNECTED );
      WIFI_BSUBWAY_Printf( "�F�؁@Error %d\n", NHTTP_RAP_EVILCHECK_GetStatusCode( p_data ) );

      for( i=0; i<p_wk->poke_num; i++ ){
        poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
        WIFI_BSUBWAY_Printf( "�F�؁@Error pokenum %d  result %d\n", i, poke_result );
      }
    }

    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;


    WIFI_BSUBWAY_Printf( "\n" );
    return TRUE;
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Personal Data �A�b�v�f�[�g
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  GF_ASSERT( p_wk->setup );

  WIFI_BSUBWAY_Printf( "�l�����@Upload\n" );
  {
    s32 rank, room_no;
    s32 win;
    // ���탉���N�E�����E�������������擾
    rank = BSUBWAY_WIFIDATA_GetPlayerRank( p_wk->cp_bsubway_wifi );
    room_no = BSUBWAY_WIFIDATA_GetPlayerRoomNo( p_wk->cp_bsubway_wifi );
    win = BSUBWAY_SCOREDATA_GetWifiNum( p_wk->cp_bsubway_score );

    GF_ASSERT_MSG(0,"���C�u�����Ή��̈ו��� 100426\n");
    Dpw_Bt_UploadPlayerAsync(  rank,  room_no,  win, &p_wk->bt_player, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
    WIFI_BSUBWAY_Printf( "Start\n" );
    ERROR_DATA_GetAsyncStart( p_error );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  Personal Data �A�b�v�f�[�g�����҂�
 *
 *  @param  p_wk      ���[�N
 *  @param  p_error   �G���[���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  s32 result;

  // ��M�����҂�
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( !ERROR_DATA_IsError( p_error ) ){
      return TRUE;
    }
  }
  return FALSE;
}







//-------------------------------------
/// ErrorData
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  ErrorData������
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ERROR) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Erro���m�@�\���@����M�����J�n
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Error���m�@�\�� SERVERSTATUS�@��M����
 *
 *  @param  p_wk  �G���[���[�N
 *
 *  @retval TRUE    ��M����
 *  @retval FALSE   ��M��
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_GetAsyncServerResult( WIFI_BSUBWAY_ERROR* p_wk )
{
  s32 result = 0;
  BOOL complete = FALSE;
  
  if( !Dpw_Bt_IsAsyncEnd() ){
    // �^�C���A�E�g�`�F�b�N
    p_wk->timeout ++;
    if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
      // �ߋ���
      // CommFatalErrorFunc_NoNumber(); //�����ӂ��Ƃ΂�
      // @TODO �ӂ��Ƃ΂��@SERVER�^�C���A�E�g�Ɠ��l�̏������s���B
      // �L�����Z���R�[��
      Dpw_Bt_CancelAsync();

      // �^�C���A�E�g��
      p_wk->timeout_flag = TRUE;
    }
  }else{
    result    = Dpw_Bt_GetAsyncResult();
    complete  = TRUE;
    switch (result){
    case DPW_TR_STATUS_SERVER_OK:   // ����ɓ��삵�Ă���
      WIFI_BSUBWAY_Printf(" server ok\n");
      p_wk->server_status = result;
      break;
    case DPW_TR_STATUS_SERVER_STOP_SERVICE: // �T�[�r�X��~��
    case DPW_TR_STATUS_SERVER_FULL:     // �T�[�o�[�����t
      WIFI_BSUBWAY_Printf(" server ng\n");
      p_wk->server_status = result;
      break;

    case DPW_TR_ERROR_SERVER_FULL:
    case DPW_TR_ERROR_CANCEL:
    case DPW_TR_ERROR_FAILURE:
    case DPW_TR_ERROR_SERVER_TIMEOUT:
    case DPW_TR_ERROR_DISCONNECTED: 
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      WIFI_BSUBWAY_Printf(" server ng\n");
      p_wk->error_code = result;
      break;

    case DPW_TR_ERROR_FATAL:      //!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
    default:
      NetErr_DispCall(TRUE);
      break;
    }
  }

  return complete;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Error���m�@�\���@����M����
 *
 *  @param  p_wk        ���[�N
 *  @param  p_result    ���ʊi�[��
 *
 *  @retval TRUE    ��M����
 *  @retval FALSE   ��M��
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result )
{
  s32 result = 0;
  BOOL complete = FALSE;
  
  if( !Dpw_Bt_IsAsyncEnd() ){
    // �^�C���A�E�g�`�F�b�N
    p_wk->timeout ++;
    if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
      // �ߋ���
      // CommFatalErrorFunc_NoNumber(); //�����ӂ��Ƃ΂�
      // @TODO �ӂ��Ƃ΂��@SERVER�^�C���A�E�g�Ɠ��l�̏������s���B
      // �L�����Z���R�[��
      Dpw_Bt_CancelAsync();

      // �^�C���A�E�g��
      p_wk->timeout_flag = TRUE;
    }
  }else{
    result    = Dpw_Bt_GetAsyncResult();
    complete  = TRUE;
    switch (result){
    case DPW_TR_ERROR_SERVER_FULL:
    case DPW_TR_ERROR_CANCEL:
    case DPW_TR_ERROR_FAILURE:
    case DPW_TR_ERROR_SERVER_TIMEOUT:
    case DPW_TR_ERROR_DISCONNECTED: 
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      WIFI_BSUBWAY_Printf(" recv ng\n");
      p_wk->error_code = result;
      break;

    case DPW_TR_ERROR_FATAL:      //!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
      NetErr_DispCall(TRUE);
      break;

    default:
      if( result >= 0 )
      {
        WIFI_BSUBWAY_Printf(" recv ok\n");
        *p_result = result;
      }
      else
      {
        // �}�C�i�X��Error
        NetErr_DispCall(TRUE);
      }
      break;
    }
  }

  return complete;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Dpw Bt�̃G���[��ԃ`�F�b�N
 *
 *  @param  cp_wk   ���[�N
 *
 *  @retval TRUE    �G���[���
 *  @retval FALSE   �G���[��Ԃł͂Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  // BT�G���[
  if( (cp_wk->error_code < 0) ){
    return TRUE;
  }
  // SERVER�G���[
  if( cp_wk->server_status != DPW_BT_STATUS_SERVER_OK ){
    return TRUE;
  }
  // nhttp�G���[
  if( cp_wk->nhttp_error != BSUBWAY_NHTTP_ERROR_NONE ){
    return TRUE;
  }
  // �^�C���A�E�g
  if( cp_wk->timeout_flag ){
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ErrorData Dpw BT�̃G���[���b�Z�[�W�擾
 *
 *  @param  cp_wk   ���[�N
 *    
 *  @return �G���[���b�Z�[�WID
 */
//-----------------------------------------------------------------------------
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  int msgno =0;

  // @TODO �G���[���b�Z�[�W���ł��B
  //

  // BtError
  if( cp_wk->error_code < 0 ){
    switch( cp_wk->error_code ){
    case DPW_BT_ERROR_SERVER_TIMEOUT:
    case DPW_BT_ERROR_DISCONNECTED:
      msgno = msg_wifi_bt_error_003;
      break;
    case DPW_BT_ERROR_SERVER_FULL:
      msgno = msg_wifi_bt_error_002;
      break;
    case DPW_BT_ERROR_FAILURE:
    case DPW_BT_ERROR_CANCEL:
      msgno = msg_wifi_bt_error_004;
      break;
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      msgno = msg_wifi_bt_error_005;
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  // SERVERError
  }else if( cp_wk->server_status != DPW_BT_STATUS_SERVER_OK ){
    switch( cp_wk->server_status ){
    case DPW_BT_STATUS_SERVER_STOP_SERVICE:
      msgno = msg_wifi_bt_error_001;
      break;
    case DPW_BT_STATUS_SERVER_FULL:
      msgno = msg_wifi_bt_error_002;
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  // nhttp �G���[
  }else if( cp_wk->nhttp_error != BSUBWAY_NHTTP_ERROR_NONE ){
    switch( cp_wk->nhttp_error ){
    case BSUBWAY_NHTTP_ERROR_POKE_ERROR:
      msgno = msg_wifi_bt_error_003;
      break;
    case BSUBWAY_NHTTP_ERROR_DISCONNECTED:
      msgno = msg_wifi_bt_error_003;
      break;
    default:
      GF_ASSERT(0);
      break;
    }
  }
  else if( cp_wk->timeout_flag ){
    // �^�C���A�E�g�G���[
    msgno = msg_wifi_bt_error_003;
  }
  else{
    GF_ASSERT(0);
  }
  

  return msgno;
}

//----------------------------------------------------------------------------
/**
 *  @brief  NhttpError  �ݒ�
 *
 *  @param  p_wk    ���[�N
 *  @param  error   �G���[�^�C�v
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error )
{
  p_wk->nhttp_error = error;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �^�C���A�E�g���J�n
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �^�C���A�E�g�J�E���g
 *
 *  @param  p_wk  ���[�N
 *
 *  �^�C���A�E�g�ɂȂ����玩���ŃG���[�����֍s���܂��B
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_OnlyTimeOutCount( WIFI_BSUBWAY_ERROR* p_wk )
{
  // �^�C���A�E�g�`�F�b�N
  p_wk->timeout ++;
  if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
    p_wk->timeout_flag = TRUE;
  }
}


//-------------------------------------
/// CodeIn
//=====================================
//----------------------------------------------------------------------------
/**
 *  @briefCodeIn�J�n
 *
 *  @param  p_wk      ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void CODEIN_StartRoomNoProc( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  static s_CODEIN_BLOCK[CODE_BLOCK_MAX] = {
    VIEW_NUMBER_KETA_ROOM_NO,
    0,
    0,
  };

  s_CODEIN_BLOCK[ 0 ] = ROOM_DATA_GetRoomKeta( &p_wk->roomdata );
  p_wk->p_codein = CodeInput_ParamCreate( heapID, CODEIN_MODE_TRAIN_NO, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), VIEW_NUMBER_KETA_ROOM_NO, s_CODEIN_BLOCK );
  
  // �v���b�N�R�[��
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(codein), &CodeInput_ProcData, p_wk->p_codein );
}

//----------------------------------------------------------------------------
/**
 *  @briefCodeIn�J�n
 *
 *  @param  p_wk      ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void CODEIN_StartRankProc( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  static int s_CODEIN_BLOCK[CODE_BLOCK_MAX] = {
    VIEW_NUMBER_KETA_RANK,
    0,
    0,
  };

  p_wk->p_codein = CodeInput_ParamCreate( heapID, CODEIN_MODE_RANK, DPW_BT_RANK_NUM, VIEW_NUMBER_KETA_RANK, s_CODEIN_BLOCK );
  
  // �v���b�N�R�[��
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(codein), &CodeInput_ProcData, p_wk->p_codein );
}

//----------------------------------------------------------------------------
/**
 *  @brief  CodeIn�I��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk )
{
  if( p_wk->p_codein )
  {
    WIFI_BSUBWAY_Printf( "CodeIn return %d\n", p_wk->p_codein->end_state );
    
    CodeInput_ParamDelete( p_wk->p_codein );
    p_wk->p_codein = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  CodeIn�@���͕�����擾
 *
 *  @param  cp_wk   ���[�N
 *  @param  p_str �@�i�[�o�b�t�@
 */
//-----------------------------------------------------------------------------
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str )
{
  GF_ASSERT( cp_wk->p_codein );

  GFL_STR_CopyBuffer( p_str, cp_wk->p_codein->strbuf );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���͐��l���擾
 *
 *  @param  cp_wk 
 */
//-----------------------------------------------------------------------------
static int CODEIN_GetCode( const WIFI_BSUBWAY* cp_wk )
{
  const STRCODE eonCode = GFL_STR_GetEOMCode();
  const STRCODE *code = GFL_STR_GetStringCodePointer( cp_wk->p_codein->strbuf );

  int num = 0;
  u32 digit = 1;
  u16 arrDigit = 0;
  u16 tempNo;

  static const u8 MAX_DIGIT = 6;

  while( code[arrDigit] != eonCode )
  {
    if( arrDigit >= MAX_DIGIT )
    {
      //���I�[�o�[
      GF_ASSERT(0);
    }

    tempNo = code[arrDigit] - L'0';
    if( tempNo >= 10 )
    {
      tempNo = code[arrDigit] - L'�O';
      if( tempNo >= 10 )
      {
        //�z��O�̕����������Ă���
        GF_ASSERT(0);
      }
    }
    /*
    num += tempNo * digit;
    digit *= 10;
    */
    num *= 10;
    num += tempNo;
    arrDigit++;
  }
  return num;
}




//-------------------------------------
/// Login
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  Login���� �J�n
 *
 *  @param  p_wk      ���[�N  
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->login.gamedata  = p_wk->p_gamedata;
  p_wk->login.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->login.bgm       = WIFILOGIN_BGM_NONE;
  p_wk->login.display   = WIFILOGIN_DISPLAY_UP;
  p_wk->login.nsid      = WB_NET_BSUBWAY;
  p_wk->login.pSvl      = &p_wk->svl_result;


  // �v���b�N�R�[��
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &p_wk->login );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Login���� ��n��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  Login���� ����
 *
 *  @param  cp_wk   ���[�N
 *
 *  @retval WIFILOGIN_RESULT_LOGIN, //���O�C������
 *  @retval WIFILOGIN_RESULT_CANCEL,//�L�����Z������
 */
//-----------------------------------------------------------------------------
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk )
{
  return cp_wk->login.result;
}



//-------------------------------------
/// Logout
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  Logout�����@�J�n
 *
 *  @param  p_wk      ���[�N
 *  @param  heapID    �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->logout.gamedata  = p_wk->p_gamedata;
  p_wk->logout.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->logout.display   = WIFILOGIN_DISPLAY_UP;
  p_wk->logout.bgm       = WIFILOGIN_BGM_NONE;

  // �v���b�N�R�[��
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(wifi_login), &WiFiLogout_ProcData, &p_wk->logout );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Logout����  �I��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk )
{
}




//-------------------------------------
/// ���C����������
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�@���C������
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval WIFI_BSUBWAY_RESULT_OK,                   //�A�b�v�f�[�g�E�_�E�����[�h����
 *  @retval WIFI_BSUBWAY_RESULT_NG,                   //���s
 *  @retval WIFI_BSUBWAY_RESULT_CANCEL,               //�L�����Z������
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  BSUBWAY_MAIN_RESULT (*pFunc[ WIFI_BSUBWAY_MODE_MAX ])( WIFI_BSUBWAY* p_wk, HEAPID heapID ) = 
  {
    WiFiBsubway_Main_ScoreUpload,
    WiFiBsubway_Main_GamedataDownload,
    WiFiBsubway_Main_SuccessdataDownload,
  };
  GF_ASSERT( p_wk->p_param->mode < WIFI_BSUBWAY_MODE_MAX );
  
  return pFunc[ p_wk->p_param->mode ]( p_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �O����̃A�b�v���[�h
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_PERSON_MSG_00:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_008 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP:
    PERSONAL_DATA_SetUpNhttpPokemon( &p_wk->personaldata, &p_wk->bt_error, &p_wk->svl_result, heapID );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT:
    if( PERSONAL_DATA_SetUpNhttpPokemonWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      p_wk->seq++;
    }
    break;
  case SCORE_UPLOAD_SEQ_PERSON_UPDATE:
    PERSONAL_DATA_UploadPersonalData( &p_wk->personaldata, &p_wk->bt_error );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_UPDATE_WAIT:
    if( PERSONAL_DATA_UploadPersonalDataWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SAVE:
    // �o�g��Tower ���A�b�v���[�h�t���O���N���A
    BSUBWAY_SCOREDATA_SetFlag( p_wk->p_bsubway_score, BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD, BSWAY_SETMODE_reset );

    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SAVE_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_009 );
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_PERSON_MSG_01:
    if( VIEW_PrintMain( &p_wk->view ) )
    {
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_PERSON_END:
    return BSUBWAY_MAIN_RESULT_OK;

  default:
    GF_ASSERT(0);
    break;
  }

  return BSUBWAY_MAIN_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �Q�[�����̃_�E�����[�h
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK:
    {
      u32 rank = BSUBWAY_SCOREDATA_GetWifiRank( p_wk->p_bsubway_score );

      GF_ASSERT( (rank >= 1) && (rank <= DPW_BT_RANK_NUM) );
      
      // �����B
      ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, rank );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){

      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00:
    VIEW_EndTimeIcon( &p_wk->view );
    VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_001 );
    p_wk->seq++;  
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO:
    CODEIN_StartRoomNoProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT:

    p_wk->code_input = CODEIN_GetCode( p_wk );
    
    // ��������擾
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      RTCTime time;
      RTCDate date;
      u32 rank = ROOM_DATA_GetRank( &p_wk->roomdata );
      u32 roomno = p_wk->code_input;
      u32 roomnum = ROOM_DATA_GetRoomNum( &p_wk->roomdata );


      // GameSpy�T�[�o�[���ԁE���t���擾
      DWC_GetDateTime( &date, &time);

      // ���͔͈͓����H
      if( (roomno < 1) || (roomno > roomnum) ){
        
        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG;

      }
      // �����͊��Ƀ_�E�����[�h���Ă��邩�H
      else if( BSUBWAY_WIFIDATA_CheckRoomDataFlag( p_wk->p_bsubway_wifi, rank, roomno, &date )){
        
        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG;
        
      }else{
        
        // �����B
        ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );

        // ���b�Z�[�W�\��
        VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNo( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
        VIEW_SetWordNumber( &p_wk->view, 1, ROOM_DATA_GetRank( &p_wk->roomdata ), VIEW_NUMBER_KETA_RANK );
        VIEW_PrintStream( &p_wk->view, msg_wifi_bt_002 );

        VIEW_StartTimeIcon( &p_wk->view );

        p_wk->seq++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_SAVE:
    // �Z�[�u�f�[�^���f
    {
      ROOM_DATA_SavePlayerData( &p_wk->roomdata, p_wk->p_bsubway_wifi );
    }

    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_004 );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_02:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END:
    return BSUBWAY_MAIN_RESULT_OK;


  // �{���_�E�����[�h�ς݃��b�Z�[�W�\��
  case SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_003 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_005 );
    p_wk->seq++;
    break;
    
  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO:
    VIEW_StartYesNo( &p_wk->view );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_WAIT:
    {
      u32 result = VIEW_MainYesNo( &p_wk->view );
      if( result == 0 ){
        return BSUBWAY_MAIN_RESULT_CANCEL;
      }else if( result == BMPMENU_CANCEL ){

        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00;
      }
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
  return BSUBWAY_MAIN_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����̃_�E�����[�h
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00:
    VIEW_EndTimeIcon( &p_wk->view );
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_006 );
    p_wk->seq++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK:
    CODEIN_StartRankProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT:
    p_wk->code_input = CODEIN_GetCode( p_wk );
    // ��������擾
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      // ��񂪔͈͓����H
      if( (p_wk->code_input >= 1) && (p_wk->code_input <= DPW_BT_RANK_NUM) ){
        
        // �����B
        ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );
        VIEW_StartTimeIcon( &p_wk->view );
        p_wk->seq ++;
      }else{

        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_007 );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_01:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO:
    CODEIN_StartRoomNoProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT:
    p_wk->code_input = CODEIN_GetCode( p_wk );
    // ��������擾
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      u32 room_num = ROOM_DATA_GetRoomNum( &p_wk->roomdata );
      
      //�l���͈͓����H
      if( (p_wk->code_input < 1) || (p_wk->code_input > room_num) ){
        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG;
      }else{
      
        // �����B
        ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );

        // ���b�Z�[�W�\��
        VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNo( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
        VIEW_SetWordNumber( &p_wk->view, 1, ROOM_DATA_GetRank( &p_wk->roomdata ), VIEW_NUMBER_KETA_RANK );
        VIEW_PrintStream( &p_wk->view, msg_wifi_bt_002 );

        VIEW_StartTimeIcon( &p_wk->view );

        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_SAVE:
    // �Z�[�u�f�[�^���f
    {
      ROOM_DATA_SaveLeaderData( &p_wk->roomdata, p_wk->p_bsubway_wifi );
    }
    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_004 );
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_03:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;


  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END:
    return BSUBWAY_MAIN_RESULT_OK;


  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_010 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO:
    VIEW_StartYesNo( &p_wk->view );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_WAIT:
    {
      u32 result = VIEW_MainYesNo( &p_wk->view );
      if( result == 0 ){
        return BSUBWAY_MAIN_RESULT_CANCEL;
      }else if( result == BMPMENU_CANCEL )
      {

        // ROOM_DATA���Z�b�g
        ROOM_DATA_Init( &p_wk->roomdata );
        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00;
      }
    }
    break;


  default:
    GF_ASSERT(0);
    break;
  }
  return BSUBWAY_MAIN_RESULT_CONTINUE;
}



  
//-------------------------------------
/// �ڑ��E�ؒf�E�G���[
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �ڑ��J�n
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk )
{
  // ���E�����ڑ�������
  DWCUserData* p_userdata;    // �F�؍ς݂�DWCUSER�f�[�^�������Ȃ��͂�
  s32 profileId;

  WIFI_BSUBWAY_Printf( "Connect Bsuway Start\n" );

  // DWCUser�\���̎擾
  p_userdata = WifiList_GetMyUserInfo( p_wk->p_wifilist );

  MyStatus_SetProfileID( p_wk->p_mystatus, WifiList_GetMyGSID( p_wk->p_wifilist ) );

  // �����ȃf�[�^���擾
  profileId = MyStatus_GetProfileID( p_wk->p_mystatus );
  WIFI_BSUBWAY_Printf("Dpw�T�[�o�[���O�C����� profileId=%08x\n", profileId);

  // DPW_BT������
  Dpw_Bt_Init( profileId, DWC_CreateFriendKey( p_userdata ),LIBDPW_SERVER_TYPE );

  WIFI_BSUBWAY_Printf("Dpw Bsubway ������\n");

  Dpw_Bt_GetServerStateAsync();
  ERROR_DATA_GetAsyncStart( &p_wk->bt_error );

  WIFI_BSUBWAY_Printf("Dpw Bsubway �T�[�o�[��Ԏ擾�J�n\n");

  p_wk->dpw_main_do = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڑ��E�G�C�g
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE    �ڑ�����
 *  @retval FALSE   �ڑ��r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
  return ERROR_DATA_GetAsyncResult( &p_wk->bt_error, &result );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ؒf�J�n
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk )
{
  if( Dpw_Bt_IsAsyncEnd() == FALSE ){

    WIFI_BSUBWAY_Printf("Dpw Bsubway �ؒf\n");
    // �L�����Z���R�[��
    Dpw_Bt_CancelAsync();
    ERROR_DATA_GetAsyncStart( &p_wk->bt_error );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ؒf�E�G�C�g
 *  
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
  // ���łɐؒf�ς݁H
  if(p_wk->dpw_main_do == FALSE){
    return TRUE;
  }


  if( ERROR_DATA_GetAsyncResult( &p_wk->bt_error, &result ) ){
    // �I��
    Dpw_Bt_End();

    p_wk->dpw_main_do = FALSE;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �G���[����
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval BOOL  ErrorWait�̗L��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk )
{
  // Net�G���[�`�F�b�N
  if( GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE) )
  {
    // �I����
    return FALSE;
  }

  // ���̑� Net�G���[�̏ꍇ
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE ){
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

    WIFI_BSUBWAY_Printf( "not message error %d\n", cp_error->errorRet );

#if 0
    //GDB��SC�̐ؒf��Fatal�G���[�ȊO�́A�ʏ����Ƃ��邽�߁A���m���Ȃ�
    if( (cp_error->errorRet == DWC_ERROR_GDB_ANY
          || cp_error->errorRet == DWC_ERROR_SCL_ANY)
        && (cp_error->errorType != DWC_ETYPE_DISCONNECT
          && cp_error->errorType != DWC_ETYPE_FATAL) )
    { 
      return FALSE;
    }
#endif
    return FALSE;
  }
  
  // Bt�G���[�`�F�b�N
  if( ERROR_DATA_IsError( &p_wk->bt_error ) )
  {
    s32 msg_id;
    //�@�G���[�\������
    // �G���[�\���I����҂�
    msg_id = ERROR_DATA_GetPrintMessageID( &p_wk->bt_error );

    // TimeWait Off
    VIEW_EndTimeIcon( &p_wk->view );
    VIEW_PrintStream( &p_wk->view, msg_id );
    return TRUE;
  }


  // Not Error
  GF_ASSERT(0);
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �G���[�����@�E�G�C�g
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE    ����
 *  @retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk )
{
  return VIEW_PrintMain( &p_wk->view );
}





//-------------------------------------
/// View
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �`�揈��  ������
 *
 *  @param  p_wk      ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void VIEW_Init( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID )
{
  GF_ASSERT( p_wk->setup == FALSE );
  
  // �`��V�X�e������
  p_wk->p_graphic = WIFI_BSUBWAY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, heapID );

  // �q�[�vID�ۑ�
  p_wk->heapID = heapID;

  // ��{�O���t�B�b�N�]��
  VIEW_InitResource( p_wk, heapID );

  // �r�b�g�}�b�v����
  p_wk->p_win = GFL_BMPWIN_Create(
    VIEW_FRAME_MAIN_WIN,
    1 , 19, 30 ,4,
    VIEW_PAL_MSG, GFL_BMP_CHRAREA_GET_B );

  BmpWinFrame_Write( p_wk->p_win, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar), VIEW_PAL_WIN_FRAME );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_win), 15);
  GFL_BMPWIN_MakeScreen(p_wk->p_win);
  GFL_BMPWIN_TransVramCharacter(p_wk->p_win);
  GFL_BG_LoadScreenV_Req( VIEW_FRAME_MAIN_WIN );

  // ���b�Z�[�W�f�[�^����
  p_wk->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_bsubway_dat, heapID );
  p_wk->p_wordset = WORDSET_Create( heapID );
  p_wk->p_font = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );
  p_wk->p_strtmp  = GFL_STR_CreateBuffer( 128, heapID );
  p_wk->p_str  = GFL_STR_CreateBuffer( 128, heapID );


  // �v�����g�^�X�N
  p_wk->p_msg_tcbsys = GFL_TCBL_Init( heapID , heapID , 1 , 0 );

  p_wk->setup = TRUE;

  // OAM�ʂ̕\��ON
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // �ʐM�A�C�R��ON
  GFL_NET_ReloadIcon();

}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�揈��  �j��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void VIEW_Exit( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->setup == FALSE )
  {
    return ;
  }
  
  // 
  if(p_wk->p_msg_stream)
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
  }

  // �v�����g�^�X�N
  GFL_TCBL_Exit( p_wk->p_msg_tcbsys );

  // ���b�Z�[�W�f�[�^����
  GFL_MSG_Delete( p_wk->p_msgdata );
  WORDSET_Delete( p_wk->p_wordset );
  GFL_FONT_Delete( p_wk->p_font );
  GFL_STR_DeleteBuffer( p_wk->p_strtmp );
  GFL_STR_DeleteBuffer( p_wk->p_str );

  // ��{�O���t�B�b�N�]��
  VIEW_ExitResource( p_wk );

  // TIMEICON�������Ă���
  VIEW_EndTimeIcon( p_wk );

  // YESNO�����j��
  if( p_wk->p_yesno ){
    BmpMenu_YesNoMenuExit( p_wk->p_yesno );
    p_wk->p_yesno = NULL;
  }


  // win�j��
  GFL_BMPWIN_Delete( p_wk->p_win );
  
  // �`��V�X�e������
  WIFI_BSUBWAY_GRAPHIC_Exit( p_wk->p_graphic );


  GFL_FONTSYS_SetDefaultColor();
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_VIEW) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�揈��  ���C��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void VIEW_Main( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->setup == FALSE ){
    return ;
  }
  
  GFL_TCBL_Main( p_wk->p_msg_tcbsys );
  ConnectBGPalAnm_Main( &p_wk->bg_palanm );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�NResource�̃Z�b�g�A�b�v
 *
 *  @param  p_wk      ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void VIEW_InitResource( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_LOGIN, heapID );
  u32 char_ofs;
  
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifi_login_connect_win_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  heapID);
  // �T�u���BG0�L�����]��
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_SUB_BACKGROUND, 0, 0, heapID);

  // �T�u���BG0�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win2_d_NSCR,
                                            VIEW_FRAME_SUB_BACKGROUND, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);


  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifi_login_connect_win_NCLR,
                                    PALTYPE_MAIN_BG, 0, 0,  heapID);
  // ���C�����BG0�L�����]��
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_MAIN_BACKGROUND, 0, 0, heapID);

  // ���C�����BG0�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win1_u_NSCR,
                                            VIEW_FRAME_MAIN_BACKGROUND, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);

  // ���C�����BG3�L�����]��
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_MAIN_BACKGROUND_BASE, 0, 0, heapID);

  // ���C�����BG3�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win2_u_NSCR,
                                            VIEW_FRAME_MAIN_BACKGROUND_BASE, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);


  //�p���b�g�A�j���V�X�e���쐬
  ConnectBGPalAnm_InitBg(&p_wk->bg_palanm, p_handle, NARC_wifi_login_connect_ani_NCLR, heapID, VIEW_FRAME_MAIN_BACKGROUND, VIEW_FRAME_MAIN_BACKGROUND_BASE );

  GFL_ARC_CloseDataHandle(p_handle);



  // �t���[������
  GFL_BG_AllocCharacterArea( VIEW_FRAME_MAIN_WIN, GFL_BG_1CHRDATASIZ, GFL_BG_CHRAREA_GET_F ); //0char�������Ƃ�
  GFL_BG_AllocCharacterArea( VIEW_FRAME_MAIN_YESNO, GFL_BG_1CHRDATASIZ, GFL_BG_CHRAREA_GET_F ); //0char�������Ƃ�
  p_wk->frame_bgchar = BmpWinFrame_GraphicSetAreaMan( VIEW_FRAME_MAIN_WIN, VIEW_PAL_WIN_FRAME, MENU_TYPE_SYSTEM, heapID);
  p_wk->frame_bgchar_sys = BmpWinFrame_GraphicSetAreaMan( VIEW_FRAME_MAIN_YESNO, VIEW_PAL_WIN_FRAME, MENU_TYPE_SYSTEM, heapID);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                              0x20*VIEW_PAL_MSG, 0x20, heapID);
}


//----------------------------------------------------------------------------
/**
 *  @brief  Resource�j��
 */
//-----------------------------------------------------------------------------
static void VIEW_ExitResource( WIFI_BSUBWAY_VIEW* p_wk )
{
  GFL_BG_FreeCharacterArea( VIEW_FRAME_MAIN_WIN,
      GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar),
      GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_bgchar));

  GFL_BG_FreeCharacterArea( VIEW_FRAME_MAIN_YESNO,
      GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar_sys),
      GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_bgchar_sys));

  ConnectBGPalAnm_End( &p_wk->bg_palanm );
  G2_BlendNone();

}




//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�`��J�n
 *
 *  @param  p_wk    ���[�N
 *  @param  msg_id  ���b�Z�[�WID
 */
//-----------------------------------------------------------------------------
static void VIEW_PrintStream( WIFI_BSUBWAY_VIEW* p_wk, u32 msg_id )
{
  if( p_wk->p_msg_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
    p_wk->p_msg_stream = NULL;
  }

  GFL_MSG_GetString( p_wk->p_msgdata, msg_id, p_wk->p_strtmp );
  WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_strtmp );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_win), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  p_wk->p_msg_stream = PRINTSYS_PrintStream(p_wk->p_win,0,0, p_wk->p_str, p_wk->p_font,
                                        MSGSPEED_GetWait(), p_wk->p_msg_tcbsys, 2, p_wk->heapID, 15);

  BmpWinFrame_Write( p_wk->p_win, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar), VIEW_PAL_WIN_FRAME );

  GFL_BMPWIN_TransVramCharacter(p_wk->p_win);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�`�抮���҂�
 *
 *  @param  cp_wk   ���[�N
 *
 *  @retval TRUE  ����
 *  @retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL VIEW_PrintMain( WIFI_BSUBWAY_VIEW* p_wk )
{
  int state;
  
  if( p_wk->p_msg_stream == NULL ){
    return TRUE;
  }
  
  state = PRINTSYS_PrintStreamGetState( p_wk->p_msg_stream );
  
  if( state == PRINTSTREAM_STATE_DONE ){
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
    p_wk->p_msg_stream = NULL;
    return TRUE;
  }else if ( state == PRINTSTREAM_STATE_PAUSE )
  {
    if( (GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE) || GFL_UI_TP_GetTrg() ){
      PRINTSYS_PrintStreamReleasePause( p_wk->p_msg_stream );
    }
  }
  
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g�ɐ��l��ݒ�
 *
 *  @param  p_wk      ���[�N
 *  @param  idx       �C���f�b�N�X
 *  @param  number    �i���o�[
 *  @param  keta      ��
 *  @param  dispType  �\���^�C�v
 */
//-----------------------------------------------------------------------------
static void VIEW_SetWordNumber( WIFI_BSUBWAY_VIEW* p_wk, u32 idx, u32 number, u32 keta )
{
  WORDSET_RegisterNumber( p_wk->p_wordset, idx, number, keta, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YES NO�E�B���h�E�̕\�� ON
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void VIEW_StartYesNo( WIFI_BSUBWAY_VIEW* p_wk )
{
  static BMPWIN_YESNO_DAT s_BMPWIN_YESNO_DAT = {
    VIEW_FRAME_MAIN_YESNO,
    24, 13,   // X Y
    VIEW_PAL_MSG,        // PAL
    0,                  // CHR
  };
  
  GF_ASSERT( p_wk->p_yesno == NULL );

  s_BMPWIN_YESNO_DAT.chrnum = GFL_ARCUTIL_TRANSINFO_GetPos( p_wk->frame_bgchar_sys );
  
  p_wk->p_yesno = BmpMenu_YesNoSelectInit( &s_BMPWIN_YESNO_DAT, 
      GFL_ARCUTIL_TRANSINFO_GetPos( p_wk->frame_bgchar_sys ), VIEW_PAL_WIN_FRAME, 0, p_wk->heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YesNo�܂�
 *
 *  @param  p_wk 
 *
 * @retval  "BMPMENU_NULL �I������Ă��Ȃ�"
 * @retval  "0        �͂���I��"
 * @retval  "BMPMENU_CANCEL ������or�L�����Z��"
 */
//-----------------------------------------------------------------------------
static u32 VIEW_MainYesNo( WIFI_BSUBWAY_VIEW* p_wk )
{
  u32 result = BMPMENU_CANCEL;

  if( p_wk->p_yesno )
  {
    result = BmpMenu_YesNoSelectMain( p_wk->p_yesno );
    if( result != BMPMENU_NULL )
    {
      p_wk->p_yesno = NULL;
    }
  }
   
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �^�C���A�C�R���@�J�n
 *
 *	@param	p_wk    ���[�N 
 */
//-----------------------------------------------------------------------------
static void VIEW_StartTimeIcon( WIFI_BSUBWAY_VIEW* p_wk )
{
  GF_ASSERT( p_wk->p_timeicon == NULL );
  p_wk->p_timeicon = TIMEICON_Create( GFUser_VIntr_GetTCBSYS(), p_wk->p_win, 15, TIMEICON_DEFAULT_WAIT, p_wk->heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �^�C���A�C�R���@�I��
 *
 *	@param	p_wk    ���[�N 
 */
//-----------------------------------------------------------------------------
static void VIEW_EndTimeIcon( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->p_timeicon )
  {
    TILEICON_Exit( p_wk->p_timeicon );
    p_wk->p_timeicon =NULL;
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief  �Z�[�u�J�n
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SAVE_Start( WIFI_BSUBWAY* p_wk )
{
  GF_ASSERT( p_wk->save_do == FALSE );
  GAMEDATA_SaveAsyncStart(p_wk->p_gamedata);
  p_wk->save_do = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �Z�[�u���C��
 *
 *  @param  p_wk  ���[�N
 *
 *  @retval TRUE  ����
 *  @retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL SAVE_Main( WIFI_BSUBWAY* p_wk )
{
  SAVE_RESULT result;

  if( p_wk->save_do == FALSE ){
    return TRUE;
  }

  result = GAMEDATA_SaveAsyncMain(p_wk->p_gamedata);
  if( (result == SAVE_RESULT_OK) ){
    p_wk->save_do = FALSE;
    return TRUE;
  }
  return FALSE;
}


