//======================================================================
/*
 * @file	field_g3dobj.c
 * @brief	�t�B�[���h 3D�I�u�W�F�N�g
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_g3dobj.h"

//======================================================================
//  define
//======================================================================
enum
{
  TEX_TRANS_NON = 0,
  TEX_TRANS_SET,
  TEX_TRANS_WAIT,
};

enum
{
  OBJ_USE_FALSE = 0,
  OBJ_USE_TRUE,
  OBJ_USE_RES_WAIT,
};

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FLD_G3DOBJ_RES
//--------------------------------------------------------------
typedef struct
{
  GFL_G3D_RES *pResMdl;
  GFL_G3D_RES *pResTex; //NULL=pResMdl����e�N�X�`�����擾
  GFL_G3D_RES *pResAnmTbl[FLD_G3DOBJ_ANM_MAX];
  u8 anm_count;
  u8 tex_trans_flag;
  u8 padding[2]; //byte rest
}FLD_G3DOBJ_RES;

//--------------------------------------------------------------
/// FLD_G3DOBJ
//--------------------------------------------------------------
typedef struct
{
  u8 useFlag;
  u8 cullingFlag;
  u8 vanishFlag;
  u8 dmy;

  u16 resIdx; //�g�p���郊�\�[�X�C���f�b�N�X
  u16 mdlIdx; //�g�p���郂�f���C���f�b�N�X
    
  GFL_G3D_OBJSTATUS status;
  
  GFL_G3D_OBJ *pObj;
  GFL_G3D_ANM *pAnmTbl[FLD_G3DOBJ_ANM_MAX];
}FLD_G3DOBJ;

//--------------------------------------------------------------
/// FLD_G3DOBJ_CTRL
//--------------------------------------------------------------
struct _TAG_FLD_G3DOBJ_CTRL
{
  HEAPID heapID;
  u16 trans_flag;
  u16 res_max;
  u16 obj_max;
  FLD_G3DOBJ_RES *pResTbl;
  FLD_G3DOBJ *pObjTbl;
};

//======================================================================
//  proto
//======================================================================
static void transTexture( FLD_G3DOBJ_RES *res );
static void setResource(
    FLD_G3DOBJ_RES *res, HEAPID heapID,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag );
static void delResource( FLD_G3DOBJ_RES *res );
static void setObject( FLD_G3DOBJ *obj,
    FLD_G3DOBJ_RES *res, u32 resIdx, u32 mdlIdx, HEAPID heapID,
    BOOL init_status );
static void delObject( FLD_G3DOBJ *obj, const FLD_G3DOBJ_RES *res );

//======================================================================
//  FLD_G3DOBJ_CTRL  
//======================================================================
//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_CTRL�쐬
 * @param heapID �g�p����HEAPID
 * @param res_max �쐬���郊�\�[�X�ő吔
 * @param obj_max �쐬����OBJ�ő吔
 * @retval FLD_G3DOBJ_CTRL
 */
//--------------------------------------------------------------
FLD_G3DOBJ_CTRL * FLD_G3DOBJ_CTRL_Create(
    HEAPID heapID, u16 res_max, u16 obj_max )
{
  FLD_G3DOBJ_CTRL *ctrl;
  
  ctrl = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_G3DOBJ_CTRL) );
  
  ctrl->pResTbl = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(FLD_G3DOBJ_RES)*res_max );
  
  ctrl->pObjTbl = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(FLD_G3DOBJ)*obj_max );
  
  ctrl->heapID = heapID;
  ctrl->res_max = res_max;
  ctrl->obj_max = obj_max;
  ctrl->trans_flag = TRUE;
  return( ctrl );
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_CTRL�폜
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_Delete( FLD_G3DOBJ_CTRL *ctrl )
{
  u32 i;
  ctrl->trans_flag = FALSE;
  
  {
    FLD_G3DOBJ *obj = ctrl->pObjTbl;
    
    for( i = 0; i < ctrl->obj_max; i++, obj++ ){
      if( obj->useFlag == OBJ_USE_TRUE && obj->pObj != NULL ){
        u32 idx = obj->resIdx;
        delObject( obj, &ctrl->pResTbl[idx] );
      }
    }
    
    GFL_HEAP_FreeMemory( ctrl->pObjTbl );
  }
  
  {
    FLD_G3DOBJ_RES *res = ctrl->pResTbl;
    
    for( i = 0; i < ctrl->res_max; i++, res++ ){
      delResource( res );
    }

    GFL_HEAP_FreeMemory( ctrl->pResTbl );
  }
  
  GFL_HEAP_FreeMemory( ctrl );
}

//--------------------------------------------------------------
/**
 * �e�N�X�`���]�����N�G�X�g����
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_Trans( FLD_G3DOBJ_CTRL *ctrl )
{
  u32 i;
  
  if( ctrl->trans_flag == TRUE ){
    int i = 0;
    FLD_G3DOBJ *obj = ctrl->pObjTbl;
    FLD_G3DOBJ_RES *res = ctrl->pResTbl;
    
    while( i < ctrl->res_max ){
      if( res->tex_trans_flag == TEX_TRANS_WAIT ){
        transTexture( res );
      }
      res++;
      i++;
    }
    
    i = 0;
    res = ctrl->pResTbl;
    
    while( i < ctrl->obj_max ){
      if( obj->useFlag == OBJ_USE_RES_WAIT ){
        KAGAYA_Printf( "FLD G3DOBJ TransObject Index %xH\n", obj->resIdx );
        setObject( obj, &res[obj->resIdx],
            obj->resIdx, obj->mdlIdx, ctrl->heapID, FALSE );
      }
      
      obj++;
      i++;
    }
  }
}

//--------------------------------------------------------------
/**
 * �`��
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_Draw( FLD_G3DOBJ_CTRL *ctrl )
{
  u32 i = 0;
  FLD_G3DOBJ *obj = ctrl->pObjTbl;
  
  while( i < ctrl->obj_max ){
    if( obj->useFlag == OBJ_USE_TRUE ){
      if( obj->vanishFlag == FALSE ){
        if( obj->cullingFlag == TRUE ){
          GFL_G3D_DRAW_DrawObjectCullingON( obj->pObj, &obj->status );
        }else{
          GFL_G3D_DRAW_DrawObject( obj->pObj, &obj->status );
        }
      }
    }
    obj++;
    i++;
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�쐬
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval mdl �ǉ�����FLD_G3DMDL_ARCIDX
 * @retval tex �ǉ�����FLD_G3DTEX_ARCIDX NULL=mdl���̃e�N�X�`�����Q��
 * @retval anm �ǉ�����FLD_G3DANM_ARCIDX NULL=����
 * @param transFlag TRUE=�e�N�X�`�����]�� FALSE=FLD_G3DOBJ_CTRL_Trans()�œ]��
 * @retval u16 �o�^���ꂽ���\�[�X�C���f�b�N�X
 */
//--------------------------------------------------------------
u16 FLD_G3DOBJ_CTRL_CreateResource( FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag )
{
  u16 i;
  FLD_G3DOBJ_RES *res = ctrl->pResTbl;
  
  for( i = 0; i < ctrl->res_max; i++, res++ ){
    if( res->pResMdl == NULL ){
      setResource( res, ctrl->heapID, head, transFlag );
      return( i );
    }
  }
  
  GF_ASSERT( i );
  return( 0 );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�폜
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �폜���郊�\�[�X�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_DeleteResource( FLD_G3DOBJ_CTRL *ctrl, u16 idx )
{
  FLD_G3DOBJ_RES *res = &ctrl->pResTbl[idx];
  GF_ASSERT( res->pResMdl != NULL );
  delResource( res );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�ǉ�
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param resIdx �g�p���郊�\�[�X�C���f�b�N�X
 * @param mdlIdx resIdx�œo�^����Ă��郂�f���Ŏg�p����C���f�b�N�X
 * @retval u16 �ǉ����ꂽOBJ�C���f�b�N�X
 */
//--------------------------------------------------------------
u16 FLD_G3DOBJ_CTRL_AddObject(
    FLD_G3DOBJ_CTRL *ctrl, const u16 resIdx, u16 mdlIdx )
{
  u16 i;
  FLD_G3DOBJ *obj = ctrl->pObjTbl;
  
  for( i = 0; i < ctrl->obj_max; i++, obj++ ){
    if( obj->useFlag == OBJ_USE_FALSE && obj->pObj == NULL ){
      FLD_G3DOBJ_RES *res = &ctrl->pResTbl[resIdx];
      GF_ASSERT( res->tex_trans_flag != TEX_TRANS_NON );
      
      if( res->tex_trans_flag == TEX_TRANS_WAIT ){ //������
        obj->resIdx = resIdx;
        obj->mdlIdx = mdlIdx;
        obj->useFlag = OBJ_USE_RES_WAIT;
        
        obj->status.scale.x = FX32_ONE;
        obj->status.scale.y = FX32_ONE;
        obj->status.scale.z = FX32_ONE;
        MTX_Identity33( &obj->status.rotate );
      }else{
        setObject( obj, res, resIdx, mdlIdx, ctrl->heapID, TRUE );
      }
      return( i );
    }
  }
  
  GF_ASSERT( 0 );
  return( 0 );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�폜
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �폜����OBJ�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_DeleteObject( FLD_G3DOBJ_CTRL *ctrl, u16 idx )
{
  u16 flag;
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  
  ctrl->trans_flag = FALSE;
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    delObject( obj, &ctrl->pResTbl[obj->resIdx] );
  }
  
  ctrl->trans_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�C���f�b�N�X����GFL_G3D_OBJ�擾
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �擾����OBJ�C���f�b�N�X
 * @retval GFL_G3D_OBJ NULL=���\�[�X�o�^�҂��ɂ��܂��o�^����Ă��Ȃ�
 */
//--------------------------------------------------------------
GFL_G3D_OBJ * FLD_G3DOBJ_CTRL_GetObject( FLD_G3DOBJ_CTRL *ctrl, u16 idx )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  return( obj->pObj );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�C���f�b�N�X����GFL_G3D_OBJSTATUS�擾
 * @param ctrl  FLD_G3DOBJ_CTRL
 * @param idx �擾����OBJ�C���f�b�N�X
 * @retval GFL_G3D_OBJSTATUS
 */
//--------------------------------------------------------------
GFL_G3D_OBJSTATUS * FLD_G3DOBJ_CTRL_GetObjStatus(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  return( &obj->status );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�֕`�悷��ۂ̍��W�w��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param pos ���W
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetObjPos(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, const VecFx32 *pos )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  obj->status.trans = *pos;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�֕`�悷��ۂ̃J�����O�`�F�b�N�w��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param flag true=�J�����O����Bfalse=���Ȃ�
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetObjCullingFlag(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  obj->cullingFlag = flag;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�֕`�悷��ۂ̔�\���ݒ�
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param flag TRUE=��\�� FALSE=�\��
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetObjVanishFlag(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  obj->vanishFlag = flag;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F���[�v�A�j��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @retval BOOL TRUE=�A�j�����[�v�����B
 * @note �A�j���w�肪�����ꍇ�͉��������Ԃ�
 */
//--------------------------------------------------------------
BOOL FLD_G3DOBJ_CTRL_LoopAnimeObject( FLD_G3DOBJ_CTRL *ctrl, u16 idx )
{
  BOOL ret = FALSE;
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    int i = 0;
    int max = ctrl->pResTbl[obj->resIdx].anm_count;
    
    while( i < max ){
      if( GFL_G3D_OBJECT_LoopAnimeFrame(obj->pObj,i,FX32_ONE) == FALSE ){
        ret = TRUE;
      }
      i++;
    }
  }
  
  return( ret );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �e�N�X�`���Z�b�g
 * @param res FLD_G3DOBJ_RES
 * @retval nothing
 */
//--------------------------------------------------------------
static void transTexture( FLD_G3DOBJ_RES *res )
{
  res->tex_trans_flag = TEX_TRANS_NON; //re vwait
  
  if( res->pResTex != NULL ){
    GFL_G3D_TransVramTexture( res->pResTex );
  }else{
    GFL_G3D_TransVramTexture( res->pResMdl );
  }
  
  res->tex_trans_flag = TEX_TRANS_SET;
}

//--------------------------------------------------------------
/**
 * ���\�[�X�Z�b�g
 * @param res FLD_G3DOBJ_RES
 * @retval nothing
 */
//--------------------------------------------------------------
static void setResource(
    FLD_G3DOBJ_RES *res, HEAPID heapID,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag )
{
  if( head->arcHandleMdl != NULL ){
    res->pResMdl = GFL_G3D_CreateResourceHandle(
        head->arcHandleMdl, head->arcIdxMdl );
  }
  
  if( head->arcHandleTex != NULL ){
    res->pResTex = GFL_G3D_CreateResourceHandle(
        head->arcHandleTex, head->arcIdxTex );
  }
  
  if( res->pResMdl != NULL || res->pResTex != NULL ){
    if( transFlag == TRUE ){ //���]��
      transTexture( res );
    }else{
      res->tex_trans_flag = TEX_TRANS_WAIT; 
    }
  }
  
  if( head->arcHandleAnm != NULL ){
    int i;
    res->anm_count = head->anmCount;
    GF_ASSERT( res->anm_count );
    GF_ASSERT( res->anm_count < FLD_G3DOBJ_ANM_MAX );
    
    MI_CpuClear32( res->pResAnmTbl,
        sizeof(GFL_G3D_RES*)*FLD_G3DOBJ_ANM_MAX );
    
    for( i = 0; i < res->anm_count; i++ ){
      res->pResAnmTbl[i] = GFL_G3D_CreateResourceHandle(
          head->arcHandleAnm, head->arcIdxAnmTbl[i] );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�폜
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void delResource( FLD_G3DOBJ_RES *res )
{
  u32 flag = res->tex_trans_flag;
  
  res->tex_trans_flag = TEX_TRANS_NON; //�]���҂��폜�̍ۂ̊��荞�݂��l��
   
  if( flag == TEX_TRANS_SET ){
    if( res->pResTex != NULL ){
      GFL_G3D_FreeVramTexture( res->pResTex );
    }else if( res->pResMdl != NULL ){
      GFL_G3D_FreeVramTexture( res->pResMdl );
    }
  }
  
  if( res->pResTex != NULL ){
    GFL_G3D_DeleteResource( res->pResTex );
  }
  
  if( res->pResMdl != NULL ){
    GFL_G3D_DeleteResource( res->pResMdl );
  }
  
  if( res->anm_count ){
    int i;
    for( i = 0; i < res->anm_count; i++ ){
      GFL_G3D_DeleteResource( res->pResAnmTbl[i] );
    }
  }
  
  MI_CpuClear( res, sizeof(FLD_G3DOBJ_RES) );
  res->tex_trans_flag = TEX_TRANS_SET;
}

//--------------------------------------------------------------
/**
 * OBJ�Z�b�g
 * @param res FLD_G3DOBJ_RES
 * @param resIdx �g�p���郊�\�[�X�C���f�b�N�X
 * @param mdlIdx �g�p���郂�f���C���f�b�N�X
 * @param heapID HEAPID
 * @param init_status �X�e�[�^�X����������
 * @retval nothing
 */
//--------------------------------------------------------------
static void setObject( FLD_G3DOBJ *obj,
    FLD_G3DOBJ_RES *res, u32 resIdx, u32 mdlIdx, HEAPID heapID,
    BOOL init_status )
{
  int i;
  GFL_G3D_RND *pRnd;
  GFL_G3D_RES *pResTex = NULL;
  
  obj->useFlag = OBJ_USE_TRUE;
  obj->resIdx = resIdx;
  obj->mdlIdx = mdlIdx;
  
  GF_ASSERT( res->tex_trans_flag == TEX_TRANS_SET );

  if( res->tex_trans_flag == TEX_TRANS_SET ){
    pResTex = res->pResMdl;
    
    if( res->pResTex != NULL ){
      pResTex = res->pResTex;
    }
  }
  
  pRnd = GFL_G3D_RENDER_Create( res->pResMdl, mdlIdx, pResTex );
  
  if( res->anm_count ){
    MI_CpuClear32( obj->pAnmTbl, sizeof(GFL_G3D_ANM*)*FLD_G3DOBJ_ANM_MAX );
    
    for( i = 0; i < res->anm_count; i++ ){
      obj->pAnmTbl[i] = GFL_G3D_ANIME_Create( pRnd, res->pResAnmTbl[i], 0 );
    }
  }
  
  obj->pObj = GFL_G3D_OBJECT_Create( pRnd, obj->pAnmTbl, res->anm_count );
  
  if( res->anm_count ){
    for( i = 0; i < res->anm_count; i++ ){
	    GFL_G3D_OBJECT_EnableAnime( obj->pObj, i );
    }
  }
  
  if( init_status ){
    obj->status.scale.x = FX32_ONE;
    obj->status.scale.y = FX32_ONE;
    obj->status.scale.z = FX32_ONE;
    MTX_Identity33( &obj->status.rotate );
  }
}

//--------------------------------------------------------------
/**
 * OBJ�폜
 * @param obj FLD_G3DOBJ
 * @param res FLD_G3DOBJ_RES
 * @retval nothing
 */
//--------------------------------------------------------------
static void delObject( FLD_G3DOBJ *obj, const FLD_G3DOBJ_RES *res )
{
  if( obj->pAnmTbl != NULL ){
    int i;
    for( i = 0; i < res->anm_count; i++ ){
      GFL_G3D_ANIME_Delete( obj->pAnmTbl[i] );
    }
  }
  
  if( obj->pObj != NULL ){
    GFL_G3D_RND *pRnd;
    pRnd = GFL_G3D_OBJECT_GetG3Drnd( obj->pObj );
    GFL_G3D_OBJECT_Delete( obj->pObj );
    GFL_G3D_RENDER_Delete( pRnd );
  }
  
  MI_CpuClear( obj, sizeof(FLD_G3DOBJ) );
}

//======================================================================
//  FLD_G3DOBJ_RES_HEADER
//======================================================================
//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_RES_HEADER ������
 * @param head FLD_G3OBJ_RES_HEADER
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_Init( FLD_G3DOBJ_RES_HEADER *head )
{
  MI_CpuClear32( head, sizeof(FLD_G3DOBJ_RES_HEADER) );
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_RES_HEADER ���f���Z�b�g
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ���[�h��ARCHANDLE*
 * @param idx handle���烍�[�h����f�[�^�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_SetMdl(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle, u16 idx )
{
  GF_ASSERT( head->arcHandleMdl == NULL );
  head->arcHandleMdl = handle;
  head->arcIdxMdl = idx;
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_RES_HEADER �e�N�X�`���Z�b�g
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ���[�h��ARCHANDLE*
 * @param idx handle���烍�[�h����f�[�^�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_SetTex(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle, u16 idx )
{
  GF_ASSERT( head->arcHandleTex == NULL );
  head->arcHandleTex = handle;
  head->arcIdxTex = idx;
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_RES_HEADER �A�j���A�[�J�C�u�n���h���Z�b�g
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ���[�h��ARCHANDLE*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle )
{
  GF_ASSERT( head->arcHandleAnm == NULL );
  head->arcHandleAnm = handle;
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_RES_HEADER �A�j���Z�b�g
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ���[�h��ARCHANDLE*
 * @retval nothing
 * @attention �����Ă��̊֐����ĂԎ��ŕ����o�^���\�B
 * �ő�o�^����FLD_G3DOBJ_ANM_MAX�B
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
    FLD_G3DOBJ_RES_HEADER *head, u16 idx )
{
  GF_ASSERT( head->anmCount < FLD_G3DOBJ_ANM_MAX );
  head->arcIdxAnmTbl[head->anmCount] = idx;
  head->anmCount++;
}
