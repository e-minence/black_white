
//============================================================================================
/**
 * @file	title.c
 * @brief	DS�Ń��b�V�[�̂��܂��@�^�C�g���v���O����
 * @author	sogabe
 * @date	2007.02.06
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"
#include "yt_common.h"
#include "yt_net.h"

#include "sample_graphic/yossyegg.naix"

#define __TITLE_H_GLOBAL__
#include "title.h"

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�g��������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_InitTitle(GAME_PARAM *gp)
{
	//BG�V�X�e��������
	GFL_BG_Init(gp->heapID);

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

		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG0, VISIBLE_ON );

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_TITLE_V2_NCGR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_TITLE_V2_NSCR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_STAGESEL_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_STAGESEL_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramPalette(0,NARC_yossyegg_TITLE_BG_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN|GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB,16,0,2);

	YT_JobNoSet(gp,YT_MainTitleNo);

	GFL_SOUND_LoadHeapState(gp->mus_level[MUS_LEVEL_JINGLE]);
	GFL_SOUND_LoadGroupData(GROUP_TITLE);
	GFL_SOUND_PlayBGM(SEQ_TITLE);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�g�����C������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
enum{
	YT_SEQ_TITLE_KEY_WAIT=0,
	YT_SEQ_TITLE_END,
	YT_SEQ_TITLE_NET_CHILD,
	YT_SEQ_TITLE_NET_PARENT,
	YT_SEQ_TITLE_MULTI_BOOT,
};
void	YT_MainTitle(GAME_PARAM *gp)
{
	switch(gp->seq_no){
	case YT_SEQ_TITLE_KEY_WAIT:
#if 0
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START){
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
			gp->seq_no++;
		}
#endif
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
			gp->raster_flag=0;
			gp->seq_no++;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X){
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
			gp->raster_flag=1;
			gp->seq_no++;
		}
		//FRAME_NUM���w�肵��VRAM�N���A�̎���
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B){
//			GFL_BG_ClearFrame(GFL_BG_FRAME3_M);
//			GFL_DISP_ClearVRAM();
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L){ // L�{�^���������ꂽ
            YT_NET_Init(gp, FALSE);  // �ʐM�J�n
			gp->seq_no = YT_SEQ_TITLE_NET_CHILD;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R){ // R�{�^���������ꂽ
            YT_NET_Init(gp, TRUE);  // �ʐM�J�n
			gp->seq_no = YT_SEQ_TITLE_NET_PARENT;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){ // SELECT�{�^���������ꂽ
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
			gp->seq_no = YT_SEQ_TITLE_MULTI_BOOT;
		}
		break;
	case YT_SEQ_TITLE_END:
		if(GFL_FADE_CheckFade()==FALSE){
			GFL_BG_Exit();
			YT_JobNoSet(gp,YT_InitGameNo);
		}
        break;
    case YT_SEQ_TITLE_NET_CHILD:
    case YT_SEQ_TITLE_NET_PARENT:
        if(YT_NET_Main(gp->pNetParam)){
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
            gp->seq_no = YT_SEQ_TITLE_END;
        }
        break;
	case YT_SEQ_TITLE_MULTI_BOOT:
		if(GFL_FADE_CheckFade()==FALSE){
			GFL_BG_Exit();
			YT_JobNoSet(gp,YT_InitMultiBootNo);
		}
        break;
	}
}

