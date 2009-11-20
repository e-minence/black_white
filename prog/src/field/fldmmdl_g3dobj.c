//======================================================================
/**
 * @file	fldmmdl_g3dobj.c
 * @brief	���샂�f���@G3D�I�u�W�F�Ǘ�
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "field_g3dobj.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// RESCODE
//--------------------------------------------------------------
typedef struct
{
  u16 residx;
  u16 code;
}RESCODE;

//--------------------------------------------------------------
/// MMDL_G3DOBJCONT
//--------------------------------------------------------------
struct _TAG_MMDL_G3DOBJCONT
{
  
  MMDLSYS *mmdlsys;
  FLD_G3DOBJ_CTRL *g3dobj_ctrl;

  u16 max;
  RESCODE *restbl;
};

//======================================================================
//  proto
//======================================================================
static void rescode_Init( MMDL_G3DOBJCONT *objcont, HEAPID heapID );
static void rescode_Delete( MMDL_G3DOBJCONT *objcont );
static u16 rescode_RegCodeIdx( MMDL_G3DOBJCONT *objcont, u16 code, u16 idx );
static u16 rescode_GetResIdx( MMDL_G3DOBJCONT *objcont, u16 code );
static u16 rescode_GetOBJCode( MMDL_G3DOBJCONT *objcont, u16 idx );
static u16 rescode_AddResource(
    MMDLSYS *mmdlsys, MMDL_G3DOBJCONT *objcont, const u16 code );
static void rescode_DeleteResource( MMDL_G3DOBJCONT *objcont, u16 code );

//======================================================================
//  ���샂�f���@G3D�I�u�W�F�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_G3DOBJCONT G3D�I�u�W�F�Ǘ��@�Z�b�g�A�b�v
 * @param mmdlsys MMDLSYS
 * @param g3dobj_ctrl FLD_G3DOBJ_CTRL
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_G3DOBJCONT_Setup( MMDLSYS *mmdlsys, FLD_G3DOBJ_CTRL *g3dobj_ctrl )
{
  HEAPID heapID;
  MMDL_G3DOBJCONT *objcont;
  
  heapID = MMDLSYS_GetHeapID( mmdlsys );
  
  objcont =
    GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_G3DOBJCONT) );
  objcont->mmdlsys = mmdlsys;
  objcont->g3dobj_ctrl = g3dobj_ctrl;
  objcont->max = MMDLSYS_GetMMdlMax( mmdlsys );
  
  rescode_Init( objcont, heapID );
  MMDLSYS_SetG3dObjCont( mmdlsys, objcont );
}

//--------------------------------------------------------------
/**
 * MMDL_G3DOBJCONT G3D�I�u�W�F�Ǘ��@�폜
 * @param mmdlsys MMDLSYS
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_G3DOBJCONT_Delete( MMDLSYS *mmdlsys )
{
  MMDL_G3DOBJCONT *objcont;
  objcont = MMDLSYS_GetG3dObjCont( mmdlsys );
  rescode_Delete( objcont );
  GFL_HEAP_FreeMemory( objcont );
}

//--------------------------------------------------------------
/**
 * MMDL_G3DOBJCONT FLD_G3DOBJ_CTRL�擾
 * @param mmdlsys MMDLSYS
 * @retval FLD_G3DOBJ_CTRL
 */
//--------------------------------------------------------------
FLD_G3DOBJ_CTRL * MMDL_G3DOBJCONT_GetFldG3dObjCtrl( MMDL *mmdl )
{
  MMDLSYS *mmdlsys;
  MMDL_G3DOBJCONT *objcont;
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  objcont = MMDLSYS_GetG3dObjCont( mmdlsys );
  return( objcont->g3dobj_ctrl );
}

//======================================================================
//  ���\�[�X�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@������
 * @param objcont MMDL_G3DOBJCONT
 * @retval nothing
 */
//--------------------------------------------------------------
static void rescode_Init( MMDL_G3DOBJCONT *objcont, HEAPID heapID )
{
  int i;

  objcont->restbl = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(RESCODE)*objcont->max );
  
  for( i = 0; i < objcont->max; i++ ){
    objcont->restbl[i].code = OBJCODEMAX;
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�폜
 * @param objcont MMDL_G3DOBJCONT
 * @retval nothing
 */
//--------------------------------------------------------------
static void rescode_Delete( MMDL_G3DOBJCONT *objcont )
{
  int i;
  RESCODE *tbl = objcont->restbl;
  
  for( i = 0; i < objcont->max; i++, tbl++ ){
    if( tbl->code != OBJCODEMAX ){
      FLD_G3DOBJ_CTRL_DeleteResource( objcont->g3dobj_ctrl, tbl->residx );
    }
  }
  
  GFL_HEAP_FreeMemory( objcont->restbl );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�o�^
 * @param objcont MMDL_G3DOBJCONT
 * @param code
 * @param idx
 * @retval nothing
 */
//--------------------------------------------------------------
static u16 rescode_RegCodeIdx( MMDL_G3DOBJCONT *objcont, u16 code, u16 idx )
{
  int i;
  RESCODE *tbl = objcont->restbl;
  
  for( i = 0; i < objcont->max; i++, tbl++ ){
    if( tbl->code == OBJCODEMAX ){
      tbl->code = code;
      tbl->residx = idx;
      return;
    }
  }
  
  GF_ASSERT( 0 );
  return( 0 );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�w��R�[�h�̃��\�[�X�C���f�b�N�X��Ԃ�
 * @param objcont MMDL_G3DOBJCONT
 * @retval u16 ���\�[�X�C���f�b�N�X FLD_G3DOBJ_IDX_ERROR=���o�^
 */
//--------------------------------------------------------------
static u16 rescode_GetResIdx( MMDL_G3DOBJCONT *objcont, u16 code )
{
  int i;
  RESCODE *tbl = objcont->restbl;
  
  for( i = 0; i < objcont->max; i++, tbl++ ){
    if( tbl->code == code ){
      return( tbl->residx );
    }
  }
  return( FLD_G3DOBJ_IDX_ERROR );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�w��C���f�b�N�X�̃R�[�h��Ԃ�
 * @param objcont MMDL_G3DOBJCONT
 * @retval u16 ���\�[�X�C���f�b�N�X FLD_G3DOBJ_IDX_ERROR=���o�^
 */
//--------------------------------------------------------------
static u16 rescode_GetOBJCode( MMDL_G3DOBJCONT *objcont, u16 idx )
{
  int i;
  RESCODE *tbl = objcont->restbl;
  
  for( i = 0; i < objcont->max; i++, tbl++ ){
    if( tbl->code != OBJCODEMAX ){
      if( tbl->residx == idx ){
        return( tbl->code );
      }
    }
  }
  return( OBJCODEMAX );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�w��R�[�h�̃��\�[�X��ǉ�
 * @param objcont MMDL_G3D
 * @param code �ǉ�����OBJ�R�[�h
 * @retval idx ���\�[�X�C���f�b�N�X
 */
//--------------------------------------------------------------
static u16 rescode_AddResource(
    MMDLSYS *mmdlsys, MMDL_G3DOBJCONT *objcont, const u16 code )
{
  u32 anmtbl[3];
  ARCHANDLE *handle = MMDLSYS_GetResArcHandle( mmdlsys );
  FLD_G3DMDL_ARCIDX mdl;
  FLD_G3DTEX_ARCIDX tex;
  FLD_G3DANM_ARCIDX anm;
  FLD_G3DTEX_ARCIDX *p_tex = NULL;
  FLD_G3DANM_ARCIDX *p_anm = NULL;
  const OBJCODE_PARAM *prm;
  const OBJCODE_PARAM_BUF_MDL *prm_mdl;
  
  prm = MMDLSYS_GetOBJCodeParam( objcont->mmdlsys, code );
  prm_mdl = MMDL_GetOBJCodeParamBufMDL( prm );
  
  mdl.handle = handle;
  mdl.arcIdx = prm_mdl->res_idx_mdl;
  
  if( prm_mdl->res_idx_tex != 0xffff ){
    p_tex = &tex;
    tex.handle = handle;
    tex.arcIdx = prm_mdl->res_idx_tex;
  }
  
  if( prm_mdl->res_idx_anm0 != 0xffff ){
    p_anm = &anm;
    anm.arcIdxTbl = anmtbl;
    anm.handle = handle;
    
    anm.count = 1;
    anmtbl[0] = prm_mdl->res_idx_anm0;

    if( prm_mdl->res_idx_anm1 != 0xffff ){
      anm.count++;
      anmtbl[1] = prm_mdl->res_idx_anm1;
    }
    
    if( prm_mdl->res_idx_anm2 != 0xffff ){
      anm.count++;
      anmtbl[2] = prm_mdl->res_idx_anm2;
    }
  }
  
  {
    int count = 0;
    if( p_anm != NULL ){
      count = anm.count;
    }
    
    KAGAYA_Printf(
        "���샂�f�� G3DObject�ǉ� code=%dH, mdl idx = %d, anm count =%xH\n",
        code, mdl.arcIdx, count );
  }

  {
    u16 idx = FLD_G3DOBJ_CTRL_CreateResource(
      objcont->g3dobj_ctrl, &mdl, p_tex, p_anm, FALSE );
    rescode_RegCodeIdx( objcont, code, idx );
    return( idx );
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Ǘ��@�w�胊�\�[�X���폜
 * @param objcont MMDL_G3D
 * @param code �폜����OBJ�R�[�h
 * @retval nothing
 */
//--------------------------------------------------------------
static void rescode_DeleteResource( MMDL_G3DOBJCONT *objcont, u16 code )
{
  int i;
  RESCODE *tbl = objcont->restbl;
  
  for( i = 0; i < objcont->max; i++, tbl++ ){
    if( tbl->code == code ){
      FLD_G3DOBJ_CTRL_DeleteResource( objcont->g3dobj_ctrl, tbl->residx );
      tbl->code = OBJCODEMAX;
      return;
    }
  }
  
  GF_ASSERT( 0 ); //���o�^
}

//======================================================================
//  OBJ�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * OBJ�ǉ�
 * @param mmdl MMDL*
 * @param code �ǉ�����OBJ�R�[�h
 * @retval u16 �ǉ����ꂽ�I�u�W�F�C���f�b�N�X
 */
//--------------------------------------------------------------
FLD_G3DOBJ_OBJIDX MMDL_G3DOBJCONT_AddObject( MMDL *mmdl, u16 code )
{
  MMDLSYS *mmdlsys;
  MMDL_G3DOBJCONT *objcont;
  FLD_G3DOBJ_RESIDX residx;
  FLD_G3DOBJ_OBJIDX objidx;

  mmdlsys = MMDL_GetMMdlSys( mmdl );
  objcont = MMDLSYS_GetG3dObjCont( mmdlsys );
  residx = rescode_GetResIdx( objcont, code );
  
  if( residx == FLD_G3DOBJ_IDX_ERROR ){
    residx = rescode_AddResource( mmdlsys, objcont, code );
  }
  
  objidx = FLD_G3DOBJ_CTRL_AddObject( objcont->g3dobj_ctrl, residx, 0 );
  return( objidx );
}

//--------------------------------------------------------------
/**
 * OBJ�폜
 * @param mmdl MMDL*
 * @param idx �폜����I�u�W�F�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_G3DOBJCONT_DeleteObject( MMDL *mmdl, FLD_G3DOBJ_OBJIDX idx )
{
  u16 code;
  MMDLSYS *mmdlsys;
  MMDL_G3DOBJCONT *objcont;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  objcont = MMDLSYS_GetG3dObjCont( mmdlsys );
  code = rescode_GetOBJCode( objcont, idx );
  GF_ASSERT( code != OBJCODEMAX );
  
  FLD_G3DOBJ_DeleteObject( objcont->g3dobj_ctrl, idx );
  
  if( MMDL_SearchUseOBJCode(mmdl,code) == FALSE ){
    rescode_DeleteResource( objcont, code ); //���ɗ��p������΃��\�[�X���폜
  }
}
