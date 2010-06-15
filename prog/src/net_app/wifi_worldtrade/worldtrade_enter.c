//============================================================================================
/**
 * @file	worldtrade_enter.c
 * @brief	���E�������������
 * @author	Akito Mori
 * @date	06.04.16
 */
//============================================================================================
#include <gflib.h>
#include <dwc.h>
//#include "libdpw/dwci_ghttp.h"
#include "ghttp/dwci_ghttp.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dpw_tr.h>
#include "print/wordset.h"
#include "print/printsys.h"
#include "message.naix"
#include "system/wipe.h"
//#include "system/fontproc.h"
//#include "system/fontoam.h"
//#include "system/window.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
//#include "savedata/zukanwork.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "src\net_app\wifi_login\wifilogin_local.h"//�R�[���o�b�N�쐬�̂���


#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "msg/msg_wifi_system.h"

#include "net_app/connect_anm.h"

#include "net/network_define.h"
#include "net/dwc_rap.h"

#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`
#include "wifip2pmatch.naix"			// �O���t�B�b�N�A�[�J�C�u��`
#include "net/dpw_rap.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );


static int Enter_Start( WORLDTRADE_WORK *wk);
static int Enter_End( WORLDTRADE_WORK *wk);

static int Enter_MessageWait1Second( WORLDTRADE_WORK *wk );
static int Enter_Wifilogin_start( WORLDTRADE_WORK *wk );
static int Enter_Wifilogin_wait( WORLDTRADE_WORK *wk );
static int Enter_Wifilogout_start( WORLDTRADE_WORK *wk );
static int Enter_Wifilogout_wait( WORLDTRADE_WORK *wk );

//�R�[���o�b�N
static WIFILOGIN_CALLBACK_RESULT Enter_WifiLogInCallBack( WIFILOGIN_MESSAGE_WORK *p_msg, void *p_callback_wk );
static WIFILOGIN_CALLBACK_RESULT Enter_WifiLogOutCallBack( WIFILOGIN_MESSAGE_WORK *p_msg, void *p_callback_wk );

//�R�[���o�b�N���Ŏg�p����
typedef enum
{ 
  ENTER_RESULT_CONTINUE,
  ENTER_RESULT_END,
  ENTER_RESULT_ERROR,
}ENTER_RESULT;
static BOOL Enter_WifiConnectionLogin( WORLDTRADE_WORK *wk );
static ENTER_RESULT Enter_WifiConnectionLoginWait( WORLDTRADE_WORK *wk );
static BOOL Enter_DpwTrInit( WORLDTRADE_WORK *wk );
static BOOL Enter_ServerStart( WORLDTRADE_WORK *wk );

static ENTER_RESULT Enter_ServerResult( WORLDTRADE_WORK *wk );
static BOOL Enter_ProfileStart( WORLDTRADE_WORK *wk );
static ENTER_RESULT Enter_ProfileResult( WORLDTRADE_WORK *wk );
static BOOL Enter_ServerServiceError( WORLDTRADE_WORK *wk, WIFILOGIN_MESSAGE_WORK *p_msg );
static BOOL Enter_ServerServiceEnd( WORLDTRADE_WORK *wk, WIFILOGIN_MESSAGE_WORK *p_msg );

static int printCommonFunc( GFL_BMPWIN *win, STRBUF *strbuf, int x, int flag, PRINTSYS_LSB color, int font, WT_PRINT *print );

enum{
	ENTER_START=0,
	ENTER_END,

  ENTER_WIFILOGIN_PROC_START,
	ENTER_WIFILOGIN_PROC_WAIT,
  ENTER_WIFILOGOUT_PROC_START,
	ENTER_WIFILOGOUT_PROC_WAIT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	Enter_Start,				// ENTER_START=0,
	Enter_End,             	 	// ENTER_END,

  Enter_Wifilogin_start, //ENTER_WIFILOGIN_PROC_START,
	Enter_Wifilogin_wait, //ENTER_WIFILOGIN_PROC_WAIT,
  Enter_Wifilogout_start, //ENTER_WIFILOGOUT_PROC_START,
	Enter_Wifilogout_wait, //ENTER_WIFILOGOUT_PROC_WAIT,
};

///E���[���F�؃G���[�����������ۂ̃G���[���b�Z�[�W�R�[�h
enum{
	EMAIL_ERROR_SEND = -5000,
	EMAIL_ERROR_SENDFAILURE = -5001,
	EMAIL_ERROR_SUCCESS = -5002,
	EMAIL_ERROR_FAILURE = -5003,
	EMAIL_ERROR_INVALIDPARAM = -5004,
	EMAIL_ERROR_SERVERSTATE = -5005,
};

///�����^�C���A�E�g�܂ł̎���
#define TIMEOUT_TIME			(30*60*2)	//2��

//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Enter_Init(WORLDTRADE_WORK *wk, int seq)
{

  WorldTrade_ExitSystem( wk );

	// ���[�N������
	InitWork( wk );

	// ���C����ʂ����ɏo�͂����悤�ɂ���
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

	
	// ���C�v�t�F�[�h�J�n
	//WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_enter.c [172] MS ********************\n" );
#endif


    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����ON
//    CommStateWifiDPWStart( wk->param->savedata );
	

  //���[�h�ʋN���֕ύX
  switch( wk->sub_process_mode )
  { 
  case MODE_WIFILOGIN:     ///< WIFI���O�C��
  case MODE_WIFILOGIN_ERR:
    wk->subprocess_seq = ENTER_WIFILOGIN_PROC_START;
    break;

  case MODE_WIFILOGOUT:    ///< WIFI���O�A�E�g
    wk->subprocess_seq = ENTER_WIFILOGOUT_PROC_START;
    break;
  }
	return SEQ_FADEIN;
}

//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Enter_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret, temp_subprocess_seq;

	// �V�[�P���X�J�ڂŎ��s
	temp_subprocess_seq = wk->subprocess_seq;
	ret = (*Functable[wk->subprocess_seq])( wk );
	if(temp_subprocess_seq != wk->subprocess_seq){
		wk->local_seq = 0;
		wk->local_wait = 0;
	}

	return ret;
}


//==============================================================================
/**
 * @brief   ���E�����������ʏI��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Enter_End(WORLDTRADE_WORK *wk, int seq)
{
	FreeWork( wk );

  WorldTrade_InitSystem( wk );

	// ���̃T�u�v���Z�X��ݒ肷��
	WorldTrade_SubProcessUpdate( wk );
	
	// ���̃T�u�v���Z�X��������̂܂܂������琢�E�����I��
	if(wk->sub_process==WORLDTRADE_ENTER){
		OS_TPrintf("���E�����I��\n");
		return SEQ_OUT;
	}
	
	OS_TPrintf("WorldTrade_Enter end\n");
	// �����ݒ肳��Ă����珉������
	return SEQ_INIT;
}

//------------------------------------------------------------------
/**
 * ���E�������[�N������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk )
{
}


//------------------------------------------------------------------
/**
 * @brief   ���[�N���
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void FreeWork( WORLDTRADE_WORK *wk )
{
}




//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�X�^�[�g����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_Start( WORLDTRADE_WORK *wk)
{
  PMSND_PlayBGM( WORLDTRADE_BGM );


  wk->subprocess_seq  = ENTER_END;
	wk->boxSearchFlag = 1;
  wk->OpeningFlag   = 0;

  WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   GameSpy�T�[�o�[���O�C���J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  TRUE�ŏI��
 */
//------------------------------------------------------------------
static BOOL Enter_WifiConnectionLogin( WORLDTRADE_WORK *wk )
{
	DWC_NASLoginAsync();
	OS_Printf("GameSpy�T�[�o�[���O�C���J�n\n");

	return TRUE;
}
//------------------------------------------------------------------
/**
 * @brief   GameSpy�T�[�o�[���O�C�������҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  TRUE�ŏI��
 */
//------------------------------------------------------------------
static ENTER_RESULT Enter_WifiConnectionLoginWait( WORLDTRADE_WORK *wk )
{
	switch(DWC_NASLoginProcess()){
	case DWC_NASLOGIN_STATE_SUCCESS:
		OS_Printf("GameSpy�T�[�o�[���O�C������\n");
    return ENTER_RESULT_END;
		break;
	case DWC_NASLOGIN_STATE_ERROR:
	case DWC_NASLOGIN_STATE_CANCELED:
	case DWC_NASLOGIN_STATE_DIRTY:
    return ENTER_RESULT_ERROR;
	}
	
	
	return ENTER_RESULT_CONTINUE;
}



//------------------------------------------------------------------
/**
 * @brief   ���E�����T�[�o�[�ڑ����C�u����������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  TRUE�ŏI��
 */
//------------------------------------------------------------------
static BOOL Enter_DpwTrInit( WORLDTRADE_WORK *wk )
{
	// ���E�����ڑ�������
	DWCUserData		*MyUserData;		// �F�؍ς݂�DWCUSER�f�[�^�������Ȃ��͂�
	s32 profileId;

	// DWCUser�\���̎擾
	MyUserData = WifiList_GetMyUserInfo(wk->param->wifilist);

	MyStatus_SetProfileID( wk->param->mystatus, WifiList_GetMyGSID(wk->param->wifilist) );

	// �����ȃf�[�^���擾
	profileId = MyStatus_GetProfileID( wk->param->mystatus );
	OS_Printf("Dpw�T�[�o�[���O�C����� profileId=%08x\n", profileId);

	// DPW_TR������
	Dpw_Tr_Init( profileId, DWC_CreateFriendKey( MyUserData ),LIBDPW_SERVER_TYPE );
	OS_TPrintf("Dpw_Tr_Init ������\n");

	
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief   ���E�������C�u�����ڑ��󋵎擾�J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  TRUE�ŏI��
 */
//------------------------------------------------------------------
static BOOL Enter_ServerStart( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_GetServerStateAsync();

	OS_TPrintf("Dpw Trade �T�[�o�[��Ԏ擾�J�n\n");

	// �T�[�o�[��Ԋm�F�҂���
	wk->timeout_count = 0;
	
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief   �T�[�o�[��Ԋm�F�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  TRUE�ŏI��
 */
//------------------------------------------------------------------
static ENTER_RESULT Enter_ServerResult( WORLDTRADE_WORK *wk )
{
  Dpw_Tr_Main();

	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
			OS_TPrintf(" server is up!\n");
			return ENTER_RESULT_END;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
			OS_TPrintf(" server stop service.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;
		case DPW_TR_STATUS_SERVER_FULL:			// �T�[�o�[�����t
		case DPW_TR_ERROR_SERVER_FULL:
			OS_TPrintf(" server full.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// �uGTS�̂����ɂ�ɂ����ς����܂����v���^�C�g����
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// �T�[�o�[�ƒʐM�ł��܂��񁨏I��
			OS_TPrintf(" upload error. %d \n", result);
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;
		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
		default:
			// ���ӂ��Ƃ΂�
			WorldTrade_TimeIconDel(wk);
      WorldTrade_DispCallFatal();
			break;

		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal();
		}
	}
	return ENTER_RESULT_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   ���E�������C�u�����F�v���t�B�[��(E���[��)�X�V�J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static BOOL Enter_ProfileStart( WORLDTRADE_WORK *wk )
{
  DPW_RAP_CreateProfile( &wk->dc_profile, wk->param->mystatus );
	Dpw_Tr_SetProfileAsync(&wk->dc_profile, &wk->dc_profile_result);

	OS_TPrintf("Dpw Trade �v���t�B�[��(E���[��)���M\n");

	// �T�[�o�[��Ԋm�F�҂���
	wk->timeout_count = 0;
	
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief   E���[���X�V�����Ԏ��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static ENTER_RESULT Enter_ProfileResult( WORLDTRADE_WORK *wk )
{
  Dpw_Tr_Main();

	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
			OS_TPrintf(" profile is up!\n");

//			WorldTrade_TimeIconDel(wk);
			switch(wk->dc_profile_result.code){
			case DPW_PROFILE_RESULTCODE_SUCCESS:	//���̓o�^�ɐ���
				OS_TPrintf("mailAddrAuthResult = %d\n", wk->dc_profile_result.mailAddrAuthResult);
				switch(wk->dc_profile_result.mailAddrAuthResult){
				case DPW_PROFILE_AUTHRESULT_SUCCESS:	//�F�ؐ���
					// �|�P�����m�F�T�u�v���Z�X�ֈڍs
					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					return ENTER_RESULT_END;
					break;
				//�ȉ��̃G���[�����͂��̃V�[���ł͑z�肵�Ă��Ȃ����[���F�؂̌��ʂ��Ԃ����ꍇ
				// (�����̗F�B�R�[�h���ω������Ƃ��ɈȑO�Ɠ������[���A�h���X�ƃp�X���[�h��
				// �M�����Ƃ��ɂ��̂悤�ɂȂ�\��������܂��B�ʏ�͂��蓾�܂���B)�}�j���A�����p
				case DPW_PROFILE_AUTHRESULT_FAILURE:	//�F�؂Ɏ��s
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_FAILURE;
          return ENTER_RESULT_ERROR;
					break;
				case DPW_PROFILE_AUTHRESULT_SEND:	//�F�؃��[�����M����
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_SEND;
          return ENTER_RESULT_ERROR;
					break;
				case DPW_PROFILE_AUTHRESULT_SENDFAILURE:	//�F�؃��[���̑��M�Ɏ��s
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_SENDFAILURE;
          return ENTER_RESULT_ERROR;
					break;
				default:	//���肦�Ȃ����ǈꉞ�B�����ӂ��Ƃ΂�
          WorldTrade_DispCallFatal();
					break;
				}
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_INVALIDPARAM:	//�v���t�B�[���̑��M�p�����[�^�s��
				OS_TPrintf(" server stop service.\n");
				wk->ConnectErrorNo = EMAIL_ERROR_INVALIDPARAM;
        return ENTER_RESULT_ERROR;
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_SERVERSTATE:	//�T�[�o�����e�i���Xor�ꎞ��~��
				OS_TPrintf(" server stop service.\n");
				wk->ConnectErrorNo = EMAIL_ERROR_SERVERSTATE;
        return ENTER_RESULT_ERROR;
				break;
			default:
				// ���ӂ��Ƃ΂�
				OS_TPrintf("default error !\n");
				WorldTrade_TimeIconDel(wk);
        WorldTrade_DispCallFatal();
				break;
			}
			break;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
			OS_TPrintf(" server stop service.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;
		case DPW_TR_STATUS_SERVER_FULL:			// �T�[�o�[�����t
		case DPW_TR_ERROR_SERVER_FULL:
			OS_TPrintf(" server full.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// �uGTS�̂����ɂ�ɂ����ς����܂����v���^�C�g����
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// �T�[�o�[�ƒʐM�ł��܂��񁨏I��
			OS_TPrintf(" upload error. %d \n", result);
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
      return ENTER_RESULT_ERROR;
			break;
		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
		default:
			// ���ӂ��Ƃ΂�
			WorldTrade_TimeIconDel(wk);
      WorldTrade_DispCallFatal();
			break;
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal();
		}
	}
  return ENTER_RESULT_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   WIFILOGIN��PROC���Ă�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_Wifilogin_start( WORLDTRADE_WORK *wk )
{ 
  WIFILOGIN_PARAM *param;
  wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(WIFILOGIN_PARAM) );
  GFL_STD_MemClear( wk->sub_proc_wk, sizeof(WIFILOGIN_PARAM) );
  param = wk->sub_proc_wk;
  param->gamedata     = GAMESYSTEM_GetGameData(wk->param->gamesys);
  param->bg           = WIFILOGIN_BG_NORMAL;
  param->display      = WIFILOGIN_DISPLAY_UP;
  param->pSvl         = &wk->svl;
  param->nsid         = WB_NET_WIFIGTS;
  param->login_after_callback = Enter_WifiLogInCallBack;
  param->p_callback_wk  = wk;

  wk->wifi_seq  = 0;
  if( wk->sub_process_mode == MODE_WIFILOGIN_ERR )
  { 
    param->mode         = WIFILOGIN_MODE_ERROR;
  }
  else
  { 
    param->mode         = WIFILOGIN_MODE_NORMAL;
  }

  GFL_PROC_LOCAL_CallProc( wk->local_proc,
		FS_OVERLAY_ID(wifi_login), 
    &WiFiLogin_ProcData, wk->sub_proc_wk );


  //�����G���[�Ȃ�΂����ŃG���[��ʂɂȂ�
  NetErr_DispCall(FALSE); 

  wk->subprocess_seq = ENTER_WIFILOGIN_PROC_WAIT;
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   WIFILOGIN��PROC�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_Wifilogin_wait( WORLDTRADE_WORK *wk )
{ 
  WIFILOGIN_PARAM *param  = wk->sub_proc_wk;
  if( wk->local_proc_status == GFL_PROC_MAIN_NULL )
  { 
    switch( param->result )
    { 
    case WIFILOGIN_RESULT_LOGIN:
      wk->subprocess_seq = ENTER_START;
      break;

    case WIFILOGIN_RESULT_CANCEL:
      //���̃V�[�P���X��ENTER��������I��
      WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
      wk->subprocess_seq  = ENTER_END;
      break;
    }

    GFL_HEAP_FreeMemory(wk->sub_proc_wk);
  }

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   WIFILOGOUT��PROC���Ă�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_Wifilogout_start( WORLDTRADE_WORK *wk )
{ 
  WIFILOGOUT_PARAM *param;
  wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(WIFILOGOUT_PARAM) );
  GFL_STD_MemClear( wk->sub_proc_wk, sizeof(WIFILOGOUT_PARAM) );
  param = wk->sub_proc_wk;
  param->gamedata     = GAMESYSTEM_GetGameData(wk->param->gamesys);
  param->bg           = WIFILOGIN_BG_NORMAL;
  param->display      = WIFILOGIN_DISPLAY_UP;
  param->logout_before_callback = Enter_WifiLogOutCallBack;
  param->p_callback_wk  = wk;

  GFL_PROC_LOCAL_CallProc( wk->local_proc,
      FS_OVERLAY_ID(wifi_login), 
      &WiFiLogout_ProcData, wk->sub_proc_wk );

  wk->subprocess_seq = ENTER_WIFILOGOUT_PROC_WAIT;

  //�����G���[�Ȃ�΂����ŃG���[��ʂɂȂ�
  NetErr_DispCall(FALSE); 

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   WIFILOGOUT��PROC�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_Wifilogout_wait( WORLDTRADE_WORK *wk )
{ 
  if( wk->local_proc_status == GFL_PROC_MAIN_NULL )
  { 
    GFL_HEAP_FreeMemory(wk->sub_proc_wk);

    //���̃V�[�P���X��ENTER��������I��
    WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
    wk->subprocess_seq  = ENTER_END;
  }

	return SEQ_MAIN;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���O�C�����̃R�[���o�b�N
 *
 *	@param	WIFILOGIN_MESSAGE_WORK *p_msg   ���b�Z�[�W�`��
 *	@param	*p_callback_wk                  ���[�U�[���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static WIFILOGIN_CALLBACK_RESULT Enter_WifiLogInCallBack( WIFILOGIN_MESSAGE_WORK *p_msg, void *p_callback_wk )
{ 
  enum
  { 
    SEQ_LOGIN_START,
    SEQ_LOGIN_WAIT,
    SEQ_DPW_INIT,
    SEQ_SERVER_START,
    SEQ_SERVER_RESULT,
    SEQ_PROFILE_START,
    SEQ_PROFILE_RESULT,

    SEQ_ERROR_START,
    SEQ_ERROR_WAIT,
    SEQ_ERROR_END,
  };

  WORLDTRADE_WORK *wk = p_callback_wk;

  switch( wk->wifi_seq )
  { 
  case SEQ_LOGIN_START:
    if( Enter_WifiConnectionLogin( wk ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_LOGIN_WAIT:
    switch( Enter_WifiConnectionLoginWait( wk ) )
    { 
    case ENTER_RESULT_END:
      wk->wifi_seq++;
      break;
    case ENTER_RESULT_ERROR:
      wk->wifi_seq  = SEQ_ERROR_END;
      break;
    }
    break;
  case SEQ_DPW_INIT:
    if( Enter_DpwTrInit( wk ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_SERVER_START:
    if( Enter_ServerStart( wk ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_SERVER_RESULT:
    switch( Enter_ServerResult( wk ) )
    { 
    case ENTER_RESULT_END:
      wk->wifi_seq++;
      break;
    case ENTER_RESULT_ERROR:
      wk->wifi_seq  = SEQ_ERROR_START;
      break;
    }
    break;
  case SEQ_PROFILE_START:
    if( Enter_ProfileStart( wk ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_PROFILE_RESULT:
    switch( Enter_ProfileResult( wk ) )
    { 
    case ENTER_RESULT_END:
      wk->wifi_seq  = 0;
      return WIFILOGIN_CALLBACK_RESULT_SUCCESS;
      break;
    case ENTER_RESULT_ERROR:
      wk->wifi_seq  = SEQ_ERROR_START;
      break;
    }
    break;

  case SEQ_ERROR_START:
    if( Enter_ServerServiceError( wk, p_msg ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_ERROR_WAIT:
    if( Enter_ServerServiceEnd( wk, p_msg ) )
    { 
      wk->wifi_seq++;
    }
    break;
  case SEQ_ERROR_END:
    wk->wifi_seq  = 0;
    return WIFILOGIN_CALLBACK_RESULT_FAILED;
  }

  return WIFILOGIN_CALLBACK_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���O�A�E�g���̃R�[���o�b�N
 *
 *	@param	WIFILOGIN_MESSAGE_WORK *p_msg   ���b�Z�[�W�`��
 *	@param	*p_callback_wk                  ���[�U�[���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static WIFILOGIN_CALLBACK_RESULT Enter_WifiLogOutCallBack( WIFILOGIN_MESSAGE_WORK *p_msg, void *p_callback_wk )
{
  WORLDTRADE_WORK *wk = p_callback_wk;

  OS_TPrintf( "Dpw_Tr���I�����܂���\n" );
  Dpw_Tr_End();
  return WIFILOGIN_CALLBACK_RESULT_SUCCESS;
}


//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�I������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_End( WORLDTRADE_WORK *wk)
{
	// ���ԃA�C�R�������Q�d����ɂȂ�Ȃ��悤��NULL�`�F�b�N����
	WorldTrade_TimeIconDel( wk );

#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_enter.c [1037] MS ********************\n" );
#endif
	wk->subprocess_seq = 0;
	wk->sub_out_flg = 1;
	
	return SEQ_FADEOUT;
}

//==============================================================================
/**
 * @brief   �l�b�g�ɂ͌q���������ǃT�[�o�[�G���[�������\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//==============================================================================
static BOOL Enter_ServerServiceError( WORLDTRADE_WORK *wk, WIFILOGIN_MESSAGE_WORK *p_msg )
{
	int msgno =0;

	switch(wk->ConnectErrorNo){
	case DPW_TR_STATUS_SERVER_STOP_SERVICE:
		msgno = msg_gtc_error_001;
		break;
	case DPW_TR_STATUS_SERVER_FULL:
	case DPW_TR_ERROR_SERVER_FULL:
		msgno = msg_gtc_error_002;
		break;
	case DPW_TR_ERROR_SERVER_TIMEOUT:
	case DPW_TR_ERROR_DISCONNECTED:
		// �f�s�r�Ƃ̂�����������܂����B�������ɂ��ǂ�܂�
		msgno = msg_gtc_error_006;
		break;
	case DPW_TR_ERROR_CANCEL  :
	case DPW_TR_ERROR_FAILURE :
	case DPW_TR_ERROR_NO_DATA:
	case DPW_TR_ERROR_ILLIGAL_REQUEST :
	default:
		//�@������G���[���������܂����B
		msgno = msg_gtc_error_005;
		break;
	case EMAIL_ERROR_FAILURE:
		msgno = msg_gtc_email_error_004;
		break;
	case EMAIL_ERROR_SEND:
		msgno = msg_gtc_email_error_001;
		break;
	case EMAIL_ERROR_SENDFAILURE:
		msgno = msg_gtc_email_error_001;
		break;
	case EMAIL_ERROR_INVALIDPARAM:
		msgno = msg_gtc_email_error_002;
		break;
	case EMAIL_ERROR_SERVERSTATE:
		msgno = msg_gtc_email_error_003;
		break;
	}
	// �G���[�\��
  
  WIFILOGIN_MESSAGE_InfoMessageDispEx(p_msg, wk->MsgManager, msgno);

	OS_TPrintf("Error����\n");
  wk->local_seq = 0;

	return TRUE;
}

//==============================================================================
/**
 * @brief   �T�[�o�[�T�[�r�X�̖��ŏI��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//==============================================================================
static BOOL Enter_ServerServiceEnd( WORLDTRADE_WORK *wk, WIFILOGIN_MESSAGE_WORK *p_msg )
{
	switch(wk->local_seq){
	case 0:
		//Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_cleanup_000, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
    if( WIFILOGIN_MESSAGE_InfoMessageEndCheck(p_msg)){
      // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����OFF
      //CommStateWifiDPWEnd();
      DWC_CleanupInet();
      wk->local_seq++;
    }
		break;
	case 2:
		//Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_cleanup_001, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 3:
	//	if(GF_MSG_PrintEndCheck( &wk->print )==0){

    OS_TPrintf( "Dpw_Tr���I�����܂���\n" );
    Dpw_Tr_End();
			wk->local_seq++;
	//	}
		break;
	default:
		wk->local_wait++;
		if(wk->local_wait > WAIT_ONE_SECONDE_NUM){
      return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
void Enter_MessagePrint( WORLDTRADE_WORK *wk, GFL_MSGDATA *msgman, int msgno, int wait, u16 dat )
{
	// ������擾
	STRBUF *tempbuf;

	// ������擾
	tempbuf = GFL_MSG_CreateString(  msgman, msgno );

	// WORDSET�W�J
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );

	GFL_STR_DeleteBuffer(tempbuf);

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f );
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, &wk->print);

	wk->wait = 0;
}






//----------------------------------------------------------------------------------
/**
 * @brief	�`��ʒu�擾
 *
 * @param   win		BMPWIN
 * @param   strbuf	������
 * @param   x		�����I�ɉE�ɂ��炷�h�b�g��
 * @param   flag	1���ƃZ���^�����O�A�Q���ƉE�悹
 * @param   color	�J���[�w��
 * @param   font	�t�H���g�w��iFONT_TALK��FONT_SYSTEM
 *
 * @retval  int		������`��J�n�ʒu
 */
//----------------------------------------------------------------------------------
static int printCommonFunc( GFL_BMPWIN *win, STRBUF *strbuf, int x, int flag, PRINTSYS_LSB color, int font, WT_PRINT *print )
{
	int length=0,ground;
	switch(flag){
	// �Z���^�����O
	case 1:
		length = FontProc_GetPrintStrWidth( print, font, strbuf, 0 );
		
		x          = ((GFL_BMPWIN_GetSizeX(win)*8)-length)/2;
		break;

	// �E��
	case 2:
		length = FontProc_GetPrintStrWidth( print, font, strbuf, 0 );
		x          = (GFL_BMPWIN_GetSizeX(win)*8)-length;
		break;
	}
	return x;
}


//------------------------------------------------------------------
/**
 * @brief   BMPWIN���̕\���ʒu���w�肵��FONT_TALK�Ńv�����g(�`��̂݁j
 *
 * @param   win		GFL_BMPWIN
 * @param   strbuf	
 * @param   x		X���W���炷�l
 * @param   y		Y���W���炷�l
 * @param   flag	0���ƍ��񂹁A1���ƃZ���^�����O�A2���ƉE��
 * @param   color	�����F�w��i�w�i�F��BMP��h��Ԃ��܂��j
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TalkPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color, WT_PRINT *print )
{
	x = printCommonFunc( win, strbuf, x, flag, color, FONT_TALK, print );

	GF_STR_PrintColor( win, FONT_TALK, strbuf, x, y, MSG_ALLPUT, color, print);
	
}

//==============================================================================
/**
 * @brief   BMPWIN���̕\���ʒu���w�肵��FONT_SYSTEM�Ńv�����g(�`��̂݁j
 *
 * @param   win		BMPWIN
 * @param   strbuf	������
 * @param   x		X���W
 * @param   y		Y���W
 * @param   flag	0���ƍ��񂹁A1���ƃZ���^�����O�A2���ƉE��
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SysPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color, WT_PRINT *print )
{
	x = printCommonFunc( win, strbuf, x, flag, color, FONT_SYSTEM, print );

	GF_STR_PrintColor( win, FONT_SYSTEM, strbuf, x, y, MSG_ALLPUT, color, print);
}

//==============================================================================
/**
 * @brief   BMPWIN���̕\���ʒu���w�肵��FONT_SYSTEM�Ńv�����g(�`��̂݁j
 *
 * @param   win		BMPWIN
 * @param   strbuf	������
 * @param   x		X���W
 * @param   y		Y���W
 * @param   flag	0���ƍ��񂹁A1���ƃZ���^�����O�A2���ƉE��
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_TouchPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color , WT_PRINT *print )
{
	x = printCommonFunc( win, strbuf, x, flag, color, FONT_SYSTEM, print );

	GF_STR_PrintColor( win, FONT_TOUCH, strbuf, x, y, MSG_ALLPUT, color, print);
}



//==============================================================================
/**
 * @brief   GTS�������͕`��
 *
 * @param   win		BHMWPIN
 * @param   msgman	���b�Z�[�W�}�l�[�W���[
 * @param   no		0 - 4 �u�f�s�r�v�u��������v�u�悤�����݂�v�u��������v�u�������v
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_ExplainPrint( GFL_BMPWIN *win,  GFL_MSGDATA *msgman, int no, WT_PRINT *print )
{
	static const explain_tbl[]={
		msg_gts_explain_001, msg_gts_explain_002,msg_gts_explain_003,msg_gts_explain_004,msg_gts_explain_005
	};
	STRBUF *strbuf = GFL_MSG_CreateString( msgman, explain_tbl[no] );
	WorldTrade_SysPrint( win, strbuf, 0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );
	GFL_STR_DeleteBuffer(strbuf);
}

//------------------------------------------------------------------
/**
 * @brief   WIFI�A�C�R����\���o�^����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void WorldTrade_WifiIconAdd( WORLDTRADE_WORK *wk )
{
    WirelessIconEasy();
}

