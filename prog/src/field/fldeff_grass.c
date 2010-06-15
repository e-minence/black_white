//======================================================================
/**
 * @file	fldeff_grass.c
 * @brief	�t�B�[���h ���G�t�F�N�g
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_grass.h"

#include "include/gamesystem/pm_season.h"

//======================================================================
//  define
//======================================================================
#define GRASS_SHAKE_FRAME (FX32_ONE*12)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_GRASS�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_GRASS FLDEFF_GRASS;

//--------------------------------------------------------------
///	FLDEFF_GRASS�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_GRASS
{
	FLDEFF_CTRL *fectrl;
  GFL_BBDACT_SYS *bbdact_sys;
  u16 res_idx_tbl[FLDEFF_GRASS_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_GRASS
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_GRASS *eff_grass;
  MMDL *fmmdl;
  int init_gx;
  int init_gz;
  u16 obj_id;
  u16 zone_id;
  VecFx32 pos;
  FLDEFF_GRASSTYPE type;
}TASKHEADER_GRASS;

//--------------------------------------------------------------
/// TASKWORK_GRASS
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  TASKHEADER_GRASS head;
  MMDL_CHECKSAME_DATA samedata;
  u16 act_id;
}TASKWORK_GRASS;

//--------------------------------------------------------------
/// GRASS_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  u16 idx_mdl;
}GRASS_ARCIDX;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void grass_InitResource( FLDEFF_GRASS *grass );
static void grass_DeleteResource( FLDEFF_GRASS *grass );

static const FLDEFF_TASK_HEADER DATA_grassTaskHeader;

static BOOL check_MMdlPos( TASKWORK_GRASS *work );

static const GRASS_ARCIDX data_ArcIdxTbl[FLDEFF_GRASS_MAX][PMSEASON_TOTAL];
static const GFL_BBDACT_ANM * const * data_BlActAnm_GrassTbl[FLDEFF_GRASS_MAX];
static const u16 data_BlActAnmEndNo[FLDEFF_GRASS_MAX];
static const u8 data_LongShortType[FLDEFF_GRASS_MAX];

//======================================================================
//	���G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ���G�t�F�N�g ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_GRASS_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_GRASS *grass;
	FIELDMAP_WORK *fieldmap;

	grass = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_GRASS) );
	grass->fectrl = fectrl;
  
  fieldmap = FLDEFF_CTRL_GetFieldMapWork( grass->fectrl );
  grass->bbdact_sys = FIELDMAP_GetEffBbdActSys( fieldmap );
  
	grass_InitResource( grass );
	return( grass );
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_GRASS *grass = work;
  grass_DeleteResource( grass );
  GFL_HEAP_FreeMemory( grass );
}

//======================================================================
//	���G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_InitResource( FLDEFF_GRASS *grass )
{
  BOOL ret;
  int i,season;
  ARCHANDLE *handle;
  FIELDMAP_WORK *fieldmap;
  GAMESYS_WORK *gsys;
  GFL_BBDACT_G3DRESDATA data;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( grass->fectrl );
  fieldmap = FLDEFF_CTRL_GetFieldMapWork( grass->fectrl );
  gsys = FIELDMAP_GetGameSysWork( fieldmap );
  season = FLDEFF_CTRL_GetSeasonID( grass->fectrl );
  
  data.texFmt = GFL_BBD_TEXFMT_PAL16;
  data.texSiz = GFL_BBD_TEXSIZ_32x128;
  data.celSizX = 32;
  data.celSizY = 32;
  data.dataCut = GFL_BBDACT_RESTYPE_DATACUT;
  
  for( i = 0; i < FLDEFF_GRASS_MAX; i++ ){
    data.g3dres = GFL_G3D_CreateResourceHandle(
          handle, data_ArcIdxTbl[i][season].idx_mdl );
    FLDEFF_CTRL_SetGrayScaleG3DResource( grass->fectrl, data.g3dres );
    grass->res_idx_tbl[i] =
      GFL_BBDACT_AddResourceG3DResUnit( grass->bbdact_sys, &data, 1 );
  }
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_DeleteResource( FLDEFF_GRASS *grass )
{
  int i;
  
  for( i = 0; i < FLDEFF_GRASS_MAX; i++ ){
    GFL_BBDACT_RemoveResourceUnit(
        grass->bbdact_sys, grass->res_idx_tbl[i], 1 );
  }
}

//======================================================================
//	���G�t�F�N�g�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * FLDEFF_GRASSTYPE����FLDEFF_GRASSLEN���擾
 * @param type FLDEFF_GRASSTYPE
 * @retval FLDEFF_GRASSLEN
 */
//--------------------------------------------------------------
FLDEFF_GRASSLEN FLDEFF_GRASS_GetLenType( FLDEFF_GRASSTYPE type )
{
  u8 len = data_LongShortType[type];
  return( len );
}

//--------------------------------------------------------------
/**
 * ���샂�f���p���G�t�F�N�g�@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_SetMMdl( FLDEFF_CTRL *fectrl,
    MMDL *fmmdl, BOOL anm, FLDEFF_GRASSTYPE type )
{
  fx32 h;
  VecFx32 pos;
  FLDEFF_GRASS *grass;
  TASKHEADER_GRASS head;
  
  if( type >= FLDEFF_GRASS_MAX ){
    GF_ASSERT( 0 );
    return;
  }
  
  grass = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GRASS );
  head.eff_grass = grass;
  head.fmmdl = fmmdl;
  head.type = type;
  head.obj_id = MMDL_GetOBJID( fmmdl );
  head.zone_id = MMDL_GetZoneID( fmmdl );
  head.init_gx = MMDL_GetGridPosX( fmmdl );
  head.init_gz = MMDL_GetGridPosZ( fmmdl );
  MMDL_TOOL_GetCenterGridPos( head.init_gx, head.init_gz, &pos );
  pos.y = MMDL_GetVectorPosY( fmmdl );
  
  // ���[������̐ݒ�
  if( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    #if 0 //old
    pos.y += FX32_CONST(4);  
    #else
    pos.y += NUM_FX32(7);
    pos.z += NUM_FX32(-2);
    #endif
  }
  // �O���b�h����̐ݒ�
  else
  {
    MMDL_GetMapPosHeight( fmmdl, &pos, &h );
    pos.y = h;

    /*
    pos.y += NUM_FX32(10); //�҂����肾�����
    */
    pos.y += NUM_FX32(9);
    pos.z += NUM_FX32(-2);
  }
  
  head.pos = pos;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_grassTaskHeader, NULL, anm, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Init( FLDEFF_TASK *task, void *wk )
{
  u16 type;
  TASKWORK_GRASS *work = wk;
  const TASKHEADER_GRASS *head;
  GFL_BBDACT_ACTDATA actData;
  GFL_BBDACT_SYS *bbdact_sys;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  
  MMDL_InitCheckSameData( head->fmmdl, &work->samedata );
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  type = work->head.type;
  
  actData.resID = work->head.eff_grass->res_idx_tbl[type];
  actData.sizX = FX16_ONE*2;
  actData.sizY = FX16_ONE*2;
  actData.alpha = 31;
  actData.drawEnable = TRUE;
  actData.lightMask = GFL_BBD_LIGHTMASK_0;
  actData.trans = head->pos;
  actData.func = NULL;
  actData.work = work;
  
  bbdact_sys = work->head.eff_grass->bbdact_sys;
  
  work->act_id = GFL_BBDACT_AddAct( bbdact_sys, 0, &actData, 1 );
  
  if( work->act_id == GFL_BBDACT_ACTUNIT_ID_INVALID ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }
  
  GFL_BBDACT_SetAnimeTable( bbdact_sys, work->act_id,  
      (GFL_BBDACT_ANMTBL)data_BlActAnm_GrassTbl[type], 1 );
  GFL_BBDACT_SetAnimeIdxOn( bbdact_sys, work->act_id, 0 );
  
  if( FLDEFF_TASK_GetAddParam(task) == FALSE ){ //�A�j������
    work->seq_no = 1;
  }
  
  FLDEFF_TASK_CallUpdate( task ); //������
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  GFL_BBDACT_SYS *bbdact_sys = work->head.eff_grass->bbdact_sys;
  if( work->act_id != GFL_BBDACT_ACTUNIT_ID_INVALID ){
    GFL_BBDACT_RemoveAct( bbdact_sys, work->act_id, 1 );
  }
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Update( FLDEFF_TASK *task, void *wk )
{
  u16 comm;
  TASKWORK_GRASS *work = wk;
  GFL_BBDACT_SYS *bbdact_sys = work->head.eff_grass->bbdact_sys;
 
  switch( work->seq_no ){
  case 0: //�A�j���I���҂�
    #if 1 //BTS4949�@���ނ��������Ƃ��s���R
    {
      //�A�j�����ł��A�������ŁA���ړ��ō��W���X�V���ꂽ�瑦����
      u8 len = data_LongShortType[work->head.type];
      
      if( len == FLDEFF_GRASSLEN_LONG ){
        MMDL *mmdl = work->head.fmmdl;
        
        if( MMDL_CheckSameData(mmdl,&work->samedata) == TRUE ){
          if( check_MMdlPos(work) == FALSE ){
            if( MMDL_GetDirMove(mmdl) == DIR_DOWN ){
              FLDEFF_TASK_CallDelete( task );
              return;
            }
          }
        }
      }
    }
    #endif

    if( GFL_BBDACT_GetAnimeLastCommand(bbdact_sys,work->act_id,&comm) ){
      work->seq_no++;
    }
    break;
  case 1: //�A�j���I���ʒu��
    if( MMDL_CheckSameData(work->head.fmmdl,&work->samedata) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
    
    GFL_BBDACT_SetAnimeFrmIdx(
        bbdact_sys, work->act_id, data_BlActAnmEndNo[work->head.type] );
    work->seq_no++;
    break;
  case 2: //�A�j���|�[�Y
    //�r���{�[�h�A�N�^�[�X�V�ɂ���Ă�seq1�̏��������f����Ă��Ȃ��ꍇ������
    //�A�j�����I�[�܂ŗ������őΏ�
#if 0
    GFL_BBDACT_SetAnimeEnable( bbdact_sys, work->act_id, FALSE );
#endif
    work->seq_no++;
  case 3:
    if( MMDL_CheckSameData(work->head.fmmdl,&work->samedata) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
    
    if( check_MMdlPos(work) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Draw( FLDEFF_TASK *task, void *wk )
{
#if 0 //field_g3dobj�𗘗p����
  VecFx32 pos;
  TASKWORK_GRASS *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
#endif
}

//--------------------------------------------------------------
//  ���G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_grassTaskHeader =
{
  sizeof(TASKWORK_GRASS),
  grassTask_Init,
  grassTask_Delete,
  grassTask_Update,
  grassTask_Draw,
};

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�p���W�`�F�b�N����
 * @param work TASKWORK_GRASS
 * @retval BOOL TRUE=���W�ύX�Ȃ� FALSE=���W�ύX����
 * 
 * �ȉ��̑Ώ��ׂ̈̏����B
 * BTS4094�y���ނ����������̕\���D�揇�ʂ��s���R�z
 * ���ނ����ʏ�����ɕ����ƁA�������L�����̎�O��
 * �s���R�ɕ\�������悤�Ɍ����܂��B
 * �E���ނ�A�AB�œ��l�ł��B
 * �E�������ނ�̏ꍇ��������ɕ����Ə��������s���R�Ɍ����܂��B
 * �E������̏ꍇ�����ނ���������≡�����ɕ������ꍇ�͂��܂�ڗ����܂���B
 */
//--------------------------------------------------------------
static BOOL check_MMdlPos( TASKWORK_GRASS *work )
{
  const MMDL *mmdl = work->head.fmmdl;
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
  u8 len = data_LongShortType[work->head.type];
  
  if( work->head.init_gx != gx || work->head.init_gz != gz ){
    #if 1 //BTS4949�@���ނ��������Ƃ��s���R
    //�Z�����ňړ�����������Ȃ�΁A���S�ɍ��W���؂�ւ��܂ő҂�
    if( len == FLDEFF_GRASSLEN_SHORT ){
      if( MMDL_GetDirMove(mmdl) == DIR_UP ){
        gx = MMDL_GetOldGridPosX( mmdl );
        gz = MMDL_GetOldGridPosZ( mmdl );
        
        if( work->head.init_gx == gx || work->head.init_gz == gz ){
          return( TRUE );
        }
      }
    }
    #endif
    return( FALSE );
  }
  
  return( TRUE );
}

//======================================================================
//  data
//======================================================================
static const GRASS_ARCIDX data_ArcIdxTbl[FLDEFF_GRASS_MAX][PMSEASON_TOTAL] =
{
  { //�Z����
    { NARC_fldeff_kusaeff_sp_nsbtx },
    { NARC_fldeff_kusaeff_sm_nsbtx },
    { NARC_fldeff_kusaeff_at_nsbtx },
    { NARC_fldeff_kusaeff_wt_nsbtx },
  },
  { //�Z���� ��
    { NARC_fldeff_kusaeff_sp2_nsbtx },
    { NARC_fldeff_kusaeff_sm2_nsbtx },
    { NARC_fldeff_kusaeff_at2_nsbtx },
    { NARC_fldeff_kusaeff_wt2_nsbtx },
  },
  { //������
    { NARC_fldeff_lgrass_sp_nsbtx },
    { NARC_fldeff_lgrass_sm_nsbtx },
    { NARC_fldeff_lgrass_at_nsbtx },
    { NARC_fldeff_lgrass_wt_nsbtx },
  },
  { //������ ��
    { NARC_fldeff_lgrass_sp2_nsbtx },
    { NARC_fldeff_lgrass_sm2_nsbtx },
    { NARC_fldeff_lgrass_at2_nsbtx },
    { NARC_fldeff_lgrass_wt2_nsbtx },
  },
  { //����n�� �P�V�[�Y���̂�
    { NARC_fldeff_kusaeff_sn_nsbtx },
    { NARC_fldeff_kusaeff_sn_nsbtx },
    { NARC_fldeff_kusaeff_sn_nsbtx },
    { NARC_fldeff_kusaeff_sn_nsbtx },
  },
  { //����n�� �� �P�V�[�Y���̂�
    { NARC_fldeff_kusaeff_sn2_nsbtx },
    { NARC_fldeff_kusaeff_sn2_nsbtx },
    { NARC_fldeff_kusaeff_sn2_nsbtx },
    { NARC_fldeff_kusaeff_sn2_nsbtx },
  },
  { //�l�G�ω��Ȃ����@��
    { NARC_fldeff_kusaeff_sp_nsbtx },
    { NARC_fldeff_kusaeff_sp_nsbtx },
    { NARC_fldeff_kusaeff_sp_nsbtx },
    { NARC_fldeff_kusaeff_sp_nsbtx },
  },
  { //�l�G�ω��Ȃ����@��
    { NARC_fldeff_kusaeff_sp2_nsbtx },
    { NARC_fldeff_kusaeff_sp2_nsbtx },
    { NARC_fldeff_kusaeff_sp2_nsbtx },
    { NARC_fldeff_kusaeff_sp2_nsbtx },
  },
};

//�Z�����A�j��
static const GFL_BBDACT_ANM data_BlActAnm_ShortGrass[] = {
  {0,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,3},
  {1,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,3},
  {2,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,3},
  {3,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,4},
  {GFL_BBDACT_ANMCOM_END,0,0,0},
};

static const GFL_BBDACT_ANM * data_BlActAnm_ShortGrassTbl[] =
{
  data_BlActAnm_ShortGrass,
};

//�������A�j��
static const GFL_BBDACT_ANM data_BlActAnm_LongGrass[] = {
  {0,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {1,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {0,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {1,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {0,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {1,GFL_BBDACT_ANMFLIP_OFF,GFL_BBDACT_ANMFLIP_OFF,2},
  {GFL_BBDACT_ANMCOM_END,0,0,0},
};

static const GFL_BBDACT_ANM * data_BlActAnm_LongGrassTbl[] =
{
  data_BlActAnm_LongGrass,
};

//��ޕʃA�j��
static const GFL_BBDACT_ANM * const * data_BlActAnm_GrassTbl[FLDEFF_GRASS_MAX] =
{
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_SHORT
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_SHORT2
  data_BlActAnm_LongGrassTbl, //FLDEFF_GRASS_LONG
  data_BlActAnm_LongGrassTbl, //FLDEFF_GRASS_LONG2
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_SNOW
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_SNOW2
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_ALLYEAR_SHORT
  data_BlActAnm_ShortGrassTbl, //FLDEFF_GRASS_ALLYEAR_SHORT2
};

//��ޕʃA�j���I���ʒu
static const u16 data_BlActAnmEndNo[FLDEFF_GRASS_MAX] =
{
  3, //FLDEFF_GRASS_SHORT
  3, //FLDEFF_GRASS_SHORT2
  5, //FLDEFF_GRASS_LONG
  5, //FLDEFF_GRASS_LONG2
  3, //FLDEFF_GRASS_SNOW
  3, //FLDEFF_GRASS_SNOW2
  3, //FLDEFF_GRASS_ALLYEAR_SHORT
  3, //FLDEFF_GRASS_ALLYEAR_SHORT2
};

//��ޕʒ��Z
static const u8 data_LongShortType[FLDEFF_GRASS_MAX] = 
{
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_SHORT
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_SHORT2
  FLDEFF_GRASSLEN_LONG, //FLDEFF_GRASS_LONG
  FLDEFF_GRASSLEN_LONG, //FLDEFF_GRASS_LONG2
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_SNOW
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_SNOW2
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_ALLYEAR_SHORT
  FLDEFF_GRASSLEN_SHORT, //FLDEFF_GRASS_ALLYEAR_SHORT2
};
