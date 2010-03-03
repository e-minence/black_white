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

//======================================================================
//======================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef BOOL (* MP_FUNC_ADD_CHECK)( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd );
typedef void (* MP_FUNC_ADD)( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd );
typedef u16 (* MP_FUNC_SUCCESS_MSG)( WORDSET * wordset, DREAMWORLD_ITEM_DATA * gpd, SCRCMD_WORK * work );
typedef u16 (* MP_FUNC_FAILURE_MSG)( WORDSET * wordset, DREAMWORLD_ITEM_DATA * gpd, SCRCMD_WORK * work );

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
static MMDL * searchPOSTMANOBJ( FIELDMAP_WORK * fieldWork );
static int searchEventDataPostmanObj( GAMEDATA * gamedata );

//--------------------------------------------------------------
//--------------------------------------------------------------
static const DREAMWORLD_FUNC_TABLE DreamWorldFuncTable[DREAMWORLD_PRESENT_KIND_MAX];

static BOOL MP_Command_AddCheck(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd );
static u16 MP_Command_SuccessMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *gpd, SCRCMD_WORK * work );
static u16 MP_Command_FailureMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *gpd, SCRCMD_WORK * work );
static BOOL MP_Command_Receive(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd );
static u16 MP_Command_ObjStatus( FIELDMAP_WORK * fieldmap );
static u16 MP_Command_ObjID( FIELDMAP_WORK * fieldmap, GAMEDATA * gamedata );

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
  SCRCMD_WORK * work = wk;
  u16 req = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  DREAMWORLD_SAVEDATA * fd = DREAMWORLD_SV_GetDreamWorldSaveData( GAMEDATA_GetSaveControlWork( gamedata ) );
  int index;
  DREAMWORLD_ITEM_DATA gpd;
  u8 gift_type = 0;
  int restNum = DREAMWORLD_SV_GetItemRestNum(fd);

  DREAMWORLD_SV_GetRestItem( fd, &index, &gpd );
  switch ( req )
  {
  case SCR_POSTMAN_REQ_EXISTS:
    if ( restNum != 0 )
    { 
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;
  case SCR_POSTMAN_REQ_ENABLE:
    *ret_wk = MP_Command_AddCheck( gift_type, work, gamedata, &gpd );
    break;

  case SCR_POSTMAN_REQ_OK_MSG:
    *ret_wk = MP_Command_SuccessMsg( gift_type, &gpd, work );
    break;

  case SCR_POSTMAN_REQ_NG_MSG:
    *ret_wk = MP_Command_FailureMsg( gift_type, &gpd, work );
    break;

  case SCR_POSTMAN_REQ_RECEIVE:
    { //��ɓ���鏈��
      if ( MP_Command_Receive( gift_type, work, gamedata, &gpd ) == TRUE )
      {
        DREAMWORLD_SV_DeleteItem( fd, index );
      }
    }
    *ret_wk = 0;
    break;
  case SCR_POSTMAN_REQ_OBJID:
    {
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( SCRCMD_WORK_GetGameSysWork( work ) );
      *ret_wk = MP_Command_ObjID( fieldmap, gamedata );
    }
    break;
  case SCR_POSTMAN_REQ_OBJSTAT:
    {
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( SCRCMD_WORK_GetGameSysWork( work ) );
      *ret_wk = MP_Command_ObjStatus( fieldmap );
    }
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
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd )
{
  MP_FUNC_ADD_CHECK func;
  func = DreamWorldFuncTable[gift_type].add_check_func;
  if ( gpd != NULL && func )
  {
    return func( work, gamedata, gpd );
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��邩�H�`�F�b�N
//--------------------------------------------------------------
static u16 MP_Command_SuccessMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *gpd, SCRCMD_WORK * work )
{
  MP_FUNC_SUCCESS_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = DreamWorldFuncTable[gift_type].set_success_words;
  if ( gpd != NULL && func )
  {
    msg_id = func( wordset, gpd, work );
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
static u16 MP_Command_FailureMsg( u8 gift_type, DREAMWORLD_ITEM_DATA *gpd, SCRCMD_WORK * work )
{
  MP_FUNC_FAILURE_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = DreamWorldFuncTable[gift_type].set_failure_words;
  if ( gpd != NULL && func )
  {
    msg_id = func( wordset, gpd, work );
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
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd )
{ //��ɓ���鏈��
  MP_FUNC_ADD func;
  func = DreamWorldFuncTable[gift_type].add_func;
  if ( gpd != NULL && func )
  {
    func( work, gamedata, gpd );
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F�z�B����OBJID�擾
//--------------------------------------------------------------
static u16 MP_Command_ObjID( FIELDMAP_WORK * fieldmap, GAMEDATA * gamedata )
{
  MMDL * mmdl = searchPOSTMANOBJ( fieldmap );
  if ( mmdl != NULL )
  {
    return MMDL_GetOBJID( mmdl );
  }
  else
  {
    int obj_id = searchEventDataPostmanObj( gamedata );
    if ( obj_id >= 0 )
    {
      return obj_id;
    }
  }
  GF_ASSERT_MSG( 0, "�z�B�������݂��Ȃ��I�I\n" );
  return 0;
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F�z�B��OBJ�����邩�ǂ����H�`�F�b�N
//--------------------------------------------------------------
static u16 MP_Command_ObjStatus( FIELDMAP_WORK * fieldmap )
{
  MMDL * mmdl = searchPOSTMANOBJ( fieldmap );
  if ( mmdl == NULL )
  {
    return SCR_POSTMAN_OBJ_NONE;
  }
  else 
  {
    return SCR_POSTMAN_OBJ_EXISTS;
  }
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �z�B��OBJ����
 *
 * ���ݑ��݂��Ă��铮�샂�f���f�[�^����z�B�����������|�C���^��Ԃ�
 */
//--------------------------------------------------------------
static MMDL * searchPOSTMANOBJ( FIELDMAP_WORK * fieldWork )
{
  u32 no = 0;
	MMDL *mmdl;
  MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		if( MMDL_GetOBJCode(mmdl) == DELIVERY ){
			return( mmdl );
		}
	}
  return NULL;
}
//--------------------------------------------------------------
/**
 * @brief �z�B��OBJ�f�[�^����
 *
 * �C�x���g�f�[�^����A�z�B��OBJ��������obj_id��Ԃ�
 */
//--------------------------------------------------------------
static int searchEventDataPostmanObj( GAMEDATA * gamedata )
{
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );
  const MMDL_HEADER * tbl = EVENTDATA_GetNpcData( evdata );
  int max = EVENTDATA_GetNpcCount( evdata );
  int i;

  if ( tbl == NULL || max == 0 ) return -1;
  
  for ( i = 0; i < max; i ++ )
  {
    if ( tbl[i].obj_code == DELIVERY ) return tbl[i].id;
  }
  return -1;
}


//======================================================================
//  �A�C�e���擾�p�֐�
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL PFuncCheckItem( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd )
{
  u16 item_no;
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  item_no = gpd->itemindex;

  return MYITEM_AddCheck( myitem, item_no, gpd->itemnum, heapID );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void PFuncAddItem( SCRCMD_WORK * work, GAMEDATA * gamedata, DREAMWORLD_ITEM_DATA * gpd )
{
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  u16 item_no = gpd->itemindex;
  BOOL result;

  result = MYITEM_AddItem( myitem, item_no, gpd->itemnum, heapID );
  GF_ASSERT( result );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetSuccessItemWords( WORDSET * wordset, DREAMWORLD_ITEM_DATA * gpd, SCRCMD_WORK * work )
{
  MYSTATUS * mystatus = GAMEDATA_GetMyStatus( SCRCMD_WORK_GetGameData( work ) );
  u16 item_no = gpd->itemindex;
  WORDSET_RegisterPlayerName( wordset, 0, mystatus );
  WORDSET_RegisterItemName( wordset, 1, item_no );
  return msg_dream_postman_08;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetFailureItemWords( WORDSET * wordset, DREAMWORLD_ITEM_DATA * gpd, SCRCMD_WORK * work )
{
  u16 item_no = gpd->itemindex;
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

