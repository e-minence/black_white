//============================================================================================
/**
 * @file	event_gameover.c
 * @brief	�C�x���g�F�S�ŏ���
 * @since	2006.04.18
 * @date  2009.09.20
 * @author	tamada GAME FREAK inc.
 *
 * 2006.04.18 scr_tool.c,field_encount.c����Q�[���I�[�o�[�����������Ă��čč\������
 * 2009.09.20 HGSS����ڐA�J�n
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

#include "event_gameover.h"

#include "system/main.h"  //HEAPID_�`

#include "system/brightness.h"

//#include "system/snd_tool.h"
//#include "fld_bgm.h"

//#include "system/window.h"
#include "system/wipe.h"

#include "warpdata.h"			//WARPDATA_�`
#include "script.h"       //SCRIPT_CallScript
#include "event_fieldmap_control.h" //
#include "event_mapchange.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h" //SCRID_�`

//#include "situation_local.h"		//Situation_Get�`
//#include "scr_tool.h"
//#include "mapdefine.h"
//#include "..\fielddata\script\common_scr_def.h"		//SCRID_GAME_OVER_RECOVER_PC
//#include "sysflag.h"
//#include "fld_flg_init.h"			//FldFlgInit_GameOver
//#include "system/savedata.h"
//#include "poketool/pokeparty.h"
//#include "poketool\status_rcv.h"

#include "field/field_msgbg.h"
#include "system/bmp_winframe.h"
//==============================================================================================
//
//	�S�Ŋ֘A
//
//==============================================================================================
//#include "system/fontproc.h"						
//#include "system/msgdata.h"							//MSGMAN_TYPE_DIRECT
//#include "system/wordset.h"							//WORDSET_Create
#include "print/wordset.h"

//#include "fld_bmp.h"						

//#include "msgdata/msg.naix"							//NARC_msg_??_dat
//#include "msgdata/msg_gameover.h"					//msg_all_dead_??
//#include "arc/
//#include "system/arc_util.h"
//#include "system/font_arc.h"
#include "font/font.naix" //NARC_font_large_gftr
#include "message.naix"
#include "msg/msg_gameover.h"

#include "arc/fieldmap/zone_id.h"

//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
/**
 * @brief   ���������^�C�v
 */
//--------------------------------------------------------------
typedef enum {
  REVIVAL_TYPE_HOME,      ///<����ŕ���
  REVIVAL_TYPE_POKECEN,   ///<�|�P�Z���ŕ���
}REVIVAL_TYPE;

//----------------------------------------------------------------------------------------------
//	�\���̐錾
//----------------------------------------------------------------------------------------------
typedef struct{
  GAMESYS_WORK * gsys;

  HEAPID heapID;
  REVIVAL_TYPE rev_type;

	GFL_MSGDATA* msgman;						//���b�Z�[�W�}�l�[�W���[
	WORDSET* wordset;								//�P��Z�b�g
	GFL_BMPWIN *bmpwin;							//BMP�E�B���h�E�f�[�^

  PRINT_QUE *printQue;
  GFL_FONT * fontHandle;
}GAME_OVER_WORK;


#define GAME_OVER_MSG_BUF_SIZE		(1024)			//���b�Z�[�W�o�b�t�@�T�C�Y
#define GAME_OVER_FADE_SYNC			(8)				//�t�F�[�hsync��

enum { //�R���p�C����ʂ����߂ɂƂ肠����

  FLD_SYSFONT_PAL = 13,

  FBMP_COL_NULL = 0,
  FBMP_COL_BLACK = 1,
  FBMP_COL_BLK_SDW = 2,
};
//----------------------------------------------------------------------------------------------
//	BMP�E�B���h�E
//----------------------------------------------------------------------------------------------
enum{
	GAME_OVER_BMPWIN_FRAME	= GFL_BG_FRAME3_M,
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
static GMEVENT * createGameOverEvent( GAMESYS_WORK * gsys, REVIVAL_TYPE rev_type );
static GMEVENT_RESULT GMEVENT_GameOver( GMEVENT * event, int*seq, void * work );
static void scr_msg_print( GAME_OVER_WORK* wk, u16 msg_id, u8 x, u8 y );


//==============================================================================================
//==============================================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setup_bg_sys( GAME_OVER_WORK * wk )
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
		GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
	};

	GFL_DISP_SetBank( &SetBankData );
	GFL_BG_SetBGMode( &BGsys_data );
	GFL_BG_SetBGControl( GAME_OVER_BMPWIN_FRAME, &header, GFL_BG_MODE_TEXT );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_systemwin_nclr, PALTYPE_MAIN_BG, 
																0x20*GAME_OVER_BMPWIN_PL, 0x20, wk->heapID );

	GFL_BG_SetBackGroundColor(GAME_OVER_BMPWIN_FRAME,0x7FFF);
}

//--------------------------------------------------------------
/**
 * @brief	�Q�[���I�[�o�[��ʌĂяo��
 *
 * @param	gsys	GAMESYS_WORK�|�C���^
 * @param event �e�C�x���g�̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static GMEVENT * createGameOverEvent( GAMESYS_WORK * gsys, REVIVAL_TYPE rev_type )
{
  GMEVENT * event;
	GAME_OVER_WORK* wk;

  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameOver, sizeof(GAME_OVER_WORK) );
  wk = GMEVENT_GetEventWork( event );
	if( wk == NULL ){
		GF_ASSERT_MSG(0, "�������m�ۂɎ��s���܂����I" );
	}
	GFL_STD_MemClear32( wk, sizeof(GAME_OVER_WORK) );
  wk->gsys = gsys;
  wk->heapID = HEAPID_PROC;
  wk->rev_type = rev_type;

  return event;
}

//--------------------------------------------------------------
/**
 * @brief	�Q�[���I�[�o�[��ʃ��C��
 *
 * @param	tcb		TCB_PTR�^
 * @param	work	���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameOver( GMEVENT * event, int*seq, void * work )
{
	GAME_OVER_WORK * wk = work;
  int trg = GFL_UI_KEY_GetTrg();
  GAMESYS_WORK * gsys = wk->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );

	switch( *seq ){
  case 0:

    WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_WHITE);
    WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_WHITE);
    WIPE_ResetWndMask(WIPE_DISP_MAIN);
    WIPE_ResetWndMask(WIPE_DISP_SUB);

    setup_bg_sys(wk);

		wk->fontHandle = GFL_FONT_Create(
			ARCID_FONT,
      NARC_font_large_gftr, //�V�t�H���gID
			GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

    wk->printQue = PRINTSYS_QUE_Create( wk->heapID );
    //���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
    wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_gameover_dat, wk->heapID);
    wk->wordset = WORDSET_Create( wk->heapID );

    //�r�b�g�}�b�v�ǉ�
    wk->bmpwin = GFL_BMPWIN_Create(
      GAME_OVER_BMPWIN_FRAME,						//�E�C���h�E�g�p�t���[��
      GAME_OVER_BMPWIN_PX1,GAME_OVER_BMPWIN_PY1,	//�E�C���h�E�̈�̍����X,Y���W�i�L�����P�ʂŎw��j
      GAME_OVER_BMPWIN_SX, GAME_OVER_BMPWIN_SY,	//�E�C���h�E�̈��X,Y�T�C�Y�i�L�����P�ʂŎw��j
      GAME_OVER_BMPWIN_PL,						//�E�C���h�E�̈�̃p���b�g�i���o�[	
      GAME_OVER_BMPWIN_CH							//�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
    );

    //��l�����Z�b�g
    WORDSET_RegisterPlayerName( wk->wordset, 0, GAMEDATA_GetMyStatus(gamedata) );
    {
      if ( wk->rev_type == REVIVAL_TYPE_HOME )
      { //����ɖ߂�Ƃ�
        scr_msg_print( wk, msg_all_dead_05, 0, 0 );
      }
      else
      { //�|�P�Z���ɖ߂�Ƃ�
        scr_msg_print( wk, msg_all_dead_04, 0, 0 );
      }
    }

    GFL_BMPWIN_MakeTransWindow( wk->bmpwin );

    (*seq) ++;
    break;

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

	//���C����ʃu���b�N�A�E�g�҂�
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
		GFL_HEAP_FreeMemory( wk );

		return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
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
static void scr_msg_print( GAME_OVER_WORK* wk, u16 msg_id, u8 x, u8 y )
{
  PRINTSYS_LSB lsb = PRINTSYS_LSB_Make(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL);
	STRBUF* tmp_buf = GFL_STR_CreateBuffer( GAME_OVER_MSG_BUF_SIZE, wk->heapID );
	STRBUF* tmp_buf2= GFL_STR_CreateBuffer( GAME_OVER_MSG_BUF_SIZE, wk->heapID );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpwin), FBMP_COL_NULL );			//�h��Ԃ�

	GFL_MSG_GetString( wk->msgman, msg_id, tmp_buf );

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
//-----------------------------------------------------------------------------
/**
 * @todo  �Ƃ肠�����G���[����̂��߂ɍ쐬�����֐�
 */
//-----------------------------------------------------------------------------
static u16 get_warp_id( GAMEDATA * gamedata )
{
  return GAMEDATA_GetWarpID( gamedata );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static REVIVAL_TYPE getRevType( GAMEDATA * gamedata )
{
  if(	WARPDATA_GetInitializeID() == get_warp_id(gamedata) ) {
    return REVIVAL_TYPE_HOME;
  } else {
    return REVIVAL_TYPE_POKECEN;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief	�ʏ�퓬�F�S�ŃV�[�P���X
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE		�C�x���g�I��
 * @retval	FALSE		�C�x���g�p����
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_NormalGameOver(GMEVENT * event, int * seq, void *work)
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork( event );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );

	switch (*seq) {
	case 0:
		//���[�vID�Ŏw�肳�ꂽ�߂���
    //���̒��ł̓t�B�[���h���A�����͓��삵�Ȃ��I
    MAPCHG_GameOver( gsys );
		(*seq) ++;
		break;

	case 1:
		//BGM�t�F�[�h�A�E�g
		//Snd_BgmFadeOut( 0, 20 );
		(*seq) ++;
		break;

	case 2:
		//BGM�t�F�[�h�A�E�g�҂�
		/*if( Snd_FadeCheck() == 0 ) */{

			//�T�E���h�Q�[���I�[�o�[����
			//Snd_GameOverSet();

			(*seq) ++;
		}
		break;

	case 3:
		//�x��BG�ȊO��\���I�t
		SetBrightness( BRIGHTNESS_BLACK, (PLANEMASK_ALL^PLANEMASK_BG3), MASK_MAIN_DISPLAY);
		SetBrightness( BRIGHTNESS_BLACK, PLANEMASK_ALL, MASK_SUB_DISPLAY);

		//�Q�[���I�[�o�[�x��
    //GMEVENT_CallEvent( event, createGameOverEvent( gsys, getRevType(gamedata) ) );
		(*seq) ++;
		break;

	case 4:
		//�C�x���g�R�}���h�F�t�B�[���h�}�b�v�v���Z�X���A
    GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
		(*seq)++;
		break;

	case 5:
		//�\���I�t����
		SetBrightness( BRIGHTNESS_NORMAL, PLANEMASK_ALL, MASK_DOUBLE_DISPLAY);

		//�C�����Ă˃X�N���v�g
		
		//�b�������Ώۂ�OBJ���擾���鏈�����K�v�ɂȂ�
		//OS_Printf( "field_encount zone_id = %d\n", fsys->location->zone_id );
    if ( getRevType( gamedata ) == REVIVAL_TYPE_HOME ) {
			//�����l�̃��[�vID�����ŏ��̖߂��Ȃ̂Ŏ����̉�
			SCRIPT_CallScript( event, SCRID_GAMEOVER_RECOVER_HOME, NULL, NULL, HEAPID_FIELDMAP );
		}else{
			//����ȊO�����|�P�Z���̂͂�
			SCRIPT_CallScript( event, SCRID_GAMEOVER_RECOVER_POKECEN, NULL, NULL, HEAPID_FIELDMAP );
		}
		(*seq) ++;
		break;

	case 6:
		//�T�E���h���X�^�[�g����(06/07/10����Ȃ��̂ō폜)
		//Snd_RestartSet( fsys );

		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g�R�}���h�F�ʏ�S�ŏ���
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_NormalLose( GAMESYS_WORK * gsys )
{
	GMEVENT * event;
  event = GMEVENT_Create(gsys, NULL, GMEVENT_NormalGameOver, 0);
  return event;
}


