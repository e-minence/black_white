//======================================================================
/**
 * @brief  �f�o�b�O���j���[�w�ڑ��`�F�b�N�x
 * @file   event_debug_menu_connect_check.c
 * @author obata
 * @date   2010.05.01
 */
//======================================================================
#ifdef  PM_DEBUG

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

#include "fieldmap.h"
#include "font/font.naix"
#include "field_debug.h"
#include "field_event_check.h"
#include "event_debug_local.h"
#include "event_debug_all_connect_check.h" // for EVENT_DEBUG_AllConnectCheck
#include "event_debug_menu_connect_check.h"


//======================================================================
//  define
//======================================================================
#define D_MENU_CHARSIZE_X (18)    //���j���[����
#define D_MENU_CHARSIZE_Y (16)    //���j���[�c��


//======================================================================
//  proto
//======================================================================
static FLDMENUFUNC * DEBUGFLDMENU_InitExPos( FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos );
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y); 
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );
static BOOL debugMenuCallProc_AllConnectCheck( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_NowConnectCheck( DEBUG_MENU_EVENT_WORK * p_wk );


//======================================================================
//  �f�o�b�O���j���[���X�g
//======================================================================
//--------------------------------------------------------------
/// �f�o�b�O���j���[���X�g
/// �f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
  { DEBUG_FIELD_CONNECT_01, debugMenuCallProc_NowConnectCheck }, // ���݂̃}�b�v
  { DEBUG_FIELD_CONNECT_02, debugMenuCallProc_AllConnectCheck }, // �S�}�b�v
};


//--------------------------------------------------------------
/// ���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
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
  2,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
  DebugMenu_IineCallBack,  //�P�s���Ƃ̃R�[���o�b�N
};

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugMenuData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_DebugMenuList),
  DATA_DebugMenuList,
  &DATA_DebugMenuListHeader,
  1, 1, D_MENU_CHARSIZE_X, D_MENU_CHARSIZE_Y,
  NULL,
  NULL,
};

//======================================================================
//  �C�x���g�F�t�B�[���h�f�o�b�O���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @param page_id �f�o�b�O���j���[�y�[�W
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu_ConnectCheck( GAMESYS_WORK *gsys, void* wk )
{
  DEBUG_MENU_EVENT_WORK * dmew;
  GMEVENT * event;

  event = GMEVENT_Create(
    gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));

  dmew = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );

  dmew->gmSys     = gsys;
  dmew->gmEvent   = event;
  dmew->gdata     = GAMESYSTEM_GetGameData( gsys );
  dmew->fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  dmew->heapID    = HEAPID_PROC;

  // POS�\���X�V�@���@��~
  {
    FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( dmew->fieldWork );
    FIELD_DEBUG_SetPosUpdateFlag( debug, FALSE );
  }

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static FLDMENUFUNC * DebugMenuInitCore(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, void* cb_work, u16 list_pos, u16 cursor_pos )
{
  GAMESYS_WORK * gsys;
  FLDMENUFUNC * ret;
  GFL_MSGDATA *msgData;
  FLDMSGBG *msgBG;
  FLDMENUFUNC_LISTDATA *listdata;
  FLDMENUFUNC_HEADER menuH;
  u16 max;

  gsys = FIELDMAP_GetGameSysWork( fieldmap );
  msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  msgData = FLDMSGBG_CreateMSGDATA( msgBG, init->msg_arc_id );

  if (init->getMaxFunc) {
    max = init->getMaxFunc( gsys, cb_work );
  } else {
    max = init->max;
  }
  if (init->makeListFunc) {
    listdata = FLDMENUFUNC_CreateListData( max, heapID );
    init->makeListFunc( gsys, listdata, heapID, msgData, cb_work );
  } else {
    listdata = FLDMENUFUNC_CreateMakeListData( init->menulist, max, msgData, heapID );
  }

  menuH = *(init->menuH);
  {
    u8 sy = (max * 2 < init->sy) ? max * 2 : init->sy;
    FLDMENUFUNC_InputHeaderListSize( &menuH, max, init->px, init->py, init->sx, sy );
  }
  ret = FLDMENUFUNC_AddMenuList( msgBG, &menuH, listdata, list_pos, cursor_pos );
  GFL_MSG_Delete( msgData );
  return ret;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static FLDMENUFUNC * DEBUGFLDMENU_Init(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, 0, 0 );
}

static FLDMENUFUNC * DEBUGFLDMENU_InitExPos(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, list_pos, cursor_pos );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�f�o�b�O���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch (*seq) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        (*seq)++;
      }else{              //����
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( work->call_proc != NULL ){
        if( work->call_proc(work) == TRUE ){
          return( GMEVENT_RES_CONTINUE );
        }
      }

      // POS�\���X�V�@���@�ĊJ
      {
        FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( work->fieldWork );
        FIELD_DEBUG_SetPosUpdateFlag( debug, TRUE );
      }
      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �P�s���Ƃ̃R�[���o�b�N
 *
 *  @param  lw    ���[�N
 *  @param  param ����
 *  @param  y     Y�ʒu
 */
//-----------------------------------------------------------------------------
static void DebugMenu_IineCallBack( BMPMENULIST_WORK* lw, u32 param, u8 y )
{
  if( param == BMPMENULIST_LABEL ) {
    BmpMenuList_TmpColorChange( lw, 4, 0, 4 );
  }
	else {
    BmpMenuList_TmpColorChange( lw, 1, 0, 2 );
  }
}

//======================================================================
//  �f�o�b�O���j���[�Ăяo��
//======================================================================

//--------------------------------------------------------------
/// �f�o�b�O���j���[�F�S�ڑ��`�F�b�N
//--------------------------------------------------------------
static BOOL debugMenuCallProc_AllConnectCheck( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = EVENT_DEBUG_AllConnectCheck( p_wk->gmSys, NULL );

  GMEVENT_ChangeEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// �f�o�b�O���j���[�F���ڑ��`�F�b�N
//--------------------------------------------------------------
static BOOL debugMenuCallProc_NowConnectCheck( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = EVENT_DEBUG_NowConnectCheck( p_wk->gmSys, NULL );

  GMEVENT_ChangeEvent( p_wk->gmEvent, event );

  return TRUE;
}

#endif  //  PM_DEBUG

