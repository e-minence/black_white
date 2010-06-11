//==============================================================================
/**
 * @file    save_error_warning.c
 * @brief   �Z�[�u�ǂݏ����Ɏ��s�����ꍇ�̌x�����
 * @author  matsuda
 * @date    2010.04.14(��)
 */
//==============================================================================
#include <gflib.h>
#include "system\main.h"
#include "arc_def.h"

#include "print\printsys.h"
#include "print\gf_font.h"
#include "font/font.naix"
#include "msg\msg_backup_err.h"

#include "savedata/save_control.h"
#include "system/net_err.h"
#include "net_err.naix"
#include "message.naix"

#include "system/save_error_warning.h"


//==============================================================================
//	�萔��`
//==============================================================================
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

//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#define SAVE_ERR_HEAPID (HEAPID_SAVE)
#else                    //DL�q�@������
#define SAVE_ERR_HEAPID (HEAPID_MULTIBOOT)
#endif //MULTI_BOOT_MAKE

//�w�iBG�{���b�Z�[�W�f�[�^�ŃL�����N�^�̈�NETERR_PUSH_CHARVRAM_SIZE���I�[�o�[���Ă��Ȃ����`�F�b�N
SDK_COMPILER_ASSERT(MESSAGE_X_LEN*0x20*MESSAGE_Y_LEN + BG_DATA_SIZE <= NETERR_PUSH_CHARVRAM_SIZE);


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void Local_WarningDispMain(u32 msg_id);
static void Local_WarningDispInit(u32 msg_id);
static void Local_WarningDispDraw(void);
static void Local_WarningMessagePrint(u32 msg_id);


//==============================================================================
//
//	
//
//==============================================================================
//==================================================================
/**
 * �Z�[�u�R�[���o�b�N�F���[�h�Ɏ��s
 */
//==================================================================
void SaveErrorCall_Load(void)
{
  Local_WarningDispMain(msg_load_err);
}

//==================================================================
/**
 * �Z�[�u�R�[���o�b�N�F�Z�[�u�Ɏ��s
 *
 * @param   err_disable		
 */
//==================================================================
void SaveErrorCall_Save(GFL_SAVEERROR_DISABLE err_disable)
{
  u32 msg_id = err_disable == GFL_SAVEERROR_DISABLE_WRITE ? msg_flash_err : msg_write_err;
  Local_WarningDispMain(msg_id);
}


//--------------------------------------------------------------
/**
 * �x����ʃ��C��
 *
 * @param   msg_id		�\�����郁�b�Z�[�W
 */
//--------------------------------------------------------------
static void Local_WarningDispMain(u32 msg_id)
{
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  SaveControl_SystemExit(); //��ʕ`��p�̃q�[�v�m�ۂׁ̈A�Z�[�u�V�X�e����j������
#endif //MULTI_BOOT_MAKE
  Local_WarningDispInit(msg_id);

  //���̃v���C���[�ɐN������Ȃ��悤�ʐM���I��������
  if(GFL_NET_IsInit())
  {
    GFL_NET_ResetDisconnect();  ///�ؒf�������ł���U���Z�b�g
    GFL_NET_Exit(NULL);
    GFL_NET_IRCWIRELESS_ResetSystemError();  //�ԊO��WIRLESS�ؒf
    do{
      GFL_NET_Main();
    }while(GFL_NET_IsExit() == FALSE);
  }
  
  //�������[�v
  do{
    OS_Halt();
  }while(1);  //�ꉞ
}

//--------------------------------------------------------------
/**
 * ��ʂ̏����ݒ�
 */
//--------------------------------------------------------------
static void Local_WarningDispInit(u32 msg_id)
{
	//�����������ʂ������Ȃ��悤�ɁA�܂��}�X�^�[�P�x��ޔ����A�^���Âɐݒ�
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	
	//���荞�݂��~�߂�
	GX_HBlankIntr(FALSE);
	GX_VBlankIntr(FALSE);
	
	//VRAM�o���N���ޔ�
	GX_ResetBankForBG();
	GX_ResetBankForSubBG();
	GX_SetBankForBG(GX_VRAM_BG_128_C);

	//dispcnt�ޔ�
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG1);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);

	//BG1Control�ޔ�
	G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, 
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
	G2_BG1Mosaic(FALSE);
	G2_SetBG1Offset(0, 0);

	G2_BlendNone();
	
	//�G���[��ʕ`��
	Local_WarningDispDraw();
	Local_WarningMessagePrint(msg_id);
	
	//�\��ON
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(16);	//�T�u��ʂ͐^����
}

//--------------------------------------------------------------
/**
 * @brief   �G���[��ʕ`��
 */
//--------------------------------------------------------------
static void Local_WarningDispDraw(void)
{
	u32 data_size;
	void *arcData;
	NNSG2dCharacterData* charData;
	NNSG2dScreenData* scrnData;
	NNSG2dPaletteData* palData;
	BOOL  cmpFlag;
	NNSG2dPaletteCompressInfo*  cmpInfo;
	
	GFL_STD_MemClear32(G2_GetBG1CharPtr(), NETERR_PUSH_CHARVRAM_SIZE);

	//�L�����N�^
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NCGR, 0, SAVE_ERR_HEAPID);
	if(NNS_G2dGetUnpackedBGCharacterData(arcData, &charData)){
		DC_FlushRange(charData->pRawData, charData->szByte);
		//�O�̂���DMA���g�p����͔̂�����
		GFL_STD_MemCopy16(charData->pRawData, G2_GetBG1CharPtr(), charData->szByte);
		GF_ASSERT(MESSAGE_START_CHARNO*0x20 >= charData->szByte);	//����ASSERT�Ɉ���������ꍇ��BG_DATA_SIZE��傫������K�v������B���̕��G���[���b�Z�[�W�Ɋ�����L�����N�^�̈悪���Ȃ��Ȃ�̂Œ���
	}
	GFL_HEAP_FreeMemory(arcData);
	
	//�X�N���[��
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NSCR, 0, SAVE_ERR_HEAPID);
	if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) ){
		DC_FlushRange(scrnData->rawData, scrnData->szByte);
		GFL_STD_MemCopy16(scrnData->rawData, G2_GetBG1ScrPtr(), scrnData->szByte);
	}
	GFL_HEAP_FreeMemory( arcData );
	
	//�p���b�g
	arcData = GFL_ARC_UTIL_Load(ARCID_NET_ERR, NARC_net_err_net_err_NCLR, 0, SAVE_ERR_HEAPID);
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
static void Local_WarningMessagePrint(u32 msg_id)
{
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
		MESSAGE_X_LEN, MESSAGE_Y_LEN, GFL_BMP_16_COLOR, SAVE_ERR_HEAPID);
	
	//���b�Z�[�WOPEN
	{
		GFL_FONT		*fontHandle;
		GFL_MSGDATA		*mm;
    STRBUF  *src;

		fontHandle = GFL_FONT_Create(ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, SAVE_ERR_HEAPID );
		
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
        ARCID_MESSAGE, NARC_message_backup_err_dat, SAVE_ERR_HEAPID);
#else                    //DL�q�@������
      //MB_BACKUP_ERROR_MSG_FILE��Arc�����|�P�V�t�^�[�Ɖf��]���ňႤ�̂�mustiboot/src/net_err_dl.c�Œ�`
    mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
        ARCID_MESSAGE, MB_BACKUP_ERROR_MSG_FILE, SAVE_ERR_HEAPID);
#endif //MULTI_BOOT_MAKE

    src     = GFL_MSG_CreateString(mm, msg_id);
		PRINTSYS_PrintColor(bmpdata, 0, 0, src, fontHandle, PRINTSYS_LSB_Make(4, 0xb, 7));

		GFL_STR_DeleteBuffer(src);
		GFL_MSG_Delete(mm);
		
		GFL_FONT_Delete(fontHandle);
	}
	
	GFL_BMP_Delete(bmpdata);
}
