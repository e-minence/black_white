
//============================================================================================
/**
 * @file	menu.c
 * @brief	�I�����j���[
 * @author	sogabe
 * @date	2008.04.01
 */
//============================================================================================

#include "gflib.h"
#include "textprint.h"
#include "yt_common.h"

typedef struct
{
	int						seq_no;
	int						heapID;
	int						pos;
	GFL_TEXT_PRINTPARAM*	textParam;
	GFL_BMPWIN*				bmpwin[32];
}SOGA_MENU;

typedef struct {
	const char*	msg;
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;
}TESTMODE_PRINTLIST;

static const char	test_select1[]	= {"���b�V�[�̂��܂�"};
static const char	test_select2[]	= {"�u���b�N�A�E�g"};

static const int BranchJobTable[]={
	YT_InitTitleNo,
	YT_InitBlockOutNo,
};

static const TESTMODE_PRINTLIST selectList[] = {
	{ test_select1,  4,  5, 24, 1 },
	{ test_select2,  4,  7, 24, 1 },
//	{ test_select3,  4,  9, 24, 1 },
//	{ test_select4,  4, 11, 24, 1 },

//	{ test_select5,  4, 15, 24, 1 },
//	{ test_select6,  4, 17, 24, 1 },
//	{ test_select7,  4, 19, 24, 1 },
//	{ test_select8,  4, 21, 24, 1 },

//	{ test_select9,  4,  1, 24, 1 },
//	{ test_select10, 4,  3, 24, 1 },
//	{ test_select11, 4,  5, 24, 1 },
//	{ test_select12, 4,  7, 24, 1 },
//	{ test_select13, 4,  9, 24, 1 },

//	{ test_select14, 4, 13, 24, 1 },
//	{ test_select15, 4, 15, 24, 1 },
//	{ test_select16, 4, 17, 24, 1 },
//	{ test_select17, 4, 19, 24, 1 },
};

enum {
	MSG_WHITE = 0,
	MSG_RED,
};

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static const char font_path[] = {"src/gfl_graphic/gfl_font.dat"};

#define G2D_BACKGROUND_COL	(0x0000)
#define G2D_FONT_COL		(0x7fff)
#define G2D_FONTSELECT_COL	(0x001f)

static const GFL_TEXT_PRINTPARAM default_param = 
{ NULL, 0, 0, 1, 1, 1, 0, GFL_TEXT_WRITE_16 };

#define TEXT_FRM			(GFL_BG_FRAME2_M)
#define TEXT_FRM_PRI		(0)

static void msg_bmpwin_make
			(SOGA_MENU * soga_menu, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy );

void	YT_InitMenu(GAME_PARAM *gp);
void	YT_MainMenu(GAME_PARAM *gp);

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_InitMenu(GAME_PARAM *gp)
{
	SOGA_MENU	*soga_menu;

	//�W���u���[�N���m��
	soga_menu=gp->job_work=GFL_HEAP_AllocClearMemory(gp->heapID,sizeof(SOGA_MENU));

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

		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_ON );

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( gp->heapID );

	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );
	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( gp->heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//�����\���p�����[�^���[�N�쐬
	{
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocClearMemoryLo
										( gp->heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*param = default_param;
		soga_menu->textParam = param;
	}
	//�����\���r�b�g�}�b�v�̍쐬
	{
		int i;

		//�I�����ڃr�b�g�}�b�v�̍쐬
		for(i=0;i<NELEMS(selectList);i++){
			msg_bmpwin_make( soga_menu, i, selectList[i].msg,
							selectList[i].posx, selectList[i].posy,
							selectList[i].sizx, selectList[i].sizy );
		}
	}

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,16,0,0);

	GFL_BMPWIN_SetPalette( soga_menu->bmpwin[0], MSG_RED );

	YT_JobNoSet(gp,YT_MainMenuNo);
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���C��
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_MainMenu(GAME_PARAM *gp)
{
	SOGA_MENU	*soga_menu=(SOGA_MENU *)gp->job_work;
	int			i;

	GFL_BG_ClearScreen( TEXT_FRM );
	for(i=0;i<NELEMS(selectList);i++){
		GFL_BMPWIN_MakeScreen( soga_menu->bmpwin[i] );
	}
	GFL_BG_LoadScreenReq( TEXT_FRM );

	switch(soga_menu->seq_no){
	case 0:
		if((GFL_UI_KEY_GetTrg()&PAD_KEY_UP)&&(soga_menu->pos>0)){
			GFL_BMPWIN_SetPalette( soga_menu->bmpwin[soga_menu->pos], MSG_WHITE );
			soga_menu->pos--;
			GFL_BMPWIN_SetPalette( soga_menu->bmpwin[soga_menu->pos], MSG_RED );
		}
		if((GFL_UI_KEY_GetTrg()&PAD_KEY_DOWN)&&(soga_menu->pos<NELEMS(selectList)-1)){
			GFL_BMPWIN_SetPalette( soga_menu->bmpwin[soga_menu->pos], MSG_WHITE );
			soga_menu->pos++;
			GFL_BMPWIN_SetPalette( soga_menu->bmpwin[soga_menu->pos], MSG_RED );
		}
		if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_A){
			soga_menu->seq_no++;
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,0);
		}
		break;
	case 1:
		if(GFL_FADE_CheckFade()==FALSE){
			//�\��r�b�g�}�b�v�̔j��
			for(i=0;i<NELEMS(selectList);i++){
				GFL_BMPWIN_Delete( soga_menu->bmpwin[i] );
			}
			GFL_HEAP_FreeMemory( soga_menu->textParam );
			GFL_TEXT_DeleteSystem();
			GFL_BG_Exit();
			YT_JobNoSet(gp,BranchJobTable[soga_menu->pos]);
			GFL_HEAP_FreeMemory(soga_menu);
		}
		break;
	}
}

//------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�r�b�g�}�b�v�E�C���h�E�R���g���[��
 */
//------------------------------------------------------------------
static void msg_bmpwin_make
			(SOGA_MENU * soga_menu, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy )
{
	//�r�b�g�}�b�v�쐬
	soga_menu->bmpwin[bmpwinNum] = GFL_BMPWIN_Create( TEXT_FRM, px, py, sx, sy, 0, 
														GFL_BG_CHRAREA_GET_B );

	//�e�L�X�g���r�b�g�}�b�v�ɕ\��
	soga_menu->textParam->bmp = GFL_BMPWIN_GetBmp( soga_menu->bmpwin[ bmpwinNum ] );
	soga_menu->textParam->writex = 0;
	soga_menu->textParam->writey = 0;
	GFL_TEXT_PrintSjisCode( msg, soga_menu->textParam );

	//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
	GFL_BMPWIN_TransVramCharacter( soga_menu->bmpwin[bmpwinNum] );
}
