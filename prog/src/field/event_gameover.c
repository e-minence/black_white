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
#include "event_gamestart.h"
#include "event_mapchange.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h" //SCRID_�`
#include "../../../resource/fldmapdata/script/t01r0101_def.h" //SCRID_�`

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

#include "pleasure_boat.h"

//==============================================================================================
//==============================================================================================


//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static REVIVAL_TYPE getRevType( GAMEDATA * gamedata )
{
  if(	WARPDATA_GetInitializeID() == GAMEDATA_GetWarpID(gamedata) ) {
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
 * @todo
 * �|�P�Z���ł̕����������ȑO�̎d�l�ō�邽�߂ɂ̓W���[�C����OBJ��
 * OBJID�����炩�̕��@�Ŏ擾���A�A�j���[�V����������K�v������
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_NormalGameOver(GMEVENT * event, int * seq, void *work)
{
  GAMEOVER_WORK * param = work;

	switch (*seq) {
	case 0:
		//�x��BG�ȊO��\���I�t
		SetBrightness( BRIGHTNESS_BLACK, (PLANEMASK_ALL^PLANEMASK_BG3), MASK_MAIN_DISPLAY);
		SetBrightness( BRIGHTNESS_BLACK, PLANEMASK_ALL, MASK_SUB_DISPLAY);

		//�Q�[���I�[�o�[�x��
    GMEVENT_CallProc( event, FS_OVERLAY_ID(gameclear_demo), &GameOverMsgProcData, param );
		(*seq) ++;
		break;

	case 1:
		//�C�x���g�R�}���h�F�t�B�[���h�}�b�v�v���Z�X���A
    GMEVENT_CallEvent( event, EVENT_GameOver(param->gsys) );
		(*seq)++;
		break;

	case 2:
		//�\���I�t����
		SetBrightness( BRIGHTNESS_NORMAL, PLANEMASK_ALL, MASK_DOUBLE_DISPLAY);

		//�C�����Ă˃X�N���v�g
		
		//�b�������Ώۂ�OBJ���擾���鏈�����K�v�ɂȂ�
		//OS_Printf( "field_encount zone_id = %d\n", fsys->location->zone_id );
    if ( param->rev_type == REVIVAL_TYPE_HOME ) {
			//�����l�̃��[�vID�����ŏ��̖߂��Ȃ̂Ŏ����̉�
			SCRIPT_CallScript( event, SCRID_T01R0101_GAMEOVER_RECOVER, NULL, NULL, HEAPID_FIELDMAP );
		}else{
			//����ȊO�����|�P�Z���̂͂�
			SCRIPT_CallScript( event, SCRID_GAMEOVER_RECOVER_POKECEN, NULL, NULL, HEAPID_FIELDMAP );
		}
		(*seq) ++;
		break;

	case 3:
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
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
	GMEVENT * event;
  GAMEOVER_WORK * param;
  event = GMEVENT_Create( gsys, NULL, GMEVENT_NormalGameOver, sizeof(GAMEOVER_WORK) );
  param = GMEVENT_GetEventWork( event );
  param->gsys = gsys;
  param->rev_type = getRevType( gamedata );
  param->mystatus = GAMEDATA_GetMyStatus( gamedata );

  //�V���D���[�N���
  {
    PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);
    PL_BOAT_End(ptr);
  }
  return event;
}


