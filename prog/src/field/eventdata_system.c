//============================================================================================
/**
 * @file	eventdata_system.c
 * @brief	イベント起動用データのロード・保持システム
 * @author	tamada GAME FREAK inc.
 * @date	2008.11.20
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"

#include "eventdata_local.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

#include "field/location.h"

#include "fldmmdl.h"

#include "fieldmap/zone_id.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
	EVDATA_SIZE = 0x800,
	SPSCR_DATA_SIZE = 0x100,
};

//------------------------------------------------------------------
/**
 * @brief	イベント起動データシステムの定義
 */
//------------------------------------------------------------------
struct _EVDATA_SYS {
	HEAPID heapID;

	u16 now_zone_id;
	u16 bg_count;
	u16 npc_count;
	u16 connect_count;
	u16 pos_count;
	const BG_TALK_DATA * bg_data;
	const FLDMMDL_HEADER *npc_data;
	const CONNECT_DATA * connect_data;
	const POS_EVENT_DATA * pos_data;

	//ENCOUNT_DATA encount_work;
	u32 load_buffer[EVDATA_SIZE / sizeof(u32)];
	u32 spscr_buffer[SPSCR_DATA_SIZE / sizeof(u32)];
};


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {

  u8 bg_count;
  u8 npc_count;
  u8 connect_count;
  u8 pos_count;

  void * bg_data;
  void * npc_data;
  void * connect_data;
  void * pos_data;

}EVENTDATA_TABLE;

typedef struct {
  u16 zone_id;
  const EVENTDATA_TABLE * table;
}EVENTDATA_TOTAL_TABLES;
//------------------------------------------------------------------
//------------------------------------------------------------------

//#define DOOR_ID_T01R0301_EXIT01 0 //暫定！
#include "arc/fieldmap/zone_id.h"
//仮動作モデル配置データ
#include "../../../resource/fldmapdata/eventdata/zone_t01evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t01r0101evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t01r0201evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t01r0301evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t01r0401evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_r01evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t02evc.cdat"
#include "../../../resource/fldmapdata/eventdata/zone_t02pc0101evc.cdat"

//生成したイベントデータを全部インクルード
#include "../../../resource/fldmapdata/eventdata/eventdata_table.cdat"


//============================================================================================
//============================================================================================
static void loadEventDataTable(EVENTDATA_SYSTEM * evdata, u16 zone_id);

//============================================================================================
//
//	イベント起動データシステム関連
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
EVENTDATA_SYSTEM * EVENTDATA_SYS_Create(HEAPID heapID)
{
	EVENTDATA_SYSTEM * evdata = GFL_HEAP_AllocClearMemory(heapID, sizeof(EVENTDATA_SYSTEM));
	return evdata;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Delete(EVENTDATA_SYSTEM * evdata)
{
	GFL_HEAP_FreeMemory(evdata);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Clear(EVENTDATA_SYSTEM * evdata)
{
	evdata->bg_count = 0;
	evdata->npc_count = 0;
	evdata->connect_count = 0;
	evdata->pos_count = 0;
	evdata->bg_data = NULL;
	evdata->npc_data = NULL;
	evdata->connect_data = NULL;
	evdata->pos_data = NULL;
	GFL_STD_MemClear(evdata->load_buffer, EVDATA_SIZE);
	GFL_STD_MemClear(evdata->spscr_buffer, SPSCR_DATA_SIZE);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Load(EVENTDATA_SYSTEM * evdata, u16 zone_id)
{
	EVENTDATA_SYS_Clear(evdata);
	evdata->now_zone_id = zone_id;

  loadEventDataTable(evdata, zone_id);
	/* テスト的に接続データを設定 */
	switch (zone_id) {
	case ZONE_ID_T01:
		evdata->npc_count = SampleFldMMdlHeaderCount_T01;
		evdata->npc_data = SampleFldMMdlHeader_T01;
		break;
	case ZONE_ID_T02:
		evdata->npc_count = SampleFldMMdlHeaderCount_t02;
		evdata->npc_data = SampleFldMMdlHeader_t02;
		break;
	case ZONE_ID_R01:
		evdata->npc_count = SampleFldMMdlHeaderCount_R01;
		evdata->npc_data = SampleFldMMdlHeader_R01;
		break;
	case ZONE_ID_T01R0101:
		evdata->npc_count = SampleFldMMdlHeaderCount_t01r0101;
		evdata->npc_data = SampleFldMMdlHeader_t01r0101;
		break;
  case ZONE_ID_T01R0102:
    break;
	case ZONE_ID_T01R0201:
		evdata->npc_count = SampleFldMMdlHeaderCount_t01r0201;
		evdata->npc_data = SampleFldMMdlHeader_t01r0201;
		break;
  case ZONE_ID_T01R0301:
		evdata->npc_count = SampleFldMMdlHeaderCount_t01r0301;
		evdata->npc_data = SampleFldMMdlHeader_t01r0301;
		break;
  case ZONE_ID_T01R0401:
		evdata->npc_count = SampleFldMMdlHeaderCount_t01r0401;
		evdata->npc_data = SampleFldMMdlHeader_t01r0401;
		break;
 case ZONE_ID_T02PC0101:
		evdata->npc_count = SampleFldMMdlHeaderCount_t02pc0101;
		evdata->npc_data = SampleFldMMdlHeader_t02pc0101;
		break;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void loadEventDataTable(EVENTDATA_SYSTEM * evdata, u16 zone_id)
{
  int i;
  const EVENTDATA_TOTAL_TABLES * tables = TotalTables;
  for (i = 0; i < ZONE_ID_MAX; i++, tables ++)
  {
    if (tables->table == NULL) return; //centinel
    if (tables->zone_id == zone_id)
    {
      break;
    }
  }
  evdata->npc_count = tables->table->npc_count;
  evdata->npc_data = tables->table->npc_data;
  evdata->connect_count = tables->table->connect_count;
  evdata->connect_data = tables->table->connect_data;
  evdata->bg_count = tables->table->bg_count;
  evdata->bg_data = tables->table->bg_data;
  evdata->pos_count = tables->table->pos_count;
  evdata->pos_data = tables->table->pos_data;

  for (i = 0; i < evdata->connect_count; i++)
  {
    const CONNECT_DATA * cnct = &evdata->connect_data[i];
    TAMADA_Printf("CNCT:ID%02d (%08x, %08x, %08x)\n", i, cnct->pos.x, cnct->pos.y, cnct->pos.z);
  }
}

//============================================================================================
//
//	出入口接続データ関連
//
//============================================================================================
#define	ZONE_ID_SPECIAL		(0x0fff)
#define	EXIT_ID_SPECIAL		(0x0100)
	enum {
		OFS_X = -8,
		OFS_Y = 0,
		OFS_Z = 8,
	};
//------------------------------------------------------------------
//------------------------------------------------------------------
int EVENTDATA_SearchConnectIDByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos)
{
	int i;
	int x,y,z;
	const CONNECT_DATA * cnct = evdata->connect_data;
	x = FX_Whole(pos->x) - OFS_X;
	y = FX_Whole(pos->y) - OFS_Y;
	z = FX_Whole(pos->z) - OFS_Z;
	for (i = 0; i < evdata->connect_count; i++, cnct++ ) {
#if 0
		OS_Printf("CNCT:x,y,z=%d,%d,%d\n",
				FX_Whole(cnct->pos.x),FX_Whole(cnct->pos.y),FX_Whole(cnct->pos.z));
#endif
		if (cnct->pos.x != x) continue;
		if (cnct->pos.y != y) continue;
		if (cnct->pos.z != z) continue;
		OS_Printf("CNCT:zone,exit,type=%d,%d,%d\n",cnct->link_zone_id,cnct->link_exit_id,cnct->exit_type);
		OS_Printf("CNCT:x %d(%08x), y %d(%08x), z %d(%08x)\n",x,pos->x, y,pos->y, z,pos->z);
		return i;
	}
	return EXIT_ID_NONE;
}
#if 0
//------------------------------------------------------------------
//------------------------------------------------------------------
const CONNECT_DATA * EVENTDATA_SearchConnectByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos)
{
	int idx = EVENTDATA_SearchConnectIDByPos(evdata, pos);
	if (idx == EXIT_ID_NONE) return NULL;
	return evdata->connect_data + idx;
}
#endif
//------------------------------------------------------------------
//------------------------------------------------------------------
const CONNECT_DATA * EVENTDATA_GetConnectByID(const EVENTDATA_SYSTEM * evdata, u16 exit_id)
{
	const CONNECT_DATA * cnct = evdata->connect_data;
	if (cnct == NULL) {
		return NULL;
	}
	if (exit_id >= evdata->connect_count) {
		GF_ASSERT(0);
		return NULL;
	}
	return &cnct[exit_id];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL CONNECTDATA_IsSpecialExit(const CONNECT_DATA * connect)
{
	return (connect->link_exit_id == EXIT_ID_SPECIAL);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL EVENTDATA_SetLocationByExitID(const EVENTDATA_SYSTEM * evdata, LOCATION * loc, u16 exit_id)
{
  /** TODO:　見た目含めてオフセットを再計算すること */
  enum{ GRIDSIZE = 8, MV = GRIDSIZE * FX32_ONE};
  static const VecFx32 ofs = { OFS_X * FX32_ONE, OFS_Y * FX32_ONE, 0 * FX32_ONE };

	const CONNECT_DATA * connect = EVENTDATA_GetConnectByID(evdata, exit_id);
	if (connect == NULL) {
		return FALSE;
	}

	//CONNECTDATA_SetLocation(connect, loc);
	LOCATION_Init(loc);
	if (connect->link_exit_id == EXIT_ID_SPECIAL) {
		loc->type = LOCATION_TYPE_SPID;
	} else {
		loc->type = LOCATION_TYPE_EXITID;
	}
	//loc->zone_id = connect->link_zone_id;
	//loc->exit_id = connect->link_exit_id;
	//loc->pos = connect->pos;
	VEC_Set(&loc->pos,
			connect->pos.x * FX32_ONE,
			connect->pos.y * FX32_ONE,
			connect->pos.z * FX32_ONE);
  VEC_Add(&loc->pos, &ofs, &loc->pos);
	loc->dir_id = connect->exit_dir;
	loc->zone_id = evdata->now_zone_id;
	loc->exit_id = exit_id;

	return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void CONNECTDATA_SetNextLocation(const CONNECT_DATA * connect, LOCATION * loc)
{
	LOCATION_SetID(loc, connect->link_zone_id, connect->link_exit_id);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
int EVENTDATA_SearchConnectIDBySphere(const EVENTDATA_SYSTEM * evdata, const VecFx32 * sphere)
{
  enum { HIT_RANGE = FX32_ONE * 10 };
	int i;
	int x,y,z;
  fx32 len;
	const CONNECT_DATA * cnct = evdata->connect_data;
	x = FX_Whole(sphere->x) - OFS_X;
	y = FX_Whole(sphere->y) - OFS_Y;
	z = FX_Whole(sphere->z) - OFS_Z;
	for (i = 0; i < evdata->connect_count; i++, cnct++ ) {
#if 0
		OS_Printf("CNCT:x,y,z=%d,%d,%d\n",
				FX_Whole(cnct->pos.x),FX_Whole(cnct->pos.y),FX_Whole(cnct->pos.z));
#endif
    {
      fx32 lx = x - cnct->pos.x;
      fx32 ly = y - cnct->pos.y;
      fx32 lz = z - cnct->pos.z;
      fx32 len = FX_Sqrt(FX_Mul(lx,lx) + FX_Mul(ly,ly) + FX_Mul(lz, lz));
      if (len > HIT_RANGE) continue;
    }
		OS_Printf("CNCT:zone,exit,type=%d,%d,%d\n",cnct->link_zone_id,cnct->link_exit_id,cnct->exit_type);
		OS_Printf("CNCT:x %d(%08x), y %d(%08x), z %d(%08x)\n",x,sphere->x, y,sphere->y, z,sphere->z);
		return i;
	}
	return EXIT_ID_NONE;
}

//============================================================================================
//		動作モデル関連
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	動作モデルヘッダーを取得
 * @param	evdata		イベントデータへのポインタ
 * @retval	FLDMMDL_HEADER*
 */
//------------------------------------------------------------------
const FLDMMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata )
{
	return( evdata->npc_data );
}

//------------------------------------------------------------------
/**
 * @brief	動作モデル総数を取得
 * @param	evdata		イベントデータへのポインタ
 * @retval	u16
 */
//------------------------------------------------------------------
u16 EVENTDATA_GetNpcCount( const EVENTDATA_SYSTEM *evdata )
{
	return( evdata->npc_count );
}

//============================================================================================
//
//		サンプルデータ
//		※実際には外部でコンバートされたものをファイルから読み込む
//
//============================================================================================

#if 0
const FLDMMDL_HEADER SampleFldMMdlHeader_R01[] = {
	{
		0,		///<識別ID
		BOY1,	///<表示するOBJコード
		MV_RND,	///<動作コード
		0,	///<イベントタイプ
		0,	///<イベントフラグ
		0,	///<イベントID
		DIR_DOWN,	///<指定方向
		0,	///<指定パラメタ 0
		0,	///<指定パラメタ 1
		0,	///<指定パラメタ 2
		2,	///<X方向移動制限
		2,	///<Z方向移動制限
		757,	///<グリッドX
		788,	///<グリッドZ
		0,	///<Y値 fx32型
	},
	{
		1,		///<識別ID
		WOMAN1,	///<表示するOBJコード
		MV_RND,	///<動作コード
		0,	///<イベントタイプ
		0,	///<イベントフラグ
		0,	///<イベントID
		DIR_DOWN,	///<指定方向
		0,	///<指定パラメタ 0
		0,	///<指定パラメタ 1
		0,	///<指定パラメタ 2
		2,	///<X方向移動制限
		2,	///<Z方向移動制限
		756,	///<グリッドX
		796,	///<グリッドZ
		0,	///<Y値 fx32型
	},
};

const int SampleFldMMdlHeaderCount_R01 = NELEMS(SampleFldMMdlHeader_R01);
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------

