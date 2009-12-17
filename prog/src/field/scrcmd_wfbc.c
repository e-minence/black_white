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
  WORDSET* p_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // ��b�X�R�A���Z
  objid = MMDL_GetOBJID( p_frontmmdl );
  FIELD_WFBC_AddTalkPointPeople( p_wfbc, objid );


  // ���[�h�Z�b�g�ɗ�������ݒ肵�Ă���
  FIELD_WFBC_SetWordSetParentPlayer( p_wfbc, p_wordset, objid, FIELDMAP_GetHeapID( fparam->fieldMap ) );
  
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
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET* p_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
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
  // �o�g�������ł��Ȃ�
  FIELD_WFBC_SetBattlePeople( p_wfbc, objid );

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ڂ̑O�̐l�̃o�g���g���[�i�[ID�̎擾
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_GetBattleTrainerID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET* p_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  const FIELD_WFBC_PEOPLE* cp_people;
	u16 *ret_wk	= SCRCMD_GetVMWork( core, work );

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // ��b�X�R�A���Z
  objid = MMDL_GetOBJID( p_frontmmdl );

  // �l�����擾
  cp_people = FIELD_WFBC_GetPeople( p_wfbc, objid );
  cp_people_data = FIELD_WFBC_PEOPLE_GetPeopleData( cp_people );

  // �g���[�i�[ID��Ԃ�
  (*ret_wk) = cp_people_data->btl_trdata;

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A��čs�����ԂȂ̂��`�F�b�N
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_IsTakesIt( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET* p_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_core;
  const FIELD_WFBC_PEOPLE* cp_people;
	u16 *ret_wk	= SCRCMD_GetVMWork( core, work );

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // ��b�X�R�A���Z
  objid = MMDL_GetOBJID( p_frontmmdl );

  // �l�����擾
  cp_people = FIELD_WFBC_GetPeople( p_wfbc, objid );
  cp_people_core = FIELD_WFBC_PEOPLE_GetPeopleCore( cp_people );

  // �@���l���`�F�b�N
  (*ret_wk) = FIELD_WFBC_CORE_PEOPLE_IsMoodTakes( cp_people_core );

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �O�̊X�̗��������邩�H
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_IsRireki( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET* p_wordset = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_core;
  const FIELD_WFBC_PEOPLE* cp_people;
	u16 *ret_wk	= SCRCMD_GetVMWork( core, work );

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );

  // ��b�X�R�A���Z
  objid = MMDL_GetOBJID( p_frontmmdl );

  //@TODO �ʐM����ƈႤ�l���������Ă��邩�`�F�b�N����B 
  {
    // �@���l���`�F�b�N
    (*ret_wk) = TRUE;
  }

	return VMCMD_RESULT_CONTINUE;
}




