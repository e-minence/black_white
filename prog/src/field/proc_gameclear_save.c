//============================================================================================
/**
 * @file  proc_gameclear_save.c
 * @date  2009.12.20
 * @author  tamada GAMEFREAK inc.
 * @brief �Q�[���N���A���̃Z�[�u�Ȃ�
 *
 * @todo
 * �K���ɍ쐬��������ʂȂ̂ŁA���ۂɂ�UI�ǂ�������Ƃ����d�l�����Ƃɍ�蒼���B
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/gametime.h"
#include "savedata/playtime.h"
#include "ev_time.h"

#include "proc_gameclear_save.h"

#include "system/main.h"  //HEAPID_�`
#include "arc/arc_def.h" //ARCID_�`

//==============================================================================================
//
//	���b�Z�[�W�\���֘A
//
//==============================================================================================
#include "print/wordset.h"  //WORDSET_
#include "print/printsys.h" //PRINTSYS_
#include "print/gf_font.h"  // GFL_FONT_


#include "font/font.naix" //NARC_font_large_gftr
#include "message.naix"
#include "msg/msg_gameclear.h"
//#include "arc/script_message.naix"
//#include "msg/script/msg_common_scr.h"

//#include "field/field_msgbg.h"
#include "system/bmp_winframe.h"


//==============================================================================================
//==============================================================================================
//----------------------------------------------------------------------------------------------
//	�\���̐錾
//----------------------------------------------------------------------------------------------
typedef struct{
  HEAPID heapID;
  u16 arc_id;
  GAMEDATA * gamedata;
  const MYSTATUS * mystatus;

  GFL_FONT * fontHandle;
	GFL_MSGDATA* msgman;						//���b�Z�[�W�}�l�[�W���[
	WORDSET* wordset;								//�P��Z�b�g
	GFL_BMPWIN *bmpwin;							//BMP�E�B���h�E�f�[�^

  BOOL saveSuccessFlag;
  BOOL dendouSaveInitFlag;   //�u�a������v�f�[�^�̕����Z�[�u�������������������ǂ���
  BOOL dendouSaveFinishFlag; //�u�a������v�f�[�^�̕����Z�[�u�������������ǂ���
  BOOL otherSaveExist; // �����̃f�[�^�����݂��邩�ǂ���

}GAMECLEAR_MSG_WORK;


#define GAMECLEAR_MSG_BUF_SIZE		(1024)			//���b�Z�[�W�o�b�t�@�T�C�Y
#define GAMECLEAR_FADE_SYNC			(8)				//�t�F�[�hsync��

enum { //�R���p�C����ʂ����߂ɂƂ肠����

  FLD_SYSFONT_PAL = 13,

  FBMP_COL_NULL = 0,
  FBMP_COL_BLACK = 0xf,
  FBMP_COL_BLK_SDW = 2,

  PALNO_FONT = 1,//FBMP_COL_BLACK,
  PALNO_FONTSHADOW = FBMP_COL_BLK_SDW,
  PALNO_BACKGROUND = 15,//FBMP_COL_NULL,
};
//----------------------------------------------------------------------------------------------
//	BMP�E�B���h�E
//----------------------------------------------------------------------------------------------
enum{
	GAMECLEAR_BMPWIN_FRAME	= GFL_BG_FRAME0_M,
	GAMECLEAR_BMPWIN_PX1	= 3,
	GAMECLEAR_BMPWIN_PY1	= 5,
	GAMECLEAR_BMPWIN_SX		= 28,
	GAMECLEAR_BMPWIN_SY		= 15,
	GAMECLEAR_BMPWIN_PL		= FLD_SYSFONT_PAL,
	GAMECLEAR_BMPWIN_CH		= 1,
};

//==============================================================================================
//==============================================================================================
//----------------------------------------------------------------------------------------------
//	�v���g�^�C�v�錾
//----------------------------------------------------------------------------------------------

static void setup_bg_sys( HEAPID heapID );
static void release_bg_sys( void );
static void scr_msg_print( GAMECLEAR_MSG_WORK* wk, u16 msg_id, u8 x, u8 y );

static void ElboardStartChampNews( GAMECLEAR_MSG_WORK* work ); // �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����
static void UpdateFirstClearDendouData( GAMECLEAR_MSG_WORK* work ); // ����N���A���̓a������f�[�^��o�^����
static void UpdateFirstClearRecord( GAMECLEAR_MSG_WORK* work ); // ����N���A���̃��R�[�h��o�^����
static void UpdateDendouRecord( GAMECLEAR_MSG_WORK* work ); // �a�����莞�̃��R�[�h��o�^����
static void DendouSave_init( GAMECLEAR_MSG_WORK* work ); // �a������f�[�^�̍X�V����
static void DendouSave_main( GAMECLEAR_MSG_WORK* work ); // �a������f�[�^�̍X�V����


static GFL_PROC_RESULT GameClearMsgProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameClearMsgProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameClearMsgProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
const GFL_PROC_DATA GameClearMsgProcData = {
  GameClearMsgProc_Init,
  GameClearMsgProc_Main,
  GameClearMsgProc_End,
};

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT GameClearMsgProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMECLEAR_MSG_WORK * gcmwk;
  GAMECLEAR_MSG_PARAM * para = pwk;
  GAMESYS_WORK * gsys = para->gsys;

 	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMEOVER_MSG, 0x40000 );
  gcmwk = GFL_PROC_AllocWork(proc, sizeof(GAMECLEAR_MSG_WORK), HEAPID_GAMEOVER_MSG);
	GFL_STD_MemClear32( gcmwk, sizeof(GAMECLEAR_MSG_WORK) );
  gcmwk->heapID = HEAPID_GAMEOVER_MSG;
  gcmwk->arc_id = NARC_message_gameclear_dat;
  gcmwk->gamedata = GAMESYSTEM_GetGameData( gsys );
  gcmwk->mystatus = GAMEDATA_GetMyStatus( gcmwk->gamedata );

  // �Z�[�u�\���H
  {
    SAVE_CONTROL_WORK* save;
    save = GAMEDATA_GetSaveControlWork( gcmwk->gamedata );

    if( SaveControl_IsOverwritingOtherData( save ) ) {
      // �Z�[�u�s��
      gcmwk->otherSaveExist = TRUE;
    }
  }
  
  return GFL_PROC_RES_FINISH;
}
  
//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT GameClearMsgProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMECLEAR_MSG_WORK * wk = mywk;
  GAMECLEAR_MSG_PARAM * para = pwk;

  int trg = GFL_UI_KEY_GetTrg();

	switch( *seq ){
  case 0:

    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    setup_bg_sys( wk->heapID );

		wk->fontHandle = GFL_FONT_Create(
			ARCID_FONT,
      NARC_font_large_gftr, //�V�t�H���gID
			GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

    //���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
    wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
        ARCID_MESSAGE, wk->arc_id, wk->heapID);
    wk->wordset = WORDSET_Create( wk->heapID );

    //�r�b�g�}�b�v�ǉ�
    wk->bmpwin = GFL_BMPWIN_Create(
      GAMECLEAR_BMPWIN_FRAME,						//�E�C���h�E�g�p�t���[��
      GAMECLEAR_BMPWIN_PX1,GAMECLEAR_BMPWIN_PY1,	//�E�C���h�E�̈�̍����X,Y���W�i�L�����P�ʂŎw��j
      GAMECLEAR_BMPWIN_SX, GAMECLEAR_BMPWIN_SY,	//�E�C���h�E�̈��X,Y�T�C�Y�i�L�����P�ʂŎw��j
      GAMECLEAR_BMPWIN_PL,						//�E�C���h�E�̈�̃p���b�g�i���o�[	
      GAMECLEAR_BMPWIN_CH							//�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
    );

    if ( para->clear_mode == GAMECLEAR_MODE_FIRST ) {
      //�Q�[���N���A�@���߂łƂ��I
      scr_msg_print( wk, msg_gameclear_first_01, 0, 0 );
    } else {
      //�ł�ǂ�����@���߂łƂ��I
      scr_msg_print( wk, msg_gameclear_01, 0, 0 );
    }

    GFL_BMPWIN_MakeTransWindow( wk->bmpwin );

    (*seq) ++;
    break;

	//���b�Z�[�W�]���҂�
	case 1:
    {
      G2_BlendNone();
      GX_SetVisiblePlane( GX_PLANEMASK_BG0 );
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0 );
    }
		(*seq)++;
		break;

	//���C����ʃt�F�[�h�C���҂�
	case 2:
    if( GFL_FADE_CheckFade() == FALSE ) {
			(*seq)++;
		}
		break;

	//�L�[�҂�
	case 3:
    if( wk->otherSaveExist ) {
      (*seq) ++; 
    }
    else if( (trg & PAD_BUTTON_A) || (trg & PAD_BUTTON_B) ) {
      //�u���|�[�g�������Ă��܂��@�ł񂰂������Ȃ��ł��������v
      scr_msg_print( wk, msg_gameclear_report_01, 0, 0 );
      GAMEDATA_SaveAsyncStart( wk->gamedata );
      (*seq) ++;
    }
    break;

  case 4:
    // �����Z�[�u���s
    if( wk->otherSaveExist == FALSE ) {
      SAVE_RESULT result = GAMEDATA_SaveAsyncMain( wk->gamedata );

      // ���ʂ�Ԃ�
      switch( result ) {
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
        wk->saveSuccessFlag = TRUE;
        (*seq) ++;
        break;
      case SAVE_RESULT_NG:
        wk->saveSuccessFlag = FALSE;
        (*seq) ++;
        break;
      }
    }
    else {
      (*seq) ++;
    }
    break;

  case 5: 
    if( wk->otherSaveExist==FALSE && wk->saveSuccessFlag ) {
      // �d���f���Ƀ`�����s�I���j���[�X��\��
      ElboardStartChampNews( wk );

      //�u����N���A�v���Z�[�u
      if( para->clear_mode == GAMECLEAR_MODE_FIRST ) {
        UpdateFirstClearDendouData( wk ); // ���u�a������v�f�[�^
        EVTIME_SetGameClearDateTime( wk->gamedata ); // ����N���A�̓���
        UpdateFirstClearRecord( wk ); // ���R�[�h�f�[�^
      }
      //�u�a������v���Z�[�u
      else {
        UpdateDendouRecord( wk );
        DendouSave_init( wk );
      }
    }
    (*seq) ++;
    break;

  case 6:
    // �a������f�[�^���Z�[�u
    if( wk->otherSaveExist==FALSE && wk->dendouSaveInitFlag ) {
      DendouSave_main( wk );

      // �Z�[�u�I��
      if( wk->dendouSaveFinishFlag ) { 
        (*seq) ++; 
      }
    }
    else {
      (*seq) ++;
    }
    break;

  case 7:
    if (wk->otherSaveExist==FALSE && wk->saveSuccessFlag) {
      //�u���|�[�g�������܂����v
      scr_msg_print( wk, msg_gameclear_report_02, 0, 0 );
    } else {
      //�u���|�[�g�������܂���ł����v
      scr_msg_print( wk, msg_gameclear_report_03, 0, 0 );
    }
    (*seq) ++;
    break;

  case 8:
		if( (trg & PAD_BUTTON_A) || (trg & PAD_BUTTON_B) ) {
      //�u�����܂��v
      scr_msg_print( wk, msg_gameclear_02, 0, 0 );
      (*seq) ++;
    }
    break;

  case 9:
		if( (trg & PAD_BUTTON_A) || (trg & PAD_BUTTON_B) ) {
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0 );
			(*seq)++;
		}
		break;

	//���C����ʃt�F�[�h�A�E�g�҂�
	case 10:
    if( GFL_FADE_CheckFade() == FALSE ) {

			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpwin),  PALNO_BACKGROUND );		//�h��Ԃ�

			(*seq)++;
		}
		break;

	//�I���J��
	case 11:
		BmpWinFrame_Clear( wk->bmpwin, WINDOW_TRANS_ON );
		GFL_BMPWIN_Delete( wk->bmpwin );

		WORDSET_Delete( wk->wordset );
		GFL_MSG_Delete( wk->msgman );
		GFL_BG_FreeBGControl( GAMECLEAR_BMPWIN_FRAME );

    GFL_FONT_Delete( wk->fontHandle );

    release_bg_sys();

    (*seq) ++;
    break;

  case 12:
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
	GFL_BG_SetBGControl( GAMECLEAR_BMPWIN_FRAME, &header, GFL_BG_MODE_TEXT );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_systemwin_nclr, PALTYPE_MAIN_BG, 
																0x20*GAMECLEAR_BMPWIN_PL, 0x20, heapID );

	GFL_BG_SetBackGroundColor(GAMECLEAR_BMPWIN_FRAME,0);

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
  PRINTSYS_LSB lsb = PRINTSYS_LSB_Make( PALNO_FONT, PALNO_FONTSHADOW, PALNO_BACKGROUND);
	STRBUF* tmp_buf = GFL_STR_CreateBuffer( GAMECLEAR_MSG_BUF_SIZE, wk->heapID );
	STRBUF* tmp_buf2= GFL_STR_CreateBuffer( GAMECLEAR_MSG_BUF_SIZE, wk->heapID );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpwin),  PALNO_BACKGROUND );			//�h��Ԃ�

	GFL_MSG_GetString( wk->msgman, msg_id, tmp_buf );

  //��l�����Z�b�g
  WORDSET_RegisterPlayerName( wk->wordset, 0, wk->mystatus );

	WORDSET_ExpandStr( wk->wordset, tmp_buf2, tmp_buf );
  GFL_FONTSYS_SetColor( PALNO_FONT, PALNO_FONTSHADOW,  PALNO_BACKGROUND );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( wk->bmpwin ), x, y, tmp_buf2, wk->fontHandle );

	GFL_STR_DeleteBuffer( tmp_buf );
	GFL_STR_DeleteBuffer( tmp_buf2 );

  GFL_BMPWIN_TransVramCharacter( wk->bmpwin );  //transfer characters
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( wk->bmpwin ) );  //transfer screen

	return;
}


//-----------------------------------------------------------------------------
/**
 * @brief �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ElboardStartChampNews( GAMECLEAR_MSG_WORK* work )
{
  MISC* misc;
  int minutes;

  misc = GAMEDATA_GetMiscWork( work->gamedata );
  minutes = 60 * 24; // �\�����鎞��[min]
  MISC_SetChampNewsMinutes( misc, minutes );
}

//-----------------------------------------------------------------------------
/**
 * @brief ����N���A���̓a������f�[�^��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateFirstClearDendouData( GAMECLEAR_MSG_WORK* work )
{
  RTCDate date;
  POKEPARTY* party; 
  DENDOU_RECORD* record; 

  RTC_GetDate( &date );
  party  = GAMEDATA_GetMyPokemon( work->gamedata ); 
  record = GAMEDATA_GetDendouRecord( work->gamedata ); 
  DendouRecord_Add( record, party, &date, HEAPID_PROC );
}

//-----------------------------------------------------------------------------
/**
 * @brief ����N���A���̃��R�[�h��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateFirstClearRecord( GAMECLEAR_MSG_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  RECORD* record;
  PLAYTIME* ptime;
  u32 hour, minute, second, time;

  save   = GAMEDATA_GetSaveControlWork( work->gamedata );
  record = GAMEDATA_GetRecordPtr( work->gamedata );
  ptime  = SaveData_GetPlayTime( save );

  // �v���C���Ԃ��擾
  hour   = PLAYTIME_GetHour( ptime );
  minute = PLAYTIME_GetMinute( ptime );
  second = PLAYTIME_GetSecond( ptime ); 
  GF_ASSERT( hour   <= PTIME_HOUR_MAX );
  GF_ASSERT( minute <= PTIME_MINUTE_MAX );
  GF_ASSERT( second <= PTIME_SECOND_MAX );

  // �v���C���Ԃ�o�^
  time = hour * 10000 + minute * 100 + second;
  RECORD_Set( record, RECID_CLEAR_TIME, time );
}

//-----------------------------------------------------------------------------
/**
 * @brief �a�����莞�̃��R�[�h��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateDendouRecord( GAMECLEAR_MSG_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  RECORD* record;

  save = GAMEDATA_GetSaveControlWork( work->gamedata );
  record = GAMEDATA_GetRecordPtr( work->gamedata );

  // �f�[�^�㏑���`�F�b�N
  if( SaveControl_IsOverwritingOtherData( save ) ) { return; }

  RECORD_Add( record, RECID_DENDOU_CNT, 1 );
}

//-----------------------------------------------------------------------------
/**
 * @brief �a������f�[�^�̍X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DendouSave_init( GAMECLEAR_MSG_WORK* work )
{
  RTCDate date;
  POKEPARTY* party;
  SAVE_CONTROL_WORK* save;
  DENDOU_SAVEDATA* dendouData;
  LOAD_RESULT result; 

  RTC_GetDate( &date );
  party = GAMEDATA_GetMyPokemon( work->gamedata ); 
  save = GAMEDATA_GetSaveControlWork( work->gamedata );
  result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, HEAPID_PROC );
  dendouData = SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );

  // �f�[�^�㏑���`�F�b�N
  if( SaveControl_IsOverwritingOtherData( save ) ) { return; }

  DendouData_AddRecord( dendouData, party, &date, HEAPID_PROC ); 
  GAMEDATA_ExtraSaveAsyncStart( work->gamedata, SAVE_EXTRA_ID_DENDOU ); 

  work->dendouSaveInitFlag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �a������f�[�^�̍X�V����
 *
 * @param work 
 */
//-----------------------------------------------------------------------------
static void DendouSave_main( GAMECLEAR_MSG_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  SAVE_RESULT save_ret;

  GF_ASSERT( work->dendouSaveInitFlag );

  // �f�[�^�㏑���`�F�b�N
  save = GAMEDATA_GetSaveControlWork( work->gamedata );
  if( SaveControl_IsOverwritingOtherData( save ) ) { return; }

  // �Z�[�u���s
  save_ret = GAMEDATA_ExtraSaveAsyncMain( work->gamedata, SAVE_EXTRA_ID_DENDOU ); 

  // �Z�[�u�I��
  if( save_ret == SAVE_RESULT_OK || save_ret == SAVE_RESULT_NG ) { 
    SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU ); // �O���f�[�^�����
    work->dendouSaveFinishFlag = TRUE;
  }
} 
