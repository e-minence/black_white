//======================================================================
/**
 * @file  event_debug_menu_script.c
 * @brief �t�B�[���h�f�o�b�O���j���[�F�f�o�b�O�X�N���v�g
 * @author  tamada
 * @date  2009.11.29
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"      //HEAPID_
#include "arc_def.h"          //ARCID_

#include "fieldmap.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"


#include "font/font.naix"

#include "field_debug.h"


#include "event_debug_local.h"

#include "arc/fieldmap/debug_list.h"  //DEBUG_SCR_
#include "arc/fieldmap/script_seq.naix"
#include "script.h" //SCRIPT_ChangeScript

#include "arc/fieldmap/debug_symbol.naix"  //DEBUG_SYM_

#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SjisToStrcode

//======================================================================
//======================================================================

//======================================================================
//======================================================================
//--------------------------------------------------------------
/// ���j���[���ڍő吔�擾�F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static u16 DEBUG_GetDebugScriptMax( GAMESYS_WORK* gsys, void* cb_work )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_SCRSEQ, HEAPID_PROC );
  u16 size = GFL_ARC_GetDataSizeByHandle( p_handle, NARC_script_seq_debug_list_bin );
  GFL_ARC_CloseDataHandle( p_handle );
  GF_ASSERT( size % DEBUG_SCR_EACH_SIZE == 0 );
  return size / DEBUG_SCR_EACH_SIZE;
  //return DEBUG_SCR_MAX;
}

//--------------------------------------------------------------
/// ���j���[���ڐ����F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static void DEBUG_SetMenuWork_DebugScript(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id,max = DEBUG_GetDebugScriptMax(gsys, cb_work);
  
  u8 buffer[DEBUG_SCR_EACH_SIZE];
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_SCRSEQ, heapID );
  u16 * utfStr = GFL_HEAP_AllocClearMemory( heapID, DEBUG_SCR_NAME_LEN * sizeof(u16) );
  STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_SCR_NAME_LEN, heapID );

  for( id = 0; id < max; id++ ){
    u16 real_id;
    GFL_STR_ClearBuffer( strBuf );
    {
      GFL_ARC_LoadDataOfsByHandle(p_handle, NARC_script_seq_debug_list_bin,
          id * DEBUG_SCR_EACH_SIZE, DEBUG_SCR_EACH_SIZE, buffer);
      real_id = *((u16*)&buffer[DEBUG_SCR_OFS_ID]);
      DEB_STR_CONV_SjisToStrcode(
          (const char*)(buffer + DEBUG_SCR_OFS_NAME), utfStr, DEBUG_SCR_NAME_LEN );
      //OS_Printf("DEBUG SCRIPT %5d:%s\n", real_id, buffer + DEBUG_SCR_OFS_NAME);
      GFL_STR_SetStringCode( strBuf, utfStr );
    }
    FLDMENUFUNC_AddStringListData( list, strBuf, real_id, heapID );
  }
  GFL_HEAP_FreeMemory( strBuf );
  GFL_HEAP_FreeMemory( utfStr );
  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^�F�f�o�b�O�X�N���v�g
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugScriptSelectData = {
  0,
  0,                                ///<���ڍő吔�i�Œ胊�X�g�łȂ��ꍇ�A�O�j
  NULL,                             ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  &DATA_DebugMenuList_ZoneSel,      ///<���j���[�\���w��f�[�^�i���p�j
  1, 1, 20, 16,
  DEBUG_SetMenuWork_DebugScript,    ///<���j���[�����֐��ւ̃|�C���^
  DEBUG_GetDebugScriptMax,          ///<���ڍő吔�擾�֐��ւ̃|�C���^
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�f�o�b�O�X�N���v�g�I��
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuScriptSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugScriptSelectData );
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
      
      //OS_Printf("DEBUG SCRIPT SELECT %d\n", ret );
      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }
      
      SCRIPT_ChangeScript( event, ret, NULL, work->heapID );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FLDMENU_DebugScript( const DEBUG_MENU_EVENT_WORK * now_wk )
{
	GMEVENT * new_event = GMEVENT_Create( now_wk->gmSys, NULL,
      debugMenuScriptSelectEvent, sizeof(DEBUG_MENU_EVENT_WORK) );
	DEBUG_MENU_EVENT_WORK * new_wk = GMEVENT_GetEventWork( new_event );

	GFL_STD_MemClear( new_wk, sizeof(DEBUG_MENU_EVENT_WORK) );
  *new_wk = *now_wk;
  new_wk->call_proc = NULL;
  new_wk->menuFunc = NULL;

  return new_event;
}

//======================================================================
//======================================================================
typedef struct {
  DEBUG_MENU_EVENT_WORK dmew;
  u32 arc_idx;
  u32 select_id;
}DEBUG_FLAGMENU_EVENT_WORK;

//======================================================================
//======================================================================
//--------------------------------------------------------------
/// ���j���[���ڍő吔�擾�F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static u16 DEBUG_GetDebugFlagMax( GAMESYS_WORK* gsys, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_DEBUG_SYM, df_work->dmew.heapID );
  u16 size = GFL_ARC_GetDataSizeByHandle(p_handle, df_work->arc_idx );
  GFL_ARC_CloseDataHandle( p_handle );
  GF_ASSERT( size % DEBUG_SCR_EACH_SIZE == 0 );
  return size / DEBUG_SCR_EACH_SIZE;
}

//--------------------------------------------------------------
/// ���j���[���ڐ����F�f�o�b�O�X�N���v�g
//--------------------------------------------------------------
static void DEBUG_SetMenuWork_DebugFlag(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  DEBUG_FLAGMENU_EVENT_WORK * df_work = cb_work;
  int id,max = DEBUG_GetDebugFlagMax(gsys, cb_work);
  
  u8 buffer[DEBUG_SCR_EACH_SIZE];
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_DEBUG_SYM, heapID );
  u16 * utfStr = GFL_HEAP_AllocClearMemory( heapID, DEBUG_SCR_NAME_LEN * sizeof(u16) );
  STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_SCR_NAME_LEN + 1, heapID );

  for( id = 0; id < max; id++ ){
    u16 real_id;
    GFL_STR_ClearBuffer( strBuf );
    {
      GFL_ARC_LoadDataOfsByHandle(p_handle, df_work->arc_idx,
          id * DEBUG_SCR_EACH_SIZE, DEBUG_SCR_EACH_SIZE, buffer);
      real_id = *((u16*)&buffer[DEBUG_SCR_OFS_ID]);
      DEB_STR_CONV_SjisToStrcode(
          (const char*)(buffer + DEBUG_SCR_OFS_NAME), utfStr, DEBUG_SCR_NAME_LEN );
      //OS_Printf("DEBUG Flag %5d:%s\n", real_id, buffer + DEBUG_SCR_OFS_NAME);
      GFL_STR_SetStringCode( strBuf, utfStr );
    }
    FLDMENUFUNC_AddStringListData( list, strBuf, real_id, heapID );
  }
  GFL_HEAP_FreeMemory( strBuf );
  GFL_HEAP_FreeMemory( utfStr );
  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�f�o�b�O���j���[�������f�[�^�F�f�o�b�O�X�N���v�g
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugFlagSelectData = {
  0,
  0,                                ///<���ڍő吔�i�Œ胊�X�g�łȂ��ꍇ�A�O�j
  NULL,                             ///<�Q�Ƃ��郁�j���[�f�[�^�i��������ꍇ��NULL)
  &DATA_DebugMenuList_ZoneSel,      ///<���j���[�\���w��f�[�^�i���p�j
  1, 1, 20, 16,
  DEBUG_SetMenuWork_DebugFlag,    ///<���j���[�����֐��ւ̃|�C���^
  DEBUG_GetDebugFlagMax,          ///<���ڍő吔�擾�֐��ւ̃|�C���^
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�f�o�b�O�X�N���v�g�I��
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuFlagSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
	DEBUG_FLAGMENU_EVENT_WORK * new_wk = wk;
  DEBUG_MENU_EVENT_WORK * dmew = &new_wk->dmew;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( dmew->gdata );
  
  switch( (*seq) ){
  case 0:
    dmew->menuFunc = DEBUGFLDMENU_InitEx( dmew->fieldWork, dmew->heapID,  &DebugFlagSelectData, new_wk );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( dmew->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      //OS_Printf("DEBUG Flag SELECT %d\n", ret );
      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        FLDMENUFUNC_DeleteMenu( dmew->menuFunc );
        return( GMEVENT_RES_FINISH );
      }
      new_wk->select_id = ret;
      (*seq) ++;
      break;
    case 2:
      OS_Printf("DEBUG FLAG:%d\n", EVENTWORK_CheckEventFlag( evwork, new_wk->select_id ) );
      (*seq) ++;
    case 3:
      {
        int trg = GFL_UI_KEY_GetTrg();
        if (trg & PAD_BUTTON_A) {
          if (EVENTWORK_CheckEventFlag( evwork ,new_wk->select_id ) ) {
            EVENTWORK_ResetEventFlag( evwork, new_wk->select_id );
          } else {
            EVENTWORK_SetEventFlag( evwork, new_wk->select_id );
          }
          OS_Printf("DEBUG FLAG:Changed\n");
          (*seq) --;
        } else if ( trg & PAD_BUTTON_B ) {
          OS_Printf("DEBUG FLAG:Fixed\n");
          (*seq) = 1;
        }
      }
      break;
      
      
      
      //Flag_ChangeFlag( event, ret, NULL, dmew->heapID );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FLDMENU_DebugFlag( const DEBUG_MENU_EVENT_WORK * now_wk )
{
	GMEVENT * new_event = GMEVENT_Create( now_wk->gmSys, NULL,
      debugMenuFlagSelectEvent, sizeof(DEBUG_FLAGMENU_EVENT_WORK) );
	DEBUG_FLAGMENU_EVENT_WORK * new_wk = GMEVENT_GetEventWork( new_event );
  DEBUG_MENU_EVENT_WORK * dmew = &new_wk->dmew;

	GFL_STD_MemClear( new_wk, sizeof(DEBUG_FLAGMENU_EVENT_WORK) );
  *dmew = *now_wk;
  dmew->call_proc = NULL;
  dmew->menuFunc = NULL;

  new_wk->arc_idx = NARC_debug_symbol_event_flag_bin;

  return new_event;
}

