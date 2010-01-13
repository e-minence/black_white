//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_wfbc.c
 *	@brief  WFBC�p�X�N���v�g�R�}���h�S
 *	@author	tomoya takahashi
 *	@date		2009.12.17
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "print/wordset.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "fieldmap.h"
#include "field_wfbc.h"
#include "fldmmdl.h"

#include "scrcmd_wfbc.h"

#include "scrcmd_wfbc_define.h" 

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�@��b�J�n����
 *	@return
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_TalkStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  objid = MMDL_GetOBJID( p_frontmmdl );

  // ��b�J�E���g�@�i1��1��j
  FIELD_WFBC_AddTalkPointPeople( p_wfbc, objid );
  
	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��b�I���ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_TalkEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  GAMEDATA* p_gamedata = SCRCMD_WORK_GetGameData( work );
  FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gamedata );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // ��b�X�R�A���Z
  objid = MMDL_GetOBJID( p_frontmmdl );


  // Palace�Ȃ�A��čs������

  // ��b�����̂ŁA�F�X�ȃ}�X�N��OFF
  if( FIELD_WFBC_IsBattlePeople( p_wfbc, objid ) )
  {
    // �o�g�������ł��Ȃ�
    FIELD_WFBC_SetBattlePeople( p_wfbc, objid );
    
    // �����������Z
    FIELD_WFBC_EVENT_AddBCNpcWinCount( p_event );

  }

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �O�̊X�iBC�j�̗����ɂ���l�̖��O�����[�h�Z�b�g�ɐݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_SetRirekiPlayerName( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET** pp_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_core;
  const FIELD_WFBC_PEOPLE* cp_people;
  u16 idx = SCRCMD_GetVMWorkValue( core, wk );  // 
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // OBJID
  objid = MMDL_GetOBJID( p_frontmmdl );

  // ���[�h�Z�b�g�ɐݒ�
  FIELD_WFBC_SetWordSetParentPlayer( p_wfbc, *pp_wordset, objid, idx, heapID );

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC  �e���̎擾
 *
 *  FIELD_INIT�̏�ԂŌĂ΂��\��������̂ŁA���ӁI�I�I
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_GetData( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA* p_gamedata = SCRCMD_WORK_GetGameData( work );
  FIELD_WFBC_CORE* p_wfbc_core = GAMEDATA_GetMyWFBCCoreData( p_gamedata );

  u16 define = SCRCMD_GetVMWorkValue( core, wk );  // 
  u16 *ret_wk	= SCRCMD_GetVMWork( core, work );
  
  // FIELD_INIT�����݂��Ă����OK�ȏ���
  if( define == WFBC_GET_PARAM_PEOPLE_NUM )
  {
    (*ret_wk) = FIELD_WFBC_CORE_GetPeopleNum( p_wfbc_core, MAPMODE_NORMAL );
  }
  else
  // FIELDMAP�\�z��ł���K�v�����鏈��
  {
    SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
    FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
    FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
    FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
    const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
    FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gamedata );
    s16 gx, gy, gz;
    MMDL* p_frontmmdl;
    u32 objid;
    const FIELD_WFBC_CORE_PEOPLE* cp_people_core;
    const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
    const FIELD_WFBC_PEOPLE* cp_people;

    // �ڂ̑O�̃O���b�h�擾
    FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );

    // �ڂ̑O�ɂ���l��������
    p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

    // OBJID
    if( p_frontmmdl )
    {
      objid = MMDL_GetOBJID( p_frontmmdl );

      // �l�����擾
      cp_people = FIELD_WFBC_GetPeople( p_wfbc, objid );
      if( cp_people )
      {
        cp_people_data = FIELD_WFBC_PEOPLE_GetPeopleData( cp_people );
        cp_people_core = FIELD_WFBC_PEOPLE_GetPeopleCore( cp_people );
      }
    }


    GF_ASSERT(define < WFBC_GET_PARAM_MAX);

    switch( define )
    {
    // �o�g���g���[�i�[ID
    case WFBC_GET_PARAM_BATTLE_TRAINER_ID:
      (*ret_wk) = cp_people_data->btl_trdata;
      break;
    // �X�ɂ��������邩
    case WFBC_GET_PARAM_IS_TAKES_ID:
      (*ret_wk) = FIELD_WFBC_CORE_PEOPLE_IsMoodTakes( cp_people_core );
      break;
    // �O�ɂ���BC�̊X�̗��������邩
    case WFBC_GET_PARAM_IS_RIREKI:
      (*ret_wk) = FIELD_WFBC_CORE_PEOPLE_IsParentIn( cp_people_core );
      break;
    // BC�@NPC������
    case WFBC_GET_PARAM_BC_NPC_WIN_NUM:
      (*ret_wk) = FIELD_WFBC_EVENT_GetBCNpcWinCount( p_event );
      break;
    // BC�@NPC�����ڕW��
    case WFBC_GET_PARAM_BC_NPC_WIN_TARGET:
      (*ret_wk) = FIELD_WFBC_EVENT_GetBCNpcWinTarget( p_event );
      break;
    // WF�@�����C�x���g�@�|�P�����͂��邩�H
    case WFBC_GET_PARAM_WF_IS_POKECATCH:
      if( FIELD_WFBC_GetPeopleNum( p_wfbc ) == 0 ){
        (*ret_wk) = FALSE;
      }else{
        (*ret_wk) = TRUE;
      }
      break;
    // WF �����C�x���g�@���J���A�C�e���̎擾
    case WFBC_GET_PARAM_WF_ITEM:
      (*ret_wk) = FIELD_WFBC_EVENT_GetWFPokeCatchEventItem( p_event );
      break;
    // WF �����C�x���g�@�T���|�P�����i���o�[�̎擾
    case WFBC_GET_PARAM_WF_POKE:
      (*ret_wk) = FIELD_WFBC_EVENT_GetWFPokeCatchEventMonsNo( p_event );
      break;
    // �b�������OBJID�i0�`49�j
    case WFBC_GET_PARAM_OBJ_ID:
      (*ret_wk) = objid;
      break;
    // BC�@����ƃo�g���\���`�F�b�N
    case WFBC_GET_PARAM_BC_CHECK_BATTLE:
      (*ret_wk) = FIELD_WFBC_IsBattlePeople( p_wfbc, objid );
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  }
	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  BC�@NPC�����C�x���g�@�����ڕW�������Z
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_AddBCNpcWinTarget( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK* p_gamesys = FIELDMAP_GetGameSysWork( fparam->fieldMap );
  GAMEDATA* p_gamedata = GAMESYSTEM_GetGameData( p_gamesys );
  FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gamedata );

  FIELD_WFBC_EVENT_AddBCNpcWinTarget( p_event );
	return VMCMD_RESULT_CONTINUE;
}



