
//============================================================================================
/**
 * @file	fatal_error.c
 * @brief	fatal�G���[��ʕ\��
 * @author	ohno
 * @date	2007.02.06
 */
//============================================================================================


#include "gflib.h"
#include "include/system/gfl_use.h"
#include "graphic_data/test_graphic/fatal_error.naix"
#include "include\system\main.h"

static	const	char	*GraphicFileTable[]={
	"test_graphic/fatal_error.narc",
};

//----------------------------------------------------------------------------
/**
 *	@brief	�G���[�\��
 */
//-----------------------------------------------------------------------------
void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork)
{
	//BG�V�X�e��������
	GFL_BG_Init(GFL_HEAPID_APP);

    //ARC�V�X�e��������
	GFL_ARC_Init(&GraphicFileTable[0],1);

	//VRAM�ݒ�
	{
		GFL_BG_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_0_B,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_0_F				// �e�N�X�`���p���b�g�X���b�g
		};
		GFL_DISP_SetBank( &vramSetTable );

		//VRAM�N���A
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
	}

	//���C����ʃt���[���ݒ�
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
		};
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_S, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_S );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_S );

		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG0, VISIBLE_OFF );
		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG1, VISIBLE_OFF );
		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG3, VISIBLE_OFF );
		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG2, VISIBLE_ON );

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

//		GFL_DISP_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
//		GFL_DISP_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_fatal_error_fatal_error_lz_NCGR,GFL_BG_FRAME2_M,0,0,1,GFL_HEAPID_APP);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_fatal_error_fatal_error_lz_NSCR,GFL_BG_FRAME2_M,0,0,1,GFL_HEAPID_APP);
	GFL_ARC_UTIL_TransVramPalette(0,NARC_fatal_error_fatal_error_NCLR,PALTYPE_MAIN_BG,0,0x100,GFL_HEAPID_APP);

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN|GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB,16,0,2);

	while(TRUE){
        GFL_UI_Main();
        GFL_FADE_Main();
		// VBLANK�҂�
		// ��gflib�ɓK�؂Ȋ֐����o������u�������Ă�������
		OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	}
}

