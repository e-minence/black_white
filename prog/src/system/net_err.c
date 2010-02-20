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
#include "arc_def.h"
#include "net_err.naix"

#include "print\wordset.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "font/font.naix"
#include "msg\msg_net_err.h"

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#include <dwc.h>
#include "net/dwc_rap.h"
#include "msg\msg_wifi_system.h"
#include "message.naix"
#else
#include "message_dl.naix"
#endif //MULTI_BOOT_MAKE

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
	GXVRamBG bg_bank;			///<BG VRAM�o���N���
	
	u8 font_letter;
	u8 font_shadow;
	u8 font_back;
	
	NET_ERR_STATUS status;		///<�G���[��ʃV�X�e���̏�
	s32 key_timer;				///<�L�[���͎�t�܂ł̃^�C�}�[

  //�ȉ��G���[���ۑ�
  GFL_NETSTATE_DWCERROR   wifi_error; //WIFI�̃G���[
  u8                      error;      //WIFI�ȊO�̃G���[
  u8                      net_type;   //�l�b�g�̃^�C�v
  u8                      dummy[2];
  NET_ERR_CHECK           err_important_type;  //�G���[�̏d�x�y�x����

}NET_ERR_SYSTEM;


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_ERR_SYSTEM NetErrSystem = {0};

static const int asd[]={1,2,3,4,5};

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL NetErr_DispMain(BOOL fatal_error);
static void Local_ErrDispInit(void);
static void Local_ErrDispExit(void);
static BOOL Local_SystemOccCheck(void);
static void Local_ErrDispDraw(void);
static void Local_ErrMessagePrint(void);


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
  int a;
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NET_ERR, NETERR_PUSH_CHARVRAM_SIZE + NETERR_PUSH_SCRNVRAM_SIZE + NETERR_PUSH_PLTTVRAM_SIZE + 0x100);  //+0x100�̓w�b�_��
  a=asd[0];
  a++;
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
void NetErr_SystemExit(void)
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
      OS_TPrintf("NetErr �G���[����\n");
      GFL_NET_DeleteInitializeCallback();
      NetErr_ErrorSet();
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
 * Push,Pop�L�̃G���[��ʈꔭ�Ăяo���@���y�x�G���[��p�B�ʐM�̏I�������͍s���܂���
 *
 * �G���[��ʂ�\�������ꍇ�A���[�U�[������̑�����s��Ȃ�����(A������)
 * ���̊֐����Ŗ������[�v���܂��B
 */
//==================================================================
void NetErr_DispCallPushPop(void)
{
	if(Local_SystemOccCheck() == FALSE){
		GF_ASSERT(0); //�V�X�e��������Ă��Ȃ�
		return;
	}

	//�G���[��ʕ`��
	Local_ErrDispInit();
	
//		OS_SpinWait(10000);
	
	while((PAD_Read() & ERR_DISP_END_BUTTON) != 0){
		;	//�{�^������x�����܂ő҂�
	}
	
	while((PAD_Read() & ERR_DISP_END_BUTTON) == 0){
		;	//�G���[��ʏI���{�^�����������܂ő҂�
	}
	
	//�G���[��ʏI��
	Local_ErrDispExit();
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

	//�G���[��ʕ`��
	Local_ErrDispInit();
	
//		OS_SpinWait(10000);
	
	while(1){
		;	//�d���ؒf���܂�
	}
	
	//�G���[��ʏI��
	Local_ErrDispExit();
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
    
    if( nes->net_type == GFL_NET_TYPE_WIFI
        || nes->net_type == GFL_NET_TYPE_WIFI_LOBBY
        || nes->net_type == GFL_NET_TYPE_WIFI_GTS )
    { 

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
      //WIFI�Ȃ�΁A�y�x�Əd�x�𔻒�
      if( nes->wifi_error.errorType == DWC_ETYPE_LIGHT
          || nes->wifi_error.errorType == DWC_ETYPE_SHOW_ERROR )
      { 
        nes->err_important_type  = NET_ERR_CHECK_LIGHT;
      }
      else
#endif  //MULTI_BOOT_MAKE
      { 
        nes->err_important_type  = NET_ERR_CHECK_HEAVY;
      }
    }
    else
    {
      //����ȊO�͏d�x�����Ȃ�
      nes->err_important_type  = NET_ERR_CHECK_HEAVY;
    }
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
#if defined( DEBUG_ONLY_FOR_ohno ) | defined( DEBUG_ONLY_FOR_matsuda ) | defined( DEBUG_ONLY_FOR_toru_nagihashi )
  GFL_NET_StateSetWifiError( 1, 1, 1 );
  NetErr_ErrorSet();
#endif
  OS_TPrintf( "���[�U�[����G���[�ݒ胊�N�G�X�g���������܂���\n" );


}
#endif  //PM_DEBUG

//--------------------------------------------------------------
/**
 * @brief   �A�v���p�F�G���[���������������ׂ�
 *
 * @retval  TRUE:�G���[����
 * @retval  FALSE:���퓮��
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
 * �A�v���p�F�G���[��ʕ\�����N�G�X�g
 */
//==================================================================
void NetErr_App_ReqErrorDisp(void)
{
  GF_ASSERT(NetErrSystem.status == NET_ERR_STATUS_ERROR);
  NetErrSystem.status = NET_ERR_STATUS_REQ;
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
		//�G���[��ʕ`��
  	Local_ErrDispInit();
		while(1){}
	}
  
	if(nes->status == NET_ERR_STATUS_REQ){
    //�ʐM���C�u�����I���҂�  ��check�@�y�x�G���[�̏ꍇ�͏I�����������Ȃ�
    GFL_NET_Exit(NULL);
    do{
      GFL_NET_Main();
      OS_TPrintf("GFL_NET_IsExit�̊�����҂��Ă��܂�\n");
    }while(GFL_NET_IsExit() == FALSE);

		//�G���[��ʕ`��
		Local_ErrDispInit();
		
//		OS_SpinWait(10000);
		
		while((PAD_Read() & ERR_DISP_END_BUTTON) != 0){
			;	//�{�^������x�����܂ő҂�
		}
		
		while((PAD_Read() & ERR_DISP_END_BUTTON) == 0){
			;	//�G���[��ʏI���{�^�����������܂ő҂�
		}
		
		//�G���[��ʏI��
		Local_ErrDispExit();
		nes->status = NET_ERR_STATUS_NULL;
    nes->err_important_type = NET_ERR_CHECK_NONE;
		nes->key_timer = 0;
  	return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ��ʏ���ޔ����A�G���[��ʂ�\��
 */
//--------------------------------------------------------------
static void Local_ErrDispInit(void)
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
	nes->h_intr = GX_HBlankIntr(FALSE);
	nes->v_intr = GX_VBlankIntr(FALSE);
	
	//VRAM�o���N���ޔ�
	nes->bg_bank = GX_ResetBankForBG();
	GX_SetBankForBG(GX_VRAM_BG_128_C);
	OS_TPrintf("VRAM BANK = %d\n", nes->bg_bank);

	//dispcnt�ޔ�
	nes->dispcnt = GX_GetDispCnt();
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG1);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	
	//VRAM�̃f�[�^��ޔ�(�O�̂��ߏ��BG1�̐ݒ�����Ă���s���Ă���)
	GFL_STD_MemCopy16(G2_GetBG1CharPtr(), nes->push_char_p, NETERR_PUSH_CHARVRAM_SIZE);
	GFL_STD_MemCopy16(G2_GetBG1ScrPtr(), nes->push_scrn_p, NETERR_PUSH_SCRNVRAM_SIZE);
	GFL_STD_MemCopy16((void*)HW_PLTT, nes->push_pltt_p, NETERR_PUSH_PLTTVRAM_SIZE);

	//BG1Control�ޔ�
	nes->bg1cnt = G2_GetBG1Control();
	G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, 
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, nes->bg1cnt.bgExtPltt);
	G2_BG1Mosaic(FALSE);
	G2_SetBG1Offset(0, 0);

	//�t�H���g�J���[�ޔ�
	GFL_FONTSYS_GetColor(&nes->font_letter, &nes->font_shadow, &nes->font_back);
	GFL_FONTSYS_SetColor(4, 0xb, 7);
	
	//��check�@�ޔ��ł��Ȃ�����
	G2_BlendNone(); //WriteOnly�ׁ̈A���W�X�^�ޔ����o���Ă��Ȃ�
	
	//�G���[��ʕ`��
	Local_ErrDispDraw();
	Local_ErrMessagePrint();
	
	//�\��ON
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(16);	//�T�u��ʂ͐^����
}

//--------------------------------------------------------------
/**
 * @brief   �G���[��ʂ��I�������A���̉�ʂɕ��A������
 */
//--------------------------------------------------------------
static void Local_ErrDispExit(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	int x, y;
	
	if(Local_SystemOccCheck() == FALSE){
		return;
	}
	
	//�����������ʂ������Ȃ��悤�ɁA�^���Âɐݒ�
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	
	//�t�H���g�J���[���A
	GFL_FONTSYS_SetColor(nes->font_letter, nes->font_shadow, nes->font_back);

	//BG1Control���A
	G2_SetBG1Control(nes->bg1cnt.screenSize, nes->bg1cnt.colorMode,
		nes->bg1cnt.screenBase, nes->bg1cnt.charBase, nes->bg1cnt.bgExtPltt);
	G2_BG1Mosaic(nes->bg1cnt.mosaic);
	GFL_NET_BG1PosGet(&x, &y);
	G2_SetBG1Offset(x, y);

	//VRAM���A
	GFL_STD_MemCopy16(nes->push_char_p, G2_GetBG1CharPtr(), NETERR_PUSH_CHARVRAM_SIZE);
	GFL_STD_MemCopy16(nes->push_scrn_p, G2_GetBG1ScrPtr(), NETERR_PUSH_SCRNVRAM_SIZE);
	GFL_STD_MemCopy16(nes->push_pltt_p, (void*)HW_PLTT, NETERR_PUSH_PLTTVRAM_SIZE);
	
	//dispcnt���A
	GX_SetGraphicsMode(nes->dispcnt.dispMode, nes->dispcnt.bgMode, nes->dispcnt.bg0_2d3d);
	GX_SetVisiblePlane(nes->dispcnt.visiblePlane);
	GX_SetVisibleWnd(nes->dispcnt.visibleWnd);

	//VRAM�o���N��񕜋A
	GX_ResetBankForBG();
	GX_SetBankForBG(nes->bg_bank);

	//���荞�ݕ��A
	GX_HBlankIntr(nes->h_intr);
	GX_VBlankIntr(nes->v_intr);
	
	//�\��ON
	GX_SetMasterBrightness(nes->master_brightness);
	GXS_SetMasterBrightness(nes->master_brightness_sub);
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
 * @brief   �G���[���b�Z�[�W�\��
 */
//--------------------------------------------------------------
static void Local_ErrMessagePrint(void)
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
    if( nes->net_type == GFL_NET_TYPE_WIFI 
        || nes->net_type == GFL_NET_TYPE_WIFI_LOBBY 
        || nes->net_type == GFL_NET_TYPE_WIFI_GTS )
    { 
      int type;
      //WIFI�̏ꍇ
      
      WORDSET_RegisterNumber( wordset, 0, nes->wifi_error.errorCode, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

      mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
			ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_NET_TEMP);

      //WIFI�ŕ\�����郁�b�Z�[�W���擾
      type    = GFL_NET_DWC_ErrorType( nes->wifi_error.errorCode, nes->wifi_error.errorType);
      if(type == 11){
        msgno = dwc_error_0015;
        type = 11;
      }
      else if(nes->wifi_error.errorCode == ERRORCODE_HEAP)
      {
        msgno = dwc_error_0014;
        type = 12;
      }
      else
      {
        if( type >= 0 )
        {
          msgno = dwc_error_0001 + type;
        }
        else
        {
          msgno = dwc_error_0012;
        }
      }
      OS_TPrintf("�G���[���b�Z�[�W %d \n",msgno);
    }
    else
    { 
      //���̑��̏ꍇ
      WORDSET_RegisterNumber( wordset, 0, nes->error, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

      mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
          ARCID_MESSAGE, NARC_message_net_err_dat, HEAPID_NET_TEMP);

      switch( nes->error )
      { 
        //�v���I�ł͂Ȃ��G���[
      case WM_ERRCODE_NO_KEYSET:
      //case WM_ERRCODE_NO_DATASET: //WM_ERRCODE_NO_KEYSET�Ɠ����l
      case WM_ERRCODE_NO_CHILD:
      case WM_ERRCODE_TIMEOUT:
      case WM_ERRCODE_SEND_QUEUE_FULL:
      case WM_ERRCODE_INVALID_POLLBITMAP:
      case WM_ERRCODE_SEND_FAILED:
        msgno = 0;
        break;

      default:
        msgno = 1;
      }
    }
#else
    {
      //�}���`�u�[�g�ł͏�ɓd����؂��Ă��炤
      WORDSET_RegisterNumber( wordset, 0, nes->error, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
          ARCID_MESSAGE, NARC_message_dl_net_err_dat, HEAPID_NET_TEMP);
      msgno = 1;
      
    }
#endif



    src     = GFL_MSG_CreateString(mm, msgno);
		strbuf  = GFL_MSG_CreateString(mm, msgno);
    WORDSET_ExpandStr( wordset, strbuf, src );
		PRINTSYS_Print(bmpdata, 0, 0, strbuf, fontHandle);

		GFL_STR_DeleteBuffer(strbuf);
		GFL_STR_DeleteBuffer(src);
		GFL_MSG_Delete(mm);
		
		GFL_FONT_Delete(fontHandle);

    WORDSET_Delete( wordset );
	}
	
	GFL_BMP_Delete(bmpdata);
}
