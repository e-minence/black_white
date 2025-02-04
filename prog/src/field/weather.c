//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather.c
 *	@brief		天気システム
 *	@author		tomoya takahashi
 *	@date		2009.03.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>

#include "weather_task.h"

#include "weather.h"

#include "field_3dpriority.h"
#include "field_3dbg.h"

// 各天気
#include "weather_sunny.h"
#include "weather_snow.h"
#include "weather_rain.h"
#include "weather_storm.h"
#include "weather_mirage.h"
#include "weather_mist.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
//#define DEBUG_WEATHER_CHAGNE	// YXABで天気をかえる
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	天気変更タイプ
//=====================================
enum{
	FIELD_WEATHER_CHANGETYPE_NORMAL,		// 通常
	FIELD_WEATHER_CHANGETYPE_MULTI,			// マルチ（流しながら）

	FIELD_WEATHER_CHANGETYPE_NUM,			// システム内で使用
} ;

//-------------------------------------
///	シーケンス
//=====================================
// NORMAL
enum {
	FIELD_WEATHER_NORMAL_SEQ_NONE,
	
	FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT,
	FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT,
	FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD,
	FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN,
} ;

// MULTI
enum {
	FIELD_WEATHER_MULTI_SEQ_NONE,
	
	FIELD_WEATHER_MULTI_SEQ_FADE_INOUT,
	FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT,
} ;



//-------------------------------------
///	天気ワーク　保持数
//=====================================
enum{
	FIELD_WEATHER_WORK_NOW,
	FIELD_WEATHER_WORK_NEXT,

	FIELD_WEATHER_WORK_NUM,
};



//-------------------------------------
///	セルユニットワーク数
//=====================================
#define FIELD_WEATHER_CLUNIT_WORK_MAX	( 64 )


//-------------------------------------
///	3DBG
//=====================================
#define FIELD_WEATHER_3DBG_NUM  (WEATHER_TASK_3DBG_NUM)
static const FIELD_3DBG_SETUP	sc_FIELD_3DBG_SETUP[FIELD_WEATHER_3DBG_NUM] = {
  {
    256, 192,
    FLD_3DPRI_WEATHER_OFFS,
    0,
  },
  {
    256, 192,
    FLD_3DPRI_WEATHER_OFFS + (8<<FX32_SHIFT),
    0,
  },
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	天気システムワーク
//=====================================
struct _FIELD_WEATHER{

	// 3DBG
	FIELD_3DBG* p_3dbg[FIELD_WEATHER_3DBG_NUM];

	// 切り替えシーケンス
	u16		change_type;
	u16		seq;
	WEATHER_NO		now_weather;
	WEATHER_NO		next_weather;
  u32           next_weather_envse;

	// 予約
	u16		booking_weather;

	// セルアクターユニット
	GFL_CLUNIT*	p_unit;
  GFL_CLSYS_REND* p_rend;

	// 天気動作システム
	WEATHER_TASK* p_task[ FIELD_WEATHER_WORK_NUM ];
	
	// 今のオーバレイID
	FSOverlayID	now_overlayID;

	GFL_TCB* p_vintr;
};



//-----------------------------------------------------------------------------
/**
 *					天気情報
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	天気情報型
//=====================================
typedef struct {
	const WEATHER_TASK_DATA*	cp_data;			// 天気情報
	FSOverlayID					overlay_id;			// オーバーレイID
} FIELD_WEATHER_DATA;
static const FIELD_WEATHER_DATA sc_FIELD_WEATHER_DATA[] = {
	// 晴れ
	{
		&c_WEATHER_TASK_DATA_SUNNY,
		FS_OVERLAY_ID(field_weather_sunny)
	},


	// 雪
	{
		&c_WEATHER_TASK_DATA_SNOW,
		FS_OVERLAY_ID(field_weather_snow)
	},

	// 雨
	{
		&c_WEATHER_TASK_DATA_RAIN,
		FS_OVERLAY_ID(field_weather_rain)
	},


	// 砂嵐
	{
		&c_WEATHER_TASK_DATA_STORM,
		FS_OVERLAY_ID(field_weather_storm)
	},

	// 吹雪
	{
		&c_WEATHER_TASK_DATA_SNOWSTORM,
		FS_OVERLAY_ID(field_weather_snow)
	},

	// あられ
	{
		&c_WEATHER_TASK_DATA_ARARE,
		FS_OVERLAY_ID(field_weather_snow)
	},

	// ライカミ
	{
		&c_WEATHER_TASK_DATA_RAIKAMI,
		FS_OVERLAY_ID(field_weather_rain)
	},

	// カザカミ
	{
		&c_WEATHER_TASK_DATA_KAZAKAMI,
		FS_OVERLAY_ID(field_weather_rain)
	},

	// ダイヤモンドダスト
	{
		&c_WEATHER_TASK_DATA_DIAMONDDUST,
		FS_OVERLAY_ID(field_weather_snow)
	},

	// 霧
	{
		&c_WEATHER_TASK_DATA_MIST,
		FS_OVERLAY_ID(field_weather_mist)
	},


	// パレスホワイト霧
	{
		&c_WEATHER_TASK_DATA_PALACE_WHITE_MIST,
		FS_OVERLAY_ID(field_weather_mist)
	},

	// パレスブラック霧
	{
		&c_WEATHER_TASK_DATA_PALACE_BLACK_MIST,
		FS_OVERLAY_ID(field_weather_mist)
	},

	// 砂嵐　強い
	{
		&c_WEATHER_TASK_DATA_STORM_HIGH,
		FS_OVERLAY_ID(field_weather_storm)
	},


	// パレスホワイト霧 HIGH
	{
		&c_WEATHER_TASK_DATA_PALACE_WHITE_MIST_HIGH,
		FS_OVERLAY_ID(field_weather_mist)
	},

	// パレスブラック霧　HIGH
	{
		&c_WEATHER_TASK_DATA_PALACE_BLACK_MIST_HIGH,
		FS_OVERLAY_ID(field_weather_mist)
	},




  /*未使用
	// 夕立
	{
		&c_WEATHER_TASK_DATA_EVENING_RAIN,
		FS_OVERLAY_ID(field_weather_rain)
	},

	// 雷雨
	{
		&c_WEATHER_TASK_DATA_SPARKRAIN,
		FS_OVERLAY_ID(field_weather_rain)
	},

	// 蜃気楼
	{
		&c_WEATHER_TASK_DATA_MIRAGE,
		FS_OVERLAY_ID(field_weather_storm)
	},
  */

};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	システム管理
//=====================================
static void FIELD_WEATHER_OVERLAY_Load( FIELD_WEATHER* p_sys, FSOverlayID overlay );
static void FIELD_WEATHER_OVERLAY_UnLoad( FIELD_WEATHER* p_sys );
static void FIELD_WEATHER_VBlank( GFL_TCB* p_tcb, void* p_work );

static BOOL FIELD_WEATHER_LOCAL_Change( FIELD_WEATHER* p_sys, WEATHER_NO weather_no );

//-------------------------------------
///	天気切り替え処理
//=====================================
static void FIELD_WEATHER_CHANGE_Normal( FIELD_WEATHER* p_sys, HEAPID heapID );
static void FIELD_WEATHER_CHANGE_Multi( FIELD_WEATHER* p_sys, HEAPID heapID );


//-------------------------------------
///	天気切り替えタイプの取得
//=====================================
static u32 FIELD_WEATHER_GetChangeType( u16 now_weather, u16 next_weather );



//----------------------------------------------------------------------------
/**
 *	@brief	フィールド天気システム	初期化
 *
 *	@param	cp_camera	フィールドカメラ
 *	@param	heapID		ヒープID
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
FIELD_WEATHER* FIELD_WEATHER_Init( const FIELD_CAMERA* cp_camera, FIELD_LIGHT* p_light, FIELD_FOG_WORK* p_fog, const FIELD_ZONEFOGLIGHT* cp_zonefog, FIELD_SOUND* p_sound, u16 season, HEAPID heapID )
{
	FIELD_WEATHER* p_sys;
	int i;

  // データ数と定義が一致するかチェック
  GF_ASSERT( NELEMS(sc_FIELD_WEATHER_DATA) == WEATHER_NO_NUM );

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WEATHER) );

	// 3DBGシステム初期化
  for( i=0; i<FIELD_WEATHER_3DBG_NUM; i++ ){
  	p_sys->p_3dbg[i] = FIELD_3DBG_Create( &sc_FIELD_3DBG_SETUP[i], heapID );
  }

	// メンバ初期化
	p_sys->change_type		= FIELD_WEATHER_CHANGETYPE_NORMAL;
	p_sys->seq				= 0;
	p_sys->now_weather		= FIELD_WEATHER_NO_NONE;
	p_sys->next_weather		= FIELD_WEATHER_NO_NONE;
	p_sys->booking_weather	= FIELD_WEATHER_NO_NONE;
	p_sys->now_overlayID	= GFL_OVERLAY_BLANK_ID;

	// セルユニット初期化
	p_sys->p_unit = GFL_CLACT_UNIT_Create( FIELD_WEATHER_CLUNIT_WORK_MAX, FIELD_WEATHER_CLUNIT_PRI, heapID );
  // レンダラーの設定
  {
    static const GFL_REND_SURFACE_INIT sc_REND_SURFADE[] = 
    {
      {
        0, 0, 256, 192,
        CLSYS_DRAW_MAIN,
        CLSYS_REND_CULLING_TYPE_NOT_AFFINE
      },
    };
    p_sys->p_rend = GFL_CLACT_USERREND_Create( sc_REND_SURFADE, NELEMS(sc_REND_SURFADE), heapID );

    // 設定
    GFL_CLACT_UNIT_SetUserRend( p_sys->p_unit, p_sys->p_rend );
  }
	

	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		p_sys->p_task[ i ] = WEATHER_TASK_Init( p_sys->p_unit, cp_camera, p_light, p_fog, cp_zonefog, p_sys->p_3dbg, p_sound, season, heapID );
	}

	// 割り込み初期化
	p_sys->p_vintr = GFUser_VIntr_CreateTCB( FIELD_WEATHER_VBlank, (void*)p_sys, 64 );
	

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フィールド天気破棄
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Exit( FIELD_WEATHER* p_sys )
{
	int i;

	// 割り込み破棄
	GFL_TCB_DeleteTask( p_sys->p_vintr );

	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		WEATHER_TASK_Exit( p_sys->p_task[i] );
	}

	// オーバーレイのアンロード
	FIELD_WEATHER_OVERLAY_UnLoad( p_sys );

  // レンダラーの破棄
  GFL_CLACT_USERREND_Delete( p_sys->p_rend );

	// セルユニット破棄
	GFL_CLACT_UNIT_Delete( p_sys->p_unit );


	// 3DBGシステム破棄
  for( i=0; i<FIELD_WEATHER_3DBG_NUM; i++ ){
  	FIELD_3DBG_Delete( p_sys->p_3dbg[i] );
  }

	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	メイン処理
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Main( FIELD_WEATHER* p_sys, HEAPID heapID )
{
	int i;
	static void (*pFunc[FIELD_WEATHER_CHANGETYPE_NUM])( FIELD_WEATHER* p_sys, HEAPID heapID ) ={
		FIELD_WEATHER_CHANGE_Normal,
		FIELD_WEATHER_CHANGE_Multi,
	};

#ifdef DEBUG_WEATHER_CHAGNE
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		FIELD_WEATHER_Change( p_sys, WEATHER_NO_SNOW );
	}else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		FIELD_WEATHER_Change( p_sys, WEATHER_NO_SUNNY );
	}else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		FIELD_WEATHER_Change( p_sys, WEATHER_NO_RAIN );
	}else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		FIELD_WEATHER_Change( p_sys, WEATHER_NO_STORM );
	}
	
#endif

	// 切り替え処理
	GF_ASSERT( NELEMS( pFunc ) > p_sys->change_type );
	pFunc[ p_sys->change_type ]( p_sys, heapID );

	// 予約チェック
	if( p_sys->seq == 0 ){
		if( p_sys->booking_weather != FIELD_WEATHER_NO_NONE ){
			FIELD_WEATHER_Change( p_sys, p_sys->booking_weather );
			p_sys->booking_weather = FIELD_WEATHER_NO_NONE;
		}
	}

	// タスクメイン
	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		WEATHER_TASK_Main( p_sys->p_task[i], heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	3D描画
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_3DWrite( FIELD_WEATHER* p_sys )
{
  int i;
	if( p_sys ){
    for( i=0; i<FIELD_WEATHER_3DBG_NUM; i++ ){
		  FIELD_3DBG_Write( p_sys->p_3dbg[i] );
    }
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  準備中か？チェック
 *
 *	@param	cp_sys システム
 *
 *	@retval TRUE   準備中
 *	@retval FALSE  動作中
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WEATHER_IsLoading( const FIELD_WEATHER* cp_sys )
{
  int i;
  int info;
	if( cp_sys ){
    for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
      info = WEATHER_TASK_GetInfo( cp_sys->p_task[i] );
      if( (info == WEATHER_TASK_INFO_LOADING) ){
        return TRUE;
      }
    }
	}
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	天気フェードインなし設定
 *
 *	@param	p_sys			システムワーク
 *	@param	weather_no		天気ナンバー
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Set( FIELD_WEATHER* p_sys, WEATHER_NO weather_no, HEAPID heapID )
{
	int i;
	
	// 天気ナンバーチェック
	GF_ASSERT( weather_no < WEATHER_NO_NUM );
	GF_ASSERT( NELEMS(sc_FIELD_WEATHER_DATA) > weather_no );

	// 変更する意味があるか？
	// 晴れは、ZONE用フォグやライトの反映がるかもしれないので、
	// 絶対に変更
	if( (weather_no != p_sys->now_weather) || (weather_no == WEATHER_NO_SUNNY) ){

		// 全情報を破棄
		for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
			WEATHER_TASK_ForceEnd( p_sys->p_task[i] );
		}

		// オーバーレイ破棄
		FIELD_WEATHER_OVERLAY_UnLoad( p_sys );
		
		// シーケンスを初期化
		p_sys->seq = 0;

		// オーバーレイ読み込み
		FIELD_WEATHER_OVERLAY_Load( p_sys, sc_FIELD_WEATHER_DATA[ weather_no ].overlay_id );

		// フェードなし初期化
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				sc_FIELD_WEATHER_DATA[ weather_no ].cp_data, WEATHER_TASK_INIT_NORMAL, FALSE, WEATHER_TASK_FOG_USE, TRUE, heapID );

		p_sys->now_weather	= weather_no;
		p_sys->next_weather	= FIELD_WEATHER_NO_NONE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	フェードインあり　天気変更
 *
 *	@param	p_sys			システムワーク
 *	@param	weather_no		天気ナンバー
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Change( FIELD_WEATHER* p_sys, WEATHER_NO weather_no )
{
  FIELD_WEATHER_LOCAL_Change( p_sys, weather_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief  環境音ではなく、通常SEとして音を出す。
 *
 *	@param	p_sys         システム
 *	@param	weather_no    天気
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_ChangeNotEnvSe( FIELD_WEATHER* p_sys, WEATHER_NO weather_no )
{
  FIELD_WEATHER_LOCAL_Change( p_sys, weather_no );
  p_sys->next_weather_envse = FALSE;  // SEを通常SEで再生
}

//----------------------------------------------------------------------------
/**
 *	@brief	天気ナンバーの取得
 *
 *	@param	cp_sys	システムワーク
 *	
 *	@return	今の天気ナンバーを取得
 */
//-----------------------------------------------------------------------------
WEATHER_NO FIELD_WEATHER_GetWeatherNo( const FIELD_WEATHER* cp_sys )
{
	return cp_sys->now_weather;
}

//----------------------------------------------------------------------------
/**
 *	@brief	次の天気ナンバーの取得
 *
 *	@param	cp_sys	システムワーク
 *
 *	@return	次の天気ナンバー
 */
//-----------------------------------------------------------------------------
WEATHER_NO FIELD_WEATHER_GetNextWeatherNo( const FIELD_WEATHER* cp_sys )
{
	return cp_sys->next_weather;
}






//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	オーバーレイ読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	overlay		オーバーレイ
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_OVERLAY_Load( FIELD_WEATHER* p_sys, FSOverlayID overlay )
{
	GF_ASSERT( p_sys->now_overlayID == GFL_OVERLAY_BLANK_ID );
	p_sys->now_overlayID = overlay;
	GFL_OVERLAY_Load( p_sys->now_overlayID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オーバーレイ破棄
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_OVERLAY_UnLoad( FIELD_WEATHER* p_sys )
{
	if( p_sys->now_overlayID != GFL_OVERLAY_BLANK_ID ){
		GFL_OVERLAY_Unload( p_sys->now_overlayID );
		p_sys->now_overlayID = GFL_OVERLAY_BLANK_ID;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank	関数
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_VBlank( GFL_TCB* p_tcb, void* p_work )
{
	FIELD_WEATHER* p_sys = p_work;
  int i;

  for( i=0; i<FIELD_WEATHER_3DBG_NUM; i++ ){
  	FIELD_3DBG_VBlank( p_sys->p_3dbg[i] ); 
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  天気変更リクエスト処理
 *
 *	@param	p_sys
 *	@param	weather_no 
 *
 *	@retval TRUE    変更した
 *	@retval FALSE   同じ天気だったので変更しなかった。
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_WEATHER_LOCAL_Change( FIELD_WEATHER* p_sys, WEATHER_NO weather_no )
{
	// 天気ナンバーチェック
	GF_ASSERT( weather_no < WEATHER_NO_NUM );
	GF_ASSERT( NELEMS(sc_FIELD_WEATHER_DATA) > weather_no );

	// 実行中のリクエストがあるかチェック
	if( p_sys->seq != 0 ){
		p_sys->booking_weather = weather_no;
    //TOMOYA_Printf( "booking_weather %d\n", weather_no );
		return FALSE;
	}

	// 天気が一緒なら何もしない
	// 晴れは、ZONE用フォグやライトの反映がるかもしれないので、
	// 絶対に変更
	if( (p_sys->now_weather == weather_no) && ((weather_no != WEATHER_NO_SUNNY)) ){
		return FALSE;
	}

	// 次の天気を設定
	p_sys->next_weather = weather_no;
  p_sys->next_weather_envse = TRUE;
  //TOMOYA_Printf( "change_weather %d\n", weather_no );
	
	// フェードアウト＋フェードイン
	p_sys->change_type	= FIELD_WEATHER_GetChangeType( p_sys->now_weather, p_sys->next_weather );
	p_sys->seq			= FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT;

  return TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	天気変更　処理	通常
 *
 *	@param	p_sys		システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_CHANGE_Normal( FIELD_WEATHER* p_sys, HEAPID heapID )
{
	switch( p_sys->seq ){
	case FIELD_WEATHER_NORMAL_SEQ_NONE:
		break;
	
		
	case FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT:
		WEATHER_TASK_End( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				TRUE, WEATHER_TASK_FOG_USE );
		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT;
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT:
		if( WEATHER_TASK_GetInfo( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ] ) == WEATHER_TASK_INFO_NONE ){
			//  オーバーレイのアンロード
			FIELD_WEATHER_OVERLAY_UnLoad( p_sys );

			p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD;
		}
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD:
		//  オーバーレイのアンロード
		FIELD_WEATHER_OVERLAY_Load( p_sys, sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].overlay_id );
		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN;
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN:
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].cp_data, 
				WEATHER_TASK_INIT_DIV, TRUE, WEATHER_TASK_FOG_USE, p_sys->next_weather_envse, heapID );

		// 次への情報を消去
		p_sys->now_weather	= p_sys->next_weather;
		p_sys->next_weather	= FIELD_WEATHER_NO_NONE;
    p_sys->next_weather_envse = TRUE;

    //TOMOYA_Printf( "start_weather %d\n", p_sys->now_weather );

		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NONE;
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	天気マルチ変更処理
 *	
 *	@param	p_sys		システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_CHANGE_Multi( FIELD_WEATHER* p_sys, HEAPID heapID )
{
	switch( p_sys->seq ){
	case FIELD_WEATHER_MULTI_SEQ_NONE:
		break;
	
		
	case FIELD_WEATHER_MULTI_SEQ_FADE_INOUT:
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ], 
				sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].cp_data, 
				WEATHER_TASK_INIT_DIV, TRUE, WEATHER_TASK_FOG_WITH, 
        p_sys->next_weather_envse, heapID );

		WEATHER_TASK_End( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				TRUE, WEATHER_TASK_FOG_WITH );

		p_sys->seq = FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT;
		break;
	
	case FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT:
		if( WEATHER_TASK_GetInfo( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ] ) == WEATHER_TASK_INFO_NONE ){

			// 次を今に
			{
				WEATHER_TASK* p_tmp;

				p_tmp = p_sys->p_task[ FIELD_WEATHER_WORK_NOW ];
				p_sys->p_task[ FIELD_WEATHER_WORK_NOW ]		= p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ];
				p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ]	= p_tmp;
				p_sys->now_weather	= p_sys->next_weather;
				p_sys->next_weather = FIELD_WEATHER_NO_NONE;
        p_sys->next_weather_envse = TRUE;
			}
			
			p_sys->seq = FIELD_WEATHER_MULTI_SEQ_NONE;
		}
		break;
	
	default:
		GF_ASSERT(0);
		break;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  今の天気からnext天気に変更するときの変更タイプを取得
 *
 *	@param	now_weather
 *	@param	next_weather 
 *
 *	@return 天気　変更　タイプ
 */
//-----------------------------------------------------------------------------
static u32 FIELD_WEATHER_GetChangeType( u16 now_weather, u16 next_weather )
{
  u32 ret = FIELD_WEATHER_CHANGETYPE_NORMAL;
  
  switch( now_weather ){
  case WEATHER_NO_SNOW:
    if( (next_weather == WEATHER_NO_SNOWSTORM) || 
        (next_weather == WEATHER_NO_ARARE) ){
      ret = FIELD_WEATHER_CHANGETYPE_MULTI;
    }
    break;

  case WEATHER_NO_SNOWSTORM:
    if( (next_weather == WEATHER_NO_SNOW) || 
        (next_weather == WEATHER_NO_ARARE) ){
      ret = FIELD_WEATHER_CHANGETYPE_MULTI;
    }
    break;

  case WEATHER_NO_ARARE:
    if( (next_weather == WEATHER_NO_SNOWSTORM) || 
        (next_weather == WEATHER_NO_SNOW) ){
      ret = FIELD_WEATHER_CHANGETYPE_MULTI;
    }
    break;
  }

  return ret;
}

