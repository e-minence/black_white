//======================================================================
/**
 * @file  field_comm_actor.c
 * @brief  
 * @author
 * @date
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_comm_actor.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_COMM_ACTOR
//--------------------------------------------------------------
typedef struct
{
  u32 id;
  u32 del_flag;
  MMDL *fmmdl;
}FIELD_COMM_ACTOR;

//--------------------------------------------------------------
/// FIELD_COMM_ACTOR_CTRL
//--------------------------------------------------------------
struct _TAG_FIELD_COMM_ACTOR_CTRL
{
  int max;
  HEAPID heapID;
  MMDLSYS *fmmdlsys;
  FIELD_COMM_ACTOR *act_tbl;
};

//--------------------------------------------------------------
/// MV_COMMACT_WORK
//--------------------------------------------------------------
typedef struct
{
  const u16 *watch_dir;
  const VecFx32 *watch_pos;
  const BOOL *watch_vanish;
  FIELD_COMM_ACTOR *comm_actor;
}MV_COMMACT_WORK;

//======================================================================
//  proto
//======================================================================
static void fldcommAct_DeleteActor( FIELD_COMM_ACTOR *act );

static MMDL * fldcommAct_fmmdl_Add(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 code,
    const u16 *watch_dir, const VecFx32 *watch_pos, const BOOL *watch_vanish,
    FIELD_COMM_ACTOR *comm_actor );
static void fldcommAct_fmmdl_SetWatchData(
    MMDL *fmmdl, const u16 *dir, const VecFx32 *pos, const BOOL *vanish,
    FIELD_COMM_ACTOR *comm_actor );

static u16 grid_ChangeFourDir( u16 dir );

static const MMDL_HEADER fldcommActro_MMdlHeader;

//======================================================================
//  �t�B�[���h�@�ʐM�p�A�N�^�[����
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@������
 * @param max �A�N�^�[�ő吔
 * @param fmmdlsys MMDLSYS*
 * @param heapID HEAPID
 * @retval FIELD_COMM_ACTOR_CTRL
 */
//--------------------------------------------------------------
FIELD_COMM_ACTOR_CTRL * FIELD_COMM_ACTOR_CTRL_Create(
    int max, MMDLSYS *fmmdlsys, HEAPID heapID )
{
  FIELD_COMM_ACTOR_CTRL *act_ctrl;
  
  act_ctrl = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(FIELD_COMM_ACTOR_CTRL) );
  act_ctrl->max = max;
  act_ctrl->fmmdlsys = fmmdlsys;
  
  act_ctrl->act_tbl =
    GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_COMM_ACTOR)*max );
  return( act_ctrl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@�폜�B
 * �ǉ�����Ă����A�N�^�[���폜�����B
 * @param act_ctrl FIELD_COMM_ACTOR_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_COMM_ACTOR_CTRL_Delete( FIELD_COMM_ACTOR_CTRL *act_ctrl )
{
  int i = 0;
  FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
  
  for( i = 0; i < act_ctrl->max; i++, act++ ){
    if( act->fmmdl != NULL ){
      fldcommAct_DeleteActor( act );
    }
  }
  
  GFL_HEAP_FreeMemory( act_ctrl->act_tbl );
  GFL_HEAP_FreeMemory( act_ctrl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@�A�N�^�[�ǉ�
 * @param act_ctrl FIELD_COMM_ACTOR_CTRL*
 * @param id �C�ӂ̊Ǘ�ID
 * @param code �\���R�[�h HERO��
 * @param watch_dir �����Q�Ɛ�
 * @param watch_pos ���W�Q�Ɛ�
 * @param watch_vanish �\���t���O TRUE=��\�� NULL�w��=�Q�Ƃ��܂���B
 * @retval nothing
 * @attention watch_dir��watch_pos,vanish_flag����ɎQ�Ƃ��A
 * �A�N�^�[�֔��f������̂ŁA�A�N�^�[���쒆��watch_dir��
 * watch_pos,watch_vanish�͏�ɕێ����ĉ������B
 */
//--------------------------------------------------------------
void FIELD_COMM_ACTOR_CTRL_AddActor(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id, u16 code,
    const u16 *watch_dir, const VecFx32 *watch_pos, const BOOL *watch_vanish )
{
  int i;
  u16 dir;
  FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
  
  dir = grid_ChangeFourDir( *watch_dir ); //�p�x->�l����
  
  for( i = 0; i < act_ctrl->max; i++, act++ ){
    if( act->fmmdl == NULL ){
      act->fmmdl = fldcommAct_fmmdl_Add(
          act_ctrl, code, watch_dir, watch_pos, watch_vanish, act );
      act->id = id;
      OS_Printf( "FIELD_COMM_ACTOR AddActor ID %d\n", id );
      return;
    }
  }
  
  GF_ASSERT( 0 && "ERROR COMM ACTOR MAX\n");
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@�A�N�^�[�폜
 * @param act_ctrl  FIELD_COMM_ACTOR_CTRL*
 * @param id �ǉ����Ɏw�肵���Ǘ�ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_COMM_ACTOR_CTRL_DeleteActro(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id )
{
  int i;
  FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
  
  for( i = 0; i < act_ctrl->max; i++, act++ ){
    if( act->fmmdl != NULL ){
      if( act->id == id ){
        fldcommAct_DeleteActor( act );
        return;
      }
    }
  }
  
  GF_ASSERT( 0 && "ERROR COMM ACTOR DELETE\n" );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@�w����W�ɂ���ʐM�A�N�^�[ID��Ԃ�
 * @param FIELD_COMM_ACTOR_CTRL
 * @param gx ���ׂ�O���b�hX���W
 * @param gz ���ׂ�O���b�hZ���W
 * @param outID gx,gz�ɋ���A�N�^�[ID�̊i�[��
 * @retval BOOL TRUE=gx,gz�ɒʐM�A�N�^�[������BFALSE=���Ȃ�
 */
//--------------------------------------------------------------
BOOL FIELD_COMM_ACTOR_CTRL_SearchGridPos(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, s16 gx, s16 gz, u32 *outID )
{
  MMDL *mmdl;
  
  mmdl = MMDLSYS_SearchGridPos( act_ctrl->fmmdlsys, gx, gz, FALSE );
  
  if( mmdl != NULL ){
    int i;
    FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
    
    for( i = 0; i < act_ctrl->max; i++, act++ ){
      if( act->fmmdl != NULL && act->fmmdl == mmdl ){
        *outID = act->id;
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�@�A�N�^�[���g�p���Ă��铮�샂�f���擾
 * @param act_ctrl FIELD_COMM_ACTOR_CTRL
 * @param id �ǉ����Ɏw�肵���Ǘ�ID
 * @retval MMDL MMDL*
 */
//--------------------------------------------------------------
MMDL * FIELD_COMM_ACTOR_CTRL_GetMMdl(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id )
{
  int i;
  FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
  
  for( i = 0; i < act_ctrl->max; i++, act++ ){
    if( act->fmmdl != NULL ){
      if( act->id == id ){
        return( act->fmmdl );
      }
    }
  }
  
  GF_ASSERT( 0 && "ERROR COMM ACTOR UNKNOW ID\n" );
  return( NULL );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�@�A�N�^�[�폜����
 * @param act FIELD_COMM_ACTOR
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldcommAct_DeleteActor( FIELD_COMM_ACTOR *act )
{
  if( act->del_flag == FALSE ){
    MMDL_Delete( act->fmmdl );
  }
  
  MI_CpuClear8( act, sizeof(FIELD_COMM_ACTOR) );
}

//======================================================================
//  �ʐM�p�A�N�^�[�@���샂�f��
//======================================================================
//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���ǉ�
 * @param act_ctrl  FIELD_COMM_ACTOR_CTRL
 * @param code  �\���R�[�h HERO��
 * @param watch_pos �\��������W
 * @retval MMDL* �ǉ����ꂽMMDL
 */
//--------------------------------------------------------------
static MMDL * fldcommAct_fmmdl_Add(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 code,
    const u16 *watch_dir, const VecFx32 *watch_pos, const BOOL *watch_vanish,
    FIELD_COMM_ACTOR *comm_actor )
{
  MMDL *fmmdl;
  MMDLSYS *fmmdlsys = act_ctrl->fmmdlsys;
  MMDL_HEADER head = fldcommActro_MMdlHeader;
  
  head.obj_code = code;
  fmmdl = MMDLSYS_AddMMdl( fmmdlsys, &head, 0 );
  
  fldcommAct_fmmdl_SetWatchData(
      fmmdl, watch_dir, watch_pos, watch_vanish, comm_actor );
  
  MMDL_InitPosition( fmmdl, watch_pos, grid_ChangeFourDir(*watch_dir) );
  MMDL_SetStatusBitHeightGetOFF( fmmdl, TRUE );
  MMDL_SetStatusBitNotZoneDelete( fmmdl, TRUE );
  MMDL_SetMoveBitRejectPauseMove( fmmdl, TRUE );
  return( fmmdl );
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@���쏉����
 * @param   fmmdl MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCommActor_Init( MMDL *fmmdl )
{
  MV_COMMACT_WORK *work;
  work = MMDL_InitMoveProcWork( fmmdl, sizeof(MV_COMMACT_WORK) );
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@����폜
 * @param fmmdl MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCommActor_Delete( MMDL *fmmdl )
{
  MV_COMMACT_WORK *work;
  work = MMDL_GetMoveProcWork( fmmdl );
  work->comm_actor->del_flag = TRUE; //�ʐM���A�N�^�[�ɍ폜�ʒm
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@����
 * @param fmmdl MMDL*
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCommActor_Move( MMDL *fmmdl )
{
  MV_COMMACT_WORK *work;
  work = MMDL_GetMoveProcWork( fmmdl );
  
  {
    u16 dir;
    dir = grid_ChangeFourDir( *work->watch_dir );
    MMDL_SetDirDisp( fmmdl, dir );
  }
  
  {
    VecFx32 pos;
    u16 status = DRAW_STA_STOP;
    
    MMDL_GetVectorPos( fmmdl, &pos );
    
    if( pos.x != work->watch_pos->x ||
        pos.y != work->watch_pos->y ||
        pos.z != work->watch_pos->z ){
        MMDL_InitPosition( fmmdl,
            work->watch_pos, MMDL_GetDirDisp(fmmdl) );
        status = DRAW_STA_WALK_8F;
    }
    
    MMDL_SetDrawStatus( fmmdl, status );
  }
  
  if( work->watch_vanish != NULL ){
    if( (*work->watch_vanish) == TRUE ){
      MMDL_SetStatusBitVanish( fmmdl, TRUE );
    }else{
      MMDL_SetStatusBitVanish( fmmdl, FALSE );
    }
  }
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@�Q�ƍ��W���Z�b�g
 * @param fmmdl MMDL*
 * @param pos �Q�Ƃ�����W
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldcommAct_fmmdl_SetWatchData(
    MMDL *fmmdl, const u16 *dir, const VecFx32 *pos, const BOOL *vanish,
    FIELD_COMM_ACTOR *comm_actor )
{
  MV_COMMACT_WORK *work;
  work = MMDL_GetMoveProcWork( fmmdl );
  work->watch_dir = dir;
  work->watch_pos = pos;
  work->watch_vanish = vanish;
  work->comm_actor = comm_actor;
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * 360�x����->�S������
 * @param  dir  �����@0x10000�P��
 * @retval  u16 DIR_UP��
 */
//--------------------------------------------------------------
static u16 grid_ChangeFourDir( u16 dir )
{
#if 0
  if( (dir>0x2000) && (dir<0x6000) ){
    dir = DIR_LEFT;
  }else if( (dir >= 0x6000) && (dir <= 0xa000) ){
    dir = DIR_DOWN;
  }else if( (dir > 0xa000)&&(dir < 0xe000) ){
    dir = DIR_RIGHT;
  }else{
    dir = DIR_UP;
  }
#else
  //�ʐM��FIELD_PLAYER_GetDir()�ō̂�
#endif
  return( dir );
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/// �ʐM�A�N�^�[�pMMDL_HEADER
//--------------------------------------------------------------
static const MMDL_HEADER fldcommActro_MMdlHeader =
{
  MMDL_ID_COMMACTOR,
  0,
  MV_COMM_ACTOR,
  0,  ///<�C�x���g�^�C�v
  0,  ///<�C�x���g�t���O
  0,  ///<�C�x���gID
  0,  ///<�w�����
  0,  ///<�w��p�����^ 0
  0,  ///<�w��p�����^ 1
  0,  ///<�w��p�����^ 2
  4,  ///<X�����ړ�����
  4,  ///<Z�����ړ�����
  0,  ///<�O���b�hX
  0,  ///<�O���b�hZ
  0,  ///<Y�l fx32�^
};
