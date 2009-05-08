//======================================================================
/**
 * @file	field_comm_actor.c
 * @brief	
 * @author
 * @data
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_comm_actor.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_COMM_ACTOR
//--------------------------------------------------------------
typedef struct
{
  u32 id;
  FLDMMDL *fmmdl;
}FIELD_COMM_ACTOR;

//--------------------------------------------------------------
/// FIELD_COMM_ACTOR_CTRL
//--------------------------------------------------------------
struct _TAG_FIELD_COMM_ACTOR_CTRL
{
  int max;
  HEAPID heapID;
  FLDMMDLSYS *fmmdlsys;
  FIELD_COMM_ACTOR *act_tbl;
};

//--------------------------------------------------------------
/// MV_COMMACT_WORK
//--------------------------------------------------------------
typedef struct
{
  const u16 *watch_dir;
  const VecFx32 *watch_pos;
}MV_COMMACT_WORK;

//======================================================================
//	proto
//======================================================================
static void fldcommAct_DeleteActor( FIELD_COMM_ACTOR *act );

static FLDMMDL * fldcommAct_fmmdl_Add(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 code,
    const u16 *watch_dir, const VecFx32 *watch_pos );
static void fldcommAct_fmmdl_SetWatchPos(
    FLDMMDL *fmmdl, const VecFx32 *pos );

static const FLDMMDL_HEADER fldcommActro_FldMMdlHeader;

//======================================================================
//  �t�B�[���h�@�ʐM�p�A�N�^�[����
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[����@������
 * @param max �A�N�^�[�ő吔
 * @param fmmdlsys FLDMMDLSYS*
 * @param heapID HEAPID
 * @retval FIELD_COMM_ACTOR_CTRL
 */
//--------------------------------------------------------------
FIELD_COMM_ACTOR_CTRL * FIELD_COMM_ACTOR_CTRL_Create(
    int max, FLDMMDLSYS *fmmdlsys, HEAPID heapID )
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
 * @param id �Ǘ�ID
 * @param code �\���R�[�h HERO��
 * @param watch_pos �\��������W
 * @retval FIELD_COMM_ACTOR* �ǉ����ꂽFIELD_COMM_ACTRO*
 */
//--------------------------------------------------------------
void FIELD_COMM_ACTOR_CTRL_AddActor( FIELD_COMM_ACTOR_CTRL *act_ctrl,
    u32 id, u16 code, const u16 *watch_dir, const VecFx32 *watch_pos )
{
  int i;
  FIELD_COMM_ACTOR *act = act_ctrl->act_tbl;
  
  for( i = 0; i < act_ctrl->max; i++, act++ ){
    if( act->fmmdl == NULL ){
      act->fmmdl = fldcommAct_fmmdl_Add(
          act_ctrl, code, watch_dir, watch_pos );
      act->id = id;
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
 * �t�B�[���h�ʐM�p�A�N�^�[�@�A�N�^�[�폜����
 * @param act FIELD_COMM_ACTOR
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldcommAct_DeleteActor( FIELD_COMM_ACTOR *act )
{
  FLDMMDL_Delete( act->fmmdl );
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
 * @retval FLDMMDL* �ǉ����ꂽFLDMMDL
 */
//--------------------------------------------------------------
static FLDMMDL * fldcommAct_fmmdl_Add(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 code,
    const u16 *watch_dir, const VecFx32 *watch_pos )
{
  FLDMMDL *fmmdl;
  FLDMMDLSYS *fmmdlsys = act_ctrl->fmmdlsys;
  FLDMMDL_HEADER head = fldcommActro_FldMMdlHeader;
  
  head.obj_code = code;
  fmmdl = FLDMMDLSYS_AddFldMMdl( fmmdlsys, &head, 0 );
  fldcommAct_fmmdl_SetWatchPos( fmmdl, watch_pos );
  //���W�ݒ�
  //�����A�A�g���r���[�g�����ݒ�
  return( fmmdl );
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@���쏉����
 * @param   fmmdl FLDMMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCommActor_Init( FLDMMDL *fmmdl )
{
  MV_COMMACT_WORK *work;
  work = FLDMMDL_InitMoveProcWork( fmmdl, sizeof(MV_COMMACT_WORK) );
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@����
 * @param fmmdl FLDMMDL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCommActor_Move( FLDMMDL *fmmdl )
{
  MV_COMMACT_WORK *work;
  work = FLDMMDL_GetMoveProcWork( fmmdl );
}

//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@���샂�f���@�Q�ƍ��W���Z�b�g
 * @param fmmdl FLDMMDL*
 * @param pos �Q�Ƃ�����W
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldcommAct_fmmdl_SetWatchPos(
    FLDMMDL *fmmdl, const VecFx32 *pos )
{
  MV_COMMACT_WORK *work;
  work = FLDMMDL_GetMoveProcWork( fmmdl );
  work->watch_pos = pos;
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/// �ʐM�A�N�^�[�pFLDMMDL_HEADER
//--------------------------------------------------------------
static const FLDMMDL_HEADER fldcommActro_FldMMdlHeader =
{
  FLDMMDL_ID_COMMACTOR,
  0,
  MV_DMY,
  0,	///<�C�x���g�^�C�v
  0,	///<�C�x���g�t���O
  0,	///<�C�x���gID
  0,	///<�w�����
  0,	///<�w��p�����^ 0
  0,	///<�w��p�����^ 1
  0,	///<�w��p�����^ 2
  4,	///<X�����ړ�����
  4,	///<Z�����ړ�����
  0,	///<�O���b�hX
  0,	///<�O���b�hZ
  0,	///<Y�l fx32�^
};
