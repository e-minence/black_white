//======================================================================
/*
 * @file	field_g3dobj.c
 * @brief	フィールド 3Dオブジェクト
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
  GFL_G3D_RES *pResTex; //NULL=pResMdlからテクスチャを取得
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
  u32 useFlag:2;
  u32 cullingFlag:1;
  u32 vanishFlag:1;
  u32 outerDrawFlag:1;
  u32 frameFlag:1;
  u32 padding_bit:32-6;
  
  u16 resIdx; //使用するリソースインデックス
  u16 mdlIdx; //使用するモデルインデックス
    
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
 * FLD_G3DOBJ_CTRL作成
 * @param heapID 使用するHEAPID
 * @param res_max 作成するリソース最大数
 * @param obj_max 作成するOBJ最大数
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
 * FLD_G3DOBJ_CTRL削除
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
 * テクスチャ転送リクエスト消化
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
 * 描画
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
 * FLD_G3DOBJ_CTRL グレースケール設定
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param gray_scale グレースケール
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetGrayScale( FLD_G3DOBJ_CTRL *ctrl, u8 *gray_scale )
{
  ctrl->gray_scale = gray_scale;
}

//--------------------------------------------------------------
/**
 * リソース作成
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval mdl 追加するFLD_G3DMDL_ARCIDX
 * @retval tex 追加するFLD_G3DTEX_ARCIDX NULL=mdl内のテクスチャを参照
 * @retval anm 追加するFLD_G3DANM_ARCIDX NULL=無し
 * @param transFlag TRUE=テクスチャ即転送 FALSE=FLD_G3DOBJ_CTRL_Trans()で転送
 * @retval u16 登録されたリソースインデックス
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
 * リソース作成
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval mdl 追加するFLD_G3DMDL_ARCIDX
 * @retval tex 追加するFLD_G3DTEX_ARCIDX NULL=mdl内のテクスチャを参照
 * @retval anm 追加するFLD_G3DANM_ARCIDX NULL=無し
 * @param transFlag TRUE=テクスチャ即転送 FALSE=FLD_G3DOBJ_CTRL_Trans()で転送
 * @retval u16 登録されたリソースインデックス
 */
//--------------------------------------------------------------
FLD_G3DOBJ_RESIDX FLD_G3DOBJ_CTRL_CreateResource( FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag )
{
  return( createResource(ctrl,head,transFlag,ctrl->gray_scale) );
}

//--------------------------------------------------------------
/**
 * リソース作成　グレースケールを適用しない
 * @param ctrl FLD_G3DOBJ_CTRL
 * @retval mdl 追加するFLD_G3DMDL_ARCIDX
 * @retval tex 追加するFLD_G3DTEX_ARCIDX NULL=mdl内のテクスチャを参照
 * @retval anm 追加するFLD_G3DANM_ARCIDX NULL=無し
 * @param transFlag TRUE=テクスチャ即転送 FALSE=FLD_G3DOBJ_CTRL_Trans()で転送
 * @retval u16 登録されたリソースインデックス
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
 * リソース削除
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 削除するリソースインデックス
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
 * オブジェ追加
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param resIdx 使用するリソースインデックス
 * @param mdlIdx resIdxで登録されているモデルで使用するインデックス
 * @param pos 表示する座標 NULL=無視。
 * @retval u16 追加されたOBJインデックス
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
      
      if( res->tex_trans_flag == TEX_TRANS_WAIT ){ //準備中
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
 * オブジェ削除
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 削除するOBJインデックス
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
 * オブジェインデックスからGFL_G3D_OBJSTATUS取得
 * @param ctrl  FLD_G3DOBJ_CTRL
 * @param idx 取得するOBJインデックス
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
 * オブジェへ描画する際の座標指定
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @param pos 座標
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
 * オブジェへ描画する際のカリングチェック指定
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @param flag true=カリングする。false=しない
 * @retval nothing
 * @attention FLD_G3DOBJ_CTRL_AddObject()時、カリングはONになっている。
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
 * オブジェへ描画する際の非表示設定
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @param flag TRUE=非表示 FALSE=表示
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
 * オブジェアニメ
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @retval BOOL FALSE=アニメ終了
 * @note アニメ指定が無い場合は何もせず戻り値FALSEで返る。
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
  }else if( obj->useFlag == OBJ_USE_RES_WAIT ){ //リソース待ち
    ret = TRUE; //アニメは実行中とする
  }
  
  return( ret );
}

//--------------------------------------------------------------
/**
 * オブジェループアニメ
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @retval BOOL FALSE=アニメループした。
 * @note アニメ指定が無い場合は何もせず戻り値FALSEで返る
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
  }else if( obj->useFlag == OBJ_USE_RES_WAIT ){ //リソース待ち
    ret = TRUE; //アニメは実行中とする
  }
  
  return( ret );
}

//--------------------------------------------------------------
/**
 * オブジェアニメフレーム指定
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @param anm_no 何番目のアニメにセットするか FLD_G3DOBJ_ANM_MAX=全部
 * @param frame セットするフレーム
 * @retval nothing
 * @note アニメ指定が無い場合は何もしない。
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
 * オブジェアニメフレーム取得
 * @param ctrl  fld_g3dobj_ctrl
 * @param idx 指定するobjインデックス
 * @param anm_no 何番目のアニメフレームを取得するか
 * @retval fx32 フレーム
 * @note アニメ指定が無い場合は0を返す
 */
//--------------------------------------------------------------
fx32 FLD_G3DOBJ_CTRL_GetAnimeFrame(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, int anm_no )
{
  int frame = 0;
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  GF_ASSERT( obj->useFlag != OBJ_USE_FALSE );
  
  if( obj->useFlag == OBJ_USE_TRUE ){
    if( GFL_G3D_OBJECT_GetAnimeFrame(obj->pObj,anm_no,&frame) == FALSE ){
      frame = 0;
    }
  }
  return( (fx32)frame );
}

//--------------------------------------------------------------
/**
 * オブジェクトをユーザー側から独自に描画する
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 指定するobjインデックス
 * @param flag TRUE=独自で描画、FALSE=FLD_G3DOBJ_CTRL側で描画
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
 * オブジェクトを独自で描画する
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 指定するobjインデックス
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
 * 1フレーム毎にクリアされるフレームフラグをセット
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 指定するobjインデックス
 * @param flag セットするフラグ
 * @retval nothing
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_CTRL_SetFrameFlag(  
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  obj->frameFlag = flag;
}

//--------------------------------------------------------------
/**
 * 1フレーム毎にクリアされるフレームフラグを取得
 * @param ctrl FLD_G3DOBJ_CTRL
 * @param idx 指定するobjインデックス
 * @retval BOOL フレームフラグ
 */
//--------------------------------------------------------------
BOOL FLD_G3DOBJ_CTRL_GetFrameFlag(  
    const FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx )
{
  FLD_G3DOBJ *obj = &ctrl->pObjTbl[idx];
  return( obj->frameFlag );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * テクスチャセット
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
 * リソースセット
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
  
  if( gray_scale != NULL ){ //グレースケールの適用
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
    if( transFlag == TRUE ){ //即転送
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
 * リソース削除
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void delResource( FLD_G3DOBJ_RES *res )
{
  u32 flag = res->tex_trans_flag;
  
  res->tex_trans_flag = TEX_TRANS_NON; //転送待ち削除の際の割り込みを考慮
   
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
 * OBJセット
 * @param res FLD_G3DOBJ_RES
 * @param resIdx 使用するリソースインデックス
 * @param mdlIdx 使用するモデルインデックス
 * @param heapID HEAPID
 * @param init_status ステータス初期化あり
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
  
  obj->useFlag = OBJ_USE_TRUE; //設定途中に描画が呼ばれても反応しない
}

//--------------------------------------------------------------
/**
 * OBJ削除
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
 * FLD_G3DOBJ_RES_HEADER 初期化
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
 * FLD_G3DOBJ_RES_HEADER モデルセット
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ロード先ARCHANDLE*
 * @param idx handleからロードするデータインデックス
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
 * FLD_G3DOBJ_RES_HEADER テクスチャセット
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ロード先ARCHANDLE*
 * @param idx handleからロードするデータインデックス
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
 * FLD_G3DOBJ_RES_HEADER アニメアーカイブハンドルセット
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ロード先ARCHANDLE*
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
 * FLD_G3DOBJ_RES_HEADER アニメセット
 * @param head FLD_G3DOBJ_RES_HEADER
 * @param handle ロード先ARCHANDLE*
 * @retval nothing
 * @attention 続けてこの関数を呼ぶ事で複数登録が可能。
 * 最大登録数はFLD_G3DOBJ_ANM_MAX。
 */
//--------------------------------------------------------------
void FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
    FLD_G3DOBJ_RES_HEADER *head, u16 idx )
{
  GF_ASSERT( head->anmCount < FLD_G3DOBJ_ANM_MAX );
  head->arcIdxAnmTbl[head->anmCount] = idx;
  head->anmCount++;
}
