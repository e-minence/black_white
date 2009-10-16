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
#include "eventdata_local.h"
#include "field/eventdata_sxy.h"


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
  MMDL *dummy;
  MMDL *fmmdl;
  MMDLSYS *fmmdlsys;
  
  fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  
  //�A�����OBJ����ID���n���ꂽ��
  if( obj_id == SCR_OBJID_MV_PAIR ){
    fmmdl = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
  //�����_�~�[OBJ����ID���n���ꂽ��
  }else if( obj_id == SCR_OBJID_DUMMY ){
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    dummy = SCRIPT_GetMemberWork( sc, ID_EVSCR_DUMMY_OBJ );
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
  u8 *num;
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
  
  //�A�j���[�V�����̐��𑫂�
  num = SCRIPT_GetMemberWork( sc, ID_EVSCR_ANMCOUNT );
  (*num)++;
  
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
  
  *ret_wk = MMDL_TOOL_FlipDir( MMDL_GetDirDisp( player ) );

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
  u16 gx = VMGetU16( core );
  u16 gz = VMGetU16( core );
  u16 dir = VMGetU16( core );
  u16 id = VMGetU16( core );
  u16 code = VMGetU16( core );
  u16 move = VMGetU16( core );
  int zone_id = FIELDMAP_GetZoneID( fparam->fieldMap );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  
  mmdl = MMDLSYS_AddMMdlParam(
      mmdlsys, gx, gz, dir, id, code, move, zone_id );
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
  GF_ASSERT( mmdl != NULL && "OBJ DEL �Ώۂ�OBJ�����܂���\n" );
  MMDL_Delete( mmdl );
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
	u16 x		= SCRCMD_GetVMWorkValue(core,work);
	u16 y		= SCRCMD_GetVMWorkValue(core,work);
	u16 z		= SCRCMD_GetVMWorkValue(core,work);
	u16 dir		= SCRCMD_GetVMWorkValue(core,work);
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  
	//�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾
	mmdl = MMDLSYS_SearchOBJID( mmdlsys, obj_id );
  GF_ASSERT( mmdl != NULL && "ERROR OBJ ID" );
  
  MMDL_InitGridPosition( mmdl, x, y, z, dir );
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
  MMDL **fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  MMDL *player = scmd_GetMMdlPlayer( work );
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );

  //���@�����~�`�F�b�N
  if( CheckStepWatchBit(PLAYER_BIT) &&
    MMDL_CheckEndAcmd(player) == TRUE ){
    MMDL_OnStatusBitMoveProcPause( player );
    ResetStepWatchBit(PLAYER_BIT);
  }
  
  //�b�������Ώۓ����~�`�F�b�N
  if( CheckStepWatchBit(OTHER_BIT) &&
    MMDL_CheckStatusBitMove(*fmmdl) == FALSE ){
    MMDL_OnStatusBitMoveProcPause( *fmmdl );
    ResetStepWatchBit(OTHER_BIT);
  }
  
  //���@�̘A����������~�`�F�b�N
  if( CheckStepWatchBit(PLAYER_PAIR_BIT) ){
    MMDL *player_pair = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
    
    if( MMDL_CheckStatusBitMove(player_pair) == FALSE ){
      MMDL_OnStatusBitMoveProcPause( player_pair );
      ResetStepWatchBit(PLAYER_PAIR_BIT);
    }
  }
  
  //�b�������Ώۂ̘A����������~�`�F�b�N
  if( CheckStepWatchBit(OTHER_PAIR_BIT) ){
    #ifndef SCRCMD_PL_NULL
    MMDL *other_pair = FieldOBJ_MovePairSearch(*fldobj);
    if (FieldOBJ_StatusBitCheck_Move(other_pair) == 0) {
      FieldOBJ_MovePause(other_pair);
      ResetStepWatchBit(OTHER_PAIR_BIT);
    }
    #else
    GF_ASSERT( 0 );
    #endif
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
  MMDL **fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  MMDL *player = scmd_GetMMdlPlayer( work );
  MMDL *player_pair = MMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
  
#ifndef SCRCMD_PL_NULL
  MMDL *other_pair = FieldOBJ_MovePairSearch(*fldobj);
#else
  MMDL *other_pair = NULL;
#endif

  InitStepWatchBit();
  MMDLSYS_PauseMoveProc( fmmdlsys );
  
  if( MMDL_CheckEndAcmd(player) == FALSE ){
    SetStepWatchBit(PLAYER_BIT);
    MMDL_OffStatusBitMoveProcPause( player );
  }
  
  if( *fmmdl != NULL ){
    if( MMDL_CheckStatusBitMove(*fmmdl) == TRUE ){
      SetStepWatchBit(OTHER_BIT);
      MMDL_OffStatusBitMoveProcPause( *fmmdl );
    }
  }

  if( player_pair ){
    #ifndef SCRCMD_PL_NULL
    if( SysFlag_PairCheck(SaveData_GetEventWork(fsys->savedata)) == 1
        && FieldOBJ_StatusBitCheck_Move(player_pair) != 0) {
      SetStepWatchBit(PLAYER_PAIR_BIT);
      FieldOBJ_MovePauseClear( player_pair );
    }
    #else
    GF_ASSERT( 0 );
    #endif
  }
  
  if( other_pair ){
    if( MMDL_CheckStatusBitMove(other_pair) == TRUE ){
      SetStepWatchBit(OTHER_PAIR_BIT);
      MMDL_OffStatusBitMoveProcPause( other_pair );
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
  MMDL **fmmdl;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  
  if( (*fmmdl) == NULL ){
    MMDLSYS *fmmdlsys;
    fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
    MMDLSYS_PauseMoveProc( fmmdlsys );
    
    #ifndef SCRCMD_PL_NULL
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
    #endif
  }else{
    EvCmdTalkObjPauseAll( core, wk );
  }
  
  return VMCMD_RESULT_SUSPEND;
}
#endif

VMCMD_RESULT SCRCMD_SUB_ObjPauseAll( VMHANDLE *core, SCRCMD_WORK *work )
{
  SCRIPT_WORK *sc;
  MMDL **fmmdl;
  
  sc = SCRCMD_WORK_GetScriptWork( work );
  fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  
  if( (*fmmdl) == NULL ){
    MMDLSYS *fmmdlsys;
    fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
    MMDLSYS_PauseMoveProc( fmmdlsys );
    
    #ifndef SCRCMD_PL_NULL
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
    #endif
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
  MMDL *jiki,**fmmdl;
  u16 dir;
  
  jiki = scmd_GetMMdlPlayer( work );

  dir = MMDL_GetDirDisp( jiki );
  
  dir = MMDL_TOOL_FlipDir( dir );
  fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
  
  if( (*fmmdl) == NULL ){
    return VMCMD_RESULT_CONTINUE;
  }
  
  MMDL_SetDirDisp( *fmmdl, dir );
  MMDL_SetDrawStatus( *fmmdl, DRAW_STA_STOP );
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
 * ���@MMDL�擾�@��
 * @param work SCRCMD_WORK
 * @retval
 */
//--------------------------------------------------------------
static MMDL * scmd_GetMMdlPlayer( SCRCMD_WORK *work )
{
  MMDLSYS *fmmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *fmmdl = MMDLSYS_SearchOBJID( fmmdlsys, MMDL_ID_PLAYER );
  return( fmmdl );
}

