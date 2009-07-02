//==============================================================================================
/**
 * @file	com_error_warning_reset.c
 * @brief	�ʐM�G���[��ʁi�\�t�g���Z�b�g�j
 * @author
 * @date
 */
//==============================================================================================
#include <gflib.h>

#include "net_old/communication.h"
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
#include "msg/msg_communication.h"
#include "print/wordset.h"

extern void ComErrorWarningResetCall( int heapID, int type, int code );
//================================================================
//
//
//
//
//
//================================================================
//�a�f�R���g���[���萔
#define COMERRRST_TEXT_FRAME	(GFL_BG_FRAME0_M)
/*��[GS_CONVERT_TAG]*/

#define COMERRRST_NULL_PALETTE	( 0x6c21 )		//�a�f�o�b�N�O���E���h�p���b�g

#define	COMERRRST_TEXT_PALNUM	(1)
#define COMERRRST_NWIN_CGXNUM	(512 - MENU_WIN_CGX_SIZ)
#define COMERRRST_NWIN_PALNUM	(2)

#define COMERRRST_STRBUF_SIZE	(0x180)	//���b�Z�[�W�o�b�t�@�T�C�Y
// ���b�Z�[�W�E�B���h�E
#define	COMERRRST_WIN_PX		(3)
#define	COMERRRST_WIN_PY		(3)
#define	COMERRRST_WIN_SX		(26)
#define	COMERRRST_WIN_SY		(18)
#define	COMERRRST_WIN_PAL		(COMERRRST_TEXT_PALNUM)
#define	COMERRRST_WIN_CGX		(COMERRRST_NWIN_CGXNUM-(COMERRRST_WIN_SX*COMERRRST_WIN_SY))

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
// ��pVIntr
//=================================================================================================
static void LocalVBlankIntr(void)
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);

	MI_WaitDma(GX_DEFAULT_DMAID);
}

//=================================================================================================
//
// �ꊇ�R�[��
//
//=================================================================================================
void ComErrorWarningResetCall( int heapID, int type, int code )
{
	GFL_BMPWIN*		msgwin;
	GFL_MSGDATA*	msgman;
	/*��[GS_CONVERT_TAG]*/
	STRBUF*				msgstr;
	GFL_FONT 			*fontHandle;
	STRBUF*				msgtmp;
	WORDSET*			wordset;
	int					msgID;

	PMSND_StopBGM();

	switch( type ){
	case COMM_ERRORTYPE_ARESET:
	default:
		msgID = Msg_CommError;
		break;
	case COMM_ERRORTYPE_POWEROFF:
		msgID = Msg_CommFatalError;
		break;
	case COMM_ERRORTYPE_TITLE:
		msgID = Msg_CommErrorMystery;
		break;
	case COMM_ERRORTYPE_GTS:
		msgID = Msg_CommErrorGTSbusy;
		break;
	case 4:
		msgID = Msg_CommFatalError02;
		break;

	case COMM_ERRORTYPE_ARESET_WIFI:
		msgID = Msg_WiFi_CommError;
		break;

	case COMM_ERRORTYPE_TITLE_WIFI:
		msgID = Msg_WiFi_CommErrorMystery;
		break;
	}

	WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
	WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );

//	(void)OS_DisableIrqMask(OS_IE_V_BLANK);
//	OS_SetIrqFunction(OS_IE_V_BLANK, LocalVBlankIntr);
//	(void)OS_EnableIrqMask(OS_IE_V_BLANK);

//	sys_VBlankFuncChange( NULL, NULL );	// VBlank�Z�b�g
//	sys_HBlankIntrSet( NULL,NULL );		// HBlank�Z�b�g

	GFL_DISP_GX_InitVisibleControl();
	/*��[GS_CONVERT_TAG]*/
	GFL_DISP_GXS_InitVisibleControl();
	/*��[GS_CONVERT_TAG]*/
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
//	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );
//	sys.disp3DSW = DISP_3D_TO_MAIN;
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	/*��[GS_CONVERT_TAG]*/
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	GFL_DISP_SetBank( &vramSetTable );		//�u�q�`�l�ݒ�
	GFL_BG_SetBGMode( &BGsys_data );			// �a�f�V�X�e���ݒ�
	GFL_BG_SetBGControl( COMERRRST_TEXT_FRAME, &hd0, GFL_BG_MODE_TEXT );
	GFL_BG_ClearScreen( COMERRRST_TEXT_FRAME );
	//���b�Z�[�W�E�C���h�E�L�������p���b�g�ǂݍ���
	BmpWinFrame_GraphicSet(	 COMERRRST_TEXT_FRAME,
													COMERRRST_NWIN_CGXNUM, COMERRRST_NWIN_PALNUM, 0, heapID );
	//�t�H���g�p���b�g�ǂݍ��݁i�V�X�e���j
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, COMERRRST_TEXT_PALNUM * (2*16), heapID );

	GFL_BG_SetClearCharacter( COMERRRST_TEXT_FRAME, 32, 0, heapID );
	/*��[GS_CONVERT_TAG]*/
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, COMERRRST_NULL_PALETTE );	//�w�i�F������
	/*��[GS_CONVERT_TAG]*/
	/*��[GS_CONVERT_TAG]*/
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, COMERRRST_NULL_PALETTE );	//�w�i�F������
	/*��[GS_CONVERT_TAG]*/
	/*��[GS_CONVERT_TAG]*/
	//���b�Z�[�W�f�[�^�ǂݍ���
	msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_communication_dat, heapID );
	/*��[GS_CONVERT_TAG]*/
	msgstr = GFL_STR_CreateBuffer( COMERRRST_STRBUF_SIZE, heapID);
	msgtmp = GFL_STR_CreateBuffer( COMERRRST_STRBUF_SIZE, heapID);
	//MSG_PrintInit();//���b�Z�[�W�\���V�X�e��������
	wordset = WORDSET_Create( heapID );
  fontHandle = GFL_FONT_Create( 
    ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  
	//	GF_BGL_BmpWinAddEx( bgl, &msgwin,&windata );//�r�b�g�}�b�v�쐬
	msgwin = GFL_BMPWIN_CreateFixPos(
		COMERRRST_TEXT_FRAME,			//�E�C���h�E�g�p�t���[��
		COMERRRST_WIN_PX,COMERRRST_WIN_PY,	//�E�C���h�E�̈�̍���XY���W
		COMERRRST_WIN_SX,COMERRRST_WIN_SY,	//�E�C���h�E�̈��XY�T�C�Y
		COMERRRST_WIN_PAL,				//�E�C���h�E�̈�̃p���b�g�i���o�[
		COMERRRST_WIN_CGX);				//�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[

//	GF_BGL_BmpWinFill( &msgwin, FBMP_COL_WHITE, 0, 0, COMERRRST_WIN_SX*8, COMERRRST_WIN_SY*8 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(msgwin), 0 );
	BmpWinFrame_Write( msgwin, WINDOW_TRANS_ON, COMERRRST_NWIN_CGXNUM, COMERRRST_NWIN_PALNUM );
	/*��[GS_CONVERT_TAG]*/

//	WORDSET_RegisterNumber( wordset, 0, code, 5, StrNumberDispType_ZERO, StrNumberCodeType_HANKAKU );
  WORDSET_RegisterNumber( wordset, 0, code,  5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
	GFL_MSG_GetString( msgman, msgID, msgtmp );	//������̎擾
	WORDSET_ExpandStr( wordset, msgstr, msgtmp );

//	GF_STR_PrintSimple(	&msgwin, FONT_SYSTEM, msgstr, 0, 0, 0, NULL );//������̕\��
	PRINTSYS_Print(	GFL_BMPWIN_GetBmp(msgwin), 0,0, msgstr, fontHandle );//������̕\��
	GFL_STR_DeleteBuffer( msgstr );

	GFL_DISP_SetDispOn();
	/*��[GS_CONVERT_TAG]*/
	WIPE_ResetBrightness( WIPE_DISP_MAIN );
	WIPE_ResetBrightness( WIPE_DISP_SUB );
//	SetBrightness( BRIGHTNESS_NORMAL, PLANEMASK_ALL, MASK_DOUBLE_DISPLAY );
	WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
	WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);

	GFL_BMPWIN_Delete( msgwin );
	/*��[GS_CONVERT_TAG]*/
	GFL_MSG_Delete( msgman );
	WORDSET_Delete( wordset );
	GFL_FONT_Delete( fontHandle );

	GFL_STR_DeleteBuffer(msgstr);
	GFL_STR_DeleteBuffer(msgtmp);

	/*��[GS_CONVERT_TAG]*/
}




