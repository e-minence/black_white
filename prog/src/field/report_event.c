//============================================================================================
/**
 * @file		report_event.h
 * @brief		�t�B�[���h���j���[�E���|�[�g����
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
#include "savedata/playtime.h"
#include "sound/pm_sndsys.h"
#include "font/font.naix"

#include "message.naix"
#include "msg/msg_report.h"

#include "script_message.naix"
#include "msg/script/msg_common_scr.h"

#include "fieldmap.h"
#include "field_subscreen.h"

#include "report_event.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���V�[�P���X
enum {
	REPORT_SEQ_SUBDISP_SET = 0,					// �T�u��ʃZ�b�g
	REPORT_SEQ_INIT_WAIT,								// �T�u��ʏ������҂�
	REPORT_SEQ_INIT_MESSAGE,						// �������b�Z�[�W
	REPORT_SEQ_WRITE_YESNO_SET,					// �Z�[�u���܂����H�y�͂��E�������z�Z�b�g
	REPORT_SEQ_WRITE_YESNO_WAIT,				// �Z�[�u���܂����H�y�͂��E�������z�҂�
	REPORT_SEQ_OVERWRITE_YESNO_SET,			// �㏑�����܂����H�y�͂��E�������z�Z�b�g
	REPORT_SEQ_OVERWRITE_YESNO_WAIT,		// �㏑�����܂����H�y�͂��E�������z�҂�
	REPORT_SEQ_SAVE_INIT,								// �Z�[�u�����ݒ�
	REPORT_SEQ_SAVE_MAIN,								// �Z�[�u���s
	REPORT_SEQ_RESULT_OK_WAIT,					// �Z�[�u�������b�Z�[�W�҂�
	REPORT_SEQ_RESULT_SE_WAIT,					// �r�d�҂�
	REPORT_SEQ_RESULT_NG_WAIT,					// �Z�[�u���s���b�Z�[�W�҂�
	REPORT_SEQ_END_TRG_WAIT,						// �Z�[�u��̃L�[�҂�
	REPORT_SEQ_SUBDISP_RESET,						// �T�u��ʃ��Z�b�g
	REPORT_SEQ_END,											// �I��
};

#define	REPORT_STR_LEN		( 512 )		// ������o�b�t�@�̒���

#define	BGPAL_MSGFNT	( 12 )				// �p���b�g�F���b�Z�[�W�t�H���g
#define	BGPAL_WINFRM	( 13 )				// �p���b�g�F���b�Z�[�W�E�B���h�E
#define	BGPAL_YESNO		( 14 )				// �p���b�g�F�͂��E������

#define	WINFRM_CGXNUM		( 1 )				// ���b�Z�[�W�E�B���h�E�L�����ǂݍ��݈ʒu

// ���b�Z�[�W�E�B���h�E�f�[�^
#define	MSG_PX		( 1 )
#define	MSG_PY		( 1 )
#define	MSG_SX		( 30 )
#define	MSG_SY		( 4 )

//�u�͂��E�������v�f�[�^
#define	YESNO_PX	( 32 )
#define	YESNO_PY	( 12 )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void InitReportBmpWin( FMENU_REPORT_EVENT_WORK * wk );
static void ExitReportBmpWin( FMENU_REPORT_EVENT_WORK * wk );
static void SetReportMsg( FMENU_REPORT_EVENT_WORK * wk, u32 strIdx );
static void SetReportMsgBuff( FMENU_REPORT_EVENT_WORK * wk );
static BOOL MainReportMsg( FMENU_REPORT_EVENT_WORK * wk );
static void SetReportPlayerAnime( FMENU_REPORT_EVENT_WORK * work );
static void ResetReportPlayerAnime( FMENU_REPORT_EVENT_WORK * work );
static void InitReportYesNo( FMENU_REPORT_EVENT_WORK * wk );
static void ExitReportYesNo( FMENU_REPORT_EVENT_WORK * wk );
static void SetReportYesNo( FMENU_REPORT_EVENT_WORK * wk );
static s32 MainReportYesNo( FMENU_REPORT_EVENT_WORK * wk );


//--------------------------------------------------------------------------------------------
/**
 * ���C������
 *
 * @param		wk		���[�N
 * @param		seq		�V�[�P���X
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
BOOL REPORTEVENT_Main( FMENU_REPORT_EVENT_WORK * wk, int * seq )
{
	switch( (*seq) ){
	case REPORT_SEQ_SUBDISP_SET:						// �T�u��ʃZ�b�g
		FIELD_SUBSCREEN_Change( FIELDMAP_GetFieldSubscreenWork(wk->fieldWork), FIELD_SUBSCREEN_REPORT );
		*seq = REPORT_SEQ_INIT_WAIT;
		break;

	case REPORT_SEQ_INIT_WAIT:							// �T�u��ʏ������҂�
		if( FIELD_SUBSCREEN_CanChange(FIELDMAP_GetFieldSubscreenWork(wk->fieldWork)) == TRUE ){
			wk->msgData = GFL_MSG_Create(
												GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE,
												NARC_script_message_common_scr_dat, wk->heapID );
			wk->strBuff = GFL_STR_CreateBuffer( REPORT_STR_LEN, wk->heapID );
			wk->tcbl = GFL_TCBL_Init( wk->heapID, wk->heapID, 1, 4 );
			InitReportBmpWin( wk );
			InitReportYesNo( wk );
			*seq = REPORT_SEQ_INIT_MESSAGE;
		}
		break;

	case REPORT_SEQ_INIT_MESSAGE:						// �������b�Z�[�W
		SetReportMsg( wk, msg_common_report_01 );
		*seq = REPORT_SEQ_WRITE_YESNO_SET;
		break;

	case REPORT_SEQ_WRITE_YESNO_SET:				// �Z�[�u���܂����H�y�͂��E�������z�Z�b�g
		if( MainReportMsg( wk ) == FALSE ){
			SetReportYesNo( wk );
			*seq = REPORT_SEQ_WRITE_YESNO_WAIT;
		}
		break;

	case REPORT_SEQ_WRITE_YESNO_WAIT:				// �Z�[�u���܂����H�y�͂��E�������z�҂�
		switch( MainReportYesNo(wk) ){
		case 0:	// �͂�
			// �ʐM���ŃZ�[�u�ł��Ȃ�
      if( GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(wk->gsys)) == GAME_COMM_NO_INVASION ){
				SetReportMsg( wk, msg_common_report_13 );
				*seq = REPORT_SEQ_RESULT_NG_WAIT;
			// �Ⴄ�Z�[�u�f�[�^������
			}else if( SaveControl_IsOverwritingOtherData(wk->sv) == TRUE ){
				SetReportMsg( wk, msg_common_report_08 );
				*seq = REPORT_SEQ_RESULT_NG_WAIT;
			// �Z�[�u�f�[�^������
			}else if( SaveData_GetExistFlag(wk->sv) == TRUE ){
				SetReportMsg( wk, msg_common_report_02 );
				*seq = REPORT_SEQ_OVERWRITE_YESNO_SET;
			// �Z�[�u
			}else{
				SetReportMsg( wk, msg_common_report_03 );
				*seq = REPORT_SEQ_SAVE_INIT;
			}
			break;

		case 1:	// ������
			*seq = REPORT_SEQ_SUBDISP_RESET;
			break;
		}
		break;

	case REPORT_SEQ_OVERWRITE_YESNO_SET:		// �㏑�����܂����H�y�͂��E�������z�Z�b�g
		if( MainReportMsg( wk ) == FALSE ){
			SetReportYesNo( wk );
			*seq = REPORT_SEQ_OVERWRITE_YESNO_WAIT;
		}
		break;

	case REPORT_SEQ_OVERWRITE_YESNO_WAIT:		// �㏑�����܂����H�y�͂��E�������z�҂�
		switch( MainReportYesNo(wk) ){
		case 0:	// �͂�
			SetReportMsg( wk, msg_common_report_03 );
			*seq = REPORT_SEQ_SAVE_INIT;
			break;

		case 1:	// ������
			*seq = REPORT_SEQ_SUBDISP_RESET;
			break;
		}
		break;

	case REPORT_SEQ_SAVE_INIT:							// �Z�[�u�����ݒ�
		if( MainReportMsg( wk ) == FALSE ){
			FIELD_SUBSCREEN_SetReportStart( FIELDMAP_GetFieldSubscreenWork(wk->fieldWork) );
			SetReportPlayerAnime( wk );
			PLAYTIME_SetSaveTime( SaveData_GetPlayTime(wk->sv) );
			GAMEDATA_SaveAsyncStart( GAMESYSTEM_GetGameData(wk->gsys) );
			*seq = REPORT_SEQ_SAVE_MAIN;
		}
		break;

	case REPORT_SEQ_SAVE_MAIN:							// �Z�[�u���s
		switch( GAMEDATA_SaveAsyncMain(GAMESYSTEM_GetGameData(wk->gsys)) ){
		case SAVE_RESULT_CONTINUE:
		case SAVE_RESULT_LAST:
			break;
      
		case SAVE_RESULT_OK:
			FIELD_SUBSCREEN_SetReportEnd( FIELDMAP_GetFieldSubscreenWork(wk->fieldWork) );
			ResetReportPlayerAnime( wk );
			{
				WORDSET * wset;
				STRBUF * str;
				wset = WORDSET_Create( wk->heapID );
				str  = GFL_MSG_CreateString( wk->msgData, msg_common_report_04 );
				WORDSET_RegisterPlayerName( wset, 0, GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(wk->gsys)) );
				WORDSET_ExpandStr( wset, wk->strBuff, str );
				GFL_STR_DeleteBuffer( str );
				WORDSET_Delete( wset );
			}
			SetReportMsgBuff( wk );
			*seq = REPORT_SEQ_RESULT_OK_WAIT;
			break;

		case SAVE_RESULT_NG:
			FIELD_SUBSCREEN_SetReportEnd( FIELDMAP_GetFieldSubscreenWork(wk->fieldWork) );
			ResetReportPlayerAnime( wk );
			SetReportMsg( wk, msg_common_report_06 );
			*seq = REPORT_SEQ_RESULT_NG_WAIT;
			break;
		}
		break;

	case REPORT_SEQ_RESULT_OK_WAIT:		// �Z�[�u�������b�Z�[�W�҂�
		if( MainReportMsg( wk ) == FALSE ){
			PMSND_PlaySE( SEQ_SE_SAVE );
			*seq = REPORT_SEQ_RESULT_SE_WAIT;
		}
		break;

	case REPORT_SEQ_RESULT_SE_WAIT:		// �r�d�҂�
		if( PMSND_CheckPlayingSEIdx(SEQ_SE_SAVE) == FALSE ){
			*seq = REPORT_SEQ_END_TRG_WAIT;
		}
		break;

	case REPORT_SEQ_RESULT_NG_WAIT:		// �Z�[�u���s���b�Z�[�W�҂�
		if( MainReportMsg( wk ) == FALSE ){
			*seq = REPORT_SEQ_END_TRG_WAIT;
		}
		break;

	case REPORT_SEQ_END_TRG_WAIT:			// �Z�[�u��̃L�[�҂�
		if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
			*seq = REPORT_SEQ_SUBDISP_RESET;
		}
		break;

	case REPORT_SEQ_SUBDISP_RESET:					// �T�u��ʃ��Z�b�g
		ExitReportYesNo( wk );
		ExitReportBmpWin( wk );
	  GFL_TCBL_Exit( wk->tcbl );
		GFL_STR_DeleteBuffer( wk->strBuff );
    GFL_MSG_Delete( wk->msgData );
		FIELD_SUBSCREEN_Change( FIELDMAP_GetFieldSubscreenWork(wk->fieldWork), FIELD_SUBSCREEN_TOPMENU );
		*seq = REPORT_SEQ_END;
		break;

	case REPORT_SEQ_END:										// �I��
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ����ʉ�b�E�B���h�E�쐬
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitReportBmpWin( FMENU_REPORT_EVENT_WORK * wk )
{
	wk->win = GFL_BMPWIN_Create(
							GFL_BG_FRAME2_S, MSG_PX, MSG_PY, MSG_SX, MSG_SY, BGPAL_MSGFNT, GFL_BMP_CHRAREA_GET_B );

	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME2_S, WINFRM_CGXNUM, BGPAL_WINFRM, MENU_TYPE_SYSTEM, wk->heapID );

	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, BGPAL_MSGFNT*0x20, 0x20, wk->heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * ����ʉ�b�E�B���h�E�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitReportBmpWin( FMENU_REPORT_EVENT_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->win );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�`��
 *
 * @param		wk				���[�N
 * @param		strIdx		������C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetReportMsg( FMENU_REPORT_EVENT_WORK * wk, u32 strIdx )
{
  GFL_MSG_GetString( wk->msgData, strIdx, wk->strBuff );
	SetReportMsgBuff( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�`��i�o�b�t�@�j
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetReportMsgBuff( FMENU_REPORT_EVENT_WORK * wk )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), 15 );
	BmpWinFrame_Write( wk->win, WINDOW_TRANS_OFF, WINFRM_CGXNUM, BGPAL_WINFRM );

	wk->stream = PRINTSYS_PrintStream(
									wk->win,
									0, 0, wk->strBuff,
									FLDMSGBG_GetFontHandle(wk->msgBG),
									MSGSPEED_GetWait(),
									wk->tcbl,
									10,		// tcbl pri
									wk->heapID,
									15 );	// clear color
	GFL_BMPWIN_MakeTransWindow_VBlank( wk->win );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\�����C��
 *
 * @param		wk			���[�N
 *
 * @retval	"TRUE = �\����"
 * @retval	"FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainReportMsg( FMENU_REPORT_EVENT_WORK * wk )
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

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�͂��E�������v������
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitReportYesNo( FMENU_REPORT_EVENT_WORK * wk )
{
	GFL_MSGDATA * mman = GFL_MSG_Create(
													GFL_MSG_LOAD_NORMAL,
													ARCID_MESSAGE, NARC_message_report_dat, wk->heapID );

	wk->ynList[0].str      = GFL_MSG_CreateString( mman, REPORT_STR_14 );
  wk->ynList[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->ynList[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
	wk->ynList[1].str      = GFL_MSG_CreateString( mman, REPORT_STR_15 );
  wk->ynList[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->ynList[1].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

	GFL_MSG_Delete( mman );

	wk->ynRes = APP_TASKMENU_RES_Create(
								GFL_BG_FRAME2_S, BGPAL_YESNO,
								FLDMSGBG_GetFontHandle( wk->msgBG ),
								FLDMSGBG_GetPrintQue( wk->msgBG ),
								wk->heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�͂��E�������v�폜
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitReportYesNo( FMENU_REPORT_EVENT_WORK * wk )
{
	APP_TASKMENU_RES_Delete( wk->ynRes );

	GFL_STR_DeleteBuffer( wk->ynList[1].str );
	GFL_STR_DeleteBuffer( wk->ynList[0].str );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�͂��E�������v�J�n
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetReportYesNo( FMENU_REPORT_EVENT_WORK * wk )
{
	APP_TASKMENU_INITWORK	mwk;

  mwk.heapId   = wk->heapID;
  mwk.itemNum  = 2;
  mwk.itemWork = wk->ynList;
  mwk.posType  = ATPT_RIGHT_DOWN;
  mwk.charPosX = YESNO_PX;
  mwk.charPosY = YESNO_PY;
	mwk.w        = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
	mwk.h        = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

	wk->ynWork = APP_TASKMENU_OpenMenu( &mwk, wk->ynRes );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�͂��E�������v���C��
 *
 * @param		wk			���[�N
 *
 * @retval	"0 = �͂�"
 * @retval	"1 = ������"
 * @retval	"-1 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static s32 MainReportYesNo( FMENU_REPORT_EVENT_WORK * wk )
{
	APP_TASKMENU_UpdateMenu( wk->ynWork );
	if( APP_TASKMENU_IsFinish( wk->ynWork ) == TRUE ){
		APP_TASKMENU_CloseMenu( wk->ynWork );
		if( APP_TASKMENU_GetCursorPos( wk->ynWork ) == 0 ){
			return 0;
		}else{
			return 1;
		}
	}

	return -1;
}


// ���@���|�[�g��
static void SetReportPlayerAnime( FMENU_REPORT_EVENT_WORK * wk )
{
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );
	if( FIELD_PLAYER_CheckChangeEventDrawForm(fld_player) == TRUE ){
		MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
		FIELD_PLAYER_SetRequest( fld_player, FIELD_PLAYER_REQBIT_REPORT );
		FIELD_PLAYER_UpdateRequest( fld_player );
		//�|�[�Y���������A�j������悤��
		MMDL_OffMoveBitMoveProcPause( mmdl );
		MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_ANM );
	}
}

// ���@���ɖ߂�
static void ResetReportPlayerAnime( FMENU_REPORT_EVENT_WORK * wk )
{
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );
	if( FIELD_PLAYER_CheckChangeEventDrawForm(fld_player) == TRUE ){
		FIELD_PLAYER_SetRequest( fld_player, FIELD_PLAYER_REQBIT_MOVE_FORM_TO_DRAW_FORM );
		FIELD_PLAYER_UpdateRequest( fld_player );
	}
}
