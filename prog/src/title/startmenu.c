//============================================================================================
/**
 * @file		startmenu.c
 * @brief		�ŏ�����E����������s���g�b�v���j���[
 * @author	ariizumi
 * @author	Hiroyuki Nakamura
 * @data		09/01/07
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"
#include "system/blink_palanm.h"
#include "system/bgwinfrm.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "system/ds_system.h"
#include "savedata/mystatus.h"
#include "savedata/playtime.h"
#include "savedata/zukan_savedata.h"
#include "savedata/situation.h"
#include "savedata/misc.h"
#include "savedata/c_gear_data.h"
#include "sound/pm_sndsys.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "msg/msg_startmenu.h"
#include "field/zonedata.h"
#include "net/dwc_raputil.h"
#include "net_app/mystery.h"
#include "net/delivery_beacon.h"
#include "net_app/wifibattlematch.h"
#include "app/mictest.h"
#include "app/app_keycursor.h"
#include "gamesystem/msgspeed.h"
#include "multiboot/mb_parent_sys.h"  //�]���}�V��

#include "title/title.h"
#include "title/game_start.h"
#include "title/startmenu.h"
#include "title/startmenu_def.h"

#include "font/font.naix"
#include "startmenu.naix"
#include "wifileadingchar.naix"

#include "title_def.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"  // SYS_FLAG_SPEXIT_REQUEST
#include "field/eventwork.h"	//EVENTWORK_
#include "field/location.h"


//============================================================================================
//	�萔��`
//============================================================================================

//���[�N
typedef struct {

	SAVE_CONTROL_WORK * savedata;
  MYSTATUS * mystatus;
	MISC * misc;
	CGEAR_SAVEDATA * cgear;
	EVENTWORK * evwk;

	GFL_TCB * vtask;					// TCB ( VBLANK )

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[OBJ_ID_MAX];
	u32	chrRes[CHRRES_MAX];
	u32	palRes[PALRES_MAX];
	u32	celRes[CELRES_MAX];

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;						// �P��Z�b�g
	STRBUF * exp;							// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;					// �v�����g�L���[
	PRINT_STREAM * stream;		// �v�����g�X�g���[��
	GFL_TCBLSYS * tcbl;
	APP_KEYCURSOR_WORK * kcwk;	// ���b�Z�[�W����J�[�\��
	BOOL	stream_clear_flg;

	BMPMENU_WORK * mwk;

	PRINT_UTIL	util[BMPWIN_MAX];
	PRINT_UTIL	utilWin;

//	DELIVERY_BEACON_WORK * bwk;		//�u�s�v�c�ȑ��蕨�v�r�[�R���Ǘ����[�N
//	BOOL	hushigiFlag;						//�u�s�v�c�ȑ��蕨�v����M���t���O TRUE = ����

	u16 * listFrame[LIST_ITEM_MAX];

	BGWINFRM_WORK * wfrm;

	BLINKPALANM_WORK * blink;		// �J�[�\���A�j�����[�N

	u8	list[LIST_ITEM_MAX];
	u8	listPos;
	u8	listResult;

	u8	subSeq;
	u8	wait;
	u8	continueRet;

	u8	btnSeq;
	u8	btnCnt;

	s8	cursorPutPos;
	int	bgScroll;
	int	bgScrollCount;
	int	bgScrollSpeed;

	int	mainSeq;
	int	nextSeq;
	int	fadeSeq;

	u32	svBreak;		// �Z�[�u�f�[�^�j���t���O
	u32	svBreakPos;	// �Z�[�u�f�[�^�j�����b�Z�[�W�\���ʒu
 
}START_MENU_WORK;

// ���X�g���ڃf�[�^
typedef struct {
	u16	scrnIdx;		// �X�N���[���C���f�b�N�X
	u16	sy;					// �x�T�C�Y
}LIST_ITEM_DATA;

enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_FADE,
	MAINSEQ_BUTTON_ANM,

	MAINSEQ_INIT_WAIT,
	MAINSEQ_MAIN,
	MAINSEQ_SCROLL,

	MAINSEQ_CONTINUE,
	MAINSEQ_NEWGAME,
//	MAINSEQ_HUSHIGI,

	MAINSEQ_SAVE_BREAK,

	MAINSEQ_END_SET,

	MAINSEQ_END,
};

typedef int (*pSTARTMENU_FUNC)(START_MENU_WORK*);

#define	SAVE_BREAK_MESSAGE_MAX		( 8 )		// �Z�[�u�f�[�^�j�����b�Z�[�W��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static int MainSeq_Init( START_MENU_WORK * wk );
static int MainSeq_Release( START_MENU_WORK * wk );
static int MainSeq_Fade( START_MENU_WORK * wk );
static int MainSeq_ButtonAnm( START_MENU_WORK * wk );
static int MainSeq_InitWait( START_MENU_WORK * wk );
static int MainSeq_Main( START_MENU_WORK * wk );
static int MainSeq_Scroll( START_MENU_WORK * wk );
static int MainSeq_Continue( START_MENU_WORK * wk );
static int MainSeq_NewGame( START_MENU_WORK * wk );
//static int MainSeq_Hushigi( START_MENU_WORK * wk );
static int MainSeq_SaveBreak( START_MENU_WORK * wk );
static int MainSeq_EndSet( START_MENU_WORK * wk );

static void InitVBlank( START_MENU_WORK * wk );
static void ExitVBlank( START_MENU_WORK * wk );
static void InitVram(void);
static void InitBg(void);
static void ExitBg(void);
static void LoadBgGraphic(void);
static void InitMsg( START_MENU_WORK * wk );
static void ExitMsg( START_MENU_WORK * wk );
static void InitBmp( START_MENU_WORK * wk );
static void ExitBmp( START_MENU_WORK * wk );
static void InitObj( START_MENU_WORK * wk );
static void ExitObj( START_MENU_WORK * wk );
static void InitBgWinFrame( START_MENU_WORK * wk );
static void ExitBgWinFrame( START_MENU_WORK * wk );
static void InitBlinkAnm( START_MENU_WORK * wk );
static void ExitBlinkAnm( START_MENU_WORK * wk );

static void InitList( START_MENU_WORK * wk );
static void LoadListFrame( START_MENU_WORK * wk );
static void UnloadListFrame( START_MENU_WORK * wk );
static void PutListObj( START_MENU_WORK * wk, u16 idx, s16 py );
static void InitListPut( START_MENU_WORK * wk );
static void PutListItem( START_MENU_WORK * wk, u8 item, s8 py );
static void ChangeListItemPalette( START_MENU_WORK * wk, u8 item, s8 py, u8 pal );
static s8 GetListPutY( START_MENU_WORK * wk, s8 py );

static void ScrollObj( START_MENU_WORK * wk, int val );
static void SetBlendAlpha(void);
static int SetFadeIn( START_MENU_WORK * wk, int next );
static int SetFadeOut( START_MENU_WORK * wk, int next );
static BOOL CursorMove( START_MENU_WORK * wk, s8 vec );
static void VanishListObj( START_MENU_WORK * wk, BOOL flg );
static void PutNewGameWarrning( START_MENU_WORK * wk );
static void ClearNewGameWarrning( START_MENU_WORK * wk, BOOL flg );
static void StartMessage( START_MENU_WORK * wk, int strIdx );
static void ClearMessage( START_MENU_WORK * wk );
static BOOL MainMessage( START_MENU_WORK * wk );
static void SetYesNoMenu( START_MENU_WORK * wk );
static void PutContinueInfo( START_MENU_WORK * wk );
static void ClearContinueInfo( START_MENU_WORK * wk );
//static void InitHushigiCheck( START_MENU_WORK * wk );
//static void ExitHushigiCheck( START_MENU_WORK * wk );
//static BOOL CheckHushigiBeacon( START_MENU_WORK * wk );
static int SetButtonAnm( START_MENU_WORK * wk, int next );
static BOOL CheckSaveDataBreak( START_MENU_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

FS_EXTERN_OVERLAY(pdw_acc);
extern const GFL_PROC_DATA PDW_ACC_MainProcData;

// PROC
const GFL_PROC_DATA StartMenuProcData = {
  START_MENU_ProcInit,
  START_MENU_ProcMain,
  START_MENU_ProcEnd,
};

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_128_D,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

// ���C���V�[�P���X
static const pSTARTMENU_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Fade,
	MainSeq_ButtonAnm,

	MainSeq_InitWait,
	MainSeq_Main,
	MainSeq_Scroll,

	MainSeq_Continue,
	MainSeq_NewGame,
//	MainSeq_Hushigi,

	MainSeq_SaveBreak,

	MainSeq_EndSet
};

// ���X�g���ڃf�[�^
static const LIST_ITEM_DATA ListItemData[] =
{
	{ NARC_startmenu_list_frame1_lz_NSCR, LIST_FRAME_CONTINUE_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_NEW_GAME_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_HUSHIGI_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_BATTLE_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_GAME_SYNC_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_WIFI_SET_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_MIC_TEST_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_MACHINE_SY },
};

// BMPWIN�f�[�^
static const u8 BmpWinData[][6] =
{
	{	//�u��������n�߂�v
		BMPWIN_LIST_FRM, BMPWIN_TITLE_PX, BMPWIN_TITLE_PY,
		BMPWIN_TITLE_SX, BMPWIN_TITLE_SY, BMPWIN_LIST_PAL
	},
	{	// �v���C���[��
		BMPWIN_LIST_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_LIST_PAL
	},
	{	// �Z�[�u�ꏊ
		BMPWIN_LIST_FRM, BMPWIN_PLACE_PX, BMPWIN_PLACE_PY,
		BMPWIN_PLACE_SX, BMPWIN_PLACE_SY, BMPWIN_LIST_PAL
	},
	{	// �v���C����
		BMPWIN_LIST_FRM, BMPWIN_TIME_PX, BMPWIN_TIME_PY,
		BMPWIN_TIME_SX, BMPWIN_TIME_SY, BMPWIN_LIST_PAL
	},
	{	// �}��
		BMPWIN_LIST_FRM, BMPWIN_ZUKAN_PX, BMPWIN_ZUKAN_PY,
		BMPWIN_ZUKAN_SX, BMPWIN_ZUKAN_SY, BMPWIN_LIST_PAL
	},
	{	// �o�b�W
		BMPWIN_LIST_FRM, BMPWIN_BADGE_PX, BMPWIN_BADGE_PY,
		BMPWIN_BADGE_SX, BMPWIN_BADGE_SY, BMPWIN_LIST_PAL
	},
	{	//�u�ŏ�����n�߂�v
		BMPWIN_LIST_FRM, BMPWIN_START_PX, BMPWIN_START_PY,
		BMPWIN_START_SX, BMPWIN_START_SY, BMPWIN_LIST_PAL
	},
	{	//�u�s�v�c�ȑ��蕨�v
		BMPWIN_LIST_FRM, BMPWIN_HUSHIGI_PX, BMPWIN_HUSHIGI_PY,
		BMPWIN_HUSHIGI_SX, BMPWIN_HUSHIGI_SY, BMPWIN_LIST_PAL
	},
	{	//�u�o�g�����v
		BMPWIN_LIST_FRM, BMPWIN_BATTLE_PX, BMPWIN_BATTLE_PY,
		BMPWIN_BATTLE_SX, BMPWIN_BATTLE_SY, BMPWIN_LIST_PAL
	},
	{	//�u�Q�[���V���N�ݒ�v
		BMPWIN_LIST_FRM, BMPWIN_GAMESYNC_PX, BMPWIN_GAMESYNC_PY,
		BMPWIN_GAMESYNC_SX, BMPWIN_GAMESYNC_SY, BMPWIN_LIST_PAL
	},
	{	//�uWi-Fi�ݒ�v
		BMPWIN_LIST_FRM, BMPWIN_WIFI_PX, BMPWIN_WIFI_PY,
		BMPWIN_WIFI_SX, BMPWIN_WIFI_SY, BMPWIN_LIST_PAL
	},
	{	//�u�}�C�N�e�X�g�v
		BMPWIN_LIST_FRM, BMPWIN_MIC_PX, BMPWIN_MIC_PY,
		BMPWIN_MIC_SX, BMPWIN_MIC_SY, BMPWIN_LIST_PAL
	},
	{	//�u�]���}�V�����g���v
		BMPWIN_LIST_FRM, BMPWIN_MACHINE_PX, BMPWIN_MACHINE_PY,
		BMPWIN_MACHINE_SX, BMPWIN_MACHINE_SY, BMPWIN_LIST_PAL
	},

	{	// ���b�Z�[�W
		BMPWIN_MSG_FRM, BMPWIN_MSG_PX, BMPWIN_MSG_PY,
		BMPWIN_MSG_SX, BMPWIN_MSG_SY, BMPWIN_MSG_PAL
	},
};

// OBJ
static const GFL_CLWK_DATA PlayerObjData = { PLAYER_OBJ_PX, PLAYER_OBJ_PY, 1, 0, 1 };
static const GFL_CLWK_DATA NewObjData = { NEW_OBJ_PX, 0, 0, 0, 1 };

// �Z�[�u�f�[�^�j�����b�Z�[�W�\���e�[�u��
static const u32 SaveBreakMessage[][2] =
{	// �r�b�g, ���b�Z�[�W
	{	// ���|�[�g��������
		FST_NORMAL_BREAK_BIT,
		START_MENU_LOAD_WARNING_01
	},
	{	// �o�g���r�f�I���������i�����ꂩ�P�ȏ�j
		FST_EXTRA_REC_MINE_BREAK_BIT|FST_EXTRA_REC_DL_0_BREAK_BIT|FST_EXTRA_REC_DL_1_BREAK_BIT|FST_EXTRA_REC_DL_2_BREAK_BIT,
		START_MENU_LOAD_WARNING_02
	},
	{	// C�M�A �X�L��
		FST_EXTRA_CGEAR_PICTURE_BREAK_BIT,
		START_MENU_LOAD_WARNING_03
	},
	{	// �g���C�A���n�E�X
		FST_EXTRA_BATTLE_EXAMINATION_BREAK_BIT,
		START_MENU_LOAD_WARNING_04,
	},
	{	// �~���[�W�J��
		FST_EXTRA_STREAMING_BREAK_BIT,
		START_MENU_LOAD_WARNING_05
	},
	{	// �|�P�����}�� �ǎ�
		FST_EXTRA_ZUKAN_WALLPAPER_BREAK_BIT,
		START_MENU_LOAD_WARNING_06
	},
	{	// �a������|�P����
		FST_EXTRA_DENDOU_BREAK_BIT,
		START_MENU_LOAD_WARNING_07
	},
	{	// �ӂ����Ȃ�������́i�r�N�e�B�j
		FST_OUTSIDE_MYSTERY_BREAK_BIT,
		START_MENU_LOAD_WARNING_08
	}
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
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_STARTMENU, 0x80000 );
  wk = GFL_PROC_AllocWork( proc, sizeof(START_MENU_WORK), HEAPID_STARTMENU );
  GFL_STD_MemClear( wk, sizeof(START_MENU_WORK) );

	wk->savedata = SaveControl_GetPointer();
  wk->mystatus = SaveData_GetMyStatus( wk->savedata );
	wk->misc     = SaveData_GetMisc( wk->savedata );
	wk->cgear    = CGEAR_SV_GetCGearSaveData( wk->savedata );
	wk->evwk     = SaveData_GetEventWork( wk->savedata );

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
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk = mywk;

	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
	  return GFL_PROC_RES_FINISH;
	}

  GFL_TCBL_Main( wk->tcbl );
	GFL_CLACT_SYS_Main();

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
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk;
	u8	result;
	u8	continueRet;
	
	wk = mywk;
	result = wk->listResult;
	continueRet = wk->continueRet;

	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_STARTMENU );

	switch( result ){
	case LIST_ITEM_CONTINUE:			// ��������
//		GameStart_Continue();
		if( continueRet == 0 ){
			GameStart_ContinueNet();
		}else{
			GameStart_ContinueNetOff();
		}
		break;

	case LIST_ITEM_NEW_GAME:			// �ŏ�����
		GameStart_Beginning();
		break;

	case LIST_ITEM_HUSHIGI:				// �s�v�c�ȑ��蕨
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mystery), &MysteryGiftProcData, NULL);
		break;

	case LIST_ITEM_BATTLE:				// �o�g�����
    { 
      WIFIBATTLEMATCH_PARAM *p_param  = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(WIFIBATTLEMATCH_PARAM) );
      GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_PARAM) );
      p_param->mode             = WIFIBATTLEMATCH_MODE_MAINMENU;
      p_param->is_auto_release  = TRUE; //����PROC���I�������̈����������ō폜����
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifibattlematch_sys), &WifiBattleMatch_ProcData, p_param );
    }
		break;

	case LIST_ITEM_GAME_SYNC:			// �Q�[���V���N�ݒ�
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &PDW_ACC_MainProcData, NULL);
		break;

	case LIST_ITEM_WIFI_SET:			// Wi-Fi�ݒ�
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifi_util), &WifiUtilProcData, NULL);
		break;

	case LIST_ITEM_MIC_TEST:			// �}�C�N�e�X�g
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL);
		break;

	case LIST_ITEM_MACHINE:				// �]���}�V�����g��
	  {
      MB_PARENT_INIT_WORK *initWork = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(MB_PARENT_INIT_WORK) );
      initWork->mode = MPM_MOVIE_TRANS;
      //�Q�[���f�[�^�̊m�ۂƃ��[�N�̊J���̓��[�h�����čs���B
  		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(multiboot), &MultiBoot_ProcData, initWork);
  	}
		break;

	default:		// �L�����Z��
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
		break;
	}

  return GFL_PROC_RES_FINISH;
}


//============================================================================================
//	���C���V�[�P���X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( START_MENU_WORK * wk )
{
/*
	// �Z�[�u�f�[�^���Ȃ��ꍇ�́u�ŏ�����v�̏����ɔ�΂�
	if( SaveData_GetExistFlag( wk->savedata ) == FALSE ){
		wk->listResult = LIST_ITEM_NEW_GAME;
		return MAINSEQ_END;
	}
*/

#ifdef PM_DEBUG
	// �S���j���[�\������
	if( SaveData_GetExistFlag( wk->savedata ) == TRUE ){
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
//			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_HUSHIGI, MISC_STARTMENU_FLAG_OPEN );
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_BATTLE, MISC_STARTMENU_FLAG_OPEN );
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_GAMESYNC, MISC_STARTMENU_FLAG_OPEN );
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_MACHINE, MISC_STARTMENU_FLAG_OPEN );
		}
	}
#endif	// PM_DEBUG

	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN);

	InitVram();
	InitBg();
	LoadBgGraphic();
	InitMsg( wk );

	InitList( wk );
	LoadListFrame( wk );

	InitBmp( wk );
	InitObj( wk );

	InitBgWinFrame( wk );

	InitListPut( wk );

	InitBlinkAnm( wk );

	SetBlendAlpha();

//	InitHushigiCheck( wk );

	InitVBlank( wk );

	return MAINSEQ_INIT_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( START_MENU_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
//		ExitHushigiCheck( wk );
		wk->subSeq++;

	case 1:
		if( GFL_NET_IsResetEnable() == FALSE ){
			break;
		}

		ExitVBlank( wk );

		ExitBlinkAnm( wk );

		ExitBgWinFrame( wk );

		ExitObj( wk );
		ExitBmp( wk );

		UnloadListFrame( wk );

		ExitMsg( wk );
		ExitBg();

		// �u�����h������
		G2_BlendNone();
		G2S_BlendNone();
		// �\��������
		GFL_DISP_GX_SetVisibleControlDirect( 0 );
		GFL_DISP_GXS_SetVisibleControlDirect( 0 );

		return MAINSEQ_END;
	}

	return MAINSEQ_RELEASE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�F�[�h�҂�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Fade( START_MENU_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->fadeSeq;
	}
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�^���A�j��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ButtonAnm( START_MENU_WORK * wk )
{
	switch( wk->btnSeq ){
	case 0:
	case 2:
		if( wk->btnCnt == 0 ){
			ChangeListItemPalette(
				wk, wk->list[wk->listPos], GetListPutY(wk,wk->cursorPutPos), CURSOR_PALETTE2 );
			wk->btnCnt = 4;
			wk->btnSeq++;
		}else{
			wk->btnCnt--;
		}
		break;

	case 1:
	case 3:
		if( wk->btnCnt == 0 ){
			ChangeListItemPalette(
				wk, wk->list[wk->listPos], GetListPutY(wk,wk->cursorPutPos), LIST_PALETTE );
			wk->btnCnt = 4;
			wk->btnSeq++;
		}else{
			wk->btnCnt--;
		}
		break;

	case 4:
		return wk->nextSeq;
	}

	return MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�������҂�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitWait( START_MENU_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<BMPWIN_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->util[i], wk->que );
	}

	if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
		if( CheckSaveDataBreak( wk ) == TRUE ){
			return MAINSEQ_SAVE_BREAK;
		}
		return SetFadeIn( wk, MAINSEQ_MAIN );
	}
	return MAINSEQ_INIT_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Main( START_MENU_WORK * wk )
{
	BLINKPALANM_Main( wk->blink );

/*
	// �s�v�c�ȑ��蕨��M�`�F�b�N
	if( CheckHushigiBeacon( wk ) == TRUE ){
		return MAINSEQ_HUSHIGI;
	}
*/

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
		switch( wk->list[wk->listPos] ){
		case LIST_ITEM_CONTINUE:		// ��������
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_CONTINUE );

		case LIST_ITEM_NEW_GAME:		// �ŏ�����
			return SetButtonAnm( wk, MAINSEQ_NEWGAME );

		case LIST_ITEM_HUSHIGI:			// �s�v�c�ȑ��蕨
//			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_HUSHIGI, MISC_STARTMENU_FLAG_VIEW );
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_END_SET );

		case LIST_ITEM_BATTLE:			// �o�g�����
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_BATTLE, MISC_STARTMENU_FLAG_VIEW );
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_END_SET );

		case LIST_ITEM_GAME_SYNC:		// �Q�[���V���N�ݒ�
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_GAMESYNC, MISC_STARTMENU_FLAG_VIEW );
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_END_SET );

		case LIST_ITEM_MACHINE:			// �]���}�V�����g��
			MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_MACHINE, MISC_STARTMENU_FLAG_VIEW );
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_END_SET );

		case LIST_ITEM_WIFI_SET:		// Wi-Fi�ݒ�
		case LIST_ITEM_MIC_TEST:		// �}�C�N�e�X�g
			wk->listResult = wk->list[wk->listPos];
			return SetButtonAnm( wk, MAINSEQ_END_SET );
		}
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
		wk->listResult = LIST_ITEM_MAX;
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
		if( CursorMove( wk, -1 ) == TRUE ){
			PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
			return MAINSEQ_SCROLL;
		}
	}

	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
		if( CursorMove( wk, 1 ) == TRUE ){
			PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
			return MAINSEQ_SCROLL;
		}
	}

	return MAINSEQ_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���X�g�X�N���[��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Scroll( START_MENU_WORK * wk )
{
	if( wk->bgScrollCount == 3 ){
		wk->bgScrollCount = 0;
		wk->bgScrollSpeed = 0;
		BLINKPALANM_InitAnimeCount( wk->blink );
		ChangeListItemPalette( wk, wk->list[wk->listPos], GetListPutY(wk,wk->cursorPutPos), CURSOR_PALETTE );
		return MAINSEQ_MAIN;
	}

	wk->bgScroll += wk->bgScrollSpeed;
	if( wk->bgScroll < 0 ){
		wk->bgScroll += 512;
	}else if( wk->bgScroll >= 512 ){
		wk->bgScroll -= 512;
	}

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );

	ScrollObj( wk, -wk->bgScrollSpeed );

	wk->bgScrollCount++;

	return MAINSEQ_SCROLL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v����
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Continue( START_MENU_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		// ���X�g��\��
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );
		VanishListObj( wk, FALSE );
		// C-GEAR�`�F�b�N
		if( CGEAR_SV_GetCGearONOFF(wk->cgear) == FALSE ){
			wk->continueRet = 1;
			wk->subSeq = 0;
			return SetFadeOut( wk, MAINSEQ_RELEASE );
		}
		wk->wait = CONTINUE_1ST_WAIT;
		wk->subSeq++;
		break;

	case 1:
		if( wk->wait != 0 ){
			wk->wait--;
			break;
		}
		// ���b�Z�[�W�\��
		StartMessage( wk, START_MENU_STR_CONTINUE_01 );
		PutContinueInfo( wk );
		wk->subSeq++;
		break;

	case 2:
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
			SetYesNoMenu( wk );
			wk->subSeq++;
		}
		break;

	case 3:
		// �͂��E�������҂�
		switch( BmpMenu_YesNoSelectMain( wk->mwk ) ){
		case 0:
			// �{�̐ݒ�̖����ݒ�
			if( DS_SYSTEM_IsAvailableWireless() == FALSE ){
				StartMessage( wk, START_MENU_STR_ATTENTION_01 );
				wk->subSeq = 5;
				break;
			}
			// �y�A�����^���R���g���[��
			if( DS_SYSTEM_IsRestrictUGC() == TRUE ){
				StartMessage( wk, START_MENU_STR_ATTENTION_02 );
				wk->subSeq = 5;
				break;
			}
			wk->continueRet = 0;
			wk->subSeq = 4;
			break;

		case BMPMENU_CANCEL:
			StartMessage( wk, START_MENU_STR_CONTINUE_02 );
			wk->subSeq = 7;
//			wk->continueRet = 1;
//			wk->subSeq = 4;
			break;
		}
		break;

	case 4:		// �I��
		ClearContinueInfo( wk );
		ClearMessage( wk );
		wk->subSeq = 0;
		return SetFadeOut( wk, MAINSEQ_RELEASE );

	case 5:		// �����ݒ�G���[
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
			wk->subSeq++;
		}
		break;

	case 6:
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
			wk->continueRet = 1;
			wk->subSeq = 4;
		}
		break;

	case 7:
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
			SetYesNoMenu( wk );
			wk->subSeq++;
		}
		break;

	case 8:
		// �͂��E�������҂�
		switch( BmpMenu_YesNoSelectMain( wk->mwk ) ){
		case 0:
			wk->continueRet = 1;
			wk->subSeq = 4;
			break;

		case BMPMENU_CANCEL:
			StartMessage( wk, START_MENU_STR_CONTINUE_01 );
			wk->subSeq = 2;
			break;
		}
		break;
	}

	PRINTSYS_QUE_Main( wk->que );
	PRINT_UTIL_Trans( &wk->utilWin, wk->que );

	return MAINSEQ_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�ŏ�����v����
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_NewGame( START_MENU_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		// �������炪�����ꍇ
		if( wk->list[0] == LIST_ITEM_NEW_GAME ){
			wk->listResult = LIST_ITEM_NEW_GAME;
			wk->subSeq = 0;
			return SetFadeOut( wk, MAINSEQ_RELEASE );
		}else{
			PutNewGameWarrning( wk );
			wk->subSeq++;
		}
		break;

	case 1:
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->subSeq++;
		}
		break;

	case 2:
		// �`�{�^��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
			ClearNewGameWarrning( wk, TRUE );
			wk->listResult = LIST_ITEM_NEW_GAME;
			wk->subSeq = 0;
			return SetFadeOut( wk, MAINSEQ_RELEASE );
		}
		// �a�{�^��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
			PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
			ClearNewGameWarrning( wk, FALSE );
			ChangeListItemPalette(
				wk, wk->list[wk->listPos], GetListPutY(wk,wk->cursorPutPos), CURSOR_PALETTE );
			wk->subSeq = 0;
			return MAINSEQ_MAIN;
		}
		break;
	}

	PRINTSYS_QUE_Main( wk->que );
	PRINT_UTIL_Trans( &wk->utilWin, wk->que );

	return MAINSEQ_NEWGAME;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�s�v�c�ȑ��蕨�v��M
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
/*
static int MainSeq_Hushigi( START_MENU_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		// �r�[�R���擾�I���҂�
		if( GFL_NET_IsResetEnable() == TRUE ){
			wk->subSeq++;
		}
		break;

	case 1:
		// ���X�g��\��
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );
		VanishListObj( wk, FALSE );
		wk->subSeq++;
		break;

	case 2:
		// ���b�Z�[�W�\��
		StartMessage( wk, START_MENU_STR_ATTENTION_03 );
		wk->subSeq++;
		break;

	case 3:
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
			wk->subSeq++;
		}
		break;

	case 4:
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
			ClearMessage( wk );
			wk->subSeq++;
		}
		break;

	case 5:
		// ���X�g�č\�z
		MISC_SetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_HUSHIGI, MISC_STARTMENU_FLAG_OPEN );
		InitList( wk );
		InitListPut( wk );
		GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
		GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
		BLINKPALANM_InitAnimeCount( wk->blink );
		wk->subSeq++;
		break;

	case 6:
		// ���X�g�\��
		VanishListObj( wk, TRUE );
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_ON );
		wk->subSeq = 0;
		return MAINSEQ_MAIN;
	}

	return MAINSEQ_HUSHIGI;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�Z�[�u�f�[�^�j��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SaveBreak( START_MENU_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:		// ��ʍ쐬
		GFL_DISP_GX_SetVisibleControl(
			GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_OFF );
		GFL_DISP_GXS_SetVisibleControl(
			GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_OFF );
		GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, BLUE_BACK_GROUND_COLOR );
		GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, BLUE_BACK_GROUND_COLOR );
		wk->subSeq++;
		return SetFadeIn( wk, MAINSEQ_SAVE_BREAK );

	case 1:		// ���b�Z�[�W�\��
		while( 1 ){
			if( wk->svBreakPos >= SAVE_BREAK_MESSAGE_MAX ){
				ClearMessage( wk );
				wk->subSeq = 3;
				return SetFadeOut( wk, MAINSEQ_SAVE_BREAK );
			}
			if( wk->svBreak & SaveBreakMessage[wk->svBreakPos][0] ){
				break;
			}
			wk->svBreakPos++;
		}
		StartMessage( wk, SaveBreakMessage[wk->svBreakPos][1] );
		wk->svBreakPos++;
		wk->subSeq++;
		break;

	case 2:		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
	    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
				wk->subSeq = 1;
			}
		}
		break;

	case 3:		// ��ʕ��A
		GFL_DISP_GX_SetVisibleControl(
			GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl(
			GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_ON );
		wk->subSeq = 0;
		return SetFadeIn( wk, MAINSEQ_MAIN );
	}

	return MAINSEQ_SAVE_BREAK;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�I���ݒ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	���̏���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndSet( START_MENU_WORK * wk )
{
	return SetFadeOut( wk, MAINSEQ_RELEASE );
}


//============================================================================================
//	�������֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK����
 *
 * @param		tcb		GFL_TCB
 * @param		work	�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�����ݒ�
 *
 * @param		wk	�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitVBlank( START_MENU_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�����폜
 *
 * @param		wk	�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitVBlank( START_MENU_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
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
	GFL_DISP_ClearVRAM( NULL );
	GFL_DISP_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBg(void)
{
	GFL_BG_Init( HEAPID_STARTMENU );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF���b�Z�[�W�E�B���h�E
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���X�g����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���X�g�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xb000, GX_BG_CHARBASE_0x08000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// �T�u��ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF�t���[��
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�O���t�B�b�N�ǂݍ���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadBgGraphic(void)
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_startmenu_bgu_NCLR, PALTYPE_MAIN_BG, 0, 0x20*6, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_startmenu_bgu_lz_NCGR, GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_startmenu_base_bgu_lz_NSCR,
		GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_STARTMENU );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_startmenu_bgd_NCLR, PALTYPE_SUB_BG, 0, 0x20*6, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_startmenu_bgd_lz_NCGR, GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_startmenu_base_bgd_lz_NSCR,
		GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_STARTMENU );

	GFL_ARC_CloseDataHandle( ah );

	// �V�X�e���E�B���h�E
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME0_M, MESSAGE_WIN_CHAR_NUM,
		MESSAGE_WIN_PLTT_NUM, MENU_TYPE_SYSTEM, HEAPID_STARTMENU );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
		FONT_PALETTE_M*0x20, 0x20, HEAPID_STARTMENU );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
		FONT_PALETTE_S*0x20, 0x20, HEAPID_STARTMENU );

#if PM_VERSION == VERSION_WHITE
	// �z���C�g�̏ꍇ�͔w�i�̃p���b�g��ύX
	GFL_BG_ChangeScreenPalette( GFL_BG_FRAME3_M, 0, 0, 32, 24, BGPAL_VER_WHITE );
	GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, 0, 0, 32, 24, BGPAL_VER_WHITE );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���A�j��������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBlinkAnm( START_MENU_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	wk->blink = BLINKPALANM_Create(
								CURSOR_PALETTE*16, 16, GFL_BG_FRAME2_M, HEAPID_STARTMENU );

	BLINKPALANM_SetPalBufferArcHDL(
		wk->blink, ah, NARC_startmenu_bgu_NCLR, CURSOR_PALETTE*16, CURSOR_PALETTE2*16 );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���A�j���폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBlinkAnm( START_MENU_WORK * wk )
{
	BLINKPALANM_Exit( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitMsg( START_MENU_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_startmenu_dat, HEAPID_STARTMENU );
	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STARTMENU );
	wk->wset = WORDSET_Create( HEAPID_STARTMENU );
	wk->que  = PRINTSYS_QUE_Create( HEAPID_STARTMENU );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_STARTMENU );
	wk->tcbl = GFL_TCBL_Init( HEAPID_STARTMENU, HEAPID_STARTMENU, 1, 4 );
	wk->kcwk = APP_KEYCURSOR_Create( 15, TRUE, FALSE, HEAPID_STARTMENU );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitMsg( START_MENU_WORK * wk )
{
	APP_KEYCURSOR_Delete( wk->kcwk );
  GFL_TCBL_Exit( wk->tcbl );
	GFL_STR_DeleteBuffer( wk->exp );
	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBmp( START_MENU_WORK * wk )
{
	const u8 * dat;
	STRBUF * str;
	u32	i;

	GFL_BMPWIN_Init( HEAPID_STARTMENU );

	for( i=0; i<BMPWIN_MAX; i++ ){
		dat = BmpWinData[i];
		wk->util[i].win = GFL_BMPWIN_Create(
												dat[0], dat[1], dat[2], dat[3], dat[4], dat[5], GFL_BMP_CHRAREA_GET_B );
	}

	// ���ڕ�����`��
	// ��������n�߂�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_01 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_TITLE], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// ��l����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_06 );
	WORDSET_RegisterPlayerName( wk->wset, 0, wk->mystatus );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	if( MyStatus_GetMySex( wk->mystatus ) == PM_MALE ){
		PRINT_UTIL_PrintColor( &wk->util[BMPWIN_NAME], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05BLN );
	}else{
		PRINT_UTIL_PrintColor( &wk->util[BMPWIN_NAME], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05RN );
	}
	GFL_STR_DeleteBuffer( str );
	// �ꏊ
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_10 );
	{
		u16	num;
		ZONEDATA_Open( HEAPID_STARTMENU_L );
		// ����ڑ�
		if( EVENTWORK_CheckEventFlag( wk->evwk, SYS_FLAG_SPEXIT_REQUEST ) == TRUE ){
			SITUATION * situation;
			const LOCATION * spLoc;
			situation = SaveData_GetSituation( wk->savedata );
			spLoc = Situation_GetSpecialLocation( situation );
			num = ZONEDATA_GetPlaceNameID( spLoc->zone_id );
		// �ʏ�
		}else{
			PLAYERWORK_SAVE	plwk;
			SaveData_SituationLoad_PlayerWorkSave( wk->savedata, &plwk );
			num = ZONEDATA_GetPlaceNameID( plwk.zoneID );
		}
		ZONEDATA_Close();
		WORDSET_RegisterPlaceName( wk->wset, 0, num );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_PLACE], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �v���C����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_03 );
	{
		PLAYTIME * ptime = SaveData_GetPlayTime( wk->savedata );
		WORDSET_RegisterNumber( wk->wset, 0, PLAYTIME_GetHour(ptime), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wk->wset, 1, PLAYTIME_GetMinute(ptime), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_TIME], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �}��
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_04 );
	{
		u16	num = ZUKANSAVE_GetZukanPokeSeeCount( ZUKAN_SAVEDATA_GetZukanSave(wk->savedata), HEAPID_STARTMENU );
		WORDSET_RegisterNumber( wk->wset, 0, num, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_ZUKAN], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �o�b�W
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_05 );
	{
		int	num = MISC_GetBadgeCount( SaveData_GetMisc(wk->savedata) );
		WORDSET_RegisterNumber( wk->wset, 0, num, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_BADGE], wk->que, 0, 0, wk->exp, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );

	// �ŏ�����n�߂�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_02 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_START], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �s�v�c�ȑ��蕨
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_03 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_HUSHIGI], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �o�g�����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_05 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_BATTLE], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �Q�[���V���N�ݒ�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_06 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_GAMESYNC], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// Wi-Fi�ݒ�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_04 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_WIFI], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �}�C�N�e�X�g
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_08 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_MIC], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �]���}�V�����g��
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_07 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_MACHINE], wk->que, 0, 4, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBmp( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<BMPWIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->util[i].win );
	}

	GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitObj( START_MENU_WORK * wk )
{
	{	// �V�X�e��������
		const GFL_CLSYS_INIT init = {
			0, 0,									// ���C���@�T�[�t�F�[�X�̍�����W
			0, 512,								// �T�u�@�T�[�t�F�[�X�̍�����W
			4,										// ���C�����OAM�Ǘ��J�n�ʒu	4�̔{��
			124,									// ���C�����OAM�Ǘ���				4�̔{��
			4,										// �T�u���OAM�Ǘ��J�n�ʒu		4�̔{��
			124,									// �T�u���OAM�Ǘ���					4�̔{��
			0,										// �Z��Vram�]���Ǘ���
	
			CHRRES_MAX,						// �o�^�ł���L�����f�[�^��
			PALRES_MAX,						// �o�^�ł���p���b�g�f�[�^��
			CELRES_MAX,						// �o�^�ł���Z���A�j���p�^�[����
			0,										// �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

		  16,										// ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		  16										// �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		};
		GFL_CLACT_SYS_Create( &init, &VramTbl, HEAPID_STARTMENU );
	}

	{	// ���\�[�X�ǂݍ���
		ARCHANDLE * ah;
		
		// �v���C���[
		ah = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, HEAPID_STARTMENU_L );

		if( MyStatus_GetMySex( wk->mystatus ) == PM_MALE ){
			wk->chrRes[CHRRES_PLAYER] = GFL_CLGRP_CGR_Register(
																		ah, NARC_wifileadingchar_hero_NCGR,
																		FALSE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
		  wk->palRes[PALRES_PLAYER] = GFL_CLGRP_PLTT_Register(
																		ah, NARC_wifileadingchar_hero_NCLR,
																		CLSYS_DRAW_MAIN, PALNUM_PLAYER*0x20, HEAPID_STARTMENU );
	    wk->celRes[CELRES_PLAYER] = GFL_CLGRP_CELLANIM_Register(
																		ah,
																		NARC_wifileadingchar_hero_NCER,
																		NARC_wifileadingchar_hero_NANR,
																		HEAPID_STARTMENU );
		}else{
			wk->chrRes[CHRRES_PLAYER] = GFL_CLGRP_CGR_Register(
																		ah, NARC_wifileadingchar_heroine_NCGR,
																		FALSE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
		  wk->palRes[PALRES_PLAYER] = GFL_CLGRP_PLTT_Register(
																		ah, NARC_wifileadingchar_heroine_NCLR,
																		CLSYS_DRAW_MAIN, PALNUM_PLAYER*0x20, HEAPID_STARTMENU );
	    wk->celRes[CELRES_PLAYER] = GFL_CLGRP_CELLANIM_Register(
																		ah,
																		NARC_wifileadingchar_heroine_NCER,
																		NARC_wifileadingchar_heroine_NANR,
																		HEAPID_STARTMENU );
		}

		GFL_ARC_CloseDataHandle( ah );

		// ���̑�
		ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );
		wk->chrRes[CHRRES_OBJ_U] = GFL_CLGRP_CGR_Register(
																ah, NARC_startmenu_obj_u_lz_NCGR,
																TRUE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
	  wk->palRes[PALRES_OBJ_U] = GFL_CLGRP_PLTT_Register(
																ah, NARC_startmenu_obj_u_NCLR,
																CLSYS_DRAW_MAIN, PALNUM_OBJ_U*0x20, HEAPID_STARTMENU );
    wk->celRes[CELRES_OBJ_U] = GFL_CLGRP_CELLANIM_Register(
																ah,
																NARC_startmenu_obj_u_NCER,
																NARC_startmenu_obj_u_NANR,
																HEAPID_STARTMENU );
		GFL_ARC_CloseDataHandle( ah );
	}

	{	// OBJ�ǉ�
		u32	i;

		wk->clunit = GFL_CLACT_UNIT_Create( OBJ_ID_MAX, 0, HEAPID_STARTMENU );

		wk->clwk[OBJ_ID_PLAYER] = GFL_CLACT_WK_Create(
																wk->clunit,
																wk->chrRes[CHRRES_PLAYER],
																wk->palRes[PALRES_PLAYER],
																wk->celRes[CELRES_PLAYER],
																&PlayerObjData, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_PLAYER], FALSE );

		for( i=OBJ_ID_NEW_HUSHIGI; i<=OBJ_ID_NEW_MACHINE; i++ ){
			wk->clwk[i] = GFL_CLACT_WK_Create(
											wk->clunit,
											wk->chrRes[CHRRES_OBJ_U],
											wk->palRes[PALRES_OBJ_U],
											wk->celRes[CELRES_OBJ_U],
											&NewObjData, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
			GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[i], TRUE );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
		}
	}

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i�폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitObj( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<OBJ_ID_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
	GFL_CLACT_UNIT_Delete( wk->clunit );

	for( i=0; i<CHRRES_MAX; i++ ){
		GFL_CLGRP_CGR_Release( wk->chrRes[i] );
	}
	for( i=0; i<PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
	}
	for( i=0; i<CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
	}

	GFL_CLACT_SYS_Delete();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BGWINFRAME������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBgWinFrame( START_MENU_WORK * wk )
{
	u32	i;

	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_NONE, LIST_ITEM_MAX, HEAPID_STARTMENU );

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		BGWINFRM_Add( wk->wfrm, i, BMPWIN_LIST_FRM, LIST_FRAME_SX, ListItemData[i].sy );
	}

	// ��������
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_TITLE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_NAME].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_PLACE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_TIME].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_ZUKAN].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_BADGE].win );

	// ����ȊO�̍���
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_NEW_GAME, wk->util[BMPWIN_START].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_HUSHIGI, wk->util[BMPWIN_HUSHIGI].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_BATTLE, wk->util[BMPWIN_BATTLE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_GAME_SYNC, wk->util[BMPWIN_GAMESYNC].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_WIFI_SET, wk->util[BMPWIN_WIFI].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_MIC_TEST, wk->util[BMPWIN_MIC].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_MACHINE, wk->util[BMPWIN_MACHINE].win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BGWINFRAME�폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBgWinFrame( START_MENU_WORK * wk )
{
	BGWINFRM_Exit( wk->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetBlendAlpha(void)
{
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 3 );
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG2, 16, 3 );
}


//============================================================================================
//	���j���[���X�g�֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g������
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitList( START_MENU_WORK * wk )
{
	u32	i;
	
	i = 0;
	// ��������
	if( SaveData_GetExistFlag( wk->savedata ) == TRUE ){
		wk->list[i] = LIST_ITEM_CONTINUE;
		i++;
	}
	// �ŏ�����
	wk->list[i] = LIST_ITEM_NEW_GAME;
	i++;
	// �s�v�c�ȑ��蕨
//	if( MISC_GetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_HUSHIGI ) != 0 ){
		wk->list[i] = LIST_ITEM_HUSHIGI;
		i++;
//	}
	// �o�g�����
	if( MISC_GetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_BATTLE ) != 0 ){
		wk->list[i] = LIST_ITEM_BATTLE;
		i++;
	}
	// �Q�[���V���N�ݒ�
	if( MISC_GetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_GAMESYNC ) != 0 ){
		wk->list[i] = LIST_ITEM_GAME_SYNC;
		i++;
	}
	// Wi-Fi�ݒ�
	wk->list[i] = LIST_ITEM_WIFI_SET;
	i++;
	// �}�C�N�e�X�g
	wk->list[i] = LIST_ITEM_MIC_TEST;
	i++;
	// �]���}�V�����g��
	if( MISC_GetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_MACHINE ) != 0 ){
		wk->list[i] = LIST_ITEM_MACHINE;
		i++;
	}

	// �_�~�[�����Ă���
	for( i=i; i<LIST_ITEM_MAX; i++ ){
		wk->list[i] = LIST_ITEM_MAX;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�t���[���ǂݍ���
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadListFrame( START_MENU_WORK * wk )
{
	ARCHANDLE * ah;
	u32	i;
	
	ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		const LIST_ITEM_DATA * item;
		NNSG2dScreenData * scrn;
		void * buf;
		u32	siz;

		item = &ListItemData[i];
		siz  = LIST_FRAME_SX * item->sy * 2;

		wk->listFrame[i] = GFL_HEAP_AllocMemory( HEAPID_STARTMENU, siz );
		buf = GFL_ARCHDL_UTIL_LoadScreen( ah, item->scrnIdx, TRUE, &scrn, HEAPID_STARTMENU );
		GFL_STD_MemCopy16( (void *)scrn->rawData, wk->listFrame[i], siz );
		GFL_HEAP_FreeMemory( buf );
	}

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�t���[���폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void UnloadListFrame( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		GFL_HEAP_FreeMemory( wk->listFrame[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g���OBJ�z�u
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		idx		OBJ index
 * @param		py		�\���x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutListObj( START_MENU_WORK * wk, u16 idx, s16 py )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
	pos.y = py;
	GFL_CLACT_WK_SetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�z�u�i�S�́j
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListPut( START_MENU_WORK * wk )
{
	u32	i;
	BOOL continue_flg;
	s8	py;

	wk->listPos = 0;
	wk->bgScroll = 0;
	wk->cursorPutPos = LIST_ITEM_PY;
	py = LIST_ITEM_PY;

	continue_flg = FALSE;

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		if( wk->list[i] == LIST_ITEM_MAX ){
			break;
		}
		PutListItem( wk, wk->list[i], py );

		if( wk->list[i] == LIST_ITEM_CONTINUE ){
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_PLAYER], TRUE );
/*
		}else if( wk->list[i] == LIST_ITEM_HUSHIGI ){
			if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_HUSHIGI) == MISC_STARTMENU_FLAG_OPEN ){
				PutListObj( wk, OBJ_ID_NEW_HUSHIGI, py*8+ListItemData[wk->list[i]].sy*8/2 );
				GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_HUSHIGI], TRUE );
			}
*/
		}else if( wk->list[i] == LIST_ITEM_BATTLE ){
			if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_BATTLE) == MISC_STARTMENU_FLAG_OPEN ){
				PutListObj( wk, OBJ_ID_NEW_BATTLE, py*8+ListItemData[wk->list[i]].sy*8/2 );
				GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_BATTLE], TRUE );
			}
		}else if( wk->list[i] == LIST_ITEM_GAME_SYNC ){
			if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_GAMESYNC) == MISC_STARTMENU_FLAG_OPEN ){
				PutListObj( wk, OBJ_ID_NEW_GAMESYNC, py*8+ListItemData[wk->list[i]].sy*8/2 );
				GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_GAMESYNC], TRUE );
			}
		}else if( wk->list[i] == LIST_ITEM_MACHINE ){
			if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_MACHINE) == MISC_STARTMENU_FLAG_OPEN ){
				PutListObj( wk, OBJ_ID_NEW_MACHINE, py*8+ListItemData[wk->list[i]].sy*8/2 );
				GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_MACHINE], TRUE );
			}
		}

		py += ListItemData[wk->list[i]].sy;
	}

	PutListObj( wk, OBJ_ID_PLAYER, PLAYER_OBJ_PY );

	ChangeListItemPalette( wk, wk->list[0], wk->cursorPutPos, CURSOR_PALETTE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�z�u�i���ڂ��Ɓj
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		item	����
 * @param		py		�\���x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutListItem( START_MENU_WORK * wk, u8 item, s8 py )
{
	GFL_BG_WriteScreenExpand(
		GFL_BG_FRAME2_M,
		LIST_ITEM_PX, py, LIST_FRAME_SX, ListItemData[item].sy,
		wk->listFrame[item],
		0, 0, LIST_FRAME_SX, ListItemData[item].sy );

	GFL_BG_WriteScreenExpand(
		GFL_BG_FRAME1_M,
		LIST_ITEM_PX, py, LIST_FRAME_SX, ListItemData[item].sy,
		BGWINFRM_FrameBufGet(wk->wfrm,item),
		0, 0, LIST_FRAME_SX, ListItemData[item].sy );

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );

/*	y < 64�Ɏ��܂�̂ŁA1�񂸂`���K�v�͂Ȃ�
	u32	i;

	if( item == LIST_ITEM_MAX ){
		for( i=0; i<3; i++ ){
			if( py >= LIST_SCRN_SY ){
				py -= LIST_SCRN_SY;
			}else if( py < 0 ){
				py += LIST_SCRN_SY;
			}
			GFL_BG_FillScreen(
				GFL_BG_FRAME2_M, 0,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1, 0 );
			GFL_BG_FillScreen(
				GFL_BG_FRAME1_M, 0,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1, 0 );
			py++;
		}
	}else{
		for( i=0; i<ListItemData[item].sy; i++ ){
			if( py >= LIST_SCRN_SY ){
				py -= LIST_SCRN_SY;
			}else if( py < 0 ){
				py += LIST_SCRN_SY;
			}
			GFL_BG_WriteScreenExpand(
				GFL_BG_FRAME2_M,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1,
				wk->listFrame[item],
				0, i,
				LIST_FRAME_SX,
				ListItemData[item].sy );
			GFL_BG_WriteScreenExpand(
				GFL_BG_FRAME1_M,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1,
				BGWINFRM_FrameBufGet(wk->wfrm,item),
				0, i,
				LIST_FRAME_SX,
				ListItemData[item].sy );
			py++;
		}
	}

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g���ڃp���b�g�ύX
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		item	����
 * @param		py		�\���x���W
 * @param		pal		�p���b�g
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeListItemPalette( START_MENU_WORK * wk, u8 item, s8 py, u8 pal )
{
	GFL_BG_ChangeScreenPalette(
		GFL_BG_FRAME2_M, LIST_ITEM_PX, py, LIST_FRAME_SX, ListItemData[item].sy, pal );

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );

/*	y < 64�Ɏ��܂�̂ŁA1�񂸂`���K�v�͂Ȃ�
	u32	i;

	for( i=0; i<ListItemData[item].sy; i++ ){
		if( py >= LIST_SCRN_SY ){
			py -= LIST_SCRN_SY;
		}else if( py < 0 ){
			py += LIST_SCRN_SY;
		}
		GFL_BG_ChangeScreenPalette(
			GFL_BG_FRAME2_M,
			LIST_ITEM_PX, py,
			LIST_FRAME_SX, 1,
			pal );
		py++;
	}

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
*/
}


static s8 GetListPutY( START_MENU_WORK * wk, s8 py )
{
	return ( py + ( wk->bgScroll / 8 ) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g��̂n�a�i���X�N���[��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		val		�ړ��l
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ScrollObj( START_MENU_WORK * wk, int val )
{
	GFL_CLACTPOS	pos;
	u32	i;

	for( i=0; i<OBJ_ID_MAX; i++ ){
		GFL_CLACT_WK_GetPos( wk->clwk[i], &pos, CLSYS_DRAW_MAIN );
		pos.y += val;
		GFL_CLACT_WK_SetPos( wk->clwk[i], &pos, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		vec		�ړ�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static BOOL CursorMove( START_MENU_WORK * wk, s8 vec )
{
	u8	now_item;
	u8	now_pos;
	u8	mv_item;
	s8	mv_pos;
	s8	cur_py;

	now_pos  = wk->listPos;
	now_item = wk->list[ now_pos ];

	wk->bgScrollCount = 0;

	if( vec < 0 ){
		if( wk->listPos == 0 ){
			return FALSE;
		}

		mv_pos  = now_pos + vec;
		mv_item = wk->list[ mv_pos ];

		cur_py = wk->cursorPutPos - ListItemData[mv_item].sy;

		ChangeListItemPalette( wk, now_item, GetListPutY(wk,wk->cursorPutPos), LIST_PALETTE );

		if( wk->list[0] == LIST_ITEM_CONTINUE ){
			if( cur_py < LIST_ITEM_PY ){
				if( mv_pos == 0 ){
					wk->bgScrollSpeed = ( ListItemData[ mv_item ].sy - (wk->cursorPutPos-LIST_ITEM_PY) ) / 3 * -8;
					wk->cursorPutPos = LIST_ITEM_PY;
				}else{
					wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * -8;
				}
			}else{
				wk->cursorPutPos = cur_py;
			}
		}else{
			wk->cursorPutPos = cur_py;
		}

	}else{
		if( ( now_pos + vec ) >= LIST_ITEM_MAX ){
			return FALSE;
		}
		if( wk->list[now_pos+vec] == LIST_ITEM_MAX ){
			return FALSE;
		}

		mv_pos  = now_pos + vec;
		mv_item = wk->list[ mv_pos ];

		cur_py = wk->cursorPutPos + ListItemData[now_item].sy;

		ChangeListItemPalette( wk, now_item, GetListPutY(wk,wk->cursorPutPos), LIST_PALETTE );

		if( wk->list[0] == LIST_ITEM_CONTINUE ){
			if( cur_py == 24 ){
				wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * 8;
			}else if( ( cur_py + ListItemData[mv_item].sy ) > 24 ){
				wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * 8;
			}else{
				wk->cursorPutPos = cur_py;
			}
		}else{
			wk->cursorPutPos = cur_py;
		}

	}

	wk->listPos += vec;

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g��̂n�a�i�̕\���؂�ւ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		flg		TRUE = �\��, FALSE = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VanishListObj( START_MENU_WORK * wk, BOOL flg )
{
	u32	i;

	// ��\��
	if( flg == FALSE ){
		for( i=OBJ_ID_PLAYER; i<=OBJ_ID_NEW_MACHINE; i++ ){
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], flg );
		}
	// �\��
	}else{
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_PLAYER], TRUE );

		for( i=0; i<LIST_ITEM_MAX; i++ ){
			if( wk->list[i] == LIST_ITEM_MAX ){
				break;
			}
/*
			if( wk->list[i] == LIST_ITEM_HUSHIGI ){
				if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_HUSHIGI) == MISC_STARTMENU_FLAG_OPEN ){
					GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_HUSHIGI], TRUE );
				}
			}else if( wk->list[i] == LIST_ITEM_BATTLE ){
*/
			if( wk->list[i] == LIST_ITEM_BATTLE ){
				if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_BATTLE) == MISC_STARTMENU_FLAG_OPEN ){
					GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_BATTLE], TRUE );
				}
			}else if( wk->list[i] == LIST_ITEM_GAME_SYNC ){
				if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_GAMESYNC) == MISC_STARTMENU_FLAG_OPEN ){
					GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_GAMESYNC], TRUE );
				}
			}else if( wk->list[i] == LIST_ITEM_MACHINE ){
				if( MISC_GetStartMenuFlag(wk->misc,MISC_STARTMENU_TYPE_MACHINE) == MISC_STARTMENU_FLAG_OPEN ){
					GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_MACHINE], TRUE );
				}
			}
		}
	}
}


//============================================================================================
//	�e���ڂ̏���
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�x���E�B���h�E�\��
 *
 * @param		px		�\���w���W
 * @param		py		�\���x���W
 * @param		sx		�w�T�C�Y
 * @param		sy		�x�T�C�Y
 * @param		frm		�a�f�t���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutWarrningWindow( u8 px, u8 py, u8 sx, u8 sy, u8 frm )
{
	// ����
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM, px, py, 1, 1, WARRNING_WIN_PLTT_NUM );
	// �E��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+2, px+sx-1, py, 1, 1, WARRNING_WIN_PLTT_NUM );
	// ����
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+6, px, py+sy-1, 1, 1, WARRNING_WIN_PLTT_NUM );
	// �E��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+8, px+sx-1, py+sy-1, 1, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+1, px+1, py, sx-2, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+7, px+1, py+sy-1, sx-2, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+3, px, py+1, 1, sy-2, WARRNING_WIN_PLTT_NUM );
	// �E
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+5, px+sx-1, py+1, 1, sy-2, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+4, px+1, py+1, sx-2, sy-2, WARRNING_WIN_PLTT_NUM );

	GFL_BG_LoadScreenV_Req( frm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�x���E�B���h�E�N���A
 *
 * @param		px		�\���w���W
 * @param		py		�\���x���W
 * @param		sx		�w�T�C�Y
 * @param		sy		�x�T�C�Y
 * @param		frm		�a�f�t���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClearWarrningWindow( u8 px, u8 py, u8 sx, u8 sy, u8 frm )
{
	GFL_BG_FillScreen( frm, 0, px, py, sx, sy, 0 );
	GFL_BG_LoadScreenV_Req( frm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�ŏ�����n�߂�v�̌x���E�B���h�E�\��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutNewGameWarrning( START_MENU_WORK * wk )
{
	STRBUF * str;

	wk->utilWin.win = GFL_BMPWIN_Create(
											BMPWIN_NEWGAME_WIN_FRM,
											BMPWIN_NEWGAME_WIN_PX, BMPWIN_NEWGAME_WIN_PY,
											BMPWIN_NEWGAME_WIN_SX, BMPWIN_NEWGAME_WIN_SY,
											BMPWIN_NEWGAME_WIN_PAL, GFL_BMP_CHRAREA_GET_B );

	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_06 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_ATTENTION_PY, str, wk->font, FCOL_WP05RN );
	GFL_STR_DeleteBuffer( str );
	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_07 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_MESSAGE_PY, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �`
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_08 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_A_BUTTON_PY, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );
	// �a
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_09 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_B_BUTTON_PY, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeScreen( wk->utilWin.win );
	GFL_BG_LoadScreenV_Req( BMPWIN_NEWGAME_WIN_FRM );

	// �E�B���h�E�`��
	PutWarrningWindow(
		NEW_GAME_WARRNING_WIN_PX, NEW_GAME_WARRNING_WIN_PY,
		NEW_GAME_WARRNING_WIN_SX, NEW_GAME_WARRNING_WIN_SY, GFL_BG_FRAME2_M );

	// ���X�g��ޔ�
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_X_SET, 256 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_X_SET, 256 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, 0 );

	VanishListObj( wk, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�ŏ�����n�߂�v�̌x���E�B���h�E�N���A
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		flg		TRUE = ��\��
 *
 * @return	none
 *
 * @li	flg = FALSE �̏ꍇ��BMPWIN�̍폜�̂�
 */
//--------------------------------------------------------------------------------------------
static void ClearNewGameWarrning( START_MENU_WORK * wk, BOOL flg )
{
	GFL_BMPWIN_Delete( wk->utilWin.win );

	if( flg == TRUE ){ return; }

	ClearWarrningWindow(
		NEW_GAME_WARRNING_WIN_PX, NEW_GAME_WARRNING_WIN_PY,
		NEW_GAME_WARRNING_WIN_SX, NEW_GAME_WARRNING_WIN_SY, GFL_BG_FRAME2_M );

	GFL_BG_ClearScreenCodeVReq( BMPWIN_NEWGAME_WIN_FRM, 0 );

	// ���X�g���A
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_X_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_X_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );

	VanishListObj( wk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u��������n�߂�v�̏��E�B���h�E�\��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutContinueInfo( START_MENU_WORK * wk )
{
	STRBUF * str;

	wk->utilWin.win = GFL_BMPWIN_Create(
											BMPWIN_CONTINUE_WIN_FRM,
											BMPWIN_CONTINUE_WIN_PX, BMPWIN_CONTINUE_WIN_PY,
											BMPWIN_CONTINUE_WIN_SX, BMPWIN_CONTINUE_WIN_SY,
											BMPWIN_CONTINUE_WIN_PAL, GFL_BMP_CHRAREA_GET_B );

	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_04 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, CONTINUE_INFO_ATTENTION_PY, str, wk->font, FCOL_WP05RN );
	GFL_STR_DeleteBuffer( str );
	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_05 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, CONTINUE_INFO_MESSAGE_PY, str, wk->font, FCOL_WP05WN );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeScreen( wk->utilWin.win );
	GFL_BG_LoadScreenV_Req( BMPWIN_CONTINUE_WIN_FRM );

	// �E�B���h�E�`��
	PutWarrningWindow(
		CONTINUE_INFO_WIN_PX, CONTINUE_INFO_WIN_PY,
		CONTINUE_INFO_WIN_SX, CONTINUE_INFO_WIN_SY, GFL_BG_FRAME1_S );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u��������n�߂�v�̏��E�B���h�E�N���A
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClearContinueInfo( START_MENU_WORK * wk )
{
/*
	ClearWarrningWindow(
		CONTINUE_INFO_WIN_PX, CONTINUE_INFO_WIN_PY,
		CONTINUE_INFO_WIN_SX, CONTINUE_INFO_WIN_SY, GFL_BG_FRAME1_S );
	GFL_BG_ClearScreenCodeVReq( BMPWIN_CONTINUE_WIN_FRM, 0 );
*/

	GFL_BMPWIN_Delete( wk->utilWin.win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�\���J�n
 *
 * @param		wk				�^�C�g�����j���[���[�N
 * @param		strIdx		������C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void StartMessage( START_MENU_WORK * wk, int strIdx )
{
  GFL_MSG_GetString( wk->mman, strIdx, wk->exp );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[BMPWIN_MSG].win), 15 );
	BmpWinFrame_Write(
		wk->util[BMPWIN_MSG].win, WINDOW_TRANS_OFF, MESSAGE_WIN_CHAR_NUM, MESSAGE_WIN_PLTT_NUM );

	wk->stream = PRINTSYS_PrintStream(
								wk->util[BMPWIN_MSG].win,
								0, 0, wk->exp,
								wk->font,
								MSGSPEED_GetWait(),
								wk->tcbl,
								10,		// tcbl pri
								HEAPID_STARTMENU,
								15 );	// clear color

	wk->stream_clear_flg = FALSE;

	GFL_BMPWIN_MakeTransWindow_VBlank( wk->util[BMPWIN_MSG].win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W��\��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClearMessage( START_MENU_WORK * wk )
{
	BmpWinFrame_Clear( wk->util[BMPWIN_MSG].win, WINDOW_TRANS_ON_V );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W���C��
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	"TRUE = ���b�Z�[�W������"
 * @retval	"FALSE = ���b�Z�[�W�����I��"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainMessage( START_MENU_WORK * wk )
{
	APP_KEYCURSOR_Main( wk->kcwk, wk->stream, wk->util[BMPWIN_MSG].win );

  switch( PRINTSYS_PrintStreamGetState(wk->stream) ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      PRINTSYS_PrintStreamShortWait( wk->stream, 0 );
    }
		wk->stream_clear_flg = FALSE;
    break;

  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
		if( wk->stream_clear_flg == FALSE ){
			if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
	      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
	      PRINTSYS_PrintStreamReleasePause( wk->stream );
				wk->stream_clear_flg = TRUE;
	    }
		}
    break;

  case PRINTSTREAM_STATE_DONE: //�I��
    PRINTSYS_PrintStreamDelete( wk->stream );
		wk->stream_clear_flg = FALSE;
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�������ݒ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
static void SetYesNoMenu( START_MENU_WORK * wk )
{
	BMPWIN_YESNO_DAT	dat;

	dat.frmnum = BMPWIN_YESNO_FRM;
	dat.pos_x  = BMPWIN_YESNO_PX;
	dat.pos_y  = BMPWIN_YESNO_PY;
	dat.palnum = BMPWIN_YESNO_PAL;
	dat.chrnum = 0;

	wk->mwk = BmpMenu_YesNoSelectInit(
							&dat, MESSAGE_WIN_CHAR_NUM, MESSAGE_WIN_PLTT_NUM, 0, HEAPID_STARTMENU );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�s�v�c�ȑ��蕨�v��M�������邩
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
/*
static void InitHushigiCheck( START_MENU_WORK * wk )
{
	if( MISC_GetStartMenuFlag( wk->misc, MISC_STARTMENU_TYPE_HUSHIGI ) == 0 ){
		DELIVERY_BEACON_INIT init;
    GFL_STD_MemClear( &init, sizeof(DELIVERY_BEACON_INIT) );
    init.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
    init.data[0].datasize = 0;  //�f�[�^�S�̃T�C�Y
    init.data[0].pData = NULL;     // ��M�o�b�t�@�f�[�^
    init.data[0].LangCode  = CasetteLanguage;     // �󂯎�錾��R�[�h
    init.data[0].version   = 1<<GET_VERSION();     // �󂯎��o�[�W�����̃r�b�g
    init.dataNum = 1;  //�󂯎�葤�͂P
    init.ConfusionID = 0;   //�������Ȃ����߂�ID
    init.heapID = HEAPID_STARTMENU;


		wk->bwk = DELIVERY_BEACON_Init( &init );
		DELIVERY_BEACON_RecvStart( wk->bwk );
		wk->hushigiFlag = TRUE;
	}else{
		wk->hushigiFlag = FALSE;
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�s�v�c�ȑ��蕨�v���[�N�폜
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
/*
static void ExitHushigiCheck( START_MENU_WORK * wk )
{
	if( wk->bwk != NULL ){
		DELIVERY_BEACON_End( wk->bwk );
		wk->bwk = NULL;
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�s�v�c�ȑ��蕨�v��M�`�F�b�N
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
/*
static BOOL CheckHushigiBeacon( START_MENU_WORK * wk )
{
	// ��M�s����
	if( wk->hushigiFlag == FALSE ){
		return FALSE;
	}

	DELIVERY_BEACON_Main( wk->bwk );

	if( DELIVERY_BEACON_RecvSingleCheck(wk->bwk) == TRUE ){
		ExitHushigiCheck( wk );
		wk->hushigiFlag = FALSE;
		return TRUE;
	}
	return FALSE;
}
*/

//============================================================================================
//	���̑�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���ݒ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetFadeIn( START_MENU_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STARTMENU );
	wk->fadeSeq = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�ݒ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetFadeOut( START_MENU_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STARTMENU );
	wk->fadeSeq = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���A�j���ݒ�
 *
 * @param		wk		�^�C�g�����j���[���[�N
 * @param		next	�A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetButtonAnm( START_MENU_WORK * wk, int next )
{
	wk->btnCnt = 0;
	wk->btnSeq = 0;
	wk->nextSeq = next;
	return MAINSEQ_BUTTON_ANM;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�f�[�^�j���`�F�b�N
 *
 * @param		wk		�^�C�g�����j���[���[�N
 *
 * @retval	"TRUE = �j������"
 * @retval	"FALSE = �j���Ȃ�"
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckSaveDataBreak( START_MENU_WORK * wk )
{
	wk->svBreak = SaveControl_GetLoadResult( wk->savedata );

	// �j���Ȃ�
	if( wk->svBreak == 0 ){
		return FALSE;
	}
	// �j������
	return TRUE;
}

















