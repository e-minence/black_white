//======================================================================
/**
 * @file  fldmmdl_save.c
 * @brief  ���샂�f���@�Z�[�u����
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#define MMDL_SYSTEM_FILE
#include "fldmmdl.h"
#include "fldmmdl_work.h"

#include "arc/fieldmap/fldmmdl_mdlparam.naix"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  MMDL_SAVEWORK�\���� size 80
//--------------------------------------------------------------
typedef struct
{
  u32 status_bit;      ///<�X�e�[�^�X�r�b�g
  
#if 0 //wb �t���O���� ����r�b�g�̓N���A�����l�ɂ���
  u32 move_bit;        ///<����r�b�g
#else
  //�g�������l�����f�[�^�̈掩�͎̂c���Ă����B�����g��
  u8 gx_size;
  u8 gz_size;
  u8 padding[2];
#endif
  
  u8 obj_id;        ///<OBJ ID
  u8 move_code;       ///<����R�[�h
  s8 move_limit_x;    ///<X�����ړ�����
  s8 move_limit_z;    ///<Z�����ړ�����
  
  s8 dir_head;      ///<MMDL_H�w�����
  s8 dir_disp;      ///<���݌����Ă������
  s8 dir_move;      ///<���ݓ����Ă������
  u8 dummy;        ///<�_�~�[
  
  u16 zone_id;      ///<�]�[�� ID
  u16 obj_code;      ///<OBJ�R�[�h
  u16 event_type;      ///<�C�x���g�^�C�v
  u16 event_flag;      ///<�C�x���g�t���O
  u16 event_id;      ///<�C�x���gID
  
  u16 param0;        ///<�w�b�_�w��p�����^
  u16 param1;        ///<�w�b�_�w��p�����^
  u16 param2;        ///<�w�b�_�w��p�����^
  
  s16 gx_init;      ///<�����O���b�hX
  s16 gy_init;      ///<�����O���b�hY
  s16 gz_init;      ///<�����O���b�hZ
  s16 gx_now;        ///<���݃O���b�hX
  s16 gy_now;        ///<���݃O���b�hY
  s16 gz_now;        ///<���݃O���b�hZ
  
  fx32 fx32_y;      ///<fx32�^�̍����l
  
  u8 move_proc_work[MMDL_MOVE_WORK_SIZE];///<����֐��p���[�N
  u8 move_sub_proc_work[MMDL_MOVE_SUB_WORK_SIZE];///<����T�u�֐��p���[�N
}MMDL_SAVEWORK;

//--------------------------------------------------------------
///  MMDL_SAVEDATA�\����
//--------------------------------------------------------------
struct _TAG_MMDL_SAVEDATA
{
  MMDL_SAVEWORK SaveWorkBuf[MMDL_SAVEMMDL_MAX];
};

//======================================================================
//  proto
//======================================================================
static void mmdl_SaveData_SaveMMdl(
  const MMDL *mmdl, MMDL_SAVEWORK *save );
static void mmdl_SaveData_LoadMMdl(
  MMDL *mmdl, const MMDL_SAVEWORK *save, MMDLSYS *fos );

static BOOL mmdlsys_SearchUseMMdl(
  const MMDLSYS *fos, MMDL **mmdl, u32 *no );
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys );

#if 0
static void mmdlsys_GetOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, OBJCODE_PARAM *param );
#endif

//======================================================================
//  MMDL_SAVEDATA
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@�T�C�Y�擾
 * @param  nothing
 * @retval  u32  �T�C�Y
 */
//--------------------------------------------------------------
u32 MMDL_SAVEDATA_GetWorkSize( void )
{
  return( sizeof(MMDL_SAVEDATA) );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@������
 * @param  p  MMDL_SAVEDATA
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Init( void *p )
{
  MI_CpuClear8( p, MMDL_SAVEDATA_GetWorkSize() );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���Z�[�u
 * @param  mmdlsys �Z�[�u����MMDLSYS
 * @param  savedata LDMMDL_SAVEDATA
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Save(
  MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata )
{
  u32 no = 0;
  MMDL *mmdl;
  MMDL_SAVEWORK *save;
  
  //�Z�[�u�O�Ƀo�b�t�@���N���A
  MMDL_SAVEDATA_Init( savedata );
  save = savedata->SaveWorkBuf;
  
  while( mmdlsys_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( (mmdl->move_bit & MMDL_MOVEBIT_NOT_SAVE) == 0 ){
      mmdl_SaveData_SaveMMdl( mmdl, save );
      save++;
    }
  }
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f�����[�h
 * @param  mmdlsys  MMDLSYS
 * @param  save  ���[�h����MMDL_SAVEWORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Load(
  MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata )
{
  u32 no = 0;
  MMDL *mmdl;
  MMDL_SAVEWORK *save = savedata->SaveWorkBuf;
  
  while( no < MMDL_SAVEMMDL_MAX ){
    if( (save->status_bit&MMDL_STABIT_USE) ){
      mmdl = mmdlsys_SearchSpaceMMdl( mmdlsys );
      mmdl_SaveData_LoadMMdl( mmdl, save, mmdlsys );
    }
    save++;
    no++;
  }

  mmdlsys->mmdl_count = no;
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���@�Z�[�u
 * @param  fldmmdl    �Z�[�u����MMDL*
 * @param  save MMDL_SAVEWORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SaveData_SaveMMdl(
  const MMDL *mmdl, MMDL_SAVEWORK *save )
{
  save->status_bit = mmdl->status_bit;
//save->move_bit = MMDL_GetMoveBit( mmdl );
  save->gx_size = mmdl->gx_size;
  save->gz_size = mmdl->gz_size;
  save->obj_id = mmdl->obj_id;
  save->zone_id = mmdl->zone_id;
  save->obj_code = mmdl->obj_code;
  save->move_code = mmdl->move_code;
  save->event_type = mmdl->event_type;
  save->event_flag = mmdl->event_flag;
  save->event_id = mmdl->event_id;
  save->dir_head = mmdl->dir_head;
  save->dir_disp = mmdl->dir_disp;
  save->dir_move = mmdl->dir_move;
  save->param0 = mmdl->param0;
  save->param1 = mmdl->param1;
  save->param2 = mmdl->param2;
  save->move_limit_x = mmdl->move_limit_x;
  save->move_limit_z = mmdl->move_limit_z;
  save->gx_init = mmdl->gx_init;
  save->gy_init = mmdl->gy_init;
  save->gz_init = mmdl->gz_init;
  save->gx_now = mmdl->gx_now;
  save->gy_now = mmdl->gy_now;
  save->gz_now = mmdl->gz_now;
  save->fx32_y = mmdl->vec_pos_now.y;
  
  MI_CpuCopy8( mmdl->move_proc_work,
    save->move_proc_work, MMDL_MOVE_WORK_SIZE );
  MI_CpuCopy8( mmdl->move_sub_proc_work,
    save->move_sub_proc_work, MMDL_MOVE_SUB_WORK_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���@���[�h
 * @param  fldmmdl    �Z�[�u����MMDL*
 * @param  save MMDL_SAVEWORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SaveData_LoadMMdl(
  MMDL *mmdl, const MMDL_SAVEWORK *save, MMDLSYS *fos )
{
  MI_CpuClear8( mmdl, MMDL_SIZE );

  mmdl->status_bit = save->status_bit;
//mmdl->move_bit = save->move_bit;
  mmdl->gx_size = save->gx_size;
  mmdl->gz_size = save->gz_size;
  mmdl->obj_id = save->obj_id;
  mmdl->zone_id = save->zone_id;
  mmdl->obj_code = save->obj_code; 
  mmdl->move_code = save->move_code;
  mmdl->event_type = save->event_type;
  mmdl->event_flag = save->event_flag;
  mmdl->event_id = save->event_id;
  mmdl->dir_head = save->dir_head;
  mmdl->dir_disp = save->dir_disp;
  mmdl->dir_move = save->dir_move;
  mmdl->param0 = save->param0;
  mmdl->param1 = save->param1;
  mmdl->param2 = save->param2;
  mmdl->move_limit_x = save->move_limit_x;
  mmdl->move_limit_z = save->move_limit_z;
  mmdl->gx_init = save->gx_init;
  mmdl->gy_init = save->gy_init;
  mmdl->gz_init = save->gz_init;
  mmdl->gx_now = save->gx_now;
  mmdl->gy_now = save->gy_now;
  mmdl->gz_now = save->gz_now;
  
  MI_CpuCopy8( save->move_proc_work,
    mmdl->move_proc_work, MMDL_MOVE_WORK_SIZE );
  MI_CpuCopy8( save->move_sub_proc_work,
    mmdl->move_sub_proc_work, MMDL_MOVE_SUB_WORK_SIZE );
  
  mmdl->pMMdlSys = fos;
  
  { //���W���A
    s16 grid;
    VecFx32 pos = {0,0,0};
    
    grid = mmdl->gx_now;
    mmdl->gx_old = grid;
    pos.x = GRID_SIZE_FX32( grid ) + MMDL_VEC_X_GRID_OFFS_FX32;
    
    grid = mmdl->gy_now;
    mmdl->gy_old = grid;
    pos.y = save->fx32_y; //�Z�[�u���̍�����M�p����
    
    grid = mmdl->gz_now;
    mmdl->gz_old = grid;
    pos.z = GRID_SIZE_FX32( grid ) + MMDL_VEC_Z_GRID_OFFS_FX32;
    
    mmdl->vec_pos_now = pos;
    
    //�X�e�[�^�X�r�b�g���ꎞ�I�ɍ~�낵
    //MMDLSYS_LoadOBJCodeParam()���o�����̑Ώۂ���O��
    mmdl->status_bit &= ~MMDL_STABIT_USE;
    MMDLSYS_LoadOBJCodeParam( fos, mmdl->obj_code, &mmdl->objcode_param );
    mmdl->status_bit |= MMDL_STABIT_USE;
    
    { //�Ǘ��\�w��I�t�Z�b�g���W�ݒ�
      const OBJCODE_PARAM *param = &mmdl->objcode_param;
      mmdl->offset_x = param->offs_x; //�I�t�Z�b�g
      mmdl->offset_y = param->offs_y;
      mmdl->offset_z = param->offs_z;
    }
  }
  
  { //�X�e�[�^�X�r�b�g���A
    mmdl->status_bit |= MMDL_STABIT_USE;
    mmdl->move_bit |= MMDL_MOVEBIT_ATTR_GET_ERROR; //Attribute�Ď擾
    
    if( (mmdl->status_bit & MMDL_STABIT_MOVEPROC_INIT) ){
      mmdl->move_bit |= MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER;
    }
  }
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �g�p���Ă���t�B�[���h���샂�f����T���B
 * @param  fos  MMDLSYS *
 * @param  mmdl  MMDL*�i�[��
 * @param  no  �����J�n���[�Nno�B�擪���猟������ۂ͏����l0���w��B
 * @retval BOOL TRUE=���샂�f���擾���� FALSE=no����I�[�܂Ō������擾����
 */
//--------------------------------------------------------------
static BOOL mmdlsys_SearchUseMMdl(
  const MMDLSYS *fos, MMDL **mmdl, u32 *no )
{
  u32 max = fos->mmdl_max;
  
  if( (*no) < max ){
    MMDL *check_obj = &(((MMDLSYS*)fos)->pMMdlBuf[*no]);
    
    do{
      (*no)++;
      if( (check_obj->status_bit & MMDL_STABIT_USE) ){
        *mmdl = check_obj;
        return( TRUE );
      }
      check_obj++;
    }while( (*no) < max );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���̋󂫂�T��
 * @param  sys      MMDLSYS *
 * @retval  MMDL  �󂫂�MMDL*�@�󂫂������ꍇ��NULL
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys )
{
  int i,max;
  MMDL *mmdl;
  
  i = 0;
  max = sys->mmdl_max;
  mmdl = ((MMDLSYS*)sys)->pMMdlBuf;
  
  do{
    if( (mmdl->status_bit & MMDL_STABIT_USE) == 0 ){
      return( mmdl );
    }
    
    mmdl++;
    i++;
  }while( i < max );
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h���f�[�^�z��v�f���ɕϊ�
 * @param code OBJ CODE
 * @retval u16 code���f�[�^�̕��тɂ��킹�����l
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_OBJCodeToDataNumber( u16 code )
{
  if( code < OBJCODEEND_BBD ){
    return( code );
  }
  
  if( code >= OBJCODESTART_TPOKE && code < OBJCODEEND_TPOKE ){
    code = (code - OBJCODESTART_TPOKE) + OBJCODECOUNT_BBD;
    return( code );
  }
  
  if( code >= OBJCODESTART_MDL && code < OBJCODEEND_MDL ){
    code = (code - OBJCODESTART_MDL) + OBJCODECOUNT_BBD + OBJCODECOUNT_TPOKE;
    return( code );
  }
  
  GF_ASSERT( 0 );
  return( BOY1 ); //�G���[���p�Ƃ��Ė���ȃR�[�h��Ԃ�
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �擾
 * @param  mmdlsys  MMDLSYS *
 * @param  code  �擾����OBJ�R�[�h
 * @retval  OBJCODE_PARAM*
 */
//--------------------------------------------------------------
#if 0
static void mmdlsys_GetOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, OBJCODE_PARAM *param )
{
  u16 size = sizeof( OBJCODE_PARAM );
  u16 offs = OBJCODE_PARAM_TOTAL_NUMBER_SIZE +
    (size * MMDL_TOOL_OBJCodeToDataNumber(code));
  
  GFL_ARC_LoadDataOfsByHandle( mmdlsys->arcH_param, 
      NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
      offs, size, &param );
}
#endif
