//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_crowd_people.c
 *	@brief  群集管理処理
 *	@author	tomoya takahshi
 *	@date		2010.01.26
 *
 *	モジュール名：FIELD_CROWD_PEOPLE
 *
 *	C03の路地用　群集処理　　沢山の動作モデルを使用して、表現
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/script_message.naix"

#include "system/timezone.h"

#include "field/field_const.h"
#include "field/field_msgbg.h"

#include "fieldmap.h"
#include "ev_time.h"
#include "field_crowd_people.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h" //SCRID_DUMMY


#include "fieldmap/field_crowd_people.naix"

#include "msg/script/msg_crowd_people_scr.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

// 動作シーケンス
enum
{
  // 通常/意地悪共通
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT=0,// 動作完了待ち
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK,    // 判断＋指示
  FIELD_CROWD_PEOPLE_SEQ_NORMAL_END,     // 終了

};

// 動作タイプ
enum
{
  FIELD_CROWD_PEOPLE_TYPE_NORMAL,   // 普通の人
  FIELD_CROWD_PEOPLE_TYPE_NASTY,    // いじわる　な人

  FIELD_CROWD_PEOPLE_TYPE_MAX,    //
};

// 人物最大数
#define FIELD_CROWD_PEOPLE_MAX  (32)

// 群集OBJ識別ID
#define FIELD_CROWD_PEOPLE_ID (48) // 仕掛けようOBJIDを使用させてもらう

// 人物基本値ヘッダー
static MMDL_HEADER s_MMDL_HEADER = 
{
  FIELD_CROWD_PEOPLE_ID,  ///<識別ID
  BOY1,                      ///<表示するOBJコード
  MV_DMY,                 ///<動作コード
  0,                      ///<イベントタイプ
  0,                      ///<イベントフラグ
  0,                      ///<イベントID
  0,                      ///<指定方向
  0,                      ///<指定パラメタ 0
  0,                      ///<指定パラメタ 1
  0,                      ///<指定パラメタ 2
	0,			                ///<X方向移動制限
	0,			                ///<Z方向移動制限
	MMDL_HEADER_POSTYPE_GRID,///<ポジションタイプ
};

// オブセットを動かす数字
#define FIELD_CROWD_PEOPLE_OFFSET_MOVE_MIN  ( -4 )
#define FIELD_CROWD_PEOPLE_OFFSET_MOVE_RND  ( 8 )

//-------------------------------------
///	通常動作　定数
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_GRID (2) // 何歩前までチェックするのか？


//-------------------------------------
///	意地悪動作　定数
//=====================================
#define FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID (1) // 何歩前までチェックするのか？
#define FIELD_CROWD_PEOPLE_PUT_PAR (10) // FIELD_CROWD_PEOPLE_PUT_PAR回に一回意地悪がでる



//-------------------------------------
///	起動情報
//=====================================
#define FIELD_CROWD_PEOPLE_BOOT_POINT_MAX (2)
#define FIELD_CROWD_PEOPLE_OBJ_CODE_MAX (4)   // 表示OBJ_CODEの最大数

#define FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME (6) // 歩きのフレーム数


#define FIELD_CROWD_PEOPLE_SCRIPT_MAX   (6) // OBJCODEのスクリプトの数

#define FIELD_CROWD_PEOPLE_TIMEZONE_TBL_MAX   (6) // タイムゾーンテーブルの数


//-------------------------------------
///	起動情報リストのデータ
//=====================================
typedef u32 BOOT_ZONEID_TYPE;


//-------------------------------------
///	騒音　定数
//=====================================
#define NOISE_TIME_MIN  ( 16)
#define NOISE_TIME_WAIT_NUM  ( 1 )
#define NOISE_MSG_MAX  ( 20 )
#define NOISE_SIZX  ( 8 )
#define NOISE_SIZY  ( 2 )
#define NOISE_WK_WAIT  ( 48)

#define NOISE_WK_MAX  ( 16 )

static const u8 sc_NOISE_X[ NOISE_WK_MAX ] = 
{
  1,
  22,
  2,
  20,
  4,
  21,
  3,
  19,
  1,
  19,
  2,
  22,
  3,
  20,
  4,
  21,
};
static const u8 sc_NOISE_Y[ NOISE_WK_MAX ] = 
{
  6,
  2,
  17,
  14,
  17,
  10,
  14,
  2,
  10,
  17,
  10,
  6,
  2,
  6,
  14,
  6,
};


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	objcode スクリプトテーブル
//=====================================
typedef struct 
{
  u32 objcode;
  u16 script[FIELD_CROWD_PEOPLE_SCRIPT_MAX];
} FIELD_CROWD_PEOPLE_OBJSCRIPT;


//-------------------------------------
///	起動点情報
//=====================================
typedef struct 
{
  // 起動グリッド範囲
  u16 top;
  u16 bottom;
  u16 left;
  u16 right;

  // 移動方向
  u16 move_dir;

  // 移動距離
  u16 move_grid;

} FIELD_CROWD_PEOPLE_BOOT_POINT;

//-------------------------------------
///	起動情報
//=====================================
typedef struct 
{
  s16 wait[FIELD_CROWD_PEOPLE_TIMEZONE_TBL_MAX];       // 起動ウエイト 0最短 
  u8 point_num;  // 起動ポイントの数
  u8 obj_code_num;// OBJCODEの数
  u16 obj_code_tbl[FIELD_CROWD_PEOPLE_OBJ_CODE_MAX]; // OBJCODEのテーブル

  u8 noise_start;
  u8 noise_num;
  u8 move_frame;
  u8 pad;
  
  FIELD_CROWD_PEOPLE_BOOT_POINT point[FIELD_CROWD_PEOPLE_BOOT_POINT_MAX];
} FIELD_CROWD_PEOPLE_BOOT_DATA;


//-------------------------------------
///	起動コントロールシステム
//=====================================
typedef struct 
{
  FIELD_CROWD_PEOPLE_BOOT_DATA boot_data;
  s16 wait[ FIELD_CROWD_PEOPLE_BOOT_POINT_MAX ];


  FIELD_CROWD_PEOPLE_OBJSCRIPT* p_objcode_script_tbl;
  u32 objcode_num;
} FIELD_CROWD_PEOPLE_BOOL_CONTROL;


//-------------------------------------
///	騒音ワーク
//=====================================
typedef struct {
  u16 flag;
  u8 x;
  u8 y;
  u16 id;
  s16 wait;
} FIELD_CROWD_PEOPLE_NOISE_WK;



//-------------------------------------
///	サブウィンドウ　騒音システム
//=====================================
typedef struct 
{
  s16 wait;
  s16 wait_max;
  u32 add_idx;
  GFL_MSGDATA * p_msgdata;
  FLDMSGBG* p_fmb;

  FIELD_CROWD_PEOPLE_NOISE_WK wk[NOISE_WK_MAX];
} FIELD_CROWD_PEOPLE_NOISE;


//-------------------------------------
///	管理１オブジェ
//=====================================
typedef struct 
{
  u8 flag;
  u8 seq;
  u8 type;
  u8 move_dir;
  u16 move_grid;
  u16 event_id;
  u16 id;
  u16 move_acmd;
  MMDL* p_mmdl;

  const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point;

  u16 last_side;  // 1つ前がよけか？
  u16 last_dir;   // よけた方向

  
} FIELD_CROWD_PEOPLE_WK;


//-------------------------------------
///	管理ワーク
//=====================================
struct _FIELD_CROWD_PEOPLE
{
  FIELD_PLAYER* p_player;
  MMDLSYS* p_fos;
  FIELD_CROWD_PEOPLE_WK people_wk[FIELD_CROWD_PEOPLE_MAX];

  FIELD_CROWD_PEOPLE_BOOL_CONTROL boot_control;

  FIELD_CROWD_PEOPLE_NOISE noise;
} ;




//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------




static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Delete( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIELD_CROWD_PEOPLE_TASK_Update( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );



// 
static FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_GetClearWk( FIELD_CROWD_PEOPLE* p_sys );
static const FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_Search( const FIELD_CROWD_PEOPLE* cp_sys, u16 gx, u16 gz, u16 move_dir, u16 type, u32* p_hit );

//-------------------------------------
///	ワーク管理
//=====================================
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_data, u32 idx );
static void FIELD_CROWD_PEOPLE_WK_Exit( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_Main( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point );
static void FIELD_CROWD_PEOPLE_WK_ChangeNormalMove( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_ClearMove( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_ThroughMove( FIELD_CROWD_PEOPLE_WK* p_wk, const MMDL* cp_hitmmdl );



// 起動管理
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int timezone, int zone_id, HEAPID heapID );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, FIELD_CROWD_PEOPLE* p_sysdata, int timezone );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( const FIELD_CROWD_PEOPLE* cp_sysdata, const FIELD_CROWD_PEOPLE_BOOT_DATA* cp_data, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point, FIELD_CROWD_PEOPLE_WK* p_people, u16 add_grid );
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk, FIELD_CROWD_PEOPLE* p_sysdata, int timezone );
static u16 FIELD_CROWD_PEOPLE_BOOT_CONTROL_GetMoveAcmd( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk );

// タイプごとの動き
static void FIELD_CROWD_PEOPLE_WK_SetUpNormal( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_SetUpNasty( FIELD_CROWD_PEOPLE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_WK_MainNormal( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );
static void FIELD_CROWD_PEOPLE_WK_MainNasty( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player );


// 引数歩前に自機がいないかチェック
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontAllMMdl( const FIELD_CROWD_PEOPLE_WK* cp_wk, const MMDLSYS* cp_fos, MMDL** pp_mmdl, u16 grid, u16 min_grid );
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( const FIELD_CROWD_PEOPLE_WK* cp_wk, const MMDL* cp_mmdl, u16 grid, u16 min_grid );
static BOOL FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( s16 mygx, s16 mygz, s16 pl_gx, s16 pl_gz, u16 grid, u16 min_grid, u16 move_dir );



// NOISE管理
static void FIELD_CROWD_PEOPLE_NOISE_Init( FIELD_CROWD_PEOPLE_NOISE* p_wk, FLDMSGBG* p_fmb, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, int timezone, HEAPID heapID );
static void FIELD_CROWD_PEOPLE_NOISE_Exit( FIELD_CROWD_PEOPLE_NOISE* p_wk );
static void FIELD_CROWD_PEOPLE_NOISE_Clear( FIELD_CROWD_PEOPLE_NOISE* p_wk );
static void FIELD_CROWD_PEOPLE_NOISE_Main( FIELD_CROWD_PEOPLE_NOISE* p_wk, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, const FIELDMAP_WORK* cp_fieldmap );

static void FIELD_CROWD_PEOPLE_NOISEWK_Init( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, u16 id );
static void FIELD_CROWD_PEOPLE_NOISEWK_Start( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb, GFL_MSGDATA* p_msgdata, int noise_msg_start, int noise_msg_num );
static void FIELD_CROWD_PEOPLE_NOISEWK_Main( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb );
static void FIELD_CROWD_PEOPLE_NOISEWK_Clear( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk );
static void FIELD_CROWD_PEOPLE_NOISEWK_Off( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb );



const FLDMAPFUNC_DATA c_FLDMAPFUNC_CROWDPEOPLE_DATA = 
{
  0,
  sizeof(FIELD_CROWD_PEOPLE),
  FIELD_CROWD_PEOPLE_TASK_Create,
  FIELD_CROWD_PEOPLE_TASK_Delete,
  FIELD_CROWD_PEOPLE_TASK_Update,
  NULL,
};





//----------------------------------------------------------------------------
/**
 *	@brief  群集ワーク生成
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Create( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work ) {
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;
  int zone_id;
  HEAPID heapID;
  FLDMSGBG* p_fmb = FIELDMAP_GetFldMsgBG( p_fieldmap );
  int timezone;
  GAMESYS_WORK * p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys ); 

  // 管理システムワーク初期化
  p_wk->p_player  = FIELDMAP_GetFieldPlayer( p_fieldmap );
  p_wk->p_fos     = FIELDMAP_GetMMdlSys( p_fieldmap );
  zone_id = FIELDMAP_GetZoneID( p_fieldmap );
  timezone = EVTIME_GetSeasonTimeZone( p_gdata );
  heapID = FIELDMAP_GetHeapID( p_fieldmap );

  GF_ASSERT( timezone < TIMEZONE_MAX );

  // ゾーンから起動情報を読み込み
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( &p_wk->boot_control, timezone, zone_id, heapID );

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Init( &p_wk->people_wk[i], p_wk->p_fos, zone_id, &p_wk->boot_control, i ); 
  }

  // 全員を配置
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( &p_wk->boot_control, p_wk, timezone );


  // 騒音システム初期化
  FIELD_CROWD_PEOPLE_NOISE_Init( &p_wk->noise, p_fmb, &p_wk->boot_control, timezone, heapID );

  // ワークをフィールドマップに登録
  // field_event_check.cでイベント発生時に群集をとめるため。
  GF_ASSERT( FIELDMAP_GetCrowdPeopleSys( p_fieldmap ) == NULL );
  FIELDMAP_SetCrowdPeopleSys( p_fieldmap, p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  群集ワーク破棄
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Delete( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;

  // ワークをフィールドマップから破棄
  GF_ASSERT( FIELDMAP_GetCrowdPeopleSys( p_fieldmap ) == p_wk );
  FIELDMAP_SetCrowdPeopleSys( p_fieldmap, NULL );
  
  // 管理システムワーク破棄
  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Exit( &p_wk->people_wk[i] ); 
  }

  // ゾーンから起動情報を破棄
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( &p_wk->boot_control );

  // 騒音システム破棄
  FIELD_CROWD_PEOPLE_NOISE_Exit( &p_wk->noise );

  p_wk->p_player  = NULL;
  p_wk->p_fos     = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  群集　更新
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_TASK_Update( FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIELD_CROWD_PEOPLE* p_wk = p_work;
  int i;
  int timezone = FIELDMAP_GetSeasonTimeZone( p_fieldmap );

  GF_ASSERT( timezone < TIMEZONE_MAX );
  
  // 起動チェック
  FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( &p_wk->boot_control, p_wk, timezone ); 


  // イベント中は、全ワークの動作方法をNORMALに変更
  if( FIELDMAP_CheckDoEvent( p_fieldmap ) ){
    for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
    {
      FIELD_CROWD_PEOPLE_WK_ChangeNormalMove( &p_wk->people_wk[i] );
    }
  }
  
  // 管理システムワーク更新
  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_WK_Main( &p_wk->people_wk[i], p_wk->p_fos, p_wk->p_player ); 
  }

  // 騒音システム更新
  FIELD_CROWD_PEOPLE_NOISE_Main( &p_wk->noise, &p_wk->boot_control, p_fieldmap );
}




//----------------------------------------------------------------------------
/**
 *	@brief  サブwindowを全クリア
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
void FIELD_CROWD_PEOPLE_ClearSubWindow( FIELD_CROWD_PEOPLE* p_sys )
{
  FIELD_CROWD_PEOPLE_NOISE_Clear( &p_sys->noise );
}






//----------------------------------------------------------------------------
/**
 *	@brief  空いているワークを取得
 *
 *	@param	p_sys 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_GetClearWk( FIELD_CROWD_PEOPLE* p_sys )
{
  int i;

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    if( p_sys->people_wk[i].flag == FALSE )
    {
      return &p_sys->people_wk[i];
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  gx gzからmove_dirの方向で　type動作しているオブジェを検索
 *
 *	@param	cp_sys       システムワーク
 *	@param	gx          グリッドX
 *	@param	gz          グリッドZ
 *	@param	move_dir    動作方向
 *	@param	type        タイプ 
 *	@param  p_hit       完全一致チェック用
 *
 *	@return 人物ワーク
 */
//-----------------------------------------------------------------------------
static const FIELD_CROWD_PEOPLE_WK* FIELD_CROWD_PEOPLE_Search( const FIELD_CROWD_PEOPLE* cp_sys, u16 gx, u16 gz, u16 move_dir, u16 type, u32* p_hit )
{
  int i;
  s16 move_x;
  s16 move_z;
  s16 dif_x;
  s16 dif_z;
  s16 mdl_gx, mdl_gz;



  switch( move_dir )
  {
  case DIR_UP:
    move_x = 0;
    move_z = -1;
    break;
  case DIR_DOWN:
    move_x = 0;
    move_z = 1;
    break;
  case DIR_RIGHT:
    move_x = 1;
    move_z = 0;
    break;
  case DIR_LEFT:
    move_x = -1;
    move_z = 0;
    break;
  }

  *p_hit = FALSE;
  

  for( i=0; i<FIELD_CROWD_PEOPLE_MAX; i++ )
  {
    if( cp_sys->people_wk[i].flag )
    {
      if(cp_sys->people_wk[i].type == FIELD_CROWD_PEOPLE_TYPE_NASTY)
      {
        // グリッド方向にいるか？
        mdl_gx = MMDL_GetGridPosX( cp_sys->people_wk[i].p_mmdl );
        mdl_gz = MMDL_GetGridPosZ( cp_sys->people_wk[i].p_mmdl );

        dif_x = mdl_gx - gx;
        dif_z = mdl_gz - gz;

        if( dif_x != 0 ){
          dif_x /= MATH_ABS( dif_x );
        }
        if( dif_z != 0 ){
          dif_z /= MATH_ABS( dif_z );
        }

        // 完全一致もチェック
        if( (dif_x == 0) && (dif_z == 0) ){
          *p_hit = TRUE;
          return &cp_sys->people_wk[i];
        }

        // 移動方向が一致したら、OK
        if( (dif_x == move_x) && (dif_z == move_z) )
        {
          return &cp_sys->people_wk[i];
        }
      }
    }
  }

  return NULL;
}



//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク  初期化
 *
 *	@param	p_wk      ワーク
 *	@param	p_fos     動作モデルシステム
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Init( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, int zone_id, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_data, u32 idx )
{
  u16 obj_code;
  u32 rand = GFUser_GetPublicRand(0);
  int i;


  // 見た目決定
  // テーブルからランダムで。
  obj_code = cp_data->boot_data.obj_code_tbl[ rand % cp_data->boot_data.obj_code_num ];

  s_MMDL_HEADER.obj_code = obj_code;

  //スクリプト用に再抽選
  rand = GFUser_GetPublicRand(0);

  //　OBJ独自の情報の決定
  p_wk->id = FIELD_CROWD_PEOPLE_ID + idx;
  for( i=0; i<cp_data->objcode_num; i++ )
  {
    if( cp_data->p_objcode_script_tbl[i].objcode == obj_code )
    {
      p_wk->event_id = cp_data->p_objcode_script_tbl[i].script[ rand % FIELD_CROWD_PEOPLE_SCRIPT_MAX ];
      break;
    }
  }
  // OBJCODEのスクリプトを設定したかチェック
  GF_ASSERT( i < cp_data->objcode_num );
  
  p_wk->p_mmdl    = MMDLSYS_AddMMdl( p_fos, &s_MMDL_HEADER, zone_id ); 
  p_wk->flag      = 0;
  p_wk->seq       = 0;
  p_wk->type      = 0;
  p_wk->move_dir  = 0;
  p_wk->move_acmd = FIELD_CROWD_PEOPLE_BOOT_CONTROL_GetMoveAcmd( cp_data );

  // Vanish
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, TRUE );

  // Not Save
  MMDL_SetMoveBitNotSave( p_wk->p_mmdl, TRUE );

  // 描画オフセット設定
  {
    VecFx32 offset = {0,0,0};

    // ハングリッド　＋　−
    offset.x = (FIELD_CROWD_PEOPLE_OFFSET_MOVE_MIN<<FX32_SHIFT) + GFUser_GetPublicRand(FIELD_CROWD_PEOPLE_OFFSET_MOVE_RND<<FX32_SHIFT);
    MMDL_SetVectorDrawOffsetPos( p_wk->p_mmdl, &offset );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク　破棄
 *
 *	@param	p_wk      ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Exit( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  // オブジェを破棄
  MMDL_Delete( p_wk->p_mmdl );
  p_wk->p_mmdl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク　メイン
 *
 *	@param	p_wk        ワーク
 *	@param  p_fos       動作システムワーク
 *	@param	cp_player   プレイヤーシステム
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_Main( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  static void (* const cpFunc[FIELD_CROWD_PEOPLE_TYPE_MAX])( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player ) = 
  {
    FIELD_CROWD_PEOPLE_WK_MainNormal,
    FIELD_CROWD_PEOPLE_WK_MainNasty,
  };
  
  if( p_wk->flag == FALSE ){
    return ;
  }
  
  GF_ASSERT( p_wk->type < FIELD_CROWD_PEOPLE_TYPE_MAX );
  
  // タイプごとの動き
  cpFunc[ p_wk->type ]( p_wk, p_fos, cp_player );

  // 座標を表示
#ifdef PM_DEBUG
  /*
  {
    VecFx32 pos;
    MMDL_GetVectorPos( p_wk->p_mmdl, &pos );
    TOMOYA_Printf( "mdl pos x[%d] y[%d] z[%d]\n", FX_Whole(pos.x), FX_Whole(pos.y), FX_Whole(pos.z) );
  }
  */
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク　動作セットアップ
 *
 *	@param	p_wk        ワーク
 *	@param	p_fos       動作モデルシステム
 *	@param	cp_player   主人公ワーク
 *	@param  type        動作タイプ
 *	@param	gx          開始グリッド位置
 *	@param	gz          開始グリッド位置
 *	@param	move_dir    動作方向
 *	@param  move_grid   動作グリッド距離
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUp( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player, u16 type, u16 gx, u16 gz, u16 move_dir, u16 move_grid, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point )
{
  static void (* const cpFunc[FIELD_CROWD_PEOPLE_TYPE_MAX])( FIELD_CROWD_PEOPLE_WK* p_wk ) = 
  {
    FIELD_CROWD_PEOPLE_WK_SetUpNormal,
    FIELD_CROWD_PEOPLE_WK_SetUpNasty,
  };

  GF_ASSERT( p_wk->flag == FALSE );

  // 動作開始
  p_wk->flag = TRUE; 

  // 表示ON
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, FALSE );

  // 人物の情報を大きく変更
  {
    MMDL_HEADER header = s_MMDL_HEADER;
    header.obj_code = MMDL_GetOBJCode( p_wk->p_mmdl );
    header.dir      = move_dir;
    header.id       = p_wk->id;
    MMDLHEADER_SetGridPos( &header, gx, gz, 0 );
    MMDL_ChangeMoveParam( p_wk->p_mmdl, &header );
  }
  p_wk->type = type;
  p_wk->move_dir = move_dir;
  p_wk->seq = 0;
  p_wk->move_grid = move_grid;

  p_wk->last_side = FALSE;
  p_wk->last_dir  = 0;

  // Not Save
  MMDL_SetMoveBitNotSave( p_wk->p_mmdl, TRUE );

  // 起動ポイントを保存
  p_wk->cp_point = cp_point;

  // 内部ワーク情報の設定
  cpFunc[ type ]( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Normal動作に変更する。
 *
 *	@param	p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_ChangeNormalMove( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  if( p_wk->flag == TRUE ){
    if( p_wk->type == FIELD_CROWD_PEOPLE_TYPE_NASTY ){
      if( MMDL_GetEventID( p_wk->p_mmdl ) == SCRID_DUMMY ){ // 目の前に主人公がいる場合は変えない。　前に歩けているときのみ。　そうしないと話しかけた人がうごきだしてしまう。
        p_wk->type = FIELD_CROWD_PEOPLE_TYPE_NORMAL;
        FIELD_CROWD_PEOPLE_WK_SetUpNormal( p_wk );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brieif 動作情報のクリア
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_ClearMove( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  p_wk->flag = FALSE;
  p_wk->type = 0;
  p_wk->move_dir = 0;
  p_wk->seq = 0;

  // 表示OFF
  MMDL_SetStatusBitVanish( p_wk->p_mmdl, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  さけ動作
 *
 *	@param	p_wk
 *	@param	cp_hitmmdl 
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_ThroughMove( FIELD_CROWD_PEOPLE_WK* p_wk, const MMDL* cp_hitmmdl )
{
  u16 player_dir = MMDL_GetDirDisp( cp_hitmmdl );
  u16 move_dir;
  s16 now_x, now_z;

  now_x = MMDL_GetGridPosX( p_wk->p_mmdl );
  now_z = MMDL_GetGridPosZ( p_wk->p_mmdl );

  if( p_wk->last_side )
  {
    // 1つ前もよけなら同じ方向によける
    move_dir = p_wk->last_dir;
  }
  else
  {
    
    // よける
    // よける方向を決定
    if( (player_dir == p_wk->move_dir) || (MMDL_TOOL_FlipDir(player_dir) == p_wk->move_dir) )
    {
      // ランダムで９０度先のどちらかの方向にする
      if( GFUser_GetPublicRand(2) == 0 )
      {
        move_dir = player_dir + 2;
      }
      else
      {
        move_dir = MMDL_TOOL_FlipDir(player_dir + 2);
      }
    }
    else
    {
      // プレイヤーの反対方向によける
      move_dir = MMDL_TOOL_FlipDir(player_dir);
    }
  }

  // その方向によけて、エリアの外にいかないか？
  now_x += MMDL_TOOL_GetDirAddValueGridX( move_dir );
  now_z += MMDL_TOOL_GetDirAddValueGridZ( move_dir );
  if( (move_dir == DIR_UP) || (move_dir == DIR_DOWN) )
  {
    if( (now_z < p_wk->cp_point->top) || (now_z > p_wk->cp_point->bottom) )
    {
      // 逆方向によける
      move_dir = MMDL_TOOL_FlipDir(move_dir);
    }
  }
  else
  {
    if( (now_x < p_wk->cp_point->left) || (now_x > p_wk->cp_point->right) )
    {
      // 逆方向によける
      move_dir = MMDL_TOOL_FlipDir(move_dir);
    }
  }

  // よけまーす。
  MMDL_SetAcmd( p_wk->p_mmdl, p_wk->move_acmd + move_dir );

  // 設定した方向を保存
  p_wk->last_side = TRUE;
  p_wk->last_dir = move_dir;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物　起動　管理処理　初期化
 *
 *	@param	p_wk        ワーク
 *	@param	zone_id     ゾーンID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Init( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, int timezone, int zone_id, HEAPID heapID )
{
  int i;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_FIELD_CROWD_PEOPLE, GFL_HEAP_LOWID(heapID) );

  {
    BOOT_ZONEID_TYPE* p_list;
    u32 list_num;
    u32 list_idx;
    u32 size;

    // 起動IDXを求める
    p_list = GFL_ARC_LoadDataAllocByHandle( p_handle, NARC_field_crowd_people_list_data_dat, GFL_HEAP_LOWID(heapID) );
    size = GFL_ARC_GetDataSizeByHandle( p_handle, NARC_field_crowd_people_list_data_dat );
    list_num = size / sizeof(BOOT_ZONEID_TYPE);

    for( i=0; i<list_num; i++ )
    {
      if( p_list[i] == zone_id )
      {
        break;
      }
    }
    GF_ASSERT_MSG( i < list_num, "zoneに群集情報がありません。\n" );
    list_idx = i;

    TOMOYA_Printf( "crowd people list_idx %d\n", list_idx );
    
    // 起動ポイント情報を読み込み
    GFL_ARC_LoadDataOfsByHandle( p_handle, list_idx, 0, sizeof(FIELD_CROWD_PEOPLE_BOOT_DATA), &p_wk->boot_data );

    GFL_HEAP_FreeMemory( p_list );
  }

  // スクリプト情報読み込み
  {
    u32 size;
    size = GFL_ARC_GetDataSizeByHandle( p_handle, NARC_field_crowd_people_script_dat );


    p_wk->p_objcode_script_tbl = GFL_ARC_LoadDataAllocByHandle( p_handle, NARC_field_crowd_people_script_dat, heapID );
    p_wk->objcode_num = size / sizeof(FIELD_CROWD_PEOPLE_OBJSCRIPT);
  }

  GFL_ARC_CloseDataHandle( p_handle );

  for( i=0; i<FIELD_CROWD_PEOPLE_BOOT_POINT_MAX; i++ )
  {
    //p_wk->wait[i] = GFUser_GetPublicRand(p_wk->boot_data.wait[timezone]); // 初期ウエイト値
    p_wk->wait[i] = 0; // 初期ウエイト値  確実にあける。
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  人物　起動　管理処理　破棄
 *
 *	@param	p_wk        ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Exit( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk->p_objcode_script_tbl );
  p_wk->p_objcode_script_tbl = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物　起動　メイン処理
 *
 *	@param	p_wk        ワーク
 *	@param	p_sysdata   システムデータ
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_Main( FIELD_CROWD_PEOPLE_BOOL_CONTROL* p_wk, FIELD_CROWD_PEOPLE* p_sysdata, int timezone )
{
  int i;

  GF_ASSERT( timezone < TIMEZONE_MAX );
  
  for( i=0; i<p_wk->boot_data.point_num; i++ )
  {
    // ウエイト時間がいったら、人を出す
    if( p_wk->wait[i] >= p_wk->boot_data.wait[timezone] )
    {
      FIELD_CROWD_PEOPLE_WK* p_people = FIELD_CROWD_PEOPLE_GetClearWk( p_sysdata );
      if( p_people )
      {
        // そのポイントから人を出す。
        FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( p_sysdata, &p_wk->boot_data, &p_wk->boot_data.point[i], p_people, 0 ); 
        p_wk->wait[i] = 0;
      }
      else
      {
        TOMOYA_Printf( "CROWD people people none\n" );
      }
    }
    else
    {
      p_wk->wait[i] ++;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動管理処理　人物情報のセットアップ
 *
 *	@param	cp_data       起動データ
 *	@param	cp_point      起動ポイントデータ
 *	@param	p_people      セットアップ人物
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( const FIELD_CROWD_PEOPLE* cp_sysdata, const FIELD_CROWD_PEOPLE_BOOT_DATA* cp_data, const FIELD_CROWD_PEOPLE_BOOT_POINT* cp_point, FIELD_CROWD_PEOPLE_WK* p_people, u16 add_grid )
{
  u16 gx, gz;
  u16 type = FIELD_CROWD_PEOPLE_TYPE_NORMAL;
  u32 rand = GFUser_GetPublicRand( 0 );
  s32 move_grid;
  u32 hit;
  
  // 起動グリッド決定
  if( (cp_point->move_dir == DIR_UP) || (cp_point->move_dir == DIR_DOWN) )
  {
    gx = cp_point->left + (rand % ((cp_point->right - cp_point->left)+1));
    if(cp_point->move_dir == DIR_UP){
      gz = cp_point->top - add_grid;
    }else{
      gz = cp_point->top + add_grid;
    }
  }
  else
  {
    if(cp_point->move_dir == DIR_LEFT){
      gx = cp_point->left - add_grid;
    }else{
      gx = cp_point->left + add_grid;
    }
    gz = cp_point->top + (rand % ((cp_point->bottom - cp_point->top)+1));
  }

  // 動作タイプ決定
  // そのグリッド上に話せる人がいなければ、話せる意地悪な人を出す。
  if( FIELD_CROWD_PEOPLE_Search( cp_sysdata, gx, gz, cp_point->move_dir, FIELD_CROWD_PEOPLE_TYPE_NASTY, &hit ) == NULL )
  {
    if( (rand % FIELD_CROWD_PEOPLE_PUT_PAR) == 0 )
    {
      type = FIELD_CROWD_PEOPLE_TYPE_NASTY;
    }
  }

  // 完全一致だった場合、初期化しない。
  if( hit ){
    TOMOYA_Printf( "完全一致！！！！\n" );
    return ;
  }

  move_grid = cp_point->move_grid - add_grid;
  if( move_grid <= 0 ){
    move_grid = 1;
  }

  // 設定
  FIELD_CROWD_PEOPLE_WK_SetUp( p_people, cp_sysdata->p_fos, cp_sysdata->p_player, type, gx, gz, cp_point->move_dir, cp_point->move_grid - add_grid, cp_point );
}


//----------------------------------------------------------------------------
/**
 *	@brief  開始人物配置
 *
 *	@param	p_wk        ワーク
 *	@param	p_sysdata   システムデータ
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_BOOT_CONTROL_StartUp( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk, FIELD_CROWD_PEOPLE* p_sysdata, int timezone )
{
  int i, j;
  int total_time;
  int total_people_num;
  int time;
  GF_ASSERT( timezone < TIMEZONE_MAX );

  for( i=0; i<cp_wk->boot_data.point_num; i++ )
  {
    // 全部の位置を歩くために必要な全体タイム
    total_time = cp_wk->boot_data.point[i].move_grid * FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME;

    // waitで割ると、何人分かチェック
    total_people_num = total_time / cp_wk->boot_data.wait[timezone];

    // その人数セットアップ
    time = 0;
    for( j=0; j<total_people_num; j++ )
    {
      FIELD_CROWD_PEOPLE_WK* p_people = FIELD_CROWD_PEOPLE_GetClearWk( p_sysdata );

      if( p_people ){
        FIELD_CROWD_PEOPLE_BOOT_CONTROL_SetUpPeople( p_sysdata, &cp_wk->boot_data, &cp_wk->boot_data.point[i], p_people, time / FIELD_CROWD_PEOPLE_OBJ_WALK_FRAME ); 
        time += cp_wk->boot_data.wait[timezone];
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動に使うアクションコマンドを取得
 *
 *	@param	cp_wk     ワーク
 *
 *	@return アクションコマンド
 */
//-----------------------------------------------------------------------------
static u16 FIELD_CROWD_PEOPLE_BOOT_CONTROL_GetMoveAcmd( const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_wk )
{
  if( cp_wk->boot_data.move_frame == 2 ){
    return AC_WALK_U_2F;
  }else if( cp_wk->boot_data.move_frame == 4 ){
    return AC_WALK_U_4F;
  }else if( cp_wk->boot_data.move_frame == 6 ){
    return AC_WALK_U_6F;
  }else if( cp_wk->boot_data.move_frame == 8 ){
    return AC_WALK_U_8F;
  }else if( cp_wk->boot_data.move_frame == 16 ){
    return AC_WALK_U_16F;
  }
  // 対応していません。
  GF_ASSERT( 0 );
  return AC_WALK_U_8F;
}




//----------------------------------------------------------------------------
/**
 *	@brief  通常動作のセットアップ
 *
 *	@param	p_wk  ワーク
 */ 
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUpNormal( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  MMDL_SetStatusBitHeightGetOFF( p_wk->p_mmdl, TRUE );  // 高さ取得OFF
  MMDL_SetStatusBitAttrGetOFF( p_wk->p_mmdl, TRUE );    // アトリビュート取得OFF
  MMDL_SetStatusBitFellowHit( p_wk->p_mmdl, FALSE );    // 他のオブジェとの判定 OFF

  // イベント起動なし
  MMDL_SetEventID( p_wk->p_mmdl, SCRID_DUMMY );
}

//----------------------------------------------------------------------------
/**
 *	@brief  意地悪動作のセットアップ
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_SetUpNasty( FIELD_CROWD_PEOPLE_WK* p_wk )
{
  MMDL_SetStatusBitHeightGetOFF( p_wk->p_mmdl, TRUE );  // 高さ取得OFF
  MMDL_SetStatusBitAttrGetOFF( p_wk->p_mmdl, TRUE );    // アトリビュート取得OFF
  MMDL_SetStatusBitFellowHit( p_wk->p_mmdl, TRUE );    // 他のオブジェとの判定 ON

  // 最初は、イベント起動なし
  MMDL_SetEventID( p_wk->p_mmdl, SCRID_DUMMY );
}

//----------------------------------------------------------------------------
/**
 *	@brief  通常オブジェ動作
 *
 *	@param	p_wk        ワーク
 *	@param	p_fos       動作オブジェシステム
 *	@param	cp_player   主人公ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_MainNormal( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  switch( p_wk->seq )
  {
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT:// 動作完了待ち
    if( MMDL_CheckEndAcmd( p_wk->p_mmdl ) )
    {
      p_wk->seq ++;
    }
    else
    {
      break;
    }
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK:    // 判断＋指示

    // 終了処理
    if( p_wk->move_grid <= 0 )
    {
      // 終了
      p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_END;
      break;
    }

    {
      MMDL* p_hit_mmdl;
      // みんなとぶつかるかチェック
      if( FIELD_CROWD_PEOPLE_WK_IsFrontAllMMdl( p_wk, p_fos, &p_hit_mmdl, FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_GRID, 0 ) )
      {
        // 避ける動作
        FIELD_CROWD_PEOPLE_WK_ThroughMove( p_wk, p_hit_mmdl );
      }
      else
      {
        // そのまま進む
        MMDL_SetAcmd( p_wk->p_mmdl, p_wk->move_acmd + p_wk->move_dir );
        p_wk->move_grid --; // 移動距離減算
      }
    }
    p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT;
    break;

  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_END:     // 終了
    // 情報のクリア
    FIELD_CROWD_PEOPLE_WK_ClearMove( p_wk );
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  意地悪オブジェ動作  直進するのみです。
 *
 *	@param	p_wk        ワーク
 *	@param	p_fos       動作オブジェシステム
 *	@param	cp_player   主人公ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_WK_MainNasty( FIELD_CROWD_PEOPLE_WK* p_wk, MMDLSYS* p_fos, const FIELD_PLAYER* cp_player )
{
  switch( p_wk->seq )
  {
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT:// 動作完了待ち
    if( MMDL_CheckEndAcmd( p_wk->p_mmdl ) )
    {
      p_wk->seq ++;
    }
    else
    {
      break;
    }
  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALK:    // 判断＋指示

    // 終了処理
    if( p_wk->move_grid <= 0 )
    {
      // 終了
      p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_END;
      break;
    }

    {
      MMDL* p_hit_mmdl;

      // 主人公とぶつかるかチェック
      if( FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( p_wk, FIELD_PLAYER_GetMMdl(cp_player), FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID, FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID ) )
      {
        // ぶつかるときにしか話しかけられない。
        // イベント起動あり。
        MMDL_SetEventID( p_wk->p_mmdl, p_wk->event_id );
        break;
      }
      // その他の人とぶつかるかチェック
      else if( FIELD_CROWD_PEOPLE_WK_IsFrontAllMMdl( p_wk, p_fos, &p_hit_mmdl, FIELD_CROWD_PEOPLE_WK_PLAYER_HIT_NASTY_GRID, 0 ) )
      {
        // その他の人とぶつかってしまったら、通常動作に移行する。
        FIELD_CROWD_PEOPLE_WK_ThroughMove( p_wk, p_hit_mmdl );

        // 移動中は、イベント起動なし
        MMDL_SetEventID( p_wk->p_mmdl, SCRID_DUMMY );

        // 普通の群集に戻す。
        FIELD_CROWD_PEOPLE_WK_ChangeNormalMove( p_wk );
      }
      else
      {
        // そのまま進む
        MMDL_SetAcmd( p_wk->p_mmdl, p_wk->move_acmd + p_wk->move_dir );
        p_wk->move_grid --; // 移動距離減算

        // 移動中は、イベント起動なし
        MMDL_SetEventID( p_wk->p_mmdl, SCRID_DUMMY );

      }
      p_wk->seq = FIELD_CROWD_PEOPLE_SEQ_NORMAL_WALKWAIT;
    }
    break;

  case FIELD_CROWD_PEOPLE_SEQ_NORMAL_END:     // 終了
    // 情報のクリア
    FIELD_CROWD_PEOPLE_WK_ClearMove( p_wk );
    break;

  default:
    GF_ASSERT(0);
    break;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  あたらないか、全モデルでチェック
 *
 *	@param	cp_wk
 *	@param	cp_fos
 *	@param  pp_mmdl 当たったモデル
 *	@param	grid 
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontAllMMdl( const FIELD_CROWD_PEOPLE_WK* cp_wk, const MMDLSYS* cp_fos, MMDL** pp_mmdl, u16 grid, u16 min_grid )
{
	u32 no = 0;
	MMDL *mmdl;
  BOOL result = FALSE;

	while( MMDLSYS_SearchUseMMdl(cp_fos,&mmdl,&no) == TRUE ){
    if( cp_wk->p_mmdl != mmdl ){
      if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_FELLOW_HIT_NON) == 0 )
      {
        // 表示されている？ 
        if( MMDL_CheckStatusBitVanish( mmdl ) == FALSE ){
          result = FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( cp_wk, mmdl, grid, min_grid );
          if( result ){
            *pp_mmdl = mmdl;
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  grid歩前までに自機がいないかチェック
 *
 *	@param	cp_wk         ワーク
 *	@param	cp_player     プレイヤー
 *	@param  grid          グリッド
 *
 *	@retval TRUE  いる
 *	@retval FALSE いない
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_CROWD_PEOPLE_WK_IsFrontPlayer( const FIELD_CROWD_PEOPLE_WK* cp_wk, const MMDL* cp_mmdl, u16 grid, u16 min_grid )
{
  s16 player_gx, player_gz;
  s16 my_gx, my_gz;
  const MMDL* cp_playermmdl = cp_mmdl;
  
  player_gx = MMDL_GetGridPosX( cp_playermmdl );
  player_gz = MMDL_GetGridPosZ( cp_playermmdl );
  my_gx = MMDL_GetGridPosX( cp_wk->p_mmdl );
  my_gz = MMDL_GetGridPosZ( cp_wk->p_mmdl );
  
  if( FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( my_gx, my_gz, player_gx, player_gz, grid, min_grid, cp_wk->move_dir ) )
  {
    return TRUE;
  }
  player_gx = MMDL_GetOldGridPosX( cp_playermmdl );
  player_gz = MMDL_GetOldGridPosZ( cp_playermmdl );
  
  return FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( my_gx, my_gz, player_gx, player_gz, grid, min_grid, cp_wk->move_dir );
}

//----------------------------------------------------------------------------
/**
 *	@brief  grid歩前にいるのかのグリッド単位のチェック
 *
 *	@param	mygx
 *	@param	mygz
 *	@param	pl_gx
 *	@param	pl_gz
 *	@param	grid 
 *
 *	@retval TRUE  いる
 *	@retval FALSE いない
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_CROWD_PEOPLE_WK_TOOL_IsGridHit( s16 mygx, s16 mygz, s16 pl_gx, s16 pl_gz, u16 grid, u16 min_grid, u16 move_dir )
{
  s32 grid_dif;
  s32 grid_dif_2;

  switch( move_dir )
  {
  case DIR_UP:
    grid_dif = mygz - pl_gz;
    grid_dif_2 = mygx - pl_gx;
    break;
  case DIR_DOWN:
    grid_dif = pl_gz - mygz;
    grid_dif_2 = mygx - pl_gx;
    break;
  case DIR_LEFT:
    grid_dif = mygx - pl_gx;
    grid_dif_2 = mygz - pl_gz;
    break;
  case DIR_RIGHT:
    grid_dif = pl_gx - mygx;
    grid_dif_2 = mygz - pl_gz;
    break;

  default:
    GF_ASSERT(0);
    break;
  }

  if( ((grid_dif <= grid) && (grid_dif >= min_grid)) && (grid_dif_2 == 0) )
  {
    return TRUE;
  }
  return FALSE;
}




//----------------------------------------------------------------------------
/**
 *	@brief  騒音システム  初期化
 *
 *	@param	p_wk      ワーク
 *	@param	heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Init( FIELD_CROWD_PEOPLE_NOISE* p_wk, FLDMSGBG* p_fmb, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, int timezone, HEAPID heapID )
{
  int i;
  u32 rand = GFUser_GetPublicRand(0);
  
  p_wk->wait = 0;
  p_wk->wait_max = NOISE_TIME_MIN + ( rand % (cp_boot->boot_data.wait[timezone]*NOISE_TIME_WAIT_NUM) );
  p_wk->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_crowd_people_scr_dat, heapID );
  p_wk->p_fmb = p_fmb;

  p_wk->add_idx = rand % NOISE_WK_MAX;

  for( i=0; i<NOISE_WK_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_NOISEWK_Init( &p_wk->wk[i], i );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  騒音システム　破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Exit( FIELD_CROWD_PEOPLE_NOISE* p_wk )
{
  int i;
  // 全OFF
  for( i=0; i<NOISE_WK_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_NOISEWK_Off( &p_wk->wk[i], p_wk->p_fmb );
  }
  p_wk->p_fmb = NULL;
  
  GFL_MSG_Delete( p_wk->p_msgdata );
  p_wk->p_msgdata = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  騒音システム　総クリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Clear( FIELD_CROWD_PEOPLE_NOISE* p_wk )
{
  int i;
  // 全OFF
  for( i=0; i<NOISE_WK_MAX; i++ )
  {
    FIELD_CROWD_PEOPLE_NOISEWK_Off( &p_wk->wk[i], p_wk->p_fmb );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  騒音システム　メイン
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISE_Main( FIELD_CROWD_PEOPLE_NOISE* p_wk, const FIELD_CROWD_PEOPLE_BOOL_CONTROL* cp_boot, const FIELDMAP_WORK* cp_fieldmap )
{
  int i;
  u32 rand = GFUser_GetPublicRand(0);
  int timezone = FIELDMAP_GetSeasonTimeZone( cp_fieldmap );

  GF_ASSERT( timezone < TIMEZONE_MAX );

  if( FIELDMAP_CheckDoEvent( cp_fieldmap ) == FALSE )
  {
    p_wk->wait ++;

    // ウエイトたったら、ランダムメッセージ表示
    if( p_wk->wait >= p_wk->wait_max )
    {
      // 表示
      FIELD_CROWD_PEOPLE_NOISEWK_Start( &p_wk->wk[ p_wk->add_idx ], p_wk->p_fmb, p_wk->p_msgdata, cp_boot->boot_data.noise_start, cp_boot->boot_data.noise_num );
      p_wk->add_idx = (p_wk->add_idx + 1) % NOISE_WK_MAX;

      // ウエイト初期化
      p_wk->wait = 0;
      p_wk->wait_max = NOISE_TIME_MIN + (rand%(cp_boot->boot_data.wait[timezone]*NOISE_TIME_WAIT_NUM));
    }

    // メッセージ表示
    for( i=0; i<NOISE_WK_MAX; i++ )
    {
      FIELD_CROWD_PEOPLE_NOISEWK_Main( &p_wk->wk[i], p_wk->p_fmb );
    }
  }
  else
  {
    // 全OFF
    for( i=0; i<NOISE_WK_MAX; i++ )
    {
      FIELD_CROWD_PEOPLE_NOISEWK_Off( &p_wk->wk[i], p_wk->p_fmb );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *	@brief  ワークの初期化
 *
 *	@param	p_wk  ワーク
 *	@param	id    ID
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Init( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, u16 id )
{
  p_wk->flag = FALSE;

  // 各種情報は今決る
  p_wk->x     = sc_NOISE_X[id];
  p_wk->y     = sc_NOISE_Y[id];
  p_wk->id    = id;
  p_wk->wait  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  メッセージ開始
 *
 *	@param	p_wk
 *	@param	p_fmb 
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Start( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb, GFL_MSGDATA* p_msgdata, int noise_msg_start, int noise_msg_num )
{
  u32 rand = GFUser_GetPublicRand( 0 );
  p_wk->flag = TRUE;
  p_wk->wait = 0;
  
  FLDSUBMSGWIN_Add( p_fmb, p_msgdata, 
      noise_msg_start + (rand%noise_msg_num), p_wk->id,
      p_wk->x, p_wk->y, NOISE_SIZX, NOISE_SIZY );
}


//----------------------------------------------------------------------------
/**
 *	@brief  メイン処理
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Main( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb )
{
  if( p_wk->flag == FALSE )
  {
    return ;
  }
  
  // waitまって消える
  p_wk->wait ++;
  if( p_wk->wait >= NOISE_WK_MAX )
  {
    FLDSUBMSGWIN_Delete( p_fmb, p_wk->id );
    FIELD_CROWD_PEOPLE_NOISEWK_Clear( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワークOFF処理
 */ 
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Clear( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk )
{
  p_wk->wait = 0;
  p_wk->flag = FALSE;

  //その他のデータは次も使います
}

//----------------------------------------------------------------------------
/**
 *	@brief  OFFにする
 *
 *	@param	p_wk
 *	@param	p_fmb 
 */
//-----------------------------------------------------------------------------
static void FIELD_CROWD_PEOPLE_NOISEWK_Off( FIELD_CROWD_PEOPLE_NOISE_WK* p_wk, FLDMSGBG* p_fmb )
{
  if( p_wk->flag == FALSE )
  {
    return ;
  }
  
  FLDSUBMSGWIN_Delete( p_fmb, p_wk->id );
  FIELD_CROWD_PEOPLE_NOISEWK_Clear( p_wk );
}




