//======================================================================
/**
 * @file	fldeff_encount.c
 * @brief	フィールド エフェクトエンカウント用エフェクト群
 * @author  Miyuki Iwasawa	
 * @date	09.11.18
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field/fieldmap_call.h"
#include "field_effect.h"
#include "fldmmdl.h"
#include "effect_encount.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fldeff_encount.h"
#include "field_encount_local.h"

//======================================================================
//  define
//======================================================================
#define GRASS_SHAKE_FRAME (FX32_ONE*12)
#define ENCOUNT_IMDRES_MAX  (4)
#define ENCOUNT_ANMRES_MAX  (4)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_ENCOUNT型
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_ENCOUNT FLDEFF_ENCOUNT;

//--------------------------------------------------------------
/// リソース型定義 
//--------------------------------------------------------------
typedef struct _EFFRES_DATA{
  u8  anm_num;
  u8  ptn_num:3;
  u8  unit_num:3;
  u8  season_f:1;
  u8  inout_f:1;
  u16 se_no;
  u16 mdl_id;
  u16 anm_tbl[ENCOUNT_ANMRES_MAX];
}EFFRES_DATA;

typedef struct _EFFECT_OBJ{
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[ENCOUNT_ANMRES_MAX];
  GFL_G3D_RND *obj_rnd;
}EFFECT_OBJ;

//--------------------------------------------------------------
///	FLDEFF_ENCOUNT構造体
//--------------------------------------------------------------
struct _TAG_FLDEFF_ENCOUNT
{
	FLDEFF_CTRL *fectrl;
  FLDEFF_ENC_ID eff_id;
  GFL_G3D_RES *g3d_res_mdl[ENCOUNT_IMDRES_MAX];
  GFL_G3D_RES *g3d_res_anm[ENCOUNT_ANMRES_MAX];
  EFFECT_OBJ  g3d_obj[ENCOUNT_IMDRES_MAX];

  EFFRES_DATA data;
};

//--------------------------------------------------------------
/// TASKHEADER_ENCOUNT
//--------------------------------------------------------------
typedef struct
{
  FIELD_ENCOUNT* fld_enc;
  FLDEFF_ENCOUNT *eff_enc;
  MMDLSYS* mmdl_sys;
  MMDL *fmmdl;

  u16 gx,gz;
  FLDEFF_ENC_ID eff_id;
  u8  ptn_id;
  VecFx32 pos;
}TASKHEADER_ENCOUNT;

//--------------------------------------------------------------
/// TASKWORK_ENCOUNT
//--------------------------------------------------------------
typedef struct
{
  u16 seq_no;
  u8 anm_pause_f;
  u8 hide_f;
  TASKHEADER_ENCOUNT head;
  MMDL_CHECKSAME_DATA samedata;

#if 0
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[ENCOUNT_ANMRES_MAX];
  GFL_G3D_RND *obj_rnd;
#endif

  FIELD_ENCOUNT* fld_enc;
}TASKWORK_ENCOUNT;

#define ENCEFF_SE_GRASS (SEQ_SE_FLD_70)
#define ENCEFF_SE_WATER (SEQ_SE_FLD_71)
#define ENCEFF_SE_BIRD  (SEQ_SE_FLD_72)
#define ENCEFF_SE_SMOKE (SEQ_SE_FLD_73)

/*
 *  EFFENC_TYPE_IDのならびに対応するエフェクトIDのテーブル
 */
static const u8 DATA_EncEffType2EffectID[EFFENC_TYPE_MAX] = {
 FLDEFF_ENCID_SGRASS,  ///<短い草ノーマル
 FLDEFF_ENCID_SGRASS,  ///<短い草豪雪地帯
 FLDEFF_ENCID_SGRASS,  ///<短い草常春
 FLDEFF_ENCID_LGRASS,  ///<長い草
 FLDEFF_ENCID_CAVE,    ///<洞窟
 FLDEFF_ENCID_WATER,   ///<淡水
 FLDEFF_ENCID_SEA,     ///<海
 FLDEFF_ENCID_BRIDGE,  ///<橋
};

/*
 * エフェクトリソース定義データ
 *
 * anm_num  ->  アニメデータファイル数(ixxの種類数)
 * unit_num ->  四季別やIn/Out別でセットになっているデータ数
 * ptn_num  ->  同じエフェクトだが、テクスチャが異なるパターンの数。
 *              短い草は四季のunitを3パターン(ノーマル/豪雪地帯/通年春)もっている
 *              リソースの数はixxの種別ごとに unit_num*ptn_num分持っている
 * 
 * アーカイブリソースのデータの並び anm_num = 2, unit_num = 2, ptn_num = 2の場合
 *  ＊種別の異なるファイルは連続している必要はありません
 *
 *     data_unitA.imd
 *     data_unitB.imd
 *
 *     data_unitA1.ica 
 *     data_unitA2.ica 
 *     data_unitB1.ica 
 *     data_unitB2.ica 
 *
 *     data_unitA1.itp
 *     data_unitA2.itp 
 *     data_unitB1.itp 
 *     data_unitB2.itp 
*/
static EFFRES_DATA DATA_EffEncountRes[FLDEFF_ENCID_MAX] = {
 {  //短い草 FLDEFF_ENCID_SGRASS
   1, 3, 4, TRUE,FALSE, ENCEFF_SE_GRASS,  //anm_num, ptn_num, unit_num, season_f,inout_f,se_no
   NARC_fldeff_short_grass_sp_nsbmd,
   {
     NARC_fldeff_short_grass_sp_nsbta,
     0,0,0
   },
 },
 {  //FLDEFF_ENCID_LGRASS,  ///<長い草
   1, 1, 4, TRUE,FALSE, ENCEFF_SE_GRASS,
   NARC_fldeff_long_grass_sp_nsbmd,
   {
     NARC_fldeff_long_grass_sp_nsbta,
     0,0,0
   },
 },
 {  //FLDEFF_ENCID_CAVE,    ///<洞窟
   2, 1, 1, FALSE,FALSE, ENCEFF_SE_SMOKE,
   NARC_fldeff_soil_smoke_nsbmd,
   {
     NARC_fldeff_soil_smoke_nsbta,
     NARC_fldeff_soil_smoke_nsbca,
     0,0
   },
 },
 {  //FLDEFF_ENCID_WATER,   ///<淡水
   1, 1, 2, FALSE,TRUE, ENCEFF_SE_WATER,
   NARC_fldeff_freshwater_in_nsbmd,
   {
     NARC_fldeff_freshwater_in_nsbca,
     0,0,0
   },
 },
 {  //FLDEFF_ENCID_SEA,     ///<海
   1, 1, 1, FALSE,FALSE, ENCEFF_SE_WATER,
   NARC_fldeff_sea_out_nsbmd,
   {
     NARC_fldeff_sea_out_nsbca,
     0,0,0
   },
 },
 {  //FLDEFF_ENCID_BRIDGE,  ///<橋
   3, 1, 1, FALSE,FALSE, ENCEFF_SE_BIRD,
   NARC_fldeff_bird_shadow_nsbmd,
   {
     NARC_fldeff_bird_shadow_nsbta,
     NARC_fldeff_bird_shadow_nsbma,
     NARC_fldeff_bird_shadow_nsbca,
     0
   },
 },
};

//======================================================================
//	プロトタイプ
//======================================================================
static FLDEFF_ENCOUNT* enc_CreateWork( FLDEFF_CTRL* fectrl, HEAPID heapID, FLDEFF_ENC_ID type );
static void enc_InitResource( FLDEFF_ENCOUNT* enc, FLDEFF_ENC_ID eff_id );
static void enc_InitResourceSGrass( FLDEFF_ENCOUNT* enc, EFFRES_DATA* data, ARCHANDLE* handle );
static void enc_InitResourceNormal( FLDEFF_ENCOUNT* enc, EFFRES_DATA* data, ARCHANDLE* handle );
static void enc_DeleteResource( FLDEFF_ENCOUNT* enc );
static void sub_PlaySE( TASKWORK_ENCOUNT* work, FLDEFF_ENCOUNT* enc );

static const FLDEFF_TASK_HEADER DATA_encountTaskHeader;

//======================================================================
//	エフェクトエンカウント　エフェクト　システム
//======================================================================
//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクト 初期化
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	エフェクト使用ワーク
 */
//--------------------------------------------------------------
void * FLDEFF_ENCOUNT_SGrassInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_SGRASS );
}
void * FLDEFF_ENCOUNT_LGrassInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_LGRASS );
}
void * FLDEFF_ENCOUNT_CaveInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_CAVE );
}
void * FLDEFF_ENCOUNT_WaterInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_WATER );
}
void * FLDEFF_ENCOUNT_SeaInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_SEA );
}
void * FLDEFF_ENCOUNT_BridgeInit( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  return enc_CreateWork( fectrl, heapID, FLDEFF_ENCID_BRIDGE );
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクト 削除
 * @param fectrl FLDEFF_CTRL
 * @param	work	エフェクト使用ワーク
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_ENCOUNT_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_ENCOUNT *enc = work;
  enc_DeleteResource( enc );
  GFL_HEAP_FreeMemory( enc );
}

//======================================================================
//	エフェクトエンカウント　エフェクト　リソース
//======================================================================

//--------------------------------------------------------------
/*
 * @brief   コモンワーク生成＆初期化
 * @param   fectrl FLDEFF_CTRL*
 * @param   heapID ヒープID
 * @param   eff_id  リソース初期化タイプID
 * @retval  FLDEFF_ENCOUNT* 
 */
//--------------------------------------------------------------
static FLDEFF_ENCOUNT* enc_CreateWork( FLDEFF_CTRL* fectrl, HEAPID heapID, FLDEFF_ENC_ID eff_id ) 
{
  FLDEFF_ENCOUNT* enc;
	enc = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_ENCOUNT) );
	enc->fectrl = fectrl;

  enc_InitResource( enc, eff_id );
  return enc;
}

//--------------------------------------------------------------
/*
 * @brief  コモンリソース初期化
 * @param   enc FLDEFF_ENCOUNT*
 * @param   リソース初期化タイプ
 * @retval  nothing
 */
//--------------------------------------------------------------
static void enc_InitResource( FLDEFF_ENCOUNT* enc, FLDEFF_ENC_ID eff_id ) 
{
  ARCHANDLE *handle;

  enc->data = DATA_EffEncountRes[eff_id];

  handle = FLDEFF_CTRL_GetArcHandleEffect( enc->fectrl );

  if( eff_id == FLDEFF_ENCID_SGRASS ){
    enc_InitResourceSGrass( enc, &enc->data, handle );
  }else{
    enc_InitResourceNormal( enc, &enc->data, handle );
  }
}

static void enc_CreateObject( EFFECT_OBJ* obj, FLDEFF_ENCOUNT* enc, u8 ptn_id )
{
  int i;

  obj->obj_rnd =
    GFL_G3D_RENDER_Create( enc->g3d_res_mdl[ptn_id], 0, enc->g3d_res_mdl[ptn_id] );

  for(i = 0;i < enc->data.anm_num;i++){
    obj->obj_anm[i] =
    GFL_G3D_ANIME_Create( obj->obj_rnd, enc->g3d_res_anm[i+ptn_id*enc->data.anm_num], 0 );
  }
  
  obj->obj = GFL_G3D_OBJECT_Create( obj->obj_rnd, obj->obj_anm, enc->data.anm_num );

  for(i = 0;i < enc->data.anm_num;i++){
    GFL_G3D_OBJECT_EnableAnime( obj->obj, i );
  }
}

static void enc_DeleteObject( EFFECT_OBJ* obj, FLDEFF_ENCOUNT* enc )
{
  int i;

  for( i = 0;i < enc->data.anm_num; i++){
    GFL_G3D_ANIME_Delete( obj->obj_anm[i] );
  }
  GFL_G3D_OBJECT_Delete( obj->obj );
	GFL_G3D_RENDER_Delete( obj->obj_rnd );
}

/*
 *  @brief  短い草用リソース初期化
 *
 *  短い草だけ(ノーマル、豪雪地帯用、通年春地帯用、の3パターンあり
 *  通年春用は専用素材を持たないので特殊処理)
 */
static void enc_InitResourceSGrass( FLDEFF_ENCOUNT* enc, EFFRES_DATA* data, ARCHANDLE* handle )
{
  int i,j,ofs = 0;
  BOOL ret;
  
  ofs = FLDEFF_CTRL_GetSeasonID( enc->fectrl );

  //ノーマル
  enc->g3d_res_mdl[0] = GFL_G3D_CreateResourceHandle( handle, data->mdl_id+ofs );
  ret = GFL_G3D_TransVramTexture( enc->g3d_res_mdl[0] );
  GF_ASSERT( ret );
  enc->g3d_res_anm[0]	= GFL_G3D_CreateResourceHandle( handle, data->anm_tbl[0]+ofs );
  enc_CreateObject( &enc->g3d_obj[0], enc, 0 );
 
  //豪雪地帯
  ofs += data->unit_num;
  enc->g3d_res_mdl[1] = GFL_G3D_CreateResourceHandle( handle, data->mdl_id+ofs );
  ret = GFL_G3D_TransVramTexture( enc->g3d_res_mdl[1] );
  GF_ASSERT( ret );
  enc->g3d_res_anm[1]	= GFL_G3D_CreateResourceHandle( handle, data->anm_tbl[0]+ofs );
  enc_CreateObject( &enc->g3d_obj[1], enc, 1 );

  //通年春
  enc->g3d_res_mdl[2] = GFL_G3D_CreateResourceHandle( handle, data->mdl_id );
  ret = GFL_G3D_TransVramTexture( enc->g3d_res_mdl[2] );
  GF_ASSERT( ret );
  enc->g3d_res_anm[2]	= GFL_G3D_CreateResourceHandle( handle, data->anm_tbl[0] );
  enc_CreateObject( &enc->g3d_obj[2], enc, 2 );
}

/*
 *  @brief  その他用共通リソース初期化
 */
static void enc_InitResourceNormal( FLDEFF_ENCOUNT* enc, EFFRES_DATA* data, ARCHANDLE* handle )
{
  int i,j,ofs = 0;
  BOOL ret;
  
  if( data->season_f ){
    ofs = FLDEFF_CTRL_GetSeasonID( enc->fectrl );
  }else if( data->inout_f ){
    ofs = FLDEFF_CTRL_GetAreaInOutSwitch( enc->fectrl );
  }

  for(i = 0;i < data->ptn_num;i++ ){
    int data_ofs = ofs+(i*data->unit_num);

    enc->g3d_res_mdl[i] = GFL_G3D_CreateResourceHandle( handle, data->mdl_id+data_ofs );
    ret = GFL_G3D_TransVramTexture( enc->g3d_res_mdl[i] );
    GF_ASSERT( ret );
    
    for(j = 0;j < data->anm_num;j++){
      enc->g3d_res_anm[j+i*data->anm_num]	=
        GFL_G3D_CreateResourceHandle( handle, data->anm_tbl[j]+data_ofs );
    }
    enc_CreateObject( &enc->g3d_obj[i], enc, i );
  }
}

//--------------------------------------------------------------
/*
 * @brief  コモンリソース解放
 * @param   enc FLDEFF_ENCOUNT*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void enc_DeleteResource( FLDEFF_ENCOUNT* enc ) 
{
  int i;
  
  for(i = 0;i < enc->data.ptn_num;i++){
    enc_DeleteObject( &enc->g3d_obj[i], enc );
  }

  for(i = 0;i < enc->data.anm_num*enc->data.ptn_num;i++){
 	  GFL_G3D_DeleteResource( enc->g3d_res_anm[i] );
  }
  for(i = 0;i < enc->data.ptn_num;i++){
    GFL_G3D_FreeVramTexture( enc->g3d_res_mdl[i] );
 	  GFL_G3D_DeleteResource( enc->g3d_res_mdl[i] );
  }
}

//======================================================================
//	エフェクトエンカウント　エフェクト　タスク
//======================================================================
//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクト追加(レールマップ上で呼び出しても無視されます)
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK* FLDEFF_ENCOUNT_SetEffect( FIELD_ENCOUNT* fld_enc, FLDEFF_CTRL *fectrl, u16 gx, u16 gz, fx32 height, EFFENC_TYPE_ID type )
{
  FLDEFF_ENCOUNT* enc;
  TASKHEADER_ENCOUNT head;
  VecFx32 pos;
  FLDEFF_ENC_ID  eff_id;
  FIELDMAP_WORK* fieldMapWork = FLDEFF_CTRL_GetFieldMapWork( fectrl );

  if( FIELDMAP_GetBaseSystemType( fieldMapWork ) != FLDMAP_BASESYS_GRID ){
     return NULL; //レールマップでは生成しない
  }

  eff_id = DATA_EncEffType2EffectID[type];
  enc = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_ENC_SGRASS+eff_id );

  if( enc == NULL ){
    return NULL;
  }

  MI_CpuClear8( &head, sizeof(TASKHEADER_ENCOUNT));
  head.fld_enc = fld_enc;
  head.eff_enc = enc;
  head.mmdl_sys = FIELDMAP_GetMMdlSys( fieldMapWork );

  head.gx = gx;
  head.gz = gz;
  head.eff_id = eff_id;

  if( eff_id == FLDEFF_ENCID_SGRASS ){  //短い草にだけ3パターンある
    head.ptn_id = type-EFFENC_TYPE_SGRASS_NORMAL;
  }else{
    head.ptn_id = 0;
  }
  
  head.pos.x = GRID_SIZE_FX32(gx)+GRID_HALF_FX32;
  head.pos.z = GRID_SIZE_FX32(gz)+GRID_HALF_FX32;
  head.pos.y = height;

  return FLDEFF_CTRL_AddTask(
      fectrl, &DATA_encountTaskHeader, NULL, TRUE, &head, 0 );
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　アニメ再生フック
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 *
 * 純粋にDeleteのタイミングまで待つと、フェードアウト後に
 * SEが鳴ってしまうことがあるのでフックする
 */
//--------------------------------------------------------------
void FLDEFF_ENCOUNT_AnmPauseSet( FLDEFF_TASK* task, BOOL pause_f )
{
  if( task != NULL ){
    TASKWORK_ENCOUNT *work = (TASKWORK_ENCOUNT*)FLDEFF_TASK_GetWork( task );
    work->anm_pause_f = pause_f;
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント エフェクト隠す　
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_ENCOUNT_HideSet( FLDEFF_TASK* task, BOOL hide_f )
{
  if( task != NULL ){
    TASKWORK_ENCOUNT *work = (TASKWORK_ENCOUNT*)FLDEFF_TASK_GetWork( task );
    work->hide_f = hide_f;
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクトタスク　初期化
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void encountTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_ENCOUNT *work = wk;
  const TASKHEADER_ENCOUNT *head;
  FLDEFF_ENCOUNT* enc;
 
  head = FLDEFF_TASK_GetAddPointer( task );
  work->fld_enc = head->fld_enc;
  
  work->head = *head;
  enc = work->head.eff_enc;

  FLDEFF_TASK_SetPos( task, &head->pos );
 
#if 0
  work->obj_rnd =
    GFL_G3D_RENDER_Create( enc->g3d_res_mdl[work->head.ptn_id], 0, enc->g3d_res_mdl[work->head.ptn_id] );
  
  {
    int i;

    for(i = 0;i < enc->data.anm_num;i++){
      work->obj_anm[i] =
      GFL_G3D_ANIME_Create( work->obj_rnd, enc->g3d_res_anm[i+work->head.ptn_id*enc->data.anm_num], 0 );
    }
  
    work->obj = GFL_G3D_OBJECT_Create( work->obj_rnd, work->obj_anm, enc->data.anm_num );

    for(i = 0;i < enc->data.anm_num;i++){
      GFL_G3D_OBJECT_EnableAnime( work->obj, i );
    }
  }
#endif

  if( FLDEFF_TASK_GetAddParam(task) == FALSE ){ //アニメ無し
    work->seq_no = 1;
  }
  
  FLDEFF_TASK_CallUpdate( task ); //即動作
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクトタスク　削除
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void encountTask_Delete( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_ENCOUNT *work = wk;
  FLDEFF_ENCOUNT* enc = work->head.eff_enc;

#if 0 
  for( i = 0;i < enc->data.anm_num; i++){
    GFL_G3D_ANIME_Delete( work->obj_anm[i] );
  }
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
#endif
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクトタスク　更新
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void encountTask_Update( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_ENCOUNT *work = wk;
  FLDEFF_ENCOUNT* enc = work->head.eff_enc;
  
  if(!FIELDMAP_IsReady( FLDEFF_CTRL_GetFieldMapWork( enc->fectrl ) )){
    return;
  }
  if( work->anm_pause_f ){
    return;
  }
  
  sub_PlaySE( work, enc );
  
  for( i = 0;i < enc->data.anm_num; i++){
//    GFL_G3D_OBJECT_LoopAnimeFrame( work->obj, i, FX32_ONE );
    GFL_G3D_OBJECT_LoopAnimeFrame( enc->g3d_obj[work->head.ptn_id].obj, i, FX32_ONE );
  }
}

static void sub_PlaySE( TASKWORK_ENCOUNT* work, FLDEFF_ENCOUNT* enc )
{
  int frame;
  u16 dis = 0;
  static const u8 volume_tbl[] = { 127, 90, 60, 40 };
 
//  GFL_G3D_OBJECT_GetAnimeFrame( work->obj, 0, &frame );
  GFL_G3D_OBJECT_GetAnimeFrame( enc->g3d_obj[work->head.ptn_id].obj, 0, &frame );
  if(frame != 0){
    return;
  }

  if( EFFECT_ENC_GetDistanceToPlayer( work->fld_enc, &dis) == FALSE ){
    return;
  }
  dis /= 5;
  if(dis > 3){
    dis = 3;
  }
  FSND_PlayEnvSEVol(GAMEDATA_GetFieldSound(work->fld_enc->gdata), enc->data.se_no, volume_tbl[dis]); 
}

//--------------------------------------------------------------
/**
 * エフェクトエンカウント　エフェクトタスク　描画
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void encountTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_ENCOUNT *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  FLDEFF_ENCOUNT* enc = work->head.eff_enc;
  
  if( work->hide_f ){
    return;
  }
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
//  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
  GFL_G3D_DRAW_DrawObjectCullingON( enc->g3d_obj[work->head.ptn_id].obj, &status );
}

//--------------------------------------------------------------
//  エフェクトエンカウント　エフェクトタスク　ヘッダー
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_encountTaskHeader =
{
  sizeof(TASKWORK_ENCOUNT),
  encountTask_Init,
  encountTask_Delete,
  encountTask_Update,
  encountTask_Draw,
};
