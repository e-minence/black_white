//==============================================================================================
/**
 * @file	com_dwc_warning.c
 * @brief	�c�v�b���C�u�����x�����
 * @author
 * @date
 */
//==============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "net_app/net_bugfix.h"
#include "system/wipe.h"
#include "system/palanm.h"
#include "system/main.h"
#include "sound/pm_sndsys.h"
#include "system/bmp_winframe.h"
#include "msg/msg_wifi_system.h"


#if	AFTERMASTER_070122_WARNING_DISP_FIX
extern	void sys_SetDummyVBlankIntr(void);
#endif

extern void sleepFunc(void);

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
void DWClibWarningCall( int heapID, int error_msg_id );

//================================================================
//
//
//
//
//
//================================================================
//�a�f�R���g���[���萔
#define SVERR_TEXT_FRAME	(GFL_BG_FRAME0_M)
/*��[GS_CONVERT_TAG]*/

#define SVERR_NULL_PALETTE	( 0x6c21 )		//�a�f�o�b�N�O���E���h�p���b�g

#define	SVERR_TEXT_PALNUM	(1)
#define SVERR_NWIN_CGXNUM	(512 - MENU_WIN_CGX_SIZ)
#define SVERR_NWIN_PALNUM	(2)

#define SVERR_STRBUF_SIZE	(0x180)	//���b�Z�[�W�o�b�t�@�T�C�Y
// ���b�Z�[�W�E�B���h�E
#define	SVERR_WIN_PX		(3)
#define	SVERR_WIN_PY		(3)
#define	SVERR_WIN_SX		(26)
#define	SVERR_WIN_SY		(18)
#define	SVERR_WIN_PAL		(SVERR_TEXT_PALNUM)
#define	SVERR_WIN_CGX		(SVERR_NWIN_CGXNUM-(SVERR_WIN_SX*SVERR_WIN_SY))

//================================================================
//================================================================
static const GFL_DISP_VRAM vramSetTable = {
	/*��[GS_CONVERT_TAG]*/
	GX_VRAM_BG_256_AB,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_NONE,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_NONE,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_NONE,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE			// �e�N�X�`���p���b�g�X���b�g
	};

static const GFL_BG_SYS_HEADER BGsys_data = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0, GX_BGMODE_0,
	GX_BG0_AS_2D
	};
static const GFL_BG_BGCNT_HEADER hd0 = {
	0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	/*��[GS_CONVERT_TAG]*/
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	};

//=================================================================================================
//
// �ꊇ�R�[��
//
//=================================================================================================
void DWClibWarningCall( int heapID, int error_msg_id )
{
	//	GF_BGL_INI*			bgl;
	GFL_BMPWIN*		msgwin;
	GFL_MSGDATA*	msgman;
	/*��[GS_CONVERT_TAG]*/
	STRBUF*				msgstr;
	GFL_FONT 			*fontHandle;
	int msgID;

	PMSND_StopBGM();

	msgID = dwc_message_0001;

	WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
	WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );

	//#if	AFTERMASTER_070122_WARNING_DISP_FIX
	//#if !(AFTER_MASTER_070411_COMM_N25_EUR_FIX)
	//sys_SetDummyVBlankIntr();
	//#endif
	//#endif
	//sys_VBlankFuncChange( NULL, NULL );	// VBlank�Z�b�g
	//sys_HBlankIntrSet( NULL,NULL );		// HBlank�Z�b�g

	GFL_DISP_GX_InitVisibleControl();
	/*��[GS_CONVERT_TAG]*/
	GFL_DISP_GXS_InitVisibleControl();
	/*��[GS_CONVERT_TAG]*/
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	//sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );
	//sys.disp3DSW = DISP_3D_TO_MAIN;
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	/*��[GS_CONVERT_TAG]*/
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	GFL_DISP_SetBank( &vramSetTable );		//�u�q�`�l�ݒ�
	GFL_BG_SetBGMode( &BGsys_data );			// �a�f�V�X�e���ݒ�
	GFL_BG_SetBGControl( SVERR_TEXT_FRAME, &hd0, GFL_BG_MODE_TEXT );// ���C���a�f�V�X�e���ݒ�
	GFL_BG_ClearScreen( SVERR_TEXT_FRAME );
	//���b�Z�[�W�E�C���h�E�L�������p���b�g�ǂݍ���
	BmpWinFrame_GraphicSet( SVERR_TEXT_FRAME, SVERR_NWIN_CGXNUM, SVERR_NWIN_PALNUM, 0, heapID );
	/*��[GS_CONVERT_TAG]*/
	//�t�H���g�p���b�g�ǂݍ��݁i�V�X�e���j
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, SVERR_TEXT_PALNUM * (2*16), heapID );

	GFL_BG_SetClearCharacter( SVERR_TEXT_FRAME, 32, 0, heapID );
	/*��[GS_CONVERT_TAG]*/
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, SVERR_NULL_PALETTE );	//�w�i�F������
	/*��[GS_CONVERT_TAG]*/
	/*��[GS_CONVERT_TAG]*/
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, SVERR_NULL_PALETTE );	//�w�i�F������
	/*��[GS_CONVERT_TAG]*/
	/*��[GS_CONVERT_TAG]*/
	//���b�Z�[�W�f�[�^�ǂݍ���
	//	msgman = MSGMAN_Create( GflMsgLoadType_DIRECT, ARC_MSG, NARC_msg_wifi_system_dat, heapID );
	msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, heapID );
	/*��[GS_CONVERT_TAG]*/
	msgstr = GFL_STR_CreateBuffer( SVERR_STRBUF_SIZE, heapID);
	//MSG_PrintInit();//���b�Z�[�W�\���V�X�e��������


	fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );
	msgwin = GFL_BMPWIN_CreateFixPos(SVERR_TEXT_FRAME,
																	 SVERR_WIN_PX,SVERR_WIN_PY,	//�E�C���h�E�̈�̍���XY���W
																	 SVERR_WIN_SX,SVERR_WIN_SY,	//�E�C���h�E�̈��XY�T�C�Y
																	 SVERR_WIN_PAL,  SVERR_WIN_CGX );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(msgwin), 0 );

//	GFL_BG_BmpWinFill( msgwin, FBMP_COL_WHITE, 0, 0, SVERR_WIN_SX*8, SVERR_WIN_SY*8 );
	BmpWinFrame_Write( msgwin, WINDOW_TRANS_ON, SVERR_NWIN_CGXNUM, SVERR_NWIN_PALNUM );
	/*��[GS_CONVERT_TAG]*/

	GFL_MSG_GetString( msgman, msgID, msgstr );	//������̎擾
	PRINTSYS_Print(	GFL_BMPWIN_GetBmp(msgwin), 0,0, msgstr, fontHandle );//������̕\��
	GFL_STR_DeleteBuffer( msgstr );

	GFL_DISP_SetDispOn();
	/*��[GS_CONVERT_TAG]*/
	WIPE_ResetBrightness( WIPE_DISP_MAIN );
	WIPE_ResetBrightness( WIPE_DISP_SUB );
	WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
	WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);

	while( 1 ){
		int	keyData = GFL_UI_KEY_GetTrg();

		sleepFunc();
		if( keyData & PAD_BUTTON_A ){
			break;
		}
		OS_WaitIrq(TRUE, OS_IE_V_BLANK); 	// �u�u�����N�҂�
	}

	GFL_BMPWIN_Delete(msgwin);
	GFL_MSG_Delete( msgman );//���b�Z�[�W�f�[�^�j��
	GFL_FONT_Delete( fontHandle );
	GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
	GFL_BG_FreeBGControl( SVERR_TEXT_FRAME );
}



