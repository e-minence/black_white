////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �G�ߕ\���I���C�x���g
 * @file   event_debug_season_disyplay.c
 * @author obata
 * @date   2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "gamesystem/pm_season.h"
#include "gamesystem/gamesystem.h"
#include "fieldmap.h"
#include "event_debug_season_display.h"
#include "message.naix"  // NARC_xxxx
#include "msg/msg_d_field.h"  // for DEBUG_FIELD_STR_xxxx
#include "event_debug_local.h"  // for DEBUG_MENU_INITIALIZER
#include "event_season_display.h" // for EVENT_SeasonDisplay


/// �ǂ��ł��W�����v�@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
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

static const FLDMENUFUNC_LIST DATA_SeasonMenuList[PMSEASON_TOTAL] =
{
  { DEBUG_FIELD_STR_SPRING, (void*)PMSEASON_SPRING },
  { DEBUG_FIELD_STR_SUMMER, (void*)PMSEASON_SUMMER },
  { DEBUG_FIELD_STR_AUTUMN, (void*)PMSEASON_AUTUMN },
  { DEBUG_FIELD_STR_WINTER, (void*)PMSEASON_WINTER },
};

static const DEBUG_MENU_INITIALIZER DebugSeasonJumpMenuData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SeasonMenuList),
  DATA_SeasonMenuList,
  &DATA_DebugMenuList_ZoneSel, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};


//======================================================================================
// ���l�G�\���C�x���g���[�N
//======================================================================================
typedef struct
{
  HEAPID           heapID;
  GAMESYS_WORK*      gsys;
  GAMEDATA*         gdata;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;

} EVENT_WORK;


//======================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//======================================================================================
static GMEVENT_RESULT debugMenuSeasonSelectEvent( GMEVENT *event, int *seq, void *wk );


//======================================================================================
// ���O�����J�֐�
//======================================================================================

//--------------------------------------------------------------------------------------
/**
 * @brief �l�G�\���I���C�x���g���쐬����
 */
//--------------------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_SeasonDispSelect( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, NULL, debugMenuSeasonSelectEvent, sizeof(EVENT_WORK) );
   // ������
  work = GMEVENT_GetEventWork( event );
  work->heapID   = heapID;
  work->gsys     = gsys;
  work->gdata    = GAMESYSTEM_GetGameData( gsys );
  work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->menuFunc = NULL;
  return event;
}


//======================================================================================
// ������J�֐�
//======================================================================================

//--------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�F�l�G�\���I��
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuSeasonSelectEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK *work = wk;

  switch( (*seq) )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  
                                        &DebugSeasonJumpMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL )
      { //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }
      else
      {
        GMEVENT_ChangeEvent( event, 
                             EVENT_SeasonDisplay(work->gsys, work->fieldmap, ret, ret) );
      }
    }
    break;
  }
  
  return( GMEVENT_RES_CONTINUE );
}
