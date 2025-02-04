//======================================================================
/**
 * @file	field_gimmick_d06.c
 * @brief	フィールドギミック　D06電気洞窟
 * @authaor	kagaya
 * @date	2008.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "savedata/gimmickwork.h"

#include "arc/fieldmap/zone_id.h"

#include "fieldmap.h"
#include "map_attr.h"
#include "fldeff_d06denki.h"
#include "field_gimmick_def.h"

#include "field_gimmick_d06.h"

//浮遊石配置cdatファイル
#include "../../../resource/fldmapdata/gimmick/d06/d06stone.cdat"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// typedef 
//--------------------------------------------------------------
typedef struct _TAG_D06GMK_WORK D06GMK_WORK;
typedef struct _TAG_D06_WORK D06_WORK;

//--------------------------------------------------------------
// STONE_POS
//--------------------------------------------------------------
typedef struct
{
  int x;
  int y;
  int z;
}STONE_POS;

//--------------------------------------------------------------
// STONE_INDEX
//--------------------------------------------------------------
typedef struct
{
  u32 zone_id;
  u32 stone_max;
  STONE_POS *stone_pos;
}STONE_INDEX;

//--------------------------------------------------------------
/// D06GMK_WORK
//--------------------------------------------------------------
struct _TAG_D06GMK_WORK
{
  u8 init_flag;
  D06_WORK *d06_work;
};

//--------------------------------------------------------------
/// BIGSTONE_WORK
//--------------------------------------------------------------
typedef struct
{
  MMDL *mmdl;
  FLDEFF_TASK *eff_task;
}BIGSTONE_WORK;

//--------------------------------------------------------------
/// MINISTONE_WORK
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_TASK *eff_task;
}MINISTONE_WORK;

//--------------------------------------------------------------
/// D06_WORK
//--------------------------------------------------------------
struct _TAG_D06_WORK
{
  u32 zone_id;
  D06GMK_WORK *d06gmk_work;
  FIELDMAP_WORK *fieldmap;
  
  BIGSTONE_WORK big_stone_tbl[BIGSTONE_MAX];
  MINISTONE_WORK mini_stone_tbl[MINISTONE_MAX];
};

//======================================================================
//  proto
//======================================================================
static GMEVENT * d06Event_SetPushBigStone(
    FIELDMAP_WORK *fieldmap, MMDL *mmdl, u16 dir );

static void bigStone_Add( D06_WORK *d06,
    BIGSTONE_WORK *work, int gx, int gz );
static void bigStone_Recover(
    D06_WORK *d06, BIGSTONE_WORK *work, MMDL *mmdl );
static BOOL bigStone_CheckElectoricRockCore(
    D06_WORK *d06, BIGSTONE_WORK *work, u16 dir, u16 range, int *outLength );
static int bigStone_CheckElectoricRock(
    D06_WORK *d06, BIGSTONE_WORK *work, u16 dir );
static int bigStone_CheckGirthElectoricRock(
    D06_WORK *d06, BIGSTONE_WORK *work );

static void miniStone_Add( D06_WORK *d06,
    MINISTONE_WORK *work, int gx, int gy, int gz );
static void miniStone_SetShake( FIELDMAP_WORK *fieldmap, int no );

static MMDL * d06_AddMMdl( FIELDMAP_WORK *fieldmap, int gx, int gz );
static D06_WORK * d06_getFieldMapD06Work( FIELDMAP_WORK *fieldmap );
static int d06_ZoneID_to_GimmickID( u32 zone_id );

//======================================================================
//  cdatファイルデータインデックス関連
//======================================================================
static const STONE_INDEX * getBigStoneIndex( u32 zone_id )
{
  u32 i = 0;
  const STONE_INDEX *index = (const STONE_INDEX*)data_d06BigStoneZoneIndex;
  
  for( ; i < BIGSTONE_ZONECOUNT; i++, index++ ){
    if( index->zone_id == zone_id ){
      return( index );
    }
  }

  GF_ASSERT( 0 );
  index = (const STONE_INDEX*)data_d06BigStoneZoneIndex;
  return( index );
}

static const STONE_POS * getBigStonePosData( u32 zone_id )
{
  const STONE_INDEX *index = getBigStoneIndex( zone_id );
  return( index->stone_pos );
}

static u32 getBigStonePosMax( u32 zone_id )
{
  const STONE_INDEX *index = getBigStoneIndex( zone_id );
  return( index->stone_max );
}

static const STONE_INDEX * getMiniStoneIndex( u32 zone_id )
{
  u32 i = 0;
  const STONE_INDEX *index = (const STONE_INDEX *)data_d06MiniStoneZoneIndex;
  
  for( ; i < MINISTONE_ZONECOUNT; i++, index++ ){
    if( index->zone_id == zone_id ){
      return( index );
    }
  }

  GF_ASSERT( 0 );
  index = (const STONE_INDEX *)data_d06MiniStoneZoneIndex;
  return( index );
}

static const STONE_POS * getMiniStonePosData( u32 zone_id )
{
  const STONE_INDEX *index = getMiniStoneIndex( zone_id );
  return( index->stone_pos );
}

static u32 getMiniStonePosMax( u32 zone_id )
{
  const STONE_INDEX *index = getMiniStoneIndex( zone_id );
  return( index->stone_max );
}

//======================================================================
//  D06電気洞窟
//======================================================================
//--------------------------------------------------------------
/**
 * ギミック　D06電気洞窟　初期化
 * @param fieldmap  FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void D06_GIMMICK_Setup( FIELDMAP_WORK *fieldmap )
{
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK *gmk_work = GAMEDATA_GetGimmickWork( gdata );
  FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
  u32 zone_id = FIELDMAP_GetZoneID( fieldmap );
  int gmk_id = d06_ZoneID_to_GimmickID( zone_id );
  D06GMK_WORK *d06gmk = GIMMICKWORK_Get( gmk_work, gmk_id );
  D06_WORK *d06 = GFL_HEAP_AllocClearMemory( heap_id, sizeof(D06_WORK) );
  
  { //ワーク初期化
    d06gmk->d06_work = d06;
    d06->d06gmk_work = d06gmk;
    d06->fieldmap = fieldmap;
    d06->zone_id = zone_id;
  }
  
  { //エフェクトセットアップ
    FLDEFF_PROCID id = FLDEFF_PROCID_D06DENKI;
    FLDEFF_CTRL_RegistEffect( fectrl, &id, 1 );
  }
  
  if( d06gmk->init_flag == FALSE ){ //浮遊石セットアップ
    u32 i;
    u32 max = getBigStonePosMax( d06->zone_id );
    const STONE_POS *stone_pos = getBigStonePosData( d06->zone_id );
    
    for( i = 0; i < max; i++, stone_pos++ ){
      bigStone_Add( d06, &d06->big_stone_tbl[i],
        stone_pos->x, stone_pos->z );
    }
  }else{
    u32 i = 0, no = 0;
    MMDL *mmdl;
    MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
    
	  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&i) ){
      if( MMDL_GetOBJID(mmdl) == MMDL_ID_GIMMICK ){
        bigStone_Recover( d06, &d06->big_stone_tbl[no], mmdl );
        no++;
        if( no >= BIGSTONE_MAX ){
          break;
        }
      }
    }
  }
  
  { //小さな浮遊石
    u32 i;
    u32 max = getMiniStonePosMax( d06->zone_id );
    const STONE_POS *stone_pos = getMiniStonePosData( d06->zone_id );

    for( i = 0; i < max; i++, stone_pos++ ){
      miniStone_Add( d06, &d06->mini_stone_tbl[i], 
          stone_pos->x, stone_pos->y, stone_pos->z );
    }
  }

  d06gmk->init_flag = TRUE;
}

//--------------------------------------------------------------
/**
 * ギミック　D06電気洞窟　終了
 * @param fieldmap  FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void D06_GIMMICK_End( FIELDMAP_WORK *fieldmap )
{
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK *gmk_work = GAMEDATA_GetGimmickWork( gdata );
  FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
  u32 zone_id = FIELDMAP_GetZoneID( fieldmap );
  int gmk_id = d06_ZoneID_to_GimmickID( zone_id );
  D06GMK_WORK *d06gmk = GIMMICKWORK_Get( gmk_work, gmk_id );
  
  GFL_HEAP_FreeMemory( d06gmk->d06_work );
}

//--------------------------------------------------------------
/**
 * ギミック　D06電気洞窟　動作
 * @param fieldmap  FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void D06_GIMMICK_Move( FIELDMAP_WORK *fieldmap )
{
}

//======================================================================
//  電気洞窟　イベント
//======================================================================
//--------------------------------------------------------------
/**
 * 電気洞窟　イベントチェック
 * @param fieldmap  FIELDMAP_WORK
 * @param dir DIR_UP等
 * @retval GMEVENT NULL=イベントなし
 */
//--------------------------------------------------------------
GMEVENT * D06_GIMMICK_CheckPushEvent( FIELDMAP_WORK *fieldmap, u16 dir )
{
  u32 i;
  s16 gx,gz;
  VecFx32 pos;
  MMDL *mmdl;
  MMDLSYS *mmdlsys;
  FIELD_PLAYER *fld_player;
  GMEVENT *event = NULL;
  D06_WORK *d06 = d06_getFieldMapD06Work( fieldmap );
  
  fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  FIELD_PLAYER_GetPos( fld_player, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  gx = SIZE_GRID_FX32( pos.x );
  gz = SIZE_GRID_FX32( pos.z );
  
  KAGAYA_Printf( "GX=%d,GZ=%d\n", gx, gz );
  
  {
    u32 max = getMiniStonePosMax( d06->zone_id );
    const STONE_POS *stone_pos = getMiniStonePosData( d06->zone_id );
    
    for( i = 0; i < max; i++, stone_pos++ ){
      if( stone_pos->x == gx && stone_pos->z == gz ){
        miniStone_SetShake( fieldmap, i );
      }
    }
  }
  
  mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
  
  mmdl = MMDLSYS_SearchGridPos( mmdlsys, gx, gz, FALSE );
  
  if( mmdl != NULL ){
    if( MMDL_GetOBJID(mmdl) == MMDL_ID_GIMMICK ){
      event = d06Event_SetPushBigStone( fieldmap, mmdl, dir );
    }
  }
  
  return( event );
}

//======================================================================
//  電気洞窟　イベント　大きな浮遊石移動
//======================================================================
//--------------------------------------------------------------
/// EVENT_BIGSTONE_PUSH_WORK
//--------------------------------------------------------------
typedef struct
{
  u16 dir;
  u16 stone_no;
  int length;
  BIGSTONE_WORK *big_stone;
  D06_WORK *d06_work;
}EVENT_BIGSTONE_PUSH_WORK;

static GMEVENT_RESULT event_BigStonePush(
    GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT event_BigStoneShake(
    GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * 電気洞窟　イベントセット　大きな浮遊石押し
 * @param fieldmap FIELDMAP_WORK
 * @param mmdl 対象のMMDL*
 * @param dir 押した方向
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
static GMEVENT * d06Event_SetPushBigStone(
    FIELDMAP_WORK *fieldmap, MMDL *mmdl, u16 dir )
{
  u16 no;
  int length;
  GMEVENT *event;
  BIGSTONE_WORK *big_stone;
  EVENT_BIGSTONE_PUSH_WORK *work;
  D06_WORK *d06 = d06_getFieldMapD06Work( fieldmap );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldmap );
  
  for( no = 0; no < BIGSTONE_MAX; no++ ){
    if( d06->big_stone_tbl[no].mmdl == mmdl ){
      break;
    }
  }
  
  if( no >= BIGSTONE_MAX ){
    GF_ASSERT( 0 );
    no = 0;
  }
  
  big_stone = &d06->big_stone_tbl[no];

  if( bigStone_CheckGirthElectoricRock(d06,big_stone) == TRUE ){
    return( NULL );
  }

  length = bigStone_CheckElectoricRock( d06, big_stone, dir );
  
  if( length == 0 ){
    event = GMEVENT_Create( gsys, NULL,
        event_BigStoneShake, sizeof(EVENT_BIGSTONE_PUSH_WORK) );
  }else{
    event = GMEVENT_Create( gsys, NULL,
        event_BigStonePush, sizeof(EVENT_BIGSTONE_PUSH_WORK) );
  }
  
  work = GMEVENT_GetEventWork( event );
  MI_CpuClear( work, sizeof(EVENT_BIGSTONE_PUSH_WORK) );
  
  work->dir = dir;
  work->d06_work = d06;
  work->stone_no = no;
  work->big_stone = big_stone;
  work->length = length;
  return( event );
}

//--------------------------------------------------------------
/**
 * イベント　大きな浮遊石押し
 * @param event GMEVENT
 * @param seq event seq
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_BigStonePush(
    GMEVENT *event, int *seq, void *wk )
{
  EVENT_BIGSTONE_PUSH_WORK *work = wk;
  FIELDMAP_WORK *fieldmap = work->d06_work->fieldmap;
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *m_jiki = FIELD_PLAYER_GetMMdl( fld_player );
  
  switch( (*seq) ){
  case 0:
    if( MMDL_CheckPossibleAcmd(m_jiki) == TRUE ){
      MMDL_SetAcmd( m_jiki,
          MMDL_ChangeDirAcmdCode(work->dir,AC_STAY_WALK_U_16F) );
      PMSND_PlaySE( SEQ_SE_FLD_63 );
      (*seq)++;
    }
    break;
  case 1:
    if( MMDL_CheckPossibleAcmd(work->big_stone->mmdl) == TRUE ){
      if( work->length > 0 ){
        u16 ac = AC_WALK_U_16F;
        
        if( work->length <= 1 ){
          ac = AC_WALK_U_4F; 
        }else if( work->length <= 2 ){
          ac = AC_WALK_U_4F;
        }else if( work->length <= 4 ){
          ac = AC_WALK_U_8F;
        }
        
        MMDL_SetAcmd( work->big_stone->mmdl,
          MMDL_ChangeDirAcmdCode(work->dir,ac) );
        work->length--;
      }else{
        PMSND_PlaySE( SEQ_SE_FLD_50 );
        (*seq)++;
      }
    }
    break;
  case 2:
    if( MMDL_CheckEndAcmd(m_jiki) == TRUE ){
      MMDL_EndAcmd( work->big_stone->mmdl );
      MMDL_EndAcmd( m_jiki );
      FLDEFF_D06DENKI_BIGSTONE_OFFShakeV( work->big_stone->eff_task );
      return( GMEVENT_RES_FINISH );
    }
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * イベント　大きな浮遊石揺らし
 * @param event GMEVENT
 * @param seq event seq
 * @param wk event work
 * @retval 
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_BigStoneShake(
    GMEVENT *event, int *seq, void *wk )
{
  EVENT_BIGSTONE_PUSH_WORK *work = wk;
  FIELDMAP_WORK *fieldmap = work->d06_work->fieldmap;
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *m_jiki = FIELD_PLAYER_GetMMdl( fld_player );
  
  switch( (*seq) ){
  case 0:
    if( MMDL_CheckPossibleAcmd(m_jiki) == TRUE ){
      MMDL_SetAcmd( m_jiki,
          MMDL_ChangeDirAcmdCode(work->dir,AC_STAY_WALK_U_16F) );
      PMSND_PlaySE( SEQ_SE_WALL_HIT );
      (*seq)++;
    }
  case 1:
    FLDEFF_D06DENKI_BIGSTONE_ONShakeH( work->big_stone->eff_task );
    (*seq)++;
    break;
  case 2:
    if( FLDEFF_D06DENKI_BIGSTONE_CheckShakeH(
          work->big_stone->eff_task) == FALSE ){
      break;
    }
    (*seq)++;
  case 3:
    if( MMDL_CheckEndAcmd(m_jiki) == TRUE ){
      MMDL_EndAcmd( m_jiki );
      return( GMEVENT_RES_FINISH );
    }
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  電気洞窟　大きな浮遊石
//======================================================================
//--------------------------------------------------------------
/**
 * 大きな浮遊石　追加
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @param gx 追加するグリッド座標X
 * @param gz 追加するグリッド座標Z
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigStone_Add( D06_WORK *d06,
    BIGSTONE_WORK *work, int gx, int gz )
{
  VecFx32 pos;
  
  work->mmdl = d06_AddMMdl( d06->fieldmap, gx, gz );
  MMDL_GetVectorPos( work->mmdl, &pos );
  work->eff_task = FLDEFF_D06DENKI_BIGSTONE_Add(
      FIELDMAP_GetFldEffCtrl(d06->fieldmap), &pos, work->mmdl );
  if( bigStone_CheckGirthElectoricRock(d06,work) == TRUE ){
    FLDEFF_D06DENKI_BIGSTONE_OFFShakeV( work->eff_task );
  }
}

//--------------------------------------------------------------
/**
 * 大きな浮遊石　復帰
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigStone_Recover(
    D06_WORK *d06, BIGSTONE_WORK *work, MMDL *mmdl )
{
  VecFx32 pos;
  work->mmdl = mmdl;
  MMDL_GetVectorPos( mmdl, &pos );
  work->eff_task = FLDEFF_D06DENKI_BIGSTONE_Add(
      FIELDMAP_GetFldEffCtrl(d06->fieldmap), &pos, work->mmdl );
  if( bigStone_CheckGirthElectoricRock(d06,work) == TRUE ){
    FLDEFF_D06DENKI_BIGSTONE_OFFShakeV( work->eff_task );
  }
}

//--------------------------------------------------------------
/**
 * 大きな浮遊石　移動先に電気岩があるか
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @param dir DIR_UP等
 * @param range 検索するグリッド範囲
 * @param outLength 電気岩までのグリッド距離
 * @retval BOOL TRUE=電気岩がある FALSE=ない
 */
//--------------------------------------------------------------
static BOOL bigStone_CheckElectoricRockCore(
    D06_WORK *d06, BIGSTONE_WORK *work, u16 dir, u16 range, int *outLength )
{
  MMDL *mmdl = work->mmdl;
  u32 ret;
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attr_val;
  u16 i = 0;
  BOOL hit = FALSE;
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( d06->fieldmap );
  s16 gx = MMDL_GetGridPosX( mmdl );
  s16 gy = MMDL_GetGridPosY( mmdl );
  s16 gz = MMDL_GetGridPosZ( mmdl );
  
  do{
    MMDL_TOOL_AddDirGrid( dir, &gx, &gz, 1 );
    ret = MMDL_HitCheckMoveCurrent( mmdl, gx, gy, gz, dir );
    ret &= ~MMDL_MOVEHITBIT_LIM;
    
    if( ret != MMDL_MOVEHITBIT_NON )
    {
      if( (ret & MMDL_MOVEHITBIT_ATTR) )
      {
        MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
        pos.y = MMDL_GetVectorPosY( mmdl );
        attr = MAPATTR_GetAttribute( mapper, &pos );
        
        if( attr != MAPATTR_ERROR )
        {
          attr_val = MAPATTR_GetAttrValue( attr );
          
          if( MAPATTR_VALUE_CheckElecRock(attr_val) == TRUE )
          {
            hit = TRUE;
          }
        }
      }
      break;
    }
    
    i++;
  }while( i < range );
  
  if( hit == FALSE ){
    i = 0;
  }
  
  *outLength = i;
  return( hit );
}

//--------------------------------------------------------------
/**
 * 大きな浮遊石　移動先に電気岩があるか
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @param dir DIR_UP等
 * @retval int 電気岩までのグリッド距離。0=ない、もしくは隣にある。
 * @attention 32グリッド以上はエラーとする。
 */
//--------------------------------------------------------------
static int bigStone_CheckElectoricRock(
    D06_WORK *d06, BIGSTONE_WORK *work, u16 dir )
{
  int length;
  bigStone_CheckElectoricRockCore( d06, work, dir, 32, &length );
  return( length );
}

//--------------------------------------------------------------
/**
 * 大きな浮遊石　周囲に電気岩があるか
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @retval BOOL TRUE=ある
 */
//--------------------------------------------------------------
static int bigStone_CheckGirthElectoricRock(
    D06_WORK *d06, BIGSTONE_WORK *work )
{
  u16 dir;
  int length;
  for( dir = DIR_UP; dir < DIR_MAX4; dir++ ){
    if( bigStone_CheckElectoricRockCore(d06,work,dir,1,&length) == TRUE ){
      return( TRUE );
    }
  }
  return( FALSE );
}

//======================================================================
//  電気洞窟　小さな浮遊石
//======================================================================
//--------------------------------------------------------------
/**
 * 小さな浮遊石　追加
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @param gx 追加するグリッド座標X
 * @param gz 追加するグリッド座標Z
 * @retval nothing
 */
//--------------------------------------------------------------
static void miniStone_Add( D06_WORK *d06,
    MINISTONE_WORK *work, int gx, int gy, int gz )
{
  VecFx32 pos;
  MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
  pos.y = GRID_SIZE_FX32( gy ) + GRID_HALF_FX32;
  work->eff_task = FLDEFF_D06DENKI_MINISTONE_Add(
      FIELDMAP_GetFldEffCtrl(d06->fieldmap), &pos );
}

//--------------------------------------------------------------
/**
 * 小さな浮遊石　アニメセット
 * @param d06 D06_WORK
 * @param work BIGSTONE_WORK*
 * @param gx 追加するグリッド座標X
 * @param gz 追加するグリッド座標Z
 * @retval nothing
 */
//--------------------------------------------------------------
static void miniStone_SetShake( FIELDMAP_WORK *fieldmap, int no )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK *gmk_work = GAMEDATA_GetGimmickWork( gdata );
  u32 zone_id = FIELDMAP_GetZoneID( fieldmap );
  int gmk_id = d06_ZoneID_to_GimmickID( zone_id );
  D06GMK_WORK *d06gmk = GIMMICKWORK_Get( gmk_work, gmk_id );
  D06_WORK *d06 = d06gmk->d06_work;
  MINISTONE_WORK *mini_stone = &d06->mini_stone_tbl[no];
  FLDEFF_D06DENKI_MINISTONE_SetShake( mini_stone->eff_task );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * 大きな浮遊石　動作モデル追加
 * @param fieldmap FIELDMAP_WORK
 * @param gx 追加するグリッド座標X
 * @param gz 追加するグリッド座標Z
 * @retval MMDL*
 */
//--------------------------------------------------------------
static MMDL * d06_AddMMdl( FIELDMAP_WORK *fieldmap, int gx, int gz )
{
  MMDL *mmdl;
  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
  int zone_id = FIELDMAP_GetZoneID( fieldmap );
  mmdl = MMDLSYS_AddMMdlParam( mmdlsys, gx, gz, DIR_UP,
      MMDL_ID_GIMMICK, NONDRAW, MV_DMY, zone_id );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * fieldmap -> d06ワーク
 * @param fieldmap  FIELDMAP_WORK
 * @retval D06_WORK*
 */
//--------------------------------------------------------------
static D06_WORK * d06_getFieldMapD06Work( FIELDMAP_WORK *fieldmap )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK *gmk_work = GAMEDATA_GetGimmickWork( gdata );
  u32 zone_id = FIELDMAP_GetZoneID( fieldmap );
  int gmk_id = d06_ZoneID_to_GimmickID( zone_id );
  D06GMK_WORK *d06gmk = GIMMICKWORK_Get( gmk_work, gmk_id );
  return( d06gmk->d06_work );
}

//--------------------------------------------------------------
/**
 * ゾーンID -> ギミックID
 * @param
 * @retval
 */
//--------------------------------------------------------------
static int d06_ZoneID_to_GimmickID( u32 zone_id )
{
  switch( zone_id ){
  case ZONE_ID_D06R0101:
    return( FLD_GIMMICK_D06R0101 );
  case ZONE_ID_D06R0201:
    return( FLD_GIMMICK_D06R0201 );
  case ZONE_ID_D06R0301:
    return( FLD_GIMMICK_D06R0301 );
  }
  GF_ASSERT( 0 );
  return( FLD_GIMMICK_D06R0101 );
}
