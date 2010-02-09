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
//TODO
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"


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
//#include "savedata/email_savedata.h"
//TODO


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static void _systemMessagePrint( WORLDTRADE_WORK *wk, int msgno );
static void errorDisp(WORLDTRADE_WORK* wk, int type, int code);


static int Enter_Start( WORLDTRADE_WORK *wk);
static int Enter_ConnectYesNoSelect( WORLDTRADE_WORK *wk );
static int Enter_EndYesNoSelect( WORLDTRADE_WORK *wk );
static int Enter_InternetConnect( WORLDTRADE_WORK *wk );
static int Enter_InternetConnectWait( WORLDTRADE_WORK *wk );
static int Enter_DpwTrInit( WORLDTRADE_WORK *wk );
static int Enter_ServerStart( WORLDTRADE_WORK *wk );
static int Enter_ProfileStart( WORLDTRADE_WORK *wk );
static int Enter_ProfileResult( WORLDTRADE_WORK *wk );
static int Enter_End( WORLDTRADE_WORK *wk);
///static int Enter_YesNo( WORLDTRADE_WORK *wk);
static int Enter_YesNoSelect( WORLDTRADE_WORK *wk);
static int Enter_MessageWait( WORLDTRADE_WORK *wk );
static int Enter_MessageWaitYesNoStart(WORLDTRADE_WORK *wk);
static int Enter_ServerServiceError( WORLDTRADE_WORK *wk );
static int Enter_ServerServiceEnd( WORLDTRADE_WORK *wk );
static int Enter_ServerResult( WORLDTRADE_WORK *wk );
static int Enter_EndStart( WORLDTRADE_WORK *wk );
static int Enter_ForceEndStart( WORLDTRADE_WORK *wk );
static int Enter_ForceEnd( WORLDTRADE_WORK *wk ) ;
static int Enter_ForceEndMessage( WORLDTRADE_WORK *wk );
static int Enter_MessageWait1Second( WORLDTRADE_WORK *wk );
static int Enter_WifiConnectionLogin( WORLDTRADE_WORK *wk );
static int Enter_WifiConnectionLoginWait( WORLDTRADE_WORK *wk );
static int Enter_DwcErrorPrint( WORLDTRADE_WORK *wk );
static int Enter_ErrorPadWait( WORLDTRADE_WORK *wk );
static int Enter_Wifilogin_start( WORLDTRADE_WORK *wk );
static int Enter_Wifilogin_wait( WORLDTRADE_WORK *wk );
static int Enter_Wifilogout_start( WORLDTRADE_WORK *wk );
static int Enter_Wifilogout_wait( WORLDTRADE_WORK *wk );


static int printCommonFunc( GFL_BMPWIN *win, STRBUF *strbuf, int x, int flag, PRINTSYS_LSB color, int font, WT_PRINT *print );

enum{
	ENTER_START=0,
	ENTER_CONNECT_YESNO_SELECT,
	ENTER_INTERNET_CONNECT,
	ENTER_INTERNET_CONNECT_WAIT,
	ENTER_WIFI_CONNECTION_LOGIN,
	ENTER_WIFI_CONNECTION_LOGIN_WAIT,
	ENTER_DPWTR_INIT,
	ENTER_SERVER_START,
	ENTER_SERVER_RESULT,
	ENTER_PROFILE_START,
	ENTER_PROFILE_RESULT,
	ENTER_END,
	ENTER_MES_WAIT,
	ENTER_MES_WAIT_YESNO_START, 
	ENTER_YESNO_SELECT,
	ENTER_END_START,
	ENTER_END_YESNO_SELECT,
	ENTER_FORCE_END_START,
	ENTER_FORCE_END,
	ENTER_FORCE_END_MESSAGE,
	ENTER_MES_WAIT_1_SECOND,

	ENTER_SERVER_SERVICE_ERROR,
	ENTER_SERVER_SERVICE_END,
	
	ENTER_DWC_ERROR_PRINT,
	ENTER_ERROR_PAD_WAIT,

  ENTER_WIFILOGIN_PROC_START,
	ENTER_WIFILOGIN_PROC_WAIT,
  ENTER_WIFILOGOUT_PROC_START,
	ENTER_WIFILOGOUT_PROC_WAIT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	Enter_Start,				// ENTER_START=0,
	Enter_ConnectYesNoSelect,	// ENTER_CONNECT_YESNO_SELECT,
	Enter_InternetConnect,		// ENTER_INTERNET_CONNECT
	Enter_InternetConnectWait,	// ENTER_INTERNET_CONNECT_WAIT
	Enter_WifiConnectionLogin,  // ENTER_WIFI_CONNECTION_LOGIN
	Enter_WifiConnectionLoginWait, // ENTER_WIFI_CONNECTION_LOGIN_WAIT
	Enter_DpwTrInit,			// ENTER_DPWTR_INIT
	Enter_ServerStart,			// ENTER_SERVER_START
	Enter_ServerResult,			// ENTER_SERVER_RESULT
	Enter_ProfileStart,			// ENTER_PROFILE_START
	Enter_ProfileResult,		// ENTER_PROFILE_RESULT
	Enter_End,             	 	// ENTER_END,
	Enter_MessageWait,     	 	// ENTER_MES_WAIT
	Enter_MessageWaitYesNoStart,// ENTER_MES_WAIT_YESNO_START
	Enter_YesNoSelect,			// ENTER_YESNO_SELECT
	Enter_EndStart,				// ENTER_END_START
	Enter_EndYesNoSelect,		// ENTER_END_YESNO_SELECT
	Enter_ForceEndStart,		// ENTER_FORCE_END_START
	Enter_ForceEnd,				// ENTER_FORCE_END
	Enter_ForceEndMessage,		// ENTER_FORCE_END_MESSAGE
	Enter_MessageWait1Second,	// ENTER_MES_WAIT_1_SECOND
	
	Enter_ServerServiceError,	// ENTER_SERVER_SERVICE_ERROR
	Enter_ServerServiceEnd,		// ENTER_SERVER_SERVICE_END

	Enter_DwcErrorPrint,		// ENTER_DWC_ERROR_PRINT
	Enter_ErrorPadWait,			// ENTER_ERROR_PAD_WAIT

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
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_enter.c [172] MS ********************\n" );
#endif


    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����ON
//    CommStateWifiDPWStart( wk->param->savedata );
	// Wifi�ʐM�A�C�R��
//	WorldTrade_WifiIconAdd( wk );  //2768
	

  //���[�h�ʋN���֕ύX
  switch( wk->sub_process_mode )
  { 
  case MODE_WIFILOGIN:     ///< WIFI���O�C��
    wk->subprocess_seq = ENTER_WIFILOGIN_PROC_START;
    break;
  case MODE_CONNECT:       ///< GTS�T�[�o�[�ڑ�������
    wk->subprocess_seq = ENTER_START;
    break;
  case MODE_DISCONNECT:   ///< GTS�T�[�o�[�ؒf������
    wk->subprocess_seq = ENTER_FORCE_END_START;
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
  wk->procsys = GFL_PROC_LOCAL_boot( HEAPID_WORLDTRADE );
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
  GFL_PROC_LOCAL_Exit( wk->procsys );
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
	// WIFI�R�l�N�V�����ɐڑ����܂����H
  //
  //
  //  WIFILOGIN�ŃC���^�[�l�b�g�ڑ�������̂ł���Ȃ�
//  wk->subprocess_seq  = ENTER_INTERNET_CONNECT; //YES
  wk->subprocess_seq  = ENTER_WIFI_CONNECTION_LOGIN; //YES


#if 0 //NO
			WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
			wk->subprocess_seq  = ENTER_END;
#endif

	wk->boxSearchFlag = 1;


#if 0
	// �f�[�^�G���R�[�h�e�X�g
	{
		u16  *src,*dest;
		int ret,i;
		src  = (u16*)GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, 3008 );
		dest = (u16*)GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, 4064 );
		for(i=0;i<3064/2;i++){
			src[i] = GFUser_GetPublicRand(GFUSER_RAND_PAST_MAX)();
		}
		ret = DWC_Base64Encode((const char*)src, 3008, NULL, 4064);
		OS_Printf("Base64Encoded size is %d\n", ret);
	}
#endif

	OS_TPrintf("Enter �J�n\n");

	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   �ڑ����J�n���܂����H
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ConnectYesNoSelect( WORLDTRADE_WORK *wk )
{
//	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );
	u32 ret = WorldTrade_TouchSwMain(wk);

	if(ret==TOUCH_SW_RET_YES){
			// WIFI���������J�n
			TOUCH_SW_FreeWork( wk->tss );
			//Enter_MessagePrint( wk, wk->LobbyMsgManager, msg_wifilobby_002, 1, 0x0f0f );
			wk->subprocess_seq  = ENTER_INTERNET_CONNECT;
			WorldTrade_TimeIconAdd(wk);
	}else if(ret==TOUCH_SW_RET_NO){
			// �I��
			TOUCH_SW_FreeWork( wk->tss );
#if 0
		  //  CommStateWifiDPWEnd();
#else
			//GFL_NET_Exit(NULL);
#endif
      GF_ASSERT(0);
			WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
			wk->subprocess_seq  = ENTER_END;
	}

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �ڑ����I�����܂����H
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_EndStart( WORLDTRADE_WORK *wk ) 
{

  //YES
  if( 1 )
  { 
    if(!DWC_CheckInet()){		
      // �ڑ����J�n���܂����H
      wk->subprocess_seq  = ENTER_START;
    }else{
      //				 WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
      //				wk->subprocess_seq  = ENTER_END;

      // ���ɐڑ��ς݂Ȃ�
      // �T�[�o�[�`�F�b�N�̌�^�C�g�����j���[��
      WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_SERVER_CHECK );
      wk->sub_returnprocess = WORLDTRADE_TITLE;
      wk->subprocess_seq    = ENTER_END;

    }

    //NO
  }else if( 0 ){
    // WIFI���������I��
    if(DWC_CheckInet()){		
      DWC_CleanupInet();
    }
    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����OFF
    //CommStateWifiDPWEnd();
    GF_ASSERT(0);
    WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
    wk->subprocess_seq  = ENTER_END;
  }

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �ڑ����I�����܂����H�i���E�����̏I���j
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_EndYesNoSelect( WORLDTRADE_WORK *wk )
{
	u32 ret = WorldTrade_TouchSwMain(wk);
	if(ret==TOUCH_SW_RET_YES){
		if(!DWC_CheckInet()){		
			// �ڑ����J�n���܂����H
			wk->subprocess_seq  = ENTER_START;
		}else{
			//				 WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
			//				wk->subprocess_seq  = ENTER_END;

			// ���ɐڑ��ς݂Ȃ�
			// �T�[�o�[�`�F�b�N�̌�^�C�g�����j���[��
			WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_SERVER_CHECK );
			wk->sub_returnprocess = WORLDTRADE_TITLE;
			wk->subprocess_seq    = ENTER_END;

		}
	}else if(ret==TOUCH_SW_RET_NO ){
		// WIFI���������I��
		if(DWC_CheckInet()){		
			DWC_CleanupInet();
		}

    GF_ASSERT(0);
		// �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����OFF
		//CommStateWifiDPWEnd();
		WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
		wk->subprocess_seq  = ENTER_END;
	}
	

	return SEQ_MAIN;
	
}




//------------------------------------------------------------------
/**
 * @brief   ���ɑI���͏I����Ă���̂�WIFI����ڑ�����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ForceEndStart( WORLDTRADE_WORK *wk ) 
{
	// �ڑ����I�����܂�
  //wk->subprocess_seq  = ENTER_FORCE_END;
  //
  //WIFILOGIN�̒��ŃC���^�[�l�b�g�ؒf������̂ł���Ȃ���
  wk->subprocess_seq  = ENTER_WIFILOGOUT_PROC_START;
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �ڑ��I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ForceEnd( WORLDTRADE_WORK *wk )
{
    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����OFF
    //CommStateWifiDPWEnd();
		//TODO
	// WIFI���������I��
    DWC_CleanupInet();

    wk->subprocess_seq  = ENTER_WIFILOGOUT_PROC_START;

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �ڑ��I�����b�Z�[�W
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ForceEndMessage( WORLDTRADE_WORK *wk )
{
	//Enter_MessagePrint( wk, wk->SystemMsgManager, dwc_message_0012, 1, 0x0f0f );
	//WorldTrade_SetNextSeq( wk, ENTER_MES_WAIT_1_SECOND, ENTER_END );

	wk->subprocess_seq  = ENTER_END;
		
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �C���^�[�l�b�g�ڑ��J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_InternetConnect( WORLDTRADE_WORK *wk )
{
    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����ON
    //CommStateWifiDPWStart( wk->param->savedata );
	//sys_SleepNG(SLEEPTYPE_COMM);

	WorldTrade_WifiIconAdd( wk );  //2768
	DWC_InitInetEx(&wk->stConnCtrl,GFL_DMA_NET_NO,COMM_POWERMODE,COMM_SSL_PRIORITY);
	DWC_ConnectInetAsync();
	
	wk->subprocess_seq = ENTER_INTERNET_CONNECT_WAIT;
	OS_TPrintf("InternetConnet Start\n");

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �l�b�g�ڑ��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_InternetConnectWait( WORLDTRADE_WORK *wk )
{
	DWC_ProcessInet();

	if (DWC_CheckInet())
	{
		switch (DWC_GetInetStatus())
		{
		case DWC_CONNECTINET_STATE_ERROR:
			{
				// �G���[�\��
				DWCError err;
				int errcode;
                DWCErrorType errtype;
                
				err = DWC_GetLastErrorEx(&errcode,&errtype);
				wk->ErrorRet  = err;
				wk->ErrorCode = errcode;
                wk->ErrorType = errtype;

				OS_TPrintf("   Error occurred %d %d.\n", err, errcode);
			}
			DWC_ClearError();
			DWC_CleanupInet();

			WorldTrade_TimeIconDel( wk );
			wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;

			break;
		case DWC_CONNECTINET_STATE_NOT_INITIALIZED:
		case DWC_CONNECTINET_STATE_IDLE:
		case DWC_CONNECTINET_STATE_OPERATING:
		case DWC_CONNECTINET_STATE_OPERATED:
		case DWC_CONNECTINET_STATE_DISCONNECTING:
		case DWC_CONNECTINET_STATE_DISCONNECTED:
		default:
			OS_TPrintf("DWC_CONNECTINET_STATE_DISCONNECTED!\n");
			//break;
		case DWC_CONNECTINET_STATE_FATAL_ERROR:
			{
				// �G���[�\��
				DWCError err;
				int errcode;
				err = DWC_GetLastError(&errcode);

				OS_TPrintf("   Error occurred %d %d.\n", err, errcode);
				WorldTrade_TimeIconDel(wk);
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				wk->ConnectErrorNo = DPW_TR_ERROR_SERVER_TIMEOUT;
			}
			break;

		case DWC_CONNECTINET_STATE_CONNECTED:
	        {	// �ڑ����\������B�X�܂̏ꍇ�͓X�܏����\������B
				DWCApInfo apinfo;
	
				DWC_GetApInfo(&apinfo);
	
	            OS_TPrintf("   Connected to AP type %d.\n", apinfo.aptype);

	            if (apinfo.aptype == DWC_APINFO_TYPE_SHOP)
	            {
					OS_TPrintf("<Shop AP Data>\n");
	                OS_TPrintf("area code: %d.\n", apinfo.area);
	                OS_TPrintf("spotinfo : %s.\n", apinfo.spotinfo);
	            }
	        }
	        // �R�l�N�g�����H
			wk->subprocess_seq = ENTER_WIFI_CONNECTION_LOGIN;
			break;
		}

		// ���ԃA�C�R������

	}
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   GameSpy�T�[�o�[���O�C���J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_WifiConnectionLogin( WORLDTRADE_WORK *wk )
{
	DWC_NASLoginAsync();
	wk->subprocess_seq = ENTER_WIFI_CONNECTION_LOGIN_WAIT;
	OS_Printf("GameSpy�T�[�o�[���O�C���J�n\n");

	return SEQ_MAIN;
}
//------------------------------------------------------------------
/**
 * @brief   GameSpy�T�[�o�[���O�C�������҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_WifiConnectionLoginWait( WORLDTRADE_WORK *wk )
{
	switch(DWC_NASLoginProcess()){
	case DWC_NASLOGIN_STATE_SUCCESS:
		OS_Printf("GameSpy�T�[�o�[���O�C������\n");
		wk->subprocess_seq = ENTER_DPWTR_INIT;
		break;
	case DWC_NASLOGIN_STATE_ERROR:
	case DWC_NASLOGIN_STATE_CANCELED:
	case DWC_NASLOGIN_STATE_DIRTY:
		WorldTrade_TimeIconDel(wk);
		OS_Printf("GameSpy�T�[�o�[���O�C�����s\n");
		{
			int errCode;
			DWCErrorType errType;
			DWCError dwcError;
			dwcError = DWC_GetLastErrorEx( &errCode, &errType );
			wk->ErrorRet  = dwcError;
			wk->ErrorCode = errCode;

			DWC_ClearError();
			DWC_CleanupInet();

			//���肦�Ȃ��͂������A�ǂ̃G���[�ɂ�����������Ȃ��\�����l�����A�����l�Ƃ��Ď��̃V�[�P���X���ɐݒ肵�Ă���
			wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;

			switch(errType){
			case DWC_ETYPE_LIGHT:
			case DWC_ETYPE_SHOW_ERROR:
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_GHTTP:
				DWC_ShutdownGHTTP();
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_DISCONNECT:
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_FM:
				OS_TPrintf("DWC_ETYPE_SHUTDOWN_FM!\n");
				DWC_ShutdownFriendsMatch();
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
				break;
			case DWC_ETYPE_SHUTDOWN_ND:	//���̃V�[�P���X�ł͂��肦�Ȃ��̂ňꉞ�����ӂ��Ƃ΂��ɂ���
				OS_TPrintf("DWC_ETYPE_SHUTDOWN_ND!\n");
				//break;
			case DWC_ETYPE_FATAL:
				// �����ӂ��Ƃ΂�
				CommFatalErrorFunc_NoNumber();
				break;
			}

			// 20000�ԑ���L���b�`������errType�����ł��낤�ƃ��Z�b�g�G���[��
			if(errCode<-20000 && errCode >=-29999){
//				CommSetErrorReset(COMM_ERROR_RESET_TITLE);
				OS_Printf("dwcError = %d  errCode = %d, errType = %d\n", dwcError, errCode, errType);
				wk->subprocess_seq = ENTER_DWC_ERROR_PRINT;
			}
		}
		break;
	}
	
	
	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   ���E�����T�[�o�[�ڑ����C�u����������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_DpwTrInit( WORLDTRADE_WORK *wk )
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

	OS_TPrintf("Dpw Trade ������\n");

	wk->subprocess_seq = ENTER_SERVER_START;
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���E�������C�u�����ڑ��󋵎擾�J�n
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ServerStart( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_GetServerStateAsync();

	OS_TPrintf("Dpw Trade �T�[�o�[��Ԏ擾�J�n\n");

	// �T�[�o�[��Ԋm�F�҂���
	wk->subprocess_seq = ENTER_SERVER_RESULT;
	wk->timeout_count = 0;
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �T�[�o�[��Ԋm�F�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ServerResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
			OS_TPrintf(" server is up!\n");
		
		#if 0
			// �|�P�����m�F�T�u�v���Z�X�ֈڍs
			WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
			wk->subprocess_seq  = ENTER_END;
		#else
			wk->subprocess_seq  = ENTER_PROFILE_START;
		#endif
			break;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
			OS_TPrintf(" server stop service.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_STATUS_SERVER_FULL:			// �T�[�o�[�����t
		case DPW_TR_ERROR_SERVER_FULL:
			OS_TPrintf(" server full.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// �uGTS�̂����ɂ�ɂ����ς����܂����v���^�C�g����
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// �T�[�o�[�ƒʐM�ł��܂��񁨏I��
			OS_TPrintf(" upload error. %d \n", result);
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
		default:
			// ���ӂ��Ƃ΂�
			WorldTrade_TimeIconDel(wk);
			CommFatalErrorFunc_NoNumber();
			break;

		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
			CommFatalErrorFunc_NoNumber();	//�����ӂ��Ƃ΂�
		}
	}
	return SEQ_MAIN;
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
static int Enter_ProfileStart( WORLDTRADE_WORK *wk )
{
	EMAILSAVE_DCProfileCreate_Update(wk->param->savedata, &wk->dc_profile);
	Dpw_Tr_SetProfileAsync(&wk->dc_profile, &wk->dc_profile_result);

	OS_TPrintf("Dpw Trade �v���t�B�[��(E���[��)���M\n");

	// �T�[�o�[��Ԋm�F�҂���
	wk->subprocess_seq = ENTER_PROFILE_RESULT;
	wk->timeout_count = 0;
	
	return SEQ_MAIN;
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
static int Enter_ProfileResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
			OS_TPrintf(" profile is up!\n");

			WorldTrade_TimeIconDel(wk);
			switch(wk->dc_profile_result.code){
			case DPW_PROFILE_RESULTCODE_SUCCESS:	//���̓o�^�ɐ���
				OS_TPrintf("mailAddrAuthResult = %d\n", wk->dc_profile_result.mailAddrAuthResult);
				switch(wk->dc_profile_result.mailAddrAuthResult){
				case DPW_PROFILE_AUTHRESULT_SUCCESS:	//�F�ؐ���
					// �|�P�����m�F�T�u�v���Z�X�ֈڍs
					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					wk->subprocess_seq  = ENTER_END;
					break;
				//�ȉ��̃G���[�����͂��̃V�[���ł͑z�肵�Ă��Ȃ����[���F�؂̌��ʂ��Ԃ����ꍇ
				// (�����̗F�B�R�[�h���ω������Ƃ��ɈȑO�Ɠ������[���A�h���X�ƃp�X���[�h��
				// �M�����Ƃ��ɂ��̂悤�ɂȂ�\��������܂��B�ʏ�͂��蓾�܂���B)�}�j���A�����p
				case DPW_PROFILE_AUTHRESULT_FAILURE:	//�F�؂Ɏ��s
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_FAILURE;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				case DPW_PROFILE_AUTHRESULT_SEND:	//�F�؃��[�����M����
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_SEND;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				case DPW_PROFILE_AUTHRESULT_SENDFAILURE:	//�F�؃��[���̑��M�Ɏ��s
					OS_TPrintf(" mail service error\n");
					wk->ConnectErrorNo = EMAIL_ERROR_SENDFAILURE;
					wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
					break;
				default:	//���肦�Ȃ����ǈꉞ�B�����ӂ��Ƃ΂�
					CommFatalErrorFunc_NoNumber();
					break;
				}
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_INVALIDPARAM:	//�v���t�B�[���̑��M�p�����[�^�s��
				OS_TPrintf(" server stop service.\n");
				wk->ConnectErrorNo = EMAIL_ERROR_INVALIDPARAM;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			case DPW_PROFILE_RESULTCODE_ERROR_SERVERSTATE:	//�T�[�o�����e�i���Xor�ꎞ��~��
				OS_TPrintf(" server stop service.\n");
				wk->ConnectErrorNo = EMAIL_ERROR_SERVERSTATE;
				wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
				break;
			default:
				// ���ӂ��Ƃ΂�
				OS_TPrintf("default error !\n");
				WorldTrade_TimeIconDel(wk);
				CommFatalErrorFunc_NoNumber();
				break;
			}
			break;
		case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
			OS_TPrintf(" server stop service.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_STATUS_SERVER_FULL:			// �T�[�o�[�����t
		case DPW_TR_ERROR_SERVER_FULL:
			OS_TPrintf(" server full.\n");
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// �uGTS�̂����ɂ�ɂ����ς����܂����v���^�C�g����
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// �T�[�o�[�ƒʐM�ł��܂��񁨏I��
			OS_TPrintf(" upload error. %d \n", result);
			WorldTrade_TimeIconDel(wk);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = ENTER_SERVER_SERVICE_ERROR;
			break;
		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
		default:
			// ���ӂ��Ƃ΂�
			WorldTrade_TimeIconDel(wk);
			CommFatalErrorFunc_NoNumber();
			break;
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
			CommFatalErrorFunc_NoNumber();	//�����ӂ��Ƃ΂�
		}
	}
	return SEQ_MAIN;
}




//------------------------------------------------------------------
/**
 * @brief   Wifi�ڑ��G���[��\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_DwcErrorPrint( WORLDTRADE_WORK *wk )
{
	int type;
	
    type =  GFL_NET_DWC_ErrorType(-wk->ErrorCode, wk->ErrorType);


	OS_Printf("error code = %d, type = %d\n", wk->ErrorCode, type);

    errorDisp(wk, type, -wk->ErrorCode);


	wk->subprocess_seq = ENTER_ERROR_PAD_WAIT;
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �v�����g��L�[�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_ErrorPadWait( WORLDTRADE_WORK *wk )
{
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE || GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL || GFL_UI_TP_GetTrg()){
		BmpMenuWinClear( wk->SubWin, WINDOW_TRANS_ON );
		wk->subprocess_seq = ENTER_START;
	}
	return SEQ_MAIN;
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

  GFL_PROC_LOCAL_CallProc( wk->procsys, FS_OVERLAY_ID(wifi_login), 
      &WiFiLogin_ProcData, wk->sub_proc_wk );

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

  if( GFL_PROC_MAIN_NULL == GFL_PROC_LOCAL_Main( wk->procsys ) )
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

  GFL_PROC_LOCAL_CallProc( wk->procsys, FS_OVERLAY_ID(wifi_login), 
      &WiFiLogout_ProcData, wk->sub_proc_wk );

  wk->subprocess_seq = ENTER_WIFILOGOUT_PROC_WAIT;

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
  if( GFL_PROC_MAIN_NULL == GFL_PROC_LOCAL_Main( wk->procsys ) )
  { 
    GFL_HEAP_FreeMemory(wk->sub_proc_wk);

    //���̃V�[�P���X��ENTER��������I��
    WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
    wk->subprocess_seq  = ENTER_END;
  }

	return SEQ_MAIN;
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
    //WirelessIconEasyEnd();

	// ���ԃA�C�R�������Q�d����ɂȂ�Ȃ��悤��NULL�`�F�b�N����
	WorldTrade_TimeIconDel( wk );

	
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_enter.c [1037] MS ********************\n" );
#endif
	wk->subprocess_seq = 0;
	wk->sub_out_flg = 1;
	
	return SEQ_FADEOUT;
}

//------------------------------------------------------------------
/**
 * @brief   �͂��E�������I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_YesNoSelect( WORLDTRADE_WORK *wk)
{
	u32 ret = WorldTrade_TouchSwMain(wk);

  if(ret==TOUCH_SW_RET_YES){
    // WIFI���������I��

    TOUCH_SW_FreeWork( wk->tss );
    WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
    wk->subprocess_seq = ENTER_END;
	}else if(ret==TOUCH_SW_RET_NO){
			// �������������g���C
			TOUCH_SW_FreeWork( wk->tss );
			wk->subprocess_seq = ENTER_START;
	}
	
/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// �������������g���C
			wk->subprocess_seq = ENTER_START;
		}else{
			// WIFI���������I��
			 WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
			wk->subprocess_seq = ENTER_END;
		}
	}
*/
	return SEQ_MAIN;
	
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
static int Enter_ServerServiceError( WORLDTRADE_WORK *wk )
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
  // @todo
	//Enter_MessagePrint( wk, wk->MsgManager, msgno, 1, 0x0f0f );
  wk->subprocess_seq  = ENTER_SERVER_SERVICE_END;
	OS_TPrintf("Error����\n");

	return SEQ_MAIN;
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
static int Enter_ServerServiceEnd( WORLDTRADE_WORK *wk )
{
	switch(wk->local_seq){
	case 0:
		//Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_cleanup_000, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 1:
		//if( GF_MSG_PrintEndCheck( &wk->print )==0){
		    // �ʐM�G���[�Ǘ��̂��߂ɒʐM���[�`����OFF
		    //CommStateWifiDPWEnd();
		    DWC_CleanupInet();
			wk->local_seq++;
	//	}
		break;
	case 2:
		//Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_cleanup_001, 1, 0x0f0f );
		wk->local_seq++;
		break;
	case 3:
	//	if(GF_MSG_PrintEndCheck( &wk->print )==0){
			wk->local_seq++;
	//	}
		break;
	default:
		wk->local_wait++;
		if(wk->local_wait > WAIT_ONE_SECONDE_NUM){
			WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
			wk->subprocess_seq  = ENTER_END;
		}
		break;
	}
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ��b�I����҂��Ď��̃V�[�P���X��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_MessageWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}

//------------------------------------------------------------------
/**
 * @brief   ��b�\����1�b�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_MessageWait1Second( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		if(wk->wait > WAIT_ONE_SECONDE_NUM){
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
		wk->wait++;
	}
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   ��b�\����҂�����Łu�͂��E�������v���J�n����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int Enter_MessageWaitYesNoStart(WORLDTRADE_WORK *wk)
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		//wk->tss = WorldTrade_TouchWinYesNoMake(WORLDTRADE_YESNO_PY2, YESNO_OFFSET, 8, 0 );
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;
	
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


//------------------------------------------------------------------
/**
 * @brief   ���������G���[�\��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   msgno	���b�Z�[�WNO
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void _systemMessagePrint( WORLDTRADE_WORK *wk, int msgno )
{
	STRBUF *tmpString = GFL_STR_CreateBuffer( DWC_ERROR_BUF_NUM, HEAPID_WORLDTRADE );
    GFL_MSG_GetString(  wk->SystemMsgManager, msgno, tmpString );
    WORDSET_ExpandStr( wk->WordSet, wk->ErrorString, tmpString );

    // ��b�E�C���h�E�g�`��
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SubWin), 15 );
    BmpWinFrame_Write(wk->SubWin, WINDOW_TRANS_OFF, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );
    // ������`��J�n
    GF_STR_PrintSimple( wk->SubWin, FONT_TALK,
                                       wk->ErrorString, 0, 0, &wk->print);

	GFL_STR_DeleteBuffer(tmpString);
}

//----------------------------------------------------------------------------------
/**
 * @brief	Wifi�R�l�N�V�����G���[�̕\��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   type	�G���[�^�C�v
 * @param   code	�G���[�R�[�h
 */
//----------------------------------------------------------------------------------
static void errorDisp(WORLDTRADE_WORK* wk, int type, int code)
{
    int msgno;

    if(type != -1){
        msgno = dwc_error_0001 + type;
    }
    else{
        msgno = dwc_error_0012;
    }
//    EndMessageWindowOff(wk);
    WORDSET_RegisterNumber(wk->WordSet, 0, code,
                           5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);

	BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
    _systemMessagePrint(wk, msgno);

#if 0
    switch(type){
      case 1:
      case 4:
      case 5:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // �Đڑ����t�B�[���h��
        break;
      case 6:
      case 7:
      case 8:
      case 9:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;//WIFIP2PMATCH_POWEROFF_INIT;  // �d����؂邩�t�B�[���h
        break;
      case 10:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // ���j���[�ꗗ��
        break;
      case 0:
      case 2:
      case 3:
      case 11:
      default:
        wk->seq = WIFIP2PMATCH_MODE_CHECK_AND_END;  // �t�B�[���h
        break;
    }
#endif
}



