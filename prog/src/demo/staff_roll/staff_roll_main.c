//============================================================================================
/**
 * @file		staff_roll_main.c
 * @brief		�G���f�B���O�E�X�^�b�t���[�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.19
 *
 *	���W���[�����FSRMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"

#include "font/font.naix"
#include "message.naix"
#include "msg/msg_staff_list_jp.h"
#include "msg/msg_staff_list_eng.h"
#include "title/title_res.h"
#include "demo3d.naix"

#include "staff_roll_main.h"
#include "staff_roll.naix"


//============================================================================================
//============================================================================================

#ifdef PM_DEBUG
#define	LOCAL_VERSION		( VERSION_BLACK )		// �f�o�b�O�p�i�R�~�b�g���̓u���b�N�ɂ��邱�ƁI�j
#else
#define	LOCAL_VERSION		( VERSION_BLACK )		// ���i�ł̓u���b�N����{�Ƃ���
#endif	// PM_DEBUG

// ����m�ۗp�q�[�v�h�c
#define	HEAPID_STAFF_ROLL_L		( GFL_HEAP_LOWID(HEAPID_STAFF_ROLL) )

enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_WIPE,
	MAINSEQ_RELEASE,

	MAINSEQ_START_WAIT,
	MAINSEQ_MAIN,

	MAINSEQ_END,
};

enum {
	SUBSEQ_INIT = 0,
	SUBSEQ_WAIT,
	SUBSEQ_STR_PUT,
	SUBSEQ_STR_CLEAR,

	SUBSEQ_LOGO_PUT,

	SUBSEQ_END,
};

#ifdef	PM_DEBUG
#define	LIST_START_INIT_WAIT	( wk->testStartInitWait )
#define	LIST_START_END_WAIT		( wk->testStartEndWait )
#define	LOGO_PUT_WAIT					( wk->testLogoWait )
#else
#define	LIST_START_INIT_WAIT	( 47 )
#define	LIST_START_END_WAIT		( 41 )
#define	LOGO_PUT_WAIT					( 160 )
#endif	// PM_DEBUG

#if	PM_VERSION == LOCAL_VERSION
#define	BG_COLOR		( 0 )														// �o�b�N�O���E���h�J���[
#define	FCOL_WP00V	( PRINTSYS_MACRO_LSB(15,2,0) )	// �t�H���g�J���[�F�f�t�H���g
#else
#define	BG_COLOR		( 0xffff )											// �o�b�N�O���E���h�J���[
#define	FCOL_WP00V	( PRINTSYS_MACRO_LSB(1,2,0) )		// �t�H���g�J���[�F�f�t�H���g
#endif
#define	FCOL_WP00R	( PRINTSYS_MACRO_LSB(3,4,0) )		// �t�H���g�J���[�F��
#define	FCOL_WP00B	( PRINTSYS_MACRO_LSB(5,6,0) )		// �t�H���g�J���[�F��
#define	FCOL_WP00Y	( PRINTSYS_MACRO_LSB(7,8,0) )		// �t�H���g�J���[�F��
#define	FCOL_WP00G	( PRINTSYS_MACRO_LSB(9,10,0) )	// �t�H���g�J���[�F��
#define	FCOL_WP00O	( PRINTSYS_MACRO_LSB(11,12,0) )	// �t�H���g�J���[�F�I�����W
#define	FCOL_WP00P	( PRINTSYS_MACRO_LSB(13,14,0) )	// �t�H���g�J���[�F�s���N

#define	MBG_PAL_FONT	( 15 )		// ���C����ʃt�H���g�p���b�g�i�a�f�j
#define	SBG_PAL_FONT	( 15 )		// �T�u��ʃt�H���g�p���b�g�i�a�f�j

#define	BMPWIN_TRANS_MAIN_FLAG		( 1 )
#define	BMPWIN_TRANS_SUB_FLAG			( 2 )

#define	EXP_BUFF_SIZE		( 128 )		// ���b�Z�[�W�W�J�p�o�b�t�@�T�C�Y

#define	ITEMLIST_MSG_NONE		( 0xffff )		// ���b�Z�[�W�Ȃ�

// ���x��
enum {
	ITEMLIST_LABEL_NONE = 0,
	ITEMLIST_LABEL_STR_PUT,
	ITEMLIST_LABEL_STR_CLEAR,
	ITEMLIST_LABEL_SCROLL_START,
	ITEMLIST_LABEL_SCROLL_STOP,
	ITEMLIST_LABEL_END,

	ITEMLIST_LABEL_LOGO_PUT,
	ITEMLIST_LABEL_3D_PUT,
	ITEMLIST_LABEL_3D_CLEAR,
	ITEMLIST_LABEL_3D_CAMERA_REQUEST,

	ITEMLIST_LABEL_VERSION,

	ITEMLIST_LABEL_MAX,
};

// �`��ʒu
enum {
	STR_PUT_MODE_LEFT = 0,		// ���l��
	STR_PUT_MODE_RIGHT,				// �E�l��
	STR_PUT_MODE_CENTER,			// ����
};

#define SKIP_SPEED					( 4 )
#define	STR_FADE_SPEED			( 4 )
#define	LOGO_FADEIN_SPEED		( 16 )
#define	LOGO_FADEOUT_SPEED	( 1 )

typedef int (*pCOMM_FUNC)(SRMAIN_WORK*,ITEMLIST_DATA*);


//============================================================================================
//============================================================================================
static int MainSeq_Init( SRMAIN_WORK * wk );
static int MainSeq_Wipe( SRMAIN_WORK * wk );
static int MainSeq_Release( SRMAIN_WORK * wk );
static int MainSeq_StartWait( SRMAIN_WORK * wk );
static int MainSeq_Main( SRMAIN_WORK * wk );

static void InitVram(void);
static void InitBg(void);
static void ExitBg(void);
static void LoadBgGraphic(void);
static void InitVBlank( SRMAIN_WORK * wk );
static void ExitVBlank( SRMAIN_WORK * wk );
static void InitMsg( SRMAIN_WORK * wk );
static void ExitMsg( SRMAIN_WORK * wk );

static void CreateListData( SRMAIN_WORK * wk );
static void DeleteListData( SRMAIN_WORK * wk );

static void InitBmp( SRMAIN_WORK * wk );
static void ExitBmp( SRMAIN_WORK * wk );
static void SetBmpWinTransFlag( SRMAIN_WORK * wk, u32 flg );
static void TransBmpWinChar( SRMAIN_WORK * wk );

static void Init3D( SRMAIN_WORK * wk );
static void Exit3D( SRMAIN_WORK * wk );
static void Main3D( SRMAIN_WORK * wk );
static void CameraMoveInit( SR3DCAMERA_MOVE * mvwk );
static VecFx32 CameraMoveValMake( const VecFx32 * now, const VecFx32 * mvp, u32 cnt );
static void CameraMoveMain( SRMAIN_WORK * wk );
static void CameraMoveFlagSet( SR3DCAMERA_MOVE * mvwk, BOOL flg );


static int SetFadeIn( SRMAIN_WORK * wk, int next );
static int SetFadeOut( SRMAIN_WORK * wk, int next );
static void CheckSkip( SRMAIN_WORK * wk );
static void ChangeBrightness3D( SRMAIN_WORK * wk );

static void ListScroll( SRMAIN_WORK * wk );

static int CreateItemData( SRMAIN_WORK * wk );

static BOOL PutStr( SRMAIN_WORK * wk );
static BOOL ClearStr( SRMAIN_WORK * wk );

static BOOL PutLogo( SRMAIN_WORK * wk );

static BOOL Comm_LabelNone( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelStrPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelStrClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelScrollStart( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelScrollStop( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelEnd( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelLogoPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_Label3DPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_Label3DClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_Label3DCameraRequest( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelVersion( SRMAIN_WORK * wk, ITEMLIST_DATA * item );

#ifdef	PM_DEBUG
static void DebugGridSet(void);
static void DebugCameraPrint( SRMAIN_WORK * wk );
#endif	// PM_DEBUG


FS_EXTERN_OVERLAY(ui_common);


//============================================================================================
//============================================================================================
static const pSRMAIN_FUNC	MainSeq[] = {
	MainSeq_Init,
	MainSeq_Wipe,
	MainSeq_Release,

	MainSeq_StartWait,
	MainSeq_Main,
};

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_D,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_NONE,							// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_NONE,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_01_AB,						// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0123_E,				// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

// �t�H���g�J���[�e�[�u��
static const PRINTSYS_LSB FontColorTbl[] = {
	FCOL_WP00V,		// �t�H���g�J���[�F�f�t�H���g
	FCOL_WP00R,		// �t�H���g�J���[�F��
	FCOL_WP00B,		// �t�H���g�J���[�F��
	FCOL_WP00Y,		// �t�H���g�J���[�F��
	FCOL_WP00G,		// �t�H���g�J���[�F��
	FCOL_WP00O,		// �t�H���g�J���[�F�I�����W
	FCOL_WP00P,		// �t�H���g�J���[�F�s���N
};

// �R�}���h�e�[�u��
static const pCOMM_FUNC CommFunc[] = {
	Comm_LabelNone,					// ���x���F�Ȃ�
	Comm_LabelStrPut,				// ���x���F������ꊇ�\��
	Comm_LabelStrClear,			// ���x���F�\���N���A
	Comm_LabelScrollStart,	// ���x���F�X�N���[���J�n
	Comm_LabelScrollStop,		// ���x���F�X�N���[����~
	Comm_LabelEnd,					// ���x���F�I��

	Comm_LabelLogoPut,					// ���x���F���S�\��
	Comm_Label3DPut,						// ���x���F�R�c�\��
	Comm_Label3DClear,					// ���x���F�R�c�N���A
	Comm_Label3DCameraRequest,	// ���x���F�R�c�J�����ړ����N�G�X�g

	Comm_LabelVersion,			// ���x���F�o�[�W�����ʏ���
};




//============================================================================================
//	�V�[�P���X
//============================================================================================

BOOL SRMAIN_Main( SRMAIN_WORK * wk )
{
	CheckSkip( wk );

	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

#ifdef	SRMAIN_DRAW_3D
	Main3D( wk );
#endif	// SRMAIN_DRAW_3D

	return TRUE;
}

static int MainSeq_Init( SRMAIN_WORK * wk )
{
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// ���ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );

#ifdef	PM_DEBUG
	wk->testStartInitWait = 47;
	wk->testLogoWait = 160;
	wk->testStartEndWait = 41;
/*
	wk->testStartInitWait = 31;
	wk->testLogoWait = 128;
	wk->testStartEndWait = 39;
*/
#endif	// PM_DEBUG

	InitVram();
	InitBg();
	LoadBgGraphic();
	InitMsg( wk );
	InitBmp( wk );
#ifdef	SRMAIN_DRAW_3D
	Init3D( wk );
#endif	// SRMAIN_DRAW_3D

	CreateListData( wk );

	InitVBlank( wk );

	PMSND_PlayBGM_WideChannel( SEQ_BGM_ENDING );

//	return SetFadeIn( wk, MAINSEQ_START_WAIT );
		return MAINSEQ_START_WAIT;
//	return SetFadeIn( wk, MAINSEQ_MAIN );
}

static int MainSeq_Wipe( SRMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->nextSeq;
	}
	return MAINSEQ_WIPE;
}

static int MainSeq_Release( SRMAIN_WORK * wk )
{
	ExitVBlank( wk );

	DeleteListData( wk );

#ifdef	SRMAIN_DRAW_3D
	Exit3D( wk );
#endif	// SRMAIN_DRAW_3D
	ExitBmp( wk );
	ExitMsg( wk );
	ExitBg();

#if	PM_VERSION == LOCAL_VERSION
	// �P�x���Œ�ɂ��Ă����i�u���b�N�j
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
#else
	// �P�x���ō��ɂ��Ă����i�z���C�g�j
	GX_SetMasterBrightness( 16 );
	GXS_SetMasterBrightness( 16 );
#endif
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return MAINSEQ_END;
}

// �J�n���̂r�d�Đ�
static int MainSeq_StartWait( SRMAIN_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		if( wk->wait >= LIST_START_INIT_WAIT ){
			wk->wait = 0;
			wk->subSeq++;
		}else{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
				OS_Printf( "[0] push a button : wait = %d\n", wk->wait );
			}
			if( wk->skipFlag == 1 ){
				wk->wait += SKIP_SPEED;
			}else{
				wk->wait++;
			}
		}
		break;

	case 1:
		if( PutLogo( wk ) == FALSE ){
			wk->subSeq++;
		}
		break;

	case 2:
		if( wk->wait >= LIST_START_END_WAIT ){
			wk->wait = 0;
			wk->subSeq = 0;
			return MAINSEQ_MAIN;
		}else{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
				OS_Printf( "[2] push a button : wait = %d\n", wk->wait );
			}
			if( wk->skipFlag == 1 ){
				wk->wait += SKIP_SPEED;
			}else{
				wk->wait++;
			}
		}
		break;
	}
	return MAINSEQ_START_WAIT;
}

static int MainSeq_Main( SRMAIN_WORK * wk )
{
#ifdef	PM_DEBUG
	// �O���b�h�\��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		DebugGridSet();
	}
	// �ꎞ��~
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		wk->debugStopFlg ^= 1;
	}
	if( wk->debugStopFlg == TRUE ){
		return MAINSEQ_MAIN;
	}
	// �I��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}
#endif

	if( wk->subSeq == SUBSEQ_INIT ){
		while( 1 ){
			ITEMLIST_DATA * item = &wk->list[wk->listPos];
//			OS_Printf( "[%d] : label = %d, msg = %d\n", wk->listPos, item->label, item->msgIdx );
			if( CommFunc[item->label]( wk, item ) == TRUE ){
				wk->listPos++;
				break;
			}
			wk->listPos++;
		}
	}

	switch( wk->subSeq ){
	case SUBSEQ_WAIT:
		if( wk->listWait <= 0 ){
			wk->listWait = 0;
			wk->subSeq = SUBSEQ_INIT;
		}else{
			if( wk->skipFlag == 1 ){
				wk->listWait -= SKIP_SPEED;
			}else{
				wk->listWait--;
			}
		}
		break;

	case SUBSEQ_STR_PUT:
		if( PutStr( wk ) == FALSE ){
			wk->subSeq = SUBSEQ_INIT;
		}
		break;

	case SUBSEQ_STR_CLEAR:
		if( ClearStr( wk ) == FALSE ){
			wk->subSeq = SUBSEQ_INIT;
		}
		break;

/*
	case SUBSEQ_LOGO_PUT:
		if( PutLogo( wk ) == FALSE ){
			wk->subSeq = SUBSEQ_INIT;
		}
		break;
*/

	case SUBSEQ_END:
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	ListScroll( wk );

	return MAINSEQ_MAIN;
}


//============================================================================================
//	������
//============================================================================================

static void InitVram(void)
{
	GFL_DISP_ClearVRAM( 0 );
	GFL_DISP_SetBank( &VramTbl );
}

static void InitBg(void)
{
	GFL_BG_Init( HEAPID_STAFF_ROLL );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF������
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���S
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_23, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_256X16 );
	}

	{	// ��]�g�k��������
		MtxFx22 mtx;
	  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE*9, FX32_ONE*9, GFL_CALC2D_AFFINE_MAX_NORMAL );
		GFL_BG_SetAffine( GFL_BG_FRAME3_M, &mtx, 128, 96 );
	}
/*
	{
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x08000, 0x10000,
			GX_BG_EXTPLTT_23, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_256X16 );
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_256X16 );
	}

	{	// ��]�g�k��������
		MtxFx22 mtx;
	  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
		G2_SetBG3Affine( &mtx, 0, 0, 0, 0 );
		G2S_SetBG3Affine( &mtx, 0, 0, 0, 0 );
	}
*/


	{	// �T�u��ʁF������
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}

#ifdef PM_DEBUG
	{	// �O���b�h��
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x1000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_TEXT );
	}
#endif	// PM_DEBUG

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
}

static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );

#ifdef PM_DEBUG
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
#endif	// PM_DEBUG

	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );

	GFL_BG_Exit();
}

static void LoadBgGraphic(void)
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( TITLE_RES_ARCID, HEAPID_STAFF_ROLL_L );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, TITLE_RES_LOGO_NCGR, GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_STAFF_ROLL );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, TITLE_RES_LOGO_NSCR, GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_STAFF_ROLL );

	GFL_ARC_CloseDataHandle( ah );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr,
		PALTYPE_MAIN_BG, MBG_PAL_FONT*0x20, 0x20, HEAPID_STAFF_ROLL );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr,
		PALTYPE_SUB_BG, SBG_PAL_FONT*0x20, 0x20, HEAPID_STAFF_ROLL );

	// �o�b�N�O���E���h�J���[
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME1_M, BG_COLOR );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME1_S, BG_COLOR );
}

static void LoadLogoPalette( SRMAIN_WORK * wk, BOOL flg )
{
	if( flg == TRUE ){
		ARCHANDLE * ah = GFL_ARC_OpenDataHandle( TITLE_RES_ARCID, HEAPID_STAFF_ROLL );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, TITLE_RES_LOGO_NCLR, PALTYPE_MAIN_BG, 0, 0, HEAPID_STAFF_ROLL );
		GFL_ARC_CloseDataHandle( ah );
	}else{
		// �t�H���g�p���b�g
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr,
			PALTYPE_MAIN_BG, MBG_PAL_FONT*0x20, 0x20, HEAPID_STAFF_ROLL );
	}
	// �o�b�N�O���E���h�J���[
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME1_M, BG_COLOR );
}


static void VBlankTask( GFL_TCB * tcb, void * work )
{
	SRMAIN_WORK * wk = work;

	TransBmpWinChar( wk );
  GFL_BG_VBlankFunc();
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static void InitVBlank( SRMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

static void ExitVBlank( SRMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

static void InitMsg( SRMAIN_WORK * wk )
{
	if( wk->dat->mojiMode == MOJIMODE_HIRAGANA ){
		wk->mman = GFL_MSG_Create(
								GFL_MSG_LOAD_NORMAL,
								ARCID_MESSAGE, NARC_message_staff_list_jp_dat, HEAPID_STAFF_ROLL );
	}else{
		wk->mman = GFL_MSG_Create(
								GFL_MSG_LOAD_NORMAL,
								ARCID_MESSAGE, NARC_message_staff_list_eng_dat, HEAPID_STAFF_ROLL );
	}
  
//	wk->wset = WORDSET_Create( HEAPID_STAFF_ROLL );
//	wk->que  = PRINTSYS_QUE_Create( HEAPID_STAFF_ROLL );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUFF_SIZE, HEAPID_STAFF_ROLL );

//	OS_Printf( "heap size [0] = 0x%x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_STAFF_ROLL) );
	wk->font[SRMAIN_FONT_NORMAL] = GFL_FONT_Create(
																	ARCID_FONT, NARC_font_large_gftr,
//																	GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STAFF_ROLL );
																	GFL_FONT_LOADTYPE_MEMORY, FALSE, HEAPID_STAFF_ROLL );
//	OS_Printf( "heap size [1] = 0x%x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_STAFF_ROLL) );

	wk->font[SRMAIN_FONT_SMALL] = GFL_FONT_Create(
//																	ARCID_FONT, NARC_font_large_gftr,
																	ARCID_FONT, NARC_font_small_batt_gftr,
//																	GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STAFF_ROLL );
																	GFL_FONT_LOADTYPE_MEMORY, FALSE, HEAPID_STAFF_ROLL );
//	OS_Printf( "heap size [2] = 0x%x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_STAFF_ROLL) );
}

static void ExitMsg( SRMAIN_WORK * wk )
{
	GFL_FONT_Delete( wk->font[SRMAIN_FONT_SMALL] );
	GFL_FONT_Delete( wk->font[SRMAIN_FONT_NORMAL] );

	GFL_STR_DeleteBuffer( wk->exp );
//	PRINTSYS_QUE_Delete( wk->que );
//	WORDSET_Delete( wk->wset );
	GFL_MSG_Delete( wk->mman );
}

static void CreateListData( SRMAIN_WORK * wk )
{
	if( wk->dat->mojiMode == MOJIMODE_HIRAGANA ){
		wk->list = GFL_ARC_LoadDataAlloc(
								ARCID_STAFF_ROLL, NARC_staff_roll_staff_list_jp_dat, HEAPID_STAFF_ROLL );
	}else{
		wk->list = GFL_ARC_LoadDataAlloc(
								ARCID_STAFF_ROLL, NARC_staff_roll_staff_list_eng_dat, HEAPID_STAFF_ROLL );
	}
}

static void DeleteListData( SRMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->list );
}


//============================================================================================
//	�a�l�o
//============================================================================================

static void InitBmp( SRMAIN_WORK * wk )
{
	GFL_BMPWIN_Init( HEAPID_STAFF_ROLL );
//	wk->util[0].win = GFL_BMPWIN_Create( GFL_BG_FRAME1_M, 0, 0, 32, 32, MBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B );
	wk->util[0].win = GFL_BMPWIN_Create( GFL_BG_FRAME1_M, 0, 0, 32, 25, MBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B );
	wk->util[1].win = GFL_BMPWIN_Create( GFL_BG_FRAME1_S, 0, 0, 32, 28, SBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B );

//	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[0].win), 0, 16, 256, 32, 1 );
//	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[1].win), 0, 32, 256, 16, 2 );

/*
	GFL_BMPWIN_MakeScreen( wk->util[0].win );
	GFL_BMPWIN_MakeScreen( wk->util[1].win );

	GFL_BMPWIN_TransVramCharacter( wk->util[0].win );
	GFL_BMPWIN_TransVramCharacter( wk->util[1].win );
*/
	GFL_BMPWIN_MakeTransWindow( wk->util[0].win );
	GFL_BMPWIN_MakeTransWindow( wk->util[1].win );
}

static void ExitBmp( SRMAIN_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->util[0].win );
	GFL_BMPWIN_Delete( wk->util[1].win );
	GFL_BMPWIN_Exit();
}


static void SetBmpWinTransFlag( SRMAIN_WORK * wk, u32 flg )
{
	wk->bmpTransFlag |= flg;
}

static void TransBmpWinChar( SRMAIN_WORK * wk )
{
	if( wk->bmpTransFlag & BMPWIN_TRANS_MAIN_FLAG ){
		GFL_BMPWIN_TransVramCharacter( wk->util[0].win );
	}
	if( wk->bmpTransFlag & BMPWIN_TRANS_SUB_FLAG ){
		GFL_BMPWIN_TransVramCharacter( wk->util[1].win );
	}
	wk->bmpTransFlag = 0;
}


//============================================================================================
//	3D
//============================================================================================
#ifdef	SRMAIN_DRAW_3D

//static void Poke3DMove( GFL_G3D_SCENEOBJ * obj, void * work );

// ���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES G3DUtilResTbl[] =
{
#if	PM_VERSION == LOCAL_VERSION
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_b_nsbmd, GFL_G3D_UTIL_RESARC },		// 00: ���f��
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_b_nsbca, GFL_G3D_UTIL_RESARC },		// 01: �A�j��
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_b_nsbtp, GFL_G3D_UTIL_RESARC },		// 02: �A�j��
#else
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_w_nsbmd, GFL_G3D_UTIL_RESARC },		// 00: ���f��
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_w_nsbca, GFL_G3D_UTIL_RESARC },		// 01: �A�j��
	{ ARCID_STAFF_ROLL, NARC_staff_roll_staffroll_w_nsbtp, GFL_G3D_UTIL_RESARC },		// 02: �A�j��
#endif
};

// �A�j���e�[�u��
static const GFL_G3D_UTIL_ANM G3DUtil_AnmTbl[] =
{
	{ 1, 0 },		// 00: �A�j��
	{ 2, 0 },		// 01: �A�j��
};

// 3D OBJ�e�[�u��
static const GFL_G3D_UTIL_OBJ G3DUtilObjTbl[] =
{
	{ 0, 0, 0, G3DUtil_AnmTbl, NELEMS(G3DUtil_AnmTbl) },
};

// �ݒ�f�[�^
static const GFL_G3D_UTIL_SETUP G3DUtilSetup = {
	G3DUtilResTbl, NELEMS(G3DUtilResTbl),
	G3DUtilObjTbl, NELEMS(G3DUtilObjTbl),
};

/*	�풓���傫���Ȃ�̂Ŏg���Ȃ�...
// 3D OBJ�f�[�^
static const GFL_G3D_SCENEOBJ_DATA	ObjData[] =
{
	{	// �|�P����
		0,			// OBJ ID
		0,			// ����v���C�I���e�B
		0,			// �\���v���C�I���e�B
		31,			// �A���t�@�u�����h
		FALSE,		// �J�����O�t���O
		TRUE,		// drowSW (????)
		{	//	�I�u�W�F�N�g�`����
			{ 0, 0, 0 },										// trans
			{ FX32_ONE, FX32_ONE, FX32_ONE },					// scale
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	// rotate
		},
		Poke3DMove,	// ����֐�
	},
};
*/

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light_data[] = {
  { 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};

static const GFL_G3D_LIGHTSET_SETUP light3d_setup = { light_data, NELEMS(light_data) };

// �J�����ݒ�֘A
#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear      ( 1 << FX32_SHIFT )
#define cameraFar       ( 1024 << FX32_SHIFT )

// �R�c�J�����ݒ�
/*
#define	CAMERA_POS_X			( 98978 )
#define	CAMERA_POS_Y			( 177097 )
#define	CAMERA_POS_Z			( 214916 )
#define	CAMERA_TARGET_X		( 0 )
#define	CAMERA_TARGET_Y		( 124745 )
#define	CAMERA_TARGET_Z		( 0 )
*/
#define	CAMERA_POS_X			( 3681 )
#define	CAMERA_POS_Y			( 94888 )
#define	CAMERA_POS_Z			( 42318 )
#define	CAMERA_TARGET_X		( 0 )
#define	CAMERA_TARGET_Y		( 124745 )
#define	CAMERA_TARGET_Z		( 0 )


#ifdef PM_DEBUG
//static VecFx32 test_pos    = { 0, BADGE3D_CAMERA_POS_Y, BADGE3D_CAMERA_POS_Z };
//static VecFx32 test_target = { 0, BADGE3D_CAMERA_TARGET_Y, 0 };
static VecFx32 test_pos    = { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z };
static VecFx32 test_target = { CAMERA_TARGET_X, CAMERA_TARGET_Y, CAMERA_TARGET_Z };
static int     moveflag;
#endif


static void Init3D( SRMAIN_WORK * wk )
{
//	OS_Printf( "heap size [3] = 0x%x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_STAFF_ROLL) );

	// �R�c�V�X�e���N��
	GFL_G3D_Init(
		GFL_G3D_VMANLNK,		// �e�N�X�`���}�l�[�W���g�p���[�h�i�t���[�����[�h�i�Ƃ���ςȂ��j�j
		GFL_G3D_TEX256K,		// �e�N�X�`���}�l�[�W���T�C�Y 128KB(1�u���b�N)
		GFL_G3D_VMANLNK,		// �p���b�g�}�l�[�W���g�p���[�h�i�t���[�����[�h�i�Ƃ���ςȂ��j�j
		GFL_G3D_PLT64K,			// �p���b�g�}�l�[�W���T�C�Y
		0x1000,							// �W�I���g���o�b�t�@�̎g�p�T�C�Y
		HEAPID_STAFF_ROLL,	// �q�[�vID
		NULL );							// �Z�b�g�A�b�v�֐�(NULL�̎���DefaultSetUp)

	// �n���h���쐬
	wk->g3d_util = GFL_G3D_UTIL_Create( 3, 1, HEAPID_STAFF_ROLL );
	wk->g3d_unit = GFL_G3D_UTIL_AddUnit( wk->g3d_util, &G3DUtilSetup );

/*	�풓���傫���Ȃ�̂Ŏg���Ȃ�...
	// �Ǘ��V�X�e���쐬
	wk->g3d_scene = GFL_G3D_SCENE_Create(
										wk->g3d_util,					// �ˑ�����g3Dutil
										1000,									// �z�u�\�ȃI�u�W�F�N�g��
										4,										// �P�I�u�W�F�N�g�Ɋ��蓖�Ă郏�[�N�̃T�C�Y
										32,										// �A�N�Z�T����
										FALSE,								// �p�[�e�B�N���V�X�e���̋N���t���O
										HEAPID_STAFF_ROLL );	// �q�[�vID

	// OBJ�ǉ�
	wk->g3d_obj_id = GFL_G3D_SCENEOBJ_Add( wk->g3d_scene, ObjData, NELEMS(ObjData), 0 );

	{
		GFL_G3D_SCENEOBJ * obj = GFL_G3D_SCENEOBJ_Get( wk->g3d_scene, wk->g3d_obj_id );
		GFL_G3D_SCENEOBJ_DisableAnime( obj, 0 );
		GFL_G3D_SCENEOBJ_ResetAnimeFrame( obj, 0 );
		GFL_G3D_SCENEOBJ_EnableAnime( obj, 0 );
	}
*/
	{
		GFL_G3D_OBJ * obj;
		int	anm;
		u32	i;

		obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->g3d_unit );
		anm = GFL_G3D_OBJECT_GetAnimeCount( obj );

		for( i=0; i<anm; i++ ){
			GFL_G3D_OBJECT_EnableAnime( obj, i );
//			GFL_G3D_OBJECT_SetAnimeFrame( obj, i, 0 );
		}
	}

  // ���C�g�쐬
	wk->g3d_light = GFL_G3D_LIGHT_Create( &light3d_setup, HEAPID_STAFF_ROLL );
  GFL_G3D_LIGHT_Switching( wk->g3d_light );
/*
  // �J���������ݒ�
  {
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 }; 
    proj.param1 = FX_SinIdx( cameraPerspway ); 
    proj.param2 = FX_CosIdx( cameraPerspway ); 
    GFL_G3D_SetSystemProjection( &proj ); 
  }
  {	// �J�����쐬
    VecFx32    pos = { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z };
    VecFx32 target = { CAMERA_TARGET_X, CAMERA_TARGET_Y, CAMERA_TARGET_Z };
    wk->g3d_camera = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_STAFF_ROLL );
		GFL_G3D_CAMERA_Switching( wk->g3d_camera );
  }
*/
	{
		VecFx32 pos			= { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z };
		VecFx32 up			= { 0, FX32_ONE, 0 };
		VecFx32 target	= { CAMERA_TARGET_X, CAMERA_TARGET_Y, CAMERA_TARGET_Z };

		wk->g3d_camera = GFL_G3D_CAMERA_Create(
											GFL_G3D_PRJPERS, 
											FX_SinIdx( defaultCameraFovy/2 * PERSPWAY_COEFFICIENT ),
											FX_CosIdx( defaultCameraFovy/2 * PERSPWAY_COEFFICIENT ),
											defaultCameraAspect, 0,
											defaultCameraNear, defaultCameraFar, 0,
											&pos, &up, &target, HEAPID_STAFF_ROLL );
		GFL_G3D_CAMERA_Switching( wk->g3d_camera );
	}


	G3X_AntiAlias( TRUE );	// �Z�b�g�A�b�v�֐��ō�����ق����������ǁB�B�B

	GFL_BG_SetBGControl3D( 2 );				// BG�ʐݒ�i�����͗D��x�j
//	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	CameraMoveInit( &wk->cameraMove );
//	CameraMoveFlagSet( &wk->cameraMove, TRUE );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );

#if	PM_VERSION == LOCAL_VERSION
	G2_SetBlendBrightness( GX_PLANEMASK_BG0, -16 );
#else
	G2_SetBlendBrightness( GX_PLANEMASK_BG0, 16 );
#endif

}

static void Exit3D( SRMAIN_WORK * wk )
{
	GFL_G3D_CAMERA_Delete( wk->g3d_camera );
	GFL_G3D_LIGHT_Delete( wk->g3d_light );

/*	�풓���傫���Ȃ�̂Ŏg���Ȃ�...
	GFL_G3D_SCENEOBJ_Remove( wk->g3d_scene, wk->g3d_obj_id, NELEMS(ObjData) );
	GFL_G3D_SCENE_Delete( wk->g3d_scene );
*/
	GFL_G3D_UTIL_DelUnit( wk->g3d_util, wk->g3d_unit );
	GFL_G3D_UTIL_Delete( wk->g3d_util );

	GFL_G3D_Exit();
}


static void Main3D( SRMAIN_WORK * wk )
{
	if( ( GFL_DISP_GetMainVisible() & GX_PLANEMASK_BG0 ) == 0 ){
		return;
	}

#ifdef PM_DEBUG
	DebugCameraPrint( wk );
#endif

/*	�풓���傫���Ȃ�̂Ŏg���Ȃ�...
	{
		GFL_G3D_SCENEOBJ * obj = GFL_G3D_SCENEOBJ_Get( wk->g3d_scene, wk->g3d_obj_id );
		GFL_G3D_SCENEOBJ_LoopAnimeFrame( obj, 0, FX32_ONE );
	}

	GFL_G3D_SCENE_Main( wk->g3d_scene );
	GFL_G3D_SCENE_Draw( wk->g3d_scene );
*/

	ChangeBrightness3D( wk );

	CameraMoveMain( wk );

	{
    GFL_G3D_OBJ * obj;
		int	anm;
		u32	i;

		obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->g3d_unit );
		anm = GFL_G3D_OBJECT_GetAnimeCount( obj );

		for( i=0; i<anm; i++ ){
			GFL_G3D_OBJECT_LoopAnimeFrame( obj, i, FX32_ONE );
		}
	}

	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();

	{
		GFL_G3D_OBJSTATUS	st =
		{
			{ 0, 0, 0 },																				// trans
			{ FX32_ONE, FX32_ONE, FX32_ONE },										// scale
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	// rotate
		};
		GFL_G3D_OBJ * obj;

		obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->g3d_unit );
		GFL_G3D_DRAW_DrawObject( obj, &st );
	}

	GFL_G3D_DRAW_End();
}

/*
static void Poke3DMove( GFL_G3D_SCENEOBJ * obj, void * work )
{
}
*/

static const SR3DCAMERA_PARAM CameraMoveTable[] =
{
	{ { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z }, { CAMERA_TARGET_X, CAMERA_TARGET_Y, CAMERA_TARGET_Z } },
	{ { 1636, 95706, 63177 }, { 0, 124745, 0 } },
	{ { -5317, 111657, 78310 }, { 0, 124745, 0 } },
	{ { -7362, 159919, 115938 }, { 32311, 124745, 0 } },
	{ { -64622, 150921, 81582 }, { 245809, 108794, 129244 } },
	{ { -17178, 158283, 90580 }, { 199183, 98978, -41718 } },
	{ { -18405, 189776, 93443 }, { 959514, 154602, 0 } },
};

static void CameraMoveInit( SR3DCAMERA_MOVE * mvwk )
{
	u32	i;

	mvwk->tbl    = CameraMoveTable;
	mvwk->tblMax = NELEMS(CameraMoveTable);
	mvwk->cnt    = 0;
//	mvwk->cntMax = 0;
	mvwk->pos    = 0;
	mvwk->flg = FALSE;

	for( i=0; i<SRMAIN_CAMERA_REQ_MAX; i++ ){
		mvwk->req[i].no  = 0xffff;
		mvwk->req[i].cnt = 0;
	}
}

static void CameraMoveRequestSet( SR3DCAMERA_MOVE * mvwk, u16 no, u16 cnt )
{
	u32	i;

	for( i=0; i<SRMAIN_CAMERA_REQ_MAX; i++ ){
		if( mvwk->req[i].no == 0xffff ){
			mvwk->req[i].no  = no;
			mvwk->req[i].cnt = cnt;
			break;
		}
	}
}

static void CameraMoveRequestShift( SR3DCAMERA_MOVE * mvwk )
{
	u32	i;

	for( i=0; i<SRMAIN_CAMERA_REQ_MAX-1; i++ ){
		mvwk->req[i] = mvwk->req[i+1];
	}
	mvwk->req[SRMAIN_CAMERA_REQ_MAX-1].no  = 0xffff;
	mvwk->req[SRMAIN_CAMERA_REQ_MAX-1].cnt = 0;
}

static VecFx32 CameraMoveValMake( const VecFx32 * now, const VecFx32 * mvp, u32 cnt )
{
	VecFx32	vec;

	vec.x = ( GFL_STD_Abs(now->x-mvp->x) << 8 ) / cnt;
	if( now->x > mvp->x ){
		vec.x *= -1;
	}
	vec.y = ( GFL_STD_Abs(now->y-mvp->y) << 8 ) / cnt;
	if( now->y > mvp->y ){
		vec.y *= -1;
	}
	vec.z = ( GFL_STD_Abs(now->z-mvp->z) << 8 ) / cnt;
	if( now->z > mvp->z ){
		vec.z *= -1;
	}
	return vec;
}

static VecFx32 CameraMoveCore( const VecFx32 * p, const VecFx32 * mv, u32 cnt )
{
	VecFx32	vec;
	fx32	val;

	vec = *p;

	val = ( GFL_STD_Abs(mv->x) * cnt ) >> 8;
	if( mv->x > 0 ){
		vec.x += val;
	}else{
		vec.x -= val;
	}
	val = ( GFL_STD_Abs(mv->y) * cnt ) >> 8;
	if( mv->y > 0 ){
		vec.y += val;
	}else{
		vec.y -= val;
	}
	val = ( GFL_STD_Abs(mv->z) * cnt ) >> 8;
	if( mv->z > 0 ){
		vec.z += val;
	}else{
		vec.z -= val;
	}
	return vec;
}

static void CameraMoveMain( SRMAIN_WORK * wk )
{
	SR3DCAMERA_MOVE * mvwk = &wk->cameraMove;

	if( mvwk->flg == FALSE ){
		return;
	}

	if( mvwk->cnt == 0 ){
		VecFx32	vec;

		mvwk->cntMax = mvwk->req[0].cnt;

		GFL_G3D_CAMERA_GetPos( wk->g3d_camera, &vec );
		mvwk->val.pos = CameraMoveValMake( &vec, &mvwk->tbl[mvwk->req[0].no].pos, mvwk->cntMax );

		GFL_G3D_CAMERA_GetTarget( wk->g3d_camera, &vec );
		mvwk->val.target = CameraMoveValMake( &vec, &mvwk->tbl[mvwk->req[0].no].target, mvwk->cntMax );
	}

	if( mvwk->cnt >= mvwk->cntMax ){
		mvwk->param.pos    = mvwk->tbl[mvwk->req[0].no].pos;
		mvwk->param.target = mvwk->tbl[mvwk->req[0].no].target;
		mvwk->pos = mvwk->req[0].no;
		mvwk->cnt = 0;
		CameraMoveRequestShift( mvwk );
		if( mvwk->req[0].no == 0xffff ){
			CameraMoveFlagSet( mvwk, FALSE );
		}
	}else{
		mvwk->param.pos    = CameraMoveCore( &mvwk->tbl[mvwk->pos].pos, &mvwk->val.pos, mvwk->cnt );
		mvwk->param.target = CameraMoveCore( &mvwk->tbl[mvwk->pos].target, &mvwk->val.target, mvwk->cnt );
		if( wk->skipFlag == 1 ){
			mvwk->cnt += SKIP_SPEED;
		}else{
			mvwk->cnt++;
		}
	}

	GFL_G3D_CAMERA_SetPos( wk->g3d_camera, &mvwk->param.pos );
	GFL_G3D_CAMERA_SetTarget( wk->g3d_camera, &mvwk->param.target );
	GFL_G3D_CAMERA_Switching( wk->g3d_camera );
}

static void CameraMoveFlagSet( SR3DCAMERA_MOVE * mvwk, BOOL flg )
{
	mvwk->flg = flg;
}


#endif	// SRMAIN_DRAW_3D



//============================================================================================
//	���̑�
//============================================================================================

static int SetFadeIn( SRMAIN_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAFF_ROLL );
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}

static int SetFadeOut( SRMAIN_WORK * wk, int next )
{
#if	PM_VERSION == LOCAL_VERSION
	// �u���b�N�A�E�g
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAFF_ROLL );
#else
	// �z���C�g�A�E�g
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_WHITE, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAFF_ROLL );
#endif
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}

#define	G3D_FADEIN_SPEED	( FX32_ONE / 16 )

static void ChangeBrightness3D( SRMAIN_WORK * wk )
{
	if( wk->g3d_briCount > FX32_CONST(16) ){ return; }

	if( wk->skipFlag == 1 ){
		wk->g3d_briCount += ( G3D_FADEIN_SPEED * SKIP_SPEED );
	}else{
		wk->g3d_briCount += G3D_FADEIN_SPEED;
	}

#if	PM_VERSION == LOCAL_VERSION
	G2_SetBlendBrightness( GX_PLANEMASK_BG0, -16+wk->g3d_briCount/FX32_ONE );
#else
	G2_SetBlendBrightness( GX_PLANEMASK_BG0, 16-wk->g3d_briCount/FX32_ONE );
#endif
}


static void CheckSkip( SRMAIN_WORK * wk )
{
	if( wk->dat->fastMode == TRUE ){
		if( wk->skipCount == 0 ){
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ){
				wk->skipFlag = 1;
			}else{
				wk->skipFlag = 0;
			}
		}
	}

	wk->skipCount = ( wk->skipCount + 1 ) & 3;
}


#define	ITEMLIST_SCROLL_SPEED		( FX32_CONST(1) )


static void ShiftChar( u32 * src, u32 * next, u32 shift )
{
	u32	i;

	if( next != NULL ){
		for( i=0; i<shift; i++ ){
			next[8-shift+i] = src[i];
		}
	}
	for( i=0; i<8-shift; i++ ){
		src[i] = src[shift+i];
	}
}

static void ListScroll( SRMAIN_WORK * wk )
{
	if( wk->listScrollFlg == FALSE ){ return; }

	wk->listScrollCnt += ITEMLIST_SCROLL_SPEED;
	if( wk->listScrollCnt >= FX32_ONE ){
		wk->listScrollCnt -= FX32_ONE;
	}else{
		return;
	}

	// �����ŃV�t�g����
	{
		u8 * mBmp;
		u8 * sBmp;
		u32 p1;
		u32 p2;
		u32	shift;
		u16	i, j;

		// �V�t�g�l
		if( wk->skipFlag == 0 ){
			shift = 1;
		}else{
			shift = SKIP_SPEED;
		}

		mBmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->util[0].win) );
		sBmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->util[1].win) );

		// ���ʂ��V�t�g
		p1 = 0;
		for( j=0; j<32; j++ ){
			ShiftChar( (u32 *)&mBmp[p1], NULL, shift );
			p1 += 0x20;
		}
		for( i=1; i<25; i++ ){
			p1 = ( i << 5 ) << 5;
			p2 = ( ( i - 1 ) << 5 ) << 5;
			for( j=0; j<32; j++ ){
				ShiftChar( (u32 *)&mBmp[p1], (u32 *)&mBmp[p2], shift );
				p1 += 0x20;
				p2 += 0x20;
			}
		}

		// ����ʂ��V�t�g
		p1 = 0;
		p2 = ( 24 << 5 ) << 5;
		for( j=0; j<32; j++ ){
			ShiftChar( (u32 *)&sBmp[p1], (u32 *)&mBmp[p2], shift );
			p1 += 0x20;
			p2 += 0x20;
		}
		for( i=1; i<28; i++ ){
			p1 = ( i << 5 ) << 5;
			p2 = ( ( i - 1 ) << 5 ) << 5;
			for( j=0; j<32; j++ ){
				ShiftChar( (u32 *)&sBmp[p1], (u32 *)&sBmp[p2], shift );
				p1 += 0x20;
				p2 += 0x20;
			}
		}

//		wk->bmpShitfFlag = 1;
		SetBmpWinTransFlag( wk, BMPWIN_TRANS_MAIN_FLAG|BMPWIN_TRANS_SUB_FLAG );
	}
}

// �����`��
static void PrintStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item, GFL_BMP_DATA * bmp, u32 py )
{
	u32	px;

	GFL_MSG_GetString( wk->mman, item->msgIdx, wk->exp );
	// ���l��
	if( item->putMode == STR_PUT_MODE_LEFT ){
		px = item->px + item->offs_x;
	// �E�l��
	}else if( item->putMode == STR_PUT_MODE_RIGHT ){
		px = item->px + item->offs_x - PRINTSYS_GetStrWidth( wk->exp, wk->font[item->font], 0 );
	// ����
	}else if( item->putMode == STR_PUT_MODE_CENTER ){
		px = item->px + item->offs_x - ( PRINTSYS_GetStrWidth( wk->exp, wk->font[item->font], 0 ) >> 1 );
	}else{
		// �A�T�[�g
	}
	PRINTSYS_PrintColor( bmp, px, py, wk->exp, wk->font[item->font], FontColorTbl[item->color] );
}

// ��s������쐬
static void MakeScrollStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	if( item->font == SRMAIN_FONT_NORMAL ){
		PrintStr( wk, item, GFL_BMPWIN_GetBmp(wk->util[1].win), 192 );
	}else{
		PrintStr( wk, item, GFL_BMPWIN_GetBmp(wk->util[1].win), 192+2 );
	}
}

// �ꊇ�`�敶����쐬
static void MakePutStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	GFL_BMP_DATA * bmp;
	u32	py;

	if( item->wait < 256 ){
		bmp = GFL_BMPWIN_GetBmp( wk->util[0].win );
		py  = item->wait;
		wk->putFrame |= BMPWIN_TRANS_MAIN_FLAG;
	}else{
		bmp = GFL_BMPWIN_GetBmp( wk->util[1].win );
		py  = item->wait - 256;
		wk->putFrame |= BMPWIN_TRANS_SUB_FLAG;
	}

	PrintStr( wk, item, bmp, py );
}


static BOOL PutStr( SRMAIN_WORK * wk )
{
	switch( wk->labelSeq ){
	case 0:
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD|GX_BLEND_PLANEMASK_BG0, 0, 16 );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD|GX_BLEND_PLANEMASK_BG0, 0, 16 );
		}
		wk->bmpTransFlag = wk->putFrame;
		wk->labelSeq++;
		break;

	case 1:
		if( wk->skipFlag == 1 ){
			wk->britness += ( STR_FADE_SPEED * SKIP_SPEED );
		}else{
			wk->britness += STR_FADE_SPEED;
		}
		{
			s32	p1, p2;
			if( wk->britness > 16 ){
				p1 = 16;
				p2 = 0;
			}else{
				p1 = wk->britness;
				p2 = 16 - wk->britness;
			}
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				G2_ChangeBlendAlpha( p1, p2 );
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				G2S_ChangeBlendAlpha( p1, p2 );
			}
		}
		if( wk->britness >= 16 ){
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				G2_BlendNone();
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				G2S_BlendNone();
			}
			wk->britness = 0;
			wk->labelSeq = 0;
			return FALSE;
		}
		break;
	}
	return TRUE;
}

static BOOL ClearStr( SRMAIN_WORK * wk )
{
	switch( wk->labelSeq ){
	case 0:
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD|GX_BLEND_PLANEMASK_BG0, 16, 0 );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD|GX_BLEND_PLANEMASK_BG0, 16, 0 );
		}
		wk->labelSeq++;

	case 1:
		if( wk->skipFlag == 1 ){
			wk->britness += ( STR_FADE_SPEED * SKIP_SPEED );
		}else{
			wk->britness += STR_FADE_SPEED;
		}
		{
			s32	p1, p2;
			if( wk->britness > 16 ){
				p1 = 0;
				p2 = 16;
			}else{
				p1 = 16 - wk->britness;
				p2 = wk->britness;
			}
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				G2_ChangeBlendAlpha( p1, p2 );
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				G2S_ChangeBlendAlpha( p1, p2 );
			}
		}
		if( wk->britness >= 16 ){
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[0].win), 0 );
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[1].win), 0 );
			}
			wk->bmpTransFlag = wk->putFrame;
			wk->britness = 0;
			wk->labelSeq++;
		}
		break;

	case 2:
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_BlendNone();
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_BlendNone();
		}
		wk->putFrame = 0;
		wk->labelSeq = 0;
		return FALSE;
	}

	return TRUE;
}

static BOOL PutLogo( SRMAIN_WORK * wk )
{
	MtxFx22 mtx;

	// �g�k�P
	switch( wk->labelSeq ){
	case 0:
		LoadLogoPalette( wk, TRUE );
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG3, GX_BLEND_PLANEMASK_BD, 0, 16 );
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GX_SetMasterBrightness( 0 );
		GXS_SetMasterBrightness( 0 );
		wk->britness = 0;
		wk->labelSeq++;
		break;

	case 1:
		if( wk->skipFlag == 1 ){
			wk->britness += ( SKIP_SPEED * 2 );
		}else{
			wk->britness += 2;
		}
		if( wk->britness >= 16 ){
			G2_ChangeBlendAlpha( 16, 0 );
		  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
			GFL_BG_SetAffine( GFL_BG_FRAME3_M, &mtx, 128, 96 );
			wk->britness = 0;
			wk->listWait = LOGO_PUT_WAIT;
			wk->labelSeq++;
		}else{
			G2_ChangeBlendAlpha( wk->britness, 16-wk->britness );
		  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE*(8-wk->britness/2), FX32_ONE*(8-wk->britness/2), GFL_CALC2D_AFFINE_MAX_NORMAL );
			GFL_BG_SetAffine( GFL_BG_FRAME3_M, &mtx, 128, 96 );
		}
		break;

	case 2:
		if( wk->skipFlag == 1 ){
			wk->listWait -= SKIP_SPEED;
		}else{
			wk->listWait--;
		}
		if( wk->listWait <= 0 ){
			wk->listWait = 0;
			wk->labelSeq++;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
			OS_Printf( "[1] push a button : start %d, wait = %d\n", LOGO_PUT_WAIT, wk->listWait );
		}
		break;

	case 3:
		if( wk->skipFlag == 1 ){
			wk->britness += ( SKIP_SPEED * 2 );
		}else{
			wk->britness += 2;
		}
		if( wk->britness >= 16 ){
			G2_BlendNone();
			GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
			GFL_BG_SetAffine( GFL_BG_FRAME3_M, &mtx, 128, 96 );
			LoadLogoPalette( wk, FALSE );
			wk->britness = 0;
			wk->labelSeq = 0;
			return FALSE;
		}else{
			G2_ChangeBlendAlpha( 16-wk->britness, wk->britness );
		  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE*(1+wk->britness/2), FX32_ONE*(1+wk->britness/2), GFL_CALC2D_AFFINE_MAX_NORMAL );
			GFL_BG_SetAffine( GFL_BG_FRAME3_M, &mtx, 128, 96 );
		}
		break;
	}

	return TRUE;
}



//============================================================================================
//	�R�}���h
//============================================================================================

// ���x���F�Ȃ�
static BOOL Comm_LabelNone( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	// �E�F�C�g
	if( item->msgIdx == ITEMLIST_MSG_NONE ){
		wk->listWait = item->wait;
		wk->subSeq = SUBSEQ_WAIT;
		return TRUE;
	}

	// ��s������
	if( wk->listWait == 0 ){
//		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[1].win), 0, 192, 256, 64, 0 );
		wk->listWait = item->wait;
	}
	MakeScrollStr( wk, item );
	if( item[1].label != ITEMLIST_LABEL_NONE || item[1].wait != 0 ){
		wk->subSeq = SUBSEQ_WAIT;
		return TRUE;
	}

	return FALSE;
}

// ������ꊇ�\��
static BOOL Comm_LabelStrPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	// �\���^�C�v�ݒ�
	if( wk->subSeq == SUBSEQ_INIT ){
		wk->labelType = item->labelType;
		wk->subSeq = SUBSEQ_STR_PUT;
	}

	// �����`��
	MakePutStr( wk, item );

	// �����ꊇ�\������Ȃ��Ƃ�
	if( item[1].label != ITEMLIST_LABEL_STR_PUT ){
		return TRUE;
	}

	return FALSE;
}

// �\���N���A
static BOOL Comm_LabelStrClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	wk->labelType = item->labelType;
	wk->subSeq = SUBSEQ_STR_CLEAR;
	return TRUE;
}

// �X�N���[���J�n
static BOOL Comm_LabelScrollStart( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
//	OS_Printf( "���X�N���[���J�n\n" );
	wk->listScrollFlg = TRUE;
	return FALSE;
}

// �X�N���[����~
static BOOL Comm_LabelScrollStop( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
//	OS_Printf( "���X�N���[����~\n" );
	wk->listScrollFlg = FALSE;
	return FALSE;
}

// �I��
static BOOL Comm_LabelEnd( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	wk->subSeq = SUBSEQ_END;
	return TRUE;
}

// ���S�\��
static BOOL Comm_LabelLogoPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
//	wk->listWait = item->wait;
//	wk->subSeq = SUBSEQ_LOGO_PUT;
	return TRUE;
}

// �R�c�\��
static BOOL Comm_Label3DPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	return TRUE;
}

// �R�c�N���A
static BOOL Comm_Label3DClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	return TRUE;
}

// �R�c�J�����ړ����N�G�X�g
static BOOL Comm_Label3DCameraRequest( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	SR3DCAMERA_MOVE * mvwk = &wk->cameraMove;

	CameraMoveRequestSet( mvwk, item->labelType, item->wait );
	CameraMoveFlagSet( mvwk, TRUE );

	return TRUE;
}

// �o�[�W�����ʏ���
static BOOL Comm_LabelVersion( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
#if	PM_VERSION == LOCAL_VERSION
	item = &item[item->labelType];
#else
	item = &item[item->labelType+1];
#endif

	MakeScrollStr( wk, item );

	wk->listWait = item->wait;
	wk->subSeq = SUBSEQ_WAIT;

	return TRUE;
}


//============================================================================================
//	�f�o�b�O
//============================================================================================
#ifdef PM_DEBUG

static const u8 GridChar[] = {
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80,
	0x88, 0x88, 0x88, 0x88,

	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x60,
	0x88, 0x88, 0x88, 0x68,

	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x06, 0x00, 0x00, 0x80,
	0x86, 0x88, 0x88, 0x88,
};

static void DebugGridSet(void)
{
	GFL_BG_LoadCharacter( GFL_BG_FRAME2_M, GridChar, 0x60, 1 );
	GFL_BG_LoadCharacter( GFL_BG_FRAME3_S, GridChar, 0x60, 1 );

	GFL_BG_ClearScreenCode( GFL_BG_FRAME2_M, 0xf001 );
	GFL_BG_ClearScreenCode( GFL_BG_FRAME3_S, 0xf001 );

	GFL_BG_FillScreen( GFL_BG_FRAME2_M, 0xf002, 15, 0, 1, 24, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME3_S, 0xf002, 15, 0, 1, 24, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME2_M, 0xf003, 16, 0, 1, 24, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME3_S, 0xf003, 16, 0, 1, 24, GFL_BG_SCRWRT_PALIN );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME3_S );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}

static void DebugCameraPrint( SRMAIN_WORK * wk )
{
	if( wk->debugStopFlg == TRUE ){
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){
				test_pos.z -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){
				test_pos.z += FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){
				test_pos.y -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
				test_pos.y += FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){
				test_pos.x -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){
				test_pos.x += FX32_ONE/10;
			}
			GFL_G3D_CAMERA_SetPos( wk->g3d_camera, &test_pos );
			OS_Printf( "POS : x = %d, y = %d, z = %d\n", test_pos.x, test_pos.y, test_pos.z );
		}
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){
				test_target.z -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){
				test_target.z += FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){
				test_target.y -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
				test_target.y += FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){
				test_target.x -= FX32_ONE/10;
			}
			if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){
				test_target.x += FX32_ONE/10;
			}
			GFL_G3D_CAMERA_SetTarget( wk->g3d_camera, &test_target );
			OS_Printf( "TARGET : x = %d, y = %d, z = %d\n", test_target.x, test_target.y, test_target.z );
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			test_pos.x = CAMERA_POS_X;
			test_pos.y = CAMERA_POS_Y;
			test_pos.z = CAMERA_POS_Z;
			test_target.x = CAMERA_TARGET_X;
			test_target.y = CAMERA_TARGET_Y;
			test_target.z = CAMERA_TARGET_Z;
			GFL_G3D_CAMERA_SetPos( wk->g3d_camera, &test_pos );
			GFL_G3D_CAMERA_SetTarget( wk->g3d_camera, &test_target );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
			test_pos.x = 0;
			test_pos.y = 0;
			test_pos.z = 0;
			test_target.x = 0;
			test_target.y = 0;
			test_target.z = 0;
			GFL_G3D_CAMERA_SetPos( wk->g3d_camera, &test_pos );
			GFL_G3D_CAMERA_SetTarget( wk->g3d_camera, &test_target );
		}

	  GFL_G3D_CAMERA_Switching( wk->g3d_camera );
//	  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
//	    anime_speed = FX32_ONE;
//	  }else{
//			anime_speed = 0;
//	  }
	}
}

#endif // PM_DEBUG

