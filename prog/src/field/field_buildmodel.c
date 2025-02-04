//============================================================================================
/**
 * @file	field_buildmodel.c
 * @brief		配置モデルの制御
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.20
 *
 * @note
 * メモリ削減：アニメリソースの重複を減らすことでもう少し可能。
 * エリアIDと配置モデルリストの関係が、エリアデータの並びに依存した
 * 計算になっているのでデータ追加・削除の際には注意すること。
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

#include "savedata/save_control.h"    //テストでランダムマップの都市の種類を取得
#include "savedata/randommap_save.h"  //テストでランダムマップの都市の種類を取得

#include "arc/fieldmap/area_id.h"
#include "arc/fieldmap/buildmodel_outdoor.naix"
#include "arc/fieldmap/buildmodel_indoor.naix"

#include "map/dp3format.h"

#include "sound/pm_sndsys.h"  //PMSND_
#include "sound/wb_sound_data.sadl" //SE指定

#include "field/field_comm/intrude_work.h"



//============================================================================================
//  DEBUG リソースメモリ使用量の検査
//============================================================================================
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

static u32 BMODEL_DEBUG_RESOURCE_MemorySize = 0;  // リソース

// リソースメモリーサイズ追加
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( x )    BMODEL_DEBUG_RESOURCE_MemorySize += GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// リソースメモリーサイズ削除
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( x )    BMODEL_DEBUG_RESOURCE_MemorySize -= GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// リソースメモリーサイズ アロケータ用 追加
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( x )    BMODEL_DEBUG_RESOURCE_MemorySize += NNS_FndGetSizeForMBlockExpHeap((x))
// リソースメモリーサイズ  削除
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( x )    BMODEL_DEBUG_RESOURCE_MemorySize -= NNS_FndGetSizeForMBlockExpHeap((x))
// リソースメモリーサイズ クリーンチェック
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete    GF_ASSERT( BMODEL_DEBUG_RESOURCE_MemorySize == 0 )


#else   // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE


// リソースメモリーサイズ追加
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( x )   /**/ 
// リソースメモリーサイズ削除
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( x )    /**/
// リソースメモリーサイズ アロケータ用 追加
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( x )   /**/ 
// リソースメモリーサイズ  削除
#define BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( x )   /**/ 
// リソースメモリーサイズ クリーンチェック
#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete   /**/ 

#endif  // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

//============================================================================================
//============================================================================================

enum {
  FLD_G3D_MAP_OBJST_MAX = 32,
  FLD_G3D_MAP_OBJID_NULL = 0xffffffff,

  MAPBLOCK_MAX = 9,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum { GLOBAL_OBJ_ANMCOUNT	= 4 };

//------------------------------------------------------------------
//  外部制御可能な配置モデルの登録上限
//------------------------------------------------------------------
enum { BMODEL_USE_MAX = 7 };

//------------------------------------------------------------------
//  配置モデル登録キー用の定義
//------------------------------------------------------------------
enum {
  BMODEL_UNIQ_VALUE = 0xA5A0,
  BMODEL_UNIQ_MASK  = 0xfff0,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  BMODEL_ID_MAX = 400,      ///<配置モデルの種類上限
  BMODEL_ENTRY_MAX = 128,   ///<配置モデルの登録上限（メモリの方が先に制約条件となるはず）

  BMODEL_ID_NG  = BMODEL_ID_MAX,
  BMODEL_ENTRY_NG = BMODEL_ENTRY_MAX,

  BMANIME_NULL_ID  = 0xffff,
  BMANIME_NULL_OFS  = 0xffffffff,
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
  BM_ANMMODE_REVERSE,     ///<アニメ状態：逆再生で適用中
  BM_ANMMODE_TIMEZONE,    ///<アニメ状態：時間帯で制御
}BM_ANMMODE;


//------------------------------------------------------------------
/// 配置モデル　エリア情報パッケージヘッダ
//  
//  header の下に、datanum+1 分のオフセット値 （u32）
//
//  datanum / 2 がファイル総数  
//  前半  BMINFO
//  交換　NSBMD
//------------------------------------------------------------------
#define BM_AREA_HEADER_DATAID 'BA'  //"AB"の逆 
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u8  datanum;
    u8  dummy1;
    u32 ofs[]; ///<datanum + 1分
} BM_AREA_HEADER;

//------------------------------------------------------------------
/// 配置モデルアニメ情報
//------------------------------------------------------------------
typedef struct _FIELD_BMANIME_DATA
{ 
  u8 anm_type;  ///<BMANIME_TYPE  アニメの種類指定
  u8 prg_type;  ///<BMANIME_PROG_TYPE 動作プログラムの種類指定
  u8 anmset_num; ///<アニメセット数
  u8 ptn_count; ///<パターン数

  u32 ofs[GLOBAL_OBJ_ANMCOUNT]; ///<アニメbinaryデータのオフセット
}FIELD_BMANIME_DATA;

//------------------------------------------------------------------
/// 配置モデル情報
//------------------------------------------------------------------
typedef struct {
  u16 bm_id;              ///<配置モデルID  (BMODEL_ID)
  u16 prog_id;                  ///<BM_PROG_ID プログラム指定ID
  u16 sub_bm_id;          ///<従属モデル指定ID (BMODEL_ID)
  s16 sx, sy, sz;               ///<従属モデルの相対位置
  u16 anm_id;                   ///<アニメ指定ID
  u16 pad;                ///<padding
  FIELD_BMANIME_DATA animeData; ///<アニメ指定データ

  // メモリでは、この下に、アニメデータが続いています。
}BMINFO;

//------------------------------------------------------------------
/// 配置モデルオブジェクトリソース
//------------------------------------------------------------------
typedef struct {
  const BMINFO * bmInfo;              ///<配置モデル情報
  GFL_G3D_RES*  g3DresMdl;            ///<モデルリソース
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
/**
 * FLD_G3D_MAPの保持するFLD_G3D_MAP_GLOBALOBJ_STを
 * 参照、コントロールするためのラッパーオブジェクト
 */
//------------------------------------------------------------------
struct _G3DMAPOBJST{
  FLD_G3D_MAP * g3Dmap;   ///<所属FLD_G3D_MAPへの参照
  u32 entryNoBackup;      ///<モデル指定IDのバックアップ
  u16 index;              ///<所属g3Dmap内でのインデックス
  u16 viewFlag;           ///<可視設定フラグ
  FLD_G3D_MAP_GLOBALOBJ_ST * objSt; ///<実オブジェクトへのポインタ
};

//------------------------------------------------------------------
/// 配置モデル制御ワーク（外部からの制御用）
//------------------------------------------------------------------
struct _FIELD_BMODEL {
  FIELD_BMODEL_MAN * man; ///<マネジャーへのポインタ
  u32 currentAnmIdx;    ///<アニメNo保持（最新のもの）
  G3DMAPOBJST * objst;  ///<元となったOBJSTへのポインタ
  OBJ_HND objHdl;         ///<モデル制御オブジェクト
  GFL_G3D_OBJSTATUS g3dObjStatus; ///<表示制御オブジェクト
};

//------------------------------------------------------------------
///時間帯アニメ制御ワーク
//------------------------------------------------------------------
typedef struct {
  TIMEZONE NowTimeZone;   ///<現在の時間帯
  TIMEZONE OldTimeZone;   ///<直前の時間帯
  BOOL update_flag;       ///<更新が必要かどうか
  u8 index;               ///<時間帯から算出されたアニメ指定
  u8 season;              ///<季節
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

  ///エリア配置モデル情報
  BM_AREA_HEADER* bm_areadata;

  ///配置モデルID→登録IDの変換テーブル
	u8 BMIDToEntryTable[BMODEL_ID_MAX];

  ///配置モデル登録数
	u16 entryCount;

	FLD_G3D_MAP_GLOBALOBJ	g3dMapObj;						///<共通オブジェクト

	u32	objRes_Count;		  		///<共通オブジェクトリソース数
	OBJ_RES * objRes;					///<共通オブジェクトリソース

	GFL_G3D_RES*	g3DresTex;  ///<テクスチャセット

  u32 objHdl_Count;
  OBJ_HND * objHdl;

  FIELD_BMODEL * bmodels[BMODEL_USE_MAX];

  G3DMAPOBJST g3DmapObjSt[FLD_G3D_MAP_OBJST_MAX * MAPBLOCK_MAX];
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
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man, BMODEL_ID default_id );
static u16 calcArcIndex(u16 area_id);

static void loadAreaBMData( FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id );
static void releaseAreaBMData( FIELD_BMODEL_MAN * man );
static u8 BMIDtoEntryNo(const FIELD_BMODEL_MAN * man, BMODEL_ID id);

static const BMINFO* BM_AREA_HEADER_getBMINFOBuff( const BM_AREA_HEADER* header, u32 index );
static const void* BM_AREA_HEADER_getNSBMDData( const BM_AREA_HEADER* header, u32 count );

static const void* BM_ANIME_DATA_GetAnimeData( const FIELD_BMANIME_DATA* bmanime, u32 idx );

static void loadBMTextureSet(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id, u8* gray_scale);
static void freeBMTextureSet(FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * BMODELMAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id);
static const BMINFO * BMODELMAN_GetBMInfoEntry(const FIELD_BMODEL_MAN * man, u32 entryNo);

//アニメデータの取得処理
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo);

static u32 BMANIME_getCount(const FIELD_BMANIME_DATA * data);
static void DEBUG_BMANIME_dump(const FIELD_BMANIME_DATA * data);

//------------------------------------------------------------------
//------------------------------------------------------------------

static void createAllResource(FIELD_BMODEL_MAN * man, u8 *gray_scale);
static void deleteAllResource(FIELD_BMODEL_MAN * man);

static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj);
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj);

static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, u16 entryNo, u8 *gray_scale);
static void OBJRES_finalize( OBJ_RES * objRes );
static void OBJHND_initialize(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl, const OBJ_RES* objRes);
static void OBJHND_finalize( OBJ_HND * objHdl );
static void OBJHND_animate(const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req);
static BOOL OBJHND_getAnimeStatus( const OBJ_HND * objHdl, u32 anmNo);
static fx32 OBJHND_getAnimeFrame( const OBJ_HND * objHdl, u32 anmNo);

static void TIMEANIME_CTRL_init( TIMEANIME_CTRL * tmanm_ctrl, u8 season );
static void TIMEANIME_CTRL_update( TIMEANIME_CTRL * tmanm_ctrl );
static BOOL TIMEANIME_CTRL_needUpdate( const TIMEANIME_CTRL * tmanm_ctrl );
static u8   TIMEANIME_CTRL_getIndex( const TIMEANIME_CTRL * tmanm_ctrl );

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST * status, int idx);
static void G3DMAPOBJST_init(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByObject(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj);
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap);

static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel );

static void BMODELMAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);
static void BMODELMAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);

static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos);
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
FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID, FLDMAPPER * fldmapper, u16 season)
{	
  int i;
	FIELD_BMODEL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_BMODEL_MAN));
	man->heapID = heapID;
  man->fldmapper = fldmapper;
  TIMEANIME_CTRL_init( &man->tmanm_ctrl, season );

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
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      FIELD_BMODEL_Delete(man->bmodels[i]);
    }
  }
  deleteFullTimeObjHandle( man, &man->g3dMapObj );
  deleteAllResource( man );

  // BMAreaデータ破棄
  releaseAreaBMData( man );

  freeBMTextureSet( man );
	GFL_HEAP_FreeMemory(man);

  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_IsAllDelete;
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

  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i])
    {
      OBJ_HND * objHdl = &man->bmodels[i]->objHdl;
      OBJHND_animate( man, objHdl );
    }
  }

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
void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid, const AREADATA * areadata, u8* gray_scale)
{	
	u16 area_id = ZONEDATA_GetAreaID(zoneid);
	u16 bmlist_index = calcArcIndex(area_id);
  ARCID bmtex_arc_id;
  ARCID mdl_arc_id;
  BMODEL_ID default_id;
  int i;

  if (AREADATA_GetInnerOuterSwitch(areadata) != 0) 
  {	
    mdl_arc_id = ARCID_BMODEL_OUTDOOR_AREA;
    bmtex_arc_id = ARCID_BMODEL_TEXSET_OUTDOOR;
    default_id = NARC_output_buildmodel_outdoor_rock_01_nsbmd;
  }
  else
  {	
    mdl_arc_id = ARCID_BMODEL_INDOOR_AREA;
    bmtex_arc_id = ARCID_BMODEL_TEXSET_INDOOR;
    default_id = NARC_output_buildmodel_indoor_dust01_nsbmd;
  }

  // エリア別配置モデルデータの読み込み
  loadAreaBMData( man, mdl_arc_id, bmlist_index );

  //エリア別配置モデルテクスチャセットの読み込み
  loadBMTextureSet(man, bmtex_arc_id, bmlist_index, gray_scale );

	//ID→登録順序の変換データテーブル生成
	makeBMIDToEntryTable( man, default_id );

  createAllResource( man, gray_scale );
  createFullTimeObjHandle( man, &man->g3dMapObj );
}

//============================================================================================
//
//    外部公開関数：
//
//============================================================================================
static u8 BMODELMAN_GetEntryIndex(const FIELD_BMODEL_MAN* man, BMODEL_ID id);
static BOOL BMODELMAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo);

//------------------------------------------------------------------
/**
 * @brief 配置モデルIDを登録済み配置モデルのインデックスに変換する
 * @param man 配置モデルマネジャーへのポインタ
 */
//------------------------------------------------------------------
static u8 BMODELMAN_GetEntryIndex(const FIELD_BMODEL_MAN * man, BMODEL_ID id)
{	
  return BMIDtoEntryNo( man, id );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const BMINFO * BMODELMAN_GetBMInfo(const FIELD_BMODEL_MAN * man, BMODEL_ID bm_id)
{
  u16 entryNo = BMIDtoEntryNo( man, bm_id );

  return BM_AREA_HEADER_getBMINFOBuff( man->bm_areadata, entryNo );
}

static const BMINFO * BMODELMAN_GetBMInfoEntry(const FIELD_BMODEL_MAN * man, u32 entryNo)
{
  return BM_AREA_HEADER_getBMINFOBuff( man->bm_areadata, entryNo );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL BMODELMAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo)
{
  u16 submodel_id;
  const BMINFO * bmInfo;
  bmInfo = BMODELMAN_GetBMInfo( man, bm_id );
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
static inline u16 getResourceID( const PositionSt * objStatus )
{
  return objStatus->resourceID + ( objStatus->resourceIDupper << 8 );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
int FIELD_BMODEL_MAN_ResistAllMapObjects
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  FLD_G3D_MAP_GLOBALOBJ_ST status;
  int dataCount, resistCount, count;


  for( dataCount=0, resistCount = 0, count = objCount; dataCount<count ; resistCount++, dataCount++ )
  {
    if (resistCount >= FLD_G3D_MAP_OBJST_MAX)
    {
      OS_Printf("マップブロック内の配置モデル数が%dを超えているため表示できません\n",
          FLD_G3D_MAP_OBJST_MAX);
      GF_ASSERT(0);
    }

    FIELD_BMODEL_MAN_ResistMapObject(man, g3Dmap, &objStatus[dataCount], resistCount);

    if (TRUE == BMODELMAN_GetSubModel(man,
          getResourceID( &objStatus[dataCount] ), &status.trans, &status.id) )
    {
      //TAMADA_Printf("Resist Sub Model:index(%d) model id(%d)\n", dataCount, status.id);
      resistCount++;
      status.rotate = objStatus[dataCount].rotate;
      status.trans.x += objStatus[dataCount].xpos;
      status.trans.y += objStatus[dataCount].ypos;
      status.trans.z -= objStatus[dataCount].zpos;
      G3DMAPOBJST_create(man, g3Dmap, &status, resistCount);
    }

  }

  // 実際の登録数を返す
  return resistCount;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ReleaseAllMapObjects
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap)
{
  G3DMAPOBJST_deleteByG3Dmap( man, g3Dmap );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_MAN_ResistMapObject
(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount)
{
  FLD_G3D_MAP_GLOBALOBJ_ST status;
  status.id = BMIDtoEntryNo( man, getResourceID( objStatus ) );
  VEC_Set(&status.trans, objStatus->xpos, objStatus->ypos, -objStatus->zpos);
  status.rotate = (u16)(objStatus->rotate);
  G3DMAPOBJST_create(man, g3Dmap, &status, objCount);
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void BMODELMAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
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
static void BMODELMAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel)
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
FIELD_BMODEL * FIELD_BMODEL_MAN_GetBModelByUniqKey( FIELD_BMODEL_MAN * man, u16 uniq_key )
{
  u16 masked =(uniq_key & BMODEL_UNIQ_MASK );
  u16 raw_key = ( uniq_key & ~BMODEL_UNIQ_MASK );
  if ( masked != BMODEL_UNIQ_MASK ) return NULL;
  if (raw_key >= BMODEL_USE_MAX ) return NULL;
  return man->bmodels[ raw_key ];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 FIELD_BMODEL_MAN_GetUniqKey( FIELD_BMODEL_MAN * man, const FIELD_BMODEL * bmodel )
{
  int i;
  if ( bmodel == NULL ) return 0;
  for (i = 0; i < BMODEL_USE_MAX; i++)
  {
    if (man->bmodels[i] == bmodel)
    {
      return ( i | BMODEL_UNIQ_MASK );
    }
  }
  GF_ASSERT(0);
  return 0;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
FLD_G3D_MAP_GLOBALOBJ * FIELD_BMODEL_MAN_GetGlobalObjects(FIELD_BMODEL_MAN * man)
{
  return &man->g3dMapObj;
}




//============================================================================================
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  エリア別配置モデルデータの読み込み
 *
 * @param man
 * @param arc_id  エリア別配置モデルリストのアーカイブ指定ID（屋内or屋外）
 * @param file_id  エリアIDから変換したファイル指定ID
 */
//-----------------------------------------------------------------------------
static void loadAreaBMData( FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id )
{
  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(arc_id, GFL_HEAP_LOWID(man->heapID) );

  {
    u16 data_max = GFL_ARC_GetDataFileCntByHandle(handle);
    if (data_max <= file_id)
    {	
      GF_ASSERT_MSG(0, "配置モデルリストデータがありません(%d<%d)\n", data_max, file_id);
      file_id = 0;		//ハングアップ回避
    }
  }

  // 読み込み
	{	
    man->bm_areadata = GFL_ARC_LoadDataAllocByHandle( handle, file_id, man->heapID );
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->bm_areadata );

    GF_ASSERT( man->bm_areadata->DataID == BM_AREA_HEADER_DATAID );

    man->entryCount = man->bm_areadata->datanum / 2;
    
		if(man->entryCount > BMODEL_ID_MAX)
		{	
			GF_ASSERT_MSG(0, "配置モデルリストデータがオーバー（size=%d ARCINDEX=%d)\n", man->entryCount, file_id);
			man->entryCount = BMODEL_ENTRY_MAX;	//ハングアップ回避
		}
	}
	
	GFL_ARC_CloseDataHandle(handle);


}

//----------------------------------------------------------------------------
/**
 *	@brief  エリア配置モデル情報release
 */
//-----------------------------------------------------------------------------
static void releaseAreaBMData( FIELD_BMODEL_MAN * man )
{
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->bm_areadata );
  GFL_HEAP_FreeMemory( man->bm_areadata );
  man->entryCount = 0;
}

//------------------------------------------------------------------
//BMODEL_IDで引くBMIDToEntryTableにはentryToBMIDTableへのオフセットを格納
//------------------------------------------------------------------
static void makeBMIDToEntryTable(FIELD_BMODEL_MAN * man, BMODEL_ID default_id )
{
	u8 entryNo;
  u8 clearCode;
  int i;

	GFL_STD_MemFill(man->BMIDToEntryTable, BMODEL_ENTRY_MAX, sizeof(man->BMIDToEntryTable));

	for (entryNo = 0; entryNo < man->entryCount; entryNo++)
	{	
    const BMINFO* bmInfo = BMODELMAN_GetBMInfoEntry( man, entryNo );
		BMODEL_ID bm_id = bmInfo->bm_id;
		man->BMIDToEntryTable[bm_id] = entryNo;
	}

  //Outdoor/Indoorそれぞれのデフォルトのエントリーナンバーを取り出す
  clearCode = man->BMIDToEntryTable[ default_id ];
  for ( i = 0; i < BMODEL_ID_MAX; i ++ )
  {
    //初期値のままの箇所にはデフォルトに対応したエントリナンバーを書き込む
    if ( man->BMIDToEntryTable[i] == BMODEL_ENTRY_MAX )
    {
      man->BMIDToEntryTable[i] = clearCode;
    }
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


//----------------------------------------------------------------------------
/**
 *	@brief  配置モデルエリアデータからBMINFOを取得
 *
 *	@param	header  ヘッダー
 *  
 *	@return BMINFO
 */
//-----------------------------------------------------------------------------
static const BMINFO* BM_AREA_HEADER_getBMINFOBuff( const BM_AREA_HEADER* header, u32 index )
{
  GF_ASSERT( index < (header->datanum/2) );
  return (const BMINFO*)( (u32)(header) + header->ofs[index] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  配置モデルエリアデータからNSBMDデータを取得
 *
 *	@param	header  ヘッダー
 *	@param	count   カウント
 *
 *	@return NSBMDデータ
 */
//-----------------------------------------------------------------------------
static const void* BM_AREA_HEADER_getNSBMDData( const BM_AREA_HEADER* header, u32 count )
{
  GF_ASSERT( count < (header->datanum/2) );
  return (const void*)( (u32)(header) + header->ofs[(header->datanum/2) + count] );
}


//----------------------------------------------------------------------------
/**
 *	@brief  配置アニメデータ  インデックスのリソースを取得
 *
 *	@param	bmanime   アニメデータ
 *	@param	idx       アニメインデックス
 *
 *	@return nsbca nsbva nsbtp nsbta nsbma
 */
//-----------------------------------------------------------------------------
static const void* BM_ANIME_DATA_GetAnimeData( const FIELD_BMANIME_DATA* bmanime, u32 idx )
{
  GF_ASSERT( idx < GLOBAL_OBJ_ANMCOUNT );
  GF_ASSERT( bmanime->ofs[ idx ] != BMANIME_NULL_OFS );

  return (const void*)( (u32)(bmanime) + bmanime->ofs[idx] );
}





//------------------------------------------------------------------
/**
 * @brief エリアIDから配置モデル情報へのインデックス取得
 * @param area_id 
 * @return  u16 情報アーカイブの指定ID
 *
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
  GF_ASSERT( 0 );
	return 0;	//エラー対処
}

//------------------------------------------------------------------
/**
 * @brief エリア別配置モデルIDテクスチャセットの読み込み
 * @param man
 * @param arc_id  エリア別配置モデルリストのアーカイブ指定ID（屋内or屋外）
 * @param file_id  エリアIDから変換したファイル指定ID
 */
//------------------------------------------------------------------
static void loadBMTextureSet(FIELD_BMODEL_MAN * man, u16 arc_id, u16 file_id, u8* gray_scale)
{
  man->g3DresTex = GFL_G3D_CreateResourceArc( arc_id, file_id );
  
  if( gray_scale != NULL ){
    FLDMAPPER_Field_Grayscale( man->g3DresTex, gray_scale );
  }
  if(	!GFL_G3D_TransVramTextureAndFreeImageEntity( man->g3DresTex )){
    GF_ASSERT(0);
    return;
  }
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->g3DresTex );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( GFL_G3D_GetResourceFileHeader( man->g3DresTex ) );
}

//------------------------------------------------------------------
/**
 * @brief エリア別配置モデルIDテクスチャセットの破棄
 * @param man
 * @param arc_id  エリア別配置モデルリストのアーカイブ指定ID（屋内or屋外）
 * @param file_id  エリアIDから変換したファイル指定ID
 */
//------------------------------------------------------------------
static void freeBMTextureSet(FIELD_BMODEL_MAN * man)
{
  if(man->g3DresTex != NULL){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( man->g3DresTex ) );
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->g3DresTex );
    GFL_G3D_FreeVramTexture( man->g3DresTex );
    GFL_G3D_DeleteResource( man->g3DresTex );
    man->g3DresTex = NULL;
  }
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
    if (data->ofs[anmNo] != BMANIME_NULL_OFS)
    { 
      count ++;
    }
  }
  GF_ASSERT( count == data->anmset_num * data->ptn_count );
  return count;
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
  TAMADA_Printf("%d %d %d\n",data->prg_type, data->anmset_num, data->ptn_count);
  for (anmNo = 0; anmNo < GLOBAL_OBJ_ANMCOUNT; anmNo++)
  {
    TAMADA_Printf("%08x ", data->ofs[anmNo]);
  }
  TAMADA_Printf("\n");
}
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
static const FIELD_BMANIME_DATA * BMINFO_getAnimeData(const BMINFO * bmInfo)
{
  return &bmInfo->animeData;
}

//------------------------------------------------------------------
/**
 * @brief 検索IDの取得
 */
//------------------------------------------------------------------
static BM_SEARCH_ID BMINFO_getSearchID( const BMINFO * bmInfo )
{
  const u8 search_id[] = {
    BM_SEARCH_ID_NULL,        //BM_PROG_ID_NONE
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_NORMAL,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_AUTO,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_BADGEGATE,
    BM_SEARCH_ID_SANDSTREAM,  //BM_PROG_ID_SANDSTREAM,
    BM_SEARCH_ID_NULL,        //BM_PROG_ID_PCELEVATOR,
    BM_SEARCH_ID_PCMACHINE,   //BM_PROG_ID_PCMACHINE,
    BM_SEARCH_ID_PC,          //BM_PROG_ID_PC,
    BM_SEARCH_ID_PCEV_DOOR,   //BM_PROG_ID_PCEV_DOOR,
    BM_SEARCH_ID_PCEV_FLOOR,  //BM_PROG_ID_PCEV_FLOOR,
    BM_SEARCH_ID_WARP,        //BM_PROG_ID_WARP,
    BM_SEARCH_ID_EVENT,       //BM_PROG_ID_EVENT,
    BM_SEARCH_ID_PPMACHINE,   //BM_PROG_ID_PPMACHINE,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_ELEVATOR,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_BELL,
    BM_SEARCH_ID_DOOR,        //BM_PROG_ID_DOOR_AUTO2
  };
  SDK_COMPILER_ASSERT( NELEMS(search_id) == BM_PROG_ID_MAX );

  if ( bmInfo->prog_id >= BM_PROG_ID_MAX )
  {
    GF_ASSERT_MSG(0, "配置モデルプログラム指定(%d)のエラーです\n", bmInfo->prog_id );
    return BM_SEARCH_ID_NULL;
  }
  return search_id[ bmInfo->prog_id ];
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void TIMEANIME_CTRL_init(TIMEANIME_CTRL * tmanm_ctrl, u8 season)
{
  tmanm_ctrl->NowTimeZone = TIMEZONE_MAX; //ありえない数
  tmanm_ctrl->season      = season;
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
  tmanm_ctrl->NowTimeZone = PM_RTC_GetTimeZone(tmanm_ctrl->season);
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
//  リソース生成
//
//
//============================================================================================
//------------------------------------------------------------------
//配置モデルマネジャーからの内容で生成
//------------------------------------------------------------------
static void createAllResource(FIELD_BMODEL_MAN * man, u8 *gray_scale)
{ 
  u32 entryCount = man->entryCount;

	if( entryCount == 0 ){
    return;
  }

  man->objRes_Count = entryCount;
  man->objRes = GFL_HEAP_AllocClearMemory ( man->heapID, sizeof(OBJ_RES) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->objRes );


  /** 配置モデルリソース登録 */
  {
    u8 entryNo;

    for ( entryNo = 0; entryNo < man->objRes_Count; entryNo++ )
    {
      OBJ_RES * objRes = &man->objRes[entryNo];
      //TAMADA_Printf("BM:Create Rsc %d (%d)\n", entryNo, bm_id);
      OBJRES_initialize( man, objRes, entryNo, gray_scale );
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
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->objRes );
  GFL_HEAP_FreeMemory( man->objRes );
  man->objRes_Count = 0;
  man->objRes = NULL;
}


//------------------------------------------------------------------
/** 配置モデルG3D_OBJECT登録 */
//------------------------------------------------------------------
static void createFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
  int i;
  u32 entryCount = man->entryCount;

  if( entryCount == 0 ){
    return;
  }

  g3dMapObj->gobjCount = entryCount;
  g3dMapObj->gobj = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(FLD_G3D_MAP_OBJ) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( g3dMapObj->gobj );

  man->objHdl_Count = entryCount;
  man->objHdl = GFL_HEAP_AllocClearMemory( man->heapID, sizeof(OBJ_HND) * entryCount );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( man->objHdl );

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
static void deleteFullTimeObjHandle(FIELD_BMODEL_MAN * man, FLD_G3D_MAP_GLOBALOBJ * g3dMapObj)
{
	if( g3dMapObj->gobj != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3dMapObj->gobj );
		GFL_HEAP_FreeMemory( g3dMapObj->gobj );
		g3dMapObj->gobj = NULL;
	}
	if( g3dMapObj->gobjIDexchange != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3dMapObj->gobjIDexchange );
		GFL_HEAP_FreeMemory( g3dMapObj->gobjIDexchange );
		g3dMapObj->gobjIDexchange = NULL;
	}

  if( man->objHdl != NULL ){
    int i;
    for( i=0; i<man->objHdl_Count; i++ ){
			OBJHND_finalize( &man->objHdl[i] );
    }
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( man->objHdl );
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
// オブジェクトリソースを作成
//------------------------------------------------------------------
static void OBJRES_initialize( FIELD_BMODEL_MAN * man, OBJ_RES * objRes, u16 entryNo, u8 *gray_scale)
{
	GFL_G3D_RES* resTex;

  //配置モデルに対応した情報へのポインタをセット
  objRes->bmInfo = BMODELMAN_GetBMInfoEntry( man, entryNo );

  //モデルデータ生成
  objRes->g3DresMdl = GFL_HEAP_AllocClearMemory( man->heapID, GFL_G3D_GetResourceHeaderSize() );
	GFL_G3D_CreateResourceAuto( objRes->g3DresMdl, (void*)BM_AREA_HEADER_getNSBMDData( man->bm_areadata, entryNo ) ); //配置モデル内で、リソースデータを書き換えることはない。
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objRes->g3DresMdl );


  {
    const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objRes->bmInfo);
    int anmNo;
    int count = BMANIME_getCount(anmData);
    for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
      if ( anmNo < count ) {
        objRes->g3DresAnm[anmNo] = GFL_HEAP_AllocClearMemory( man->heapID, GFL_G3D_GetResourceHeaderSize() );
        GFL_G3D_CreateResourceAuto( objRes->g3DresAnm[anmNo], (void*)BM_ANIME_DATA_GetAnimeData( anmData, anmNo ) );
        BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objRes->g3DresAnm[anmNo] );
      } else {
        objRes->g3DresAnm[anmNo] = NULL;
      }
    }
  }

}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void OBJRES_finalize( OBJ_RES * objRes )
{
  int anmNo;
	GFL_G3D_RES*	resTex;

  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objRes->g3DresAnm[anmNo] );
      GFL_HEAP_FreeMemory( objRes->g3DresAnm[anmNo] );
      objRes->g3DresAnm[anmNo] = NULL;
    }
  }

  if( objRes->g3DresMdl != NULL ){
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objRes->g3DresMdl );
    GFL_HEAP_FreeMemory( objRes->g3DresMdl );
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

static void OBJHND_TYPENONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void OBJHND_TYPENONE_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );
static void OBJHND_TYPEEVENT_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req );

static void OBJHND_TYPETIMEZONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEEVENT_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPEETERNAL_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );
static void OBJHND_TYPENONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl );

static void disableAllAnime(OBJ_HND * objHdl);
//------------------------------------------------------------------
///   各アニメタイプごとの処理関数テーブル
//------------------------------------------------------------------
static const OBJHND_FUNCS objHndFuncTable[BMANIME_TYPE_MAX] = {
  { //BMANIME_TYPE_NONE アニメしない
    OBJHND_TYPENONE_initAnime,
    OBJHND_TYPENONE_animate,
    OBJHND_TYPENONE_setAnime,
  },
  { //BMANIME_TYPE_ETERNAL  ずっとおなじアニメ
    OBJHND_TYPEETERNAL_initAnime,
    OBJHND_TYPEETERNAL_animate,
    OBJHND_TYPENONE_setAnime,
  },
  { //BMANIME_TYPE_EVENT  イベントからアニメリクエスト（通常停止）
    OBJHND_TYPENONE_initAnime,
    OBJHND_TYPEEVENT_animate,
    OBJHND_TYPEEVENT_setAnime,
  },
  { //BMANIME_TYPE_TIMEZONE 時間帯によるアニメ変更
    OBJHND_TYPETIMEZONE_initAnime,
    OBJHND_TYPETIMEZONE_animate,
    OBJHND_TYPENONE_setAnime,
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

  //レンダー生成
  g3Drnd = GFL_G3D_RENDER_Create( objRes->g3DresMdl, 0, man->g3DresTex );
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( g3Drnd );
  BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );

  //アニメオブジェクト生成
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if( objRes->g3DresAnm[anmNo] != NULL ){
      anmTbl[anmNo] = GFL_G3D_ANIME_Create( g3Drnd, objRes->g3DresAnm[anmNo], 0 );  
      BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( anmTbl[anmNo] );
      BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Plus( GFL_G3D_ANIME_GetAnmObj( anmTbl[anmNo] ) );
      //モデルとアニメがずれているとメモリ破壊が発生するので、ここで検出
      GF_ASSERT_MSG( GFL_HEAP_CheckHeapSafe( man->heapID ) == TRUE,
          "BuildModel Create Error! id=%d\n", objRes->bmInfo->bm_id );
    } else {
      anmTbl[anmNo] = NULL;
    }
  }
  //GFL_G3D_OBJECT生成
  objHdl->g3Dobj = GFL_G3D_OBJECT_Create( g3Drnd, anmTbl, GLOBAL_OBJ_ANMCOUNT );
  objHdl->res = objRes;
  BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Plus( objHdl->g3Dobj );

  //アニメ状態初期化
  {
    BMANIME_TYPE type = anmData->anm_type;
    objHndFuncTable[type].init_anime_func( man, objHdl );
  }
}

//------------------------------------------------------------------
///OBJHND初期化：アニメタイプその他
//------------------------------------------------------------------
static void OBJHND_TYPENONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
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
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 set_num = anmData->anmset_num;
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT && anmNo<set_num; anmNo++ ){
    GFL_G3D_OBJECT_EnableAnime( objHdl->g3Dobj, anmNo );  //renderとanimeの関連付け
    GFL_G3D_OBJECT_ResetAnimeFrame( objHdl->g3Dobj, anmNo ); 
    objHdl->anmMode[anmNo] = BM_ANMMODE_LOOP;
  }
  for( ;anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo ++)
  {
    objHdl->anmMode[anmNo] = BM_ANMMODE_NOTHING;
  }
}
//------------------------------------------------------------------
///OBJHND初期化：アニメタイプTIMEZONE
//------------------------------------------------------------------
static void OBJHND_TYPETIMEZONE_initAnime( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
{
  int anmNo, nowNo;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  GF_ASSERT( anmData->anmset_num == 1 );  //時間帯アニメはセット数が大きいのは無理！
  nowNo = TIMEANIME_CTRL_getIndex( &man->tmanm_ctrl );
  for( anmNo=0; anmNo<GLOBAL_OBJ_ANMCOUNT; anmNo++ ){
    if (anmNo != nowNo)
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
        BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3Danm );
        BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( GFL_G3D_ANIME_GetAnmObj( g3Danm ) );
        GFL_G3D_ANIME_Delete( g3Danm ); 
      }
    }
    g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( objHdl->g3Dobj );
  
    //各種ハンドル＆リソース削除
    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( objHdl->g3Dobj );
    GFL_G3D_OBJECT_Delete( objHdl->g3Dobj );
    objHdl->g3Dobj = NULL;
    objHdl->res = NULL;

    BMODEL_DEBUG_RESOURCE_MEMORY_SIZE_Minus( g3Drnd );
    BMODEL_DEBUG_RESOURCE_MEMORY_ALLOCATOR_SIZE_Minus( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );
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
static void OBJHND_TYPENONE_animate( const FIELD_BMODEL_MAN * man, OBJ_HND * objHdl )
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
    case BM_ANMMODE_REVERSE:
      result = GFL_G3D_OBJECT_IncAnimeFrame( objHdl->g3Dobj, anmNo, -FX32_ONE );
      if (!result) {
        GFL_G3D_OBJECT_SetAnimeFrame( objHdl->g3Dobj, anmNo, 0 );
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
//------------------------------------------------------------------
static void applyNormalAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for (i = 0; i < anmData->anmset_num; i++) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    GFL_G3D_OBJECT_ResetAnimeFrame(objHdl->g3Dobj, setNo + i );
    objHdl->anmMode[setNo + i] = BM_ANMMODE_TEMPORARY;
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyLoopAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for (i = 0; i < anmData->anmset_num; i++) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    GFL_G3D_OBJECT_ResetAnimeFrame(objHdl->g3Dobj, setNo + i );
    objHdl->anmMode[setNo + i] = BM_ANMMODE_LOOP;
  }
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyReverseAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for ( i = 0; i < anmData->anmset_num; i++ ) {
    GFL_G3D_OBJECT_EnableAnime(objHdl->g3Dobj, setNo + i );
    {
      GFL_G3D_ANM * g3Danm = GFL_G3D_OBJECT_GetG3Danm( objHdl->g3Dobj, setNo + i );
      NNSG3dAnmObj * anmObj = GFL_G3D_ANIME_GetAnmObj( g3Danm );
      fx32 num = NNS_G3dAnmObjGetNumFrame( anmObj );
      GFL_G3D_OBJECT_SetAnimeFrame(objHdl->g3Dobj, setNo + i, (const int*)&num );
    }
    objHdl->anmMode[setNo + i] = BM_ANMMODE_REVERSE;
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void applyStopAnime( OBJ_HND * objHdl, u32 anmNo )
{
  int i;
  const FIELD_BMANIME_DATA * anmData = BMINFO_getAnimeData(objHdl->res->bmInfo);
  u32 setNo = anmData->anmset_num * anmNo;
  for ( i = 0; i < anmData->anmset_num; i++ ) {
    if (objHdl->anmMode[setNo + i] != BM_ANMMODE_NOTHING) {
      objHdl->anmMode[setNo + i] = BM_ANMMODE_STOP;
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
  GF_ASSERT_MSG(req < BMANM_REQ_MAX, "BMANM_REQ(%d)は未対応です\n", req ); //未定義リクエスト

  switch ((BMANM_REQUEST)req) {
  case BMANM_REQ_START:
    disableAllAnime( objHdl );
    applyNormalAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_REVERSE_START:
    disableAllAnime( objHdl );
    applyReverseAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_LOOP:
    disableAllAnime( objHdl );
    applyLoopAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_STOP:
    applyStopAnime( objHdl, anmNo );
    break;
  case BMANM_REQ_END:
    disableAllAnime( objHdl );
    break;
    break;
  }
}
//------------------------------------------------------------------
/// OBJHNDアニメ変更処理：その他
//------------------------------------------------------------------
static void OBJHND_TYPENONE_setAnime( OBJ_HND * objHdl, u32 anmNo, BMANM_REQUEST req )
{
  /* DO NOTHING !! */
}

//------------------------------------------------------------------
/**
 * @brief アニメ状態取得
 * @return  BOOL  TRUE=停止状態　FALSE=動作中
 */
//------------------------------------------------------------------
static BOOL OBJHND_getAnimeStatus( const OBJ_HND * objHdl, u32 anmNo)
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
  case BM_ANMMODE_REVERSE:
    return FALSE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションフレームを取得する
 *
 *	@param	objHdl  ハンドル
 *  
 *	@return フレーム数
 */
//-----------------------------------------------------------------------------
static fx32 OBJHND_getAnimeFrame( const OBJ_HND * objHdl, u32 anmNo)
{
  int frame;
  
  GF_ASSERT( objHdl );
  GFL_G3D_OBJECT_GetAnimeFrame( objHdl->g3Dobj, anmNo, &frame );

  return frame;
}



//============================================================================================
//
//  FLD_G3D_MAPの保持するFLD_G3D_MAP_GLOBALOBJ_STを参照、コントロールするための
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
  obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
  obj->index = 0;
  obj->viewFlag = FALSE;
  obj->objSt = NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static G3DMAPOBJST * G3DMAPOBJST_create(
    FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST * status, int idx)
{
  int i;
  for (i = 0; i < NELEMS(man->g3DmapObjSt); i++)
  {
    G3DMAPOBJST * obj = &man->g3DmapObjSt[i];
    if ( G3DMAPOBJST_exists(obj) == FALSE )
    {
      // 多重登録チェック
      GF_ASSERT( FLD_G3D_MAP_GetGlobalObj(g3Dmap, idx) == NULL );
      
      FLD_G3D_MAP_ResistGlobalObj( g3Dmap, status, idx );
      obj->g3Dmap = g3Dmap;
      obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
      obj->index = idx;
      obj->viewFlag = TRUE;
      obj->objSt = FLD_G3D_MAP_GetGlobalObj( g3Dmap, idx );

      return obj;
    }
  }
  GF_ASSERT(0);
  return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void G3DMAPOBJST_deleteByG3Dmap(FIELD_BMODEL_MAN * man, FLD_G3D_MAP * g3Dmap)
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
  GF_ASSERT( obj->objSt == FLD_G3D_MAP_GetGlobalObj(obj->g3Dmap, obj->index) );
  FLD_G3D_MAP_ReleaseGlobalObj( obj->g3Dmap, obj->index );
  G3DMAPOBJST_init( man, obj );
}

//------------------------------------------------------------------
///  表示されているかどうかのチェック
//------------------------------------------------------------------
static BOOL G3DMAPOBJST_isVanished( const G3DMAPOBJST * obj )
{
  return (obj->objSt->id == FLD_G3D_MAP_OBJID_NULL );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static u8 G3DMAPOBJST_getEntryNo( const G3DMAPOBJST * obj )
{
  if ( obj->objSt->id == FLD_G3D_MAP_OBJID_NULL )
  {
    return obj->entryNoBackup;
  }
  return obj->objSt->id;
}

//------------------------------------------------------------------
/// 配置モデル実データ：表示制御
//------------------------------------------------------------------
void G3DMAPOBJST_changeViewFlag(G3DMAPOBJST * obj, BOOL flag)
{
  GF_ASSERT(obj->viewFlag == !flag);
  if (flag)
  {
    obj->objSt->id = obj->entryNoBackup;
    obj->entryNoBackup = FLD_G3D_MAP_OBJID_NULL;
    obj->viewFlag = TRUE;
  }
  else
  {
    obj->entryNoBackup = obj->objSt->id;
    obj->objSt->id = FLD_G3D_MAP_OBJID_NULL;
    obj->viewFlag = FALSE;
  }
}

//------------------------------------------------------------------
/// 配置モデル実データ：表示ID変更
//------------------------------------------------------------------
void G3DMAPOBJST_changeModelID( const FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj, BMODEL_ID new_id )
{
  obj->objSt->id = BMIDtoEntryNo( man, new_id );
}

//============================================================================================
//
//
//  外部公開関数：
//    G3DMAPOBJST経由での操作関数群
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 配置モデル実データ：検索処理
 * @param man   配置モデルマネジャーへのポインタ
 * @param rect  検索範囲
 * @param search  検索する配置モデルのID
 * @param num     見つかった配置モデル数
 * @return  G3DMAPOBJSTの配列
 */
//------------------------------------------------------------------
G3DMAPOBJST ** FIELD_BMODEL_MAN_CreateObjStatusList
( FIELD_BMODEL_MAN* man, const FLDHIT_RECT * rect, BM_SEARCH_ID search, u32 * num )
{
  G3DMAPOBJST ** result;
  int objNo, count = 0;
  result = GFL_HEAP_AllocClearMemory(man->heapID, sizeof(G3DMAPOBJST*) * FLD_G3D_MAP_OBJST_MAX);

  GF_ASSERT( search < BM_SEARCH_ID_MAX );
  for (objNo = 0; objNo < NELEMS(man->g3DmapObjSt); objNo ++)
  {
    VecFx32 pos;
    G3DMAPOBJST * obj = &man->g3DmapObjSt[objNo];
    if ( G3DMAPOBJST_exists(obj) == FALSE ) continue;
    if (search != BM_SEARCH_ID_NULL &&
        search != G3DMAPOBJST_getSearchID(man, obj) )
    { //検索対象が「何でも」でなく、対象OBJと一致しない場合は次を検索
      continue;
    }

    FLD_G3D_MAP_GetTrans( obj->g3Dmap, &pos );
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
/**
 * @brief 指定位置付近の配置モデルを検索する
 * @param bmodel_man  配置モデルマネジャーへのポインタ
 * @param id          検索ID（BM_SEARCH_ID参照）
 * @param rect         検索位置
 * @retval NULL   見つからなかった
 * @retval G3DMAPOBJST *  見つけたドア配置モデルへの参照
 *
 * @note
 * 内部でFIELD_BMODEL_MAN_CreateObjStatusListを使用し、見つけた先頭の要素を返している
 */
//------------------------------------------------------------------
G3DMAPOBJST * FIELD_BMODEL_MAN_SearchObjStatusRect(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const FLDHIT_RECT * rect)
{
  G3DMAPOBJST * entry = NULL;
  G3DMAPOBJST ** array;
  u32 result_num;

  GF_ASSERT( id < BM_SEARCH_ID_MAX );
  {
    //矩形範囲内の配置モデルリストを生成する
    array = FIELD_BMODEL_MAN_CreateObjStatusList(bmodel_man, rect, id, &result_num);
  }
  entry = array[0];
  //矩形範囲内の配置モデルリストを解放する
  GFL_HEAP_FreeMemory(array);

  return entry;
}
//------------------------------------------------------------------
/**
 * @brief 指定位置付近の配置モデルを検索する
 * @param bmodel_man  配置モデルマネジャーへのポインタ
 * @param id          検索ID（BM_SEARCH_ID参照）
 * @param pos         検索位置
 * @retval NULL   見つからなかった
 * @retval G3DMAPOBJST *  見つけたドア配置モデルへの参照
 *
 * @note
 * 内部でFIELD_BMODEL_MAN_CreateObjStatusListを使用し、見つけた先頭の要素を返している
 */
//------------------------------------------------------------------
G3DMAPOBJST * FIELD_BMODEL_MAN_SearchObjStatusPos(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos)
{
  //検索矩形を作成
  FLDHIT_RECT rect;
  makeRect(&rect, pos);
  return FIELD_BMODEL_MAN_SearchObjStatusRect( bmodel_man, id, &rect );
}


//------------------------------------------------------------------
/// 配置モデル実データ：検索IDの取得
//------------------------------------------------------------------
BM_SEARCH_ID G3DMAPOBJST_getSearchID( const FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj )
{
  u8 entryNo;
  const BMINFO* bmInfo;
  
  GF_ASSERT( G3DMAPOBJST_exists(obj) );
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );

  bmInfo = BMODELMAN_GetBMInfoEntry( man, entryNo );
  return BMINFO_getSearchID(bmInfo);
}

//------------------------------------------------------------------
/// 配置モデル実データ：アニメ反映
//------------------------------------------------------------------
void G3DMAPOBJST_setAnime( FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj, u32 anm_idx, BMANM_REQUEST req )
{
  u8 entryNo;
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  OBJHND_setAnime( &man->objHdl[entryNo], anm_idx, req );
}

//------------------------------------------------------------------
/// 配置モデル実データ：位置取得
//------------------------------------------------------------------
void G3DMAPOBJST_getPos(G3DMAPOBJST * obj, VecFx32 * dst)
{
  VecFx32 pos;
  FLD_G3D_MAP_GetTrans( obj->g3Dmap, &pos );
  VEC_Add( &obj->objSt->trans, &pos, dst );
}

//----------------------------------------------------------------------------
/**
 *	@brief  配置モデル実データ：アニメーションフレーム取得
 *
 *	@param	obj       オブジェ
 *	@param  anm_idx   アニメーションインデックス
 *
 *	@return アニメーションフレーム
 */
//-----------------------------------------------------------------------------
fx32 G3DMAPOBJST_getAnimeFrame(FIELD_BMODEL_MAN* man, const G3DMAPOBJST * obj, u32 anm_idx )
{
  u8 entryNo;
  entryNo = G3DMAPOBJST_getEntryNo( obj );
  //entryNo = obj->objSt->id;
  ENTRYNO_ASSERT( man, entryNo );
  return OBJHND_getAnimeFrame( &man->objHdl[entryNo], anm_idx );
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
FIELD_BMODEL * FIELD_BMODEL_Create(FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj)
{
  const OBJ_RES * objRes;
  const FLD_G3D_MAP_GLOBALOBJ_ST * status = obj->objSt;
  VecFx32 drawOffset;

  FIELD_BMODEL * bmodel = GFL_HEAP_AllocMemory( man->heapID, sizeof(FIELD_BMODEL) );
  bmodel->man = man;
  bmodel->objst = obj;
  bmodel->currentAnmIdx = BMANM_INDEX_NULL;

  //ローテーション設定
  {
    fx32 sin = FX_SinIdx(status->rotate);
    fx32 cos = FX_CosIdx(status->rotate);
    MTX_RotY33( &bmodel->g3dObjStatus.rotate, sin, cos );
  }
  //位置設定：g3Dmapに依存せず表示するため、ここで座標加算しておく
  FLD_G3D_MAP_GetTrans( obj->g3Dmap, &bmodel->g3dObjStatus.trans );
  VEC_Add( &bmodel->g3dObjStatus.trans, &status->trans, &bmodel->g3dObjStatus.trans );
  FLDMAPPER_GetDrawOffset( man->fldmapper, &drawOffset );
  VEC_Add( &bmodel->g3dObjStatus.trans, &drawOffset, &bmodel->g3dObjStatus.trans );
  
  //スケール設定
  VEC_Set( &bmodel->g3dObjStatus.scale, FX32_ONE, FX32_ONE, FX32_ONE );

  GF_ASSERT( status->id < man->objRes_Count );
  objRes = &man->objRes[status->id];
  OBJHND_initialize( man, &bmodel->objHdl, objRes );

  BMODELMAN_EntryBuildModel( man, bmodel );
  G3DMAPOBJST_changeViewFlag( bmodel->objst, FALSE ); //コピー元を非表示にする
  return bmodel;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create_Search(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos )
{
  G3DMAPOBJST * obj;
  FIELD_BMODEL * entry;

  obj = FIELD_BMODEL_MAN_SearchObjStatusPos( bmodel_man, id, pos );
  if (obj == NULL) return NULL;
  entry = FIELD_BMODEL_Create( bmodel_man, obj );

  return entry;
}

//------------------------------------------------------------------
/**
 * @brief 登録済みのリソースを利用して配置モデルを作成する
 * @param man       配置モデルマネージャ
 * @param bmodel_id 作成する配置モデルのID
 * @param status    表示ステータス
 * @return 作成した配置モデル
 */
//------------------------------------------------------------------
FIELD_BMODEL * FIELD_BMODEL_Create_Direct(
    FIELD_BMODEL_MAN * man, BMODEL_ID bmodel_id, const GFL_G3D_OBJSTATUS* status )
{
  FIELD_BMODEL* bmodel = NULL;
  const OBJ_RES * objRes;
  u8 entryNo;

  // インスタンス生成
  bmodel = GFL_HEAP_AllocMemory( man->heapID, sizeof(FIELD_BMODEL) );
  bmodel->man = man;
  bmodel->objst = NULL;
  bmodel->currentAnmIdx = BMANM_INDEX_NULL;

  // 表示ステータス設定
  bmodel->g3dObjStatus = *status;
  // モデル制御オブジェクトを作成
  entryNo = BMIDtoEntryNo( man, bmodel_id );
  objRes = &(man->objRes[entryNo]);
  OBJHND_initialize( man, &bmodel->objHdl, objRes );

  BMODELMAN_EntryBuildModel( man, bmodel );
  return bmodel;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_BMODEL_Delete(FIELD_BMODEL * bmodel)
{
  if ( bmodel == NULL ) return;
  BMODELMAN_releaseBuildModel( bmodel->man, bmodel );

  OBJHND_finalize( &bmodel->objHdl );
  if ( bmodel->objst ) {
    G3DMAPOBJST_changeViewFlag( bmodel->objst, TRUE ); //コピー元を再表示する
  }

  GFL_HEAP_FreeMemory(bmodel);
}

//------------------------------------------------------------------
/// 配置モデル：アニメ変更
//------------------------------------------------------------------
void FIELD_BMODEL_SetAnime(FIELD_BMODEL * bmodel, u32 idx, BMANM_REQUEST req)
{
  if ( bmodel == NULL ) return;
  bmodel->currentAnmIdx = idx;
  OBJHND_setAnime(&bmodel->objHdl, idx, req);
}

//------------------------------------------------------------------
/// 配置モデル：アニメ変更（直前のインデックス対象）
//------------------------------------------------------------------
void FIELD_BMODEL_SetCurrentAnime( FIELD_BMODEL * bmodel, BMANM_REQUEST req )
{
  if ( bmodel == NULL ) return;
  OBJHND_setAnime(&bmodel->objHdl, bmodel->currentAnmIdx, req);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_WaitAnime( FIELD_BMODEL * bmodel, u32 idx )
{
  if (bmodel == NULL) return TRUE;
  if ( FIELD_BMODEL_GetAnimeStatus( bmodel, idx ) == TRUE )
  {
    FIELD_BMODEL_SetAnime( bmodel, idx, BMANM_REQ_STOP );
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_BMODEL_WaitCurrentAnime( FIELD_BMODEL * bmodel )
{
  if (bmodel == NULL) return TRUE;
  if ( FIELD_BMODEL_GetCurrentAnimeStatus( bmodel) == TRUE )
  {
    FIELD_BMODEL_SetCurrentAnime( bmodel, BMANM_REQ_STOP );
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
/// 配置モデル：アニメ状態取得
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetAnimeStatus( const FIELD_BMODEL * bmodel, u32 idx)
{
  if ( bmodel == NULL ) return FALSE;
  return OBJHND_getAnimeStatus( &bmodel->objHdl, idx );
}

//------------------------------------------------------------------
/// 配置モデル：アニメ状態取得（直前のインデックス対象）
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetCurrentAnimeStatus( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return FALSE;
  return OBJHND_getAnimeStatus( &bmodel->objHdl, bmodel->currentAnmIdx );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BM_PROG_ID FIELD_BMODEL_GetProgID(const FIELD_BMODEL * bmodel)
{
  if ( bmodel == NULL ) return BM_PROG_ID_NONE;
  return bmodel->objHdl.res->bmInfo->prog_id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
const G3DMAPOBJST * FIELD_BMODEL_GetG3DMAPOBJST( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return NULL;
  return bmodel->objst;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel )
{
  if ( bmodel == NULL ) return;
  GFL_G3D_DRAW_DrawObject( bmodel->objHdl.g3Dobj, &bmodel->g3dObjStatus );
}

//------------------------------------------------------------------
/**
 * @brief 配置モデルに対応付けされたSEの取得
 */
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetSENo( const FIELD_BMODEL * bmodel, u32 anm_idx, u16 * se_no )
{
  static const struct {
    u16 prog_id;
    u16 seID[BMANM_INDEX_MAX];
  } SeTbl[] = {
    { BM_PROG_ID_DOOR_NORMAL,   { SEQ_SE_FLD_20,  SEQ_SE_FLD_21 } },
    { BM_PROG_ID_DOOR_AUTO,     { SEQ_SE_FLD_22,  SEQ_SE_FLD_22 } },
    //{ BM_PROG_ID_BADGEGATE,   { SEQ_SE_FLD_20,  SEQ_SE_FLD_21 } },
    { BM_PROG_ID_PCELEVATOR,    { SEQ_SE_FLD_22,  SEQ_SE_FLD_22 } },
    { BM_PROG_ID_DOOR_ELEVATOR, { SEQ_SE_FLD_134, SEQ_SE_FLD_134 } },
    { BM_PROG_ID_DOOR_BELL,     { SEQ_SE_FLD_138, SEQ_SE_FLD_139 } },
    { BM_PROG_ID_DOOR_AUTO2,    { SEQ_SE_FLD_22, SEQ_SE_FLD_22 } },
  };
  int i;
  BM_PROG_ID id;

  if ( bmodel == NULL ) return FALSE;
  id = FIELD_BMODEL_GetProgID( bmodel );
  *se_no = 0;
  if ( anm_idx >= BMANM_INDEX_MAX )
  {
    GF_ASSERT_MSG( 0, "anm_idx(%d) >= BMANM_INDEX_MAX\n", anm_idx );
    return FALSE;
  }
  for (i = 0; i < NELEMS(SeTbl); i++)
  {
    if (SeTbl[i].prog_id == id)
    {
      *se_no = SeTbl[i].seID[anm_idx];
      //TAMADA_Printf("BMANIME_PROG_ID(%d) SE(%d)\n", id, *se_no);
      return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------
/// 配置モデル：現在のアニメ対応SEの取得
//------------------------------------------------------------------
BOOL FIELD_BMODEL_GetCurrentSENo( const FIELD_BMODEL * bmodel, u16 * se_no )
{
  if ( bmodel == NULL ) return FALSE;
  return FIELD_BMODEL_GetSENo( bmodel, bmodel->currentAnmIdx, se_no );
}

//------------------------------------------------------------------
/// 配置モデル：対応SEの終了チェック
//------------------------------------------------------------------
BOOL FIELD_BMODEL_CheckCurrentSE( const FIELD_BMODEL * bmodel )
{
  u16 se_no;
  SEPLAYER_ID playerID;
  if ( FIELD_BMODEL_GetCurrentSENo( bmodel, &se_no ) == FALSE )
  {
    return FALSE;
  }
  playerID = PMSND_GetSE_DefaultPlayerID( se_no );
  return PMSND_CheckPlaySE_byPlayerID( playerID );
}





//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos)
{
  enum {
    RECT_SIZE_W = (FIELD_CONST_GRID_SIZE * 2) << FX32_SHIFT,
    RECT_SIZE_H = (FIELD_CONST_GRID_SIZE * 3) << FX32_SHIFT      
  };
  rect->top = pos->z - RECT_SIZE_H;
  rect->bottom = pos->z + RECT_SIZE_H;
  rect->left = pos->x - RECT_SIZE_W;
  rect->right = pos->x + RECT_SIZE_W;
}



#ifdef PM_DEBUG

// スクリプト用情報を表示
void FIELD_BMODEL_DEBUG_ScriptPrintInfo( const FIELD_BMODEL * bmodel )
{
  static const char* ANMTYPE_STR[BMANIME_TYPE_MAX] = 
  {
    "BMANIME_TYPE_NONE",
    "BMANIME_TYPE_ETERNAL",
    "BMANIME_TYPE_EVENT",
    "BMANIME_TYPE_TIMEZONE",
  };
  
  if ( bmodel == NULL )
  {
    OS_TPrintf( "address == NULL :配置モデルが存在しません！\n" );
    return;
  }
  OS_TPrintf( "配置モデル ID　      %d\n", bmodel->objHdl.res->bmInfo->bm_id+1 );
  OS_TPrintf( "配置モデルアニメ ID　%d\n", bmodel->objHdl.res->bmInfo->anm_id+1 );
  OS_TPrintf( "配置モデル位置　x[%d] y[%d] z[%d]\n", FX_Whole(bmodel->g3dObjStatus.trans.x), FX_Whole(bmodel->g3dObjStatus.trans.y), FX_Whole(bmodel->g3dObjStatus.trans.z) );
  OS_TPrintf( "配置モデルグリッド　x[%d] y[%d] z[%d]\n", 
      FX_Whole(bmodel->g3dObjStatus.trans.x)/FIELD_CONST_GRID_SIZE, 
      FX_Whole(bmodel->g3dObjStatus.trans.y)/FIELD_CONST_GRID_SIZE,
      FX_Whole(bmodel->g3dObjStatus.trans.z)/FIELD_CONST_GRID_SIZE );

  OS_TPrintf( "アニメーションタイプ %s\n", ANMTYPE_STR[ bmodel->objHdl.res->bmInfo->animeData.anm_type ] );

  GF_ASSERT_MSG( bmodel->objHdl.res->bmInfo->animeData.anm_type == BMANIME_TYPE_EVENT, "スクリプトからANIMETYPE EVENT以外の配置オブジェを操作しようとしています。　位置を確認してください。\n" );
}

#endif



//============================================================================================
//  DEBUG リソースメモリ使用量の検査
//============================================================================================
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

//----------------------------------------------------------------------------
/**
 *	@brief  リソースで使用しているメモリーサイズの取得
 */
//-----------------------------------------------------------------------------
u32 FIELD_BMODEL_MAN_GetUseResourceMemorySize(void)
{
  return BMODEL_DEBUG_RESOURCE_MemorySize;
}

#endif

#include "script_def.h"
//プログラム内参照の定義とスクリプト内参照の定義がずれていたらassert
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_OPEN == BMANM_INDEX_DOOR_OPEN );
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_CLOSE == BMANM_INDEX_DOOR_CLOSE );

SDK_COMPILER_ASSERT( SCR_BMID_DOOR == BM_SEARCH_ID_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_SANDSTREAM == BM_SEARCH_ID_SANDSTREAM );
SDK_COMPILER_ASSERT( SCR_BMID_PCMACHINE == BM_SEARCH_ID_PCMACHINE );
SDK_COMPILER_ASSERT( SCR_BMID_PC == BM_SEARCH_ID_PC );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_DOOR == BM_SEARCH_ID_PCEV_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_FLOOR == BM_SEARCH_ID_PCEV_FLOOR );

