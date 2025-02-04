//======================================================================
/**
 * @file	field_player_nogrid.c
 * @brief	ノングリッド移動　フィールドプレイヤー制御
 */
//======================================================================
#include <gflib.h>

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "field_sound.h"


#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_player_core.h"
#include "field_camera.h"
#include "fldeff_shadow.h"
#include "field_rail.h"
#include "rail_attr.h"

#include "field_player_nogrid.h"

#include "include/gamesystem/pm_season.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

//======================================================================
//	define
//======================================================================
#define MV_SPEED (2*FX32_ONE) ///<移動速度


static MMDL_HEADER data_MMdlHeader =
{
	MMDL_ID_PLAYER,	///<識別ID
	HERO,	///<表示するOBJコード
	MV_RAIL_DMY,	///<動作コード
	0,	///<イベントタイプ
	0,	///<イベントフラグ
	0,	///<イベントID
	0,	///<指定方向
	0,	///<指定パラメタ 0
	0,	///<指定パラメタ 1
	0,	///<指定パラメタ 2
	MOVE_LIMIT_NOT,	///<X方向移動制限
	MOVE_LIMIT_NOT,	///<Z方向移動制限
  MMDL_HEADER_POSTYPE_RAIL,
};


//--------------------------------------------------------------
/// FIELD_PLAYER_NOGRID_MOVEBIT
//  field_player_gridの仕組みをコピー
//--------------------------------------------------------------
typedef enum
{
  ///特に無し
  FIELD_PLAYER_NOGRID_MOVEBIT_NON = (0),
  ///強制移動中
  FIELD_PLAYER_NOGRID_MOVEBIT_FORCE       = (1<<0),
  ///足元無効
  FIELD_PLAYER_NOGRID_MOVEBIT_UNDER_OFF   = (1<<1),
  ///一歩移動
  FIELD_PLAYER_NOGRID_MOVEBIT_STEP        = (1<<2),
  ///描画左回転
  FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L      = (1<<3),
  ///描画右回転
  FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R      = (1<<4),
  ///動作後強制右向き
  FIELD_PLAYER_NOGRID_MOVEBIT_TURN_R      = (1<<5),
  ///自動上昇
  FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP      = (1<<6),
  ///最大
  FIELD_PLAYER_NOGRID_MOVEBIT_BITMAX      = (1<<7),
}FIELD_PLAYER_NOGRID_MOVEBIT;

#define FIELD_PLAYER_NOGRID_MOVEBIT_MAX (3)

//--------------------------------------------------------------
///	PLAYER_MOVE
//--------------------------------------------------------------
typedef enum
{
	PLAYER_MOVE_STOP = 0,
	PLAYER_MOVE_WALK,
	PLAYER_MOVE_TURN,
	PLAYER_MOVE_HITCH,
} PLAYER_MOVE;



//--------------------------------------------------------------
/// UNDER
//--------------------------------------------------------------
typedef enum
{
  UNDER_NONE = 0, //何も無し
  UNDER_ICE, //滑る床
  UNDER_ICE_L, //滑る床 左回転
  UNDER_ICE_R, //滑る床 右回転
  UNDER_ICE_JUMP_L, //滑る床 左ジャンプ
  UNDER_ICE_JUMP_R, //滑る床 右ジャンプ
  UNDER_ICE_TURN_R, //滑る床 強制右向き
  UNDER_AUTO_UP, //自動上昇
  UNDER_MAX,
}UNDER;

//--------------------------------------------------------------
///	PLAYER_SET
//--------------------------------------------------------------
typedef enum
{
	PLAYER_SET_NON = 0,
	PLAYER_SET_STOP,
	PLAYER_SET_WALK,
	PLAYER_SET_TURN,
	PLAYER_SET_HITCH,
  PLAYER_SET_JUMP,
} PLAYER_SET;

//--------------------------------------------------------------
///	KURUKURU方向
//--------------------------------------------------------------
typedef enum
{
  KURUKURU_L,
  KURUKURU_R,

  KURUKURU_DIR_MAX,
} KURUKURU_DIR;



//--------------------------------------------------------------
///	自動上昇
//--------------------------------------------------------------
enum{
  AUTO_UP_NOINPUT_COUNT = 30, // 自動上昇が始まる値
};

//======================================================================
//	struct
//======================================================================
//-------------------------------------
///	回転ワーク
//=====================================
typedef struct 
{
  u8 kurukuru_flag;
  u8 left_or_right;
  s16 frame;
} KURUKURU_WORK;


//-------------------------------------
///	自動上昇ワーク
//=====================================
typedef struct {
  u8 auto_up_under:1; // 自動アップ床か？
  u8 auto_up_flag:1;  // 自動上昇中か？
  u8 auto_up_now_flag:1;  // 自動上昇開始か？
  u8 auto_up_time:5;  // 時間制限がきているか？
  s16 noinput_time; // 入力なし時間
  int save_dir;   //上昇中に押された方向を保存
} AUTO_UP_WORK;


//-------------------------------------
///	レール動作用補助ワーク
//=====================================
struct _FIELD_PLAYER_NOGRID
{
  int move_state;
  FIELD_PLAYER_NOGRID_MOVEBIT move_bit;
  KURUKURU_WORK kurukuru;
  AUTO_UP_WORK autoup;  //自動上昇ワーク
  
  FIELD_PLAYER_CORE* p_player_core;
	FIELDMAP_WORK* p_fieldwork;
  MMDL* p_mmdl;
  
  FIELD_RAIL_WORK* p_railwork;

} ;

//======================================================================
//	proto
//======================================================================

static PLAYER_SET nogridGetMoveStartSet( FIELD_PLAYER_NOGRID* p_player, int dir, int key_trg, int key_cont, BOOL debug );
static void nogridSetMove( FIELD_PLAYER_NOGRID* p_player, PLAYER_SET set, int dir, int key_trg, int key_cont, BOOL debug );
static void nogridMoveStartControl( FIELD_PLAYER_NOGRID* p_player, int key_cont, PLAYER_SET set );

static u32 nogrid_HitCheckMove( FIELD_PLAYER_NOGRID* p_player, MMDL *mmdl, u16 dir, MAPATTR* p_attr );

static void nogrid_KuruKuruMain( FIELD_PLAYER_NOGRID* p_player );


//自機動作ビット
static void nogrid_OnMoveBit(
    FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit );
static void nogrid_OffMoveBit(
    FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit );
static BOOL nogrid_CheckMoveBit(
    const FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit );
static void nogrid_OnMoveBitForce( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitForce( FIELD_PLAYER_NOGRID *nogrid );
static BOOL nogrid_CheckMoveBitForce( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitUnderOff( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitUnderOff( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitStep( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitStep( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitSpinL( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitSpinL( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitSpinR( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitSpinR( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitTurnR( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitTurnR( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OnMoveBitAutoUp( FIELD_PLAYER_NOGRID *nogrid );
static void nogrid_OffMoveBitAutoUp( FIELD_PLAYER_NOGRID *nogrid );

static UNDER nogrid_CheckUnder( const FIELD_PLAYER_NOGRID *nogrid );

static u16 nogrid_ControlUnder(
    FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static void nogrid_ControlUnder_Clear( FIELD_PLAYER_NOGRID *nogrid );

static u16 nogrid_ControlUnderIce( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderIceSpinL( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderIceSpinR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderIceJumpL( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderIceJumpR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderIceTurnR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );
static u16 nogrid_ControlUnderAutoUp( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug );


static u16 (* const nogrid_ControlUnderFunc[ UNDER_MAX ])( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug ) = 
{
  NULL,
  nogrid_ControlUnderIce,
  nogrid_ControlUnderIceSpinL,
  nogrid_ControlUnderIceSpinR,
  nogrid_ControlUnderIceJumpL,
  nogrid_ControlUnderIceJumpR,
  nogrid_ControlUnderIceTurnR,
  nogrid_ControlUnderAutoUp,
};



//======================================================================
//	kurukuru work
//======================================================================
static void nogrid_KuruKuru_Start( KURUKURU_WORK* wk, KURUKURU_DIR left_or_right );
static void nogrid_KuruKuru_Stop( KURUKURU_WORK* wk );
static void nogrid_KuruKuru_Main( KURUKURU_WORK* wk, MMDL* player );


//======================================================================
//	auto up work
//======================================================================
static void nogrid_AutoUp_Init( AUTO_UP_WORK* wk );
static void nogrid_AutoUp_Start( AUTO_UP_WORK* wk );
static void nogrid_AutoUp_Stop( AUTO_UP_WORK* wk );
static void nogrid_AutoUp_Main_Top( AUTO_UP_WORK* wk, int dir, const MMDL* mmdl );
static void nogrid_AutoUp_Main_Under( AUTO_UP_WORK* wk );
static int nogrid_AutoUp_GetDir( const AUTO_UP_WORK* wk, int dir );
static BOOL nogrid_AutoUp_IsAutoMove( const AUTO_UP_WORK* wk );


//======================================================================
//	proto　for nogrid_player
//======================================================================
//通常移動
static void jikiMove_Normal(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET jikiMove_Normal_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET player_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void player_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

//自転車移動
static void jikiMove_Cycle(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET jikiMove_Cycle_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void playerCycle_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

// 波乗り移動
static void jikiMove_Swim(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET jikiMove_Swim_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );
static PLAYER_SET playerSwim_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerSwim_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerSwim_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerSwim_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void playerSwim_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerSwim_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerSwim_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerSwim_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerSwim_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );




// snd
static BOOL player_CheckDirDoor( FIELD_PLAYER_NOGRID *p_player, u16 dir );


//----------------------------------------------------------------------------
/**
 *	@brief  フィールドノーグリッドプレイヤーワークの生成
 *
 *	@param	p_player    フィールドプレイヤー
 *	@param	heapID      ヒープID
 *
 *	@return ノーグリッド用ワーク
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_NOGRID* FIELD_PLAYER_NOGRID_Create( FIELD_PLAYER_CORE* p_player_core, HEAPID heapID )
{
  FIELD_PLAYER_NOGRID* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_NOGRID) );

  // ワークに保存
  p_wk->p_player_core = p_player_core;
  p_wk->p_fieldwork   = FIELD_PLAYER_CORE_GetFieldMapWork( p_player_core );
  p_wk->p_mmdl = FIELD_PLAYER_CORE_GetMMdl( p_wk->p_player_core );

  // 動作コードをレール動作に変更
  if( MMDL_GetMoveCode( p_wk->p_mmdl ) != MV_RAIL_DMY )
  {
    data_MMdlHeader.obj_code = MMDL_GetOBJCode( p_wk->p_mmdl );
    MMDL_ChangeMoveParam( p_wk->p_mmdl, &data_MMdlHeader );
  }


  // レールワークの取得
  p_wk->p_railwork = MMDL_GetRailWork( p_wk->p_mmdl );

  // 位置を初期か
  {
    RAIL_LOCATION location = {0};
    MMDL_SetRailLocation( p_wk->p_mmdl, &location );
  }

  // 各ワークの初期化
  nogrid_AutoUp_Init( &p_wk->autoup );


  // 状態の復帰
  {
    PLAYER_MOVE_FORM form;
    form = FIELD_PLAYER_CORE_GetMoveForm( p_wk->p_player_core );
    
    switch( form ){
    case PLAYER_MOVE_FORM_SWIM:
      FIELD_PLAYER_CORE_SetRequest(
          p_wk->p_player_core, FIELD_PLAYER_REQBIT_SWIM );
      FIELD_PLAYER_CORE_ForceUpdateRequest( p_wk->p_player_core );
      break;
    case PLAYER_MOVE_FORM_DIVING:
      FIELD_PLAYER_CORE_SetRequest(
          p_wk->p_player_core, FIELD_PLAYER_REQBIT_DIVING );
      FIELD_PLAYER_CORE_ForceUpdateRequest( p_wk->p_player_core );
      break;
    }
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フィールドノーグリッドプレイヤーワーク　破棄
 *
 *	@param	p_player  フィールドプレイヤー
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Delete( FIELD_PLAYER_NOGRID* p_player )
{
  GFL_HEAP_FreeMemory( p_player );
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作リセット　再構築
 *
 *	@param	p_player  プレイヤー
 *	@param	cp_pos    再始動座標
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Restart( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_pos )
{
  // 動作コードをレール動作に変更
  if( MMDL_GetMoveCode( p_player->p_mmdl ) != MV_RAIL_DMY )
  {
    data_MMdlHeader.obj_code = MMDL_GetOBJCode( p_player->p_mmdl );
    MMDL_ChangeMoveParam( p_player->p_mmdl, &data_MMdlHeader );
  }


  // レールワークの取得
  p_player->p_railwork = MMDL_GetRailWork( p_player->p_mmdl );

  // 位置を初期か
  MMDL_SetRailLocation( p_player->p_mmdl, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作停止
 *
 *	@param	p_player  プレイヤー
 *	@param	cp_pos    再始動座標
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Stop( FIELD_PLAYER_NOGRID* p_player )
{
  p_player->p_railwork = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  フィールドノーグリッドプレイヤーワーク  動作
 *
 *	@param	p_player      プレイヤー
 *	@param	key_trg       キートリガ
 *	@param	key_cont      キーコント
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Move( FIELD_PLAYER_NOGRID* p_player, int key_trg, int key_cont )
{
	u16 dir;
	BOOL debug_flag;
  PLAYER_SET set;

  GF_ASSERT( p_player->p_railwork );

  // キーから方向を取得
  dir = FIELD_PLAYER_CORE_GetKeyDir( p_player->p_player_core, key_cont );
	
	debug_flag = FALSE;
#ifdef PM_DEBUG
	if( key_cont & PAD_BUTTON_R ){
		debug_flag = TRUE;
	}
#endif

  FIELD_PLAYER_CORE_UpdateRequest( p_player->p_player_core );

  // 自動上昇処理　リクエスト処理前
  nogrid_AutoUp_Main_Top( &p_player->autoup, dir, p_player->p_mmdl );

  // 強制移動制御
  dir = nogrid_ControlUnder( p_player, dir, debug_flag );

  // 移動設定を取得
  set = nogridGetMoveStartSet( p_player, dir, key_trg, key_cont, debug_flag );

  // 移動開始、ビット管理
  nogridMoveStartControl( p_player, key_cont, set );
  
  // dir方向に移動
  nogridSetMove( p_player, set, dir, key_trg, key_cont, debug_flag );
  
  // くるくるまわす
  nogrid_KuruKuruMain( p_player );


  // 自動上昇処理　リクエスト処理後
  nogrid_AutoUp_Main_Under( &p_player->autoup );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ヒット移動チェック
 *
 *	@param	cp_player   プレイヤー
 *
 *	@retval TRUE  ヒット中
 *	@retval FALSE ひっとしてない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_PLAYER_NOGRID_IsHitch( const FIELD_PLAYER_NOGRID* cp_player )
{
  if( cp_player->move_state == PLAYER_MOVE_HITCH )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  オート動作チェック
 *
 *	@param	cp_player   プレイヤー
 *
 *	@retval TRUE  オート動作
 *	@retval FALSE オート動作じゃない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_PLAYER_NOGRID_IsAutoMove( const FIELD_PLAYER_NOGRID* cp_player )
{
  // 強制移動
  if( nogrid_CheckMoveBit(cp_player, FIELD_PLAYER_NOGRID_MOVEBIT_FORCE) )
  {
    return TRUE;
  }
  return FALSE;
}




//----------------------------------------------------------------------------
/**
 *	@brief  ロケーションの設定
 *
 *	@param	p_player      プレイヤー
 *	@param	cp_location   ロケーション
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_SetLocation( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_location )
{
  MMDL_SetRailLocation( p_player->p_mmdl, cp_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ロケーションの取得
 *
 *	@param	cp_player     プレイヤー
 *	@param	p_location    ロケーション格納先
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_GetLocation( const FIELD_PLAYER_NOGRID* cp_player, RAIL_LOCATION* p_location )
{
  MMDL_GetRailLocation( cp_player->p_mmdl, p_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Dirのロケーションを取得
 *
 *	@param	cp_player
 *	@param	dir
 *	@param	p_location 
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_GetDirLocation( const FIELD_PLAYER_NOGRID* cp_player, u16 dir, RAIL_LOCATION* p_location )
{
  MMDL_GetRailDirLocation( cp_player->p_mmdl, dir, p_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッドマッププレイヤー　position取得
 *
 *	@param	cp_player     プレイヤーワーク
 *	@param	p_pos         position
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_GetPos( const FIELD_PLAYER_NOGRID* cp_player, VecFx32* p_pos )
{
  GF_ASSERT( cp_player->p_railwork );
  FIELD_RAIL_WORK_GetPos( cp_player->p_railwork, p_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レールワークの取得
 *
 *	@param	cp_player   プレイヤー
 *
 *	@return レールワーク
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_PLAYER_NOGRID_GetRailWork( const FIELD_PLAYER_NOGRID* cp_player )
{
  GF_ASSERT( cp_player->p_railwork );
  return cp_player->p_railwork;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自機　強制停止　ノーグリッドマップ用
 *
 *	@param	p_player  プレイヤーワーク
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_ForceStop( FIELD_PLAYER_NOGRID* p_player )
{
  // 長いアクションコマンドなどが出来てきたら必要
  if( p_player->move_state == PLAYER_MOVE_HITCH )
  {
    MMDL *mmdl = p_player->p_mmdl;
    u16 dir = MMDL_GetDirDisp( mmdl );
    MMDL_FreeAcmd( mmdl );
    MMDL_SetDirDisp( mmdl, dir );
    MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
    FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
  }
}





#ifdef PM_DEBUG

static FLDMAPPER_GRIDINFODATA DEBUG_gridInfoData;

static void nogridPC_Move_SetValue( FIELD_PLAYER *fld_player,
		FIELDMAP_WORK *fieldWork, u16 key, VecFx32 *vec );
static BOOL nogridPC_Move_CalcSetGroundMove(
		const FLDMAPPER *g3Dmapper,
		FLDMAPPER_GRIDINFODATA* gridInfoData,
		VecFx32* pos, VecFx32* vecMove, fx32 speed );


//======================================================================
//	ノングリッド移動　フィールドプレイヤー制御
//======================================================================
//--------------------------------------------------------------
/**
 * 自機　ノングリッド移動
 * @param fld_player FIELD_PLAYER
 * @param key キー情報
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Free_Move( FIELD_PLAYER *fld_player, int key, fx32 onedist )
{
	VecFx32 pos;
	FIELDMAP_WORK *fieldWork;
	VecFx32	vecMove = { 0, 0, 0 };
	
	FIELD_PLAYER_GetPos( fld_player, &pos );
	fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
	
	nogridPC_Move_SetValue( fld_player, fieldWork, key, &vecMove );
	vecMove.x = FX_Mul( vecMove.x, onedist );	// 移動距離を設定
	vecMove.z = FX_Mul( vecMove.z, onedist );
	
	if (key & PAD_BUTTON_B) {
		VEC_Add( &pos, &vecMove, &pos );
    // 少数点を捨てる
    pos.x = FX_Floor( pos.x );  
    pos.y = FX_Floor( pos.y );  
    pos.z = FX_Floor( pos.z );  
		FIELD_PLAYER_SetPos( fld_player, &pos );
	} else {
		fx32 diff;
		nogridPC_Move_CalcSetGroundMove(
				FIELDMAP_GetFieldG3Dmapper(fieldWork),
				&DEBUG_gridInfoData,
				&pos, &vecMove, MV_SPEED );

    pos.x = FX_Floor( pos.x );  
    pos.y = FX_Floor( pos.y );  
    pos.z = FX_Floor( pos.z );  
		FIELD_PLAYER_SetPos( fld_player, &pos );
	}
}

//--------------------------------------------------------------
/**
 * 自機　ノングリッド移動 C3用
 * @param fld_player FIELD_PLAYER
 * @param key キー情報
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_C3_Move( FIELD_PLAYER *fld_player, int key, u16 angle )
{
	VecFx32	vecMove = { 0, 0, 0 };
	FIELDMAP_WORK *fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
	nogridPC_Move_SetValue( fld_player, fieldWork, key, &vecMove );
}



//======================================================================
//	ノングリッド自機　移動処理
//======================================================================
//--------------------------------------------------------------
/**
 * ノングリッド自機　移動制御　移動量、方向をセット
 * @param	fld_player FIELD_PLAYER
 * @param	fieldWork FIELDMAP_WORK
 * @param key キー情報
 * @param vec 移動量格納先
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogridPC_Move_SetValue( FIELD_PLAYER *fld_player,
		FIELDMAP_WORK *fieldWork, u16 key, VecFx32 *vec )
{
	u16		dir;
	MMDL *fmmdl;
	BOOL	mvFlag = FALSE;
	
	fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
	dir = FIELD_CAMERA_GetAngleYaw( FIELDMAP_GetFieldCamera(fieldWork) );
	
	if( key & PAD_KEY_UP )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x8000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x8000) );
		FIELD_PLAYER_SetDir( fld_player, dir );
		MMDL_SetDirDisp(fmmdl,DIR_UP);
	}

	if( key & PAD_KEY_DOWN )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x0000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x0000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0x8000 );
		MMDL_SetDirDisp(fmmdl, DIR_DOWN);
	}

	if( key & PAD_KEY_LEFT )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0xc000) );
		vec->z = FX_CosIdx( (u16)(dir + 0xc000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0x4000 );
		MMDL_SetDirDisp(fmmdl,DIR_LEFT);
	}

	if( key & PAD_KEY_RIGHT )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x4000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x4000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0xc000 );
		MMDL_SetDirDisp(fmmdl,DIR_RIGHT);
	}

	if (key & PAD_BUTTON_Y) {
		vec->y = -2 * FX32_ONE;
	}
	if (key & PAD_BUTTON_X) {
		vec->y = +2 * FX32_ONE;
	}
	
	if( mvFlag == TRUE ){
		if( key & PAD_BUTTON_B )
		{	
			//MMDL_SetDrawStatus(fmmdl,DRAW_STA_HERO_DASH_4F);
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
		else
		{
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
	} else {
		MMDL_SetDrawStatus(fmmdl,DRAW_STA_STOP);
	}
}

//--------------------------------------------------------------
/**
 * 移動方向の地形に沿ったベクトル取得
 * @param	vecN 地形ベクトル
 * @param	pos 座標
 * @param	vecMove 移動ベクトル
 * @param	result 結果格納先
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogridPC_Move_GetGroundMoveVec(
	const VecFx16* vecN, const VecFx32* pos,
	const VecFx32* vecMove, VecFx32* result )
{
	VecFx32	vecN32, posNext;
	fx32	by, valD;
	
	VEC_Add( pos, vecMove, &posNext );

	VEC_Set( &vecN32, vecN->x, vecN->y, vecN->z );
	valD = -( FX_Mul(vecN32.x,pos->x) + FX_Mul(vecN32.y,pos->y) + FX_Mul(vecN32.z,pos->z) ); 
	by = -( FX_Mul( vecN32.x, posNext.x ) + FX_Mul( vecN32.z, posNext.z ) + valD );
	posNext.y = FX_Div( by, vecN32.y );

	VEC_Subtract( &posNext, pos, result );
	VEC_Normalize( result, result );
}

//--------------------------------------------------------------
/**
 * ノングリッド移動　移動計算
 * @param	g3Dmapper FLDMAPPER*
 * @param gridInfoData FLDMAPPER_GRIDINFODATA
 * @param pos 座標
 * @param vecMove 移動量
 * @param speed 移動速度
 * @retval BOOL FALSE=移動不可
 */
//--------------------------------------------------------------
static BOOL nogridPC_Move_CalcSetGroundMove(
		const FLDMAPPER *g3Dmapper,
		FLDMAPPER_GRIDINFODATA* gridInfoData,
		VecFx32* pos, VecFx32* vecMove, fx32 speed )
{
	FLDMAPPER_GRIDINFO gridInfo;
	VecFx32	posNext, vecGround;
	fx32	height = 0;
	BOOL	initSw = FALSE;

	//VEC_Normalize( &vecMove, &vecMove );
	//
	if( (gridInfoData->vecN.x|gridInfoData->vecN.y|gridInfoData->vecN.z) == 0 ){
		//vecN = {0,0,0}の場合は初期状態
		VecFx16	vecNinit = { 0, FX16_ONE, 0 };

		nogridPC_Move_GetGroundMoveVec( &vecNinit, pos, vecMove, &vecGround );
		initSw = TRUE;
	} else {
		nogridPC_Move_GetGroundMoveVec( &gridInfoData->vecN, pos, vecMove, &vecGround );
	}
	VEC_MultAdd( speed, &vecGround, pos, &posNext );
	if( posNext.y < 0 ){
		posNext.y = 0;	//ベースライン
	}
	if( FLDMAPPER_CheckOutRange( g3Dmapper, &posNext ) == TRUE ){
	//	OS_Printf("マップ範囲外で移動不可\n");
		return FALSE;
	}

	//プレーヤー用動作。この位置中心に高さデータが存在するため、すべて取得して設定
	if( FLDMAPPER_GetGridInfo( g3Dmapper, &posNext, &gridInfo ) == FALSE ){
		return FALSE;
	}

	if( gridInfo.count ){
		int		i = 0, p;
		fx32	h_tmp, diff1, diff2;

		height = gridInfo.gridData[0].height;
		p = 0;
		i++;
		for( ; i<gridInfo.count; i++ ){
			h_tmp = gridInfo.gridData[i].height;

			diff1 = height - pos->y;
			diff2 = h_tmp - pos->y;

			if( FX_Mul( diff2, diff2 ) < FX_Mul( diff1, diff1 ) ){
				height = h_tmp;
				p = i;
			}
		}
		*gridInfoData = gridInfo.gridData[p];	//グリッドデータ更新
		VEC_Set( pos, posNext.x, gridInfoData->height, posNext.z );		//位置情報更新
	}
	return TRUE;
}
#endif // PM_DEBUG  フリームーブ






//----------------------------------------------------------------------------
/**
 *	@brief  各自機移動の移動情報を取得する
 *  
 *	@param	p_player    自機ワーク
 *	@param	dir         ほうこう
 *	@param	key_trg     キー
 *	@param	key_cont
 *	@param	debug       デバックフラグ
 *
 *	@return DIRから求めた移動情報
 */
//-----------------------------------------------------------------------------
static PLAYER_SET nogridGetMoveStartSet( FIELD_PLAYER_NOGRID* p_player, int dir, int key_trg, int key_cont, BOOL debug )
{
  PLAYER_MOVE_FORM form;
  PLAYER_SET set;

  form = FIELD_PLAYER_CORE_GetMoveForm( p_player->p_player_core );
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    set = jikiMove_Normal_GetSet( p_player, key_trg, key_cont, dir, debug );
	  break;
  case PLAYER_MOVE_FORM_CYCLE:
    set = jikiMove_Cycle_GetSet( p_player, key_trg, key_cont, dir, debug );
	  break;
  case PLAYER_MOVE_FORM_SWIM:
  case PLAYER_MOVE_FORM_DIVING:
    set = jikiMove_Swim_GetSet( p_player, key_trg, key_cont, dir, debug );
    break;
  default:
    GF_ASSERT( 0 );
  }

  return set;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自機動作の設定
 *
 *	@param	p_player    自機ワーク
 *	@param  dir         方向
 *	@param	key_trg     トリガ
 *	@param	key_cont    コント
 *	@param	debug       デバック
 */
//-----------------------------------------------------------------------------
static void nogridSetMove( FIELD_PLAYER_NOGRID* p_player, PLAYER_SET set, int dir, int key_trg, int key_cont, BOOL debug )
{
  PLAYER_MOVE_FORM form;

  form = FIELD_PLAYER_CORE_GetMoveForm( p_player->p_player_core );
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    jikiMove_Normal( p_player, set, key_trg, key_cont, dir, debug );
	  break;
  case PLAYER_MOVE_FORM_CYCLE:
    jikiMove_Cycle( p_player, set, key_trg, key_cont, dir, debug );
	  break;
  case PLAYER_MOVE_FORM_SWIM:
  case PLAYER_MOVE_FORM_DIVING:
    jikiMove_Swim( p_player, set, key_trg, key_cont, dir, debug );
    break;
  default:
    GF_ASSERT( 0 );
  }



#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
  {
    FIELD_RAIL_WORK_DEBUG_PrintRailGrid( p_player->p_railwork );
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作開始でのビット管理
 *
 *	@param	p_player    自機ワーク
 *	@param	set         自機動作情報
 */
//-----------------------------------------------------------------------------
static void nogridMoveStartControl( FIELD_PLAYER_NOGRID* p_player, int key_cont, PLAYER_SET set )
{
  FIELD_PLAYER_CORE_SetMoveStartKeyDir( p_player->p_player_core, key_cont );
  nogrid_OffMoveBitStep( p_player );
  
  if( set == PLAYER_SET_WALK ){
    nogrid_OffMoveBitUnderOff( p_player );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作チェック共通処理
 *
 *	@param	p_player    プレイヤーワーク
 *	@param	mmdl        モデル
 *	@param	dir         ほうこう
 *	@param  p_attr      移動先アトリビュート格納先
 *
 *	@return ヒット情報
 */
//-----------------------------------------------------------------------------
static u32 nogrid_HitCheckMove( FIELD_PLAYER_NOGRID* p_player, MMDL *mmdl, u16 dir, MAPATTR* p_attr )
{
  u32 hit = MMDL_HitCheckRailMoveDirEx( mmdl, dir, p_attr );
  return hit;
}

//----------------------------------------------------------------------------
/**
 *	@brief  MoveBitをみて、自機の絵を回転させる
 *
 *	@param	p_player  プレイヤー
 */
//-----------------------------------------------------------------------------
static void nogrid_KuruKuruMain( FIELD_PLAYER_NOGRID* p_player )
{
  // 回転開始
  if( nogrid_CheckMoveBit( p_player, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L ) )
  {
    nogrid_KuruKuru_Start( &p_player->kurukuru, KURUKURU_L );
  }
  else if( nogrid_CheckMoveBit( p_player, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R ) )
  {
    nogrid_KuruKuru_Start( &p_player->kurukuru, KURUKURU_R );
  }
  // 停止
  else
  {
    nogrid_KuruKuru_Stop( &p_player->kurukuru );
  }

  // メイン動作
  nogrid_KuruKuru_Main( &p_player->kurukuru, p_player->p_mmdl );
}






//======================================================================
//  グリッド移動　フィールドプレイヤー制御　通常移動
//======================================================================
//--------------------------------------------------------------
/**
 * グリッド移動　フィールドプレイヤー制御　通常移動
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Normal(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	MMDL *mmdl = p_player->p_mmdl;
   
	switch( set ){
	case PLAYER_SET_NON:
		player_SetMove_Non(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_STOP:
		player_SetMove_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_WALK:
		player_SetMove_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_TURN:
		player_SetMove_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_HITCH:
		player_SetMove_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
  case PLAYER_SET_JUMP:
		player_SetMove_Jump(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  PLAYER_SETの取得
 */
//-----------------------------------------------------------------------------
static PLAYER_SET jikiMove_Normal_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	PLAYER_SET set;
	MMDL *mmdl = p_player->p_mmdl;

	set = PLAYER_SET_NON;
	switch( p_player->move_state ){
	case PLAYER_MOVE_STOP:
		set = player_CheckMoveStart_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_WALK:
		set = player_CheckMoveStart_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_TURN:
		set = player_CheckMoveStart_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_HITCH:
		set = player_CheckMoveStart_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	default:
		GF_ASSERT( 0 );
	}
  return set;
}

//======================================================================
//	移動開始チェック　通常移動
//======================================================================
//--------------------------------------------------------------
/**
 * 移動開始チェック　停止中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
		if( dir != DIR_NOT ){
			u16 old_dir;
			old_dir = MMDL_GetDirDisp( mmdl );
			
			if( dir != old_dir && debug_flag == FALSE ){
				return( PLAYER_SET_TURN );
			}
			
			return( player_CheckMoveStart_Walk(
				p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
		}
		
		return( PLAYER_SET_STOP );
	}
	
	return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　移動中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT )
  {
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	{
		u32 hit = nogrid_HitCheckMove( p_player, mmdl, dir, NULL );

		if( debug_flag == TRUE )
    {
      // アトリビュートムシ
			if( hit & MMDL_MOVEHITBIT_ATTR )
      {
				hit = MMDL_MOVEHITBIT_NON;
			}
		}
		
		if( hit == MMDL_MOVEHITBIT_NON )
    {
			return( PLAYER_SET_WALK );
		}

    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      RAIL_LOCATION location;
      u32 attr;
      
      // Dir方向先のロケーション取得
      MMDL_GetRailDirLocation( mmdl, dir, &location );
      attr = MMDL_GetRailLocationAttr( mmdl, &location );
    
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //ジャンプ方向一致
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
	}
	
	return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　振り向き中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( player_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　障害物ヒット中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
	  return( player_CheckMoveStart_Walk(
		  p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( player_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//	移動セット　通常移動
//======================================================================
//--------------------------------------------------------------
/**
 * 移動開始セット 特に無し
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * 移動セット 停止
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	if( dir == DIR_NOT ){
		dir = MMDL_GetDirDisp( mmdl );
	}
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_STOP;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
}

//--------------------------------------------------------------
/**
 * 移動セット 移動
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	if( debug_flag == TRUE ){
		code = AC_WALK_U_2F;
	}else if( key_cont & PAD_BUTTON_B ){
		code = AC_DASH_U_4F;
	}else{
		code = AC_WALK_U_8F;
	}
  
	code = MMDL_ChangeDirAcmdCode( dir, code );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_WALK );
}

//--------------------------------------------------------------
/**
 * 移動セット 振り向き
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_TURN;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_TURN );
}

//--------------------------------------------------------------
/**
 * 移動セット 障害物ヒット
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_HITCH;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );

  if( (player_CheckDirDoor(p_player,dir) == FALSE) ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }
}

//--------------------------------------------------------------
/**
 * 移動セット ジャンプ
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_WALK );
}




//======================================================================
//  グリッド移動　フィールドプレイヤー制御　自転車移動
//======================================================================
//--------------------------------------------------------------
/**
 * グリッド移動　フィールドプレイヤー制御　自転車移動
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Cycle(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	MMDL *mmdl = p_player->p_mmdl;
   
	
	switch( set ){
	case PLAYER_SET_NON:
		playerCycle_SetMove_Non(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_STOP:
		playerCycle_SetMove_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_WALK:
		playerCycle_SetMove_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_TURN:
		playerCycle_SetMove_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_HITCH:
		playerCycle_SetMove_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
  case PLAYER_SET_JUMP:
		playerCycle_SetMove_Jump(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  PLAYER_SETの取得　自転車
 */
//-----------------------------------------------------------------------------
static PLAYER_SET jikiMove_Cycle_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	PLAYER_SET set;
	MMDL *mmdl = p_player->p_mmdl;

	set = PLAYER_SET_NON;
	switch( p_player->move_state ){
	case PLAYER_MOVE_STOP:
		set = playerCycle_CheckMoveStart_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_WALK:
		set = playerCycle_CheckMoveStart_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_TURN:
		set = playerCycle_CheckMoveStart_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_HITCH:
		set = playerCycle_CheckMoveStart_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	default:
		GF_ASSERT( 0 );
	}

  return set;
}

//======================================================================
//	移動開始チェック　自転車移動
//======================================================================
//--------------------------------------------------------------
/**
 * 移動開始チェック　停止中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
		if( dir != DIR_NOT ){
			u16 old_dir;
			old_dir = MMDL_GetDirDisp( mmdl );
			
			if( dir != old_dir && debug_flag == FALSE ){
				return( PLAYER_SET_TURN );
			}
			
			return( playerCycle_CheckMoveStart_Walk(
				p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
		}
		
		return( PLAYER_SET_STOP );
	}
	
	return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　移動中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT )
  {
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	{
		u32 hit = nogrid_HitCheckMove( p_player, mmdl, dir, NULL );
		
		if( debug_flag == TRUE )
    {
      // アトリビュートムシ
			if( hit & MMDL_MOVEHITBIT_ATTR )
      {
				hit = MMDL_MOVEHITBIT_NON;
			}
		}
		
		if( hit == MMDL_MOVEHITBIT_NON )
    {
			return( PLAYER_SET_WALK );
		}
    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      RAIL_LOCATION location;
      u32 attr;
      
      // Dir方向先のロケーション取得
      MMDL_GetRailDirLocation( mmdl, dir, &location );
      attr = MMDL_GetRailLocationAttr( mmdl, &location );
    
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //ジャンプ方向一致
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
	}
	
	return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　振り向き中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( playerCycle_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * 移動開始チェック　障害物ヒット中
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param dir 移動方向
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
	  return( playerCycle_CheckMoveStart_Walk(
		  p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( playerCycle_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//	移動セット　自転車移動
//======================================================================
//--------------------------------------------------------------
/**
 * 移動開始セット 特に無し
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * 移動セット 停止
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	if( dir == DIR_NOT ){
		dir = MMDL_GetDirDisp( mmdl );
	}
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_STOP;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
}

//--------------------------------------------------------------
/**
 * 移動セット 移動
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	if( debug_flag == TRUE ){
		code = AC_WALK_U_2F;
	}else{
		code = AC_WALK_U_2F;
	}
  
	code = MMDL_ChangeDirAcmdCode( dir, code );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_WALK );
}

//--------------------------------------------------------------
/**
 * 移動セット 振り向き
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_TURN;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_TURN );
}

//--------------------------------------------------------------
/**
 * 移動セット 障害物ヒット
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_HITCH;
	
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
  if( (player_CheckDirDoor(p_player,dir) == FALSE) ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }
}

//--------------------------------------------------------------
/**
 * 移動セット ジャンプ
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @param debug_flag デバッグ移動可能フラグ TRUE=可能
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_WALK );
}

// 波乗り移動
static void jikiMove_Swim(
		FIELD_PLAYER_NOGRID *p_player, PLAYER_SET set, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( p_player->p_player_core );

  switch( set ){
  case PLAYER_SET_NON:
    playerSwim_SetMove_Non(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_STOP:
    playerSwim_SetMove_Stop(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_WALK:
    playerSwim_SetMove_Walk(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_TURN:
    playerSwim_SetMove_Turn(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_HITCH:
    playerSwim_SetMove_Hitch(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
    break;
  }
}

static void playerSwim_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

static void playerSwim_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  p_player->move_state = PLAYER_MOVE_STOP;
  
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
}

static void playerSwim_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  if( debug_flag == TRUE ){
    code = AC_WALK_U_2F;
  }else{
    code = AC_WALK_U_4F;
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  p_player->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_WALK );
}

static void playerSwim_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
  
  MMDL_SetAcmd( mmdl, code );
  p_player->move_state = PLAYER_MOVE_TURN;
  
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_TURN );
}

static void playerSwim_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  p_player->move_state = PLAYER_MOVE_HITCH;
  
  FIELD_PLAYER_CORE_SetMoveValue( p_player->p_player_core, PLAYER_MOVE_VALUE_STOP );
  if( (player_CheckDirDoor(p_player,dir) == FALSE) ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }
}

// PLAYER_SET
static PLAYER_SET jikiMove_Swim_GetSet(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
  PLAYER_SET set;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( p_player->p_player_core );
   
  set = PLAYER_SET_NON;
  switch( p_player->move_state ){
  case PLAYER_MOVE_STOP:
    set = playerSwim_CheckMoveStart_Stop(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_WALK:
    set = playerSwim_CheckMoveStart_Walk(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_TURN:
    set = playerSwim_CheckMoveStart_Turn(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_HITCH:
    set = playerSwim_CheckMoveStart_Hitch(
      p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

static PLAYER_SET playerSwim_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( PLAYER_SET_TURN );
      }
      
      return( playerSwim_CheckMoveStart_Walk(
        p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( PLAYER_SET_STOP );
  }
  
  return( PLAYER_SET_NON );
}

static PLAYER_SET playerSwim_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( playerSwim_CheckMoveStart_Stop(
      p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = nogrid_HitCheckMove( p_player, mmdl, dir, &attr );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit & MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( attr != MAPATTR_ERROR ){
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
      
      if( hit == MMDL_MOVEHITBIT_NON ){
        if( (flag & MAPATTR_FLAGBIT_WATER) ){
          return( PLAYER_SET_WALK );
        }
        
        if( debug_flag == TRUE ){
          return( PLAYER_SET_WALK );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}

static PLAYER_SET playerSwim_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( playerSwim_CheckMoveStart_Stop(
      p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( playerSwim_CheckMoveStart_Walk(
    p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

static PLAYER_SET playerSwim_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( playerSwim_CheckMoveStart_Walk(
      p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( playerSwim_CheckMoveStart_Stop(
      p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( playerSwim_CheckMoveStart_Walk(
    p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}



//======================================================================
//  自機動作ビット
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT ON
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OnMoveBit(
    FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_NOGRID_MOVEBIT_BITMAX );
  nogrid->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT OFF
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OffMoveBit(
    FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_NOGRID_MOVEBIT_BITMAX );
  nogrid->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT Check
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static BOOL nogrid_CheckMoveBit(
    const FIELD_PLAYER_NOGRID *nogrid, FIELD_PLAYER_NOGRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_NOGRID_MOVEBIT_BITMAX );
  if( (nogrid->move_bit & bit) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 強制移動ビットON
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OnMoveBitForce( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 強制移動ビットOFF
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OffMoveBitForce( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 強制移動ビットチェック
 * @param nogrid FIELD_PLAYER_NOGRID
 * @retval BOOL 
 */
//--------------------------------------------------------------
static BOOL nogrid_CheckMoveBitForce( FIELD_PLAYER_NOGRID *nogrid )
{
  if( nogrid_CheckMoveBit(nogrid,FIELD_PLAYER_NOGRID_MOVEBIT_FORCE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 足元無効ON
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OnMoveBitUnderOff( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 足元無効OFF
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OffMoveBitUnderOff( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 一歩移動 ON
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OnMoveBitStep( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_STEP );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_NOGRID_MOVEBIT 一歩移動 OFF
 * @param nogrid FIELD_PLAYER_NOGRID
 * @param bit FIELD_PLAYER_NOGRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogrid_OffMoveBitStep( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_STEP );
}

//----------------------------------------------------------------------------
/**
 *	@brief  左回り回転　ON
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OnMoveBitSpinL( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L );
}

//----------------------------------------------------------------------------
/**
 *	@brief  左回り回転　OFF
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OffMoveBitSpinL( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L );
}

//----------------------------------------------------------------------------
/**
 *	@brief  右回り回転　ON
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OnMoveBitSpinR( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R );
}

//----------------------------------------------------------------------------
/**
 *	@brief  右回り回転　OFF
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OffMoveBitSpinR( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R );
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作後　強制的に右を向かせる　ON
 */
//-----------------------------------------------------------------------------
static void nogrid_OnMoveBitTurnR( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_TURN_R );
}

//----------------------------------------------------------------------------
/**
 *	@brief　動作後　強制的に右を向かせる　OFF
 */
//-----------------------------------------------------------------------------
static void nogrid_OffMoveBitTurnR( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_TURN_R );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　ON
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OnMoveBitAutoUp( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OnMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　OFF
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_OffMoveBitAutoUp( FIELD_PLAYER_NOGRID *nogrid )
{
  nogrid_OffMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP );
}





//--------------------------------------------------------------
/**
 * 自機足元をチェックする
 * @param
 * @retval
 */
//--------------------------------------------------------------
static UNDER nogrid_CheckUnder( const FIELD_PLAYER_NOGRID *nogrid )
{
  MMDL *mmdl = nogrid->p_mmdl;
  MAPATTR attr = MMDL_GetMapAttr( mmdl );
  
  if( nogrid_CheckMoveBit(
        nogrid,FIELD_PLAYER_NOGRID_MOVEBIT_UNDER_OFF) == FALSE ){
    int i = 0;
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
    
    if( MAPATTR_VALUE_CheckIce(val) == TRUE ){
      return( UNDER_ICE );
    }

    if( RAIL_ATTR_VALUE_CheckIceSpinL(val) == TRUE )
    {
      return ( UNDER_ICE_L );
    }
    if( RAIL_ATTR_VALUE_CheckIceSpinR(val) == TRUE )
    {
      return ( UNDER_ICE_R );
    }

    if( RAIL_ATTR_VALUE_CheckIceJumpL(val) == TRUE )
    {
      return ( UNDER_ICE_JUMP_L );
    }
    if( RAIL_ATTR_VALUE_CheckIceJumpR(val) == TRUE )
    {
      return ( UNDER_ICE_JUMP_R );
    }
    if( RAIL_ATTR_VALUE_CheckIceTurnR(val) == TRUE )
    {
      return ( UNDER_ICE_TURN_R );
    }
    if( RAIL_ATTR_VALUE_CheckDivingAutoUp(val) == TRUE )
    {
      return ( UNDER_AUTO_UP );
    }
  }
  
  return( UNDER_NONE );
}

//--------------------------------------------------------------
/**
 * 自機足元処理
 * @param
 * @retval
 */
//--------------------------------------------------------------
static u16 nogrid_ControlUnder(
    FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  MMDL *mmdl = nogrid->p_mmdl;

  // 移動完了しているか？
  if(MMDL_CheckPossibleAcmd(mmdl) == FALSE)
  {
    return dir;
  }
  
  if( debug == FALSE )
  {
    UNDER under = nogrid_CheckUnder( nogrid );
    
    if( under!=UNDER_NONE )
    {
      GF_ASSERT( nogrid_ControlUnderFunc[ under ] );
      return nogrid_ControlUnderFunc[ under ]( nogrid, dir, debug );
    }
  }
  
  // 強制移動停止
  nogrid_ControlUnder_Clear( nogrid );
  return( dir );
}

//----------------------------------------------------------------------------
/**
 *	@brief  足元処理の状態をクリア
 *
 *	@param	nogrid 
 */
//-----------------------------------------------------------------------------
static void nogrid_ControlUnder_Clear( FIELD_PLAYER_NOGRID *nogrid )
{
  MMDL *mmdl = nogrid->p_mmdl;


  // Pause情報のクリア
  if( nogrid_CheckMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_FORCE|FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L|FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R|FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP ) )
  {
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
  }

  // 強制移動OFF
  if( nogrid_CheckMoveBit(nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_FORCE) )
  {
    nogrid_OffMoveBitForce( nogrid );
    nogrid_OnMoveBitUnderOff( nogrid ); // 歩き出すまで強制移動はなし
    FIELD_PLAYER_CORE_SetMoveValue( nogrid->p_player_core, PLAYER_MOVE_VALUE_STOP );
  }

  // 回転OFF
  if( nogrid_CheckMoveBit(nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_L|FIELD_PLAYER_NOGRID_MOVEBIT_SPIN_R) )
  {
    u16 dir;
    
    nogrid_OffMoveBitSpinL( nogrid );
    nogrid_OffMoveBitSpinR( nogrid );

    // 描画方向の調整
    dir = MMDL_GetDirMove( mmdl );
    dir = MMDL_TOOL_FlipDir( dir );
    MMDL_SetDirAll( mmdl, dir );
  }

  // 強制的に右を向かせる？
  if( nogrid_CheckMoveBit(nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_TURN_R)  )
  {
    nogrid_OffMoveBitTurnR( nogrid );
    MMDL_SetDirAll( mmdl, DIR_RIGHT );
  }

  // 自動上昇ワークのクリア
  if( nogrid_CheckMoveBit(nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP) )
  {
    nogrid_OffMoveBitAutoUp( nogrid );
    nogrid_AutoUp_Stop( &nogrid->autoup );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  氷床処理
 *
 *	@param	nogrid    ノーグリッド
 *	@param	dir       ほうこう
 *	@param	debug     デバック
 *
 *	@return 方向
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIce( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  MAPATTR attr;
  MMDL *mmdl = nogrid->p_mmdl;
  u16 jiki_dir = MMDL_GetDirMove( mmdl );
  u32 hit = nogrid_HitCheckMove( nogrid, mmdl, jiki_dir, NULL );
	
  if( hit != MMDL_MOVEHITBIT_NON ){ //障害物ヒット
    nogrid_ControlUnder_Clear( nogrid );
    return( dir );
  }
  
  nogrid_OnMoveBitForce( nogrid );

  // スピン停止
  nogrid_OffMoveBitSpinL(nogrid);
  nogrid_OffMoveBitSpinR(nogrid);

  // 向きを合わせる
  MMDL_SetDirDisp( mmdl, jiki_dir );
  
  {
	  u16 code = MMDL_ChangeDirAcmdCode( jiki_dir, AC_WALK_U_4F );
    
    MMDL_SetAcmd( mmdl, code );
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    
    nogrid->move_state = PLAYER_MOVE_WALK;
    FIELD_PLAYER_CORE_SetMoveValue( nogrid->p_player_core, PLAYER_MOVE_VALUE_WALK );
	  return( jiki_dir );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  氷強制移動＋左回転
 *
 *	@param	nogrid
 *	@param	dir
 *	@param	debug 
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIceSpinL( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  MAPATTR attr;
  MMDL *mmdl = nogrid->p_mmdl;
  u16 jiki_dir = MMDL_GetDirMove( mmdl );
  u32 hit = nogrid_HitCheckMove( nogrid, mmdl, jiki_dir, NULL );
	
  if( hit != MMDL_MOVEHITBIT_NON ){ //障害物ヒット
    nogrid_ControlUnder_Clear( nogrid );
    return( dir );
  }
  
  nogrid_OnMoveBitForce( nogrid );
  nogrid_OnMoveBitSpinL( nogrid );

  // スピン停止
  nogrid_OffMoveBitSpinR(nogrid);
  {
	  u16 code = MMDL_ChangeDirAcmdCode( jiki_dir, AC_WALK_U_4F );
    
    MMDL_SetAcmd( mmdl, code );
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    
    nogrid->move_state = PLAYER_MOVE_WALK;
    FIELD_PLAYER_CORE_SetMoveValue( nogrid->p_player_core, PLAYER_MOVE_VALUE_WALK );
	  return( jiki_dir );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  氷強制移動＋右回転
 *
 *	@param	nogrid
 *	@param	dir
 *	@param	debug 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIceSpinR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  MAPATTR attr;
  MMDL *mmdl = nogrid->p_mmdl;
  u16 jiki_dir = MMDL_GetDirMove( mmdl );
  u32 hit = nogrid_HitCheckMove( nogrid, mmdl, jiki_dir, NULL );
	
  if( hit != MMDL_MOVEHITBIT_NON ){ //障害物ヒット
    nogrid_ControlUnder_Clear( nogrid );
    return( dir );
  }
  
  nogrid_OnMoveBitForce( nogrid );
  nogrid_OnMoveBitSpinR( nogrid );
  
  // スピン停止
  nogrid_OffMoveBitSpinL(nogrid);
  {
	  u16 code = MMDL_ChangeDirAcmdCode( jiki_dir, AC_WALK_U_4F );
    
    MMDL_SetAcmd( mmdl, code );
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    
    nogrid->move_state = PLAYER_MOVE_WALK;
    FIELD_PLAYER_CORE_SetMoveValue( nogrid->p_player_core, PLAYER_MOVE_VALUE_WALK );
	  return( jiki_dir );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  氷すべり　＋　ジャンプ音
 *
 *	@param	nogrid
 *	@param	dir
 *	@param	debug 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIceJumpL( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  // 向いている方向を取得
  u16 move_dir = MMDL_GetDirMove( nogrid->p_mmdl );
  
  if( move_dir == DIR_LEFT )
  {
    PMSND_PlaySE( SEQ_SE_DANSA );
  }
  return nogrid_ControlUnderIce( nogrid, dir, debug );
}

//----------------------------------------------------------------------------
/**
 *	@brief  氷すべり＋ジャンプ音
 *
 *	@param	nogrid
 *	@param	dir
 *	@param	debug 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIceJumpR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  u16 move_dir = MMDL_GetDirMove( nogrid->p_mmdl );
  
  if( move_dir == DIR_RIGHT )
  {
    PMSND_PlaySE( SEQ_SE_DANSA );
  }
  return nogrid_ControlUnderIce( nogrid, dir, debug );
}

//----------------------------------------------------------------------------
/**
 *	@brief  氷すべり＋右強制向き
 *
 *	@param	nogrid
 *	@param	dir
 *	@param	debug 
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderIceTurnR( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  u16 ret_dir;

  ret_dir = nogrid_ControlUnderIce( nogrid, dir, debug );
  
  // 動作後強制的に右向きに
  nogrid_OnMoveBitTurnR( nogrid );

  return ret_dir;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇
 */
//-----------------------------------------------------------------------------
static u16 nogrid_ControlUnderAutoUp( FIELD_PLAYER_NOGRID *nogrid, u16 dir, BOOL debug )
{
  u16 ret_dir;
  
  if( !nogrid_CheckMoveBit( nogrid, FIELD_PLAYER_NOGRID_MOVEBIT_AUTO_UP ) ){
    nogrid_OnMoveBitAutoUp( nogrid );
    nogrid_AutoUp_Start( &nogrid->autoup );
  }

  // 自動上昇ワークを使い方向を決める
  ret_dir = nogrid_AutoUp_GetDir( &nogrid->autoup, dir );

  // 自動上昇するのか？
  if( nogrid_AutoUp_IsAutoMove( &nogrid->autoup ) ){
    MMDL_OnStatusBit( nogrid->p_mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );

    // 自動上昇
    {
      u16 code = MMDL_ChangeDirAcmdCode( ret_dir, AC_WALK_U_16F );
      
      MMDL_SetAcmd( nogrid->p_mmdl, code );
	    //nogrid->move_state = PLAYER_MOVE_WALK;  //歩いていることにしてみる。
      FIELD_PLAYER_CORE_SetMoveValue( nogrid->p_player_core, PLAYER_MOVE_VALUE_WALK );
    }
    ret_dir = DIR_NOT;
    
  }else{
    
    MMDL_OffStatusBit( nogrid->p_mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
  }

  return ret_dir;
}




//----------------------------------------------------------------------------
/**
 *	@brief  くるくる処理　開始
 *
 *	@param	wk              ノーグリッド
 *	@param	left_or_right   左回転　右回転
 */
//-----------------------------------------------------------------------------
static void nogrid_KuruKuru_Start( KURUKURU_WORK* wk, KURUKURU_DIR left_or_right )
{
  // すでに設定済みでないかチェック
  if( (wk->kurukuru_flag == TRUE) && (left_or_right == wk->left_or_right) )
  {
    return;
  }

  // くるくるSE開始
  PMSND_PlaySE( FIELD_PLAYER_SE_NOGRID_ICE_SPIN );

  wk->kurukuru_flag = TRUE;
  wk->left_or_right = left_or_right;
  wk->frame         = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  くるくる処理　停止
 *
 *	@param	wk 
 */
//-----------------------------------------------------------------------------
static void nogrid_KuruKuru_Stop( KURUKURU_WORK* wk )
{
  if( wk->kurukuru_flag ){
    wk->kurukuru_flag = FALSE;
    wk->frame         = 0;

    // くるくるSE停止
    PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID( FIELD_PLAYER_SE_NOGRID_ICE_SPIN ) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  くるくる処理　メイン
 *
 *	@param	wk 
 */
//-----------------------------------------------------------------------------
static void nogrid_KuruKuru_Main( KURUKURU_WORK* wk, MMDL* player )
{
  static const u8 sc_DATA_KuruKuruTbl[KURUKURU_DIR_MAX][4] = 
  {
    {DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT},
    {DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT},
  };

  if( !wk->kurukuru_flag )
  {
    return ;
  }
  wk->frame = (wk->frame + 1) % 4;
  MMDL_SetForceDirDisp( player, sc_DATA_KuruKuruTbl[wk->left_or_right][wk->frame] ); 
}





//======================================================================
//	auto up work
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　初期化
 *
 *	@param	wk  ワーク
 */
//-----------------------------------------------------------------------------
static void nogrid_AutoUp_Init( AUTO_UP_WORK* wk )
{
  GFL_STD_MemClear( wk, sizeof(AUTO_UP_WORK) );
  wk->save_dir = DIR_NOT;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　開始
 */
//-----------------------------------------------------------------------------
static void nogrid_AutoUp_Start( AUTO_UP_WORK* wk )
{
  wk->auto_up_under = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　停止
 */
//-----------------------------------------------------------------------------
static void nogrid_AutoUp_Stop( AUTO_UP_WORK* wk )
{
  nogrid_AutoUp_Init( wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　メイン処理
 *
 *	@param	wk  ワーク
 *	@param	dir 今フレームの方向
 */
//-----------------------------------------------------------------------------
static void nogrid_AutoUp_Main_Top( AUTO_UP_WORK* wk, int dir, const MMDL* mmdl )
{
  if( wk->auto_up_under == FALSE ){
    return ;
  }
  
  // 時間制限が来ているときの処理
  if( wk->auto_up_time ){
    if( dir != DIR_NOT ){
      // 入力があった。
      // 時間制限処理は初期化
      wk->noinput_time = 0;
      wk->auto_up_time = FALSE;

      // もし自動上昇中なら方向を保持
      if( wk->auto_up_flag ){
        wk->save_dir = dir;
      }
    }
    
  // 時間制限が来ていないときの処理
  }else{
    if( dir == DIR_NOT ){
      if( wk->noinput_time < AUTO_UP_NOINPUT_COUNT ){
        wk->noinput_time ++;
      }else{
        // 時間制限がきた
        wk->auto_up_time = TRUE;
      }
    }
    
  }
  
  // もし、時間制限中で、動作モデルが動ける状態なら
  // 自動でアップ
  if( (MMDL_CheckPossibleAcmd(mmdl) == TRUE) ){
    if( wk->auto_up_time == FALSE ){
      wk->auto_up_flag = FALSE; //自動上昇終了
      wk->auto_up_now_flag = FALSE;
    }else{
      wk->auto_up_flag = TRUE;
      wk->auto_up_now_flag = TRUE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　動作後処理
 *
 *	@param	wk  ワーク
 */
//-----------------------------------------------------------------------------
static void nogrid_AutoUp_Main_Under( AUTO_UP_WORK* wk )
{
  if( wk->auto_up_now_flag ){
    wk->auto_up_now_flag = FALSE;
  }
  if( !wk->auto_up_flag ){
    wk->save_dir = DIR_NOT;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  自動上昇　移動方向を取得
 *
 *	@param	* wk  ワーク
 */
//-----------------------------------------------------------------------------
static int nogrid_AutoUp_GetDir( const AUTO_UP_WORK* wk, int dir )
{
  if( wk->auto_up_now_flag ){
    return DIR_DOWN;
  }else{
    if( wk->save_dir != DIR_NOT ){
      return wk->save_dir;
    }
  }


  return dir;
}

//----------------------------------------------------------------------------
/**
 *	@brief  自動動作  中かチェック
 *
 *	@param	wk ワーク
 *
 *	@retval TRUE  自動移動中
 */
//-----------------------------------------------------------------------------
static BOOL nogrid_AutoUp_IsAutoMove( const AUTO_UP_WORK* wk )
{
  return wk->auto_up_flag;
}





//----------------------------------------------------------------------------
/**
 *	@brief  Dir方向がドアかチェック
 *
 *	@param	p_player    プレイヤー
 *	@param	dir         ほうこう
 *
 *	@retval TRUE  ドア
 *	@retval FALSE じゃない
 */
//-----------------------------------------------------------------------------
static BOOL player_CheckDirDoor( FIELD_PLAYER_NOGRID *p_player, u16 dir )
{
  int idx;
  RAIL_LOCATION location;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( p_player->p_player_core );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( p_player->p_fieldwork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
  
  MMDL_GetRailDirLocation( mmdl, dir, &location );
	idx = EVENTDATA_SearchConnectIDByRailLocation( evdata, &location );
  
  if( idx == EXIT_ID_NONE ){
    return( FALSE );
  }
  return( TRUE );
}



