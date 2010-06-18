//======================================================================
/**
 * @file	scrcmd_fldmmdl.c
 * @brief	�X�N���v�g�R�}���h�F���샂�f���֘A
 * @date  2009.09.13
 * @author	tamada GAMEFREAK inc.
 *
 * 2009.09.13 scrcmd.c��蕪��������
 */
//======================================================================

#include <gflib.h>

#include "system/vm_cmd.h"

#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_fldmmdl.h"

#include "script_def.h" //SCR_OBJID_
#include "script_local.h"
#include "field_player.h" //FIELD_PLAYER
#include "fieldmap.h"     //FIELDMAP_
#include "field/field_const.h"  //GRID_TO_FX32
#include "eventdata_local.h"
#include "field/eventdata_sxy.h"

#include "fieldmap_ctrl_grid.h"

#include "field_task_manager.h"
#include "field_task_player_rot.h"
#include "field_task_player_drawoffset.h"
#include "field_task_player_fall.h"

#include "event_fldmmdl_control.h"
#include "event_rail_slipdown.h"

//======================================================================
//  �c�[���֐�
//======================================================================
static MMDL * scmd_GetMMdlPlayer( SCRCMD_WORK *work );

//======================================================================
///  ���샂�f���Ď��Ɏg�p����C�����C���֐���`
//======================================================================
#define  PLAYER_BIT      (1<<0)
#define  PLAYER_PAIR_BIT    (1<<1)
#define  OTHER_BIT      (1<<2)
#define  OTHER_PAIR_BIT    (1<<3)

static const MMDL_ACMD_LIST anime_up_table[] = {
  AC_STAY_WALK_U_8F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_down_table[] = {
  AC_STAY_WALK_D_8F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_left_table[] = {
  AC_STAY_WALK_L_8F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_right_table[] = {
  AC_STAY_WALK_R_8F, 1,
  ACMD_END, 0
};


static u8 step_watch_bit;

static inline void InitStepWatchBit(void)
{
  step_watch_bit = 0;
}

static inline BOOL CheckStepWatchBit(int mask)
{
  return (step_watch_bit & mask) != 0;
}

static inline void SetStepWatchBit(int mask)
{
  step_watch_bit |= mask;
}

static inline void ResetStepWatchBit(int mask)
{
  step_watch_bit &= (0xff ^ mask);
}

//======================================================================
//  ���샂�f��  
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD_OBJ_PTR���擾
 * @param  fsys  FIELDSYS_WORK�^�̃|�C���^
 * @param  obj_id  OBJID
 * @retval  "FIELD_OBJ_PTR"
 */
//--------------------------------------------------------------
static MMDL * FieldObjPtrGetByObjId( SCRCMD_WORK *work, u16 obj_id )
{
  //MMDL *dummy;
  MMDL *fmmdl;
  MMDLSYS *fmmdlsys;
  
  fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  
  //�A�����OBJ����ID���n���ꂽ��
  if( obj_id == SCR_OBJID_MV_PAIR ){
    fmmdl = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
  //�����_�~�[OBJ����ID���n���ꂽ��
  }else if( obj_id == SCR_OBJID_DUMMY ){
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    //dummy = SCRIPT_GetMemberWork( sc, ID_EVSCR_DUMMY_OBJ );
  //�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾
  }else{
    fmmdl = MMDLSYS_SearchOBJID( fmmdlsys, obj_id );
  }
  
  return fmmdl;
}

//--------------------------------------------------------------
/**
 * ���샂�f���A�j���[�V����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjAnime( VMHANDLE *core, void *wk )
{
  VM_CODE *p;
  GFL_TCB *anm_tcb;
  MMDL *fmmdl; //�Ώۂ̃t�B�[���hOBJ�̃|�C���^
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 obj_id = SCRCMD_GetVMWorkValue(core,work); //obj ID
  u32 pos = VMGetU32(core); //list pos
  
  fmmdl = FieldObjPtrGetByObjId( work, obj_id );
  
  //�G���[�`�F�b�N
  if( fmmdl == NULL ){
    OS_Printf( "obj_id = %d\n", obj_id );
    GF_ASSERT( (0) && "�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾���s�I" );
    return VMCMD_RESULT_CONTINUE;        //08.06.12 �v���`�i�Œǉ�
  }
  
  //�A�j���[�V�����R�}���h���X�g�Z�b�g
  p = (VM_CODE*)(core->adrs+pos);
  anm_tcb = MMDL_SetAcmdList( fmmdl, (MMDL_ACMD_LIST*)p );
  
  //TCB�Z�b�g
  SCRCMD_WORK_SetMMdlAnmTCB( work, anm_tcb );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���샂�f���A�j���[�V�����I���҂��@�E�F�C�ƕ���
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvObjAnimeWait(VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  
  if( SCRCMD_WORK_CheckMMdlAnmTCB(work) == FALSE ){
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjAnimeWait( VMHANDLE * core, void *wk )
{
  VMCMD_SetWait( core, EvObjAnimeWait );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ����R�[�h�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveCodeGet( VMHANDLE *core, void *wk )
{
  MMDL *mmdl;
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  u16 *ret_wk  = SCRCMD_GetVMWork( core, work );
  
  *ret_wk = 0;
  mmdl = MMDLSYS_SearchOBJID( mmdlsys, SCRCMD_GetVMWorkValue(core,work) );
  GF_ASSERT( mmdl != NULL && "SCRCMD GET MOVECODE NON OBJ" );
  *ret_wk = MMDL_GetMoveCode( mmdl );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ����R�[�h�ύX
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveCodeChange( VMHANDLE *core, void *wk )
{
  MMDL *mmdl;
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  u16 id = SCRCMD_GetVMWorkValue( core, work );
  u16 code = SCRCMD_GetVMWorkValue( core, work );
  
  mmdl = MMDLSYS_SearchOBJID( mmdlsys, id );
  GF_ASSERT( mmdl != NULL && "SCRCMD GET MOVECODE NON OBJ" );
  MMDL_ChangeMoveCode( mmdl, code );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * OBJ�̈ʒu���擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjPosGet( VMHANDLE *core, void *wk )
{
  u16 *x,*z;
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID(
      mmdlsys, SCRCMD_GetVMWorkValue(core,work) );
  
  x = SCRCMD_GetVMWork( core, work );
  z = SCRCMD_GetVMWork( core, work );
  *x = MMDL_GetGridPosX( mmdl );
  *z = MMDL_GetGridPosZ( mmdl );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���@�̈ʒu���擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerPosGet( VMHANDLE *core, void *wk )
{
  u16 *x,*z;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fparam->fieldMap );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  
  x = SCRCMD_GetVMWork( core, work );
  z = SCRCMD_GetVMWork( core, work );
  *x = MMDL_GetGridPosX( mmdl );
  *z = MMDL_GetGridPosZ( mmdl );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���@�̌����擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerDirGet( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDL *player = scmd_GetMMdlPlayer( work );

  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  
  *ret_wk = MMDL_GetDirDisp( player );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * OBJ��ǉ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjAdd( VMHANDLE *core, void *wk )
{
  MMDL *mmdl;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  u16 gx = SCRCMD_GetVMWorkValue( core, work );
  u16 gz = SCRCMD_GetVMWorkValue( core, work );
  u16 dir = SCRCMD_GetVMWorkValue( core, work );
  u16 id = SCRCMD_GetVMWorkValue( core, work );
  u16 code = SCRCMD_GetVMWorkValue( core, work );
  u16 move = SCRCMD_GetVMWorkValue( core, work );
  int zone_id = FIELDMAP_GetZoneID( fparam->fieldMap );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
  
  code = MMDL_TOOL_GetWorkOBJCode( evwork, code );
  mmdl = MMDLSYS_AddMMdlParam(
      mmdlsys, gx, gz, dir, id, code, move, zone_id );
  
  if( mmdl != NULL ){ //�ǉ����ꂽOBJ�͑��|�[�Y������
    MMDL_OnMoveBitMoveProcPause( mmdl );
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * OBJ���폜
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjDel( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  u16 id = SCRCMD_GetVMWorkValue( core, work );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, id );
  if ( mmdl == NULL ) {
    GF_ASSERT_MSG( 0, "OBJ DEL �Ώۂ�OBJ(%d)�����܂���\n", id );
  } else {
    MMDL_Delete( mmdl );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * OBJ��ǉ� �z�u�f�[�^����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjAddEvent( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
	u16 count = EVENTDATA_GetNpcCount( evdata );
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  int zone_id = PLAYERWORK_getZoneID( player );
  u16 id = SCRCMD_GetVMWorkValue( core, work );
  
  if( count ){
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
		const MMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
		MMDL *mmdl = MMDLSYS_AddMMdlHeaderID(
        mmdlsys, header, zone_id, count, evwork, id );
    if( mmdl != NULL ){ //�ǉ����ꂽOBJ�͑��|�[�Y������
      MMDL_OnMoveBitMoveProcPause( mmdl );
    }
    GF_ASSERT( mmdl != NULL );
  }else{
    GF_ASSERT( 0 );
  }
  
  return( VMCMD_RESULT_CONTINUE );
}

//--------------------------------------------------------------
/**
 * OBJ���폜�@OBJ�w��C�x���g�t���O��ON��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
#if 0
VMCMD_RESULT EvCmdObjDelEvent( VMHANDLE *core, void *wk )
{
  MMDL *mmdl;
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
  u16 id = SCRCMD_GetVMWorkValue( core, work );
  
  mmdl = MMDLSYS_SearchOBJID( mmdlsys, id );
  GF_ASSERT( mmdl != NULL );
  
  MMDL_DeleteEvent( mmdl, evwork );
  return( VMCMD_RESULT_CONTINUE );
}
#endif

//--------------------------------------------------------------
/**
 * OBJ�f�[�^�̍��W����ύX
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjPosChange( VMHANDLE *core, void *wk )
{
	MMDL *mmdl;
  SCRCMD_WORK *work = wk;
	u16 obj_id	= SCRCMD_GetVMWorkValue( core, work );
	s16 x		= SCRCMD_GetVMWorkValue(core,work);
	s16 y		= SCRCMD_GetVMWorkValue(core,work);
	s16 z		= SCRCMD_GetVMWorkValue(core,work);
	u16 dir		= SCRCMD_GetVMWorkValue(core,work);
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  
	//�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾
	mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  if ( mmdl == NULL ) {
    GF_ASSERT_MSG( 0, "ERROR OBJ ID(%d)", obj_id);
  } else {
    MMDL_InitGridPosition( mmdl, x, y, z, dir );
  }
	return VMCMD_RESULT_CONTINUE;
}

#if 0
//--------------------------------------------------------------
/**
 * SXY�f�[�^�̍��W����ύX
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
static BOOL EvCmdSxyPosChange( VM_MACHINE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 id = SCRCMD_GetVMWorkValue(core,work);
	u16 gx = SCRCMD_GetVMWorkValue(core,work);
	u16 gz = SCRCMD_GetVMWorkValue(core,work);
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
  
	EventData_NpcDataPosChange( core->fsys, id, gx, gz );
	return 0;
}
#endif

//--------------------------------------------------------------
/**
 * OBJ�Ɏw�肳��Ă���C�x���g�t���O���擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjEventFlagGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 obj_id	= SCRCMD_GetVMWorkValue( core, work );
  u16 *ret_wk = SCRCMD_GetVMWork( core, work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  GF_ASSERT( mmdl != NULL && "OBJ GET EVENT FLAG �Ώۂ�OBJ�����܂���\n" );
  
  if( mmdl != NULL ){
    *ret_wk = MMDL_GetEventFlag( mmdl );
  }
  
  return( VMCMD_RESULT_CONTINUE );
}

//--------------------------------------------------------------
/**
 * OBJ�Ɏw�肳��Ă���p�����[�^���擾
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjParamGet( VMHANDLE *core, void *wk )
{
  SDK_COMPILER_ASSERT( SCR_OBJPARAM0 == MMDL_PARAM_0 );
  SDK_COMPILER_ASSERT( SCR_OBJPARAM1 == MMDL_PARAM_1 );
  SDK_COMPILER_ASSERT( SCR_OBJPARAM2 == MMDL_PARAM_2 );
  SCRCMD_WORK *work = wk;
	u16 obj_id	= SCRCMD_GetVMWorkValue( core, work );
  u16 prm_no  = SCRCMD_GetVMWorkValue( core, work );
  u16 *ret_wk = SCRCMD_GetVMWork( core, work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  
  if( prm_no > MMDL_PARAM_2 ) {
    GF_ASSERT_MSG(0, "EvCmdObjParamGet:param no(%d)���傫�����܂�\n", prm_no );
  } else if( mmdl == NULL ){
    GF_ASSERT_MSG(0, "EvCmdObjParamGet:OBJ ID(%d)�����܂���\n", obj_id );
  } else {
    *ret_wk = MMDL_GetParam( mmdl, prm_no );
  }
  return( VMCMD_RESULT_CONTINUE );
}

//--------------------------------------------------------------
/**
 * OBJ���]�[���؂�ւ��ł��폜����Ȃ�OBJ�Ɏw��
 * @param
 * @retval
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjSetNotDelZoneChange( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 obj_id	= SCRCMD_GetVMWorkValue( core, work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  
  if( mmdl != NULL ){
    MMDL_SetStatusBitNotZoneDelete( mmdl, TRUE );
  }else{
    GF_ASSERT( 0 );
  }
      
  return( VMCMD_RESULT_CONTINUE );
}

//======================================================================
//  ���샂�f���@�C�x���g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @brief  OBJ����I���҂��@�E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitTalkObj( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDL *fmmdl = SCRIPT_GetTargetObj( sc );
  MMDL *player = scmd_GetMMdlPlayer( work );
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );

  //���@�����~�`�F�b�N
  if( CheckStepWatchBit(PLAYER_BIT) &&
    MMDL_CheckEndAcmd(player) == TRUE ){
    MMDL_OnMoveBitMoveProcPause( player );
    ResetStepWatchBit(PLAYER_BIT);
  }
  
  //�b�������Ώۓ����~�`�F�b�N
  if( CheckStepWatchBit(OTHER_BIT) &&
    MMDL_CheckMoveBitMove(fmmdl) == FALSE ){
    MMDL_OnMoveBitMoveProcPause( fmmdl );
    ResetStepWatchBit(OTHER_BIT);
  }
  
  //���@�̘A����������~�`�F�b�N
  if( CheckStepWatchBit(PLAYER_PAIR_BIT) ){
    MMDL *player_pair = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
    
    if( MMDL_CheckMoveBitMove(player_pair) == FALSE ){
      MMDL_OnMoveBitMoveProcPause( player_pair );
      ResetStepWatchBit(PLAYER_PAIR_BIT);
    }
  }
  
  //�b�������Ώۂ̘A����������~�`�F�b�N
  if( CheckStepWatchBit(OTHER_PAIR_BIT) ){
    MMDL *other_pair = MMDL_SearchMovePair( fmmdl );
    if( other_pair == NULL ){
      GF_ASSERT( 0 );
      ResetStepWatchBit(OTHER_PAIR_BIT);
    }else{
      if( MMDL_CheckMoveBitMove(other_pair) == FALSE ) {
        MMDL_OnMoveBitMoveProcPause( other_pair );
        ResetStepWatchBit(OTHER_PAIR_BIT);
      }
    }
  }
  
  if( step_watch_bit == 0 ){
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ��b�C�x���g�p�SOBJ�����~
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTalkObjPauseAll( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *fmmdl = SCRIPT_GetTargetObj( sc );
  MMDL *player = scmd_GetMMdlPlayer( work );
  MMDL *player_pair = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
  MMDL *other_pair = MMDL_SearchMovePair( fmmdl );
  
  InitStepWatchBit();
  MMDLSYS_PauseMoveProc( fmmdlsys );
  
  if( MMDL_CheckEndAcmd(player) == FALSE ){
    SetStepWatchBit(PLAYER_BIT);
    MMDL_OffMoveBitMoveProcPause( player );
  }
  
  if( fmmdl != NULL ){
    if( MMDL_CheckMoveBitMove(fmmdl) == TRUE ){
      SetStepWatchBit(OTHER_BIT);
      MMDL_OffMoveBitMoveProcPause( fmmdl );
    }
  }

  if( player_pair ){
    GF_ASSERT( 0 );
  }
  
  if( other_pair ){
    if( MMDL_CheckMoveBitMove(other_pair) == TRUE ){
      SetStepWatchBit(OTHER_PAIR_BIT);
      MMDL_OffMoveBitMoveProcPause( other_pair );
    }
  }
  
  VMCMD_SetWait( core, EvWaitTalkObj );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �SOBJ�����~
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
#if 0
static VMCMD_RESULT EvCmdObjPauseAll( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc;
  MMDL *fmmdl;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  fmmdl = SCRIPT_GetTargetObj( sc );
  
  if( (fmmdl) == NULL ){
    MMDLSYS *fmmdlsys;
    fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
    MMDLSYS_PauseMoveProc( fmmdlsys );
    
    //08.06.18
    //�b�������̑Ώۂ����Ȃ�BG��POS�̎�
    //�A�����OBJ�̈ړ����쒆���̃`�F�b�N�����Ă��Ȃ�
    //
    //�ӂꂠ���L��ȂǂŁA�A�����OBJ�ɑ΂��āA
    //�X�N���v�g�ŃA�j���𔭍s����ƁA
    //�A�j�����s��ꂸ�I���҂��ɂ����Ȃ��Ń��[�v���Ă��܂�
    
    {
      FIELD_OBJ_PTR player_pair =
        FieldOBJSys_MoveCodeSearch( fsys->fldobjsys, MV_PAIR );
      //�y�A�����݂��Ă���
      if (player_pair) {
        //�A������t���O�������Ă��āA�ړ����쒆�Ȃ�
        if( SysFlag_PairCheck(
          SaveData_GetEventWork(fsys->savedata)) == 1
          && FieldOBJ_StatusBitCheck_Move(player_pair) != 0) {
          
          //�y�A�̓���|�[�Y����
          FieldOBJ_MovePauseClear( player_pair );
          
          //�ړ�����̏I���҂����Z�b�g
          VM_SetWait( core, EvWaitPairObj );
          return VMCMD_RESULT_SUSPEND;
        }
      }
    }
  }else{
    EvCmdTalkObjPauseAll( core, wk );
  }
  
  return VMCMD_RESULT_SUSPEND;
}
#endif

VMCMD_RESULT SCRCMD_SUB_ObjPauseAll( VMHANDLE *core, SCRCMD_WORK *work )
{
  SCRIPT_WORK *sc;
  MMDL *fmmdl;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  fmmdl = SCRIPT_GetTargetObj( sc );
  
  if( (fmmdl) == NULL ){
    MMDLSYS *fmmdlsys;
    fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
    MMDLSYS_PauseMoveProc( fmmdlsys );
  }else{
    return( EvCmdTalkObjPauseAll(core,work) );
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �SOBJ����ĊJ
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
#if 0
static VMCMD_RESULT EvCmdObjPauseClearAll( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDLSYS_ClearPauseMoveProc( fmmdlsys );
  return VMCMD_RESULT_SUSPEND;
}
#endif
void SCRCMD_SUB_PauseClearAll( SCRCMD_WORK *work )
{
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDLSYS_ClearPauseMoveProc( fmmdlsys );
}

//--------------------------------------------------------------
/**
 * �b��������OBJ���@�����ւ̐U�����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdObjTurn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDL *jiki,*fmmdl;
  u16 dir;
  
  jiki = scmd_GetMMdlPlayer( work );

  dir = MMDL_GetDirDisp( jiki );
  
  dir = MMDL_TOOL_FlipDir( dir );
  fmmdl = SCRIPT_GetTargetObj( sc );
  
  if( fmmdl == NULL ){
    GF_ASSERT( 0 );
  } else {
    MMDL_SetDirDisp( fmmdl, dir );
    MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
  }
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief �w��ʒu��OBJ����������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetObjID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  u16 gx, gy, gz;
  u16 *ret_id     = SCRCMD_GetVMWork( core, work );
  u16 *ret_valid  = SCRCMD_GetVMWork( core, work );
  MMDL *mmdl;
  u16 dir;
  VecFx32 pos;
  fx32 fy;
  
  gx = SCRCMD_GetVMWorkValue( core, work );
  gy = SCRCMD_GetVMWorkValue( core, work );
  gz = SCRCMD_GetVMWorkValue( core, work );
  fy = GRID_TO_FX32( gy );
  mmdl = MMDLSYS_SearchGridPosEx(
      FIELDMAP_GetMMdlSys(fieldmap), gx, gz, fy, GRID_HALF_FX32, FALSE );
  if ( mmdl )
  {
    *ret_valid = TRUE;
    *ret_id    = MMDL_GetOBJID( mmdl );
  }
  else
  {
    *ret_valid = FALSE;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �v���C���[�̑O��1�O���b�h�ɂ���OBJ����������
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetFrontObjID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_PLAYER*    player = FIELDMAP_GetFieldPlayer( fieldmap );
  u16*             ret_id = SCRCMD_GetVMWork( core, work );  // �R�}���h������(�I�u�W�FID�̊i�[��)
  u16*          ret_valid = SCRCMD_GetVMWork( core, work );  // �R�}���h������(�I�u�W�F�̗L��)
  MMDL* mmdl;

  // �O���ɂ��铮�샂�f�����擾
  mmdl = FIELD_PLAYER_GetFrontMMdlEx( player, GRID_HALF_FX32 );
  // ���ʂ��i�[
  if( mmdl )
  {
    *ret_valid = TRUE;
    *ret_id    = MMDL_GetOBJID( mmdl );
  }
  else
  {
    *ret_valid = FALSE;
  }
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  ���@
//======================================================================
//--------------------------------------------------------------
/**
 * ���@���N�G�X�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerRequest( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  u16 req = VMGetU16( core );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  FIELD_PLAYER* fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  
  FIELD_PLAYER_SetRequest( fld_player, req );
  FIELD_PLAYER_UpdateRequest( fld_player );
  FIELD_PLAYER_ForceWaitVBlank( fld_player ); //BTS5723 100616
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���@�̏㉺�����o���N�G�X�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerUpDownEffect( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  FIELD_TASK* move;
  FIELD_TASK_MAN* man = FIELDMAP_GetTaskManager( fieldmap );
  VecFx32 vec;
  u16 type = SCRCMD_GetVMWorkValue( core, work );
  u16 frame_param = SCRCMD_GetVMWorkValue( core, work );
  u16 length = SCRCMD_GetVMWorkValue( core, work );
  u16 way = SCRCMD_GetVMWorkValue( core, work );
  int frame;
  if (type == 0) {
    frame = frame_param;
  } else {
    frame = - frame_param;
  }

  if(way == 0){
    VEC_Set( &vec, 0, (- length) <<FX32_SHIFT, 0 );
  }else{
    VEC_Set( &vec, 0, (length) <<FX32_SHIFT, 0 );
  }
  //move     = FIELD_TASK_TransDrawOffset( fieldmap, frame, &vec );
  {
    FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( fieldmap );
    move     = FIELD_TASK_TransPos( fieldmap, frame, &vec, FIELD_PLAYER_GetMMdl( player ) );
  }
  FIELD_TASK_MAN_AddTask( man, move, NULL );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���@�̏㉺�����N�G�X�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerUpDownPos( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  FIELD_TASK* move;
  FIELD_TASK_MAN* man = FIELDMAP_GetTaskManager( fieldmap );
  VecFx32 vec;
  u16 type = SCRCMD_GetVMWorkValue( core, work );
  u16 frame_param = SCRCMD_GetVMWorkValue( core, work );
  u16 length = SCRCMD_GetVMWorkValue( core, work );
  u16 way = SCRCMD_GetVMWorkValue( core, work );
  int frame;
  if (type == 0) {
    frame = frame_param;
  } else {
    frame = - frame_param;
  }

  if(way == 0){
    VEC_Set( &vec, 0, (- length) <<FX32_SHIFT, 0 );
  }else{
    VEC_Set( &vec, 0, (length) <<FX32_SHIFT, 0 );
  }
  //move     = FIELD_TASK_TransDrawOffset( fieldmap, frame, &vec );
  {
    FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( fieldmap );
    move     = FIELD_TASK_TransPos( fieldmap, frame, &vec, FIELD_PLAYER_GetMMdl( player ) );
  }
  FIELD_TASK_MAN_AddTask( man, move, NULL );

  return VMCMD_RESULT_CONTINUE;
}



//======================================================================
//
//
//    �c�[���֐�
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * ���@MMDL�擾
 * @param work SCRCMD_WORK
 * @retval
 */
//--------------------------------------------------------------
static MMDL * scmd_GetMMdlPlayer( SCRCMD_WORK *work )
{
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *fmmdl = MMDLSYS_SearchMMdlPlayer( fmmdlsys );
  return( fmmdl );
}

//--------------------------------------------------------------
/**
 * �����N���o�n�r�p�������菈��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPosPlayerTurn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  u16 dir;
  {
    int i;
    VecFx32 pos;
    FIELD_PLAYER *fld_player;
    u16 player_dir;

    u16 dir_base[4] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};

    EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
    EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );

    fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    for (i=0;i<4;i++)
    {
      const POS_EVENT_DATA *evt_data = EVENTDATA_GetPosEvent(
          evdata, evwork, &pos, dir_base[i] );
      if ( evt_data != NULL )
      {
        dir = dir_base[i];
        break;
      }
    }
  
    //���@�̌����ڂ̌������擾
    player_dir = FIELD_PLAYER_GetDir( fld_player );
    //�����̔�r������
    if (player_dir != dir)
    {
      GMEVENT * child;
      void *tbl;
      if ( dir == DIR_UP ){
        tbl = (void*)anime_up_table;
      }else if (dir == DIR_DOWN){
        tbl = (void*)anime_down_table;
      }else if (dir == DIR_LEFT){
        tbl = (void*)anime_left_table;
      }else if (dir == DIR_RIGHT){
        tbl = (void*)anime_right_table;
      }else{
        GF_ASSERT(0);
        return VMCMD_RESULT_CONTINUE;
      }
      child = EVENT_ObjAnime( gsys, fieldmap, MMDL_ID_PLAYER, tbl );
      SCRIPT_CallEvent( sc, child );
      return VMCMD_RESULT_SUSPEND;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���@�̓���`�Ԃ��擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPlayerFrom( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  u16 *ret_wk = SCRCMD_GetVMWork( core, work );
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    *ret_wk = SCR_PLAYER_FORM_NORMAL;
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    *ret_wk = SCR_PLAYER_FORM_CYCLE;
    break;
  case PLAYER_MOVE_FORM_SWIM:
    *ret_wk = SCR_PLAYER_FORM_SWIM;
    break;
  case PLAYER_MOVE_FORM_DIVING:
    *ret_wk = SCR_PLAYER_FORM_DIVING;
    break;
  default:
    GF_ASSERT( 0 );
    *ret_wk = SCR_PLAYER_FORM_NORMAL;
  }
  
  return( VMCMD_RESULT_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �n�C�W�����v
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdObjHighJump( VMHANDLE *core, void *wk )
{
	MMDL *mmdl;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 obj_id	= SCRCMD_GetVMWorkValue( core, work );
	u16 gx	= SCRCMD_GetVMWorkValue( core, work );
	u16 gy	= SCRCMD_GetVMWorkValue( core, work );
	u16 gz	= SCRCMD_GetVMWorkValue( core, work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  VecFx32 start, end;
  
	//�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾
	mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  GF_ASSERT( mmdl != NULL && "ERROR OBJ ID" );

  // ���̍��W�ƏI�����W�����߂�
  MMDL_GetVectorPos( mmdl, &start );
  end.x = GRID_TO_FX32( gx ) + MMDL_VEC_X_GRID_OFFS_FX32;
  end.y = GRID_TO_FX32( gy );
  end.z = GRID_TO_FX32( gz ) + MMDL_VEC_Z_GRID_OFFS_FX32;
  
  SCRIPT_CallEvent( sc, EVENT_HighJump(gsys, mmdl, &start, &end) );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���@�̕\���R�[�h�����@��p�̂��̂��ǂ����B
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk
 * @retval  VMCMD_RESULT
 * @note �ϑ������ǂ����̃`�F�b�N
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPlayerIllegalOBJCode( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  u16 *ret_wk = SCRCMD_GetVMWork( core, work );
  
  *ret_wk = FIELD_PLAYER_CheckIllegalOBJCode( fld_player );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  rail�}�b�v��p
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �v���C���[�̃��[���ʒu�����擾
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerRailLocationGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDL* mmdl = scmd_GetMMdlPlayer( work );
  RAIL_LOCATION rail_location;
  FIELD_RAIL_WORK* p_railWork;
  u16 *index, *ofs, *w_ofs;


  // ���[���̃I�u�W�F���H
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���P�[�V������Ԃ�
  index = SCRCMD_GetVMWork( core, work );
  ofs = SCRCMD_GetVMWork( core, work );
  w_ofs = SCRCMD_GetVMWork( core, work );

  p_railWork = MMDL_GetRailWork( mmdl );

  FIELD_RAIL_WORK_GetNotMinusRailParam( p_railWork, index, ofs, w_ofs );
  
  return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���샂�f���̃��[���ʒu�����擾
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdObjRailLocationGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID(
      mmdlsys, SCRCMD_GetVMWorkValue(core,work) );
  RAIL_LOCATION rail_location;
  FIELD_RAIL_WORK* p_railWork;
  u16 *index, *ofs, *w_ofs;


  // ���[���̃I�u�W�F���H
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���P�[�V������Ԃ�
  index = SCRCMD_GetVMWork( core, work );
  ofs = SCRCMD_GetVMWork( core, work );
  w_ofs = SCRCMD_GetVMWork( core, work );

  p_railWork = MMDL_GetRailWork( mmdl );

  FIELD_RAIL_WORK_GetNotMinusRailParam( p_railWork, index, ofs, w_ofs );
  
  return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���샂�f���@����~��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdObjRailSlipDown( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID(
      mmdlsys, SCRCMD_GetVMWorkValue(core,work) );
  SCRIPT_FLDPARAM *fldparam = SCRIPT_GetFieldParam( sc );
  FIELDMAP_WORK *fieldmap = fldparam->fieldMap;

  SCRIPT_CallEvent( sc, EVENT_RailSlipDownObj(gsys, fieldmap, mmdl) );
  return VMCMD_RESULT_SUSPEND;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����P�[�V�����̐ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerRailLocationSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDL* mmdl = scmd_GetMMdlPlayer( work );
  RAIL_LOCATION rail_location;
  FIELD_RAIL_WORK* p_railWork;
  u16 index, ofs, w_ofs;
  RAIL_LOCATION location;


  // ���[���̃I�u�W�F���H
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���P�[�V������Ԃ�
  index = SCRCMD_GetVMWorkValue( core, work );
  ofs = SCRCMD_GetVMWorkValue( core, work );
  w_ofs = SCRCMD_GetVMWorkValue( core, work );

  p_railWork = MMDL_GetRailWork( mmdl );

  FIELD_RAIL_WORK_SetNotMinusRailParam( p_railWork, index, ofs, w_ofs );
  FIELD_RAIL_WORK_GetLocation( p_railWork, &location );
  MMDL_SetRailLocation( mmdl, &location );
  
  return VMCMD_RESULT_CONTINUE;
}
VMCMD_RESULT EvCmdObjRailLocationSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl = MMDLSYS_SearchOBJID(
      mmdlsys, SCRCMD_GetVMWorkValue(core,work) );
  RAIL_LOCATION rail_location;
  FIELD_RAIL_WORK* p_railWork;
  u16 index, ofs, w_ofs;
  RAIL_LOCATION location;


  // ���[���̃I�u�W�F���H
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���P�[�V������Ԃ�
  index = SCRCMD_GetVMWorkValue( core, work );
  ofs = SCRCMD_GetVMWorkValue( core, work );
  w_ofs = SCRCMD_GetVMWorkValue( core, work );

  p_railWork = MMDL_GetRailWork( mmdl );

  FIELD_RAIL_WORK_SetNotMinusRailParam( p_railWork, index, ofs, w_ofs );
  FIELD_RAIL_WORK_GetLocation( p_railWork, &location );
  MMDL_SetRailLocation( mmdl, &location );
  
  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���[�v�ɂ�����
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdObjWarpOut( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*     work     = wk;
  SCRIPT_WORK*     script   = SCRCMD_WORK_GetScriptWork( work );
  u16              obj_id   = SCRCMD_GetVMWorkValue( core, work ); // �R�}���h������: OBJID
  MMDLSYS*         mmdlsys  = SCRCMD_WORK_GetMMdlSys( work );
  MMDL*            mmdl     = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  SCRIPT_FLDPARAM* fldparam = SCRIPT_GetFieldParam( script );
  FIELDMAP_WORK*   fieldmap = fldparam->fieldMap;
  FIELD_TASK_MAN*  taskMan  = FIELDMAP_GetTaskManager( fieldmap );

  VecFx32 vec;
  FIELD_TASK* moveTask;
  FIELD_TASK* rotTask; 

  GF_ASSERT( mmdl ); // �����G���[: �w�肳�ꂽOBJID�͑��݂��Ȃ�

  PMSND_PlaySE( SEQ_SE_FLD_05 ); // ���[�v��

  // �^�X�N�𐶐�
  VEC_Set( &vec, 0, 150<<FX32_SHIFT, 0 );
  moveTask = FIELD_TASK_TransDrawOffsetEX( fieldmap, 24, &vec, mmdl );
  rotTask  = FIELD_TASK_PlayerRotateEX( fieldmap, 24, 3, mmdl );

  // �^�X�N��o�^
  FIELD_TASK_MAN_AddTask( taskMan, rotTask, NULL );
  FIELD_TASK_MAN_AddTask( taskMan, moveTask, NULL ); 

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���샂�f���̗����ɂ��o��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdObjFallIn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*     work     = wk;
  SCRIPT_WORK*     script   = SCRCMD_WORK_GetScriptWork( work );
  u16              obj_id   = SCRCMD_GetVMWorkValue( core, work ); // �R�}���h������: OBJID
  u16              grid_x   = SCRCMD_GetVMWorkValue( core, work ); // �R�}���h������: x ���W ( �O���b�h�P�� )
  u16              grid_z   = SCRCMD_GetVMWorkValue( core, work ); // �R�}���h��O����: z ���W ( �O���b�h�P�� )
  u16              dir      = SCRCMD_GetVMWorkValue( core, work ); // �R�}���h��l����: ����
  MMDLSYS*         mmdlsys  = SCRCMD_WORK_GetMMdlSys( work );
  MMDL*            mmdl     = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  SCRIPT_FLDPARAM* fldparam = SCRIPT_GetFieldParam( script );
  FIELDMAP_WORK*   fieldmap = fldparam->fieldMap;
  FIELD_TASK_MAN*  taskMan  = FIELDMAP_GetTaskManager( fieldmap );

	// �����G���[: �w�肳�ꂽOBJID�͑��݂��Ȃ�
	if( mmdl == NULL ) {
		GF_ASSERT(0); 
		return VMCMD_RESULT_CONTINUE;
	}

	// ���샂�f���𗎉��n�_�Ɉړ�
	{
		VecFx32 pos;
		fx32 y = 0;
		s16 grid_y;

		pos.x = GRID_TO_FX32( grid_x ) + GRID_HALF_FX32; // �O���b�h�̒��S
		pos.y = 250 << FX32_SHIFT;
		pos.z = GRID_TO_FX32( grid_z ) + GRID_HALF_FX32; // �O���b�h�̒��S
		MMDL_GetMapPosHeight( mmdl, &pos, &y );
		pos.y = y;
		grid_y = SIZE_GRID_FX32( pos.y );
		MMDL_InitGridPosition( mmdl, grid_x, grid_y, grid_z, dir );
		MMDL_SetVectorPos( mmdl, &pos );
	}

	// ���샂�f���̕`��I�t�Z�b�g�������ݒ� ( ��ʊO�ɂ���悤�ɂ��� )
	{
		VecFx32 offset = { 0, 250<<FX32_SHIFT, 0 };
		MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
	}

  // �^�X�N��o�^
	{
		FIELD_TASK* task;

		task = FIELD_TASK_PlayerFall( fieldmap, mmdl, 40, 250 ); 
		FIELD_TASK_MAN_AddTask( taskMan, task, NULL );
	}

  return VMCMD_RESULT_CONTINUE;
}
