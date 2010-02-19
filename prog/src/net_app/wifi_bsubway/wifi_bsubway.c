//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_bsubway.c
 *	@brief  Wi-Fi�o�g���T�u�E�F�C
 *	@author	tomoya takahashi
 *	@date		2010.02.17
 *
 *	���W���[�����FWIFI_BSUBWAY
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include <dpw_bt.h>

#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_error.h"
#include "net/dpw_rap.h"
#include "net/nhttp_rap.h"
#include "net/nhttp_rap_evilcheck.h"

#include "system/net_err.h"
#include "system/main.h"

#include "savedata/wifilist.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "savedata/system_data.h"

#include "app/codein.h"

#include "msg/msg_wifi_bsubway.h"

#include "net_app/wifi_bsubway.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"

#include "wifi_bsubway_graphic.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���C���V�[�P���X
//=====================================
enum {

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

  BSUBWAY_SEQ_ALL_END,   // �I��

  BSUBWAY_SEQ_ERROR,      // �G���[����
  BSUBWAY_SEQ_ERROR_WAIT, // �G���[�E�G�C�g
} ;


//-------------------------------------
///	�T�u�V�[�P���X
//=====================================
enum{
  // �L�^�̃A�b�v���[�h
  SCORE_UPLOAD_SEQ_PERSON_SETUP = 0,
  SCORE_UPLOAD_SEQ_PERSON_SETUP_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_END,

  // �Q�[�����̃_�E�����[�h
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_RANK = 0,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_RANK_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END,

  // �����̃_�E�����[�h
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK = 0,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END,
};


//-------------------------------------
///	NHTTP�̃G���[����
//=====================================
typedef enum{
  BSUBWAY_NHTTP_ERROR_NONE,
  BSUBWAY_NHTTP_ERROR_POKE_ERROR,   // �|�P�������s��
  BSUBWAY_NHTTP_ERROR_DISCONNECTED, // ����I�����Ȃ�����

  BSUBWAY_NHTTP_ERROR_MAX,
} BSUBWAY_NHTTP_ERROR;


///�����^�C���A�E�g�܂ł̎���
#define BSUBWAY_TIMEOUT_TIME			(30*60*2)	//2��

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	PersonalData
//=====================================
typedef struct 
{
  BOOL setup;
	Dpw_Common_Profile			  dc_profile;					// �����̏��o�^�p�\����
	Dpw_Common_ProfileResult	dc_profile_result;	// �����̏��o�^���X�|���X�p�\����
	Dpw_Bt_Player	bt_player;							// ���������������̃f�[�^
  const MYSTATUS* cp_mystatus;


  // �|�P�����F�؁@����
  NHTTP_RAP_WORK* p_nhttp;  // �|�P�����F�؃��[�N
  u16 poke_num;
  u8 sign[NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN];

} WIFI_BSUBWAY_PERSONAL;


//-------------------------------------
///	RoomData
//=====================================
typedef struct 
{
  BOOL setup;
  BOOL in_roomdata;
  
  u16 rank;
  u16 room_no;
  u16 room_num;

	Dpw_Bt_Room 	bt_roomdata;						// �_�E�����[�h�������


} WIFI_BSUBWAY_ROOM;

//-------------------------------------
///	dpw_bt Error
//=====================================
typedef struct {
  DpwBtError          error_code;
  DpwBtServerStatus   server_status;

  u16 timeout_flag;
  s16 timeout;

  BSUBWAY_NHTTP_ERROR nhttp_error;
} WIFI_BSUBWAY_ERROR;



//-------------------------------------
///	�o�g���T�u�E�F�C�@WiFi�������[�N
//=====================================
typedef struct 
{
  // �p�����[�^
  WIFI_BSUBWAY_PARAM* p_param;

  // Login Logout
  WIFILOGIN_PARAM   login;
  WIFILOGOUT_PARAM  logout;

  // �F�ؗp���[�N
  DWCSvlResult svl_result;

  // DpwMain���܂킷�t���O
  BOOL dpw_main_do;

  // �T�u�V�[�P���X
  s32 seq;

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
	SYSTEMDATA*   p_systemdata;		// �V�X�e���Z�[�u�f�[�^�iDPW���C�u�����p�Ƃ������R�[�h��ۑ��j
	WIFI_LIST*    p_wifilist;			// �Ƃ������蒠
	MYSTATUS*     p_mystatus;			// �g���[�i�[���
  
} WIFI_BSUBWAY;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	process
//=====================================
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcEnd( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );


//-------------------------------------
///	RoomData 
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
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk );

//-------------------------------------
///	PersonalData
//=====================================
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk );
static void PERSONAL_DATA_Exit( WIFI_BSUBWAY_PERSONAL* p_wk );
// �l���F��
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error );
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// �C�V���F��
static void PERSONAL_DATA_SetUpNhttp( WIFI_BSUBWAY_PERSONAL* p_wk, DWCSvlResult* p_svl_result, WIFI_BSUBWAY_ERROR* p_error, HEAPID heapID );
static BOOL PERSONAL_DATA_SetUpNhttpWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// �|�P�����F��
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, HEAPID heapID );
static BOOL PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// �l���A�b�v�f�[�g
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );

//-------------------------------------
///	ErrorData
//=====================================
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk );
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result );
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk );
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk );
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error );
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_OnlyTimeOutCount( WIFI_BSUBWAY_ERROR* p_wk );


//-------------------------------------
///	CodeIn
//=====================================
static void CODEIN_StartProc( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk );
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str );

//-------------------------------------
///	Login
//=====================================
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk );
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk );

//-------------------------------------
///	Logout
//=====================================
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk );

//-------------------------------------
///	���C����������
//=====================================
static GFL_PROC_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static GFL_PROC_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static GFL_PROC_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static GFL_PROC_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );


//-------------------------------------
///	�ڑ��E�ؒf�E�G���[
//=====================================
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk );
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk );

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
 *	@brief  �o�g���T�u�E�F�C  process ������
 *
 *	@param	p_proc      process���[�N
 *	@param	p_seq       �V�[�P���X
 *	@param	p_param     �p�����[�^
 *	@param	p_work      ���[�N
 *
 *	@retval GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval GFL_PROC_RES_FINISH				    ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI_BSUBWAY, 0x18000 );

  p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WIFI_BSUBWAY), HEAPID_WIFI_BSUBWAY );

  // Wi-FiLogin�ς݂��`�F�b�N
  GF_ASSERT( GFL_NET_IsInit() );


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
  }

  // �V�X�e�����[�N������
  ROOM_DATA_Init( &p_wk->roomdata );
  PERSONAL_DATA_Init( &p_wk->personaldata );
  ERROR_DATA_Init( &p_wk->bt_error );
  
  // DEBUG�o��ON
  GFL_NET_DebugPrintOn();

  return GFL_PROC_RES_FINISH;  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���T�u�E�F�C  process ���C��
 *
 *	@param	p_proc      process���[�N
 *	@param	p_seq       �V�[�P���X
 *	@param	p_param     �p�����[�^
 *	@param	p_work      ���[�N
 *
 *	@retval GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval GFL_PROC_RES_FINISH				    ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk = p_work;

  // �ʐM�G���[�`�F�b�N
  if( ((*p_seq) <= BSUBWAY_SEQ_MAIN) && (GFL_FADE_CheckFade() == FALSE) && (p_wk->dpw_main_do) ){
    if( ERROR_DATA_IsError( &p_wk->bt_error ) || (NetErr_App_CheckError() != NET_ERR_CHECK_NONE) )
    {
      (*p_seq) = BSUBWAY_SEQ_ERROR;
    }
  }
  
  // ���C������
  switch( (*p_seq) ){
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
        (*p_seq) = BSUBWAY_SEQ_ALL_END;
      }
    }
    break;

  case BSUBWAY_SEQ_FADEIN:       // �t�F�[�h�C��
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 1);
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
    if( WiFiBsubway_Main( p_wk, HEAPID_WIFI_BSUBWAY ) == GFL_PROC_RES_FINISH ){
      p_wk->p_param->result = WIFI_BSUBWAY_RESULT_OK; //���������I
      (*p_seq) ++;
    }
    break;

  // �I��
  case BSUBWAY_SEQ_END:        
    WiFiBsubway_Disconnect( p_wk );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1);
    (*p_seq) ++;
    break;

  // �I���E�G�C�g
  case BSUBWAY_SEQ_END_WAIT:   
    if( WiFiBsubway_DisconnectWait( p_wk ) && (GFL_FADE_CheckFade() == FALSE) )
    {
      (*p_seq) ++;
    }
    break;

  // Logout����
  case BSUBWAY_SEQ_LOGOUT:     
    LOGOUT_StartProc( p_wk );
    (*p_seq)++;
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
    if( WiFiBsubway_Error( p_wk ) ){
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


  return GFL_PROC_RES_CONTINUE;  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���T�u�E�F�C  process �j��
 *
 *	@param	p_proc      process���[�N
 *	@param	p_seq       �V�[�P���X
 *	@param	p_param     �p�����[�^
 *	@param	p_work      ���[�N
 *
 *	@retval GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval GFL_PROC_RES_FINISH				    ///<����I��
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
  
  // proc���[�N�j��
  GFL_PROC_FreeWork( p_proc );

  // HEAPID�j��
  GFL_HEAP_DeleteHeap( HEAPID_WIFI_BSUBWAY );

  return GFL_PROC_RES_FINISH;
}


//-------------------------------------
///	RoomData 
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief�@RoomData������
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_Init( WIFI_BSUBWAY_ROOM* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ROOM) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData  Rank�̕��������Z�b�g�A�b�v
 *
 *	@param	p_wk    ���[�N
 *	@param	rank    �����N
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SetUp( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 rank )
{
  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  p_wk->rank = rank;

  // ���[�������擾
  Dpw_Bt_GetRoomNumAsync( rank );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData Rank�̕������Z�b�g�A�b�v�҂�
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
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
    if( ERROR_DATA_IsError( p_error ) ){
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
 *	@brief  RoomData �������̎擾���� 
 *
 *	@param	p_wk      ���[�N
 *	@param	room_no   �����i���o�[
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_LoadRoomData( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 room_no )
{
  GF_ASSERT( p_wk->setup );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  p_wk->room_no = room_no;
  GF_ASSERT( p_wk->room_no < p_wk->room_num );
	Dpw_Bt_DownloadRoomAsync( p_wk->rank, p_wk->room_no, &p_wk->bt_roomdata );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData �������@�擾�E�G�C�g
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL ROOM_DATA_LoadRoomDataWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  // ��M�����҂�
	if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( ERROR_DATA_IsError( p_error ) ){
      // ����
      p_wk->in_roomdata = TRUE;
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData  rank�̎擾
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRank( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->rank;
}
//----------------------------------------------------------------------------
/**
 *	@brief  RoomData roomNo�̎擾
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomNo( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->room_no;
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData roomNum�̎擾
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
 *	@brief  RoomData �������̎擾
 */
//-----------------------------------------------------------------------------
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  GF_ASSERT( cp_wk->in_roomdata );
  return &cp_wk->bt_roomdata;
}




//-------------------------------------
///	PersonalData
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  PersonalData  ������
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_PERSONAL) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PersonalData  �j��
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
 *	@brief  PersonalData  �v���t�B�[���Z�b�g�A�b�v
 *
 *	@param	p_wk        ���[�N
 *	@param	p_gamedata  �Q�[��Data
 *	@param	p_error     �G���[���[�N
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error )
{
  SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
  MYSTATUS* p_mystatus          = GAMEDATA_GetMyStatus( p_gamedata );
  WIFI_HISTORY* p_history       = SaveData_GetWifiHistory( p_savedata );
  WIFI_LIST* p_wifilist         = GAMEDATA_GetWiFiList( p_gamedata );

  GF_ASSERT( p_wk->setup == FALSE );
  
  // DC_PROFILE�쐬
  DPW_RAP_CreateProfile( &p_wk->dc_profile, p_mystatus );
  
  // �F��
	Dpw_Bt_SetProfileAsync( &p_wk->dc_profile, &p_wk->dc_profile_result );
  ERROR_DATA_GetAsyncStart( p_error );

  
  // ���[�U�[�̃Q�[�����𐶐�
  // @TODO
  // p_wk->bt_player

  // Mystatus�ۑ�
  p_wk->cp_mystatus = p_mystatus;
}

//----------------------------------------------------------------------------
/**
 *	@brief  PersonalData  �v���t�B�[���Z�b�g�A�b�v�����҂�
 *
 *	@param	p_wk      ���[�N
 *	@param	p_error   �G���[���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup == FALSE );

  // ��M�����҂�
	if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( ERROR_DATA_IsError( p_error ) ){
      // ����
      p_wk->setup = TRUE;
      return TRUE;
    }
  }
  return FALSE;
}


// �C�V���F��
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����F�؂̂��߂̐l���F��
 *
 *	@param	p_wk      ���[�N
 *	@param  p_svl_result
 *	@param  p_error
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpNhttp( WIFI_BSUBWAY_PERSONAL* p_wk, DWCSvlResult* p_svl_result, WIFI_BSUBWAY_ERROR* p_error, HEAPID heapID )
{
  BOOL result;
  GF_ASSERT( p_wk->setup ); 

  p_wk->p_nhttp = NHTTP_RAP_Init( heapID,MyStatus_GetProfileID( p_wk->cp_mystatus ), p_svl_result );

  result = NHTTP_RAP_SvlGetTokenStart( p_wk->p_nhttp );
  GF_ASSERT( result );

  // �^�C���A�E�g�`�F�b�N�J�n
  ERROR_DATA_StartOnlyTimeOut( p_error );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����F�؂̂��߂̐l���F�؁@�����҂�
 *
 *	@param	p_wk  ���[�N
 *	@param  p_error
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpNhttpWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  GF_ASSERT( p_wk->p_nhttp );

  if( NHTTP_RAP_SvlGetTokenMain( p_wk->p_nhttp ) )
  { 
    return TRUE;
  }

  // �^�C���A�E�g�J�E���g
  ERROR_DATA_OnlyTimeOutCount( p_error );
  return FALSE;
}

// �|�P�����F��
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����F�؁@
 *
 *	@param	p_wk      ���[�N
 *	@param  p_error
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, HEAPID heapID )
{
  BOOL result;
  //POKEMON_PASO_PARAM  *pp = PPPPointerGet( (POKEMON_PARAM*)wk->UploadPokemonData.postData.data );
 
  
  GF_ASSERT( p_wk->p_nhttp );

  NHTTP_RAP_PokemonEvilCheckCreate( p_wk->p_nhttp, heapID, POKETOOL_GetPPPWorkSize(), NHTTP_POKECHK_SUBWAY);

  // @TODO �|�P������ݒ肷��
  //NHTTP_RAP_PokemonEvilCheckAdd( p_wk->p_nhttp, pp, POKETOOL_GetPPPWorkSize() );

  // �ݒ�|�P������
  p_wk->poke_num = 1;

  result = NHTTP_RAP_PokemonEvilCheckConectionCreate( p_wk->p_nhttp );
  GF_ASSERT( result );

  result = NHTTP_RAP_StartConnect( p_wk->p_nhttp );  
  GF_ASSERT( result );

  // �^�C���A�E�g�`�F�b�N�J�n
  ERROR_DATA_StartOnlyTimeOut( p_error );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����F�؁@�����҂�
 *
 *	@param	p_wk      ���[�N
 *	@param  p_error
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  NHTTPError error;

  GF_ASSERT( p_wk->p_nhttp );
  
  error = NHTTP_RAP_Process( p_wk->p_nhttp );
  if( NHTTP_ERROR_BUSY != error )
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
        const s8 *cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_data, 1 );
        GFL_STD_MemCopy( cp_sign, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
      }

      for( i=0; i<p_wk->poke_num; i++ ){
        poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
        if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
          ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_POKE_ERROR );
          break;
        }
      }
    }
    else
    {
      // �F�؊m�F���s
      ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_DISCONNECTED );
    }

    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;


    return TRUE;
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Personal Data �A�b�v�f�[�g
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  GF_ASSERT( p_wk->setup );

  {
    s32 rank, room_no;
    s32 win;
    rank = 0;
    room_no = 0;
    win = 0;
    // @TODO �����ƃv���C�����A�b�v���邱��
  	Dpw_Bt_UploadPlayerAsync(  rank,  room_no,  win, &p_wk->bt_player, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
    ERROR_DATA_GetAsyncStart( p_error );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Personal Data �A�b�v�f�[�g�����҂�
 *
 *	@param	p_wk      ���[�N
 *	@param	p_error   �G���[���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  s32 result;

  // ��M�����҂�
	if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // �G���[�I���łȂ���Ί���
    if( ERROR_DATA_IsError( p_error ) ){
      return TRUE;
    }
  }
  return FALSE;
}







//-------------------------------------
///	ErrorData
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ErrorData������
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ERROR) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Erro���m�@�\���@����M�����J�n
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Error���m�@�\���@����M����
 *
 *	@param  p_wk        ���[�N
 *	@param  p_result    ���ʊi�[��
 *
 *	@retval TRUE    ��M����
 *	@retval FALSE   ��M��
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result )
{
  s32 result = 0;
  BOOL complete = FALSE;
  
	if( !Dpw_Tr_IsAsyncEnd() ){
    // �^�C���A�E�g�`�F�b�N
    p_wk->timeout ++;
    if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
      // �ߋ���
			// CommFatalErrorFunc_NoNumber();	//�����ӂ��Ƃ΂�
      // @TODO �ӂ��Ƃ΂��@SERVER�^�C���A�E�g�Ɠ��l�̏������s���B
      // �L�����Z���R�[��
      Dpw_Bt_CancelAsync();

      // �^�C���A�E�g��
      p_wk->timeout_flag = TRUE;
    }
  }else{
		result    = Dpw_Tr_GetAsyncResult();
    complete  = TRUE;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
			TOMOYA_Printf(" server ok\n");
      *p_result = result;
			break;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
		case DPW_TR_STATUS_SERVER_FULL:			// �T�[�o�[�����t
		case DPW_TR_ERROR_SERVER_FULL:
		case DPW_TR_ERROR_CANCEL:
		case DPW_TR_ERROR_FAILURE:
		case DPW_TR_ERROR_SERVER_TIMEOUT:
		case DPW_TR_ERROR_DISCONNECTED:	
			TOMOYA_Printf(" server ng\n");
      p_wk->error_code = result;
      break;

		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
		default:
      NetErr_ErrorSet();
			break;
		}
  }

  return complete;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Dpw Bt�̃G���[��ԃ`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    �G���[���
 *	@retval FALSE   �G���[��Ԃł͂Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  // BT�G���[
  if( cp_wk->error_code < 0 ){
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
 *	@brief  ErrorData Dpw BT�̃G���[���b�Z�[�W�擾
 *
 *	@param	cp_wk   ���[�N
 *    
 *	@return �G���[���b�Z�[�WID
 */
//-----------------------------------------------------------------------------
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk )
{
	int msgno =0;

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
      msgno = msg_wifi_bt_error_005;
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
 *	@brief  NhttpError  �ݒ�
 *
 *	@param	p_wk    ���[�N
 *	@param	error   �G���[�^�C�v
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error )
{
  p_wk->nhttp_error = error;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �^�C���A�E�g���J�n
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �^�C���A�E�g�J�E���g
 *
 *	@param	p_wk  ���[�N
 *
 *	�^�C���A�E�g�ɂȂ����玩���ŃG���[�����֍s���܂��B
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
///	CodeIn
//=====================================
//----------------------------------------------------------------------------
/**
 *	@briefCodeIn�J�n
 *
 *	@param	p_wk      ���[�N
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void CODEIN_StartProc( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  static int s_CODEIN_BLOCK[CODE_BLOCK_MAX] = {
    2,
    0,
    0,
  };

  p_wk->p_codein = CodeInput_ParamCreate( heapID, 2, s_CODEIN_BLOCK );
  
  // �v���b�N�R�[��
  GFL_PROC_SysCallProc( FS_OVERLAY_ID(codein), &CodeInput_ProcData, p_wk->p_codein );
}

//----------------------------------------------------------------------------
/**
 *	@brief  CodeIn�I��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk )
{
  if( p_wk->p_codein )
  {
    CodeInput_ParamDelete( p_wk->p_codein );
    p_wk->p_codein = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  CodeIn�@���͕�����擾
 *
 *	@param	cp_wk   ���[�N
 *	@param  p_str �@�i�[�o�b�t�@
 */
//-----------------------------------------------------------------------------
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str )
{
  GF_ASSERT( cp_wk->p_codein );

  GFL_STR_CopyBuffer( p_str, cp_wk->p_codein->strbuf );
}




//-------------------------------------
///	Login
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  Login���� �J�n
 *
 *	@param	p_wk      ���[�N  
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->login.gamedata  = p_wk->p_gamedata;
  p_wk->login.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->login.display   = WIFILOGIN_DISPLAY_UP;
  p_wk->login.nsid      = WB_NET_BSUBWAY;
  p_wk->login.pSvl      = &p_wk->svl_result;


  // �v���b�N�R�[��
  GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &p_wk->login );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Login���� ��n��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  Login���� ����
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval WIFILOGIN_RESULT_LOGIN, //���O�C������
 *	@retval WIFILOGIN_RESULT_CANCEL,//�L�����Z������
 */
//-----------------------------------------------------------------------------
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk )
{
  return cp_wk->login.result;
}



//-------------------------------------
///	Logout
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  Logout�����@�J�n
 *
 *	@param	p_wk      ���[�N
 *	@param	heapID    �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->logout.gamedata  = p_wk->p_gamedata;
  p_wk->logout.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->logout.display   = WIFILOGIN_DISPLAY_UP;

  // �v���b�N�R�[��
  GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogout_ProcData, &p_wk->logout );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Logout����  �I��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk )
{
}




//-------------------------------------
///	���C����������
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���T�u�E�F�C�@���C������
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval GFL_PROC_RES_FINISH				    ///<����I��
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  GFL_PROC_RESULT (*pFunc[ WIFI_BSUBWAY_MODE_MAX ])( WIFI_BSUBWAY* p_wk, HEAPID heapID ) = 
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
 *	@brief  �O����̃A�b�v���[�h
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_PERSON_SETUP:
    PERSONAL_DATA_SetUpNhttp( &p_wk->personaldata, &p_wk->svl_result, &p_wk->bt_error, heapID );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_SETUP_WAIT:
    if( PERSONAL_DATA_SetUpNhttpWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      p_wk->seq++;
    }
    break;
  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP:
    PERSONAL_DATA_SetUpNhttpPokemon( &p_wk->personaldata, &p_wk->bt_error, heapID );
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
      p_wk->seq++;
    }
    break;
  case SCORE_UPLOAD_SEQ_PERSON_END:
    return GFL_PROC_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�[�����̃_�E�����[�h
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_RANK:
    CODEIN_StartProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_RANK_WAIT:
    // ��������擾
    CODEIN_EndProc( p_wk );

    // �����B
    ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, 0 );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO:
    CODEIN_StartProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT:
    // ��������擾
    CODEIN_EndProc( p_wk );

    // �����B
    ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, 0 );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END:
    return GFL_PROC_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃_�E�����[�h
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK:
    CODEIN_StartProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT:
    // ��������擾
    CODEIN_EndProc( p_wk );

    // �����B
    ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, 0 );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO:
    CODEIN_StartProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT:
    // ��������擾
    CODEIN_EndProc( p_wk );

    // �����B
    ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, 0 );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END:
    return GFL_PROC_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}



  
//-------------------------------------
///	�ڑ��E�ؒf�E�G���[
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��J�n
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk )
{
	// ���E�����ڑ�������
	DWCUserData* p_userdata;		// �F�؍ς݂�DWCUSER�f�[�^�������Ȃ��͂�
	s32 profileId;

	// DWCUser�\���̎擾
	p_userdata = WifiList_GetMyUserInfo( p_wk->p_wifilist );

	MyStatus_SetProfileID( p_wk->p_mystatus, WifiList_GetMyGSID( p_wk->p_wifilist ) );

	// �����ȃf�[�^���擾
	profileId = MyStatus_GetProfileID( p_wk->p_mystatus );
	TOMOYA_Printf("Dpw�T�[�o�[���O�C����� profileId=%08x\n", profileId);

	// DPW_BT������
	Dpw_Bt_Init( profileId, DWC_CreateFriendKey( p_userdata ),LIBDPW_SERVER_TYPE );

	TOMOYA_Printf("Dpw Bsubway ������\n");

	Dpw_Bt_GetServerStateAsync();
  ERROR_DATA_GetAsyncStart( &p_wk->bt_error );

	TOMOYA_Printf("Dpw Bsubway �T�[�o�[��Ԏ擾�J�n\n");

  p_wk->dpw_main_do = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��E�G�C�g
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    �ڑ�����
 *	@retval FALSE   �ڑ��r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
  return ERROR_DATA_GetAsyncResult( &p_wk->bt_error, &result );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf�J�n
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk )
{
  if( Dpw_Bt_IsAsyncEnd() == FALSE ){
    // �L�����Z���R�[��
    Dpw_Bt_CancelAsync();
    ERROR_DATA_GetAsyncStart( &p_wk->bt_error );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf�E�G�C�g
 *  
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
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
 *	@brief  �G���[����
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval BOOL  ErrorWait�̗L��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk )
{
  // Net�G���[�`�F�b�N
  if( GFL_NET_DWC_ERROR_ReqErrorDisp() )
  {
    // �I����
    return FALSE;
  }
  // Bt�G���[�`�F�b�N
  if( ERROR_DATA_IsError( &p_wk->bt_error ) )
  {
    //�@�G���[�\������
    // �G���[�\���I����҂�
    return TRUE;
  }

  // Not Error
  GF_ASSERT(0);
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�����@�E�G�C�g
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk )
{
  // �\������
  return TRUE;
}

