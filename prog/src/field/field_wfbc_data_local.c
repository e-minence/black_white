//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data_local.c
 *	@brief  White Forest  Black City 基本データ 非常駐 [OVERLAY FIELD_INIT]
 *	@author	tomoya takahashi
 *	@date		2010.01.12
 *
 *	モジュール名：FIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"


#include "system/main.h"

#include "arc/arc_def.h"
#include "arc/field_wfbc_data.naix"

#include "system/gfl_use.h"

#include "field/field_wfbc_data.h"
#include "field/zonedata.h"

#include "field_status_local.h"

#include "eventwork_def.h"

#include "field_comm/intrude_work.h"


#include "net/dwc_tool.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	人物基本情報
//=====================================
static const MMDL_HEADER sc_DEFAULT_HEADER = 
{
	0,		///<識別ID
	0,	  ///<表示するOBJコード
	0,	  ///<動作コード
	0,	  ///<イベントタイプ
	0,	  ///<イベントフラグ
	0,	  ///<イベントID
	0,		///<指定方向
	0,		///<指定パラメタ 0
	0,		///<指定パラメタ 1
	0,		///<指定パラメタ 2
	0,		///<X方向移動制限
	0,		///<Z方向移動制限
};

//-------------------------------------
///	アイテム基本情報
//=====================================
static const MMDL_HEADER sc_DEFAULT_ITEM_HEADER = 
{
	0,		///<識別ID
	MONSTERBALL,	  ///<表示するOBJコード
	MV_DMY,	  ///<動作コード
	EV_TYPE_NORMAL,	  ///<イベントタイプ
	0,	  ///<イベントフラグ
	0,	  ///<イベントID
	DIR_DOWN,		///<指定方向
	0,		///<指定パラメタ 0
	0,		///<指定パラメタ 1
	0,		///<指定パラメタ 2
	0,		///<X方向移動制限
	0,		///<Z方向移動制限
};


//-----------------------------------------------------------------------------
/**
 day*					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static const FIELD_WFBC_PEOPLE_DATA* WFBC_CORE_GetNotUseNpc_HitPercent( const FIELD_WFBC_CORE* cp_wk, u32 hit_percent, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader, int* p_setidx );
static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx );
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx );
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id );


// 人データをソート
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID );
// 人データを押し出し設定
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, const FIELD_WFBC_CORE_PEOPLE* cp_people, BOOL parent_set );
// 配列がいっぱいかチェック
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array );
// 人を入れる
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people );

// 空いている人物ワークを取得
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array );


//-------------------------------------
///	人物情報
//=====================================
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus, u16 mood, BOOL parent_set );
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people );
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add );
static void WFBC_CORE_People_SetMood( FIELD_WFBC_CORE_PEOPLE* p_people, int mood );


//-------------------------------------
///	MMDLワークから、WFBC　NPCIDを取得
//=====================================
u32 FIELD_WFBC_CORE_GetMMdlNpcID( const MMDL* cp_mmdl )
{
  u32 objid;
  objid = MMDL_GetOBJID( cp_mmdl );
  objid = FIELD_WFBC_OBJID_GET( objid );

  return objid;
}


//-------------------------------------
///	FIELD_WFBC_CORE用関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief ノーマルマップの情報をクリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearNormal( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  裏世界の人情報をクリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearBack( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  街の情報を設定する
 *
 *	@param	p_wk          ワーク
 *	@param  cp_mystatus   まいステータス
 *	@param  heapID        テンポラリバッファ用ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, HEAPID tmp_heapID )
{
  int i;
  int pos_idx;
  int npc_hit_per;
  int set_npc_idx;
  FIELD_WFBC_CORE_PEOPLE* p_people;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_data;
  u32 mood;
  BOOL parent_set;
  int hit_percent_total_num = FIELD_WFBC_HIT_PERCENT_MAX;
  GF_ASSERT( p_wk );

#ifdef PM_DEBUG
  OS_SetPrintOutput_Arm9( 2 );
#endif
  
  TOMOYA_Printf( "WFBC_SETUP\n" );
  
  p_wk->data_in = TRUE;
#if PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
#else   // PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_WHITE_FOREST; 
#endif  // PM_VERSION == VERSION_BLACK

  p_wk->random_no = MyStatus_GetID( cp_mystatus );

  p_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, GFL_HEAP_LOWID(tmp_heapID) );
  cp_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_loader );

  // 人を設定する
  for( i=0; i<FIELD_WFBC_INIT_PEOPLE_NUM; i++ )
  {
    // 設定位置と人を選ぶ
    npc_hit_per = GFUser_GetPublicRand( hit_percent_total_num );

    p_people  = WFBC_CORE_GetClearPeopleArray( p_wk->people );
    cp_data = WFBC_CORE_GetNotUseNpc_HitPercent( p_wk, npc_hit_per, p_loader, &set_npc_idx );

    if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
      mood = cp_data->mood_bc;
      parent_set = TRUE;
    }else{
      mood = cp_data->mood_wf;
      parent_set = FALSE;
    }
    TOMOYA_Printf( "setUp idx:%d\n", set_npc_idx );
    WFBC_CORE_SetUpPeople( p_people, set_npc_idx, cp_mystatus, mood, parent_set );

    // 相対値を計算しなおす
    hit_percent_total_num -= cp_data->hit_percent;
    GF_ASSERT( hit_percent_total_num > 0 );
  }

  // サブ面にも3体おく
  for( i=0; i<FIELD_WFBC_INIT_BACK_PEOPLE_NUM; i++ )
  {
    // 設定位置と人を選ぶ
    npc_hit_per = GFUser_GetPublicRand( hit_percent_total_num );

    p_people  = WFBC_CORE_GetClearPeopleArray( p_wk->back_people );
    cp_data = WFBC_CORE_GetNotUseNpc_HitPercent( p_wk, npc_hit_per, p_loader, &set_npc_idx );
    
    mood = 0;
    WFBC_CORE_SetUpPeople( p_people, set_npc_idx, cp_mystatus, mood, parent_set );

    // 相対値を計算しなおす
    hit_percent_total_num -= cp_data->hit_percent;
    GF_ASSERT( hit_percent_total_num > 0 );
  }

  FIELD_WFBC_PEOPLE_DATA_Delete( p_loader );

#ifdef PM_DEBUG
  OS_SetPrintOutput_Arm9( 0 );
#endif
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBCデータの整合性チェック
 *
 *	@param	cp_wk         ワーク
 *
 *	@retval TRUE    正常
 *	@retval FALSE   不正
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );
  
  if( (cp_wk->data_in == TRUE) && (cp_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    return FALSE;
  }
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( &cp_wk->people[i] ) == FALSE )
    {
      return FALSE;
    }
  }

  // OK
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  データ調整  不正データの場合は、正常な情報に書き換えます。
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus )
{
  int i;

  GF_ASSERT( p_wk );

  // TRUE以上の値がはいっているばあい。
  if( p_wk->data_in > TRUE ){
    p_wk->data_in = TRUE;
  }
  
  // 一応データがない状態も管理しておく
  if( (p_wk->data_in == FALSE) && (p_wk->type != FIELD_WFBC_CORE_TYPE_MAX) )
  {
    p_wk->type = FIELD_WFBC_CORE_TYPE_MAX;
    TOMOYA_Printf( "OverWrite max\n" );
  }

  // 街タイプの調整
  if( (p_wk->data_in == TRUE) && (p_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    u32 pm_version, trainerID;

    pm_version  = MyStatus_GetRomCode( cp_mystatus );
    trainerID   = MyStatus_GetID( cp_mystatus );
    
    // 不正データ 自分のROMコードに合わせる
    if( Intrude_GetIntrudeRomVersion( pm_version, trainerID ) == VERSION_BLACK ){
      p_wk->type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
    }else{
      p_wk->type = FIELD_WFBC_CORE_TYPE_WHITE_FOREST;
    }

    TOMOYA_Printf( "OverWrite city\n" );
  }

  // 人物情報の調整
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Management( &p_wk->people[i] );
  }
}

// データの有無   不正データの場合、FALSEを返します。
//----------------------------------------------------------------------------
/**
 *	@brief  データの有無チェック  不正データの場合、FALSEを返します。
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE  データあり
 *	@retval FALSE データなし
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }

  if( FIELD_WFBC_CORE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人の数を返す
 *
 *	@param	cp_wk   ワーク
 *	@param  mapmode MAPMODE 進入か通常か
 *
 *	@return 人の数
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk, MAPMODE mapmode )
{
  int i;
  int num;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_array;
  GF_ASSERT( cp_wk );

  if( mapmode == MAPMODE_NORMAL )
  { 
    cp_people_array = cp_wk->people;
  }
  else
  {
    cp_people_array = cp_wk->back_people;
  }

  num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_people_array[i] ) )
    {
      num ++;
    }
  }
  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値でソートする
 *
 *	@param	p_wk    ワーク
 *	@param  heapID  ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID )
{
  WFBC_CORE_SortPeopleArray( p_wk->people, heapID );
  WFBC_CORE_SortPeopleArray( p_wk->back_people, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  1日の切り替え管理
 *
 *	@param	p_wk      ワーク
 *	@param  diff_day  経過日数
 *	@param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcOneDataStart( GAMEDATA * gamedata, s32 diff_day, HEAPID heapID )
{
  int i;
  BOOL result;
  FIELD_WFBC_CORE* p_wk;
  FIELD_WFBC_CORE_ITEM* p_item;
  FIELD_WFBC_EVENT* p_event;
  u16 random;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  PLAYER_WORK* p_player = GAMEDATA_GetMyPlayerWork( gamedata );
  u32 zone_id = PLAYERWORK_getZoneID( p_player );
  u32 back_people_num;
  u32 back_push_num;

  p_wk = GAMEDATA_GetMyWFBCCoreData( gamedata );
  p_item = GAMEDATA_GetWFBCItemData( gamedata );
  p_event = GAMEDATA_GetWFBCEventData( gamedata );
  

  // 日にちがおかしくないかチェック
  if( diff_day <= 0 )
  {
    // おかしい
    GF_ASSERT(0);
    diff_day = 1;
  }

  if( diff_day > FIELD_WFBC_MOOD_SUB_DAY_MAX )
  {
    diff_day = FIELD_WFBC_MOOD_SUB_DAY_MAX;
  }

  back_people_num = FIELD_WFBC_CORE_GetPeopleNum( p_wk, MAPMODE_INTRUDE );
  back_push_num = 0;
  
  // 全員のMoodをFIELD_WFBC_MOOD_SUB減らす
  // マスクのクリア
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // バトル、各種加算を可能とする。
      p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_INIT;
      
      result = WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_SUB * diff_day );
      
      // 裏がいっぱいなら消さない。
      if( (result) && ((back_people_num + back_push_num) >= FIELD_WFBC_PEOPLE_MAX) ) 
      {
        TOMOYA_Printf( "ＷＦＢＣ　ＮＯＴ　ＣＬＥＡＲ　ＮＰＣ\n" );
        WFBC_CORE_People_SetMood( &p_wk->people[i], 1 );
      }
      // 裏に余裕があるので消す。
      else if( result )
      {

        // もし、自分がＷＦＢＣにいるのであれば、消す処理をスキップする。
        // ＷＦＢＣ以外に行った後に消える。BTS：4235
        if( ZONEDATA_IsWfbcCalcMoodSkip( zone_id ) == FALSE ){
          TOMOYA_Printf( "ＷＦＢＣ　ＴＩＭＥ　ＵＰＤＡＴＥ　ＤＯＮＥ\n" );
        
          // 背面世界側のワークに格納後クリア
          // けつの情報を裏世界に渡す。
          WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[i] );
          FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );

          // スキップ数をカウント
          back_push_num ++;
        }else{

          // データとしていなくなったことになった人のマスクは、すべて落とす。
          // バトルのみ可能
          // Ｍｏｏｄの加算は行わない。
          p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_DUMMYMODE_INIT;

          // スキップ数をカウント
          back_push_num ++;

          TOMOYA_Printf( "ＷＦＢＣ　SKIP　ＮＰＣ\n" );

        }

      }
    }
  }

  // 空いているワークをつめる
  FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_NORMAL );

  // 今いる人の情報を使用して、アイテムを追加
  WFBC_CORE_ITEM_ClaerAll( p_item );


  // 追加登録準備
  p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, GFL_HEAP_LOWID( heapID ) );


  // WFならば、
  if( p_wk->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    // アイテムを配置
    // 新規アイテムを登録する。
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
      {
        // その人の情報を読み込み
        FIELD_WFBC_PEOPLE_DATA_Load( p_people_loader, p_wk->people[i].npc_id );
        cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );
        
        // 配置抽選
        random = GFUser_GetPublicRand(100);
        if( random < cp_people_data->goods_wf_percent )
        {
          // 配置場所を抽選
          random = GFUser_GetPublicRand(FIELD_WFBC_PEOPLE_MAX);

          // そこにアイテムを追加
          FIELD_WFBC_CORE_ITEM_SetItemData( p_item, cp_people_data->goods_wf, random );
        }
      }
    }
    
    // 村長イベントを管理
    {
      u32 people_num;
      u32 rand_index;
      u32 index_count;

      // ポケモンの選定
      people_num = FIELD_WFBC_CORE_GetPeopleNum( p_wk, MAPMODE_NORMAL );
      if( people_num>0 )
      {
        people_num *= FIELD_WFBC_PEOPLE_ENC_POKE_MAX;
        rand_index = GFUser_GetPublicRand( people_num );

        index_count = 0;

        // そのポケモンを探す
        for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
        {
          if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
          {
            // 
            index_count += FIELD_WFBC_PEOPLE_ENC_POKE_MAX;
            if( index_count > rand_index )  // 大きくなったらその中にある
            {
              // その人の情報を読み込み
              FIELD_WFBC_PEOPLE_DATA_Load( p_people_loader, p_wk->people[i].npc_id );
              cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );

              TOMOYA_Printf( "index %d\n", rand_index % FIELD_WFBC_PEOPLE_ENC_POKE_MAX );
              FIELD_WFBC_EVENT_SetWFPokeCatchEventMonsNo( p_event, cp_people_data->enc_monsno[ rand_index % FIELD_WFBC_PEOPLE_ENC_POKE_MAX ] );//BTS:6569対処
              FIELD_WFBC_EVENT_SetWFPokeCatchEventItem( p_event, cp_people_data->goods_wf );
              break;//BTS:6314対処
            }
          }
        }

      }
    }
  }

  FIELD_WFBC_PEOPLE_DATA_Delete( p_people_loader );
}

//----------------------------------------------------------------------------
/**
 *	@brief  マップジャンプでのＷＦＢＣ人物裏移動処理
 *
 *	@param	gamedata  ゲームデータ
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcZoneChange( GAMEDATA * gamedata )
{
  int i;
  BOOL result;
  FIELD_WFBC_CORE* p_wk;
  PLAYER_WORK* p_player = GAMEDATA_GetMyPlayerWork( gamedata );
  u32 zone_id = PLAYERWORK_getZoneID( p_player );

  p_wk = GAMEDATA_GetMyWFBCCoreData( gamedata );


  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      
      result = WFBC_CORE_People_IsGoBack( &p_wk->people[i] );
      if( result )
      {
        // もし、自分がＷＦＢＣにいるのであれば、消す処理をスキップする。
        // ＷＦＢＣ以外に行った後に消える。BTS：4235
        if( ZONEDATA_IsWfbcCalcMoodSkip( zone_id ) == FALSE ){
          TOMOYA_Printf( "ＷＦＢＣ　ＴＩＭＥ　ＵＰＤＡＴＥ　ＤＯＮＥ\n" );
        
          // 背面世界側のワークに格納後クリア
          // けつの情報を裏世界に渡す。
          WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[i] );
          FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
        }
      }
    }
  }
  // 空いているワークをつめる
  FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_NORMAL );
}


//----------------------------------------------------------------------------
/**
 *	@brief  街に入った時の機嫌値計算
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk )
{
  int i;
  
  // 全員のMoodをFIELD_WFBC_MOOD_SUB減らす
  // マスクのクリア
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // 加算チェック
      if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN ) )
      {
        WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_ADD_INTOWN );

        // OFF
        p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  人を足す　押し出し式
 *
 *	@param	p_wk      ワーク  
 *	@param	cp_people 格納する人
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
    WFBC_CORE_PushPeople( p_wk, cp_mystatus, cp_people, TRUE );
  }else{
    WFBC_CORE_PushPeople( p_wk, cp_mystatus, cp_people, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC情報から、MMDLヘッダーを生成する
 *
 *	@param	cp_wk     ワーク
 *	@param	heapID    ヒープID
 *
 *	@retval 動作モデルヘッダーテーブル
 *	@retval NULL  人物はいない
 */
//-----------------------------------------------------------------------------
MMDL_HEADER* FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE* cp_wk, MAPMODE mapmode, HEAPID heapID )
{
  u32 count;
  u32 num;
  MMDL_HEADER* p_buff;
  int i;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  const FIELD_WFBC_CORE_PEOPLE* cp_people_array;
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;
  
  GF_ASSERT( cp_wk );

  heapID = GFL_HEAP_LOWID( heapID );

  num = FIELD_WFBC_CORE_GetPeopleNum( cp_wk, mapmode );

  if( num == 0 )
  {
    return NULL;
  }

  if( mapmode == MAPMODE_NORMAL )
  {
    cp_people_array = cp_wk->people;
  }
  else
  {
    cp_people_array = cp_wk->back_people;
  }


  p_buff = GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_HEADER) * num );

  // ローダー生成
  p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );

  // 位置情報を読み込み
  p_pos = FIELD_WFBC_PEOPLE_POS_Create( p_people_loader, cp_wk->type, heapID );
  
  count = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_people_array[i] ) )
    {
      // 人物として登録
      p_buff[count] = sc_DEFAULT_HEADER;

      FIELD_WFBC_PEOPLE_DATA_Load( p_people_loader, FIELD_WFBC_CORE_PEOPLE_GetNpcID( &cp_people_array[i] ) );
      cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );

      p_buff[count].id          = FIELD_WFBC_OBJID_SET( FIELD_WFBC_CORE_PEOPLE_GetNpcID( &cp_people_array[i] ) );
      p_buff[count].obj_code    = cp_people_data->objid;
      p_buff[count].move_code   = MV_RND;
      
      if( mapmode == MAPMODE_NORMAL )
      {
        if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
          p_buff[count].event_id    = cp_people_data->script_bc;
        }else{
          p_buff[count].event_id    = cp_people_data->script_wf;
        }
      }
      else
      {
        if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY ){
          p_buff[count].event_id    = cp_people_data->script_plc;
        }else{
          p_buff[count].event_id    = cp_people_data->script_plcw;
        }
      }

      MMDLHEADER_SetGridPos( &p_buff[count], p_pos[i].gx, p_pos[i].gz, 0 );
      

      count ++;
    }
  }


  FIELD_WFBC_PEOPLE_POS_Delete( p_pos );

  FIELD_WFBC_PEOPLE_DATA_Delete( p_people_loader );

  return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BCショップ情報を設定
 *
 *	@param	cp_wk     WFBC情報
 *	@param	p_buff    バッファ
 *	@param	p_num     個数格納先
 *	@param  shop_idx  ショップインデックス(0〜4)
 *	@param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetShopData( const FIELD_WFBC_CORE* cp_wk, SHOP_ITEM* p_buff, u16* p_num, u32 shop_idx, HEAPID heapID )
{
  int i;
  u32 people_num;
  int start_idx;
  int end_idx;
  int roop_num;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_load;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  
  GF_ASSERT( cp_wk );
  GF_ASSERT( p_buff );
  GF_ASSERT( p_num );
  GF_ASSERT( shop_idx < FIELD_WFBC_SHOP_MAX );

  people_num = FIELD_WFBC_CORE_GetPeopleNum( cp_wk, MAPMODE_NORMAL );

  // アイテム設定開始と終了設定
  {
    start_idx = shop_idx * FIELD_WFBC_SHOP_ONE_PEOPLE;

    // shop_idx分の人物がいるのか？
    GF_ASSERT( start_idx <= people_num );

    end_idx   = start_idx + FIELD_WFBC_SHOP_ONE_PEOPLE;
    if( end_idx > people_num ){
      end_idx = people_num;
    }
    roop_num = end_idx - start_idx;

    GF_ASSERT( roop_num < SHOP_ITEM_MAX );
  }

  p_people_load = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );
  cp_people_data  = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_load );
  
  // アイテム情報の設定
  (*p_num) = 0;
  for( i=0; i<roop_num; i++ )
  {
    // 情報があるか？
    GF_ASSERT( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[start_idx + i] ) == TRUE );
    
    // 読み込み
    FIELD_WFBC_PEOPLE_DATA_Load( p_people_load, cp_wk->people[start_idx + i].npc_id );

    // アイテムと値段設定
    p_buff[(*p_num)].id     = cp_people_data->goods_bc;
    p_buff[(*p_num)].price  = cp_people_data->goods_bc_money;

    (*p_num)++;
  }

  FIELD_WFBC_PEOPLE_DATA_Delete( p_people_load );
}






//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE用関数
//=====================================

//----------------------------------------------------------------------------
/**
 *	@brief  データ調整  不正データの場合は、正常な情報に書き換えます。
 *
 *	@param	p_wk  人物ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  int i;
  STRCODE eom = GFL_STR_GetEOMCode();
  BOOL ok;


  GF_ASSERT( p_wk );

  // npc_idが範囲内？
  if( p_wk->npc_id >= FIELD_WFBC_NPCID_MAX ){
    FIELD_WFBC_CORE_PEOPLE_Clear( p_wk );
    TOMOYA_Printf("OverWrite npc_id\n");
  }

  // 機嫌値が最大値以上？
  if( p_wk->mood > FIELD_WFBC_MOOD_MAX ){
    FIELD_WFBC_CORE_PEOPLE_Clear( p_wk );
    TOMOYA_Printf("OverWrite mood\n");
  }

  // 親の名前に終端コードがある？
  if( p_wk->parent_in ){
    ok = FALSE;
    for( i=0; i<(PERSON_NAME_SIZE + EOM_SIZE); i++ ){
      if( p_wk->parent[i] == eom ){
        ok = TRUE;
      }
    }

    if( ok == FALSE ){
      FIELD_WFBC_CORE_PEOPLE_Clear( p_wk );
      TOMOYA_Printf("OverWrite parent name\n");
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  話しかけたときの計算
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_CalcTalk( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK ) )
  {
    // 計算
    WFBC_CORE_People_AddMood( p_wk, FIELD_WFBC_MOOD_ADD_TALK );

    // OFF
    p_wk->one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルしたことを設定
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_SetBattle( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  // OFF
  p_wk->one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_BATTLE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルが可能なのかチェック
 *
 *	@param	cp_wk   ワーク
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsBattle( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  // ONならバトル可能
  if( FIELD_WFBC_ONEDAY_MSK_CHECK( cp_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_BATTLE ) )
  {
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  親データの設定
 *
 *	@param	p_wk        ワーク
 *	@param  cp_mystatus 親の情報
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_SetParentData( FIELD_WFBC_CORE_PEOPLE* p_wk, const MYSTATUS* cp_mystatus )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_mystatus );

  p_wk->parent_in = TRUE;
  p_wk->parent_id = MyStatus_GetID( cp_mystatus );
  GFL_STD_MemCopy( MyStatus_GetMyName( cp_mystatus ), p_wk->parent, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  親の名前を取得
 *
 *	@param	cp_wk   ワーク
 *	@param	p_buff  バッファ
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_GetParentName( const FIELD_WFBC_CORE_PEOPLE* cp_wk, STRCODE* p_buff )
{
  GF_ASSERT( cp_wk );
  GFL_STD_MemCopy( cp_wk->parent, p_buff, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  親情報があるかチェック
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE  ある
 *	@retval FALSE ない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsParentIn( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->parent_in;
}

//----------------------------------------------------------------------------
/**
 *	@brief  親のプレイヤーIDを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return プレイヤーID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetParentID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->parent_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  NPCIDを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return NPCID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetNpcID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->npc_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値が、連れて行かれてしまいそうになる値か？
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE    はい
 *	@retval FALSE   いいえ
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsMoodTakes( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  if( cp_wk->mood > FIELD_WFBC_MOOD_TAKES )
  {
    return FALSE;
  }
  return TRUE;
}





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					WFBC人物情報へのアクセス
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	人物情報ローダー
//=====================================
struct _FIELD_WFBC_PEOPLE_DATA_LOAD {
  ARCHANDLE* p_handle;
  u32 npc_id;
  FIELD_WFBC_PEOPLE_DATA buff;
};

//----------------------------------------------------------------------------
/**
 *	@brief  人物情報ローダー生成
 *
 *	@param	npc_id    NPCID
 *	@param	heapID 
 *
 *	@return ローダー
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_PEOPLE_DATA_LOAD* FIELD_WFBC_PEOPLE_DATA_Create( u32 npc_id, HEAPID heapID )
{
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;

  p_loader = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC_PEOPLE_DATA_LOAD) );

  p_loader->p_handle = GFL_ARC_OpenDataHandle( ARCID_WFBC_PEOPLE, heapID );

  // 読み込み
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );
  GFL_ARC_LoadDataByHandle( p_loader->p_handle, npc_id, &p_loader->buff );
  p_loader->npc_id = npc_id;
  
  return p_loader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物情報ローダー破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_DATA_Delete( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk )
{
  GFL_ARC_CloseDataHandle( p_wk->p_handle );
  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物情報再読みこみ
 *
 *	@param	p_wk      ワーク
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_DATA_Load( FIELD_WFBC_PEOPLE_DATA_LOAD* p_wk, u32 npc_id )
{
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );
  GFL_ARC_LoadDataByHandle( p_wk->p_handle, npc_id, &p_wk->buff );
  p_wk->npc_id = npc_id;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物情報  取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return データバッファ
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_DATA_GetData( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk )
{
  GF_ASSERT( cp_wk );
  return &cp_wk->buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物情報  読み込まれているNPCIDを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return NPCID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_DATA_GetLoadNpcID( const FIELD_WFBC_PEOPLE_DATA_LOAD* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->npc_id;
}




//----------------------------------------------------------------------------
/**
 *	@brief  人物位置情報を読み込む
 *
 *  @param  p_loader  人物ローダーを使う
 *  @param  type      タイプ
 *	@param	heapID    ヒープID
 *
 *	@return 位置情報
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_Create( FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader, FIELD_WFBC_CORE_TYPE type, HEAPID heapID )
{
  return GFL_ARC_LoadDataAllocByHandle( p_loader->p_handle, NARC_field_wfbc_data_wb_wfbc_block_bc_pos + type, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物位置情報の破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_PEOPLE_POS_Delete( FIELD_WFBC_CORE_PEOPLE_POS* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  インデックスの位置情報を返す
 *
 *	@param	cp_wk ワーク
 *	@param	index インデックス
 *
 *	@return 位置情報
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_GetIndexData( const FIELD_WFBC_CORE_PEOPLE_POS* cp_wk, u32 index )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );

  return &cp_wk[index];
}

//----------------------------------------------------------------------------
/**
 *	@brief  アイテムのインデックスの位置情報を返す
 *
 *	@param	cp_wk   ワーク
 *	@param	index   インデックス
 *
 *	@return 位置情報
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_CORE_PEOPLE_POS* FIELD_WFBC_PEOPLE_POS_GetIndexItemPos( const FIELD_WFBC_CORE_PEOPLE_POS* cp_wk, u32 index )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );

  return &cp_wk[index + FIELD_WFBC_PEOPLE_MAX];
}

//----------------------------------------------------------------------------
/**
 *	@brief  配置アイテム数を数える
 *
 *	@param	p_wk  ワーク
 *
 *	@return アイテム数
 */
//-----------------------------------------------------------------------------
u32 WFBC_CORE_ITEM_GetNum( const FIELD_WFBC_CORE_ITEM* cp_wk )
{
  int i;
  int count = 0;
  
  // アイテムなし
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->scr_item[i] != FIELD_WFBC_ITEM_NONE )
    {
      count ++;
    }
  }
  return count;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ITEM情報を設定
 *
 *	@param	p_wk      ワーク
 *	@param	scr_item  アイテムゲットスクリプトナンバー
 *	@param	idx       配置インデックス
 *
  *	@retval TRUE  設定した
  *	@retval FALSE 設定しなかった（すでにアイテムが配置されている）
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_ITEM_SetItemData( FIELD_WFBC_CORE_ITEM* p_wk, u16 scr_item, u32 idx )
{
  GF_ASSERT( idx < FIELD_WFBC_PEOPLE_MAX );
  if( p_wk->scr_item[idx] == FIELD_WFBC_ITEM_NONE )
  {
    p_wk->scr_item[idx] = scr_item;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ITEM情報をクリア
 *
 *	@param	p_wk    ワーク
 *	@param	idx   インデックス
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ITEM_ClearItemData( FIELD_WFBC_CORE_ITEM* p_wk, u32 idx )
{
  GF_ASSERT( idx < FIELD_WFBC_PEOPLE_MAX );
  p_wk->scr_item[idx] = FIELD_WFBC_ITEM_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  配置されているアイテム情報を取得する
 *
 *	@param	cp_wk      ワーク
 *	@param	idx     インデックス
 *
 *	@retval アイテムのスクリプトID
 *	@retval FIELD_WFBC_ITEM_NONE 配置されていない
 */
//-----------------------------------------------------------------------------
u16 FIELD_WFBC_CORE_ITEM_GetItemData( const FIELD_WFBC_CORE_ITEM* cp_wk, u32 idx )
{
  GF_ASSERT( idx < FIELD_WFBC_PEOPLE_MAX );
  return cp_wk->scr_item[idx];
}

//----------------------------------------------------------------------------
/**
 *	@brief  idx位置にアイテムがあるのかチェック
 *
 *	@param	cp_wk    ワーク
 *	@param	idx   インデックス
 *
 *	@retval TRUE    ある
 *	@retval FALSE   ない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_ITEM_IsInItemData( const FIELD_WFBC_CORE_ITEM* cp_wk, u32 idx )
{
  GF_ASSERT( idx < FIELD_WFBC_PEOPLE_MAX );
  if( cp_wk->scr_item[idx] == FIELD_WFBC_ITEM_NONE )
  {
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ITEM配置情報からMMDLヘッダーを生成
 *
 *	@param	cp_wk     ワーク
 *	@param	mapmode   マップモード
 *	@param	heapID    ヒープID
 *
 *	@retval NULL  なし
 *	@retval その他　ITEMのMMDLヘッダー　設定後Freeしてください
 */
//-----------------------------------------------------------------------------
MMDL_HEADER* FIELD_WFBC_CORE_ITEM_MMDLHeaderCreateHeapLo( const FIELD_WFBC_CORE_ITEM* cp_wk, MAPMODE mapmode, FIELD_WFBC_CORE_TYPE type, HEAPID heapID )
{
  int i;
  int item_num;
  int item_index;
  MMDL_HEADER* p_buff;
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;
  const FIELD_WFBC_CORE_PEOPLE_POS* cp_itempos;

  // 自分の世界でしか表示しない
  if( mapmode != MAPMODE_NORMAL )
  {
    return NULL;
  }

  // タイプがWF？
  if( type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    return NULL;
  }

  heapID = GFL_HEAP_LOWID( heapID );

  // 総数を求める
  item_num = WFBC_CORE_ITEM_GetNum( cp_wk );
  item_index = 0;

  // バッファを生成
  p_buff = GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_HEADER) * item_num );

  // ローダー生成
  p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );

  // 位置情報を読み込み
  p_pos = FIELD_WFBC_PEOPLE_POS_Create( p_people_loader, type, heapID );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    
    if( FIELD_WFBC_CORE_ITEM_IsInItemData(cp_wk, i) )
    {
      // 位置取得
      cp_itempos = FIELD_WFBC_PEOPLE_POS_GetIndexItemPos( p_pos, i );
      
      p_buff[item_index] = sc_DEFAULT_ITEM_HEADER;
      p_buff[item_index].id       = FIELD_WFBC_ITEM_OBJID_START + i;
      p_buff[item_index].event_id = FIELD_WFBC_CORE_ITEM_GetItemData( cp_wk, i );
      GF_ASSERT( i < 10 );
      p_buff[item_index].event_flag = TMFLG_WC10_ITEM_01 + i;

      MMDLHEADER_SetGridPos( &p_buff[item_index], cp_itempos->gx, cp_itempos->gz, 0 );

      item_index ++;
    }
  }

  FIELD_WFBC_PEOPLE_POS_Delete( p_pos );
  FIELD_WFBC_PEOPLE_DATA_Delete( p_people_loader );

  return p_buff;
}


//-----------------------------------------------------------------------------
/**
 *					WFBCイベント管理ワーク
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  BC　NPCに勝った数を取得
 *
 *	@param	sv  セーブワーク
 *
 *	@return 勝った数
 */
//-----------------------------------------------------------------------------
u16 FIELD_WFBC_EVENT_GetBCNpcWinCount( const FIELD_WFBC_EVENT* cp_wk )
{
  GF_ASSERT(cp_wk);
  return cp_wk->bc_npc_win_count;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BC　NPCに勝った数を計算
 *
 *	@param	sv    セーブワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_AddBCNpcWinCount( FIELD_WFBC_EVENT* p_wk )
{
  GF_ASSERT(p_wk);
  p_wk->bc_npc_win_count ++;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BC　NPCに勝つ目標数を取得
 *
 *	@param	sv  セーブワーク
 *  
 *	@return 目標数
 */
//-----------------------------------------------------------------------------
u16 FIELD_WFBC_EVENT_GetBCNpcWinTarget( const FIELD_WFBC_EVENT* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->bc_npc_win_target;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BC　NPCに勝つ目標数を加算
 *
 *	@param	sv      セーブワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_AddBCNpcWinTarget( FIELD_WFBC_EVENT* p_wk )
{
  GF_ASSERT( p_wk );
  if( p_wk->bc_npc_win_target < FIELD_WFBC_EVENT_NPC_WIN_TARGET_MAX )
  {
    p_wk->bc_npc_win_target += FIELD_WFBC_EVENT_NPC_WIN_TARGET_ADD;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  WF　村長　もってきてほしいポケモンナンバーの取得
 *
 *	@param	sv    セーブワーク
 *
 *	@return モンスターナンバー
 */
//-----------------------------------------------------------------------------
u16 FIELD_WFBC_EVENT_GetWFPokeCatchEventMonsNo( const FIELD_WFBC_EVENT* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->wf_poke_catch_monsno; 
}

//----------------------------------------------------------------------------
/**
 *	@brief  WF　村長　もってきてほしいポケモンナンバー　設定
 *
 *	@param	sv        セーブワーク
 *	@param	mons_no   モンスターナンバー
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_SetWFPokeCatchEventMonsNo( FIELD_WFBC_EVENT* p_wk, u16 mons_no )
{
  GF_ASSERT( p_wk );
  p_wk->wf_poke_catch_monsno = mons_no;
}


//----------------------------------------------------------------------------
/**
 *	@brief  WF 村長　ご褒美　アイテムナンバー　取得
 *
 *	@param	cp_wk   ワーク
 *  
 *	@return アイテムナンバー
 */
//-----------------------------------------------------------------------------
u16 FIELD_WFBC_EVENT_GetWFPokeCatchEventItem( const FIELD_WFBC_EVENT* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->wf_poke_catch_item;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WF　村長　ご褒美　アイテム設定
 *
 *	@param	p_wk    ワーク
 *	@param	item    アイテムナンバー
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_SetWFPokeCatchEventItem( FIELD_WFBC_EVENT* p_wk, u16 item )
{
  GF_ASSERT( p_wk );
  p_wk->wf_poke_catch_item = item;
}




//-----------------------------------------------------------------------------
/**
 *        private関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  HITPERCENT計算つきの、NPCセットアップ
 *
 *	@param	cp_wk         ワーク
 *	@param	hit_percent   今回のヒットpercent値
 *	@param	p_loader      ローダー
 */
//-----------------------------------------------------------------------------
static const FIELD_WFBC_PEOPLE_DATA* WFBC_CORE_GetNotUseNpc_HitPercent( const FIELD_WFBC_CORE* cp_wk, u32 hit_percent, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader, int* p_setidx )
{
  int i;
  u32 npc_id = 0;
  u32 hit_percent_total = 0;
  const FIELD_WFBC_PEOPLE_DATA* cp_data;

  cp_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_loader );

  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    npc_id = WFBC_CORE_GetNotUseNpcID( cp_wk, i );

    // NPC情報読み込み
    FIELD_WFBC_PEOPLE_DATA_Load( p_loader, npc_id );
    hit_percent_total += cp_data->hit_percent;  

    if( hit_percent_total > hit_percent ){
      (*p_setidx) = npc_id;
      // こいつを登録
      return cp_data;
    }
  }

  TOMOYA_Printf( "WFBC_CORE_GetNotUseNpc_HitPercent おかしい。\n" );
  GF_ASSERT( 0 );
  return cp_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  まだ、人物データとして保持していない人物のIDX番目を取得する
 *
 *	@param	cp_wk     ワーク
 *	@param	idx       インデックス
 */ 
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = WFBC_CORE_IsUseNpc( cp_wk, i );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // 全部持ってる（ありえない）
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  まだ、人物データとして保持していないワークインデックスを求める
 *
 *	@param	cp_wk     ワーク
 *	@param	idx       インデックス
 */
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // 全部入ってます
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brieif NPC_IDがワーク内の人物にいるのかチェック
 *
 *	@param	cp_wk     ワーク
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE  いる
 *	@retval FALSE いない
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] ) )
    {
      if( cp_wk->people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] ) )
    {
      if( cp_wk->back_people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物配列の内容を機嫌値でソートする
 *
 *	@param	p_array   配列
 *	@param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID )
{
  int i, j;
  BOOL flag;
  FIELD_WFBC_CORE_PEOPLE* p_tmp;

  p_tmp = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( heapID ), sizeof(FIELD_WFBC_CORE_PEOPLE) );


  // 機嫌値の高い順番にバブルソート
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX-1; i++ )
  {
    flag = 0;
    for(j=0; j<FIELD_WFBC_PEOPLE_MAX-1; j++)
    {
      if( p_array[j].mood < p_array[j+1].mood )
      {
        // いれかえ
        GFL_STD_MemCopy( &p_array[j+1], p_tmp, sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( &p_array[j], &p_array[j+1], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( p_tmp, &p_array[j], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        flag =1;
      }
    }

    if( flag == 0 )
    {
      // 1度も変更がなければソート完了
      break;
    }
  }

  GFL_HEAP_FreeMemory( p_tmp );
}



//----------------------------------------------------------------------------
/**
 *	@brief  人を入れる　一番けつの情報は破棄
 *	        通常世界の配列に追加  通常配列を追い出す形で、裏世界の配列にも格納される
 *
 *	@param	p_wk          ワーク
 *	@param	cp_people     追加する人データ
 *	@param  parent_set    親の情報を設定するか？  （BCならせっていしてください）
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus, const FIELD_WFBC_CORE_PEOPLE* cp_people, BOOL parent_set )
{
  FIELD_WFBC_CORE_PEOPLE* p_add_people;
  
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_people );
  
  if( WFBC_CORE_IsPeopleArrayFull( p_wk->people ) == TRUE )
  {
    // けつの情報を裏世界に渡す。
    WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
  }

  // 通常世界の情報に追加
  p_add_people = WFBC_CORE_PushPeopleArray( p_wk->people, cp_people );
  if( parent_set )
  {
    FIELD_WFBC_CORE_PEOPLE_SetParentData( p_add_people, cp_mystatus );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  人の配列がいっぱいかチェック
 *
 *	@param	cp_array  配列
 *
 *	@retval   TRUE    いっぱい
 *	@retval   FALSE   まだ余裕がある
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_array[i] ) )
    {
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人配列に人を入れる。  一番けつの情報は破棄
 *
 *	@param	p_array     配列
 *	@param	cp_people   人
 *
 *	@retval 追加したワーク
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{ 
  int i;
  
  // すべて１つづつずらして、先頭に追加
  for( i=FIELD_WFBC_PEOPLE_MAX-1; i>0; i-- )
  {
    GFL_STD_MemCopy( &p_array[i-1], &p_array[i], sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
  GFL_STD_MemCopy( cp_people, &p_array[0], sizeof(FIELD_WFBC_CORE_PEOPLE) );

  return &p_array[0];
}




//----------------------------------------------------------------------------
/**
 *	@brief  空いてる人物ワークを返す
 *
 *	@param	p_array   配列
 *
 *	@return 空いている人物ワーク
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &p_array[i] ) )
    {
      return &p_array[i];
    }
  }
  // 空きがなし
  GF_ASSERT( 0 );
  return &p_array[0]; // フリーズは回避
}



//----------------------------------------------------------------------------
/**
 *	@brief  人物情報をセットアップ
 *
 *	@param	p_people  人物ワーク
 *	@param	npc_id    NPCID
 *	@param  mystatus  プレイヤー情報
 *	@param  mood      機嫌初期値
 *	@param  parent_setプレイヤー情報を保存するか
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus, u16 mood, BOOL parent_set )
{
  p_people->data_in = TRUE;
  p_people->npc_id  = npc_id;
  p_people->mood    = mood;
  p_people->one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;

  // 親の設定
  if( parent_set )
  {
    // 自分で初期化する
    FIELD_WFBC_CORE_PEOPLE_SetParentData( p_people, cp_mystatus );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク  背面世界に行くべきかチェック
 *
 *	@param	cp_people   人物ワーク
 *
 *	@retval TRUE  いくべき
 *	@retval FALSE いかないべき
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  if( cp_people->mood == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値の計算
 *
 *	@param	p_people  人物ワーク
 *	@param	add       加算  マイナスも可
 *
 *	@retval TRUE    背面世界にいくべき
 *	@retval FALSE   背面世界にいかないべき
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add )
{
  int mood = p_people->mood;
  BOOL ret = FALSE;
  
  if( (mood + add) > FIELD_WFBC_MOOD_MAX )
  {
    p_people->mood = FIELD_WFBC_MOOD_MAX;
  }
  else if( (mood + add) < 0 )
  {
    p_people->mood = 0;
    ret = TRUE;
  }
  else
  {
    p_people->mood += add;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値を設定
 *
 *	@param	p_people
 *	@param	mood 
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_People_SetMood( FIELD_WFBC_CORE_PEOPLE* p_people, int mood )
{
  p_people->mood = mood;
}


#ifdef PM_DEBUG
u32 FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( const FIELD_WFBC_CORE* cp_wk )
{
  u32 npc_idx;
  u32 npc_num;

  npc_num = FIELD_WFBC_CORE_GetPeopleNum( cp_wk, MAPMODE_NORMAL );
  npc_num += FIELD_WFBC_CORE_GetPeopleNum( cp_wk, MAPMODE_INTRUDE );

  npc_idx = GFUser_GetPublicRand( FIELD_WFBC_NPCID_MAX - npc_num );


  npc_idx = WFBC_CORE_GetNotUseNpcID( cp_wk, npc_idx );

  return npc_idx;
}
#endif
