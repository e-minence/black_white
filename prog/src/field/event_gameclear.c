//============================================================================================
/**
 * @file	event_gameclear.c
 * @brief	�C�x���g�F�S�ŏ���
 * @date  2009.12.14
 * @author	tamada GAME FREAK inc.
 *
 * 2009.12.14 event_gameover.c����R�s�y�ō쐬
 *
 * @todo
 * �Q�[���I�[�o�[���b�Z�[�W��PROC�ɂ����̂Ń�����������Ȃ��Ƃ��ɂ�
 * �ʃI�[�o�[���C�̈�Ɉ��z������
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

#include "event_gameclear.h"  //GAMECLEAR_MODE

#include "system/main.h"  //HEAPID_�`

#include "system/brightness.h"

#include "system/wipe.h"

//#include "warpdata.h"			//WARPDATA_�`
#include "script.h"       //SCRIPT_CallScript
//#include "event_fieldmap_control.h" //
#include "event_mapchange.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.
#include "app/local_tvt_sys.h"  //LocalTvt_ProcData etc.

#include "field/field_msgbg.h"
#include "system/bmp_winframe.h"

#include "sound/pm_sndsys.h"  //PMSND_

#include "move_pokemon.h"

//==============================================================================================
//
//	���b�Z�[�W�\���֘A
//
//==============================================================================================
#include "print/wordset.h"


#include "font/font.naix" //NARC_font_large_gftr
//#include "message.naix"
//#include "msg/msg_gameover.h"
#include "arc/script_message.naix"
#include "msg/script/msg_common_scr.h"
//#include "arc/fieldmap/zone_id.h"


//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMECLEAR_MODE clear_mode;
  BOOL saveSuccessFlag;
  u16 arc_id;
  u16 msg_id;
  const MYSTATUS * mystatus;
  void * pWork;
}GAMECLEAR_WORK;


//==============================================================================================
//==============================================================================================
//----------------------------------------------------------------------------------------------
//	�\���̐錾
//----------------------------------------------------------------------------------------------
typedef struct{
  HEAPID heapID;
  u16 arc_id;
  u16 msg_id;
  const MYSTATUS * mystatus;

	GFL_MSGDATA* msgman;						//���b�Z�[�W�}�l�[�W���[
	WORDSET* wordset;								//�P��Z�b�g
	GFL_BMPWIN *bmpwin;							//BMP�E�B���h�E�f�[�^

  PRINT_QUE *printQue;
  GFL_FONT * fontHandle;
}GAMECLEAR_MSG_WORK;


#define GAME_OVER_MSG_BUF_SIZE		(1024)			//���b�Z�[�W�o�b�t�@�T�C�Y
#define GAME_OVER_FADE_SYNC			(8)				//�t�F�[�hsync��

enum { //�R���p�C����ʂ����߂ɂƂ肠����

  FLD_SYSFONT_PAL = 13,

  FBMP_COL_NULL = 0,
  FBMP_COL_BLACK = 0xf,
  FBMP_COL_BLK_SDW = 2,
};
//----------------------------------------------------------------------------------------------
//	BMP�E�B���h�E
//----------------------------------------------------------------------------------------------
enum{
	GAME_OVER_BMPWIN_FRAME	= GFL_BG_FRAME0_M,
	//GAME_OVER_BMPWIN_PX1	= 1,//2,
	//GAME_OVER_BMPWIN_PY1	= 1,//2,
	//GAME_OVER_BMPWIN_SX		= 29,//25,
	//GAME_OVER_BMPWIN_SY		= 19,//18,
	GAME_OVER_BMPWIN_PX1	= 4,
	GAME_OVER_BMPWIN_PY1	= 5,
	GAME_OVER_BMPWIN_SX		= 25,
	GAME_OVER_BMPWIN_SY		= 15,
	GAME_OVER_BMPWIN_PL		= FLD_SYSFONT_PAL,
	GAME_OVER_BMPWIN_CH		= 1,
};

//==============================================================================================
//==============================================================================================
//----------------------------------------------------------------------------------------------
//	�v���g�^�C�v�錾
//----------------------------------------------------------------------------------------------

static void setup_bg_sys( HEAPID heapID );
static void release_bg_sys( void );
static void scr_msg_print( GAMECLEAR_MSG_WORK* wk, u16 msg_id, u8 x, u8 y );


static GFL_PROC_RESULT GameClearMsgProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameClearMsgProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameClearMsgProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================================
//==============================================================================================
static const GFL_PROC_DATA GameClearMsgProcData = {
  GameClearMsgProc_Init,
  GameClearMsgProc_Main,
  GameClearMsgProc_End,
};

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT GameClearMsgProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMECLEAR_MSG_WORK * gcmwk;
  const GAMECLEAR_WORK * gcwk = pwk;

 	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMEOVER_MSG, 0x40000 );
  gcmwk = GFL_PROC_AllocWork(proc, sizeof(GAMECLEAR_MSG_WORK), HEAPID_GAMEOVER_MSG);
	GFL_STD_MemClear32( gcmwk, sizeof(GAMECLEAR_MSG_WORK) );
  gcmwk->heapID = HEAPID_GAMEOVER_MSG;
  gcmwk->arc_id = gcwk->arc_id;
  gcmwk->msg_id = gcwk->msg_id;
  gcmwk->mystatus = gcwk->mystatus;
  
  return GFL_PROC_RES_FINISH;
}
  
//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT GameClearMsgProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMECLEAR_MSG_WORK * wk = mywk;

  int trg = GFL_UI_KEY_GetTrg();

	switch( *seq ){
  case 0:

    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_WHITE);
    WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_WHITE);
    WIPE_ResetWndMask(WIPE_DISP_MAIN);
    WIPE_ResetWndMask(WIPE_DISP_SUB);

    setup_bg_sys( wk->heapID );

		wk->fontHandle = GFL_FONT_Create(
			ARCID_FONT,
      NARC_font_large_gftr, //�V�t�H���gID
			GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

    wk->printQue = PRINTSYS_QUE_Create( wk->heapID );
    //���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
    wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
        ARCID_SCRIPT_MESSAGE, wk->arc_id, wk->heapID);
    wk->wordset = WORDSET_Create( wk->heapID );

    //�r�b�g�}�b�v�ǉ�
    wk->bmpwin = GFL_BMPWIN_Create(
      GAME_OVER_BMPWIN_FRAME,						//�E�C���h�E�g�p�t���[��
      GAME_OVER_BMPWIN_PX1,GAME_OVER_BMPWIN_PY1,	//�E�C���h�E�̈�̍����X,Y���W�i�L�����P�ʂŎw��j
      GAME_OVER_BMPWIN_SX, GAME_OVER_BMPWIN_SY,	//�E�C���h�E�̈��X,Y�T�C�Y�i�L�����P�ʂŎw��j
      GAME_OVER_BMPWIN_PL,						//�E�C���h�E�̈�̃p���b�g�i���o�[	
      GAME_OVER_BMPWIN_CH							//�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
    );

    scr_msg_print( wk, wk->msg_id, 0, 0 );

    GFL_BMPWIN_MakeTransWindow( wk->bmpwin );

    (*seq) ++;
    break;

	//���b�Z�[�W�]���҂�
	case 1:
    PRINTSYS_QUE_Main( wk->printQue );
    if( PRINTSYS_QUE_IsFinished( wk->printQue ) == TRUE )
    {
      WIPE_SYS_Start(WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_MAX,
          WIPE_FADE_WHITE, GAME_OVER_FADE_SYNC, WIPE_DEF_SYNC, wk->heapID);
      G2_BlendNone();
    }
		(*seq)++;
		break;

	//���C����ʃt�F�[�h�C���҂�
	case 2:
		if (WIPE_SYS_EndCheck()) {
			(*seq)++;
		}
		break;

	//�L�[�҂�
	case 3:
		if( (trg & PAD_BUTTON_A) || (trg & PAD_BUTTON_B) ) {
			WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
					WIPE_FADE_BLACK, GAME_OVER_FADE_SYNC, WIPE_DEF_SYNC, wk->heapID);
			(*seq)++;
		}
		break;

	//���C����ʃt�F�[�h�A�E�g�҂�
	case 4:
		if (WIPE_SYS_EndCheck()) {

			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpwin), FBMP_COL_NULL );		//�h��Ԃ�

			(*seq)++;
		}
		break;

	//�I���J��
	case 5:
		BmpWinFrame_Clear( wk->bmpwin, WINDOW_TRANS_ON );
		GFL_BMPWIN_Delete( wk->bmpwin );

		WORDSET_Delete( wk->wordset );
		GFL_MSG_Delete( wk->msgman );
		GFL_BG_FreeBGControl( GAME_OVER_BMPWIN_FRAME );

    PRINTSYS_QUE_Delete( wk->printQue ); 
    GFL_FONT_Delete( wk->fontHandle );

    release_bg_sys();

		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT GameClearMsgProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_GAMEOVER_MSG );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setup_bg_sys( HEAPID heapID )
{
	static const GFL_DISP_VRAM SetBankData = {
		GX_VRAM_BG_128_B,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_0_A,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_64K, // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K, // �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
	};

	static const GFL_BG_BGCNT_HEADER header = {
		0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
	};

  GFL_BG_Init( heapID );
	GFL_DISP_SetBank( &SetBankData );
	GFL_BG_SetBGMode( &BGsys_data );
	GFL_BG_SetBGControl( GAME_OVER_BMPWIN_FRAME, &header, GFL_BG_MODE_TEXT );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_systemwin_nclr, PALTYPE_MAIN_BG, 
																0x20*GAME_OVER_BMPWIN_PL, 0x20, heapID );

	GFL_BG_SetBackGroundColor(GAME_OVER_BMPWIN_FRAME,0x7FFF);

  GFL_BMPWIN_Init( heapID );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void release_bg_sys( void )
{
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

//--------------------------------------------------------------
/**
 * @brief	���b�Z�[�W�\��
 *
 * @param	wk			EV_WIN_WORK�^�̃A�h���X
 * @param	msg_id		���b�Z�[�WID
 * @param	x			�\���w���W
 * @param	y			�\���x���W
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void scr_msg_print( GAMECLEAR_MSG_WORK* wk, u16 msg_id, u8 x, u8 y )
{
  PRINTSYS_LSB lsb = PRINTSYS_LSB_Make(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL);
	STRBUF* tmp_buf = GFL_STR_CreateBuffer( GAME_OVER_MSG_BUF_SIZE, wk->heapID );
	STRBUF* tmp_buf2= GFL_STR_CreateBuffer( GAME_OVER_MSG_BUF_SIZE, wk->heapID );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpwin), FBMP_COL_NULL );			//�h��Ԃ�

	GFL_MSG_GetString( wk->msgman, msg_id, tmp_buf );

  //��l�����Z�b�g
  WORDSET_RegisterPlayerName( wk->wordset, 0, wk->mystatus );

	WORDSET_ExpandStr( wk->wordset, tmp_buf2, tmp_buf );
  PRINTSYS_PrintQueColor( wk->printQue , GFL_BMPWIN_GetBmp( wk->bmpwin ) , x , y ,
                          tmp_buf2, wk->fontHandle ,
                          lsb );

	GFL_STR_DeleteBuffer( tmp_buf );
	GFL_STR_DeleteBuffer( tmp_buf2 );
	return;
}





//============================================================================================
//============================================================================================
// ���C���V�[�P���X
enum {
	GMCLEAR_SEQ_INIT = 0,			// ������
	GMCLEAR_SEQ_DENDOU_DEMO,		// �a������f��
	GMCLEAR_SEQ_SAVE_START,			// �Z�[�u�J�n
	GMCLEAR_SEQ_SAVE_MESSAGE,		// �Z�[�u�����b�Z�[�W�\��
	GMCLEAR_SEQ_SAVE_MAIN,			// �Z�[�u���C��
	GMCLEAR_SEQ_SAVE_END,			// �Z�[�u�I��
	GMCLEAR_SEQ_WAIT1,				// �E�F�C�g�P
	GMCLEAR_SEQ_FADE_OUT,			// �t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_BGM_FADE_OUT,		// BGM�t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_WAIT2,				// �E�F�C�g�Q
	GMCLEAR_SEQ_ENDING_DEMO,		// �G���f�B���O�f��
	GMCLEAR_SEQ_END,				// �I��
};



//-----------------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�C�x���g
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE		�C�x���g�I��
 * @retval	FALSE		�C�x���g�p����
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameClear(GMEVENT * event, int * seq, void *work)
{
  GAMECLEAR_WORK * gcwk = work;
  GMEVENT * call_event;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gcwk->gsys );

	switch (*seq) {
	case GMCLEAR_SEQ_INIT:
    PMSND_FadeOutBGM( 30 );
    { //�t�B�[���h�}�b�v���I��������
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gcwk->gsys );
      GMEVENT * new_event = DEBUG_EVENT_GameEnd( gcwk->gsys, fieldmap );
      GMEVENT_CallEvent( event, new_event );
      //call_event = EVENT_FieldFadeOut( gcwk->gsys, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
      //GMEVENT_CallEvent( event, call_event );
    }
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_DENDOU_DEMO:
    if ( PMSND_CheckFadeOnBGM() == FALSE ) break;
    SCRIPT_CallGameClearScript( gcwk->gsys, HEAPID_PROC );
    { //�a������̑����N�̏�f��
      DEMO3D_PARAM * param = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(DEMO3D_PARAM) );
      param->demo_id = DEMO3D_ID_N_CASTLE;
      GMEVENT_CallProc( event, FS_OVERLAY_ID(demo3d), &Demo3DProcData, param );
      gcwk->pWork = param;
    }

		(*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_START:
    //�u�㏑�����܂��v�Ƃ��\������\��
    GFL_HEAP_FreeMemory( gcwk->pWork );
		//�x��BG�ȊO��\���I�t
		SetBrightness( BRIGHTNESS_BLACK, (PLANEMASK_ALL^PLANEMASK_BG3), MASK_MAIN_DISPLAY);
		SetBrightness( BRIGHTNESS_BLACK, PLANEMASK_ALL, MASK_SUB_DISPLAY);
		//
    GAMEDATA_SaveAsyncStart( gamedata );
    (*seq) ++;
		break;

  case GMCLEAR_SEQ_SAVE_MESSAGE:
    gcwk->msg_id = msg_common_report_07;
    GMEVENT_CallProc( event, NO_OVERLAY_ID, &GameClearMsgProcData, gcwk );
    (*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_MAIN:
    {
      // �����Z�[�u���s
      SAVE_RESULT result = GAMEDATA_SaveAsyncMain( gamedata );

      // ���ʂ�Ԃ�
      switch( result )
      {
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
        gcwk->saveSuccessFlag = TRUE;
        (*seq) ++;
        break;
      case SAVE_RESULT_NG:
        gcwk->saveSuccessFlag = FALSE;
        (*seq) ++;
        break;
      }
    }
		break;

	case GMCLEAR_SEQ_SAVE_END:
		//
    if (gcwk->saveSuccessFlag) {
      gcwk->msg_id = msg_common_report_04;
    } else {
      gcwk->msg_id = msg_common_report_06;
    }
    GMEVENT_CallProc( event, NO_OVERLAY_ID, &GameClearMsgProcData, gcwk );
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_WAIT1:				// �E�F�C�g�P
    (*seq) ++;
    break;
	case GMCLEAR_SEQ_FADE_OUT:			// �t�F�[�h�A�E�g�Z�b�g
		//�\���I�t����
		SetBrightness( BRIGHTNESS_NORMAL, PLANEMASK_ALL, MASK_DOUBLE_DISPLAY);
    (*seq) ++;
    break;

	case GMCLEAR_SEQ_BGM_FADE_OUT:		// BGM�t�F�[�h�A�E�g�Z�b�g
    (*seq) ++;
    break;
	case GMCLEAR_SEQ_WAIT2:				// �E�F�C�g�Q
    (*seq) ++;
    break;
	case GMCLEAR_SEQ_ENDING_DEMO:
    { //�G���f�B���O�̂����Ƀ��[�J���g�����V�[�o�[
      u16 demo_id = 0;
      LOCAL_TVT_INIT_WORK * ltwk = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(LOCAL_TVT_INIT_WORK) );
      ltwk->scriptId = demo_id;
      ltwk->gameData = gamedata;
      gcwk->pWork = ltwk;
      GMEVENT_CallProc( event, FS_OVERLAY_ID(local_tvt), &LocalTvt_ProcData, ltwk );
    }
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_END:
    GFL_HEAP_FreeMemory( gcwk->pWork );
    return GMEVENT_RES_FINISH;

  case GMCLEAR_SEQ_END + 1:
    /* �������[�v */
    return GMEVENT_RES_CONTINUE;

	}
	return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g�R�}���h�F�ʏ�S�ŏ���
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_GameClear( GAMESYS_WORK * gsys, GAMECLEAR_MODE mode )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
	GMEVENT * event;
  GAMECLEAR_WORK * gcwk;
  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameClear, sizeof(GAMECLEAR_WORK) );
  gcwk = GMEVENT_GetEventWork( event );
  gcwk->gsys = gsys;
  gcwk->clear_mode = mode;
  gcwk->arc_id = NARC_script_message_common_scr_dat;
  gcwk->msg_id = msg_common_report_06;
  gcwk->mystatus = GAMEDATA_GetMyStatus( gamedata );

  //�ړ��|�P���������`�F�b�N
  MP_RecoverMovePoke( gamedata );

  return event;
}


