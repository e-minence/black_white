//======================================================================
/**
 * @file	fldeff_namipoke.c
 * @brief	�t�B�[���h �g���|�P����
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_namipoke.h"

//======================================================================
//  define
//======================================================================
//#define DEBUG_PARAM_KEY_CHANGE

#define NAMIPOKE_SHAKE_VALUE (0x0400) ///<�����h�ꕝ
#define NAMIPOKE_SHAKE_MAX (FX32_ONE*4) ///<�h�ꕝ�ő�

#define NAMIPOKE_RIDE_RAIL_Y_OFFSET (FX32_ONE*5) ///<�^�]��\���I�t�Z�b�gY
#define NAMIPOKE_RIDE_RAIL_Z_OFFSET (FX32_ONE*7) ///<�^�]��\���I�t�Z�b�gZ

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_NAMIPOKE�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_NAMIPOKE FLDEFF_NAMIPOKE;
typedef struct _TAG_FLDEFF_NAMIPOKE_EFFECT FLDEFF_NAMIPOKE_EFFECT;

//--------------------------------------------------------------
/// RIPPLE_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  u16 vanish_flag;
  u16 dir;
  VecFx32 pos;
  VecFx32 save_pos;
  
  FLD_G3DOBJ_OBJIDX obj_idx;

  u16 rot_y;
  VecFx32 scale;
  VecFx32 offs;
}RIPPLE_WORK;

//--------------------------------------------------------------
///	FLDEFF_NAMIPOKE�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_NAMIPOKE
{
	FLDEFF_CTRL *fectrl;
  FLD_G3DOBJ_RESIDX res_idx_poke;
  FLD_G3DOBJ_RESIDX res_idx_ripple;
};

//--------------------------------------------------------------
/// TASKHEADER_NAMIPOKE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_NAMIPOKE *eff_namipoke;
  MMDL *mmdl;
  u16 obj_id;
  u16 zone_id;
  u16 dir;
  u16 dmy;
  VecFx32 pos;
}TASKHEADER_NAMIPOKE;

//--------------------------------------------------------------
/// TASKWORK_NAMIPOKE
//--------------------------------------------------------------
typedef struct
{
  GFL_G3D_OBJ *obj;
  GFL_G3D_RND *obj_rnd;

  u8 seq_no;
  u8 joint;
  u8 ripple_off;
  u8 padding;

  u16 dir;
  fx32 shake_offs;
  fx32 shake_value;
  
  TASKHEADER_NAMIPOKE head;
  
  FLD_G3DOBJ_OBJIDX obj_idx;
  
  RIPPLE_WORK ripple_work;
}TASKWORK_NAMIPOKE;

//--------------------------------------------------------------
/// FLDEFF_NAMIPOKE_EFFECT�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_NAMIPOKE_EFFECT
{
  FLDEFF_CTRL *fectrl;
  FLD_G3DOBJ_RESIDX res_idx_taki_land;
  FLD_G3DOBJ_RESIDX res_idx_taki_start_f;
  FLD_G3DOBJ_RESIDX res_idx_taki_start_s;
  FLD_G3DOBJ_RESIDX res_idx_taki_loop_f;
  FLD_G3DOBJ_RESIDX res_idx_taki_loop_s;
};

//--------------------------------------------------------------
/// TASKHEADER_NAMIPOKE_EFFECT�\����
//--------------------------------------------------------------
typedef struct
{
  BOOL dead_flag;
  FLDEFF_NAMIPOKE_EFFECT *eff_npoke_eff;
  NAMIPOKE_EFFECT_TYPE type;
  const FLDEFF_TASK *efftask_namipoke;
}TASKHEADER_NAMIPOKE_EFFECT;

//--------------------------------------------------------------
/// TASKWORK_NAMIPOKE_EFFECT�\����
//--------------------------------------------------------------
typedef struct
{
  VecFx32 offset;
  BOOL end_flag;
  FLD_G3DOBJ_OBJIDX obj_idx;
  TASKHEADER_NAMIPOKE_EFFECT head;
}TASKWORK_NAMIPOKE_EFFECT;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void namipoke_InitResource( FLDEFF_NAMIPOKE *namipoke );
static void namipoke_DeleteResource( FLDEFF_NAMIPOKE *namipoke );

static const FLDEFF_TASK_HEADER DATA_namipokeTaskHeader;
static const FLDEFF_TASK_HEADER DATA_RAIL_namipokeTaskHeader;

static void npoke_eff_InitResource( FLDEFF_NAMIPOKE_EFFECT *npoke_eff );
static void npoke_eff_DeleteResource( FLDEFF_NAMIPOKE_EFFECT *npoke_eff );

static const FLDEFF_TASK_HEADER data_npoke_effTaskHeader;

//======================================================================
//	�g���|�P���� �V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P���� ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_NAMIPOKE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_NAMIPOKE *namipoke;
	
	namipoke = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_NAMIPOKE) );
	namipoke->fectrl = fectrl;
  
	namipoke_InitResource( namipoke );
	return( namipoke );
}

//--------------------------------------------------------------
/**
 * �g���|�P���� �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_NAMIPOKE *namipoke = work;
  namipoke_DeleteResource( namipoke );
  GFL_HEAP_FreeMemory( namipoke );
}

//======================================================================
//	�g���|�P�����@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipoke_InitResource( FLDEFF_NAMIPOKE *namipoke )
{
  BOOL ret;
  ARCHANDLE *handle;
  FLD_G3DOBJ_RES_HEADER head;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( namipoke->fectrl );
  handle = FLDEFF_CTRL_GetArcHandleEffect( namipoke->fectrl );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head ); //pokemon
  
  FLD_G3DOBJ_RES_HEADER_SetMdl(&head, handle, NARC_fldeff_sea_ride_nsbmd );
  namipoke->res_idx_poke =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head ); //ripple

  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_shibuki01_nsbmd  );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_shibuki01_nsbca );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_shibuki01_nsbta );
  namipoke->res_idx_ripple =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipoke_DeleteResource( FLDEFF_NAMIPOKE *namipoke )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( namipoke->fectrl );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, namipoke->res_idx_poke );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, namipoke->res_idx_ripple );
}

//======================================================================
//	�g���|�P�����@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�g���|�P�����@�ǉ�
 * @param FLDEFF_CTRL*
 * @param pos �\�����W
 * @param mmdl MMDL �|�P�����ɏ�铮�샂�f��
 * @param joint ���샂�f���Ƃ̓��� OFF=�W���C���g���Ȃ�
 * @retval nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_NAMIPOKE_SetMMdl( FLDEFF_CTRL *fectrl,
    u16 dir, const VecFx32 *pos, MMDL *mmdl, NAMIPOKE_JOINT joint )
{
  fx32 h;
  FLDEFF_TASK *task;
  TASKHEADER_NAMIPOKE head;
  FLDEFF_NAMIPOKE *namipoke;
  
  namipoke = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_NAMIPOKE );
  head.eff_namipoke = namipoke;
  head.mmdl = mmdl;
  head.obj_id = MMDL_GetOBJID( mmdl );
  head.zone_id = MMDL_GetZoneID( mmdl );
  head.dir = dir;
  head.pos = *pos;
  
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) ){
    
    // GRID�p
    task = FLDEFF_CTRL_AddTask(
        fectrl, &DATA_namipokeTaskHeader, &head.pos, joint, &head, 0 );
    //TOMOYA_Printf( "namipoke grid setup\n" );
  }else{
    // RIIL�p
    task = FLDEFF_CTRL_AddTask(
        fectrl, &DATA_RAIL_namipokeTaskHeader, &head.pos, joint, &head, 0 );
    //TOMOYA_Printf( "namipoke rail setup\n" );
  }
  return( task );
}

//--------------------------------------------------------------
/**
 * ���샂�f���p�g���|�P�����@�W���C���g�t���O�Z�b�g
 * @param task FLDEFF_TASK*
 * @param flag TRUE=�ڑ�,FALSE=��ڑ�
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_SetJointFlag( FLDEFF_TASK *task, NAMIPOKE_JOINT flag )
{
  if( task != NULL ){
    TASKWORK_NAMIPOKE *work = FLDEFF_TASK_GetWork( task );
    work->joint = flag;
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * ���샂�f���p�g���|�P�����@���򖗃G�t�F�N�gON,OFF
 * @param task FLDEFF_TASK*
 * @param flag TRUE=�\�� FALSE=��\��
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_SetRippleEffect( FLDEFF_TASK *task, BOOL flag )
{
  if( task != NULL ){
    TASKWORK_NAMIPOKE *work = FLDEFF_TASK_GetWork( task );
    if( flag == TRUE ){
      work->ripple_off = FALSE;
    }else{
      work->ripple_off = TRUE;
    }
  }else{
    GF_ASSERT( 0 );
  }
}

#define RIP_SCALE_VECTOR_MAX (GRID_SIZE_FX32(4))
#define RIP_SCALE_VECTOR_PER (RIP_SCALE_VECTOR_MAX/100)
#define RIP_SCALE_MAX (FX32_ONE)
#define RIP_SCALE_PER (RIP_SCALE_MAX/100)
#define RIP_SCALE_VECTOR_LES (0x4000)

static fx32 ripple_scale_get( fx32 vec )
{
  if( vec > RIP_SCALE_VECTOR_MAX ){
    vec = RIP_SCALE_VECTOR_MAX;
  }
  vec /= RIP_SCALE_VECTOR_PER;
  vec = RIP_SCALE_PER * vec;
  return( vec );
}

static fx32 ripple_vecpos(
    const VecFx32 *m_pos, const VecFx32 *rip_pos, u16 dir )
{
  fx32 vec = 0;
  
  switch( dir ){
  case DIR_UP:
    vec = rip_pos->z - m_pos->z;
    break;
  case DIR_DOWN:
    vec = m_pos->z - rip_pos->z;
    break;
  case DIR_LEFT:
    vec = rip_pos->x - m_pos->x;
    break;
  case DIR_RIGHT:
    vec = m_pos->x - rip_pos->x;
    break;
  }
  return( vec );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Init( FLDEFF_TASK *task, void *wk )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE *work = wk;
  const TASKHEADER_NAMIPOKE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->dir = head->dir;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_namipoke->fectrl );
  
  work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
      obj_ctrl, work->head.eff_namipoke->res_idx_poke, 0, NULL );
  
  work->joint = FLDEFF_TASK_GetAddParam( task );
  work->shake_offs = FX32_ONE;
  work->shake_value = NAMIPOKE_SHAKE_VALUE;
  
  {
    RIPPLE_WORK *rip = &work->ripple_work;
    
    rip->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_namipoke->res_idx_ripple, 0, NULL );
    
    rip->vanish_flag = TRUE;
    rip->dir = DIR_NOT;
    
    rip->scale.x = FX32_ONE;
    rip->scale.y = FX32_ONE;
    rip->scale.z = FX32_ONE;
  }
  
  FLDEFF_TASK_CallUpdate( task ); //������
}

//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  FLD_G3DOBJ_CTRL *obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_namipoke->fectrl );
  
  FLD_G3DOBJ_CTRL_DeleteObject( obj_ctrl, work->obj_idx );
  
  {
    RIPPLE_WORK *rip = &work->ripple_work;
    FLD_G3DOBJ_CTRL_DeleteObject( obj_ctrl, rip->obj_idx );
  }
}

#define RIPPLE_OFFS_Y (-0x4000)
#define RIPPLE_OFFS_XZ (0xe000)

static void namipokeTask_DrawCore( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  GFL_G3D_OBJSTATUS *st;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE *work = wk;
  u16 rtbl[DIR_MAX4][3] = {{0,0x8000,0},{0,0,0},{0,0xc000,0},{0,0x4000,0}};
  u16 *rot = rtbl[work->dir];
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_namipoke->fectrl );
  
  //�|�P������]�A���W�ݒ�
  st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, work->obj_idx );
  GFL_CALC3D_MTX_CreateRot( rot[0], rot[1], rot[2], &st->rotate );
  FLDEFF_TASK_GetPos( task, &st->trans );
  
  if( work->ripple_work.vanish_flag == FALSE ){ //��
    RIPPLE_WORK *rip = &work->ripple_work;
    VecFx32 rip_otbl[DIR_MAX4] = {
      {0,RIPPLE_OFFS_Y,RIPPLE_OFFS_XZ},
      {0,RIPPLE_OFFS_Y,-RIPPLE_OFFS_XZ},
      {RIPPLE_OFFS_XZ,RIPPLE_OFFS_Y,0},
      {-RIPPLE_OFFS_XZ,RIPPLE_OFFS_Y,0},
    };
    u16 rip_rtbl[DIR_MAX4][3] = {
      {0,0,0},{0,0x8000,0},{0,0x4000,0},{0,0xc000,0} };
    VecFx32 *rip_offs = &rip_otbl[work->dir];
    u16 *rip_rot = rip_rtbl[work->dir];
    
    st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, rip->obj_idx );
    
    GFL_CALC3D_MTX_CreateRot(
        rip_rot[0], rip_rot[1], rip_rot[2], &st->rotate );
      
    FLDEFF_TASK_GetPos( task, &st->trans );
    st->trans.x += rip->offs.x + rip_offs->x;
    st->trans.y += rip->offs.y + rip_offs->y;
    st->trans.z += rip->offs.z + rip_offs->z;
    st->scale = rip->scale;
    
    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, FALSE );
  }else{
    RIPPLE_WORK *rip = &work->ripple_work;
    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, TRUE );
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  VecFx32 oya_offs = { 0, 0, 0 };
  
  if( work->joint == NAMIPOKE_JOINT_OFF ){ //�ڑ����Ȃ�
    namipokeTask_DrawCore( task, wk ); //�\�����������s��
    return;
  }
 	
  { //����
    work->dir = MMDL_GetDirDisp( work->head.mmdl );
  }
  
  if( work->joint == NAMIPOKE_JOINT_ON ){ //�ǐ��@�h��
		work->shake_offs += work->shake_value;
		
		if( work->shake_offs >= NAMIPOKE_SHAKE_MAX ){
			work->shake_offs = NAMIPOKE_SHAKE_MAX;
			work->shake_value = -work->shake_value;
		}else if( work->shake_offs <= FX32_ONE ){
			work->shake_offs = FX32_ONE;
			work->shake_value = -work->shake_value;
		}
    
    //�^�]��ɗh���ǉ�
    oya_offs.x = 0;
    oya_offs.y = work->shake_offs + NAMIPOKE_RIDE_Y_OFFSET;
    oya_offs.z = NAMIPOKE_RIDE_Z_OFFSET;
    MMDL_SetVectorOuterDrawOffsetPos( work->head.mmdl, &oya_offs );
  }else if( work->joint == NAMIPOKE_JOINT_ONLY ){
    MMDL_GetVectorOuterDrawOffsetPos( work->head.mmdl, &oya_offs );
  }
  
  { //���W
    VecFx32 pos;
    MMDL_GetDrawVectorPos( work->head.mmdl, &pos );
    
    pos.x -= oya_offs.x;
    pos.y -= oya_offs.y;
    pos.z -= oya_offs.z;
    
    if( work->joint != NAMIPOKE_JOINT_ONLY ){
      pos.y += work->shake_offs - FX32_ONE;
    }
    
    FLDEFF_TASK_SetPos( task, &pos );
  }
  
  if( work->joint == NAMIPOKE_JOINT_ONLY || work->ripple_off == TRUE ){
    RIPPLE_WORK *rip = &work->ripple_work;      //�g���o���Ȃ�
    rip->dir = DIR_NOT; //����X�V���ɏ�����
    
    if( work->ripple_off == TRUE ){
      rip->vanish_flag = TRUE;
    }
  }else{ //�g�G�t�F�N�g
    fx32 ret;
    VecFx32 pos;
    FLD_G3DOBJ_CTRL *obj_ctrl;
    RIPPLE_WORK *rip = &work->ripple_work;
    
    obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
        work->head.eff_namipoke->fectrl );
    FLD_G3DOBJ_CTRL_LoopAnimeObject( obj_ctrl, rip->obj_idx, FX32_ONE );
    
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    
    if( work->dir != rip->dir ){ //�����X�V
      rip->dir = work->dir;
      rip->pos = pos;
    }else if( pos.x == rip->save_pos.x && //��~ ����
        pos.y == rip->save_pos.y && pos.z == rip->save_pos.z ){
      
      switch( rip->dir ){
      case DIR_UP:
        rip->pos.z -= RIP_SCALE_VECTOR_LES;
        if( rip->pos.z < pos.z ){ rip->pos.z = pos.z; }
        break;
      case DIR_DOWN:
        rip->pos.z += RIP_SCALE_VECTOR_LES;
        if( rip->pos.z > pos.z ){ rip->pos.z = pos.z; }
        break;
      case DIR_LEFT:
        rip->pos.x -= RIP_SCALE_VECTOR_LES;
        if( rip->pos.x < pos.x ){ rip->pos.x = pos.x; }
        break;
      case DIR_RIGHT:
        rip->pos.x += RIP_SCALE_VECTOR_LES;
        if( rip->pos.x > pos.x ){ rip->pos.x = pos.x; }
        break;
      }
    }
    
    ret = ripple_vecpos( &pos, &rip->pos, rip->dir );
    
    if( ret >= RIP_SCALE_VECTOR_MAX ){
      switch( rip->dir ){
      case DIR_UP:
        rip->pos.z = pos.z + RIP_SCALE_VECTOR_MAX;
        break;
      case DIR_DOWN:
        rip->pos.z = pos.z - RIP_SCALE_VECTOR_MAX;
        break;
      case DIR_LEFT:
        rip->pos.x = pos.x + RIP_SCALE_VECTOR_MAX;
        break;
      case DIR_RIGHT:
        rip->pos.x = pos.x - RIP_SCALE_VECTOR_MAX;
        break;
      }
    }

    ret = ripple_scale_get( ret );
    
    if( ret < RIP_SCALE_PER ){
      rip->vanish_flag = TRUE;
    }else{
      rip->vanish_flag = FALSE;
      rip->scale.x = ret;
      rip->scale.y = ret;
      rip->scale.z = ret;
    }
    
    rip->save_pos = pos;
  }

  namipokeTask_DrawCore( task, wk );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 * @note �`���FLD_G3DOBJ_CTRL�ɔC���A
 * �����ł̓X�e�[�^�X�ݒ���s���݂̂ƂȂ����B
 */
//--------------------------------------------------------------
static void namipokeTask_Draw( FLDEFF_TASK *task, void *wk )
{
  //�����ŕ\�����W��ݒ肵�Ă�����
  //���샂�f���Ƃ̏����^�C�~���O���ς��A�Y������������l�ɂȂ���
  //�X�V�������ŕ\���ݒ���s���B
#if 0
  VecFx32 pos;
  GFL_G3D_OBJSTATUS *st;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE *work = wk;
  u16 rtbl[DIR_MAX4][3] = {{0,0x8000,0},{0,0,0},{0,0xc000,0},{0,0x4000,0}};
  u16 *rot = rtbl[work->dir];
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_namipoke->fectrl );
  
  //�|�P������]�A���W�ݒ�
  st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, work->obj_idx );
  GFL_CALC3D_MTX_CreateRot( rot[0], rot[1], rot[2], &st->rotate );
  FLDEFF_TASK_GetPos( task, &st->trans );
  
  if( work->ripple_work.vanish_flag == FALSE ){ //��
    RIPPLE_WORK *rip = &work->ripple_work;
    VecFx32 rip_otbl[DIR_MAX4] = {
      {0,RIPPLE_OFFS_Y,RIPPLE_OFFS_XZ},
      {0,RIPPLE_OFFS_Y,-RIPPLE_OFFS_XZ},
      {RIPPLE_OFFS_XZ,RIPPLE_OFFS_Y,0},
      {-RIPPLE_OFFS_XZ,RIPPLE_OFFS_Y,0},
    };
    u16 rip_rtbl[DIR_MAX4][3] = {
      {0,0,0},{0,0x8000,0},{0,0x4000,0},{0,0xc000,0} };
    VecFx32 *rip_offs = &rip_otbl[work->dir];
    u16 *rip_rot = rip_rtbl[work->dir];
    
    st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, rip->obj_idx );
    
    GFL_CALC3D_MTX_CreateRot(
        rip_rot[0], rip_rot[1], rip_rot[2], &st->rotate );
      
    FLDEFF_TASK_GetPos( task, &st->trans );
    st->trans.x += rip->offs.x + rip_offs->x;
    st->trans.y += rip->offs.y + rip_offs->y;
    st->trans.z += rip->offs.z + rip_offs->z;
    st->scale = rip->scale;
    
    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, FALSE );
  }else{
    RIPPLE_WORK *rip = &work->ripple_work;
    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, TRUE );
  }
#endif
}

//--------------------------------------------------------------
//  �g���|�P�����^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_namipokeTaskHeader =
{
  sizeof(TASKWORK_NAMIPOKE),
  namipokeTask_Init,
  namipokeTask_Delete,
  namipokeTask_Update,
  namipokeTask_Draw,
};




//--------------------------------------------------------------
//  �g���|�P�����^�X�N�@Rail�}�b�v�p
//
//  Rail�}�b�v�͕��������낱��ς��B
//--------------------------------------------------------------
//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_RAIL_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  VecFx32 oya_offs = { 0, 0, 0 };
  VecFx16 up_way;
  VecFx16 front_way;
  
  if( work->joint == NAMIPOKE_JOINT_OFF ){ //�ڑ����Ȃ�
    return;
  }
 	
  { //����
    work->dir = MMDL_GetDirDisp( work->head.mmdl );

    // 3D�����@�iXZ���ʂɂ���j
    MMDL_Rail_GetDirLineWay( work->head.mmdl, DIR_UP, &up_way );
    MMDL_Rail_GetDirLineWay( work->head.mmdl, work->dir, &front_way );

    up_way.y = 0;
    VEC_Fx16Normalize( &up_way, &up_way );
    front_way.y = 0;
    VEC_Fx16Normalize( &front_way, &front_way );
  }
  
  if( work->joint == NAMIPOKE_JOINT_ON ){ //�h��
		work->shake_offs += work->shake_value;
		
		if( work->shake_offs >= NAMIPOKE_SHAKE_MAX ){
			work->shake_offs = NAMIPOKE_SHAKE_MAX;
			work->shake_value = -work->shake_value;
		}else if( work->shake_offs <= FX32_ONE ){
			work->shake_offs = FX32_ONE;
			work->shake_value = -work->shake_value;
		}
    
    { //�^�]��ɗh���ǉ�
      oya_offs.x = FX_Mul( -up_way.x, NAMIPOKE_RIDE_RAIL_Z_OFFSET);
      oya_offs.y = work->shake_offs + NAMIPOKE_RIDE_RAIL_Y_OFFSET;
      oya_offs.z = FX_Mul( -up_way.z, NAMIPOKE_RIDE_RAIL_Z_OFFSET ); 
      MMDL_SetVectorOuterDrawOffsetPos( work->head.mmdl, &oya_offs );
    }
  }

  { //���W
    VecFx32 pos;
    MMDL_GetDrawVectorPos( work->head.mmdl, &pos );
    
    pos.x -= oya_offs.x;
    pos.y -= oya_offs.y;
    pos.z -= oya_offs.z;
    
    pos.y += work->shake_offs - FX32_ONE;
    FLDEFF_TASK_SetPos( task, &pos );
  }
  
  if( work->joint == NAMIPOKE_JOINT_ONLY || work->ripple_off == TRUE ){
    RIPPLE_WORK *rip = &work->ripple_work;      //�g���o���Ȃ�
    rip->dir = DIR_NOT; //����X�V���ɏ�����
    
    if( work->ripple_off == TRUE ){
      rip->vanish_flag = TRUE;
    }
  }else{ //�g�G�t�F�N�g
    fx32 ret;
    VecFx32 pos;
    FLD_G3DOBJ_CTRL *obj_ctrl;
    RIPPLE_WORK *rip = &work->ripple_work;
    
    obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
        work->head.eff_namipoke->fectrl );
    FLD_G3DOBJ_CTRL_LoopAnimeObject( obj_ctrl, rip->obj_idx, FX32_ONE );
    
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    
    if( work->dir != rip->dir ){ //�����X�V
      rip->dir = work->dir;
      rip->pos = pos;
    }else if( pos.x == rip->save_pos.x && //��~ ����
      pos.y == rip->save_pos.y && pos.z == rip->save_pos.z ){
      
      rip->pos.x += FX_Mul( front_way.x, RIP_SCALE_VECTOR_LES );
      rip->pos.z += FX_Mul( front_way.z, RIP_SCALE_VECTOR_LES );
      rip->pos.y = pos.y;
      if( front_way.x > 0 ){
        if( rip->pos.x > pos.x ){ rip->pos.x = pos.x; }
      }else if( front_way.x < 0 ){
        if( rip->pos.x < pos.x ){ rip->pos.x = pos.x; }
      }
      if( front_way.z > 0 ){
        if( rip->pos.z > pos.z ){ rip->pos.z = pos.z; }
      }else if( front_way.z < 0 ){
        if( rip->pos.z < pos.z ){ rip->pos.z = pos.z; }
      }
    }
    
    ret = VEC_Distance( &pos, &rip->pos );
    
    if( ret >= RIP_SCALE_VECTOR_MAX ){
      // �i�s�����ɂ��킹�Ĉʒu�����߂�
      rip->pos.x = pos.x - FX_Mul( front_way.x, RIP_SCALE_VECTOR_MAX );
      rip->pos.z = pos.z - FX_Mul( front_way.z, RIP_SCALE_VECTOR_MAX );
    }

    ret = ripple_scale_get( ret );
    
    if( ret <= RIP_SCALE_PER ){
      rip->vanish_flag = TRUE;
    }else{
      rip->vanish_flag = FALSE;
      rip->scale.x = ret;
      rip->scale.y = ret;
      rip->scale.z = ret;
    }
    
    rip->save_pos = pos;
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 * @note �`���FLD_G3DOBJ_CTRL�ɔC���A
 * �����ł̓X�e�[�^�X�ݒ���s���݂̂ƂȂ����B
 */
//--------------------------------------------------------------
static void namipokeTask_RAIL_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  GFL_G3D_OBJSTATUS *st;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE *work = wk;
  VecFx16 front_way_16;
  VecFx32 front_way;
  VecFx32 back_way;
  MtxFx33 rotate;
  MtxFx33 rotate_back;

  // �i�s����
  {
    MMDL_Rail_GetDirLineWay( work->head.mmdl, work->dir, &front_way_16 );
    front_way.x = front_way_16.x;
    front_way.y = 0;
    front_way.z = front_way_16.z;
    back_way.x  = -front_way_16.x;
    back_way.y  = 0;
    back_way.z  = -front_way_16.z;

    GFL_CALC3D_MTX_GetVecToRotMtxXZ( &front_way, &rotate );
    GFL_CALC3D_MTX_GetVecToRotMtxXZ( &back_way, &rotate_back );
  }
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_namipoke->fectrl );
  
  //�|�P������]�A���W�ݒ�
  st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, work->obj_idx );
  st->rotate = rotate;
  FLDEFF_TASK_GetPos( task, &st->trans );
  
  if( work->ripple_work.vanish_flag == FALSE ){ //��
    RIPPLE_WORK *rip = &work->ripple_work;
    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, FALSE );
    st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, rip->obj_idx );
    st->rotate  = rotate_back;
      
    FLDEFF_TASK_GetPos( task, &st->trans );
    st->trans.x += rip->offs.x - FX_Mul( front_way.x, RIPPLE_OFFS_XZ );
    st->trans.y += rip->offs.y + RIPPLE_OFFS_Y - FX_Mul( front_way.y, RIPPLE_OFFS_XZ );
    st->trans.z += rip->offs.z - FX_Mul( front_way.z, RIPPLE_OFFS_XZ );
    st->scale = rip->scale;
  }else{
    RIPPLE_WORK *rip = &work->ripple_work;

    FLD_G3DOBJ_CTRL_SetObjVanishFlag( obj_ctrl, rip->obj_idx, TRUE );
  }
}

//--------------------------------------------------------------
//  �g���|�P�����^�X�N Rail�}�b�v�p�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_RAIL_namipokeTaskHeader =
{
  sizeof(TASKWORK_NAMIPOKE),
  namipokeTask_Init,
  namipokeTask_Delete,
  namipokeTask_RAIL_Update,
  namipokeTask_RAIL_Draw,
};

//======================================================================
//  �g���|�P�����G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@������
 * @param	fectrl FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_NAMIPOKE_EFFECT_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_NAMIPOKE_EFFECT *npoke_eff;
	
	npoke_eff = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(FLDEFF_NAMIPOKE_EFFECT) );
	npoke_eff->fectrl = fectrl;
  
	npoke_eff_InitResource( npoke_eff );
	return( npoke_eff );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_EFFECT_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_NAMIPOKE_EFFECT *npoke_eff = work;
  npoke_eff_DeleteResource( npoke_eff );
  GFL_HEAP_FreeMemory( npoke_eff );
}

//======================================================================
//	�g���|�P�����G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_eff_InitResource( FLDEFF_NAMIPOKE_EFFECT *npoke_eff )
{
  ARCHANDLE *handle;
  FLD_G3DOBJ_RES_HEADER head;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( npoke_eff->fectrl );

  handle = FLDEFF_CTRL_GetArcHandleEffect( npoke_eff->fectrl );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_taki_land_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_land_nsbma );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_land_nsbca );
  npoke_eff->res_idx_taki_land =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );

  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_taki_start_f_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_start_f_nsbca );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_start_f_nsbtp );
  npoke_eff->res_idx_taki_start_f =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );

  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_taki_roop_f_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_roop_f_nsbca );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_roop_f_nsbtp );
  npoke_eff->res_idx_taki_loop_f =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );

  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_taki_start_s_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_start_s_nsbca );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_start_s_nsbtp );
  npoke_eff->res_idx_taki_start_s =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
      &head, handle, NARC_fldeff_taki_loop_s_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_loop_s_nsbca );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_taki_loop_s_nsbtp );
  npoke_eff->res_idx_taki_loop_s =
    FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_eff_DeleteResource( FLDEFF_NAMIPOKE_EFFECT *npoke_eff )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( npoke_eff->fectrl );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, npoke_eff->res_idx_taki_land );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, npoke_eff->res_idx_taki_start_f );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, npoke_eff->res_idx_taki_loop_f );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, npoke_eff->res_idx_taki_start_s );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, npoke_eff->res_idx_taki_loop_s );
}

//======================================================================
//  �g���|�P�����G�t�F�N�g�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@�ǉ�
 * @param FLDEFF_CTRL*
 * @param type NAMIPOKE_EFFECT_TYPE �\�����
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_NAMIPOKE_EFFECT_SetEffect( FLDEFF_CTRL *fectrl,
    NAMIPOKE_EFFECT_TYPE type, const FLDEFF_TASK *efftask_namipoke )
{
  FLDEFF_TASK *task;
  FLDEFF_NAMIPOKE_EFFECT *npoke_eff;
  TASKHEADER_NAMIPOKE_EFFECT head;
  
  head.eff_npoke_eff = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_NAMIPOKE_EFFECT );
  head.type = type;
  head.efftask_namipoke = efftask_namipoke;
  head.dead_flag = FALSE;

  task = FLDEFF_CTRL_AddTask(
      fectrl, &data_npoke_effTaskHeader, NULL, 0, &head, 1 );
  return( task );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@�ǉ��@�g���|�P�����Ɉˑ����Ȃ����W�w��^
 * @param FLDEFF_CTRL*
 * @param type NAMIPOKE_EFFECT_TYPE �\�����
 * @param pos �\��������W
 * @retval FLDEFF_TASK*
 * @attention NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH�̂݃G�t�F�N�g�I��
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_NAMIPOKE_EFFECT_SetEffectAlone( FLDEFF_CTRL *fectrl,
    NAMIPOKE_EFFECT_TYPE type, const VecFx32 *pos )
{
  FLDEFF_TASK *task;
  FLDEFF_NAMIPOKE_EFFECT *npoke_eff;
  TASKHEADER_NAMIPOKE_EFFECT head;
  
  head.eff_npoke_eff = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_NAMIPOKE_EFFECT );
  head.type = type;
  head.efftask_namipoke = NULL;
  head.dead_flag = TRUE;
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &data_npoke_effTaskHeader, pos, 0, &head, 1 );
  return( task );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@�I���`�F�b�N
 * @param task FLDEFF_TASK
 * @retval BOOL TRUE=�I��
 * @note NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH�̂�
 */
//--------------------------------------------------------------
BOOL FLDEFF_NAMIPOKE_EFFECT_CheckTaskEnd( const FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_NAMIPOKE_EFFECT *work = FLDEFF_TASK_GetWork( (FLDEFF_TASK*)task );
    return( work->end_flag );
  }else{
    GF_ASSERT( 0 );
    return( TRUE );
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_effTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE_EFFECT *work = wk;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  const TASKHEADER_NAMIPOKE_EFFECT *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_npoke_eff->fectrl );
  
  switch( work->head.type ){
  case NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH:
    work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_npoke_eff->res_idx_taki_land, 0, NULL );
    break;
  case NAMIPOKE_EFFECT_TYPE_TAKI_START_F:
    work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_npoke_eff->res_idx_taki_start_f, 0, NULL );
    break;
  case NAMIPOKE_EFFECT_TYPE_TAKI_LOOP_F:
    work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_npoke_eff->res_idx_taki_loop_f, 0, NULL );
    break;
  case NAMIPOKE_EFFECT_TYPE_TAKI_START_S:
    work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_npoke_eff->res_idx_taki_start_s, 0, NULL );
    break;
  case NAMIPOKE_EFFECT_TYPE_TAKI_LOOP_S:
    work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, work->head.eff_npoke_eff->res_idx_taki_loop_s, 0, NULL );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  if( work->head.efftask_namipoke != NULL ){
    VecFx32 pos;
    VecFx32 tbl[NAMIPOKE_EFFECT_TYPE_MAX] =
    {
      {0,0,0},
      {0,0,0},
      {0,NUM_FX32(-4),0},
      {0,0,0},
      {0,0,0},
    };
    
    work->offset = tbl[work->head.type];
    
    FLDEFF_TASK_GetPos( work->head.efftask_namipoke, &pos );
    pos.x += work->offset.x;
    pos.y += work->offset.y;
    pos.z += work->offset.z;
    FLDEFF_TASK_SetPos( task, &pos );
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_effTask_Delete( FLDEFF_TASK *task, void *wk )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE_EFFECT *work = wk;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_npoke_eff->fectrl );
  FLD_G3DOBJ_CTRL_DeleteObject( obj_ctrl, work->obj_idx );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_effTask_Update( FLDEFF_TASK *task, void *wk )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE_EFFECT *work = wk;
  
  if( work->end_flag == TRUE && work->head.dead_flag == TRUE ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_npoke_eff->fectrl );
  
  if( work->head.type == NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH ){
    if( work->end_flag == FALSE ){
      if( FLD_G3DOBJ_CTRL_AnimeObject(
            obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
        work->end_flag = TRUE;
      }
    }
  }else{
    FLD_G3DOBJ_CTRL_LoopAnimeObject( obj_ctrl, work->obj_idx, FX32_ONE );
  }
  
  if( work->head.efftask_namipoke != NULL ){
    VecFx32 pos;
    FLDEFF_TASK_GetPos( work->head.efftask_namipoke, &pos );
    pos.x += work->offset.x;
    pos.y += work->offset.y;
    pos.z += work->offset.z;
    FLDEFF_TASK_SetPos( task, &pos );
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void npoke_effTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  GFL_G3D_OBJSTATUS *st;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  TASKWORK_NAMIPOKE_EFFECT *work = wk;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl(
      work->head.eff_npoke_eff->fectrl );
  st = FLD_G3DOBJ_CTRL_GetObjStatus( obj_ctrl, work->obj_idx );
  FLDEFF_TASK_GetPos( task, &st->trans );
}

//--------------------------------------------------------------
/// �g���|�P�����G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_npoke_effTaskHeader = 
{
  sizeof(TASKWORK_NAMIPOKE_EFFECT),
  npoke_effTask_Init,
  npoke_effTask_Delete,
  npoke_effTask_Update,
  npoke_effTask_Draw,
};
