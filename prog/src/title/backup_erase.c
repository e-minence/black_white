//============================================================================================
/**
 * @file		backup_erase.c
 * @brief		�Z�[�u�f�[�^���������
 * @author	Hiroyuki Nakamura
 * @data		10/03/02
 *
 *	���W���[�����FBACKUP_ERASE
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "gamesystem/msgspeed.h"
#include "sound/pm_sndsys.h"
#include "print/printsys.h"
#include "title/title.h"
#include "app/app_keycursor.h"
#include "msg/msg_backup_erase.h"


#include "font/font.naix"
#include "backup_erase.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ����m�ۗp�q�[�v�h�c
#define	HEAPID_BACKUP_ERASE_L		( GFL_HEAP_LOWID(HEAPID_BACKUP_ERASE) )

typedef struct {
	GFL_TCB * vtask;					// TCB ( VBLANK )
	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	STRBUF * exp;							// ���b�Z�[�W�W�J�̈�
	PRINT_STREAM * stream;		// �v�����g�X�g���[��
	GFL_TCBLSYS * tcbl;
	GFL_BMPWIN * win;
	BMPMENU_WORK * mwk;
	APP_KEYCURSOR_WORK * kcwk;	// ���b�Z�[�W����J�[�\��
}BACKUP_ERASE_WORK;

// ���C���V�[�P���X
enum {
	MAINSEQ_INIT = 0,			// ������
	MAINSEQ_FADEIN,				// �t�F�[�h�C��
	MAINSEQ_CLEAR_MSG,		// ���b�Z�[�W�P
	MAINSEQ_CLEAR_YESNO,	// �͂��E�������P
	MAINSEQ_CHECK_MSG,		// ���b�Z�[�W�Q
	MAINSEQ_CHECK_YESNO,	// �͂��E�������Q
	MAINSEQ_ACTION_MSG,		// ���b�Z�[�W�R
	MAINSEQ_FADEOUT,			// �t�F�[�h�A�E�g
	MAINSEQ_RELEASE,			// �I��
};

// ���b�Z�[�W�E�B���h�E�L����
#define	WIN_CHAR_NUM		( 1 )

// �a�f�p���b�g
#define	WIN_PALETTE_M		( 14 )
#define	FONT_PALETTE_M	( 15 )

// BMPWIN
// ���b�Z�[�W
#define	BMPWIN_MSG_FRM			( GFL_BG_FRAME0_M )
#define	BMPWIN_MSG_PX				( 1 )
#define	BMPWIN_MSG_PY				( 19 )
#define	BMPWIN_MSG_SX				( 30 )
#define	BMPWIN_MSG_SY				( 4 )
#define	BMPWIN_MSG_PAL			( FONT_PALETTE_M )
// �͂��E������
#define	BMPWIN_YESNO_FRM		( GFL_BG_FRAME0_M )
#define	BMPWIN_YESNO_PX			( 24 )
#define	BMPWIN_YESNO_PY			( 13 )
#define	BMPWIN_YESNO_PAL		( FONT_PALETTE_M )

#define	EXP_BUF_SIZE		( 1024 )		// �ėp������o�b�t�@�T�C�Y

#define	BACK_GROUND_COLOR		( 0x7d8c )	// �w�i�J���[


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT BackupEraseProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BackupEraseProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BackupEraseProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void MainSeq_Init( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_FadeIn( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_ClearMessage( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_ClearYesNo( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_CheckMessage( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_CheckYesNo( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_ActionMessage( BACKUP_ERASE_WORK * wk, int * seq );
static void MainSeq_FadeOut( BACKUP_ERASE_WORK * wk, int * seq );

static void InitVram(void);
static void InitBg(void);
static void ExitBg(void);
static void InitMsg( BACKUP_ERASE_WORK * wk );
static void ExitMsg( BACKUP_ERASE_WORK * wk );
static void InitBmp( BACKUP_ERASE_WORK * wk );
static void ExitBmp( BACKUP_ERASE_WORK * wk );
static void InitVBlank( BACKUP_ERASE_WORK * wk );
static void ExitVBlank( BACKUP_ERASE_WORK * wk );

static void StartMessage( BACKUP_ERASE_WORK * wk, int strIdx );
static BOOL MainMessage( BACKUP_ERASE_WORK * wk );
static void SetYesNoMenu( BACKUP_ERASE_WORK * wk );
static void SetFadeIn(void);
static void SetFadeOut(void);


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC
const GFL_PROC_DATA BACKUP_ERASE_ProcData = {
  BackupEraseProc_Init,
  BackupEraseProc_Main,
  BackupEraseProc_End,
};




//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F������
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT BackupEraseProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BACKUP_ERASE_WORK * wk;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BACKUP_ERASE, 0x30000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(BACKUP_ERASE_WORK), HEAPID_BACKUP_ERASE );
	GFL_STD_MemClear( wk, sizeof(BACKUP_ERASE_WORK) );

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F���C��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT BackupEraseProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BACKUP_ERASE_WORK * wk = mywk;

	switch( *seq ){
	case MAINSEQ_INIT:					// ������
		MainSeq_Init( wk, seq );
		break;

	case MAINSEQ_FADEIN:				// �t�F�[�h�C��
		MainSeq_FadeIn( wk, seq );
		break;

	case MAINSEQ_CLEAR_MSG:			// ���b�Z�[�W�P
		MainSeq_ClearMessage( wk, seq );
		break;

	case MAINSEQ_CLEAR_YESNO:		// �͂��E�������P
		MainSeq_ClearYesNo( wk, seq );
		break;

	case MAINSEQ_CHECK_MSG:			// ���b�Z�[�W�Q
		MainSeq_CheckMessage( wk, seq );
		break;

	case MAINSEQ_CHECK_YESNO:		// �͂��E�������Q
		MainSeq_CheckYesNo( wk, seq );
		break;

	case MAINSEQ_ACTION_MSG:		// ���b�Z�[�W�R
		MainSeq_ActionMessage( wk, seq );
		break;

	case MAINSEQ_FADEOUT:				// �t�F�[�h�A�E�g
		MainSeq_FadeOut( wk, seq );
		break;

	case MAINSEQ_RELEASE:				// �I��
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F�I��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT BackupEraseProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_BACKUP_ERASE );

	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);

	return GFL_PROC_RES_FINISH;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F������
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_Init( BACKUP_ERASE_WORK * wk, int * seq )
{
	InitVram();
	InitBg();
	InitMsg( wk );
	InitBmp( wk );
	InitVBlank( wk );

	SetFadeIn();

	*seq = MAINSEQ_FADEIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�F�[�h�C���҂�
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_FadeIn( BACKUP_ERASE_WORK * wk, int * seq )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		StartMessage( wk, msg01 );
		*seq = MAINSEQ_CLEAR_MSG;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�������܂����H�v�\���҂�
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_ClearMessage( BACKUP_ERASE_WORK * wk, int * seq )
{
	if( MainMessage( wk ) == FALSE ){
		SetYesNoMenu( wk );
		*seq = MAINSEQ_CLEAR_YESNO;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�������܂����H�v�͂��E������
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_ClearYesNo( BACKUP_ERASE_WORK * wk, int * seq )
{
	switch( BmpMenu_YesNoSelectMain( wk->mwk ) ){
	case 0:
		StartMessage( wk, msg02 );
		*seq = MAINSEQ_CHECK_MSG;
		break;

	case BMPMENU_CANCEL:
		SetFadeOut();
		*seq = MAINSEQ_FADEOUT;
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�{���ɏ������܂����H�v�\���҂�
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_CheckMessage( BACKUP_ERASE_WORK * wk, int * seq )
{
	if( MainMessage( wk ) == FALSE ){
		SetYesNoMenu( wk );
		*seq = MAINSEQ_CHECK_YESNO;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�{���ɏ������܂����H�v�͂��E������
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_CheckYesNo( BACKUP_ERASE_WORK * wk, int * seq )
{
	switch( BmpMenu_YesNoSelectMain( wk->mwk ) ){
	case 0:
		StartMessage( wk, msg03 );
		*seq = MAINSEQ_ACTION_MSG;
		break;

	case BMPMENU_CANCEL:
		SetFadeOut();
		*seq = MAINSEQ_FADEOUT;
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�����Ă��܂��v�\���҂�
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_ActionMessage( BACKUP_ERASE_WORK * wk, int * seq )
{
	if( MainMessage( wk ) == FALSE ){
		SAVE_CONTROL_WORK * sv;
		u32	i;
		sv = SaveControl_GetPointer();
		SaveControl_Erase( sv );
		for( i=0; i<SAVE_EXTRA_ID_MAX; i++ ){
			if( SaveControl_Extra_Load( sv, i, HEAPID_BACKUP_ERASE ) == LOAD_RESULT_OK ){
				SaveControl_Extra_Erase( sv, i, HEAPID_BACKUP_ERASE );
			}
			SaveControl_Extra_Unload( sv, i );
		}
		OS_ResetSystem(0);		// �Z�[�u�ǂݍ��ݏ󋵂��X�V����ׁA�\�t�g���Z�b�g����

//		*seq = MAINSEQ_FADEOUT;		// ���Z�b�g�����̂ł���Ȃ��B
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�F�[�h�A�E�g�҂�
 *
 * @param		wk			�Z�[�u���������[�N
 * @param		seq			�V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainSeq_FadeOut( BACKUP_ERASE_WORK * wk, int * seq )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		ExitVBlank( wk );
		ExitBmp( wk );
		ExitMsg( wk );
		ExitBg();
		*seq = MAINSEQ_RELEASE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitVram(void)
{
	const GFL_DISP_VRAM tbl = {
		GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,				// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
	};

	GFL_DISP_ClearVRAM( NULL );
	GFL_DISP_SetBank( &tbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBg(void)
{
	GFL_BG_Init( HEAPID_BACKUP_ERASE );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, BACK_GROUND_COLOR );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, BACK_GROUND_COLOR );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A������
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitMsg( BACKUP_ERASE_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_backup_erase_dat, HEAPID_BACKUP_ERASE );
	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_BACKUP_ERASE );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_BACKUP_ERASE );
	wk->tcbl = GFL_TCBL_Init( HEAPID_BACKUP_ERASE, HEAPID_BACKUP_ERASE, 1, 4 );
	wk->kcwk = APP_KEYCURSOR_Create( 15, TRUE, FALSE, HEAPID_BACKUP_ERASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A���
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitMsg( BACKUP_ERASE_WORK * wk )
{
	APP_KEYCURSOR_Delete( wk->kcwk );
  GFL_TCBL_Exit( wk->tcbl );
	GFL_STR_DeleteBuffer( wk->exp );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o������
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBmp( BACKUP_ERASE_WORK * wk )
{
	GFL_BMPWIN_Init( HEAPID_BACKUP_ERASE );

	wk->win = GFL_BMPWIN_Create(
							BMPWIN_MSG_FRM, BMPWIN_MSG_PX, BMPWIN_MSG_PY,
							BMPWIN_MSG_SX, BMPWIN_MSG_SY, BMPWIN_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

	// �V�X�e���E�B���h�E
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME0_M, WIN_CHAR_NUM, WIN_PALETTE_M, MENU_TYPE_SYSTEM, HEAPID_BACKUP_ERASE );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr,
		PALTYPE_MAIN_BG, FONT_PALETTE_M*0x20, 0x20, HEAPID_BACKUP_ERASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o���
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBmp( BACKUP_ERASE_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->win );
	GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK
 *
 * @param		tcb			GFL_TCB
 * @param		work		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	GFL_BG_VBlankFunc();

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�ݒ�
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitVBlank( BACKUP_ERASE_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK���
 *
 * @param		wk			�Z�[�u���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitVBlank( BACKUP_ERASE_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�\���J�n
 *
 * @param		wk				�Z�[�u���������[�N
 * @param		strIdx		������C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void StartMessage( BACKUP_ERASE_WORK * wk, int strIdx )
{
  GFL_MSG_GetString( wk->mman, strIdx, wk->exp );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), 15 );
	BmpWinFrame_Write(
		wk->win, WINDOW_TRANS_OFF, WIN_CHAR_NUM, WIN_PALETTE_M );

	wk->stream = PRINTSYS_PrintStream(
								wk->win,
								0, 0, wk->exp,
								wk->font,
								MSGSPEED_GetWait(),
								wk->tcbl,
								10,		// tcbl pri
								HEAPID_BACKUP_ERASE,
								15 );	// clear color
	GFL_BMPWIN_MakeTransWindow_VBlank( wk->win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W���C��
 *
 * @param		wk		�Z�[�u���������[�N
 *
 * @retval	"TRUE = ���b�Z�[�W������"
 * @retval	"FALSE = ���b�Z�[�W�����I��"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainMessage( BACKUP_ERASE_WORK * wk )
{
  GFL_TCBL_Main( wk->tcbl );

  switch( PRINTSYS_PrintStreamGetState(wk->stream) ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      PRINTSYS_PrintStreamShortWait( wk->stream, 0 );
    }
    break;

  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      PRINTSYS_PrintStreamReleasePause( wk->stream );
    }
    break;

  case PRINTSTREAM_STATE_DONE: //�I��
    PRINTSYS_PrintStreamDelete( wk->stream );
		return FALSE;
	}

	APP_KEYCURSOR_Main( wk->kcwk, wk->stream, wk->win );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�������ݒ�
 *
 * @param		wk		�Z�[�u���������[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
static void SetYesNoMenu( BACKUP_ERASE_WORK * wk )
{
	BMPWIN_YESNO_DAT	dat;

	dat.frmnum = BMPWIN_YESNO_FRM;
	dat.pos_x  = BMPWIN_YESNO_PX;
	dat.pos_y  = BMPWIN_YESNO_PY;
	dat.palnum = BMPWIN_YESNO_PAL;
	dat.chrnum = 0;

	wk->mwk = BmpMenu_YesNoSelectInit(
							&dat, WIN_CHAR_NUM, WIN_PALETTE_M, 1, HEAPID_BACKUP_ERASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���ݒ�
 *
 * @param		none
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
static void SetFadeIn(void)
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BACKUP_ERASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�ݒ�
 *
 * @param		none
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
static void SetFadeOut(void)
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BACKUP_ERASE );
}
