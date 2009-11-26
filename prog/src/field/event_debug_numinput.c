/*
 *  @file   event_debug_numinput.c
 *  @brief  �f�o�b�O���l����
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"
#include "font/font.naix"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "sound/pm_sndsys.h"

#include "field_sound.h"
#include "eventwork.h"

#include "event_debug_local.h"
#include "event_debug_numinput.h"

#include "message.naix"
#include "msg/msg_d_numinput.h"


///���ڑI��p�f�[�^�̒�`
typedef enum {
  D_NINPUT_DATA_CONST,    ///<�Œ�f�[�^
  D_NINPUT_DATA_ALLOC,    ///<���������f�[�^
}D_NINPUT_DATA_TYPE;

//���l���̓��C���C�x���g���䃏�[�N
typedef struct _DNINPUT_EV_WORK{
  HEAPID  heapID;

  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  FIELDMAP_WORK* fieldWork;

  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DNINPUT_EV_WORK;


///���l���̓��X�g�������p�����[�^��`
typedef struct{ 
  D_NINPUT_DATA_TYPE data_type;       ///<�f�[�^�̎��
  u32   file_id;                      ///<D_NINPUT_DATA_ALLOC�̏ꍇ�̎Q�ƃt�@�C��ID
#if 0
  /** D_NINPUT_DATA_CONST�̏ꍇ�F�e���ڂ̐��l���͎������w��f�[�^
   *  D_NINPUT_DATA_ALLOC�̏ꍇ�F�������Ɏg�p����f�t�H���g�f�[�^
   */
  const DEBUG_NUMINPUT_LIST * pList; 
  const GMM_MENU_PARAM * pMenu;       ///<���ڃ��X�g�\���p�f�[�^
#else
  void* list;
  void* menu;
#endif
  const u32 list_count;               ///<���X�g�̑傫��

}DEBUG_NUMINPUT_INITIALIZER;


/////////////////////////////////////////////////////////
//�v���g�^�C�v
/////////////////////////////////////////////////////////
static GMEVENT_RESULT dninput_MainEvent( GMEVENT * event, int *seq, void * work);


/////////////////////////////////////////////////////////
//�f�[�^��`
/////////////////////////////////////////////////////////
static const DEBUG_NUMINPUT_INITIALIZER DATA_Initializer[D_NUMINPUT_MODE_MAX] = { 
  { D_NINPUT_DATA_CONST,  0,
    NULL /*Debug_NumInputL_Tower*/,
    NULL /*Debug_NumInputG_Tower*/, 0/*NELEMS(Debug_NumInputG_Tower)*/ },
  { D_NINPUT_DATA_CONST,  0,
    NULL /*Debug_NumInputL_Tower*/,
    NULL /*Debug_NumInputG_Tower*/, 0/*NELEMS(Debug_NumInputG_Tower)*/ },
};

/// ���l���́@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DNumInput_MenuFuncHeader =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

static const FLDMENUFUNC_LIST DATA_DNumInputMenu[D_NUMINPUT_MODE_MAX] =
{
  { dni_top_effect_encount, (void*)D_NUMINPUT_ENCEFF },
  { dni_top_scenario, (void*)D_NUMINPUT_DUMMY },
};

static const DEBUG_MENU_INITIALIZER DATA_DNumInput_MenuInitializer = {
  NARC_message_d_numinput_dat,  //���b�Z�[�W�A�[�J�C�u
  NELEMS(DATA_DNumInputMenu),  //���ڐ�max
  DATA_DNumInputMenu, //���j���[���ڃ��X�g
  &DATA_DNumInput_MenuFuncHeader, //���j���w�b�_
  1, 4, 30, 16,
  NULL/*DEBUG_SetMenuWorkZoneID_SelectZone*/,
  NULL,
};

/////////////////////////////////////////////////////////
//�֐���`
/////////////////////////////////////////////////////////
//--------------------------------------------------------------
/**
 * ���l���̓f�o�b�O���j���[�C�x���g����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
GMEVENT* EVENT_DMenuNumInput( GAMESYS_WORK* gsys, void* work )
{
  DEBUG_MENU_EVENT_WORK* dme_wk = (DEBUG_MENU_EVENT_WORK*)work;
  DNINPUT_EV_WORK* wk;
  GMEVENT* event;

  event = GMEVENT_Create( gsys, NULL, dninput_MainEvent, sizeof(DNINPUT_EV_WORK) );

  wk = GMEVENT_GetEventWork( event );
  wk->gsys = gsys; 
  wk->gdata = dme_wk->gdata; 
  wk->fieldWork = dme_wk->fieldWork;
  wk->heapID = dme_wk->heapID;

  return event;
}

/*
 *  @brief  ���l���̓��C�����j���[�C�x���g
 */
static GMEVENT_RESULT dninput_MainEvent( GMEVENT * event, int *seq, void * work)
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
  DNINPUT_EV_WORK* wk = (DNINPUT_EV_WORK*)work;

  switch(*seq)
  {
  case 0:
    wk->menuFunc = DebugMenuInit( wk->fieldWork, wk->heapID, &DATA_DNumInput_MenuInitializer );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){
        break;
      }
      FLDMENUFUNC_DeleteMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){
        return GMEVENT_RES_FINISH;
      }
      (*seq)++;
    }
    break;
  default:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

