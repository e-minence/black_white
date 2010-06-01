//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_rebattle_trainer.c
 *	@brief  �Đ�g���[�i�[�p�X�N���v�g
 *	@author	tomoya takahashi
 *	@date		2010.03.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "system/main.h"

#include "arc/arc_def.h"
#include "arc/fieldmap/rebattle_trainer.naix"

#include "item/itemsym.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "script_trainer.h"


#include "fieldmap.h"

#include "scrcmd_rebattle_trainer.h"

#include "rebattle_trainer.h"

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
//-------------------------------------
///	�Đ�g���[�i�[���
//=====================================
typedef struct {
  u16 id;
  u16 badge_num;
  u16 code;
  u16 trainer_id;
} REBATTLE_TRAINER_DATA;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static u32 ReBattle_SearchData( const REBATTLE_TRAINER_DATA* cp_data, u32 id, u32 badge_num );

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[�����m��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  HEAPID heapID = HEAPID_PROC;  // �e���|����
  void* p_data;

  p_data = GFL_ARC_UTIL_Load( ARCID_REBATTLE_TRAINER, 0, FALSE, GFL_HEAP_LOWID(heapID) );

  // �P�O�ō폜���Ă��Ȃ�
  GF_ASSERT( SCRIPT_GetReBattleTrainerData(sc) == NULL );
  SCRIPT_SetReBattleTrainerData( sc, p_data );

  TOMOYA_Printf( "�Đ�g���[�i�[�����m��\n" );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[����j��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void* p_data;

  p_data = SCRIPT_GetReBattleTrainerData(sc);
  if( p_data ){
    GFL_HEAP_FreeMemory( p_data );
    SCRIPT_SetReBattleTrainerData( sc, NULL );

    TOMOYA_Printf( "�Đ�g���[�i�[����j��\n" );
  }

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[�@�g���[�i�[ID�ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerSetUpTrID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 rebattle_id = SCRCMD_GetVMWorkValue( core, work );
  u16 objid = SCRCMD_GetVMWorkValue( core, work );
  u16 badge = SCRCMD_GetVMWorkValue( core, work );
  u16 index;
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( SCRCMD_WORK_GetGameData(work) );
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );

  index = ReBattle_SearchData( cp_data, rebattle_id, badge );


  TOMOYA_Printf( "�ݒ���@TrainerID[%d]\n", cp_data[index].trainer_id );

  // �g���[�i�[ID�̎����X�N���v�g�ݒ�
  EVENTDATA_ChangeNPCEventID( evdata, objid, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[index].trainer_id ) );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[�@�_�u���o�g�� �g���[�i�[ID�ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerSetUp2vs2TrID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 rebattle_id = SCRCMD_GetVMWorkValue( core, work );
  u16 objid0 = SCRCMD_GetVMWorkValue( core, work );
  u16 objid1 = SCRCMD_GetVMWorkValue( core, work );
  u16 badge = SCRCMD_GetVMWorkValue( core, work );
  u16 index;
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( SCRCMD_WORK_GetGameData(work) );
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );

  index = ReBattle_SearchData( cp_data, rebattle_id, badge );


  TOMOYA_Printf( "�ݒ���@TrainerID[%d]\n", cp_data[index].trainer_id );

  // �g���[�i�[ID�̎����X�N���v�g�ݒ�
  EVENTDATA_ChangeNPCEventID( evdata, objid0, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[index].trainer_id ) );
  EVENTDATA_ChangeNPCEventID( evdata, objid1, SCRIPT_GetTrainerScriptID_By2vs2TrainerID( cp_data[index].trainer_id ) );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[�@�����_���g���[�i�[�̐ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerSetUpRndTr( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 objid0 = SCRCMD_GetVMWorkValue( core, work );
  u16 objid1 = SCRCMD_GetVMWorkValue( core, work );
  u16 objid2 = SCRCMD_GetVMWorkValue( core, work );
  u16 index0 = SCRCMD_GetVMWorkValue( core, work );
  u16 index1 = SCRCMD_GetVMWorkValue( core, work );
  u16 index2 = SCRCMD_GetVMWorkValue( core, work );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( SCRCMD_WORK_GetGameData(work) );
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );

  GF_ASSERT( index0 < RB_DATA_TBL_MAX );
  GF_ASSERT( index1 < RB_DATA_TBL_MAX );
  GF_ASSERT( index2 < RB_DATA_TBL_MAX );
  
  TOMOYA_Printf( "�ݒ��� 0�@OBJCode[%d] TrainerID[%d]\n", cp_data[ index0 ].code, cp_data[ index0 ].trainer_id );
  TOMOYA_Printf( "�ݒ��� 1�@OBJCode[%d] TrainerID[%d]\n", cp_data[ index1 ].code, cp_data[ index1 ].trainer_id );
  TOMOYA_Printf( "�ݒ��� 2�@OBJCode[%d] TrainerID[%d]\n", cp_data[ index2 ].code, cp_data[ index2 ].trainer_id );

  // �����ڐݒ�
  EVENTDATA_ChangeNPCObjCode( evdata, objid0, cp_data[ index0 ].code );
  EVENTDATA_ChangeNPCObjCode( evdata, objid1, cp_data[ index1 ].code );
  EVENTDATA_ChangeNPCObjCode( evdata, objid2, cp_data[ index2 ].code );
  
  // �g���[�i�[ID��ݒ�
  EVENTDATA_ChangeNPCEventID( evdata, objid0, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ index0 ].trainer_id ) );
  EVENTDATA_ChangeNPCEventID( evdata, objid1, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ index1 ].trainer_id ) );
  EVENTDATA_ChangeNPCEventID( evdata, objid2, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ index2 ].trainer_id ) );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �S�g���[�i�[ID�̍Đ�t���O���N���A
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerClearTrEventFlag( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  int i;
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );


  // ���ׂăN���A
  for( i=0; i<RB_DATA_TBL_MAX; i++ ){
    SCRIPT_ResetEventFlagTrainer( evwork, cp_data[i].trainer_id );
  }
  
	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief�@�Đ�g���[�i�[�@�����_��NPC�C���f�b�N�X�̒��I
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerGetRndTrIndex( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 *ret_index0	= SCRCMD_GetVMWork( core, work );
  u16 *ret_index1	= SCRCMD_GetVMWork( core, work );
  u16 *ret_index2	= SCRCMD_GetVMWork( core, work );
  u16 *ret_index3	= SCRCMD_GetVMWork( core, work );
  u16 *ret_index4	= SCRCMD_GetVMWork( core, work );
  u16 *ret_index5	= SCRCMD_GetVMWork( core, work );
  u16 random_max = RB_DATA_TBL_MAX - RB_RANDOM_START;
  int i, j, k;
  u16 index[6] = {0};
  s16 data_index;
  BOOL setup;

  // ���Ԃ�Ȃ��悤�ɗ����ݒ�
  for( i=0; i<6; i++ ){

    setup = FALSE;

    // ���I
    data_index = GFUser_GetPublicRand( random_max );

    // ���Ԃ�Ȃ����l�̐擪����data_index�ڂ̐��l���g�p
    for( j=0; j<(RB_DATA_TBL_MAX - RB_RANDOM_START); j++ ){

      // ���Ԃ�`�F�b�N
      for( k=0; k<i; k++ ){
        if( index[k] == j ){
          // �X�L�b�v
          break;
        }
      }

      if( k==i ){ // ���Ԃ薳���Ȃ�Adata_index���ւ炵�A�}�C�i�X�ɂȂ���NPC���g�p����B
        data_index --;
        if( data_index < 0 ){
          index[i] = j;
          setup = TRUE;
          break;
        }
      }
    }

    GF_ASSERT( setup );

    // 1�̒��I����͂���
    random_max--;
  }

  *ret_index0 = index[0] + RB_RANDOM_START;
  *ret_index1 = index[1] + RB_RANDOM_START;
  *ret_index2 = index[2] + RB_RANDOM_START;

  *ret_index3 = index[3] + RB_RANDOM_START;
  *ret_index4 = index[4] + RB_RANDOM_START;
  *ret_index5 = index[5] + RB_RANDOM_START;

	return VMCMD_RESULT_CONTINUE;
}



//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�f�[�^�̌���
 *
 *	@param	cp_data     �f�[�^
 *	@param	id          ID
 *	@param	badge_num   �o�b�W��
 *
 *	@return �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 ReBattle_SearchData( const REBATTLE_TRAINER_DATA* cp_data, u32 id, u32 badge_num )
{
  int i;

  for( i=0; i<RB_DATA_TBL_MAX; i++ ){

    if( (cp_data[i].id == id) && 
        (cp_data[i].badge_num == badge_num) ){
      return i;
    }
  }

  OS_TPrintf( "�Đ�Data id = %d  level = %d\n", id, badge_num );
  GF_ASSERT_MSG(0, "�Đ�f�[�^�Ȃ�");

  return 0;
}



