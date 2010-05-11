//======================================================================
/**
 * @file scr_pdw.c
 * @brief �X�N���v�g�R�}���h�@PDW�֘A
 * @author	Ariizumi Nobuhiko
 * @date	10/03/06
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "sound/pm_sndsys.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "savedata/save_control.h"

#include "message.naix"
#include "msg/script/msg_pdw_scr.h"
#include "scrcmd_pdw.h"

#include "../../../resource/fldmapdata/script/pdw_scr_local.h"

#include "test/ariizumi/ari_debug.h"
//======================================================================
//  define
//======================================================================


//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// EVENT_MUSICAL_WORK
//--------------------------------------------------------------

//======================================================================
//  proto
//======================================================================

VMCMD_RESULT EvCmdPDW_CommonTools( VMHANDLE *core, void *wk );
VMCMD_RESULT EvCmdPDW_FurnitureTools( VMHANDLE *core, void *wk );
VMCMD_RESULT EvCmdPDW_SetFurnitureWord( VMHANDLE *core, void *wk );


//--------------------------------------------------------------
/**
 *  _PDW_COMMON_TOOLS PDW�ėp�c�[��
 *  @param type  �R�}���h���
 *  @param val1  �擾�p�ԍ�
 *  @param val2  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPDW_CommonTools( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );

  u16  type = SCRCMD_GetVMWorkValue( core, work );
  u16  val1 = SCRCMD_GetVMWorkValue( core, work );
  u16  val2 = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );
  
  DREAMWORLD_SAVEDATA *pdwSave = GAMEDATA_GetDreamWorkdSaveWork( gdata );
  
  ARI_TPrintf("ScriptPDWTools[%d][%d][%d]\n",type,val1,val2);
  switch( type )
  {
  case PDW_TOOLS_CHECK_ACCOUNT:
    if( DREAMWORLD_SV_GetAccount( pdwSave ) == TRUE )
    {
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;

  default:
    GF_ASSERT_MSG( 0 , "Script PDW_TOOLS type is invalid!![%d]\n",type );
    break;
  }
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 *  _PDW_FURNITURE_TOOLS PDW�Ƌ�j���[�F�ėp�c�[��
 *  @param val1  �擾�p�ԍ�
 *  @param val2  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPDW_FurnitureTools( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );

  u16  type = SCRCMD_GetVMWorkValue( core, work );
  u16  val1 = SCRCMD_GetVMWorkValue( core, work );
  u16  val2 = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  DREAMWORLD_SAVEDATA *pdwSave = GAMEDATA_GetDreamWorkdSaveWork( gdata );
  
  ARI_TPrintf("ScriptPDWFurnitureTools[%d][%d][%d]\n",type,val1,val2);
  switch( type )
  {
  case PDW_FURNITURE_TOOLS_CHECK_LIST:
    //�Ƌ�X�g�����邩�H
    {
      DREAM_WORLD_FURNITUREDATA *furData = DREAMWORLD_SV_GetFurnitureData( pdwSave , 0 );
      if( furData == NULL )
      {
        //�f�[�^�����������H
        *ret_wk = FALSE;
      }
      else
      {
        if( furData->id == 0 )
        {
          //�f�[�^������
          *ret_wk = FALSE;
        }
        else
        {
          *ret_wk = TRUE;
        }
      }
    }
    break;
    
  case PDW_FURNITURE_TOOLS_CHECK_SELECT:
    //�Ƌ��I�񂾂��H
    {
      int selectNo = DREAMWORLD_SV_GetSelectFurnitureNo( pdwSave );
      if( selectNo == DREAM_WORLD_NOFURNITURE )
      {
        *ret_wk = FALSE;
      }
      else
      {
        *ret_wk = TRUE;
      }
    }
    break;

  case PDW_FURNITURE_TOOLS_IS_SYNC_FURNITURE:
    //�O��Ƌ�𑗂������H
    {
      BOOL isSync = DREAMWORLD_SV_GetIsSyncFurniture( pdwSave );
      if( isSync == TRUE )
      {
        *ret_wk = TRUE;
      }
      else
      {
        *ret_wk = FALSE;
      }
    }
    break;

  case PDW_FURNITURE_TOOLS_RESET_SYNC_FURNITURE:
    //��̃t���O�𗎂Ƃ�
    DREAMWORLD_SV_SetIsSyncFurniture( pdwSave , FALSE );
    break;

  case PDW_FURNITURE_TOOLS_SET_SELECT_FURNITURE:
    //�Ƌ�I��ԍ��Z�b�g(val1 �ԍ�)
    DREAMWORLD_SV_SetSelectFurnitureNo( pdwSave , val1 );
    break;
    
  default:
    GF_ASSERT_MSG( 0 , "Script PDW_TOOLS type is invalid!![%d]\n",type );
    break;
  }  
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 *  _PDW_SET_FURNITURE_WORD PDW�Ƌ�j���[�F�Ƌ�Z�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param val �Z�b�g����Ƌ�ԍ�
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPDW_SetFurnitureWord( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID heapId = SCRCMD_WORK_GetHeapID( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );
  DREAMWORLD_SAVEDATA *pdwSave = GAMEDATA_GetDreamWorkdSaveWork( gdata );

  u16  idx = SCRCMD_GetVMWorkValue( core, work );
  u16  val = SCRCMD_GetVMWorkValue( core, work );

  ARI_TPrintf("ScriptPDWSetFurnitureWord[%d][%d]\n",idx,val);

  {
    DREAM_WORLD_FURNITUREDATA *furData = DREAMWORLD_SV_GetFurnitureData( pdwSave , val );
    if( furData == NULL )
    {
      GF_ASSERT_MSG( 0 , "Script _PDW_SET_FURNITURE_WORD data is invalid!!\n" );
    }
    else
    {
      if( furData->id == 0 )
      {
        //�f�[�^������
        GF_ASSERT_MSG( 0 , "Script _PDW_SET_FURNITURE_WORD data is nothing!!\n" );
      }
      else
      {
        STRBUF* str = GFL_STR_CreateBuffer( DREAM_WORLD_DATA_FURNITURE_NAME_NUM + 2 , heapId );
        GFL_STR_SetStringCode( str , furData->furnitureName );
        WORDSET_RegisterWord( wordset, idx, str, 0, FALSE, PM_LANG );
        GFL_STR_DeleteBuffer(str);
      }
    }
  }
  return VMCMD_RESULT_CONTINUE;
}
