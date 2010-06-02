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

#include "system/palanm.h"

//======================================================================
//  define
//======================================================================
enum
{
  TEX_TRANS_NON = 0,
  TEX_TRANS_SET,
  TEX_TRANS_WAIT,
};

typedef enum
{
  OBJ_USE_FALSE = 0,
  OBJ_USE_TRUE,
  OBJ_USE_RES_WAIT,
}OBJ_USE_FLAG;

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
  OBJ_USE_FLAG useFlag:2;
  u32 cullingFlag:1;
  u32 vanishFlag:1;
  u32 outerDrawFlag:1;
  u32 frameFlag:1;
  u32 padding_bit:32-6;
  
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
  u8 *gray_scale;
};

//======================================================================
//  proto
//======================================================================
static void transTexture( FLD_G3DOBJ_RES *res );
static void setResource(
    FLD_G3DOBJ_RES *res, HEAPID heapID,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag, u8 *gray_scale );
static void delResource( FLD_G3DOBJ_RES *res );
static void setObject( FLD_G3DOBJ *obj,
    FLD_G3DOBJ_RES *res, u32 resIdx, u32 mdlIdx, HEAPID heapID,
    const VecFx32 *pos, BOOL init_status );
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
            obj->resIdx, obj->mdlIdx, ctrl->heapID, NULL, FALSE );
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
      if( obj->outerDrawFlag == FALSE && obj->vanishFlag == FALSE ){
        if( obj->cullingFlag == TRUE ){
          GFL_G3D_DRAW_DrawObjectCullingON( obj->pObj, &obj->status );
        }else{
          GFL_G3D_DRAW_DrawObject( obj->pObj, &obj->status );
        }
      }
      obj->frameFlag = FALSE;
    }
    obj++;
    i++;
  }
}

//--------------------------------------------------------------
/**
 * FLD_G3DOBJ_CTRL �O���[�X�P�[���ݒ�
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param gray_scale �O���[�X�P�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetGrayScale( FLD_G3DOBJ_CTRL *ctrl, u8 *gray_scale )
{
  ctrl->gray_scale = gray_scale;
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
static FLD_G3DOBJ_RESIDX createResource( FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag, u8 *gray_scale )
{
  u16 i;
  FLD_G3DOBJ_RES *res = ctrl->pResTbl;
  
  for( i = 0; i < ctrl->res_max; i++, res++ ){
    if( res->pResMdl == NULL ){
      setResource( res, ctrl->heapID, head, transFlag, gray_scale );
      return( i );
    }
  }
  
  GF_ASSERT( i );
  return( 0 );
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
FLD_G3DOBJ_RESIDX FLD_G3DOBJ_CTRL_CreateResource( FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag )
{
  return( createResource(ctrl,head,transFlag,ctrl->gray_scale) );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�쐬�@�O���[�X�P�[����K�p���Ȃ�
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval mdl �ǉ�����FLD_G3DMDL_ARCIDX
 * @retval tex �ǉ�����FLD_G3DTEX_ARCIDX NULL=mdl���̃e�N�X�`�����Q��
 * @retval anm �ǉ�����FLD_G3DANM_ARCIDX NULL=����
 * @param transFlag TRUE=�e�N�X�`�����]�� FALSE=FLD_G3DOBJ_CTRL_Trans()�œ]��
 * @retval u16 �o�^���ꂽ���\�[�X�C���f�b�N�X
 */
//--------------------------------------------------------------
FLD_G3DOBJ_RESIDX FLD_G3DOBJ_CTRL_CreateResourceNonGrayScale(
    FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag )
{
  return( createResource(ctrl,head,transFlag,NULL) );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�폜
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �폜���郊�\�[�X�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_DeleteResource(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_RESIDX idx )
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
 * @param pos �\��������W NULL=�����B
 * @retval u16 �ǉ����ꂽOBJ�C���f�b�N�X
 */
//--------------------------------------------------------------
FLD_G3DOBJ_OBJIDX FLD_G3DOBJ_CTRL_AddObject( FLD_G3DOBJ_CTRL *ctrl,
    const u16 resIdx, u16 mdlIdx, const VecFx32 *pos )
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
        obj->cullingFlag = TRUE;
        
        obj->status.scale.x = FX32_ONE;
        obj->status.scale.y = FX32_ONE;
        obj->status.scale.z = FX32_ONE;

        if( pos != NULL ){
          obj->status.trans = *pos;
        }
        
        MTX_Identity33( &obj->status.rotate );
      }else{
        setObject( obj, res, resIdx, mdlIdx, ctrl->heapID, pos, TRUE );
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
void FLD_G3DOBJ_CTRL_DeleteObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx )
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
 * �I�u�W�F�C���f�b�N�X����GFL_G3D_OBJSTATUS�擾
 * @param ctrl  FLD_G3DOBJ_CTRL
 * @param idx �擾����OBJ�C���f�b�N�X
 * @retval GFL_G3D_OBJSTATUS
 */
//--------------------------------------------------------------
GFL_G3D_OBJSTATUS * FLD_G3DOBJ_CTRL_GetObjStatus(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx )
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
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, const VecFx32 *pos )
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
 * @attention FLD_G3DOBJ_CTRL_AddObject()���A�J�����O��ON�ɂȂ��Ă���B
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetObjCullingFlag(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag )
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
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  obj->vanishFlag = flag;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�A�j��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @retval BOOL FALSE=�A�j���I��
 * @note �A�j���w�肪�����ꍇ�͉��������߂�lFALSE�ŕԂ�B
 */
//--------------------------------------------------------------
BOOL FLD_G3DOBJ_CTRL_AnimeObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, fx32 frame )
{
  BOOL ret = FALSE;
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    int i = 0;
    int max = ctrl->pResTbl[obj->resIdx].anm_count;
    
    while( i < max ){
      if( GFL_G3D_OBJECT_IncAnimeFrame(obj->pObj,i,frame) != FALSE ){
        ret = TRUE;
      }
      i++;
    }
  }else if( obj->useFlag == OBJ_USE_RES_WAIT ){ //���\�[�X�҂�
    ret = TRUE; //�A�j���͎��s���Ƃ���
  }
  
  return( ret );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F���[�v�A�j��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @retval BOOL FALSE=�A�j�����[�v�����B
 * @note �A�j���w�肪�����ꍇ�͉��������߂�lFALSE�ŕԂ�
 */
//--------------------------------------------------------------
BOOL FLD_G3DOBJ_CTRL_LoopAnimeObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, fx32 frame )
{
  BOOL ret = FALSE;
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    int i = 0;
    int max = ctrl->pResTbl[obj->resIdx].anm_count;
    
    while( i < max ){
      if( GFL_G3D_OBJECT_LoopAnimeFrame(obj->pObj,i,frame) != FALSE ){
        ret = TRUE;
      }
      i++;
    }
  }else if( obj->useFlag == OBJ_USE_RES_WAIT ){ //���\�[�X�҂�
    ret = TRUE; //�A�j���͎��s���Ƃ���
  }
  
  return( ret );
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�A�j���t���[���w��
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param anm_no ���Ԗڂ̃A�j���ɃZ�b�g���邩 FLD_G3DOBJ_ANM_MAX=�S��
 * @param frame �Z�b�g����t���[��
 * @retval nothing
 * @note �A�j���w�肪�����ꍇ�͉������Ȃ��B
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetAnimeFrame(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, int anm_no, fx32 frame )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    int max = ctrl->pResTbl[obj->resIdx].anm_count;
    
    if( anm_no == FLD_G3DOBJ_ANM_MAX ){
      int i = 0;
      while( i < max ){
	      GFL_G3D_OBJECT_SetAnimeFrame( obj->pObj, i, (int*)&frame );
        i++;
      }
    }else if( anm_no < max ){
	    GFL_G3D_OBJECT_SetAnimeFrame( obj->pObj, anm_no, (int*)&frame );
    }
  }
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�N�g�����[�U�[������Ǝ��ɕ`�悷��
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param flag TRUE=�Ǝ��ŕ`��AFALSE=FLD_G3DOBJ_CTRL���ŕ`��
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetOuterDrawFlag(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  obj->outerDrawFlag = flag;
}

//--------------------------------------------------------------
/**
 * �I�u�W�F�N�g��Ǝ��ŕ`�悷��
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �w�肷��obj�C���f�b�N�X
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_DrawObject( 
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    GF_ASSERT( obj->outerDrawFlag == TRUE );
    if( obj->vanishFlag == FALSE ){
      if( obj->cullingFlag == TRUE ){
        GFL_G3D_DRAW_DrawObjectCullingON( obj->pObj, &obj->status );
      }else{
        GFL_G3D_DRAW_DrawObject( obj->pObj, &obj->status );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * 1�t���[�����ɃN���A�����t���[���t���O���Z�b�g
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx �w�肷��obj�C���f�b�N�X
 * @param flag �Z�b�g����t���O
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetFrameFlag(  
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  obj->frameFlag = flag;
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
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag, u8 *gray_scale )
{
  if( head->arcHandleMdl != NULL ){
    res->pResMdl = GFL_G3D_CreateResourceHandle(
        head->arcHandleMdl, head->arcIdxMdl );
  }
  
  if( head->arcHandleTex != NULL ){
    res->pResTex = GFL_G3D_CreateResourceHandle(
        head->arcHandleTex, head->arcIdxTex );
  }
  
  if( gray_scale != NULL ){ //�O���[�X�P�[���̓K�p
    GFL_G3D_RES *pTex = NULL;
    
    if( res->pResMdl != NULL ){
      pTex = res->pResMdl;
    }
     
    if( res->pResTex != NULL ){
      pTex = res->pResTex;
    }
    
    if( GFL_G3D_CheckResourceType(pTex,GFL_G3D_RES_CHKTYPE_TEX) ){
      NNSG3dResFileHeader *head = GFL_G3D_GetResourceFileHeader( pTex );
      NNSG3dResTex *tex = NNS_G3dGetTex( head ); 
      void *pData = (u8*)tex + tex->plttInfo.ofsPlttData;
      u32 size = (u32)tex->plttInfo.sizePltt << 3;
      PaletteGrayScaleShadeTable( pData, size / sizeof(u16), gray_scale );
    }
  }
  
  if( res->pResMdl != NULL || res->pResTex != NULL ){
    if( transFlag == TRUE ){ //���]��
      transTexture( res );
    }else{
      res->tex_trans_flag = TEX_TRANS_WAIT; 
    }
  }
  
  if( head->anmCount ){
    int i;
    res->anm_count = head->anmCount;
    GF_ASSERT( res->anm_count < FLD_G3DOBJ_ANM_MAX );
    GF_ASSERT( head->arcHandleAnm != NULL );
    
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
    const VecFx32 *pos, BOOL init_status )
{
  int i;
  GFL_G3D_RND *pRnd;
  GFL_G3D_RES *pResTex = NULL;
  
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
  
  if( pos != NULL ){
    obj->status.trans = *pos;
  }
  
  if( init_status ){
    obj->cullingFlag = TRUE;
    obj->status.scale.x = FX32_ONE;
    obj->status.scale.y = FX32_ONE;
    obj->status.scale.z = FX32_ONE;
    MTX_Identity33( &obj->status.rotate );
  }
  
  obj->useFlag = OBJ_USE_TRUE; //�ݒ�r���ɕ`�悪�Ă΂�Ă��������Ȃ�
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
