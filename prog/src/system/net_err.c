//==============================================================================
/**
 * @file	net_err.c
 * @brief	�ʐM�G���[��ʐ���
 * @author	matsuda
 * @date	2008.11.14(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "net\network_define.h"
#include "system/net_err.h"
#include "system/ds_system.h"
#include "arc_def.h"
#include "net_err.naix"

#include "print\wordset.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "font/font.naix"
#include "msg\msg_net_err.h"
#include "sound\pm_sndsys.h"
#include "system/machine_use.h"

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#include <dwc.h>
#include "net/dwc_rap.h"
#include "msg\msg_wifi_system.h"
#include "message.naix"
#include "sound\pm_voice.h"
#endif //MULTI_BOOT_MAKE

#ifdef PM_DEBUG

#ifdef WIFIMATCH_RATE_AUTO
#undef DEBUG_ERR_THROUGH
#define DEBUG_ERR_THROUGH  //���̒�`��ON�ɂ���ƁA�G���[�̃{�^���������X���[����[
#endif
#endif//PM_DEBUG


//==============================================================================
//	�萔��`
//==============================================================================
///�ʐM�G���[��ʂŎg�p����e���|�����q�[�v
#define HEAPID_NET_TEMP			(GFL_HEAPID_APP)

///�ʐM�G���[��ʂ��I��������{�^��
#define ERR_DISP_END_BUTTON		(PAD_BUTTON_A | PAD_BUTTON_B)

///�G���[��ʕ\����A�L�[��L���ɂ���܂ł̃E�F�C�g
#define ERR_DISP_KEY_OCC_WAIT		(60*2)

///���b�Z�[�W�`��̈�J�n�ʒu�F���[X
#define MESSAGE_START_X			(1)
///���b�Z�[�W�`��̈�J�n�ʒu�F��[Y
#define MESSAGE_START_Y			(4)
///���b�Z�[�W�`��̈�X��
#define MESSAGE_X_LEN			(30)
///���b�Z�[�W�`��̈�Y��
#define MESSAGE_Y_LEN			(16)

///���b�Z�[�W�̃p���b�g�W�J�ʒu
#define MESSAGE_PALNO			(0)

///�w�iBG�O���t�B�b�N(ncg)�̃f�[�^�T�C�Y
#define BG_DATA_SIZE			(32 * 0x20)
///���b�Z�[�W�`��J�n�L�����N�^No
#define MESSAGE_START_CHARNO	(BG_DATA_SIZE / 0x20)

//�w�iBG�{���b�Z�[�W�f�[�^�ŃL�����N�^�̈�NETERR_PUSH_CHARVRAM_SIZE���I�[�o�[���Ă��Ȃ����`�F�b�N
SDK_COMPILER_ASSERT(MESSAGE_X_LEN*0x20*MESSAGE_Y_LEN + BG_DATA_SIZE <= NETERR_PUSH_CHARVRAM_SIZE);


//==============================================================================
//	�\���̒�`
//==============================================================================
///�ʐM�G���[��ʐ���\����
typedef struct{
	u8 *push_char_p;			///<�L�����N�^VRAM�ޔ��
	u16 *push_scrn_p;			///<�X�N���[��VRAM�ޔ��
	u16 *push_pltt_p;			///<�p���b�gVRAM�ޔ��

	GXDispCnt dispcnt;
	GXBg01Control bg1cnt;
	int master_brightness;		///<���C����ʃ}�X�^�[�P�x
	int master_brightness_sub;	///<�T�u��ʃ}�X�^�[�P�x
	s32 h_intr;					///<H�u�����N���荞�݋���
	s32 v_intr;					///<V�u�����N���荞�݋���
	GXVRamBG bg_bank;			    ///<BG VRAM�o���N���
	GXVRamSubBG bg_bank_sub;	///<SUB BG VRAM�o���N���
	
	NET_ERR_STATUS status;		///<�G���[��ʃV�X�e���̏�
	s32 key_timer;				///<�L�[���͎�t�܂ł̃^�C�}�[

  u16 g2_blend_cnt;   //����J���[�̎擾
  u16 g2s_blend_cnt;  //����J���[�̎擾

  int sub_visible;    //�T�u��ʂ̕\���F�̐ݒ�
  int sub_wnd;
  u16 sub_bd_color;
  u16 dummy2;

  NET_ERR_PUSHPOP_MODE  pushpop_mode;

  //�ȉ��G���[���ۑ�
  GFL_NETSTATE_DWCERROR   wifi_error; //WIFI�̃G���[
  u8                      error;      //WIFI�ȊO�̃G���[
  u8                      net_type;   //�l�b�g�̃^�C�v
  u8                      msg_force;  //�������b�Z�[�W�t���O
  u8                      dummy;
  NET_ERR_CHECK           err_important_type;  //�G���[�̏d�x�y�x����
  u32                     wifi_msg;   //WIFI�ŕ\��������̂͐�Ɏ擾���邽��
}NET_ERR_SYSTEM;

//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_ERR_SYSTEM NetErrSystem = {0};

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL NetErr_DispMain(BOOL fatal_error);
static void Local_ErrDispInit(BOOL fatal_error,BOOL isExitWait);
static void Local_ErrDispExit(BOOL is_black_continue);
static BOOL Local_SystemOccCheck(void);
static void Local_ErrDispDraw(void);
static void Local_ErrMessagePrint(BOOL fatal_error,BOOL isExitWait);
static void Local_ErrUpdate(void);

//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʃV�X�e��������
 *
 * �Q�[�����ň�x�����Ăяo��
 */
//--------------------------------------------------------------
void NetErr_SystemInit(void)
{
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NET_ERR, NETERR_PUSH_CHARVRAM_SIZE + NETERR_PUSH_SCRNVRAM_SIZE + NETERR_PUSH_PLTTVRAM_SIZE + 0x100);  //+0x100�̓w�b�_��
	GFL_STD_MemClear(&NetErrSystem, sizeof(NET_ERR_SYSTEM));
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʃV�X�e���쐬
 * 
 * @param	�q�[�vID
 * 
 * �Q�[���̍ŏ��ɍ���āA�Ȍジ���Ǝg�p����̂ł���Έ�x�����̌Ăяo����OK�B
 * �q�[�v�ߖ�̂��߂ɁA�e�A�v���Ȃǂł̂ݎg�p����ꍇ�́A���̓s�x
 * SystemCreate,SystemExit���ĂԂ悤�ɂ��Ďg���Ă�������
 */
//--------------------------------------------------------------
void NetErr_SystemCreate(int heap_id)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;

	GF_ASSERT(nes->push_char_p == NULL);
	GF_ASSERT(nes->push_scrn_p == NULL);
	GF_ASSERT(nes->push_pltt_p == NULL);
	
	GFL_STD_MemClear(nes, sizeof(NET_ERR_SYSTEM));

	nes->push_char_p = GFL_HEAP_AllocMemory(heap_id, NETERR_PUSH_CHARVRAM_SIZE);
	nes->push_scrn_p = GFL_HEAP_AllocMemory(heap_id, NETERR_PUSH_SCRNVRAM_SIZE);
	nes->push_pltt_p = GFL_HEAP_AllocMemory(heap_id, NETERR_PUSH_PLTTVRAM_SIZE);
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʃV�X�e���j��
 */
//--------------------------------------------------------------
static void NetErr_SystemExit(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;

	GF_ASSERT(nes->push_char_p != NULL);
	GF_ASSERT(nes->push_scrn_p != NULL);
	GF_ASSERT(nes->push_pltt_p != NULL);
	
	GFL_HEAP_FreeMemory(nes->push_char_p);
	GFL_HEAP_FreeMemory(nes->push_scrn_p);
	GFL_HEAP_FreeMemory(nes->push_pltt_p);

	GFL_STD_MemClear(nes, sizeof(NET_ERR_SYSTEM));
	
	GFL_HEAP_DeleteHeap(HEAPID_NET_ERR);
}

//==================================================================
/**
 * �G���[�����Ď��F���C��
 */
//==================================================================
void NetErr_Main(void)
{
  if(NetErrSystem.status == NET_ERR_STATUS_NULL && GFL_NET_IsInit() == TRUE){
    if(GFL_NET_GetErrorCode() || GFL_NET_SystemIsError() || (GFL_NET_StateGetError()!=0)){
      //GFL_NET_StateGetWifiErrorNo();  ����̓G���[���̎擾�����Ɏg�p����
      OS_TPrintf("NetErr �G���[���� %d %d %d\n", 
        GFL_NET_GetErrorCode(), GFL_NET_SystemIsError(), GFL_NET_StateGetError());
      GFL_NET_DeleteInitializeCallback();
      NetErr_ErrorSet();

      //WIFI�̃G���[�Ȃ�΃I�[�o�[���C����������O�Ƀ��b�Z�[�W�ԍ���Ⴄ
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
      {
        const GFLNetInitializeStruct* net_init  = GFL_NET_GetNETInitStruct();
        if( net_init->bNetType == GFL_NET_TYPE_WIFI 
            || net_init->bNetType == GFL_NET_TYPE_WIFI_LOBBY 
            || net_init->bNetType == GFL_NET_TYPE_WIFI_GTS )
        { 
          const GFL_NETSTATE_DWCERROR *cp_dwc_error  = GFL_NET_StateGetWifiError();

          OS_TPrintf("Wifi�G���[�ԍ� code%d ret%d type%d user%d\n", 
              cp_dwc_error->errorCode, cp_dwc_error->errorRet, cp_dwc_error->errorType, cp_dwc_error->errorUser );
          if( cp_dwc_error->errorUser == ERRORCODE_TIMEOUT
              || cp_dwc_error->errorUser == ERRORCODE_DISCONNECT )
          {
            //�^�C���A�E�g������Ɛؒf���Ă����Ȃ�΂Ȃ��
            //�u�����ĂƂ̂����񂪂���܂����v
            NetErrSystem.wifi_msg = dwc_message_0022;
          }
          else if( cp_dwc_error->errorUser == ERRORCODE_CRC
              || cp_dwc_error->errorUser == ERRORCODE_SYSTEM 
              || cp_dwc_error->errorUser == ERRORCODE_SENDQUEUE
              || cp_dwc_error->errorUser == ERRORCODE_USER_TIMEOUT
              || cp_dwc_error->errorUser == ERRORCODE_NHTTP)
          {
            //�V�X�e���G���[�Ȃ��
            //�����񂦂�[���͂������v
            NetErrSystem.wifi_msg = dwc_error_0015;
          }
          else
          {
            //DWC�̃G���[�Ȃ�΁A�^�C�v�ɂ��
            NetErrSystem.wifi_msg = GFL_NET_DWC_ErrorType( cp_dwc_error->errorCode,
                cp_dwc_error->errorType);
          }
        }
      }
#endif 
    }
  }
}

//==================================================================
/**
 * �G���[���������Ă���΃G���[��ʂ��ꔭ�Ăяo��
 *
 * @param   fatal_error   TRUE:���Z�b�g�𑣂��G���[(A�Ŕ������Ȃ�) 
 * 
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 * 
 * �G���[��ʂ�\�������ꍇ�A���[�U�[������̑�����s��Ȃ�����(A������)
 * ���̊֐����Ŗ������[�v���܂��B
 */
//==================================================================
BOOL NetErr_DispCall(BOOL fatal_error)
{
  return NetErr_DispMain(fatal_error);
}

//==================================================================
/**
 * Fatal�G���[��p�Ƃ��ăG���[��ʂ��ꔭ�Ăяo��  ���b�Z�[�W�w��WIFI��p
 * @param   message  ���b�Z�[�W�ԍ�
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 */
//==================================================================
BOOL NetErr_App_FatalDispCallWifiMessage(int message)
{
  NetErrSystem.wifi_msg = message;
  NetErrSystem.msg_force = TRUE;
  NetErrSystem.net_type = GFL_NET_TYPE_WIFI;
  return NetErr_DispCall(TRUE);
}

//==================================================================
/**
 * Fatal�G���[��p�Ƃ��ăG���[��ʂ��ꔭ�Ăяo��
 *
 * @param   none		
 *
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 */
//==================================================================
BOOL NetErr_App_FatalDispCall(void)
{
  return NetErr_DispCall(TRUE);
}

//----------------------------------------------------------------------------
/**
 *	@brief  PUSHPOP�I�����[�h�̐ݒ�
 *
 *	@param	NET_ERR_PUSHPOP_MODE pushpop_mode ���[�h
 */
//-----------------------------------------------------------------------------
void NetErr_SetPushPopMode( NET_ERR_PUSHPOP_MODE pushpop_mode )
{
  NET_ERR_SYSTEM *nes = &NetErrSystem;
  nes->pushpop_mode = pushpop_mode;
}

//----------------------------------------------------------------------------
/**
 *	@brief  PUSHPOP�I�����[�h�̃N���A
 *
 */
//-----------------------------------------------------------------------------
void NetErr_ClearPushPopMode( void )
{
  NetErr_SetPushPopMode( NET_ERR_PUSHPOP_MODE_NORMAL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PUSHPOP��NET_ERR_PUSHPOP_MODE_BLACKOUT�ɐݒ�ꍇ��
 *	        �P�x���ǂ�
 */
//-----------------------------------------------------------------------------
void NetErr_ResetPushPopBrightness( void )
{
  NET_ERR_SYSTEM *nes = &NetErrSystem;
  GX_SetMasterBrightness(nes->master_brightness);
  GXS_SetMasterBrightness(nes->master_brightness_sub);
}

//--------------------------------------------------------------
/**
 * PushPop�p��NetMain
 */
//--------------------------------------------------------------
static void _PushPopErrNetMain(void)
{
  if(GFL_NET_IsInit() == TRUE){
    GFL_NET_Main();
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
}

//==================================================================
/**
 * Push,Pop�L�̃G���[��ʈꔭ�Ăяo���@���y�x�G���[��p�B�ʐM�̏I�������͍s���܂���
 *
 * �G���[��ʂ�\�������ꍇ�A���[�U�[������̑�����s��Ȃ�����(A������)
 * ���̊֐����Ŗ������[�v���܂��B
 */
//==================================================================
void NetErr_DispCallPushPop(void)
{
  NET_ERR_SYSTEM *nes = &NetErrSystem;
  GXSDispCnt sub_dispcnt = GXS_GetDispCnt();
  GXWndPlane gx_outside = G2_GetWndOutsidePlane();
  GXWndPlane gxs_outside = G2S_GetWndOutsidePlane();
  GXWndPlane gx_inside = G2_GetWndOBJInsidePlane();
  GXWndPlane gxs_inside = G2S_GetWndOBJInsidePlane();
  GXWndPlane gx_wnd_inside = G2_GetWndOBJInsidePlane();
  
	if(Local_SystemOccCheck() == FALSE){
		GF_ASSERT(0); //�V�X�e��������Ă��Ȃ�
		return;
	}

	//�G���[��ʕ`��
	Local_ErrDispInit(FALSE,FALSE);
	
	GX_VBlankIntr(TRUE);  //Wi-Fi�ł͌q�����Ă���܂܃G���[��ʂ��o���P�[�X�������VBlank�𐶂���
	
	//�ʐM�A�C�R���̕\��(Wi-Fi�ł͌q�����Ă���܂܃G���[��ʂ��o���P�[�X�������)
	GFL_NET_WirelessIconOBJWinON();
	if(GFL_NET_WirelessIconGetVramType() == NNS_G2D_VRAM_TYPE_2DMAIN){
  	G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG1, FALSE);
  	G2_SetWndOBJInsidePlane(GX_WND_PLANEMASK_OBJ, FALSE);
  	GX_SetVisibleWnd(GX_WNDMASK_OW);
  	GX_SetVisiblePlane(GX_PLANEMASK_OBJ | GX_PLANEMASK_BG1);
  }
  else{
  	G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG1, FALSE);
  	G2S_SetWndOBJInsidePlane(GX_WND_PLANEMASK_OBJ, FALSE);
    GXS_SetVisibleWnd(GX_WNDMASK_OW);
  	GXS_SetVisiblePlane(GX_PLANEMASK_OBJ | GX_PLANEMASK_BG1);
  }
	
//		OS_SpinWait(10000);
	
#ifndef DEBUG_ERR_THROUGH
	while((PAD_Read() & ERR_DISP_END_BUTTON) != 0){
		Local_ErrUpdate();	//�{�^������x�����܂ő҂�
		_PushPopErrNetMain();
	}
	
	while((PAD_Read() & ERR_DISP_END_BUTTON) == 0){
		Local_ErrUpdate();	//�G���[��ʏI���{�^�����������܂ő҂�
		_PushPopErrNetMain();
	}
#endif //DEBUG_ERR_THROUGH
	
	//�G���[��ʏI��

  {
    BOOL is_black_continue  = (nes->pushpop_mode == NET_ERR_PUSHPOP_MODE_BLACKOUT);
    Local_ErrDispExit( is_black_continue );
  	
  	GFL_NET_WirelessIconOBJWinOFF();
  	if(GFL_NET_WirelessIconGetVramType() == NNS_G2D_VRAM_TYPE_2DSUB){
      //���C����Local_ErrDispExit�ŕ��A���Ă���ׁASub�������A
    	GXS_SetGraphicsMode(sub_dispcnt.bgMode);
    	GXS_SetVisiblePlane(sub_dispcnt.visiblePlane);
    	GXS_SetVisibleWnd(sub_dispcnt.visibleWnd);
    	G2S_SetWndOutsidePlane(gxs_outside.planeMask, gxs_outside.effect);
    	G2S_SetWndOBJInsidePlane(gxs_inside.planeMask, gxs_inside.effect);
  	}
  	else{
    	G2_SetWndOutsidePlane(gx_outside.planeMask, gx_outside.effect);
    	G2_SetWndOBJInsidePlane(gx_inside.planeMask, gx_inside.effect);
    }
  }
}

//==================================================================
/**
 * Push,Pop�L�̃G���[��ʈꔭ�Ăяo���@��FatalError��p
 *
 * �G���[��ʂ�\�������ꍇ�A�d���ؒf�𑣂����߁A���̊֐����Ŗ������[�v���܂��B
 */
//==================================================================
void NetErr_DispCallFatal(void)
{
	if(Local_SystemOccCheck() == FALSE){
		GF_ASSERT(0); //�V�X�e��������Ă��Ȃ�
		return;
	}

	//�ʐM�ؒf
//	NetErr_ExitNetSystem();

	//�G���[��ʕ`��
	Local_ErrDispInit(TRUE,FALSE);
	
//		OS_SpinWait(10000);

  //�������[�v
  GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);  //�n�[�h���Z�b�g�������悤�Ƀt���O�𗎂Ƃ�
  do{
    MachineSystem_Main(); //�n�[�h���Z�b�g�p
  }while(1);
	
	//�G���[��ʏI��
	Local_ErrDispExit(FALSE);
}


//--------------------------------------------------------------
/**
 * @brief   �G���[�����������炱�̊֐����g�p���ăG���[��ʂ��Ăяo��
 *
 * @param   none		��X�A�G���[�������Ă��炤
 */
//--------------------------------------------------------------
void NetErr_ErrorSet(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;

	if(Local_SystemOccCheck() == FALSE || nes->status != NET_ERR_STATUS_NULL){
		return;
	}
	nes->status = NET_ERR_STATUS_ERROR;

  if( GFL_NET_IsExit() )
  { 
    GF_ASSERT_MSG( 0, "�l�b�g���I�����Ă���̂ɁA�G���[�����o����܂���\n" );
  }
  else
  { 
    const GFLNetInitializeStruct* net_init  = GFL_NET_GetNETInitStruct();
    nes->net_type     = net_init->bNetType;
    nes->wifi_error   = *GFL_NET_StateGetWifiError();
    nes->error        = GFL_NET_StateGetError();
    
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    if( nes->net_type == GFL_NET_TYPE_WIFI
        || nes->net_type == GFL_NET_TYPE_WIFI_LOBBY
        || nes->net_type == GFL_NET_TYPE_WIFI_GTS )
    { 

      //WIFI�Ȃ�΁A�y�x�Əd�x�𔻒�
      if( nes->wifi_error.errorType == DWC_ETYPE_LIGHT
          || nes->wifi_error.errorType == DWC_ETYPE_SHOW_ERROR )
      { 
        nes->err_important_type  = NET_ERR_CHECK_LIGHT;
      }
      else
      { 
        nes->err_important_type  = NET_ERR_CHECK_HEAVY;
      }
    }
    else
    {
      nes->err_important_type  = NET_ERR_CHECK_LIGHT;
    }
#else //MB�q�@����
    //��ɏd�x�̃G���[
    nes->err_important_type  = NET_ERR_CHECK_HEAVY;
#endif  //MULTI_BOOT_MAKE
  }
}

//--------------------------------------------------------------
/**
 * @brief   �G���[�����������炱�̊֐����g�p���ăG���[��ʂ��Ăяo��
 *
 * @param   none		��X�A�G���[�������Ă��炤
 */
//--------------------------------------------------------------
#if PM_DEBUG
void NetErr_DEBUG_ErrorSet(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;

  //WiFi
#if defined( DEBUG_ONLY_FOR_ohno ) | defined( DEBUG_ONLY_FOR_matsuda ) | defined( DEBUG_ONLY_FOR_toru_nagihashi ) | defined( DEBUG_ONLY_FOR_ariizumi_nobuhiko ) | defined( DEBUG_ONLY_FOR_shimoyamada )

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  if( GFL_NET_IsWifiConnect() )
  {
    GFL_NET_StateSetWifiError( -30000, DWC_ETYPE_DISCONNECT, 1, 0 );
  }
#endif //MULTI_BOOT_MAKE

  NetErr_ErrorSet();
  OS_TPrintf( "���[�U�[����G���[�ݒ胊�N�G�X�g���������܂���\n" );
#endif


}
#endif  //PM_DEBUG

//--------------------------------------------------------------
/**
 * @brief   �A�v���p�F�G���[���������������ׂ�
 *
 * @retval  NET_ERR_CHECK
 *
 * �A�v�����͂��̊֐����g�p���ăG���[���������Ă��邩���ׁA
 * �G���[���������Ă����ꍇ�́A�e�A�v�����̃G���[�p�����ֈڍs���Ă�������
 */
//--------------------------------------------------------------
NET_ERR_CHECK NetErr_App_CheckError(void)
{
  NET_ERR_SYSTEM *nes = &NetErrSystem;
  if(nes->status != NET_ERR_STATUS_NULL)
  {
    return nes->err_important_type;
  }
  return NET_ERR_CHECK_NONE;
}

//==================================================================
/**
 * �A�v���p�FNetBit�w��Őڑ����ێ��ł��Ă��邩�`�F�b�N
 *
 * @param   net_bit		      �ڑ������Ώۂ�NetID��bit�Ŏw��
 *                          ��NetID 0�Ԃ�3�Ԃ��`�F�b�N����̂ł���� ( (1 << 0) | (1 << 3) )
 *
 * @retval  NET_ERR_CHECK		
 *
 * �r�����E��OK�ɂ��Ă��Ȃ��̂ł���΁A���̃`�F�b�N�͗v��܂���B
 * �r�����E��OK�ɂ��Ă��āA�ꏏ�ɉ��炩�̃Q�[�����v���C���Ă��āA
 * ���̑��肪���Ȃ��Ȃ�����I������K�v������ꍇ�ɂ̂ݎg���K�v������܂��B
 *
 * 2010.03.03(��)���݂̏��A�N�����̒ʐM�ΐ�̂�
 */
//==================================================================
NET_ERR_CHECK NetErr_App_CheckConnectError(u32 net_bit)
{
  NetID netID;
  
  for( netID = 0; netID < GFL_NET_MACHINE_MAX; netID++ )
  {
    if( net_bit & (1 << netID) ) //�`�F�b�N�Ώۂ̃v���C���[�ł���
    {
      if(GFL_NET_IsConnectMember(netID) == FALSE)
      {
        NetErr_ErrorSet();
        return NET_ERR_CHECK_HEAVY;   //�ʐM���肪�ؒf���Ă���ׁA�G���[
      }
    }
  }
  
  return NET_ERR_CHECK_NONE;
}

//==================================================================
/**
 * �A�v���p�F�G���[��ʕ\�����N�G�X�g
 */
//==================================================================
void NetErr_App_ReqErrorDisp(void)
{
  //GF_ASSERT_MSG(NetErrSystem.status == NET_ERR_STATUS_ERROR, "status=%d\n", NetErrSystem.status);
  if( NetErrSystem.status == NET_ERR_STATUS_ERROR )
  { 
    NetErrSystem.status = NET_ERR_STATUS_REQ;
    OS_TPrintf("NetErr App ReqErrDisp\n");
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����G���[��ʕ\�����N�G�X�g  
 *	        WIFI�ł�DWC�o�R�ŃG���[�������o���Ă��܂����A
 *	        ���C�u�����̂Ȃ��ɂ�DWC�̃G���[�ɂȂ炸�A���C�u�����̌ŗL�̃G���[��
 *	        �����ꍇ������A���̂Ƃ��ɏ�LNetErr_App_ReqErrorDisp��ǂ�ł�
 *	        �����Ă��Ă��܂��̂ŁA�G���[��ԂłȂ��Ƃ����N�G�X�g�ł���֐���p�ӂ��܂����B
 *
 *	@param	msg ���b�Z�[�W�ԍ�
 */
//-----------------------------------------------------------------------------
void NetErr_App_ReqErrorDispForce(int message )
{
  if( NetErrSystem.status != NET_ERR_STATUS_REQ )
  { 
    NetErrSystem.wifi_msg = message;
    NetErrSystem.msg_force = TRUE;
    NetErrSystem.net_type = GFL_NET_TYPE_WIFI;
    NetErrSystem.status = NET_ERR_STATUS_REQ;
    OS_TPrintf("NetErr App ReqErrDisp Force\n");
  }
}

//--------------------------------------------------------------
/**
 * @brief   �O�ɑޔ�p�A�h���X��n��
 */
//--------------------------------------------------------------
void NetErr_GetTempArea( u8** charArea , u16** scrnArea , u16** plttArea )
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
  *charArea = nes->push_char_p;
  *scrnArea = nes->push_scrn_p;
  *plttArea = nes->push_pltt_p;
}

//--------------------------------------------------------------
/**
 * �G���[�������[�N���N���A
 *
 * @param   none		
 */
//--------------------------------------------------------------
void NetErr_ErrWorkInit(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	nes->status = NET_ERR_STATUS_NULL;
  nes->err_important_type = NET_ERR_CHECK_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ؒf
 *
 */
//-----------------------------------------------------------------------------
void NetErr_ExitNetSystem( void )
{ 
  if(GFL_NET_IsInit() )
  {
    GFL_NET_ResetDisconnect();  ///�ؒf�������ł���U���Z�b�g
    GFL_NET_Exit(NULL);
    GFL_NET_IRCWIRELESS_ResetSystemError();  //�ԊO��WIRLESS�ؒf
    do{
      GFL_NET_ShutdownMain();
      Local_ErrUpdate();
      //OS_WaitIrq(TRUE, OS_IE_V_BLANK);
//      OS_TPrintf("GFL_NET_IsExit�̊�����҂��Ă��܂�\n");
    }while(GFL_NET_IsExit() == FALSE);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʐ��䃁�C��
 *
 * @param   fatal_error   TRUE:���Z�b�g�𑣂��G���[(A�Ŕ������Ȃ�) 
 * 
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL NetErr_DispMain(BOOL fatal_error)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	
	if(Local_SystemOccCheck() == FALSE){
		return FALSE;
	}
  
  if(fatal_error == TRUE){
  	//�ʐM�ؒf
  	//NetErr_ExitNetSystem();
		//�G���[��ʕ`��
  	Local_ErrDispInit(fatal_error,FALSE);
    //�������[�v
    GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);  //�n�[�h���Z�b�g�������悤�Ƀt���O�𗎂Ƃ�
    do{
      MachineSystem_Main(); //�n�[�h���Z�b�g�p
    }while(1);
	}
  
	if(nes->status == NET_ERR_STATUS_REQ)
	{
    BOOL isWifi = FALSE;
    if( nes->net_type == GFL_NET_TYPE_WIFI 
        || nes->net_type == GFL_NET_TYPE_WIFI_LOBBY 
        || nes->net_type == GFL_NET_TYPE_WIFI_GTS )
    {
      isWifi = TRUE;
    }
    
    if( isWifi == TRUE )
    {
  		//�G���[��ʕ`��(�ؒf�҂��\��
  		Local_ErrDispInit(fatal_error,TRUE);
  	}

    //�ʐM���C�u�����I���҂� 
    NetErr_ExitNetSystem();

    if( isWifi == TRUE )
    {
  		//�G���[��ʕ`��(�����̏��������̂�
    	nes->h_intr = GX_HBlankIntr(FALSE);
    	nes->v_intr = GX_VBlankIntr(FALSE);
    	Local_ErrMessagePrint(fatal_error,FALSE);
    }
    else
    {
  		//�G���[��ʕ`��
  		Local_ErrDispInit(fatal_error,FALSE);
    }
		//Local_ErrDispInit(fatal_error,FALSE);
		
//		OS_SpinWait(10000);
		
#ifdef MULTI_BOOT_MAKE  //�}���`�u�[�g����
		while( TRUE )
		{
      Local_ErrUpdate(); //��ɔ�����Ȃ�
    }

#endif


#ifndef DEBUG_ERR_THROUGH
		while((PAD_Read() & ERR_DISP_END_BUTTON) != 0){
			Local_ErrUpdate();	//�{�^������x�����܂ő҂�
		}
		
		while((PAD_Read() & ERR_DISP_END_BUTTON) == 0){
      Local_ErrUpdate();	//�G���[��ʏI���{�^�����������܂ő҂�
		}
#endif //DEBUG_ERR_THROUGH
		
		//�G���[��ʏI��
		Local_ErrDispExit(FALSE);
		NetErr_ErrWorkInit();
		nes->key_timer = 0;
  	return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��ʏ���ޔ����A�G���[��ʂ�\��
 *
 * @param   fatal_error   TRUE:���Z�b�g�𑣂��G���[(A�Ŕ������Ȃ�) 
 * @param   isExitWait    TRUE:�ؒf�҂���\������
 * 
 */
//--------------------------------------------------------------
static void Local_ErrDispInit(BOOL fatal_error,BOOL isExitWait)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	
	if(Local_SystemOccCheck() == FALSE){
		return;
	}
	
	//�����������ʂ������Ȃ��悤�ɁA�܂��}�X�^�[�P�x��ޔ����A�^���Âɐݒ�
	nes->master_brightness = GX_GetMasterBrightness();
	nes->master_brightness_sub = GXS_GetMasterBrightness();
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	
	//���荞�݂��~�߂�
	if( isExitWait == FALSE )
	{
  	nes->h_intr = GX_HBlankIntr(FALSE);
  	nes->v_intr = GX_VBlankIntr(FALSE);
  }
	
	//VRAM�o���N���ޔ�
	nes->bg_bank = GX_ResetBankForBG();
	nes->bg_bank_sub = GX_ResetBankForSubBG();
	GX_SetBankForBG(GX_VRAM_BG_128_C);
	OS_TPrintf("VRAM BANK = %d\n", nes->bg_bank);

	//dispcnt�ޔ�
	nes->dispcnt = GX_GetDispCnt();
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG1);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);

  nes->sub_wnd  = GXS_GetVisibleWnd();
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);

	//BG1Control�ޔ�
	nes->bg1cnt = G2_GetBG1Control();
	G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, 
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, nes->bg1cnt.bgExtPltt);
	G2_BG1Mosaic(FALSE);
	G2_SetBG1Offset(0, 0);

	//VRAM�̃f�[�^��ޔ�(�O�̂��ߏ��BG1�̐ݒ�����Ă���s���Ă���)
	GFL_STD_MemCopy16(G2_GetBG1CharPtr(), nes->push_char_p, NETERR_PUSH_CHARVRAM_SIZE);
	GFL_STD_MemCopy16(G2_GetBG1ScrPtr(), nes->push_scrn_p, NETERR_PUSH_SCRNVRAM_SIZE);
	GFL_STD_MemCopy16((void*)HW_PLTT, nes->push_pltt_p, NETERR_PUSH_PLTTVRAM_SIZE);

	//�擾�֐����Ȃ��̂Ń��W�X�^����擾���Ă���
  nes->g2_blend_cnt  = reg_G2_BLDCNT;
  nes->g2_blend_cnt  = reg_G2S_DB_BLDCNT;
	G2_BlendNone();
	G2S_BlendNone();

  //����ʂ̑Ҕ�
  nes->sub_visible = GFL_DISP_GetSubVisible();
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );

  //����ʂ̓o�b�N�h���b�v���g���̂őҔ�
  nes->sub_bd_color  = *((u16 *)HW_DB_BG_PLTT);
  *((u16 *)HW_DB_BG_PLTT)  = 0x7eea;

	//�G���[��ʕ`��
	Local_ErrDispDraw();
	Local_ErrMessagePrint(fatal_error,isExitWait);

	//�\��ON
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);
}

//--------------------------------------------------------------
/**
 * @brief   �G���[��ʂ��I�������A���̉�ʂɕ��A������
 */
//--------------------------------------------------------------
static void Local_ErrDispExit(BOOL is_black_continue)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	int x, y;

	if(Local_SystemOccCheck() == FALSE){
		return;
	}
	
	//�����������ʂ������Ȃ��悤�ɁA�^���Âɐݒ�
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	
  //����ʂ̓o�b�N�h���b�v���g���̂őҔ�
  *((u16 *)HW_DB_BG_PLTT)  = nes->sub_bd_color;

  //����ʂ̕`��ݒ�
  GFL_DISP_GXS_SetVisibleControlDirect( nes->sub_visible );

  //�u�����h�̐ݒ�
  reg_G2_BLDCNT     = nes->g2_blend_cnt;
  reg_G2S_DB_BLDCNT = nes->g2s_blend_cnt;

	//VRAM���A
	GFL_STD_MemCopy16(nes->push_char_p, G2_GetBG1CharPtr(), NETERR_PUSH_CHARVRAM_SIZE);
	GFL_STD_MemCopy16(nes->push_scrn_p, G2_GetBG1ScrPtr(), NETERR_PUSH_SCRNVRAM_SIZE);
	GFL_STD_MemCopy16(nes->push_pltt_p, (void*)HW_PLTT, NETERR_PUSH_PLTTVRAM_SIZE);

	//BG1Control���A
	G2_SetBG1Control(nes->bg1cnt.screenSize, nes->bg1cnt.colorMode,
		nes->bg1cnt.screenBase, nes->bg1cnt.charBase, nes->bg1cnt.bgExtPltt);
	G2_BG1Mosaic(nes->bg1cnt.mosaic);
	GFL_NET_BG1PosGet(&x, &y);
	G2_SetBG1Offset(x, y);
	
	//dispcnt���A
	GX_SetGraphicsMode(nes->dispcnt.dispMode, nes->dispcnt.bgMode, nes->dispcnt.bg0_2d3d);
	GX_SetVisiblePlane(nes->dispcnt.visiblePlane);
	GX_SetVisibleWnd(nes->dispcnt.visibleWnd);

  GXS_SetVisibleWnd(nes->sub_wnd);

	//VRAM�o���N��񕜋A
  GX_ResetBankForSubBG();
	GX_ResetBankForBG();
	GX_SetBankForBG(nes->bg_bank);
	GX_SetBankForSubBG(nes->bg_bank_sub);

	//���荞�ݕ��A
	GX_HBlankIntr(nes->h_intr);
	GX_VBlankIntr(nes->v_intr);
	
	//�\��ON
  OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  if( !is_black_continue )
  {
    GX_SetMasterBrightness(nes->master_brightness);
    GXS_SetMasterBrightness(nes->master_brightness_sub);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʐ���V�X�e�����L�������ׂ�
 *
 * @retval  TRUE:�L���AFALSE:����
 */
//--------------------------------------------------------------
static BOOL Local_SystemOccCheck(void)
{
	if(NetErrSystem.push_char_p != NULL){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �G���[��ʕ`��
 */
//--------------------------------------------------------------
static void Local_ErrDispDraw(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	u32 data_size;
	void *arcData;
	NNSG2dCharacterData* charData;
	NNSG2dScreenData* scrnData;
	NNSG2dPaletteData* palData;
	BOOL  cmpFlag;
	NNSG2dPaletteCompressInfo*  cmpInfo;
	
	GFL_STD_MemClear32(G2_GetBG1CharPtr(), NETERR_PUSH_CHARVRAM_SIZE);

	//�L�����N�^
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NCGR, 0, HEAPID_NET_TEMP);
	if(NNS_G2dGetUnpackedBGCharacterData(arcData, &charData)){
		DC_FlushRange(charData->pRawData, charData->szByte);
		//�O�̂���DMA���g�p����͔̂�����
		GFL_STD_MemCopy16(charData->pRawData, G2_GetBG1CharPtr(), charData->szByte);
		OS_TPrintf("�G���[��ʁF�L�����N�^�T�C�Y��%dbyte\n", charData->szByte);
		GF_ASSERT(MESSAGE_START_CHARNO*0x20 >= charData->szByte);	//����ASSERT�Ɉ���������ꍇ��BG_DATA_SIZE��傫������K�v������B���̕��G���[���b�Z�[�W�Ɋ�����L�����N�^�̈悪���Ȃ��Ȃ�̂Œ���
	}
	GFL_HEAP_FreeMemory(arcData);
	
	//�X�N���[��
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NSCR, 0, HEAPID_NET_TEMP);
	if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) ){
		DC_FlushRange(scrnData->rawData, scrnData->szByte);
		GFL_STD_MemCopy16(scrnData->rawData, G2_GetBG1ScrPtr(), scrnData->szByte);
	}
	GFL_HEAP_FreeMemory( arcData );
	
	//�p���b�g
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NCLR, 0, HEAPID_NET_TEMP);
	cmpFlag = NNS_G2dGetUnpackedPaletteCompressInfo( arcData, &cmpInfo );
	if( NNS_G2dGetUnpackedPaletteData( arcData, &palData ) ){
		DC_FlushRange( palData->pRawData, NETERR_PUSH_PLTTVRAM_SIZE );
		GFL_STD_MemCopy16(palData->pRawData, (void*)HW_BG_PLTT, NETERR_PUSH_PLTTVRAM_SIZE);
	}
	GFL_HEAP_FreeMemory( arcData );
}


//--------------------------------------------------------------
/**
 * @brief   WIFI�G���[���b�Z�[�W�擾
 * @param   fatal_error   TRUE:���Z�b�g�𑣂��G���[(A�Ŕ������Ȃ�) 
 * 
 */
//--------------------------------------------------------------
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
static const u16 twlerror[]={
dwctwl_error_0001,
dwc_error_0002,
dwctwl_error_0003,
dwc_error_0004,
dwc_error_0005,
dwc_error_0006,
dwctwl_error_0007,
dwc_error_0008,
dwctwl_error_0009,
dwc_error_0010,
dwc_error_0011,
dwc_error_0012,
dwc_error_0013,
dwc_error_0014,
dwc_error_0015,
dwctwl_error_0016,
};

static const u16 nitroerror[]={
dwc_error_0001,
dwc_error_0002,
dwc_error_0003,
dwc_error_0004,
dwc_error_0005,
dwc_error_0006,
dwc_error_0007,
dwc_error_0008,
dwc_error_0009,
dwc_error_0010,
dwc_error_0011,
dwc_error_0012,
dwc_error_0013,
dwc_error_0014,
dwc_error_0015,
dwc_error_0016,
};
#endif //MULTI_BOOT_MAKE  //�ʏ펞����


static u32 _wifierrMessage(GFL_NETSTATE_DWCERROR* pErr, int default_msg)
{
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  u32 msgno = default_msg;
  switch(pErr->errorUser){
  case ERRORCODE_HEAP:
    return dwc_error_0014;
  case ERRORCODE_DISCONNECT:
    return dwc_message_0022;
  case ERRORCODE_CRC:
  case ERRORCODE_SYSTEM:
  case ERRORCODE_SENDQUEUE:
  case ERRORCODE_USER_TIMEOUT:
  case ERRORCODE_NHTTP:
    return dwc_error_0015;
  case ERRORCODE_TIMEOUT:
    return dwc_message_0022;
  }
  if(11 == NetErrSystem.wifi_msg){
    return dwc_error_0015;
  }
  if( NetErrSystem.wifi_msg >= 0 )
  {
    if(DS_SYSTEM_IsRunOnTwl()){
      msgno = twlerror[NetErrSystem.wifi_msg];
    }
    else{
      msgno = nitroerror[NetErrSystem.wifi_msg];
    }
  }
  else
  {
    msgno = dwc_error_0012;
  }
  return msgno;
#else
  return 0;
#endif
}


//--------------------------------------------------------------
/**
 * @brief   �G���[���b�Z�[�W�\��
 *
 * @param   fatal_error   TRUE:���Z�b�g�𑣂��G���[(A�Ŕ������Ȃ�) 
 * 
 */
//--------------------------------------------------------------
static void Local_ErrMessagePrint(BOOL fatal_error,BOOL isExitWait)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	GFL_BMP_DATA *bmpdata;
	int x, y;
	u16 *scrn_vram;
	u16 data;
	u16 *test;
	
	//�X�N���[����BMP�̈�̌`�Ŗ��ߐs����
	scrn_vram = G2_GetBG1ScrPtr();
	data = MESSAGE_START_CHARNO | (MESSAGE_PALNO << 12);
	for(y = MESSAGE_START_Y; y < MESSAGE_START_Y+MESSAGE_Y_LEN; y++){
		for(x = MESSAGE_START_X; x < MESSAGE_START_X+MESSAGE_X_LEN; x++){
			scrn_vram[y*0x20+x] = data;
			data++;
		}
	}

	//BMP�L�����N�^�̈�𕶎��w�i�F�Ŗ��߂�
	GFL_STD_MemFill16((void*)((u32)G2_GetBG1CharPtr() + MESSAGE_START_CHARNO*0x20), 
	  0x7777, MESSAGE_X_LEN * MESSAGE_Y_LEN * 32);
	
	//BMP�쐬
	bmpdata = GFL_BMP_CreateInVRAM((void*)((u32)G2_GetBG1CharPtr() + MESSAGE_START_CHARNO*0x20), 
		MESSAGE_X_LEN, MESSAGE_Y_LEN, GFL_BMP_16_COLOR, HEAPID_NET_TEMP);
	
	//���b�Z�[�WOPEN
	{
		GFL_FONT		*fontHandle;
		GFL_MSGDATA		*mm;
		STRBUF  *strbuf;
    STRBUF  *src;
    WORDSET       *wordset;
    int msgno;

    wordset = WORDSET_Create( HEAPID_NET_TEMP );


		fontHandle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NET_TEMP );
		

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    {
      if( isExitWait == TRUE )
      {
        //�ؒf�҂�
        mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
  			ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_NET_TEMP);
  			msgno = WIFI_DISCONNECT_WAIT;
      }
      else
      if( nes->net_type == GFL_NET_TYPE_WIFI 
          || nes->net_type == GFL_NET_TYPE_WIFI_LOBBY 
          || nes->net_type == GFL_NET_TYPE_WIFI_GTS )
      { 
        //WIFI�̏ꍇ
        int type;
        
        WORDSET_RegisterNumber( wordset, 0, nes->wifi_error.errorCode, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

        mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
  			ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_NET_TEMP);

        if( nes->msg_force )
        {
          msgno = nes->wifi_msg;
          nes->msg_force  = FALSE;
          OS_TPrintf("�����G���[���b�Z�[�W %d \n",msgno);
        }
        else
        {
          msgno = _wifierrMessage(&nes->wifi_error, nes->wifi_msg) ;      //WIFI�ŕ\�����郁�b�Z�[�W���擾
          OS_TPrintf("�G���[���b�Z�[�W %d \n",msgno);
        }
      }
      else
      { 
        //���̑��̏ꍇ
        //���C�����X��ԊO���ł̃G���[��
        //�G���[���������Ă��Ȃ��̂�NetErr_DispCallFatal���Ăяo�����ꍇ
        WORDSET_RegisterNumber( wordset, 0, nes->error, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

        mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
            ARCID_MESSAGE, NARC_message_net_err_dat, HEAPID_NET_TEMP);

        if( fatal_error == TRUE )
        {
          msgno = net_error_0002;
        }
        else
        {
          msgno = net_error_0001;
        }
      }
    }
#else
    {
      //�}���`�u�[�g�ł͏�ɓd����؂��Ă��炤
      //MB_ERROR_MSG_FILE��Arc�����|�P�V�t�^�[�Ɖf��]���ňႤ�̂�mustiboot/src/net_err_dl.c�Œ�`
      WORDSET_RegisterNumber( wordset, 0, nes->error, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
          ARCID_MESSAGE, MB_ERROR_MSG_FILE, HEAPID_NET_TEMP);
      msgno = 1;
      
    }
#endif



    src     = GFL_MSG_CreateString(mm, msgno);
		strbuf  = GFL_MSG_CreateString(mm, msgno);
    WORDSET_ExpandStr( wordset, strbuf, src );
		PRINTSYS_PrintColor(bmpdata, 0, 0, strbuf, fontHandle, PRINTSYS_LSB_Make(4, 0xb, 7));

		GFL_STR_DeleteBuffer(strbuf);
		GFL_STR_DeleteBuffer(src);
		GFL_MSG_Delete(mm);
		
		GFL_FONT_Delete(fontHandle);

    WORDSET_Delete( wordset );
	}
	
	GFL_BMP_Delete(bmpdata);
}


//--------------------------------------------------------------
/**
 * @brief   �G���[�����X�V����K�v���������
 */
//--------------------------------------------------------------
static void Local_ErrUpdate(void)
{
  MachineSystem_Main(); //�n�[�h���Z�b�g�p

  //LCDOFF�Ή�
  GFL_UI_Main();
  //BGM���ԉ��т�����Ή�
  PMSND_Main();
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  PMVOICE_Main();
#endif //MULTI_BOOT_MAKE
}
