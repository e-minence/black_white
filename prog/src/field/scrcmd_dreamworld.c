//======================================================================
/**
 * @file	  scrcmd_dreamworld.c
 * @brief	  �X�N���v�g�R�}���h�F�|�P�����h���[�����[���h�̔z�B��
 * @date	  2010.02.12
 * @author  k.ohno
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_dreamworld.h"
#include "field_mystery_gift.h"

#include "msg/script/msg_dream_postman.h"
#include "savedata/dreamworld_data.h"
#include "savedata/myitem_savedata.h"  //MYITEM_
#include "print/wordset.h"   //WORDSET_

#include "fldmmdl.h"
#include "fieldmap.h" //FIELDMAP_GetMMdlSys
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

#include "savedata/intrude_save.h"

#include "pdw_postman.h"

//======================================================================
//======================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef BOOL (* MP_FUNC_ADD_CHECK)( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did );
typedef void (* MP_FUNC_ADD)( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did );
typedef u16 (* MP_FUNC_SUCCESS_MSG)( WORDSET * wordset, DREAMWORLD_ITEM_DATA * did, SCRCMD_WORK * work );
typedef u16 (* MP_FUNC_FAILURE_MSG)( WORDSET * wordset, DREAMWORLD_ITEM_DATA * did, SCRCMD_WORK * work );

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {
  MP_FUNC_ADD_CHECK add_check_func;
  MP_FUNC_ADD       add_func;
  MP_FUNC_SUCCESS_MSG set_success_words;
  MP_FUNC_FAILURE_MSG set_failure_words;
}DREAMWORLD_FUNC_TABLE;


//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
//--------------------------------------------------------------
static const DREAMWORLD_FUNC_TABLE DreamWorldFuncTable[DREAMWORLD_PRESENT_KIND_MAX];

static BOOL MP_Command_AddCheck(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did );
static u16  MP_Command_SuccessMsg(
    u8 gift_type, DREAMWORLD_ITEM_DATA *did, SCRCMD_WORK * work );
static u16  MP_Command_FailureMsg(
    u8 gift_type, DREAMWORLD_ITEM_DATA *did, SCRCMD_WORK * work );
static BOOL MP_Command_Receive(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did );

//======================================================================
//
//    �X�N���v�g�R�}���h����
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �|�P�����h���[�����[���h�̃A�C�e���擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDreamWorldCommand( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *         work = wk;
  u16                    req = SCRCMD_GetVMWorkValue( core, wk );
  u16 *               ret_wk = SCRCMD_GetVMWork( core, wk );
  HEAPID              heapID = SCRCMD_WORK_GetHeapID( wk );
  GAMEDATA *        gamedata = SCRCMD_WORK_GetGameData( wk );
  DREAMWORLD_SAVEDATA *  dws = DREAMWORLD_SV_GetDreamWorldSaveData( GAMEDATA_GetSaveControlWork( gamedata ) );

  int restNum = DREAMWORLD_SV_GetItemRestNum(dws);
  TAMADA_Printf("PDW_POSTMAN: REST COUNT=%d\n", restNum );

  switch ( req )
  {
  case SCR_PDW_POSTMAN_REQ_EXISTS:
    *ret_wk = ( DREAMWORLD_SV_GetItemRestNum( dws ) > 0 );
    break;
  case SCR_PDW_POSTMAN_REQ_ENABLE:
    if ( PDW_POSTMAN_GetNGCount( gamedata, heapID, dws ) < restNum ) {
      //�󂯎���A�C�e��������
      *ret_wk = TRUE;
    } else {
      //�󂯎���A�C�e�����Ȃ�
      *ret_wk = FALSE;
    }
    break;

  case SCR_PDW_POSTMAN_REQ_OK_INFO:
    //�󂯎����A���\��
    {
      GMEVENT * event = PDW_POSTMAN_CreateInfoEvent( SCRCMD_WORK_GetGameSysWork( wk ) );
      SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
      SCRIPT_CallEvent( sc, event );
    }
    return VMCMD_RESULT_SUSPEND;
    break;

  case SCR_PDW_POSTMAN_REQ_RECEIVE:
    //��ɓ���鏈��
    PDW_POSTMAN_ReceiveItems( gamedata, heapID, dws );
    break;

  case SCR_PDW_POSTMAN_REQ_NG_ITEM:
    {
      SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
      WORDSET *wordset    = SCRIPT_GetWordSet( sc );
      u16 item_sym = PDW_POSTMAN_searchNGItem( gamedata, heapID, dws, *ret_wk );
      WORDSET_RegisterItemName( wordset, 0, item_sym );
    }
    break;

  case SCR_PDW_POSTMAN_REQ_NG_COUNT:
    *ret_wk = PDW_POSTMAN_GetNGCount( gamedata, heapID, dws );
    break;

  default:
    GF_ASSERT( 0 );
  }

  return VMCMD_RESULT_CONTINUE;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��邩�H�`�F�b�N
//--------------------------------------------------------------
static BOOL MP_Command_AddCheck(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did )
{
  MP_FUNC_ADD_CHECK func;
  func = DreamWorldFuncTable[gift_type].add_check_func;
  if ( did != NULL && func )
  {
    return func( work, gamedata, did );
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��邩�H�`�F�b�N
//--------------------------------------------------------------
static u16 MP_Command_SuccessMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *did, SCRCMD_WORK * work )
{
  MP_FUNC_SUCCESS_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = DreamWorldFuncTable[gift_type].set_success_words;
  if ( did != NULL && func )
  {
    msg_id = func( wordset, did, work );
    return msg_id;
  }
  else
  {
    return 0;  //�Ƃ肠�����̃G���[���p
  }
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎�������b�Z�[�W
//--------------------------------------------------------------
static u16 MP_Command_FailureMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *did, SCRCMD_WORK * work )
{
  MP_FUNC_FAILURE_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = DreamWorldFuncTable[gift_type].set_failure_words;
  if ( did != NULL && func )
  {
    msg_id = func( wordset, did, work );
    return msg_id;
  }
  else
  {
    return 0;  //�Ƃ肠�����̃G���[���p
  }
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��Ȃ��������b�Z�[�W
//--------------------------------------------------------------
static BOOL MP_Command_Receive(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did )
{ //��ɓ���鏈��
  MP_FUNC_ADD func;
  func = DreamWorldFuncTable[gift_type].add_func;
  if ( did != NULL && func )
  {
    func( work, gamedata, did );
    return TRUE;
  }
  return FALSE;
}



//======================================================================
//  �A�C�e���擾�p�֐�
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL PFuncCheckItem( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did )
{
  u16 item_no;
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  item_no = did->itemindex;

  return MYITEM_AddCheck( myitem, item_no, did->itemnum, heapID );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void PFuncAddItem( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * did )
{
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  u16 item_no = did->itemindex;
  BOOL result;

  result = MYITEM_AddItem( myitem, item_no, did->itemnum, heapID );
  GF_ASSERT( result );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetSuccessItemWords( WORDSET * wordset, DREAMWORLD_ITEM_DATA * did, SCRCMD_WORK * work )
{
  MYSTATUS * mystatus = GAMEDATA_GetMyStatus( SCRCMD_WORK_GetGameData( work ) );
  u16 item_no = did->itemindex;
  WORDSET_RegisterPlayerName( wordset, 0, mystatus );
  WORDSET_RegisterItemName( wordset, 1, item_no );
  return msg_dream_postman_08;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetFailureItemWords( WORDSET * wordset, DREAMWORLD_ITEM_DATA * did, SCRCMD_WORK * work )
{
  u16 item_no = did->itemindex;
  WORDSET_RegisterItemName( wordset, 0, item_no );

  return msg_dream_postman_09;
}


//======================================================================
//
//
//
//======================================================================
//--------------------------------------------------------------
//  ���蕨�̎�ޖ��̕���p�e�[�u�� ����A�C�e���̂�
//--------------------------------------------------------------
static const DREAMWORLD_FUNC_TABLE DreamWorldFuncTable[DREAMWORLD_PRESENT_KIND_MAX] = {
  { 
    PFuncCheckItem,
    PFuncAddItem,
    PFuncSetSuccessItemWords,
    PFuncSetFailureItemWords,
  },
};

