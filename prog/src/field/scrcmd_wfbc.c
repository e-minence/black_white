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

#include "arc/arc_def.h"
#include "arc/script_message.naix"

#include "print/wordset.h"

#include "field/field_msgbg.h"

#include "field_comm/intrude_work.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_field_fade.h"

#include "fieldmap.h"
#include "field_wfbc.h"
#include "fldmmdl.h"
#include "field_status_local.h"

#include "scrcmd_wfbc.h"

#include "scrcmd_wfbc_define.h" 

#include "msg/msg_place_name.h"  // for MAPNAME_xxxx
#include "msg/script/msg_plc10.h"  
#include "msg/script/msg_bc10.h"  
#include "msg/script/msg_wc10.h"  

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
// Palace�ʐM��b�C�x���g
static GMEVENT* EVENT_SetUp_WFBC_Palece_Talk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT_RESULT EVENT_WFBC_Palece_Talk( GMEVENT* p_event, int* p_seq, void* p_wk );



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

  objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );

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
  objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );


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
  WORDSET* pp_wordset = SCRIPT_GetWordSet( sc );
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
  objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );

  // ���[�h�Z�b�g�ɐݒ�
  FIELD_WFBC_SetWordSetParentPlayer( p_wfbc, pp_wordset, objid, idx, heapID );

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
    u32 mmdlid;
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
      objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );
      mmdlid = MMDL_GetOBJID( p_frontmmdl );

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
    // �b�������OBJID�i0�`29�j
    case WFBC_GET_PARAM_OBJ_ID:
      (*ret_wk) = objid;
      break;
    // BC�@����ƃo�g���\���`�F�b�N
    case WFBC_GET_PARAM_BC_CHECK_BATTLE:
      (*ret_wk) = FIELD_WFBC_IsBattlePeople( p_wfbc, objid );
      break;
    // �b�������MMDL OBJID (�t�B�[���h��̓��샂�f�������ʂ���ID)
    case WFBC_GET_PARAM_MMDL_ID:
      (*ret_wk) = mmdlid;
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

//----------------------------------------------------------------------------
/**
 *	@brief  WF  �^�[�Q�b�g�|�P�������莝���Ɏ����Ă��邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_CheckWFTargetPokemon( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA* p_gamedata = SCRCMD_WORK_GetGameData( work );
  FIELD_WFBC_CORE* p_wfbc_core = GAMEDATA_GetMyWFBCCoreData( p_gamedata );
  FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gamedata );
  POKEPARTY * p_party = GAMEDATA_GetMyPokemon( p_gamedata );
  u16 *ret_if_1 = SCRCMD_GetVMWork( core, wk ); // ��1����̃t���O
  u16 *ret_if_2 = SCRCMD_GetVMWork( core, wk ); // ��2����̃t���O
  u16 *ret_temoti   = SCRCMD_GetVMWork( core, wk ); // �莝���C���f�b�N�X
  u32 max = PokeParty_GetPokeCount( p_party );
  int  i;
  POKEMON_PARAM * pp;
  u32 target_monsno;
  u32 monsno;
  u32 mons_getplace;
  u32 mons_year;
  u32 mons_month;
  u32 mons_day;
  BOOL tamago;
  RTCDate now_date;
  u32 mons_get_year;
  u32 mons_get_month;
  u32 mons_get_day;

  // �ړI�̏����擾
  target_monsno = FIELD_WFBC_EVENT_GetWFPokeCatchEventMonsNo( p_event );
  GFL_RTC_GetDate( &now_date );

  //�e�����̏�����
  *ret_if_1 = FALSE;
  *ret_if_2 = FALSE;
  *ret_temoti = 0xffff;
  
  // �莝���̏��ƃ`�F�b�N
  for( i=0; i<max; i++ )
  {
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( p_party, i );

    monsno    = PP_Get( pp, ID_PARA_monsno, NULL );
    mons_year = PP_Get( pp, ID_PARA_birth_year, NULL );
    mons_month= PP_Get( pp, ID_PARA_birth_month, NULL );
    mons_day  = PP_Get( pp, ID_PARA_birth_day, NULL );
    mons_getplace = PP_Get( pp, ID_PARA_birth_place, NULL );
    tamago    = PP_Get( pp, ID_PARA_tamago_flag, NULL );

    mons_get_year = PP_Get( pp, ID_PARA_get_year, NULL );
    mons_get_month= PP_Get( pp, ID_PARA_get_month, NULL );
    mons_get_day  = PP_Get( pp, ID_PARA_get_day, NULL );

    TOMOYA_Printf( "target monsno %d  == monsno %d\n", target_monsno, monsno );
    TOMOYA_Printf( "tamago %d\n", tamago );
    TOMOYA_Printf( "get_place %d == WF%d\n", mons_getplace, MAPNAME_WC10 );
    TOMOYA_Printf( "year%d month%d day%d == now year%d month%d day%d \n", 
        mons_year, mons_month, mons_day, now_date.year, now_date.month, now_date.day );

    TOMOYA_Printf( "tamago year%d month%d day%d\n", 
        mons_get_year, mons_get_month, mons_get_day );
    
    // �ߊl�H
    if( (monsno == target_monsno) && (tamago == FALSE) && 
        (mons_getplace == MAPNAME_WC10) && ((mons_get_year + mons_get_day + mons_get_month) == 0) )
    {
      *ret_if_1 = TRUE;
      if( (*ret_temoti) == 0xffff )
      {  // �ŏ��̃q�b�g�̂�
        (*ret_temoti) = i;
      }
      
      // ��������v�H
      if( (mons_year == now_date.year) && (mons_month == now_date.month) && (mons_day == now_date.day) )
      {
        *ret_if_2 = TRUE;
        (*ret_temoti) = i;
      }
    }
  }

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����z�u�I�u�W�F�N�g �X�N���v�g�p���b�Z�[�W�擾
 */
//-----------------------------------------------------------------------------
#define WFBC_SCRIPT_OBJ_MSG_NUM (4)
VMCMD_RESULT EvCmdWfbc_GetAutoNpcMessage( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fparam->fieldMap );
  FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( fparam->fieldMap );
  WORDSET* pp_wordset = SCRIPT_GetWordSet( sc );
  s16 gx, gy, gz;
  MMDL* p_frontmmdl;
  u32 objid;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_core;
  const FIELD_WFBC_PEOPLE* cp_people;
  u16 *ret = SCRCMD_GetVMWork( core, wk ); // �߂�l
  u16 msg_idx = SCRCMD_GetVMWorkValue( core, wk ); // ���b�Z�[�W�C���f�b�N�X

  // �ڂ̑O�̃O���b�h�擾
  FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );


  // �ڂ̑O�ɂ���l��������
  p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );
  GF_ASSERT( p_frontmmdl );

  // OBJID
  objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );

  msg_idx -= 1;
  GF_ASSERT( msg_idx < WFBC_SCRIPT_OBJ_MSG_NUM );

  // ���b�Z�[�WID�̕Ԃ�
  if( FIELD_WFBC_GetType(p_wfbc) == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    *ret = bc10_01_01 + (WFBC_SCRIPT_OBJ_MSG_NUM*objid) + msg_idx;
  }
  else
  {
    *ret = wc10_01_01 + (WFBC_SCRIPT_OBJ_MSG_NUM*objid) + msg_idx;
  }


	return VMCMD_RESULT_CONTINUE;
}




//----------------------------------------------------------------------------
/**
 *	@brief  �i���p��b�����N��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWfbc_StartPalaceTalk( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  // �C�x���g�N��
  SCRIPT_CallEvent( scw, EVENT_SetUp_WFBC_Palece_Talk(gsys, fieldWork) );

	return VMCMD_RESULT_SUSPEND;
}


//-----------------------------------------------------------------------------
/**
 *    private�֐�
 */
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *    Palace���̉�b
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�C�x���g�V�[�P���X
//=====================================
enum {
  WFBC_PALACE_TALK_SEQ_IS_OBJID_ON,   // �e�̐��E�ɖڂ̑O�̐l������̂��`�F�b�N
  WFBC_PALACE_TALK_SEQ_WAIL_OBJID_ON, // �e�̐��E�ɖڂ̑O�̐l������̂��`�F�b�N�҂�
  WFBC_PALACE_TALK_SEQ_TALK_01,       // ����A��VERSION�̊X�ɂ����񂾂����ł��傤�B
  WFBC_PALACE_TALK_SEQ_IS_MYROMVER,   // ����ROM�o�[�W�������`�F�b�N
  WFBC_PALACE_TALK_SEQ_TALK_02,       // ���̊X�͂܂�Ȃ��E�E�E
  WFBC_PALACE_TALK_SEQ_IS_MYWFBC_CAN_TAKE, // �����̐��E�Ɏ����Ă���邩�`�F�b�N
  WFBC_PALACE_TALK_SEQ_TALK_03,       // ���̂���������
  WFBC_PALACE_TALK_SEQ_TALK_04,       // ��Ă����āI�@YES/NO�\��
  WFBC_PALACE_TALK_SEQ_TALK_05,       // �ꂾ���Ă�[�B�@NO�̏ꍇ
  WFBC_PALACE_TALK_SEQ_IS_OBJID_ON01, // ������x�e�̐��E�ɂ��邩�`�F�b�N
  WFBC_PALACE_TALK_SEQ_WAIT_OBJID_ON01,// ������x�e�̐��E�ɂ��邩�`�F�b�N
  WFBC_PALACE_TALK_SEQ_TALK_06,       // ����ς�ʂ̊X�ɂ����I
  WFBC_PALACE_TALK_SEQ_TALK_08,       // ������[�I�i�l�ړ����ړ��������Ƃ�e�ɒʒm�j
  WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_OUT,        // �}�b�v�̍č\�z �t�F�[�h�A�E�g
  WFBC_PALACE_TALK_SEQ_MAP_RESET_PEOPLE_CHANGE,   // �}�b�v�̍č\�z �t�F�[�h�A�E�g�E�G�C�g�@���@��Ă����l����������
  WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_INWAIT,        // �}�b�v�̍č\�z �t�F�[�h�A�E�g


  WFBC_PALACE_TALK_SEQ_TALK_WAIT,   //���b�Z�[�W�����܂��@

  WFBC_PALACE_TALK_SEQ_END,
} ;

//-------------------------------------
///	��b�������@�V�[�P���X
//=====================================
enum {
  WFBC_PALACE_TALKSYS_SEQ_PRINT,
  WFBC_PALACE_TALKSYS_SEQ_KEYWAIT,
  WFBC_PALACE_TALKSYS_SEQ_YESNO_START,
  WFBC_PALACE_TALKSYS_SEQ_YESNO_WAIT,
  WFBC_PALACE_TALKSYS_SEQ_CLOSE,
  WFBC_PALACE_TALKSYS_SEQ_CLOSE_WAIT,
} ;
enum{
  WFBC_PALACE_TALKMODE_NORMAL,
  WFBC_PALACE_TALKMODE_YESNO,

  WFBC_PALACE_TALKMODE_MAX,
};


//-------------------------------------
///	Palace���̉�b���[�N
//=====================================
typedef struct {
  GAMESYS_WORK* p_gsys;
  FIELDMAP_WORK* p_fieldmap;

  HEAPID heapID;
  
  FLDMSGBG*         p_fmb;
  FLDTALKMSGWIN *   p_talkwin;
  FLDMENUFUNC *     p_yesno;
  GFL_MSGDATA*      p_msgdata;
  WORDSET*          p_wordset;
  STRBUF*           p_strbuf;
  STRBUF*           p_strbuf_tmp;

  FIELD_WFBC* p_wfbc;
  const FIELD_WFBC_PEOPLE* cp_people;
  MMDL*       p_mmdl;
  VecFx32     pos;

  WFBC_COMM_DATA* p_commsys;

  FIELD_WFBC_CORE* p_my;
  FIELD_WFBC_CORE* p_oya;

  u16 npc_id;
  u16 yes_next_seq;
  u16 no_next_seq;

  u16 talk_seq;
  u8  is_yes;
  u8  mode;
  u8  err_check;
  u8  pad[1];
} WFBC_PALACE_TALK_WK;


//-------------------------------------
///	��b���b�Z�[�W���
//=====================================
static const u16 sc_WFBC_PALACE_TALK_START_IDX[FIELD_WFBC_CORE_TYPE_MAX] = 
{
  plbc10_01_01,
  plwc10_01_01,
};
#define WFBC_PALACE_TALK_NPC_MSG_NUM  (8)

// private
static void EVENT_WFBC_WFBC_TalkStart( WFBC_PALACE_TALK_WK* p_wk, u16 palace_msg_idx, int* p_seq, u16 next_seq );
static void EVENT_WFBC_WFBC_TalkStartYesNo( WFBC_PALACE_TALK_WK* p_wk, u16 palace_msg_idx, int* p_seq, u16 yes_next_seq, u16 no_next_seq );
static BOOL EVENT_WFBC_WFBC_TalkMain( WFBC_PALACE_TALK_WK* p_wk );
static BOOL EVENT_WFBC_WFBC_TalkEnd( WFBC_PALACE_TALK_WK* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�@Palace���@�̉�b
 *
 *	@param	p_gsys      �V�X�e��
 *	@param	p_fieldmap  �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------
static GMEVENT* EVENT_SetUp_WFBC_Palece_Talk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*              p_event;
  WFBC_PALACE_TALK_WK*   p_work;
  HEAPID heapID = FIELDMAP_GetHeapID( p_fieldmap );
  FLDMSGBG* p_msgbg = FIELDMAP_GetFldMsgBG( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );

  GAME_COMM_SYS_PTR p_game_comm = GAMESYSTEM_GetGameCommSysPtr(p_gsys);
  INTRUDE_COMM_SYS_PTR p_intcomm = Intrude_Check_CommConnect( p_game_comm );
  WFBC_COMM_DATA * p_wfbc_comm;
  
  if( p_intcomm == NULL ){
    return NULL;
  }
  
  p_wfbc_comm = Intrude_GetWfbcCommData( p_intcomm  );



  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_WFBC_Palece_Talk, sizeof( WFBC_PALACE_TALK_WK ) );

  // �C�x���g���[�N��������
  p_work                  = GMEVENT_GetEventWork( p_event );
  p_work->p_gsys          = p_gsys;
  p_work->p_fieldmap      = p_fieldmap;
  p_work->p_strbuf        = GFL_STR_CreateBuffer( 256, heapID );
  p_work->p_strbuf_tmp    = GFL_STR_CreateBuffer( 256, heapID );
  p_work->p_msgdata       = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_plc10_dat, heapID );
  p_work->p_wordset       = WORDSET_Create( heapID );
  p_work->p_fmb           = p_msgbg;
  p_work->p_commsys       = p_wfbc_comm;
  p_work->err_check       = TRUE;

  p_work->heapID          = GFL_HEAP_LOWID(heapID);

  // WFBC���擾
  p_work->p_my   = GAMEDATA_GetWFBCCoreData( p_gdata, GAMEDATA_WFBC_ID_MINE );
  p_work->p_oya  = GAMEDATA_GetWFBCCoreData( p_gdata, GAMEDATA_WFBC_ID_COMM );

  // �ڂ̑O�̐l�����`�F�b�N
  {
    FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
    FIELD_WFBC* p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
    FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
    const MMDLSYS * cp_mmdlsys = FIELDMAP_GetMMdlSys( p_fieldmap );
    GAMEDATA* p_gamedata = GAMESYSTEM_GetGameData( p_gsys );
    FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gamedata );
    s16 gx, gy, gz;
    MMDL* p_frontmmdl;
    u32 objid;
    const FIELD_WFBC_PEOPLE* cp_people;

    // �ڂ̑O�̃O���b�h�擾
    FIELD_PLAYER_GetFrontGridPos( p_player, &gx, &gy, &gz );

    // �ڂ̑O�ɂ���l��������
    p_frontmmdl = MMDLSYS_SearchGridPos( cp_mmdlsys, gx, gz, FALSE );
    GF_ASSERT( p_frontmmdl );

    // ���W���擾
    MMDL_GetVectorPos( p_frontmmdl, &p_work->pos );

    // OBJID
    objid = FIELD_WFBC_CORE_GetMMdlNpcID( p_frontmmdl );

    // �l�����擾
    cp_people = FIELD_WFBC_GetPeople( p_wfbc, objid );
    GF_ASSERT( cp_people );

    p_work->p_wfbc    = p_wfbc;
    p_work->cp_people = cp_people;
    p_work->p_mmdl    = p_frontmmdl;
    p_work->npc_id    = objid;

    // �����̂ق�����������
  }
  // ���������C�x���g��Ԃ�
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�@Palece�@��b�C�x���g
 *
 *	@param	p_event   �C�x���g�V�X�e��
 *	@param	p_seq     �V�[�P���X
 *	@param	p_wk      ���[�N
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_WFBC_Palece_Talk( GMEVENT* p_event, int* p_seq, void* p_wk )
{
  WFBC_PALACE_TALK_WK*   p_work = p_wk;
  GAME_COMM_SYS_PTR p_game_comm = GAMESYSTEM_GetGameCommSysPtr(p_work->p_gsys);
  INTRUDE_COMM_SYS_PTR p_intcomm = Intrude_Check_CommConnect( p_game_comm );

  // �G���[�`�F�b�N
  if( p_intcomm == NULL ){
    if( p_work->err_check ){
      (*p_seq) = WFBC_PALACE_TALK_SEQ_END;
    }
  }

  switch( (*p_seq) )
  {
  // �e�̐��E�ɖڂ̑O�̐l������̂��`�F�b�N
  case WFBC_PALACE_TALK_SEQ_IS_OBJID_ON:
    FIELD_WFBC_COMM_DATA_ClearReqAnsData( p_work->p_commsys );
    FIELD_WFBC_COMM_DATA_SetReqData( p_work->p_commsys, p_work->npc_id, FIELD_WFBC_COMM_NPC_REQ_THERE );

    (*p_seq) = WFBC_PALACE_TALK_SEQ_WAIL_OBJID_ON;
    break;

  // �e�̐��E�ɖڂ̑O�̐l������̂��`�F�b�N�҂�
  case WFBC_PALACE_TALK_SEQ_WAIL_OBJID_ON:
    if( FIELD_WFBC_COMM_DATA_WaitAnsData( p_work->p_commsys, p_work->npc_id ) )
    {
      if( FIELD_WFBC_GetAnsData( p_work->p_commsys ) == FIELD_WFBC_COMM_NPC_ANS_ON )
      {
        (*p_seq) = WFBC_PALACE_TALK_SEQ_IS_MYROMVER;
      }else{
        (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_01;
      }
    }
    break;

  // ����A��VERSION�̊X�ɂ����񂾂����ł��傤�B
  case WFBC_PALACE_TALK_SEQ_TALK_01:
    EVENT_WFBC_WFBC_TalkStart( p_work, 0, p_seq, WFBC_PALACE_TALK_SEQ_END );
    break;

  // ����ROM�o�[�W�������`�F�b�N
  case WFBC_PALACE_TALK_SEQ_IS_MYROMVER:
    if( p_work->p_my->type != p_work->p_oya->type )
    {
      // OK
      (*p_seq) = WFBC_PALACE_TALK_SEQ_IS_MYWFBC_CAN_TAKE;
    }
    else
    {
      // NG
      (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_02;
    }
    break;

  // ���̊X�͂܂�Ȃ��E�E�E
  case WFBC_PALACE_TALK_SEQ_TALK_02:
    EVENT_WFBC_WFBC_TalkStart( p_work, 1, p_seq, WFBC_PALACE_TALK_SEQ_END );
    break;

  // �����̐��E�Ɏ����Ă���邩�`�F�b�N
  case WFBC_PALACE_TALK_SEQ_IS_MYWFBC_CAN_TAKE:
    // �����̐��E�ɋ󂫂����邩�H
    if( FIELD_WFBC_CORE_GetPeopleNum( p_work->p_my, MAPMODE_NORMAL ) >= FIELD_WFBC_PEOPLE_MAX )
    {
      // �󂫂Ȃ�
      (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_03;
    }
    // ���̐l�����Ȃ��H
    else if( FIELD_WFBC_CORE_IsOnNpcIDPeople( p_work->p_my, p_work->npc_id, MAPMODE_MAX ) )
    {
      // �󂫂Ȃ�
      (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_03;
    }
    else
    {
      // OK
      (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_04;
    }
    break;

  // ���̂���������
  case WFBC_PALACE_TALK_SEQ_TALK_03:
    EVENT_WFBC_WFBC_TalkStart( p_work, 2, p_seq, WFBC_PALACE_TALK_SEQ_END );
    break;

  // ��Ă����āI�@YES/NO�\��
  case WFBC_PALACE_TALK_SEQ_TALK_04:
    EVENT_WFBC_WFBC_TalkStartYesNo( p_work, 3, p_seq, WFBC_PALACE_TALK_SEQ_IS_OBJID_ON01, WFBC_PALACE_TALK_SEQ_TALK_05 );
    break;

  // �ꂾ���Ă�[�B�@NO�̏ꍇ
  case WFBC_PALACE_TALK_SEQ_TALK_05:
    EVENT_WFBC_WFBC_TalkStart( p_work, 4, p_seq, WFBC_PALACE_TALK_SEQ_END );
    break;

  // ������x�e�̐��E�ɂ��邩�`�F�b�N
  case WFBC_PALACE_TALK_SEQ_IS_OBJID_ON01: 
    FIELD_WFBC_COMM_DATA_ClearReqAnsData( p_work->p_commsys );
    FIELD_WFBC_COMM_DATA_SetReqData( p_work->p_commsys, p_work->npc_id, FIELD_WFBC_COMM_NPC_REQ_WISH_TAKE );

    (*p_seq) = WFBC_PALACE_TALK_SEQ_WAIT_OBJID_ON01;
    break;

  // ������x�e�̐��E�ɂ��邩�`�F�b�N
  case WFBC_PALACE_TALK_SEQ_WAIT_OBJID_ON01:
    if( FIELD_WFBC_COMM_DATA_WaitAnsData( p_work->p_commsys, p_work->npc_id ) )
    {
      if( FIELD_WFBC_GetAnsData( p_work->p_commsys ) == FIELD_WFBC_COMM_NPC_ANS_TAKE_OK )
      {
        (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_08;
      }else{
        (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_06;
      }
    }
    break;

  // ����ς�ʂ̊X�ɂ����I
  case WFBC_PALACE_TALK_SEQ_TALK_06:       
    p_work->err_check = FALSE;
    EVENT_WFBC_WFBC_TalkStart( p_work, 5, p_seq, WFBC_PALACE_TALK_SEQ_END );
    break;

  // ������[�I�i�l�ړ����ړ��������Ƃ�e�ɒʒm�j
  case WFBC_PALACE_TALK_SEQ_TALK_08:       
    EVENT_WFBC_WFBC_TalkStart( p_work, 7, p_seq, WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_OUT );


    // �e�ɒʒm
    FIELD_WFBC_COMM_DATA_ClearReqAnsData( p_work->p_commsys );
    FIELD_WFBC_COMM_DATA_SetReqData( p_work->p_commsys, p_work->npc_id, FIELD_WFBC_COMM_NPC_REQ_TAKE );

    // �G���[�`�F�b�N�I��
    p_work->err_check = FALSE;
    break;

  // �}�b�v�̍č\�z �t�F�[�h�A�E�g
  case WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_OUT:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, FIELD_FADE_WAIT);
    (*p_seq) = WFBC_PALACE_TALK_SEQ_MAP_RESET_PEOPLE_CHANGE;
    break;

  // �}�b�v�̍č\�z �t�F�[�h�A�E�g�E�G�C�g�@���@��Ă����l����������
  case WFBC_PALACE_TALK_SEQ_MAP_RESET_PEOPLE_CHANGE:
    if( GFL_FADE_CheckFade() ){
      break;
    }

    {
      const MYSTATUS* cp_mystatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_work->p_gsys) );
      // �l���ڂ��ς���B
      FIELD_WFBC_COMM_DATA_Ko_ChangeNpc( p_work->p_commsys, p_work->p_my, p_work->p_oya, cp_mystatus, p_work->npc_id, p_work->heapID );
    }
    
    // ���̐l������
    {
      FIELD_WFBC_DeletePeople( p_work->p_wfbc, p_work->npc_id );
      MMDL_Delete( p_work->p_mmdl );
    }
    
    // �t�F�[�h�C��
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, FIELD_FADE_WAIT);
    (*p_seq) = WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_INWAIT;
    break;

  case WFBC_PALACE_TALK_SEQ_MAP_RESET_FADE_INWAIT:        // �}�b�v�̍č\�z �t�F�[�h�A�E�g
    if( GFL_FADE_CheckFade() ){
      break;
    }
    (*p_seq) = WFBC_PALACE_TALK_SEQ_END;
    break;

  //���b�Z�[�W�����܂��@
  case WFBC_PALACE_TALK_SEQ_TALK_WAIT:   
    if( EVENT_WFBC_WFBC_TalkMain( p_work ) )
    {
      if( p_work->is_yes ){
        *p_seq = p_work->yes_next_seq;
      }else{
        *p_seq = p_work->no_next_seq;
      }
    }
    break;

  // �I��
  case WFBC_PALACE_TALK_SEQ_END:  

    // ��bwindow������̂��܂�
    if( EVENT_WFBC_WFBC_TalkEnd( p_work ) == FALSE ){
      break;
    }
    
    // ��n��
    {
      GFL_STR_DeleteBuffer( p_work->p_strbuf );
      GFL_STR_DeleteBuffer( p_work->p_strbuf_tmp );

      GFL_MSG_Delete( p_work->p_msgdata );
      WORDSET_Delete( p_work->p_wordset );
    }
    return GMEVENT_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  ��b�J�n
 *
 *	@param	p_wk        ���[�N
 *	@param	msg_id      ���b�Z�[�WID
 *	@param	p_seq       �V�[�P���X�ύX���[�N
 *	@param	next_seq    ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
static void EVENT_WFBC_WFBC_TalkStart( WFBC_PALACE_TALK_WK* p_wk, u16 palace_msg_idx, int* p_seq, u16 next_seq )
{
  u16 msg_id;
  
  GF_ASSERT( p_wk->p_talkwin == NULL );

  msg_id = sc_WFBC_PALACE_TALK_START_IDX[ p_wk->p_oya->type ] + (p_wk->npc_id*WFBC_PALACE_TALK_NPC_MSG_NUM) + palace_msg_idx;

  TOMOYA_Printf( "print msg %d\n", msg_id );

  // ���[�h�Z�b�g���g�p���āA������W�J
  GFL_MSG_GetString( p_wk->p_msgdata, msg_id, p_wk->p_strbuf_tmp );

  WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_strbuf, p_wk->p_strbuf_tmp );

  // �`��J�n
  p_wk->p_talkwin = FLDTALKMSGWIN_AddStrBuf( p_wk->p_fmb, FLDTALKMSGWIN_IDX_AUTO, &p_wk->pos, p_wk->p_strbuf, TALKMSGWIN_TYPE_NORMAL, TAIL_SETPAT_NONE );


  (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_WAIT;
  p_wk->yes_next_seq = next_seq;

  p_wk->talk_seq = 0;
  p_wk->is_yes = TRUE;
  p_wk->mode  = WFBC_PALACE_TALKMODE_NORMAL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��b�J�n
 *
 *	@param	p_wk        ���[�N
 *	@param	msg_id      ���b�Z�[�WID
 *	@param	p_seq       �V�[�P���X�ύX���[�N
 *	@param	yes_next_seqYES�̏ꍇ�̃V�[�P���X
 *	@param	no_next_seq NO�̏ꍇ�̃V�[�P���X
 */
//-----------------------------------------------------------------------------
static void EVENT_WFBC_WFBC_TalkStartYesNo( WFBC_PALACE_TALK_WK* p_wk, u16 palace_msg_idx, int* p_seq, u16 yes_next_seq, u16 no_next_seq )
{
  u16 msg_id;
  
  GF_ASSERT( p_wk->p_talkwin == NULL );

  msg_id = sc_WFBC_PALACE_TALK_START_IDX[ p_wk->p_oya->type ] + (p_wk->npc_id*WFBC_PALACE_TALK_NPC_MSG_NUM) + palace_msg_idx;

  // ���[�h�Z�b�g���g�p���āA������W�J
  GFL_MSG_GetString( p_wk->p_msgdata, msg_id, p_wk->p_strbuf_tmp );

  WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_strbuf, p_wk->p_strbuf_tmp );

  // �`��J�n
  p_wk->p_talkwin = FLDTALKMSGWIN_AddStrBuf( p_wk->p_fmb, FLDTALKMSGWIN_IDX_AUTO, &p_wk->pos, p_wk->p_strbuf, TALKMSGWIN_TYPE_NORMAL, TAIL_SETPAT_NONE );

  (*p_seq) = WFBC_PALACE_TALK_SEQ_TALK_WAIT;
  p_wk->yes_next_seq = yes_next_seq;
  p_wk->no_next_seq = no_next_seq;

  p_wk->talk_seq = 0;
  p_wk->is_yes = TRUE;
  p_wk->mode  = WFBC_PALACE_TALKMODE_YESNO;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�����܂�
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
static BOOL EVENT_WFBC_WFBC_TalkMain( WFBC_PALACE_TALK_WK* p_wk )
{
  switch( p_wk->talk_seq )
  {
  case WFBC_PALACE_TALKSYS_SEQ_PRINT:
    {
      BOOL result;
      result = FLDTALKMSGWIN_Print( p_wk->p_talkwin );
      if( result )
      {
        p_wk->talk_seq ++;
      }
    }
    break;

  case WFBC_PALACE_TALKSYS_SEQ_KEYWAIT:
    if( GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY ){
      if( p_wk->mode == WFBC_PALACE_TALKMODE_NORMAL ){
        // �N���[�Y��
        p_wk->talk_seq = WFBC_PALACE_TALKSYS_SEQ_CLOSE;
      }else{
        // YESNO�`�F�b�N��
        p_wk->talk_seq = WFBC_PALACE_TALKSYS_SEQ_YESNO_START;
      }
    }
    break;
    
  //YESNO�`�F�b�N
  case WFBC_PALACE_TALKSYS_SEQ_YESNO_START:
    p_wk->p_yesno = FLDMENUFUNC_AddYesNoMenu( p_wk->p_fmb, FLDMENUFUNC_YESNO_YES );
    p_wk->talk_seq ++;
    break;

  //YESNO�`�F�b�N�E�G�C�g
  case WFBC_PALACE_TALKSYS_SEQ_YESNO_WAIT:
    {
      FLDMENUFUNC_YESNO result = FLDMENUFUNC_ProcYesNoMenu( p_wk->p_yesno );
      
      if( result != FLDMENUFUNC_YESNO_NULL )
      {
        if( result == FLDMENUFUNC_YESNO_YES ){
          p_wk->is_yes = TRUE;
        }else{
          p_wk->is_yes = FALSE;
        }
        p_wk->talk_seq = WFBC_PALACE_TALKSYS_SEQ_CLOSE;
        FLDMENUFUNC_DeleteMenu( p_wk->p_yesno );
        p_wk->p_yesno = NULL;
      }
    }
    break;

  //�N���[�Y
  case WFBC_PALACE_TALKSYS_SEQ_CLOSE:
    FLDTALKMSGWIN_StartClose(p_wk->p_talkwin);
    p_wk->talk_seq ++;
    break;

  // �I��
  case WFBC_PALACE_TALKSYS_SEQ_CLOSE_WAIT:
    if(FLDTALKMSGWIN_WaitClose(p_wk->p_talkwin))
    {
      p_wk->p_talkwin = NULL;
      return TRUE;
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������b�I��
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static BOOL EVENT_WFBC_WFBC_TalkEnd( WFBC_PALACE_TALK_WK* p_wk )
{
  if( p_wk->p_talkwin ){

    //CLOSE���ɑΏ�
    if( (p_wk->talk_seq == WFBC_PALACE_TALKSYS_SEQ_CLOSE_WAIT)){

      if(!FLDTALKMSGWIN_WaitClose(p_wk->p_talkwin)){
        return FALSE;
      }

      p_wk->p_talkwin = NULL;
    }else{

      FLDTALKMSGWIN_Delete( p_wk->p_talkwin );
      p_wk->p_talkwin = NULL;
    }
  }

  return TRUE;
}



