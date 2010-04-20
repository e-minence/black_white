/*
 *  @file   event_freeword_input.c
 *  @brief  ����Ⴂ�֘A�t���[���[�h���̓C�x���g
 *  @author Miyuki Iwasawa
 *  @date   10.03.25
 *
 *  NameIn�A�v�����Ăяo���A���ʂ����[�N�ɔ��f����܂ł��s��
 */

#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "fieldmap.h"
#include "event_fieldmap_control.h"
#include "sound/pm_sndsys.h"

#include "gamesystem/game_beacon.h"
#include "gamesystem/beacon_status.h"
#include "savedata/misc.h"

#include "event_freeword_input.h"

typedef struct _EVWORK_FWORD_INPUT{
  GAMESYS_WORK* gsys;
  GAMEDATA*     gdata;
  MISC*         misc;
  MYSTATUS*     my;
  FIELDMAP_WORK* fieldmap;
  BEACON_STATUS* b_status;

  u16*          ret_wk;   ///<���͂��m�肵�����ǂ�����Ԃ����[�N
  NAMEIN_PARAM  param;
 
  HEAPID  heapID;
  int mode;
  
}EVWORK_FWORD_INPUT;

static const u8 DATA_WordLength[] = {
 NAMEIN_GREETING_WORD_LENGTH,
 NAMEIN_THANKS_WORD_LENGTH,
 NAMEIN_FREE_WORD_LENGTH,
};

//==========================================================
///�v���g�^�C�v
//==========================================================
static GMEVENT_RESULT FreeWordInputEvent(GMEVENT * event, int * seq, void *work);
static void sub_ResultGet( EVWORK_FWORD_INPUT* wk, NAMEIN_PARAM* param );

//-------------------------------------------------------------------
/*
 *  @brief  �t���[���[�h���͔ėp�C�x���g����
 */
//-------------------------------------------------------------------
GMEVENT* EVENT_FreeWordInput( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, GMEVENT* parent, int mode, u16* ret_wk )
{
 	GMEVENT* event = GMEVENT_Create(gsys, parent, FreeWordInputEvent, sizeof(EVWORK_FWORD_INPUT));
	EVWORK_FWORD_INPUT *  wk = GMEVENT_GetEventWork(event);

  wk->gsys = gsys;
  wk->gdata = GAMESYSTEM_GetGameData( gsys );
  wk->fieldmap = fieldmap;
  wk->ret_wk = ret_wk;

  wk->mode = mode;
  wk->heapID = HEAPID_PROC;

  wk->misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork( wk->gdata ) );
  wk->my = GAMEDATA_GetMyStatus( wk->gdata );
  wk->b_status = GAMEDATA_GetBeaconStatus( wk->gdata );

  //���O���̓p�����[�^������
  wk->param.mode = mode;
  wk->param.hero_sex = MyStatus_GetMySex( wk->my );
  wk->param.p_misc = wk->misc;
  
  GF_ASSERT( mode >= NAMEIN_GREETING_WORD && mode <= NAMEIN_FREE_WORD );

  wk->param.wordmax = DATA_WordLength[ mode - NAMEIN_GREETING_WORD ];
  wk->param.strbuf = GFL_STR_CreateBuffer( wk->param.wordmax+EOM_SIZE, wk->heapID );

  switch( mode ){
  case NAMEIN_GREETING_WORD: // ���ꂿ�������A���t param1�����̐���
    GFL_STR_SetStringCode( wk->param.strbuf,  MISC_CrossComm_GetSelfIntroduction( wk->misc ));
    break;
  case NAMEIN_THANKS_WORD:   // ���ꂿ�������猾�t param1�����̐���
    GFL_STR_SetStringCode( wk->param.strbuf, MISC_CrossComm_GetThankyouMessage( wk->misc ) );
    break;
  case NAMEIN_FREE_WORD:  //����Ⴂ�t���[���[�h
    //�����͋󕶎�
    break;
  default:
    GF_ASSERT(0);
  }
  return event;
}

static GMEVENT_RESULT FreeWordInputEvent(GMEVENT * event, int * seq, void *work)
{
	EVWORK_FWORD_INPUT * wk = work;
	
  switch (*seq) {
  case 0:
    {
      GMEVENT* child = 
        EVENT_FieldSubProc( wk->gsys, wk->fieldmap,
          FS_OVERLAY_ID(namein), &NameInputProcData, &wk->param );
      GMEVENT_CallEvent( event, child );
    }
    (*seq)++;
    break;
  case 1:
    //���ʔ��f
    sub_ResultGet( wk, &wk->param );

    //��n��
    GFL_STR_DeleteBuffer( wk->param.strbuf );
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------
/*
 *  @brief  ���ʔ��f
 */
//-----------------------------------------------------------
static void sub_ResultGet( EVWORK_FWORD_INPUT* wk, NAMEIN_PARAM* param )
{
  if( wk->ret_wk != NULL ){
    *wk->ret_wk = ( param->cancel == FALSE );
  }
  if( param->cancel == TRUE ){  //�L�����Z�����ꂽ
    return;
  }
  switch( wk->mode ){
  case NAMEIN_GREETING_WORD: // ���ꂿ�������A���t
    MISC_CrossComm_SetSelfIntroduction( wk->misc, param->strbuf );
    GAMEBEACON_SendDataUpdate_SelfIntroduction();
    break;
  case NAMEIN_THANKS_WORD:   // ���ꂿ�������猾�t
    MISC_CrossComm_SetThankyouMessage( wk->misc, param->strbuf );
    break;
  case NAMEIN_FREE_WORD:  //����Ⴂ�t���[���[�h
    GFL_STR_CopyBuffer( BEACON_STATUS_GetFreeWordBuffer( wk->b_status ), wk->param.strbuf );
    GAMEBEACON_Set_FreeWord( wk->gdata, param->strbuf );
    break;
  default:
    GF_ASSERT(0);
  }
}

