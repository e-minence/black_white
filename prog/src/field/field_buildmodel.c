//============================================================================================
/**
 * @file	field_buildmodel.c
 * @brief		配置モデルの制御
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.20
 *
 * @todo
 * メモリ削減：（１）テクスチャリソースをエリア単位で統合する（２）アニメリソースの重複を減らす
 */
//============================================================================================

#include <gflib.h>

#include "arc_def.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "field/field_const.h"
#include "field_buildmodel.h"
#include "field_bmanime.h"

#include "system/rtc_tool.h"  //GFL_RTC_GetTimeZone

#include "field_g3d_mapper.h"		//下記ヘッダに必要

#include "system/el_scoreboard.h"

#include "savedata/save_control.h"    //テストでランダムマップの都市の種類を取得
#include "savedata/randommap_save.h"  //テストでランダムマップの都市の種類を取得

typedef EL_SCOREBOARD_TEX ELBOARD_TEX;

#include "arc/fieldmap/area_id.h"
#include "arc/fieldmap/buildmodel_info.naix"
#include "arc/fieldmap/buildmodel_outdoor.naix"
#include "arc/fieldmap/buildmodel_indoor.naix"

#include "map/dp3format.h"

//============================================================================================
//============================================================================================

enum {
  GFL_G3D_MAP_OBJST_MAX = 32,
  GFL_G3D_MAP_OBJID_NULL = 0xffffffff,

  MAPBLOCK_MAX = 9,
};
//------------------------------------------------------------------
//------------------------------------------------------------------
enum { GLOBAL_OBJ_ANMCOUNT	= 4 };

enum { BMODEL_USE_MAX = 4 };

enum{ 
  FILEID_BMODEL_ANIMEDATA = NARC_buildmodel_info_buildmodel_anime_bin,
  FILEID_BMINFO_OUTDOOR = NARC_buildmodel_info_buildmodel_outdoor_bin,
  FILEID_BMINFO_INDOOR = NARC_buildmodel_info_buildmodel_indoor_bin,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  BMODEL_ID_MAX = 400,  //とりあえず
  BMODEL_ENTRY_MAX = 128,  //とりあえず

  BMODEL_ID_NG  = BMODEL_ID_MAX,
  BMODEL_ENTRY_NG = BMODEL_ENTRY_MAX,

  BMANIME_NULL_ID  = 0xffff,
  //BM_SUBMODEL_NULL_ID = 0xffff,

  //電光掲示板で表示する文字列の長さ
  BMODEL_ELBOARD_STR_LEN = 64 * 2,
};

static const BMODEL_ID BM_SUBMODEL_NULL_ID = 0xffff;

//------------------------------------------------------------------
///アニメ状態
//------------------------------------------------------------------
typedef enum {
  BM_ANMMODE_NOTHING = 0, ///<アニメ状態：なし
  BM_ANMMODE_LOOP,        ///<アニメ状態：ループ制御
  BM_ANMMODE_STOP,        ///<アニメ状態：停止中
  BM_ANMMODE_TEMPORARY,   ///<アニメ状態：一時的に適用中
  BM_ANMMODE_TIMEZONE,    ///<アニメ状態：時間帯で制御
}BM_ANMMODE;

//------------------------------------------------------------------
/// 配置モデルアニメ情報
//------------------------------------------------------------------
typedef struct _FIELD_BMANIME_DATA
{ 
  u8 anm_type;  ///<BMANIME_TYPE  アニメの種類指定
  u8 prg_type;  ///<BMANIME_PROG_TYPE 動作プログラムの種類指定
  u8 anm_count; ///<アニメカウント（未使用）
  u8 set_count; ///<セットカウント（未使用）
  u16 IDs[GLOBAL_OBJ_ANMCOUNT]; ///<アニメアーカイブ指定ID
}FIELD_BMANIME_DATA;

//------------------------------------------------------------------
/// 配置モデル情報
//------------------------------------------------------------------
typedef struct {
  BMODEL_ID bm_id;              ///<配置モデルID
  u16 prog_id;                  ///<BM_PROG_ID プログラム指定ID
  BMODEL_ID sub_bm_id;          ///<従属モデル指定ID
  s16 sx, sy, sz;               ///<従属モデルの相対位置
  u16 anm_id;                   ///<アニメ指定ID
  FIELD_BMANIME_DATA animeData; ///<アニメ指定データ
}BMINFO;

//------------------------------------------------------------------
/// 配置モデルオブジェクトリソース
//------------------------------------------------------------------
typedef struct {
  const BMINFO * bmInfo;              ///<配置モデル情報
  GFL_G3D_RES*  g3DresMdl;            ///<モデルリソース
  GFL_G3D_RES*  g3DresTex;            ///<テクスチャリソース
  GFL_G3D_RES*  g3DresAnm[GLOBAL_OBJ_ANMCOUNT];  ///<アニメリソース
}OBJ_RES;

//------------------------------------------------------------------
/// モデル制御オブジェクト
//------------------------------------------------------------------
typedef struct {
  GFL_G3D_OBJ*  g3Dobj;              ///<オブジェクトハンドル
  const OBJ_RES * res;              ///<リソースへのポインタ
  BM_ANMMODE anmMode[GLOBAL_OBJ_ANMCOUNT];  ///<保持アニメの状態
}OBJ_HND;

//------------------------------------------------------------------
/// 配置モデル制御ワーク（外部からの制御用）
//------------------------------------------------------------------
struct _FIELD_BMODEL {
  OBJ_HND objHdl;         ///<モデル制御オブジェクト
  GFL_G3D_OBJSTATUS g3dObjStatus; ///<表示制御オブジェクト
};

//------------------------------------------------------------------
/**
 * GFL_G3D_MAPの保持するGFL_G3D_MAP_GLOBALOBJ_STを
 * 参照、コントロールするためのラッパーオブジェクト
 */
//------------------------------------------------------------------
struct _G3DMAPOBJST{
  GFL_G3D_MAP * g3Dmap;   ///<所属GFL_G3D_MAPへの参照
  u32 entryNoBackup;      ///<モデル指定IDのバックアップ
  u16 index;              ///<所属g3Dmap内でのインデックス
  u16 viewFlag;           ///<可視設定フラグ
  GFL_G3D_MAP_GLOBALOBJ_ST * objSt; ///<実オブジェクトへのポインタ
};

//------------------------------------------------------------------
///時間帯アニメ制御ワーク
//------------------------------------------------------------------
typedef struct {
  TIMEZONE NowTimeZone;   ///<現在の時間帯
  TIMEZONE OldTimeZone;   ///<直前の時間帯
  BOOL update_flag;       ///<更新が必要かどうか
  u8 index;               ///<時間帯から算出されたアニメ指定
}TIMEANIME_CTRL;

//------------------------------------------------------------------
/** 
 * @brief 配置モデルマネジャー用制御ワーク
 */
//------------------------------------------------------------------
struct _FIELD_BMODEL_MAN
{
	HEAPID heapID;

  FLDMAPPER * fldmapper;  //描画オフセット取得に必要。できたら不要にしたい
  TIMEANIME_CTRL tmanm_ctrl;  ///<時間帯アニメ制御ワーク

  ///配置モデルID→登録IDの変換テーブル
	u8 BMIDToEntryTable[BMODEL_ID_MAX];

  ///配置モデル登録数
	u16 entryCount;
  ///配置モデル情報
  BMINFO bmInfo[BMODEL_ENTRY_MAX];
  ///登録ID→配置モデルIDの変換テーブル
	BMODEL_ID entryToBMIDTable[BMODEL_ENTRY_MAX];

  ///適用する配置モデルアーカイブ指定
	ARCID mdl_arc_id;
  
  STRBUF * elb_str[FIELD_BMODEL_ELBOARD_ID_MAX];
  ELBOARD_TEX * elb_tex[FIELD_BMODEL_ELBOARD_ID_MAX];

	GFL_G3D_MAP_GLOBALOBJ	g3dMapObj;						///<共通オブジェクト

	u32	objRes_Count;		  		///<共通オブジェクトリソース数
	OBJ_RES * objRes;					///<共通オブジェクトリソース

  u32 objHdl_Count;
  OBJ_HND * objHdl;

  FIELD_BMODEL * bmodels[BMODEL_USE_MAX];

  G3DMAPOBJST g3DmapObjSt[GFL_G3D_MAP_OBJST_MAX * MAPBLOCK_MAX];
};

//============================================================================================
//
//    デバッグ用関数
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline BMODEL_ID BMIDAssert(BMODEL_ID bm_id)
{
  if (bm_id >= BMODEL_ID_MAX)
  {
    OS_TPrintf("BuildModel ID over %d\n",bm_id );
    GF_ASSERT(0);
    bm_id = 0;
  }
  return bm_id;
}
#define BMID_ASSERT(bm_id) {bm_id = BMIDAssert(bm_id);}

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u8 EntryNoAssert(const FIELD_BMODEL_MAN * man, u8 entryNo)
{
  if (entryNo >= BMODEL_ENTRY_MAX || entryNo >= man->entryCount)
  {
    OS_TPrintf("BuildModel Entry No Over(%d)\n",entryNo);
    GF_ASSERT(0);
    entryNo = 0;
  }
  return entryNo;
}

#define ENTRYNO_ASSERT(man, entryNo) {entryNo = EntryNoAssert(man, entryNo);}

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u8 AnimeNoAssert(u8 anmNo)
{
  if (anmNo >= GLOBAL_OBJ_ANMCOUNT)
  {
    OS_TPrintf("anmNo Index Over!!(%d)\n", anmNo);
    GF_ASSERT(0);
    anmNo = 0;
  }
  return anmNo;
}

#define ANIMENO_ASSERT(anmNo) {anmNo = AnimeNoAssert(anmNo);}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man);
static u16 calcArcIndex(u16 area_id);

static void loadEntryToBMIDTable(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id);
static u8 BMIDtoEntryNo(const FIELD_BMODEL_MAN * man, BMODEL_ID id);


//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * FIELD_BMODEL_MAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id);

static void BMINFO_Load(FIELD_BMODEL_MAN * man, u16 file_id);
static void BMINFO_init(BMINFO * bmInfo);
//アニメデータの取得処理
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo);
static BOOL BMINFO_isDoor(const BMINFO * bmInfo);

static u32 BMANIME_getCount(const FIELD_BMANIME_DATA * data);
static void BMANIME_init(FIELD_BMANIME_DATA * data);
static void DEBUG_BMANIME_dump(const FIELD_BMANIME_DATA * data);

//------------------------------------------------------------------
//------------------------------------------------------------------

static void createAllResource(FIELD_BMODEL_MAN * man);
static void deleteAllResource(FIELD_BMODEL_MAN * man);

static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, GFL_G3D_MAP_GLOBALOBJ * g3dMapObj);
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, GFL_G3D_MAP_GLOBALOBJ * g3dMapObj);

static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, BMODEL_ID bm_id);
static void OBJRES_finalize( OBJ_RES * objRes );
static GFL_G3D_RES* OBJRES_getResTex(const OBJ_RES * resTex);

static void OBJHND_initialize(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl, const OBJ_RES* objRes);
static void OBJHND_finalize( OBJ_HND * objHdl );
static void OBJHND_animate(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req);
static BOOL OBJHND_getAnimeStatus(OBJ_HND * objHdl, u32 anmNo);

static void TIMEANIME_CTRL_init( TIMEANIME_CTRL * tmanm_ctrl );
static void TIMEANIME_CTRL_update( TIMEANIME_CTRL * tmanm_ctrl );
static BOOL TIMEANIME_CTRL_needUpdate( const TIMEANIME_CTRL * tmanm_ctrl );
static u8   TIMEANIME_CTRL_getIndex( const TIMEANIME_CTRL * tmanm_ctrl );
//------------------------------------------------------------------
//  テクスチャ情報の登録処理
//------------------------------------------------------------------
static void entryELBoardTex(FIELD_BMODEL_MAN* man, const FIELD_BMANIME_DATA * data,
    const GFL_G3D_RES * g3DresTex);

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST * status, int idx);
static void G3DMAPOBJST_init(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByObject(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap);

static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel );

//============================================================================================
//
//
//    外部公開関数：
//    マネジャー生成･削除・ロード・メイン
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 配置モデルマネジャー生成
 * @param heapID
 */
//------------------------------------------------------------------
FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID, FLDMAPPER * fldmapper)
{	
  int i;
	FIELD_BMODEL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_BMODEL_MAN));
	man->heapID = heapID;
  man->fldmapper = fldmapper;
  TIMEANIME_CTRL_init( &man->tmanm_ctrl );

  for (i = 0; i < FIELD_BMODEL_ELBOARD_ID_MAX; i ++) 
  { 
    man->elb_str[i] = GFL_STR_CreateBuffer(BMODEL_ELBOARD_STR_LEN, heapID);
    man->elb_tex[i] = NULL;
  }

  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    man->bmodels[i] = NULL;
  }
  for (i = 0; i < NELEMS(man->g3DmapObjSt); i++)
  {
    G3DMAPOBJST_init( man, &man->g3DmapObjSt[i] );
  }

	man->g3dMapObj.gobj = NULL;
	man->g3dMapObj.gobjCount = 0;
	man->g3dMapObj.gobjIDexchange = NULL;

	man->g3dMapObj.gddobj = NULL;
	man->g3dMapObj.gddobjCount = 0;
	man->g3dMapObj.gddobjIDexchange = NULL;
	
  man->objRes_Count = 0;
  man->objRes = NULL;

  man->objHdl_Count = 0;
  man->objHdl = NULL;

	return man;
}

//------------------------------------------------------------------
/**
 * @brief 配置モデルマネジャー消去
 * @param man 配置モデルマネジャーへのポインタ
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Delete(FIELD_BMODEL_MAN * man)
{	
  int i;
  for (i = 0; i < FIELD_BMODEL_ELBOARD_ID_MAX; i ++) 
  { 
    GFL_STR_DeleteBuffer(man->elb_str[i]);
    if (man->elb_tex[i]) 
    { 
      ELBOARD_TEX_Delete(man->elb_tex[i]);
    }
  }
  deleteFullTimeObjHandle( man, &man->g3dMapObj );
  deleteAllResource( man );

	GFL_HEAP_FreeMemory(man);
}

//------------------------------------------------------------------
/**
 * @brief 配置モデルマネジャー：更新処理
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Main(FIELD_BMODEL_MAN * man)
{ 
  int i;

  TIMEANIME_CTRL_update( &man->tmanm_ctrl );

  for (i = 0; i < FIELD_BMODEL_ELBOARD_ID_MAX; i++)
  { 
    if (man->elb_tex[i])
    { 
      ELBOARD_TEX_Main(man->elb_tex[i]);
    }
  }

  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      OBJ_HND * objHdl = &man->bmodels[i]->objHdl;
      OBJHND_animate( man, objHdl );
    }
  }

  //アニメーションコントロール（暫定でフレームループさせているだけ）

  for( i=0; i<man->objHdl_Count; i++ ){
    OBJ_HND * objHdl = &man->objHdl[i];
    OBJHND_animate( man, objHdl );
  }

}

//------------------------------------------------------------------
/**
 * @brief   配置モデルマネジャー：描画処理
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Draw(FIELD_BMODEL_MAN * man)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      FIELD_BMODEL_Draw(man->bmodels[i]);
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief 配置モデルデータ読み込み処理
 * @param man 配置モデルマネジャーへのポインタ
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid, const AREADATA * areadata)
{	
	u16 area_id = ZONEDATA_GetAreaID(zoneid);
	u16 bmlist_index = calcArcIndex(area_id);
  ARCID bmlist_arc_id;
  u16 model_info_dataid;
  int i;

	GFL_STD_MemClear(man->BMIDToEntryTable, sizeof(man->BMIDToEntryTable));
	GFL_STD_MemFill16(man->entryToBMIDTable, BMODEL_ENTRY_NG, sizeof(man->entryToBMIDTable));
  for (i = 0; i < BMODEL_ENTRY_MAX; i++)
  { 
    BMINFO_init( &man->bmInfo[i] );
  }

  if (AREADATA_GetInnerOuterSwitch(areadata) != 0) 
  {	
    man->mdl_arc_id = ARCID_BMODEL_OUTDOOR;
    model_info_dataid = FILEID_BMINFO_OUTDOOR;
    bmlist_arc_id = ARCID_BMODEL_IDX_OUTDOOR;
  }
  else
  {	
    man->mdl_arc_id = ARCID_BMODEL_INDOOR;
    model_info_dataid = FILEID_BMINFO_INDOOR;
    bmlist_arc_id = ARCID_BMODEL_IDX_INDOOR;
  }

  //エリア別配置モデルIDリストの読み込み
  loadEntryToBMIDTable(man, bmlist_arc_id, bmlist_index);

	//ID→登録順序の変換データテーブル生成
	makeBMIDToEntryTable(man);

  //必要な配置モデルデータの読み込み
  BMINFO_Load(man, model_info_dataid);

  createAllResource( man );
  createFullTimeObjHandle( man, &man->g3dMapObj );
}

//============================================================================================
//
//    外部公開関数：
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 配置モデルIDを登録済み配置モデルのインデックスに変換する
 * @param man 配置モデルマネジャーへのポインタ
 */
//------------------------------------------------------------------
u8 FIELD_BMODEL_MAN_GetEntryIndex(const FIELD_BMODEL_MAN * man, BMODEL_ID id)
{	
  return BMIDtoEntryNo( man, id );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * FIELD_BMODEL_MAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id)
{
  u16 entryNo = BMIDtoEntryNo( man, bm_id );
  return &man->bmInfo[entryNo];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_MAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo)
{
  u16 submodel_id;
  const BMINFO * bmInfo;
  //現状、屋外にしかサブモデルを持つ配置モデルは存在しない
  if (man->mdl_arc_id != ARCID_BMODEL_OUTDOOR)
  {
    return FALSE;
  }
  bmInfo = FIELD_BMODEL_MAN_GetBMInfo( man, bm_id );
  submodel_id = bmInfo->sub_bm_id;
  if (submodel_id == BM_SUBMODEL_NULL_ID)
  {
    return FALSE;
  }
  *entryNo = BMIDtoEntryNo(man, submodel_id);
  VEC_Set( ofs, bmInfo->sx << FX32_SHIFT, bmInfo->sy << FX32_SHIFT, bmInfo->sz << FX32_SHIFT );
  return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief ヒープ指定の取得
 * @param man
 * @return  HEAPID  ヒープ指定
 */
//------------------------------------------------------------------
HEAPID FIELD_BMODEL_MAN_GetHeapID(const FIELD_BMODEL_MAN * man)
{
  return man->heapID;
}

//============================================================================================
//
//    外部公開関数：
//      配置モデル登録・削除処理
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ResistAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  GFL_G3D_MAP_GLOBALOBJ_ST status;
  int dataCount, resistCount, count;

  for( dataCount=0, resistCount = 0, count = objCount; dataCount<count ; resistCount++, dataCount++ )
  {
    if (resistCount >= GFL_G3D_MAP_OBJST_MAX)
    {
      OS_Printf("マップブロック内の配置モデル数が%dを超えているため表示できません\n",
          GFL_G3D_MAP_OBJST_MAX);
      GF_ASSERT(0);
    }

    FIELD_BMODEL_MAN_ResistMapObject(man, g3Dmap, &objStatus[dataCount], resistCount);

    if (TRUE == FIELD_BMODEL_MAN_GetSubModel(man,
          objStatus[dataCount].resourceID, &status.trans, &status.id) )
    {
      TAMADA_Printf("Resist Sub Model:index(%d) model id(%d)\n", dataCount, status.id);
      resistCount++;
      status.rotate = objStatus[dataCount].rotate;
      status.trans.x += objStatus[dataCount].xpos;
      status.trans.y += objStatus[dataCount].ypos;
      status.trans.z -= objStatus[dataCount].zpos;
      G3DMAPOBJST_create(man, g3Dmap, &status, resistCount);
    }

  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ReleaseAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap)
{
  G3DMAPOBJST_deleteByG3Dmap( man, g3Dmap );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ResistMapObject
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  GFL_G3D_MAP_GLOBALOBJ_ST status;
  status.id = BMIDtoEntryNo(man, objStatus->resourceID);
  VEC_Set(&status.trans, objStatus->xpos, objStatus->ypos, -objStatus->zpos);
  status.rotate = (u16)(objStatus->rotate);
  G3DMAPOBJST_create(man, g3Dmap, &status, objCount);
}


//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == bmodel)
    {
      man->bmodels[i] = NULL;
      return;
    }
  }
  GF_ASSERT(0);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
{
  int i;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == NULL)
    {
      man->bmodels[i] = bmodel;
      return;
    }
  }
  GF_ASSERT(0);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GFL_G3D_MAP_GLOBALOBJ * FIELD_BMODEL_MAN_GetGlobalObjects(FIELD_BMODEL_MAN * man)
{
  return &man->g3dMapObj;
}




//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief エリア別配置モデルIDリストの読み込み
 * @param man
 * @param arc_id  エリア別配置モデルリストのアーカイブ指定ID（屋内or屋外）
 * @param file_id  エリアIDから変換したファイル指定ID
 *
 * entryCount/entryToBMIDTableを書き換える
 */
//------------------------------------------------------------------
static void loadEntryToBMIDTable(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id)
{ 
  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(arc_id, man->heapID);

  {
    u16 data_max = GFL_ARC_GetDataFileCntByHandle(handle);
    if (data_max <= file_id)
    {	
      GF_ASSERT_MSG(0, "配置モデルリストデータがありません(%d<%d)\n", data_max, file_id);
      file_id = 0;		//とりあえずハングアップ回避
    }
  }

	{	
		u16 size = GFL_ARC_GetDataSizeByHandle(handle, file_id);
		man->entryCount = size / sizeof(BMODEL_ID);
		if(size > sizeof(man->entryToBMIDTable))
		{	
			GF_ASSERT_MSG(0, "配置モデルリストデータがオーバー（size=%d ARCINDEX=%d)\n", size, file_id);
			man->entryCount = BMODEL_ENTRY_MAX;	//とりあえずハングアップ回避
			size = sizeof(man->entryToBMIDTable);
		}
		//TAMADA_Printf("entryCount=%d\n", man->entryCount);
		GFL_ARC_LoadDataOfsByHandle(handle, file_id, 0, size, man->entryToBMIDTable);
	}
	
	GFL_ARC_CloseDataHandle(handle);
}

//------------------------------------------------------------------
//BMODEL_IDで引くBMIDToEntryTableにはentryToBMIDTableへのオフセットを格納
//------------------------------------------------------------------
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man)
{
	u8 entryNo;
	for (entryNo = 0; entryNo < man->entryCount; entryNo++)
	{	
		BMODEL_ID bm_id = man->entryToBMIDTable[entryNo];
		man->BMIDToEntryTable[bm_id] = entryNo;
	}
}

//------------------------------------------------------------------
/**
 * BMODEL_ID --> EntryNoへの変換
 */
//------------------------------------------------------------------
static u8 BMIDtoEntryNo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id)
{
	u8 entryNo;
  BMID_ASSERT( bm_id );
	entryNo = man->BMIDToEntryTable[bm_id];
  ENTRYNO_ASSERT( man, entryNo );
	return entryNo;
}

//------------------------------------------------------------------
/**
 * @brief エリアIDから配置モデル情報へのインデックス取得
 * @param area_id 
 * @return  u16 情報アーカイブの指定ID
 *
 * @todo  エリアデータの並びに依存して計算しているので移行を考えること！
 */
//------------------------------------------------------------------
static u16 calcArcIndex(u16 area_id)
{	
	if (area_id >= AREA_ID_IN01)
	{	
		return area_id - AREA_ID_IN01;
	}
	if (area_id >= AREA_ID_OUT01)
	{	
		return (area_id - AREA_ID_OUT01) / 4;
	}
	return 0;	//とりあえず
}


//============================================================================================
//
//
//    配置モデルアニメデータ
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 保持しているアニメIDの数を返す
 * @param data  アニメデータへのポインタ
 * @param u32 保持しているアニメIDの数
 */
//------------------------------------------------------------------
static u32 BMANIME_getCount(const FIELD_BMANIME_DATA * data)
{ 
  u32 anmNo,count;
  for (anmNo = 0, count = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  { 
    if (data->IDs[anmNo] != BMANIME_NULL_ID)
    { 
      count ++;
    }
  }
  return count;
}
//------------------------------------------------------------------
/**
 * @brief FIELD_BMANIME_DATAの初期化
 */
//------------------------------------------------------------------
static void BMANIME_init(FIELD_BMANIME_DATA * data)
{ 
  static const FIELD_BMANIME_DATA init = {  
    BMANIME_TYPE_NONE,  //アニメ適用の種類指定
    BMANIME_PROG_TYPE_NONE,  //動作プログラムの種類指定
    0,  //アニメカウント（仮）
    0,  //セットカウント（仮）
    //アニメアーカイブ指定ID
    { BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, BMANIME_NULL_ID, }

  };
  *data = init;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
#ifdef  PM_DEBUG
static void DEBUG_BMANIME_dump(const FIELD_BMANIME_DATA * data)
{ 
  static const char * animetype[] ={  
    "BMANIME_TYPE_NONE",
    "BMANIME_TYPE_ETERNAL",
    "BMANIME_TYPE_EVENT",
    "BMANIME_TYPE_ERROR",
  };
  int anmNo;
  int type = data->anm_type;
  if (type >= BMANIME_TYPE_MAX) type = BMANIME_TYPE_MAX - 1;
  TAMADA_Printf("FIELD_BMANIME_DATA:");
  TAMADA_Printf("%s, %d\n", animetype[data->anm_type], type);
  TAMADA_Printf("%d %d %d\n",data->prg_type, data->anm_count, data->set_count);
  for (anmNo = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  {
    TAMADA_Printf("%04x ", data->IDs[anmNo]);
  }
  TAMADA_Printf("\n");
}
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMINFO_Load(FIELD_BMODEL_MAN * man, u16 file_id)
{
  typedef struct {
    u16 anime_id;     ///<アニメ指定
    u16 prog_id;      ///<プログラム指定
    u16 submodel_id;  ///<サブモデル指定
    s16 submodel_x;
    s16 submodel_y;
    s16 submodel_z;
  }BM_INFO_BIN;

  enum {
    BM_INFO_SIZE = sizeof(BM_INFO_BIN),
    BM_ANMDATA_SIZE = sizeof(FIELD_BMANIME_DATA),
  };

  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(ARCID_BMODEL_INFO, man->heapID);
  BM_INFO_BIN * infobin = GFL_HEAP_AllocMemory(man->heapID, BM_INFO_SIZE);
  u8 entryNo;

  for (entryNo = 0; entryNo < man->entryCount; entryNo++)
  { 
    BMINFO * bmInfo = &man->bmInfo[entryNo];
    u16 anm_id;
    bmInfo->bm_id = man->entryToBMIDTable[entryNo];
    GFL_ARC_LoadDataOfsByHandle(handle, file_id, bmInfo->bm_id * BM_INFO_SIZE, BM_INFO_SIZE, infobin);
    bmInfo->prog_id = infobin->prog_id;
    bmInfo->sub_bm_id = infobin->submodel_id;
    bmInfo->sx = infobin->submodel_x;
    bmInfo->sy = infobin->submodel_y;
    bmInfo->sz = infobin->submodel_z;
    bmInfo->anm_id = infobin->anime_id;
    if (bmInfo->anm_id == 0xffff) continue;
    GFL_ARC_LoadDataOfsByHandle(handle, FILEID_BMODEL_ANIMEDATA,
        bmInfo->anm_id * BM_ANMDATA_SIZE, BM_ANMDATA_SIZE,
        &bmInfo->animeData);
    //DEBUG_BMANIME_dump(&man->animeData[entryNo]);
  }
  GFL_HEAP_FreeMemory(infobin);
  GFL_ARC_CloseDataHandle(handle);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMINFO_init(BMINFO * bmInfo)
{
  bmInfo->bm_id = 0;
  bmInfo->prog_id = BM_PROG_ID_NONE;
  bmInfo->sub_bm_id = BM_SUBMODEL_NULL_ID;
  bmInfo->sx = 0;
  bmInfo->sy = 0;
  bmInfo->sz = 0;
  bmInfo->anm_id = 0xffff;
  BMANIME_init(&bmInfo->animeData);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo)
{
  return &bmInfo->animeData;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL BMINFO_isDoor(const BMINFO * bmInfo)
{
  switch (bmInfo->prog_id)
  {
  case BM_PROG_ID_DOOR_AUTO:
  case BM_PROG_ID_DOOR_NORMAL:
    return TRUE;
  default:
    return FALSE;
  }
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void TIMEANIME_CTRL_init(TIMEANIME_CTRL * tmanm_ctrl)
{
  tmanm_ctrl->NowTimeZone = TIMEZONE_MAX; //ありえない数
  TIMEANIME_CTRL_update( tmanm_ctrl );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void TIMEANIME_CTRL_update(TIMEANIME_CTRL * tmanm_ctrl)
{
  //5段階のTIMEZONEを4種類のアニメに割り振る
  static const u8 index[TIMEZONE_MAX] = {
    0, 1, 2, 3, 3,
  };
  tmanm_ctrl->OldTimeZone = tmanm_ctrl->NowTimeZone;
  tmanm_ctrl->NowTimeZone = GFL_RTC_GetTimeZone();
  tmanm_ctrl->update_flag = (tmanm_ctrl->NowTimeZone != tmanm_ctrl->OldTimeZone);
  tmanm_ctrl->index = index[tmanm_ctrl->NowTimeZone];
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL TIMEANIME_CTRL_needUpdate(const TIMEANIME_CTRL * tmanm_ctrl)
{
  return tmanm_ctrl->update_flag;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static u8   TIMEANIME_CTRL_getIndex( const TIMEANIME_CTRL * tmanm_ctrl )
{
  return tmanm_ctrl->index;
}

//============================================================================================
//
//
//    配置モデルへの電光掲示板反映
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 電光掲示板用文字列の登録処理（直接文字列オブジェクトを渡す）
 * @param man 配置モデルマネジャーへのポインタ
 * @param id  電光掲示板指定ID
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_EntryELString(const FIELD_BMODEL_MAN* man,
    FIELD_BMODEL_ELBOARD_ID id,
    const STRBUF* str)
{ 
  GF_ASSERT(id < FIELD_BMODEL_ELBOARD_ID_MAX);

  GFL_STR_CopyBuffer(man->elb_str[id], str);
}

//------------------------------------------------------------------
/**
 * @brief 電光掲示板用文字列の登録処理（メッセージアーカイブ指定型）
 * @param man 配置モデルマネジャーへのポインタ
 * @param id  電光掲示板指定ID
 * @param msg_arc_id  メッセージアーカイブ指定ID
 * @param str_id  アーカイブ内メッセージ指定ID
 */
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_EntryELStringID(const FIELD_BMODEL_MAN * man,
    FIELD_BMODEL_ELBOARD_ID id,
    ARCID msg_arc_id, u16 str_id)
{ 
  GFL_MSGDATA * msgData;

  GF_ASSERT(id < FIELD_BMODEL_ELBOARD_ID_MAX);

  msgData = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, msg_arc_id, man->heapID);
  GFL_MSG_GetString(msgData, str_id, man->elb_str[id]);

  GFL_MSG_Delete(msgData);
}
//------------------------------------------------------------------
/**
 * @brief   テクスチャの登録処理
 * @param man 配置モデルマネジャーへのポインタ
 * @param data  アニメデータへのポインタ
 * @param g3DresTex   登録するテクスチャリソース
 *
 * 現在、電光掲示板のためだけに呼び出している。
 * 電光掲示板以外の配置モデルではなにもせずに帰る
 */
//------------------------------------------------------------------
void entryELBoardTex(FIELD_BMODEL_MAN* man, const FIELD_BMANIME_DATA * data,
    const GFL_G3D_RES * g3DresTex)
{ 
  switch ((BMANIME_PROG_TYPE)data->prg_type)
  { 
  case BMANIME_PROG_TYPE_ELBOARD1:
    man->elb_tex[FIELD_BMODEL_ELBOARD_ID1] = ELBOARD_TEX_Add(
        g3DresTex, man->elb_str[FIELD_BMODEL_ELBOARD_ID1], man->heapID);
    break;
  case BMANIME_PROG_TYPE_ELBOARD2:
    man->elb_tex[FIELD_BMODEL_ELBOARD_ID2] = ELBOARD_TEX_Add(
        g3DresTex, man->elb_str[FIELD_BMODEL_ELBOARD_ID1], man->heapID);
    break;
  case BMANIME_PROG_TYPE_NONE:
  default:
    //何もしない
    break;
  }
}


//============================================================================================
//
//
//  リソース生成
//
//
//============================================================================================
//------------------------------------------------------------------
//配置モデルマネジャーからの内容で生成
//------------------------------------------------------------------
static void createAllResource(FIELD_BMODEL_MAN * man)
{ 
  u32 entryCount = man->entryCount;

	if( entryCount == 0 ){
    return;
  }

  man->objRes_Count = entryCount;
  man->objRes = GFL_HEAP_AllocClearMemory ( man->heapID, sizeof(OBJ_RES) * entryCount );

  /** 配置モデルリソース登録 */
  {
    u8 entryNo;

    for ( entryNo = 0; entryNo < man->objRes_Count; entryNo++ )
    {
      OBJ_RES * objRes = &man->objRes[entryNo];
      BMODEL_ID bm_id = man->entryToBMIDTable[entryNo];
      //TAMADA_Printf("BM:Create Rsc %d (%d)\n", entryNo, bm_id);
      OBJRES_initialize( man, objRes, bm_id );
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void deleteAllResource(FIELD_BMODEL_MAN * man)
{
  int i;
	if( man->objRes == NULL ){
    return;
  }
  for( i=0; i<man->objRes_Count; i++ ){
    OBJRES_finalize( &man->objRes[i] );
  }
  GFL_HEAP_FreeMemory( man->objRes );
  man->objRes_Count = 0;
  man->objRes = NULL;
}


//------------------------------------------------------------------
/** 配置モデルG3D_OBJECT登録 */
//------------------------------------------------------------------
static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, GFL_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
  int i;
  u32 entryCount = man->entryCount;

  if( entryCount == 0 ){
    return;
  }

  g3dMapObj->gobjCount = entryCount;
  g3dMapObj->gobj = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(GFL_G3D_MAP_OBJ) * entryCount );

  man->objHdl_Count = entryCount;
  man->objHdl = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(OBJ_HND) * entryCount );

  for ( i = 0; i < man->objHdl_Count; i++ )
  {
    OBJ_HND * objHdl = &man->objHdl[i];
    OBJ_RES * objRes = &man->objRes[i];

    OBJHND_initialize( man, objHdl, objRes );

    g3dMapObj->gobj[i].g3DobjHQ = objHdl->g3Dobj;
    g3dMapObj->gobj[i].g3DobjLQ = NULL;
  }
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, GFL_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
	if( g3dMapObj->gobj != NULL ){
		GFL_HEAP_FreeMemory( g3dMapObj->gobj );
		g3dMapObj->gobj = NULL;
	}
	if( g3dMapObj->gobjIDexchange != NULL ){
		GFL_HEAP_FreeMemory( g3dMapObj->gobjIDexchange );
		g3dMapObj->gobjIDexchange = NULL;
	}

  if( man->objHdl != NULL ){
    int i;
    for( i=0; i<man->objHdl_Count; i++ ){
			OBJHND_finalize( &man->objHdl[i] );
    }
    GFL_HEAP_FreeMemory( man->objHdl );
    man->objHdl_Count = 0;
    man->objHdl = NULL;
  }
}

//============================================================================================
//
//  GFL_G3D 用リソース管理オブジェクト
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_G3D_RES* OBJRES_getResTex(const OBJ_RES * objRes)
{
  if ( objRes->g3DresTex != NULL ) {
    return objRes->g3DresTex;
  } else {
    return objRes->g3DresMdl;
  }
}

//------------------------------------------------------------------
// オブジェクトリソースを作成
//------------------------------------------------------------------
static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, BMODEL_ID bm_id)
{
	GFL_G3D_RES* resTex;

  //配置モデルに対応した情報へのポインタをセット
  objRes->bmInfo = FIELD_BMODEL_MAN_GetBMInfo(man, bm_id);

  //モデルデータ生成
	objRes->g3DresMdl = GFL_G3D_CreateResourceArc( man->mdl_arc_id, bm_id );

  //テクスチャ登録
  //現状、テクスチャはモデリングimdに含まれるものを使用している
  objRes->g3DresTex = NULL;
  resTex = objRes->g3DresMdl;
  if (GFL_G3D_CheckResourceType( resTex, GFL_G3D_RES_CHKTYPE_TEX ) == TRUE)
  {
	  DEBUG_Field_Grayscale( resTex );
	  GFL_G3D_TransVramTexture( resTex );
  } else {
    OS_Printf("配置モデルにテクスチャが含まれていません！(mdlID=%d)\n", bm_id);
    GF_ASSERT(0);
  }

  {
    const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objRes->bmInfo);
    int anmNo;
    int count = BMANIME_getCount(anmData);
    for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
      if ( anmNo < count ) {
        objRes->g3DresAnm[anmNo] = GFL_G3D_CreateResourceArc( ARCID_BMODEL_ANIME, anmData->IDs[anmNo] );
      } else {
        objRes->g3DresAnm[anmNo] = NULL;
      }
    }
  }

  entryELBoardTex( man, BMINFO_getAnimeData(objRes->bmInfo), resTex );

}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void OBJRES_finalize( OBJ_RES * objRes )
{
  int anmNo;
	GFL_G3D_RES*	resTex;

  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      GFL_G3D_DeleteResource( objRes->g3DresAnm[anmNo] );
      objRes->g3DresAnm[anmNo] = NULL;
    }
  }
  if( objRes->g3DresTex == NULL ){
    resTex = objRes->g3DresMdl;
  } else {
    resTex = objRes->g3DresTex;
    GFL_G3D_DeleteResource( objRes->g3DresTex );
    objRes->g3DresTex = NULL;
  }
  GFL_G3D_FreeVramTexture( resTex );

  if( objRes->g3DresMdl != NULL ){
    GFL_G3D_DeleteResource( objRes->g3DresMdl );
    objRes->g3DresMdl = NULL;
  }
}


//============================================================================================
//
//  GFL_G3D_OBJECT管理用オブジェクト
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief BMANIME_TYPE別の処理分岐用テーブル定義
 */
//------------------------------------------------------------------
typedef struct {
  ///アニメ初期化
  void (* init_anime_func)( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
  ///アニメ更新
  void (* do_anime_func)(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
  ///アニメ変更
  void (* set_anime_func)( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req);
}OBJHND_FUNCS;

static void OBJHND_TYPEOTHER_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void OBJHND_TYPEOTHER_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );
static void OBJHND_TYPEEVENT_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );

static void OBJHND_TYPETIMEZONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEEVENT_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEOTHER_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void disableAllAnime(OBJ_HND * objHdl);
//------------------------------------------------------------------
///   各アニメタイプごとの処理関数テーブル
//------------------------------------------------------------------
static const OBJHND_FUNCS objHndFuncTable[BMANIME_TYPE_MAX] = {
  { //BMANIME_TYPE_NONE アニメしない
    OBJHND_TYPEOTHER_initAnime,
    OBJHND_TYPEOTHER_animate,
    OBJHND_TYPEOTHER_setAnime,
  },
  { //BMANIME_TYPE_ETERNAL  ずっとおなじアニメ
    OBJHND_TYPEETERNAL_initAnime,
    OBJHND_TYPEETERNAL_animate,
    OBJHND_TYPEOTHER_setAnime,
  },
  { //BMANIME_TYPE_EVENT  イベントからアニメリクエスト（通常停止）
    OBJHND_TYPEOTHER_initAnime,
    OBJHND_TYPEEVENT_animate,
    OBJHND_TYPEEVENT_setAnime,
  },
  { //BMANIME_TYPE_TIMEZONE 時間帯によるアニメ変更
    OBJHND_TYPETIMEZONE_initAnime,
    OBJHND_TYPETIMEZONE_animate,
    OBJHND_TYPEOTHER_setAnime,
  },
};
//------------------------------------------------------------------
/// OBJHND初期化処理
//------------------------------------------------------------------
static void OBJHND_initialize(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl, const OBJ_RES* objRes)
{
  GFL_G3D_RND* g3Drnd;
  GFL_G3D_RES* resTex;
  GFL_G3D_ANM* anmTbl[GLOBAL_OBJ_ANMCOUNT];
  int anmNo;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objRes->bmInfo);

  resTex = OBJRES_getResTex( objRes );

  //レンダー生成
  g3Drnd = GFL_G3D_RENDER_Create( objRes->g3DresMdl, 0, resTex );

  //アニメオブジェクト生成
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      anmTbl[anmNo] = GFL_G3D_ANIME_Create( g3Drnd, objRes->g3DresAnm[anmNo], 0 );  
    } else {
      anmTbl[anmNo] = NULL;
    }
  }
  //GFL_G3D_OBJECT生成
  objHdl->g3Dobj = GFL_G3D_OBJECT_Create( g3Drnd, anmTbl, GLOBAL_OBJ_ANMCOUNT );
  objHdl->res = objRes;

  //アニメ状態初期化
  {
    BMANIME_TYPE type = anmData->anm_type;
    objHndFuncTable[type].init_anime_func( man, objHdl );
  }
}

//------------------------------------------------------------------
///OBJHND初期化：アニメタイプその他
//------------------------------------------------------------------
static void OBJHND_TYPEOTHER_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
  }
}
//------------------------------------------------------------------
///OBJHND初期化：アニメタイプETERNAL
//------------------------------------------------------------------
static void OBJHND_TYPEETERNAL_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //renderとanimeの関連付け
    GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
    objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
  }
}
//------------------------------------------------------------------
///OBJHND初期化：アニメタイプTIMEZONE
//------------------------------------------------------------------
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo, nowNo;
  nowNo = TIMEANIME_CTRL_getIndex( &man->tmanm_ctrl );
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if (anmNo == nowNo)
    {
      objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
    }
    else
    {
      GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //renderとanimeの関連付け
      GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
      objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
    }
  }
}

//------------------------------------------------------------------
/// OBJHND終了処理
//------------------------------------------------------------------
static void OBJHND_finalize( OBJ_HND * objHdl )
{
  GFL_G3D_RND*  g3Drnd;

  if( objHdl->g3Dobj != NULL ){
    int anmNo;
    //各種ハンドル取得
    u16 g3DanmCount = GFL_G3D_OBJECT_GetAnimeCount( objHdl->g3Dobj );
    for( anmNo=0; anmNo<g3DanmCount; anmNo++ ){
      GFL_G3D_ANM*  g3Danm = GFL_G3D_OBJECT_GetG3Danm( objHdl->g3Dobj, anmNo ); 
      if (g3Danm != NULL) {
        GFL_G3D_ANIME_Delete( g3Danm ); 
      }
    }
    g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( objHdl->g3Dobj );
  
    //各種ハンドル＆リソース削除
    GFL_G3D_OBJECT_Delete( objHdl->g3Dobj );
    objHdl->g3Dobj = NULL;
    objHdl->res = NULL;

    GFL_G3D_RENDER_Delete( g3Drnd );
  }
}

//------------------------------------------------------------------
/**
 * @brief OBJHNDアニメ更新処理
 */
//------------------------------------------------------------------
static void OBJHND_animate(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  BMANIME_TYPE type = anmData->anm_type;

  if( objHdl->g3Dobj == NULL ){
    return;
  }

  objHndFuncTable[type].do_anime_func( man, objHdl );
}

//------------------------------------------------------------------
/// OBJHNDアニメ更新処理：その他
//------------------------------------------------------------------
static void OBJHND_TYPEOTHER_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  /* DO NOTHING !! */
}

//------------------------------------------------------------------
/// OBJHNDアニメ更新処理：TIMEZONE
//------------------------------------------------------------------
static void OBJHND_TYPETIMEZONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  /* DO NOTHING !! */
  int anmNo = TIMEANIME_CTRL_getIndex( &man->tmanm_ctrl );
  BOOL change_flag = TIMEANIME_CTRL_needUpdate( &man->tmanm_ctrl );
  if (change_flag)
  {
    disableAllAnime( objHdl );
    GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //renderとanimeの関連付け
    GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
    objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
  }
  else
  {
    GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
  }
}

//------------------------------------------------------------------
/// OBJHNDアニメ更新処理：ETERNAL
//------------------------------------------------------------------
static void OBJHND_TYPEETERNAL_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
  }
}
//------------------------------------------------------------------
/// OBJHNDアニメ更新処理：EVENT
//------------------------------------------------------------------
static void OBJHND_TYPEEVENT_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo;

  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    BOOL result;
    switch ((BM_ANMMODE)objHdl->anmMode[anmNo])
    {
    case BM_ANMMODE_NOTHING:
    case BM_ANMMODE_STOP:
      break;
    case BM_ANMMODE_TEMPORARY:
      result = GFL_G3D_OBJECT_IncAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE );
      if (!result) {
        objHdl->anmMode[anmNo] = BM_ANMMODE_STOP;
      }
      break;
    case BM_ANMMODE_LOOP:
      GFL_G3D_OBJECT_LoopAnimeFrame( objHdl->g3Dobj, anmNo, FX32_ONE ); 
      break;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief アニメ全停止
 */
//------------------------------------------------------------------
static void disableAllAnime(OBJ_HND * objHdl)
{
  int anmNo;
  for (anmNo = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  {
    if (objHdl->anmMode[anmNo] != BM_ANMMODE_NOTHING) {
      GFL_G3D_OBJECT_DisableAnime( objHdl->g3Dobj, anmNo );
      objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
    }
  }
}
//------------------------------------------------------------------
/**
 * @brief アニメ変更処理
 */
//------------------------------------------------------------------
static void OBJHND_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req)
{
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  BMANIME_TYPE type = anmData->anm_type;
  ANIMENO_ASSERT(anmNo);

  objHndFuncTable[type].set_anime_func( objHdl, anmNo, req );
}

//------------------------------------------------------------------
/// OBJHNDアニメ変更処理：EVENT
//------------------------------------------------------------------
static void OBJHND_TYPEEVENT_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req )
{
  switch ((BMANM_REQUEST)req) {
  case BMANM_REQ_START:
    disableAllAnime( objHdl );
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, anmNo );
    GFL_G3D_OBJECT_ResetAnimeFrame(objHdl->g3Dobj, anmNo );
    objHdl->anmMode[anmNo] = BM_ANMMODE_TEMPORARY;
    break;
  case BMANM_REQ_STOP:
    if (objHdl->anmMode[anmNo] != BM_ANMMODE_NOTHING) {
      objHdl->anmMode[anmNo] = BM_ANMMODE_STOP;
    }
    break;
  case BMANM_REQ_END:
    disableAllAnime( objHdl );
    break;
  default:
    GF_ASSERT(0); //未定義リクエスト
    break;
  }
}
//------------------------------------------------------------------
/// OBJHNDアニメ変更処理：その他
//------------------------------------------------------------------
static void OBJHND_TYPEOTHER_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req )
{
  /* DO NOTHING !! */
}

//------------------------------------------------------------------
/**
 * @brief アニメ状態取得
 * @return  BOOL  TRUE=停止状態　FALSE=動作中
 */
//------------------------------------------------------------------
static BOOL OBJHND_getAnimeStatus(OBJ_HND * objHdl, u32 anmNo)
{
  ANIMENO_ASSERT(anmNo);

  switch ((BM_ANMMODE)objHdl->anmMode[anmNo]) {
  case BM_ANMMODE_NOTHING:
    return TRUE;
  case BM_ANMMODE_LOOP:
    return FALSE;
  case BM_ANMMODE_STOP:
    return TRUE;
  case BM_ANMMODE_TEMPORARY:
    return FALSE;
  }
  return FALSE;
}


//============================================================================================
//
//  GFL_G3D_MAPの保持するGFL_G3D_MAP_GLOBALOBJ_STを参照、コントロールするための
//  ラッパーオブジェクト
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline BOOL G3DMAPOBJST_exists(const G3DMAPOBJST * obj)
{
  return (obj->g3Dmap != NULL);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_init(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  obj->g3Dmap = NULL;
  obj->entryNoBackup = GFL_G3D_MAP_OBJID_NULL;
  obj->index = 0;
  obj->viewFlag = FALSE;
  obj->objSt = NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST * status, int idx)
{
  int i;
  for (i = 0; i < NELEMS(man->g3DmapObjSt); i++)
  {
    G3DMAPOBJST * obj = &man->g3DmapObjSt[i];
    if ( G3DMAPOBJST_exists(obj) == FALSE )
    {
      GFL_G3D_MAP_ResistGlobalObj( g3Dmap, status, idx );
      obj->g3Dmap = g3Dmap;
      obj->entryNoBackup = GFL_G3D_MAP_OBJID_NULL;
      obj->index = idx;
      obj->viewFlag = TRUE;
      obj->objSt = GFL_G3D_MAP_GetGlobalObj( g3Dmap, idx );

      return obj;
    }
  }
  GF_ASSERT(0);
  return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap)
{
  int i, max = NELEMS(man->g3DmapObjSt);
  for (i = 0; i < max; i++)
  {
    G3DMAPOBJST * obj = &man->g3DmapObjSt[i];
    if (obj->g3Dmap == g3Dmap)
    {
      G3DMAPOBJST_deleteByObject(man, obj);
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_deleteByObject(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  GF_ASSERT( obj->objSt == GFL_G3D_MAP_GetGlobalObj(obj->g3Dmap, obj->index) );
  GFL_G3D_MAP_ReleaseGlobalObj( obj->g3Dmap, obj->index );
  G3DMAPOBJST_init( man, obj );
}

//============================================================================================
//
//
//  外部公開関数：
//    G3DMAPOBJST経由での操作関数群
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
G3DMAPOBJST ** FIELD_BMODEL_MAN_CreateObjStatusList
( FIELD_BMODEL_MAN* man, const FLDHIT_RECT * rect, u32 * num )
{
  G3DMAPOBJST ** result;
  int objNo, count = 0;
  result = GFL_HEAP_AllocClearMemory(man->heapID, sizeof(G3DMAPOBJST*) * GFL_G3D_MAP_OBJST_MAX);

  for (objNo = 0; objNo < NELEMS(man->g3DmapObjSt); objNo ++)
  {
    VecFx32 pos;
    G3DMAPOBJST * obj = &man->g3DmapObjSt[objNo];
    if ( G3DMAPOBJST_exists(obj) == FALSE ) continue;
    GFL_G3D_MAP_GetTrans( obj->g3Dmap, &pos );
    VEC_Add( &obj->objSt->trans, &pos, &pos );
    if ( FLDHIT_RECT_IsIncludePos( rect, pos.x, pos.z ) == TRUE )
    {
      result[count] = obj;
      count ++;
      TAMADA_Printf("BLOCK:%x IDX:%02d POS(%d, %d)\n",
          obj->g3Dmap, obj->index, FX_Whole(pos.x), FX_Whole(pos.z) );
      TAMADA_Printf("Search Hit.\n");
    }
  }
  *num = count;
  return result;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_MAN_G3DMAPOBJSTisDoor(const FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj)
{
  u8 entryNo;
  GF_ASSERT( G3DMAPOBJST_exists(obj) );
  entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  return BMINFO_isDoor(&man->bmInfo[entryNo]);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void G3DMAPOBJST_changeViewFlag(G3DMAPOBJST * obj, BOOL flag)
{
  GF_ASSERT(obj->viewFlag == !flag);
  if (flag)
  {
    obj->objSt->id = obj->entryNoBackup;
    obj->entryNoBackup = GFL_G3D_MAP_OBJID_NULL;
    obj->viewFlag = TRUE;
  }
  else
  {
    obj->entryNoBackup = obj->objSt->id;
    obj->objSt->id = GFL_G3D_MAP_OBJID_NULL;
    obj->viewFlag = FALSE;
  }
}

//============================================================================================
//
//
//  外部公開関数：
//  配置モデルの生成
//    マップ情報からでなく、意識的にイベントなどで生成する場合の関数群
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create(FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj)
{
  const OBJ_RES * objRes;
  const GFL_G3D_MAP_GLOBALOBJ_ST * status = obj->objSt;
  VecFx32 drawOffset;

  FIELD_BMODEL * bmodel = GFL_HEAP_AllocMemory( man->heapID, sizeof(FIELD_BMODEL) );
  //ローテーション設定
  {
    fx32 sin = FX_SinIdx(status->rotate);
    fx32 cos = FX_CosIdx(status->rotate);
    MTX_RotY33( &bmodel->g3dObjStatus.rotate, sin, cos );
  }
  //位置設定：g3Dmapに依存せず表示するため、ここで座標加算しておく
  GFL_G3D_MAP_GetTrans( obj->g3Dmap, &bmodel->g3dObjStatus.trans );
  VEC_Add( &bmodel->g3dObjStatus.trans, &status->trans, &bmodel->g3dObjStatus.trans );
  FLDMAPPER_GetDrawOffset( man->fldmapper, &drawOffset );
  VEC_Add( &bmodel->g3dObjStatus.trans, &drawOffset, &bmodel->g3dObjStatus.trans );
  
  //スケール設定
  VEC_Set( &bmodel->g3dObjStatus.scale, FX32_ONE, FX32_ONE, FX32_ONE );

  GF_ASSERT( status->id < man->objRes_Count );
  objRes = &man->objRes[status->id];
  OBJHND_initialize( man, &bmodel->objHdl, objRes );
  return bmodel;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_Delete(FIELD_BMODEL * bmodel)
{
  OBJHND_finalize( &bmodel->objHdl );
  GFL_HEAP_FreeMemory(bmodel);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_SetAnime(FIELD_BMODEL * bmodel, u32 idx, BMANM_REQUEST req)
{
  OBJHND_setAnime(&bmodel->objHdl, idx, req);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetAnimeStatus(FIELD_BMODEL * bmodel, u32 idx)
{
  return OBJHND_getAnimeStatus( &bmodel->objHdl, idx );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BM_PROG_ID FIELD_BMODEL_GetProgID(const FIELD_BMODEL * bmodel)
{
  return bmodel->objHdl.res->bmInfo->prog_id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel )
{
  GFL_G3D_DRAW_DrawObject( bmodel->objHdl.g3Dobj, &bmodel->g3dObjStatus );
}

