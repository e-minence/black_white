//======================================================================
/**
 * @file	fld3d_ci.c
 * @brief	フィールド3Dカットイン
 * @author	Saito
 *
 */
//======================================================================
#include "fld3d_ci.h"
#include "arc_def.h"
#include "gamesystem/game_event.h"

#define OBJCOUNT_MAX  (2)
#define ANM_TYPE_MAX  (4)
#define RESCOUNT_MAX  (8)

#define UNIT_NONE   (0xffffffff)

#define NONDATA  (0xffff)

#define	PRO_MAT_Z_OFS	(20*FX32_ONE)   //プロジェクションマトリックスによるデプスバッファ操作値

#ifdef PM_DEBUG

#include "fieldmap.h" //for FIELDMAP_〜
#include "event_mapchange.h"

#endif  //PM_DEBUG


typedef struct FLD3D_CI_tag
{
  FLD_PRTCL_SYS_PTR PrtclSys;
  GFL_G3D_CAMERA *g3Dcamera;
  HEAPID HeapID;
  GFL_G3D_UTIL* Util;
  int UnitIdx;
  GFL_G3D_OBJSTATUS Status;

  GFL_PTC_PTR PrtclPtr;

  u16 PrtclWait;
  u16 MdlAnm1Wait;
  u16 MdlAnm2Wait;
  u8 PartGene;
  u8 CutInNo;

  GXPlaneMask Mask;
  u8 BgPriority[4];
  BOOL CapEndFlg;
}FLD3D_CI;

//バイナリデータフォーマット
typedef struct {
  u16 SpaIdx;
  u16 SpaWait;
  u16 MdlObjIdx[OBJCOUNT_MAX]; //0：Ａ　1：Ｂ
  u16 MdlAAnmWait;
  u16 MdlBAnmWait;
  u16 MdlAAnmIdx[ANM_TYPE_MAX];  //4つ目はデータなし
  u16 MdlBAnmIdx[ANM_TYPE_MAX];  //4つ目はデータなし
}RES_DEF_DAT;

typedef struct {
  GFL_G3D_UTIL_RES *Res;
  GFL_G3D_UTIL_OBJ *Obj;
  GFL_G3D_UTIL_ANM *Anm1;
  GFL_G3D_UTIL_ANM *Anm2;
  GFL_G3D_UTIL_SETUP Setup;
  u16 SpaWait;
  u16 MdlAAnmWait;
  u16 MdlBAnmWait;
  u8 ResNum;
  u8 ObjNum;
  u8 AnmNum;
  u8 SpaIdx;
  u8 Dummy[2];
}RES_SETUP_DAT;

typedef struct {
  GAMESYS_WORK * gsys;
  FLD3D_CI_PTR CiPtr;
  RES_SETUP_DAT SetupDat;
}FLD3D_CI_EVENT_WORK;

#ifdef PM_DEBUG
typedef struct {
  GAMESYS_WORK *gsys;
  FLD3D_CI_PTR CiPtr;
  FIELD_CAMERA * camera;
  FIELD_PLAYER * player;
  FLDNOGRID_MAPPER* mapper;
  const VecFx32 *Watch;
}FLYSKY_EFF_WORK;
#endif

static void SetupResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat, const u8 inCutInNo);
static void SetupResourceCore(FLD3D_CI_PTR ptr, const GFL_G3D_UTIL_SETUP *inSetup);
static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat);
static BOOL PlayParticle(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr);
static BOOL CheckAnmEnd(FLD3D_CI_PTR ptr);

static GMEVENT_RESULT CutInEvt( GMEVENT* event, int* seq, void* work );


static void ParticleCallBack(GFL_EMIT_PTR emit);    //@todo
static void Generate(FLD3D_CI_PTR ptr);    //@todo

static void CreateRes(RES_SETUP_DAT *outDat, const u8 inResArcIdx, const HEAPID inHeapID);
static void DeleteRes(RES_SETUP_DAT *outDat);

static GMEVENT *CreateCutInEvt(GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo);

static void PushPriority(FLD3D_CI_PTR ptr);
static void PushDisp(FLD3D_CI_PTR ptr);
static void PopPriority(FLD3D_CI_PTR ptr);
static void PopDisp(FLD3D_CI_PTR ptr);
static void ReqCapture(FLD3D_CI_PTR ptr);

static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work );

#ifdef PM_DEBUG
static GMEVENT_RESULT DebugFlySkyEffEvt( GMEVENT* event, int* seq, void* work );
#endif  //PM_DEBUG


//--------------------------------------------------------------------------------------------
/**
 * モジュール初期化
 *
 * @param   inHeapID      ヒープＩＤ
 * @param   inPrtclSysptr パーティクルシステムポインタ
 *
 * @return	ptr         モジュールポインタ
 */
//--------------------------------------------------------------------------------------------
FLD3D_CI_PTR FLD3D_CI_Init(const HEAPID inHeapID, FLD_PRTCL_SYS_PTR inPrtclSysPtr)
{
  FLD3D_CI_PTR ptr;
  
  GF_ASSERT(inPrtclSysPtr != NULL);

  ptr = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(FLD3D_CI));
  ptr->Util = GFL_G3D_UTIL_Create( RESCOUNT_MAX, OBJCOUNT_MAX, inHeapID );
  ptr->HeapID = inHeapID;
  ptr->PrtclSys = inPrtclSysPtr;
  
  ptr->UnitIdx = UNIT_NONE;

  {
    VecFx32 pos = {0, 0, FX32_ONE*(128)};
    VecFx32 target = {0,0,0};
    //カメラ作成
    ptr->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(
        &pos, &target, inHeapID );
  }
  return ptr;
}

//--------------------------------------------------------------------------------------------
/**
 * モジュール破棄
 *
 * @param   ptr     モジュールポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD3D_CI_End(FLD3D_CI_PTR ptr)
{
  u16 i;
/**  
  //解放忘れがあるかもしれないので、ユニット廃棄もしておく
  for (i=0;i<FLD_EXP_OBJ_UNIT_MAX;i++){
    if ( ptr->Unit[i].Valid){
      FLD_EXP_OBJ_DelUnit( ptr, i );
    }
  }
*/
  //カメラ解放
  GFL_G3D_CAMERA_Delete(ptr->g3Dcamera);
  GFL_G3D_UTIL_Delete( ptr->Util );
  GFL_HEAP_FreeMemory( ptr );
}

//--------------------------------------------------------------------------------------------
/**
 * リソースセットアップ
 *
 * @param   
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetupResourceCore(FLD3D_CI_PTR ptr, const GFL_G3D_UTIL_SETUP *inSetup)
{
  u16 i;
  u16 unitIdx;
  GFL_G3D_OBJSTATUS *status;
  u16 obj_num;

  const GFL_G3D_UTIL_SETUP *setup;
  
  setup = inSetup;

  unitIdx = GFL_G3D_UTIL_AddUnit( ptr->Util, setup );
  obj_num = setup->objCount;
  if (obj_num == 0){
    GF_ASSERT_MSG(0,"OBJ NOTHING\n");
  }

  ptr->UnitIdx = unitIdx;

  {
    VEC_Set( &ptr->Status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	  MTX_Identity33( &ptr->Status.rotate );
    VEC_Set( &ptr->Status.trans, 0, 0, 0 );
  }
}

void FLD3D_CI_Draw( FLD3D_CI_PTR ptr )
{
  u8 i,j;
  
#if 0
#ifdef PM_DEBUG
  static int test_ofs = 8;
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if ( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
      test_ofs++;
    }else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
      test_ofs--;
    }
      OS_Printf("ofs=%d\n",test_ofs);
  }
#endif  //PM_DEBUG  
#endif
  GFL_G3D_CAMERA_Switching( ptr->g3Dcamera );
  {
    fx32 ofs;
	  MtxFx44 org_pm,pm;
		const MtxFx44 *m;
		m = NNS_G3dGlbGetProjectionMtx();
		org_pm = *m;
		pm = org_pm;
		pm._32 += FX_Mul( pm._22, PRO_MAT_Z_OFS );
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		  //　ジオメトリコマンドを転送
    NNS_G3dGeFlushBuffer(); // 転送まち

    //ユニット数分ループ
    if (ptr->UnitIdx != UNIT_NONE){
      u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
      u16 obj_count = GFL_G3D_UTIL_GetUnitObjCount( ptr->Util, ptr->UnitIdx );
      for (j=0;j<obj_count;j++){
        GFL_G3D_OBJ* pObj;
        pObj = GFL_G3D_UTIL_GetObjHandle(ptr->Util, obj_idx+j);
        GFL_G3D_DRAW_DrawObject( pObj, &ptr->Status );
      }
    }
    NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//　ジオメトリコマンドを転送
  }
}


//カットイン呼び出し
void FLD3D_CI_CallCutIn( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo )
{
  GMEVENT * event;
  event = CreateCutInEvt(gsys, ptr, inCutInNo);
  GAMESYSTEM_SetEvent(gsys, event);
}

//カットインイベント作成
static GMEVENT *CreateCutInEvt(GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo)
{
  GMEVENT * event;
  FLD3D_CI_EVENT_WORK *work;
  int size;

  size = sizeof(FLD3D_CI_EVENT_WORK);
  ptr->PartGene = 0;
  ptr->CutInNo = inCutInNo;
  //イベント作成
  {
    event = GMEVENT_Create( gsys, NULL, CutInEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );
    work->gsys = gsys;
    work->CiPtr = ptr;
  }
  return event;
}

//カットインイベント
static GMEVENT_RESULT CutInEvt( GMEVENT* event, int* seq, void* work )
{
  FLD3D_CI_PTR ptr;
  FLD3D_CI_EVENT_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  ptr = evt_work->CiPtr;
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    //プライオリティの保存
    PushPriority(ptr);
    //表示状態の保存
    PushDisp(ptr);
    //カットイン共通ホワイトアウト処理
    (*seq)++;
    break;
  case 1:
    //ホワイトアウト待ち
    if (1){
      //キャプチャリクエスト
      ReqCapture(ptr);
      (*seq)++;
    }
    break;
  case 2:
    //キャプチャ終了待ち
    if (ptr->CapEndFlg){
      //キャプチャ終わったら、プライオリティ変更と表示変更
      GX_SetVisiblePlane( GX_PLANEMASK_BG0|GX_PLANEMASK_BG3 );
      GFL_BG_SetPriority( 0, 0 );
      GFL_BG_SetPriority( 3, 3 );
      //描画モード変更
      {
        static const GFL_BG_SYS_HEADER bg_sys_header = 
          {
            GX_DISPMODE_GRAPHICS,GX_BGMODE_3,GX_BGMODE_0,GX_BG0_AS_3D
          };
        GFL_BG_SetBGMode( &bg_sys_header );
      }
      //ＢＧセットアップ
      G2_SetBG3ControlDCBmp(
          GX_BG_SCRSIZE_DCBMP_256x256,
          GX_BG_AREAOVER_XLU,
          GX_BG_BMPSCRBASE_0x00000
      );
      GX_SetBankForBG(GX_VRAM_BG_128_D);
      //アルファブレンド
      G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,
				GX_BLEND_PLANEMASK_BG3, 0,0);

      (*seq)++;
    }
    break;
  case 3:
    //リソースロード
    SetupResource(ptr, &evt_work->SetupDat, ptr->CutInNo);
    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      OS_Printf("START::FLD3DCUTIN_HEAP_REST %x\n",size);
    }
    //フィールド表示モード切替
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_CUTIN);
    (*seq)++;
    break;
  case 4:
    //画面復帰
    if (1){
      (*seq)++;
    }
    break;
  case 5:
    {
      BOOL rc1,rc2,rc3;
      //パーティクル再生
      rc1 = PlayParticle(ptr);
      //3Ｄモデル1アニメ再生
      rc2 = PlayMdlAnm1(ptr);
      //3Ｄモデル2アニメ再生
      rc3 = PlayMdlAnm2(ptr);

      if (rc1&&rc2&&rc3){
        (*seq)++;
      }
    }
    break;
  case 6:
    //フィールドモード戻し
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_NORMAL);
    //リソース解放処理
    DeleteResource(ptr, &evt_work->SetupDat);
    (*seq)++;
    break;
  case 7:
    //プライオリティの復帰
    PopPriority(ptr);
    //表示状態の復帰
    PopDisp(ptr);
    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      OS_Printf("END::FLD3DCUTIN_HEAP_REST %x\n",size);
    }

    //マップ遷移を入れて、ＢＧのセットアップをする
    {
      GMEVENT * child;
      int zone_id;
      FIELD_PLAYER *fld_player;
      PLAYER_WORK *player_work;
      FLDMAP_CTRLTYPE map_type;

      fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      {
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
        player_work = GAMEDATA_GetMyPlayerWork(gamedata);
      }
      zone_id = player_work->zoneID;

      map_type = FIELDMAP_GetMapControlType( fieldmap );
      
      if (map_type == FLDMAP_CTRLTYPE_GRID){
        VecFx32 pos;
        //自機の座標
        FIELD_PLAYER_GetPos( fld_player, &pos );
        child = DEBUG_EVENT_ChangeMapPos(gsys, fieldmap,
            zone_id, &pos, DIR_DOWN );
      }
      else{
        RAIL_LOCATION rail_loc;
        MMDL_GetRailLocation( FIELD_PLAYER_GetMMdl( fld_player ), &rail_loc );
        child = DEBUG_EVENT_ChangeMapRailLocation(gsys, fieldmap,
            zone_id, &rail_loc, DIR_DOWN );
      }
      //イベントコール
      GMEVENT_CallEvent(event, child);
    }
    (*seq)++;
    break;
  case 8:
    //終了
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//パーティクル再生
static BOOL PlayParticle(FLD3D_CI_PTR ptr)
{
  if (!ptr->PartGene){
    if (ptr->PrtclWait > 0){
      ptr->PrtclWait--;
    }else{
      //パーティクルジェネレート
      Generate(ptr);
      ptr->PartGene = 1;
    }
  }else{
    //パーティクル終了判定
    if ( FLD_PRTCL_CheckEmitEnd( ptr->PrtclSys ) ){
      return TRUE;
    }
  }
  return FALSE;
}

//モデルアニメ1再生
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr)
{
  if (ptr->MdlAnm1Wait > 0){
    ptr->MdlAnm1Wait--;
  }else{
    BOOL rc;
    u8 i;
    u16 num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx );
    num = GFL_G3D_OBJECT_GetAnimeCount( obj );
    //アニメ再生
    for (i=0;i<num;i++){
      rc = GFL_G3D_OBJECT_IncAnimeFrame( obj, i, FX32_ONE );
    }
    if (rc == FALSE){
      return TRUE;
    }
  }
  return FALSE;
}

//モデルアニメ2再生
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr)
{
  if (ptr->MdlAnm2Wait > 0){
    ptr->MdlAnm2Wait--;
  }else{
    //アニメ再生
    BOOL rc;
    u8 i;
    u16 num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx+1 );
    num = GFL_G3D_OBJECT_GetAnimeCount( obj );
    //アニメ再生
    for (i=0;i<num;i++){
      rc = GFL_G3D_OBJECT_IncAnimeFrame( obj, i, FX32_ONE );
    }
    if (rc == FALSE){
      return TRUE;
    }
  }
  return FALSE;
}

static void SetupResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat, const u8 inCutInNo)
{
  //パーティクル生成
  ptr->PrtclPtr = FLD_PRTCL_Create(ptr->PrtclSys);
  //セットアップデータ作成
  CreateRes(outDat, inCutInNo, ptr->HeapID);
  
  //3Ｄモデルリソースをセットアップ
  SetupResourceCore(ptr, &outDat->Setup);
  //パーティクルリソースセットアップ
  {
    void *resource;
    resource = FLD_PRTCL_LoadResource(ptr->PrtclSys,
			ARCID_FLD3D_CI, outDat->SpaIdx);

    FLD_PRTCL_SetResource(ptr->PrtclSys, resource, TRUE, NULL);
  }
  //アニメを有効にする
  {
    u8 i,j;
    u16 obj_num,anm_num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj_num = GFL_G3D_UTIL_GetUnitObjCount( ptr->Util, ptr->UnitIdx );
    for(i=0;i<obj_num;i++){
      obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx+i );
      anm_num = GFL_G3D_OBJECT_GetAnimeCount( obj );
      //アニメ有効
      for (j=0;j<anm_num;j++){
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }
  //ウェイト設定
  ptr->PrtclWait = outDat->SpaWait;
  ptr->MdlAnm1Wait = outDat->MdlAAnmWait;
  ptr->MdlAnm2Wait = outDat->MdlBAnmWait;
}

static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat)
{
  DeleteRes(outDat);
  GFL_G3D_UTIL_DelUnit( ptr->Util, ptr->UnitIdx );
  ptr->UnitIdx = UNIT_NONE;
  FLD_PRTCL_Delete(ptr->PrtclSys);
}

static BOOL CheckAnmEnd(FLD3D_CI_PTR ptr)
{
  //パーティクル終了判定
  if ( !FLD_PRTCL_CheckEmitEnd( ptr->PrtclSys ) ){
    return FALSE;
  }

  return TRUE;
}



static void Generate(FLD3D_CI_PTR ptr)
{
///  GFL_PTC_SetCameraType(sys->PrtclPtr, GFL_G3D_PRJORTH);
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 0,
									ParticleCallBack, NULL);
/**  
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 1,
									ParticleCallBack, NULL);
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 2,
									ParticleCallBack, NULL);
*/                  
}

//--------------------------------------------------------------
/**
 * @brief	コールバック
 *
 * @param	emit	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ParticleCallBack(GFL_EMIT_PTR emit)
{
///	SOFT_SPRITE *ss = Particle_GetTempPtr();
	VecFx32 pos = { 0, 0, 0 };
	
	pos.z = 0x40;
	SPL_SetEmitterPosition(emit, &pos);
}

//--------------------------------------------------------------
/**
 * @brief	リソース作成関数
 *
 * @param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void CreateRes(RES_SETUP_DAT *outDat, const u8 inResArcIdx, const HEAPID inHeapID)
{
  u8 i;
  RES_DEF_DAT def_dat;
  u8 res_num,obj_num,anm1_num,anm2_num;
  u8 obj_res_start,anm1_res_start,anm2_res_start;

  //アーカイブからリソース定義をロード
  GFL_ARC_LoadData(&def_dat, ARCID_FLD3D_CI_SETUP, inResArcIdx);

  //OBJ数を調べる
  obj_num = 0;
  for (i=0;i<OBJCOUNT_MAX;i++){
    if (def_dat.MdlObjIdx[i] != NONDATA){
      obj_num++;
    }
  }
  //アニメ数を調べる
  anm1_num = 0;
  anm2_num = 0;
  for(i=0;i<ANM_TYPE_MAX;i++){
    if (def_dat.MdlAAnmIdx[i] != NONDATA){
      anm1_num++;
    }
  }
  for(i=0;i<ANM_TYPE_MAX;i++){
    if (def_dat.MdlBAnmIdx[i] != NONDATA){
      anm2_num++;
    }
  }

  obj_res_start = 0;
  anm1_res_start = obj_res_start + obj_num;
  anm2_res_start = anm1_res_start + anm1_num;

  //リソース数を調べる
  res_num = obj_num+anm1_num+anm2_num;
#if 0
  OS_Printf("obj_anm_res = %d %d %d %d\n",obj_num, anm1_num, anm2_num, res_num);
  OS_Printf("res_start = %d %d %d \n",obj_res_start,anm1_res_start,anm2_res_start );
#endif
  //リソーステーブルアロケート
  if (res_num){
    outDat->Res = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_RES)*res_num);
  }
  //ＯＢＪテーブルアロケート
  if(obj_num){
    outDat->Obj = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_OBJ)*obj_num);
  }
  //アニメテーブルアロケート
  if(anm1_num){
    outDat->Anm1 = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_ANM)*anm1_num);
  }
  //アニメテーブルアロケート
  if(anm2_num){
    outDat->Anm2 = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_ANM)*anm2_num);
  }

  //リソース数記憶
  outDat->ResNum = res_num;
  outDat->ObjNum = obj_num;
  outDat->AnmNum = anm1_num+anm2_num;
  //ウェイト記憶
  outDat->SpaWait = def_dat.SpaWait;
  outDat->MdlAAnmWait = def_dat.MdlAAnmWait;
  outDat->MdlBAnmWait = def_dat.MdlBAnmWait;

  {
    u8 i;
    u8 count = 0;
    u16 arc_id[RESCOUNT_MAX];
    arc_id[0] = def_dat.MdlObjIdx[0];
    arc_id[1] = def_dat.MdlObjIdx[1];
    arc_id[2] = def_dat.MdlAAnmIdx[0];
    arc_id[3] = def_dat.MdlAAnmIdx[1];
    arc_id[4] = def_dat.MdlAAnmIdx[2];
    arc_id[5] = def_dat.MdlBAnmIdx[0];
    arc_id[6] = def_dat.MdlBAnmIdx[1];
    arc_id[7] = def_dat.MdlBAnmIdx[2];


    for (i=0;i<RESCOUNT_MAX;i++){
      if ( arc_id[i] != NONDATA ){
        outDat->Res[count].arcive = ARCID_FLD3D_CI;
        outDat->Res[count].datID = arc_id[i];
        outDat->Res[count].arcType = GFL_G3D_UTIL_RESARC;
        count++;
      }
    }
  }

  {
    u8 i;
    for (i=0;i<anm1_num;i++){
      outDat->Anm1[i].anmresID = anm1_res_start+i;
      outDat->Anm1[i].anmdatID = 0;
    }
    for (i=0;i<anm2_num;i++){
      outDat->Anm2[i].anmresID = anm2_res_start+i;
      outDat->Anm2[i].anmdatID = 0;
    }
  }

  {
    u8 i;
    for (i=0;i<obj_num;i++){
      outDat->Obj[i].mdlresID = obj_res_start+i;  
      outDat->Obj[i].mdldatID = 0;
      outDat->Obj[i].texresID = obj_res_start+i;
    }
    if (obj_num>=1){
      outDat->Obj[0].anmTbl = outDat->Anm1;
      outDat->Obj[0].anmCount = anm1_num;
    }
    if (obj_num>=2){
      outDat->Obj[1].anmTbl = outDat->Anm2;
      outDat->Obj[1].anmCount = anm2_num;
    }
  }


  outDat->Setup.resTbl = outDat->Res;
  outDat->Setup.resCount = res_num;
  outDat->Setup.objTbl = outDat->Obj;
  outDat->Setup.objCount = obj_num;

  outDat->SpaIdx = def_dat.SpaIdx;
}

//--------------------------------------------------------------
/**
 * @brief	リソース破棄関数
 *
 * @param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void DeleteRes(RES_SETUP_DAT *outDat)
{
  if (outDat->Anm1 != NULL){
    GFL_HEAP_FreeMemory( outDat->Anm1 );
  }
  if (outDat->Anm2 != NULL){
    GFL_HEAP_FreeMemory( outDat->Anm2 );
  }
  if (outDat->Obj != NULL){
    GFL_HEAP_FreeMemory( outDat->Obj );
  }
  if(outDat->Res != NULL){
    GFL_HEAP_FreeMemory( outDat->Res );
  }
}

#ifdef PM_DEBUG
#define FLYSKY_CAM_MOVE_FRAME (10)
//--------------------------------------------------------------
/**
 * @brief	空を飛ぶカメラアングルになるイベントを作成する
 * @todo　この機能はこのファイルから切り離す
 *
 * @param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void FLD3D_CI_FlySkyCameraDebug(
    GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, FIELD_CAMERA *camera, FIELD_PLAYER * player, FLDNOGRID_MAPPER* mapper )
{
  FLYSKY_EFF_WORK *work;
  int size;
  size = sizeof(FLYSKY_EFF_WORK);
  //イベント作成
  {
    GMEVENT * event = GMEVENT_Create( gsys, NULL, DebugFlySkyEffEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );
    work->gsys = gsys;
    work->camera = camera;
    work->CiPtr = ptr;
    work->player = player;
    work->mapper = mapper;

    GAMESYSTEM_SetEvent(gsys, event);
  }
}

static GMEVENT_RESULT DebugFlySkyEffEvt( GMEVENT* event, int* seq, void* work )
{
  FLYSKY_EFF_WORK *wk;
  wk = work;

  switch(*seq){
  case 0:
    //現在ウォッチターゲットを保存
    wk->Watch = FIELD_CAMERA_GetWatchTarget(wk->camera);
    FIELD_CAMERA_StopTraceRequest(wk->camera);
    (*seq)++;
    break;
  case 1:
    if ( (wk->Watch != NULL) && FIELD_CAMERA_CheckTrace(wk->camera) ){
      break;
    }
    //レールシステム更新ストップ
    FLDNOGRID_MAPPER_SetRailCameraActive( wk->mapper, FALSE );
    //カメラパージ
    FIELD_CAMERA_FreeTarget(wk->camera);
    //現在のカメラ設定を保存
    FIELD_CAMERA_SetRecvCamParam(wk->camera);
    //カメラを空飛びカメラへ線形移動リクエスト
    {
      FLD_CAM_MV_PARAM param;
      VecFx32 player_vec;
      //主人公の3Ｄ座標を取得
      FIELD_PLAYER_GetPos( wk->player, &player_vec );
      param.Core.AnglePitch = 9688;
      param.Core.AngleYaw = 0;
      param.Core.Distance = 970752;
      param.Core.Shift.x = 0;
      param.Core.Shift.y = 16384;
      param.Core.Shift.z = 0;
      param.Core.TrgtPos = player_vec;
      param.Core.Fovy = 3640;
      param.Chk.Shift = TRUE;
      param.Chk.Pitch = TRUE;
      param.Chk.Yaw = FALSE;
      param.Chk.Dist = TRUE;
      param.Chk.Fovy = TRUE;
      param.Chk.Pos = TRUE;
      FIELD_CAMERA_SetLinerParam(wk->camera, &param, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 2:
    //カメラ移動待ち
    if ( !FIELD_CAMERA_CheckMvFunc(wk->camera) )
    {
      GMEVENT *child;
#if 0      
      {
        u16 pitch, yaw;
        fx32 len;
        u16 fovy;
        VecFx32 shift, target;
        OS_Printf("--DUMP_FIELD_CAMERA_PARAM--\n");
        pitch = FIELD_CAMERA_GetAnglePitch(wk->camera);
        yaw = FIELD_CAMERA_GetAngleYaw(wk->camera);
        len = FIELD_CAMERA_GetAngleLen(wk->camera);
        fovy = FIELD_CAMERA_GetFovy(wk->camera);
        FIELD_CAMERA_GetTargetOffset( wk->camera, &shift );
        FIELD_CAMERA_GetTargetPos( wk->camera, &target );
      
        OS_Printf("%d,%d,%d,%d,%d,%d\n", pitch, yaw, len, target.x, target.y, target.z );
        OS_Printf("%d,%d,%d,%d\n", fovy, shift.x, shift.y, shift.z );
      }
#endif      
      //カットイン演出開始
      child = CreateCutInEvt(wk->gsys, wk->CiPtr, 0);
      GMEVENT_CallEvent(event, child);
      (*seq)++;
    }
    break;
  case 3:
    {
      FLD_CAM_MV_PARAM_CHK chk;
      chk.Shift = TRUE;
      chk.Pitch = TRUE;
      chk.Yaw = FALSE;
      chk.Dist = TRUE;
      chk.Fovy = TRUE;
      chk.Pos = TRUE;
      //カメラ戻し
      FIELD_CAMERA_RecvLinerParam(wk->camera, &chk, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 4:
    //カメラ戻し待ち
    if ( !FIELD_CAMERA_CheckMvFunc(wk->camera) ){
      //カメラバインド
      if (wk->Watch != NULL){
        FIELD_CAMERA_BindTarget(wk->camera, wk->Watch);
      }
      FIELD_CAMERA_RestartTrace(wk->camera);
      //レールシステム更新開始
      FLDNOGRID_MAPPER_SetRailCameraActive( wk->mapper, TRUE );
      //復帰データのクリア
      FIELD_CAMERA_ClearRecvCamParam(wk->camera);
      return GMEVENT_RES_FINISH;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//キャプチャリクエスト
static void ReqCapture(FLD3D_CI_PTR ptr)
{
  ptr->CapEndFlg = FALSE;
  GX_SetBankForLCDC(GX_VRAM_LCDC_D);
  GFUser_VIntr_CreateTCB(Graphic_Tcb_Capture, &ptr->CapEndFlg, 0 );
}


#endif  //PM_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief	Vブランクタスク
 *
 *	@param	GFL_TCB *p_tcb
 *	@param	*p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work )
{
  BOOL *cap_end_flg = p_work;
  
	GX_SetCapture(GX_CAPTURE_SIZE_256x192,  // Capture size
                      GX_CAPTURE_MODE_A,			   // Capture mode
                      GX_CAPTURE_SRCA_3D,						 // Blend src A
                      GX_CAPTURE_SRCB_VRAM_0x00000,     // Blend src B
                      GX_CAPTURE_DEST_VRAM_D_0x00000,   // Output VRAM
                      16,             // Blend parameter for src A
                      0);            // Blend parameter for src B

  GFL_TCB_DeleteTask( p_tcb );
  if (cap_end_flg != NULL){
    *cap_end_flg = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	プライオリティプッシュ
 *
 *	@param	ptr       カットイン管理ポインタ
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushPriority(FLD3D_CI_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    ptr->BgPriority[i] = GFL_BG_GetPriority( i );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示状態プッシュ
 *
 *	@param	ptr       カットイン管理ポインタ
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushDisp(FLD3D_CI_PTR ptr)
{
  ptr->Mask = GX_GetVisiblePlane();
}

//----------------------------------------------------------------------------
/**
 *	@brief	プライオリティポップ
 *
 *	@param	ptr       カットイン管理ポインタ
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopPriority(FLD3D_CI_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    GFL_BG_SetPriority( i, ptr->BgPriority[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示状態ポップ
 *
 *	@param	ptr       カットイン管理ポインタ
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopDisp(FLD3D_CI_PTR ptr)
{
  GX_SetVisiblePlane( ptr->Mask );
  //@todo メッセージ表示等でＢＧ1を使用しているが、ＢＧ1がオンであることを期待したつくりになっているので、
  //演出後ポップするとＢＧ1はオン状態で復帰し、ごみがでる。
  //なので、ここではとりあえず、ＢＧ1をオフにしておく
  GX_SetVisiblePlane(GX_PLANEMASK_BG0);
}




