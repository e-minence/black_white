//=============================================================================
/**
 *
 *	@file		event_debug_demo3d.c
 *	@brief  �f�o�b�O���j���|�F�f���Đ�
 *	@author		hosaka genya
 *	@data		2009.12.07
 *
 */
//=============================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "demo/demo3d.h"

#include "fieldmap.h"

#include "message.naix"  // NARC_xxxx
#include "msg/msg_d_field.h"  // for DEBUG_FIELD_STR_xxxx
#include "event_debug_local.h"  // for DEBUG_MENU_INITIALIZER

#include "event_fieldmap_control.h"

#include "event_debug_demo3d.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//======================================================================================
// ���C�x���g���[�N
//======================================================================================
typedef struct
{
  HEAPID           heapID;
  GAMESYS_WORK*      gsys;
  GAMEDATA*         gdata;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  DEMO3D_PARAM   param;

} EVENT_WORK;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk );
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work );

//=============================================================================
/**
 *							�f�[�^�e�[�u��
 */
//=============================================================================

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

// �f�o�b�O���j���[������
static const DEBUG_MENU_INITIALIZER DebugDemo3DJumpMenuData = {
  NARC_message_d_field_dat,
  0,
  NULL,
  &DATA_DebugMenuList_ZoneSel, //���p
  1, 1, 16, 17,
  DebugMenuList_Make,
  DebugMenuList_GetMax,
};

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�J�n
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	heapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_Demo3DSelect( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, NULL, debugMenuDemo3DSelectEvent, sizeof(EVENT_WORK) );
   // ������
  work = GMEVENT_GetEventWork( event );
  work->heapID   = heapID;
  work->gsys     = gsys;
  work->gdata    = GAMESYSTEM_GetGameData( gsys );
  work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->menuFunc = NULL;

  return event;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g Main����
 *
 *	@param	GMEVENT *event
 *	@param	*seq
 *	@param	*wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK *work = wk;
  
  switch( (*seq) )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  
                                        &DebugDemo3DJumpMenuData );
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
        HOSAKA_Printf("menu ret = %d \n", ret);

        DEMO3D_PARAM_SetFromRTC( &work->param, work->gsys, ret+1, 0 );
        GMEVENT_CallEvent( event, EVENT_FieldSubProc( work->gsys, work->fieldmap,
        FS_OVERLAY_ID(demo3d), &Demo3DProcData, &work->param ) );
    
        (*seq)++;
      }
    }
    break;
  case 2:
    return ( GMEVENT_RES_FINISH );
  }

  return ( GMEVENT_RES_CONTINUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������ ���X�g����
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	*list
 *	@param	heapID
 *	@param	msgData
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id;
  const int max   = DEMO3D_ID_MAX-1;
  STRBUF *strBuf1 = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap  = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG          = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgData    = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
  
  for( id = 0; id < max; id++ ){
    GFL_STR_ClearBuffer( strBuf1 );
    GFL_MSG_GetString( pMsgData, DEBUG_FIELD_DEMO3D_DEMO_01 + id, strBuf1 );  // ��������擾
    FLDMENUFUNC_AddStringListData( list, strBuf1, id, heapID );
  }
  
  GFL_MSG_Delete( pMsgData );
  GFL_HEAP_FreeMemory( strBuf1 );

}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������ ���X�g���ڐ���Ԃ�
 *
 *	@param	GAMESYS_WORK* gsys
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return DEMO3D_ID_MAX-1;
}

