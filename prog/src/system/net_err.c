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

#include "print\printsys.h"
#include "print\gf_font.h"
#include "message.naix"
#include "font/font.naix"
#include "msg\msg_d_matsu.h"


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
}NET_ERR_SYSTEM;


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_ERR_SYSTEM NetErrSystem = {0};

static const int asd[]={1,2,3,4,5};

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL NetErr_DispMain(void);
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
      NetErr_ErrorSet();
    }
  }
}

//==================================================================
/**
 * �G���[���������Ă���΃G���[��ʂ��ꔭ�Ăяo��
 *
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 * 
 * �G���[��ʂ�\�������ꍇ�A���[�U�[������̑�����s��Ȃ�����(A������)
 * ���̊֐����Ŗ������[�v���܂��B
 */
//==================================================================
BOOL NetErr_DispCall(void)
{
  return NetErr_DispMain();
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
	nes->status = NET_ERR_STATUS_ERROR;
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
BOOL NetErr_App_CheckError(void)
{
	if(NetErrSystem.status != NET_ERR_STATUS_NULL){
    return TRUE;
  }
  return FALSE;
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
 * @retval  TRUE:�G���[��ʂ��Ăяo����
 * @retval  FALSE:�Ăяo���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL NetErr_DispMain(void)
{
	NET_ERR_SYSTEM *nes = &NetErrSystem;
	
	if(Local_SystemOccCheck() == FALSE){
		return FALSE;
	}

	if(nes->status == NET_ERR_STATUS_REQ){
    //�ʐM���C�u�����I���҂�
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
		STRBUF *strbuf;

#ifndef MULTI_BOOT_MAKE
		fontHandle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NET_TEMP );
		
		mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
			ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_NET_TEMP);
#else
		fontHandle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NET_TEMP );
		
		mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
			ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_NET_TEMP);
#endif  //MULTI_BOOT_MAKE
		strbuf = GFL_MSG_CreateString(mm, DM_MSG_ERR001);
		PRINTSYS_Print(bmpdata, 0, 0, strbuf, fontHandle);

		GFL_STR_DeleteBuffer(strbuf);
		GFL_MSG_Delete(mm);
		
		GFL_FONT_Delete(fontHandle);
	}
	
	GFL_BMP_Delete(bmpdata);
}

