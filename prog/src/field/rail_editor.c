//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_editor.c
 *	@brief	レールエディター	DS側処理
 *	@author	tomoya takahashi
 *	@date		2009.07.13
 *
 *	モジュール名：RIAL_EDITOR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// デバック用機能
#ifdef PM_DEBUG

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "arc/arc_def.h"
#include "arc/test_graphic/rail_editor.naix"

#include "field/fieldmap.h"
#include "event_fieldmap_control.h"
#include "field_player_nogrid.h"

#include "fieldmap_func.h"

#include "debug/gf_mcs.h"

#include "rail_editor.h"
#include "rail_editor_data.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	シーケンス
//=====================================
enum {
	RAIL_EDITOR_SEQ_INIT,				// 初期化
	RAIL_EDITOR_SEQ_CONNECT,		// 接続
	RAIL_EDITOR_SEQ_RESET,			// リセット起動
	RAIL_EDITOR_SEQ_MAIN,				// リクエスト対応
	RAIL_EDITOR_SEQ_CLOSE,			// 切断
	RAIL_EDITOR_SEQ_EXIT,				// 破棄

} ;

// 汎用受信バッファ
#define RE_TMP_BUFF_SIZE	( 8198 )

//-------------------------------------
///	レール、エリア受信バッファサイズ
//=====================================
#define RE_MCS_BUFF_RAIL_SIZE	( 8198 )
#define RE_MCS_BUFF_AREA_SIZE	( 1024 )


//-------------------------------------
///	CIRCLE移動
//=====================================
#define RM_DEF_ANGLE_MOVE	(0x100)
#define RM_DEF_LEN_MOVE		(8*FX32_ONE)
#define RM_DEF_CAMERA_LEN	( 0x38D000 )
#define RM_DEF_CAMERA_PITCH	( 0x800 )


//-------------------------------------
///	レール描画リソース
//=====================================
#define RM_DRAW_OBJ_RES_MAX	( FIELD_RAIL_TYPE_MAX )
#define RM_DRAW_OBJ_MAX	( 128 )

// positionリソース
enum
{
	// リソース
	RM_DRAW_OBJ_RES_POSITION_MDL = 0,
	RM_DRAW_OBJ_RES_POSITION_ICA,
	RM_DRAW_OBJ_RES_POSITION_IMA,
	RM_DRAW_OBJ_RES_POSITION_NUM,

	// アニメ
	RM_DRAW_OBJ_ANM_POSITION_ICA = 0,
	RM_DRAW_OBJ_ANM_POSITION_IMA,
	RM_DRAW_OBJ_ANM_POSITION_NUM,
};
#define POSITION_ICA_ANMSPEED_MOVE	( FX32_CONST(5) )
#define POSITION_ICA_ANMSPEED	( FX32_HALF )
#define POSITION_IMA_ADDSPEED	( FX32_CONST(10) )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	受信情報バッファ
//=====================================
typedef struct {
	// フラグ部
	union
	{
		struct
		{
			u32	rail_recv:1;			// レール情報受信
			u32	area_recv:1;			// エリア情報受信
			u32	select_recv:1;		// 選択情報受信
			u32	rail_req:1;				// レール情報送信リクエスト
			u32	area_req:1;				// エリア情報送信リクエスト
			u32	player_req:1;			// プレイヤー情報送信リクエスト
			u32	camera_req:1;			// カメラ情報送信リクエスト
			u32	reset_req:1;			// リセットリクエスト
			u32 end_req:1;				// 終了リクエスト
			u32 raillocation_req:1;	// レールロケーション送信リクエスト
			u32	pad_data:22;
		};
		u32 flag;	
	};

	// データ部
	u32	rail[RE_MCS_BUFF_RAIL_SIZE/4];
	u32 rail_size;
	u32 area[RE_MCS_BUFF_AREA_SIZE/4];
	u32 area_size;
	RE_MCS_SELECT_DATA select;
} RE_RECV_BUFF;


//-------------------------------------
///	レールエディターイベントワーク
//=====================================
typedef struct {
	FIELDMAP_WORK* p_fieldmap;		// フィールドマップ
	RE_RECV_BUFF*	p_recv;						// 受信バッファ
	u32*					p_tmp_buff;// 汎用受信バッファ
	VecFx32				camera_target;
	const VecFx32* cp_field_default_target;
	VecFx32				default_target_offset;

	u32	last_control_type;
	RE_MCS_SELECT_DATA select_last;


	// レール描画ワーク
	FLDMAPFUNC_WORK* p_rail_draw;
} DEBUG_RAIL_EDITOR;



//-------------------------------------
///	リソース
//=====================================
typedef struct {
	BOOL use;
	GFL_G3D_OBJSTATUS trans;
	u32 rail_type;		// ポイントか、ラインか
	u32 rail_index;		// テーブルインデックス
} RE_RAIL_DRAW_OBJ;

//-------------------------------------
///	レール情報描画処理ワーク
//=====================================
typedef struct {
	// リソース部
	GFL_G3D_RES* p_mdl[ RM_DRAW_OBJ_RES_MAX ];
	GFL_G3D_RND* p_rnd[ RM_DRAW_OBJ_RES_MAX ];
	GFL_G3D_OBJ* p_obj[ RM_DRAW_OBJ_RES_MAX ];

	// position
	GFL_G3D_RES* p_position[ RM_DRAW_OBJ_RES_POSITION_NUM ];
	GFL_G3D_ANM* p_positionanm[ RM_DRAW_OBJ_ANM_POSITION_NUM ];
	GFL_G3D_RND* p_positionrnd;
	GFL_G3D_OBJ* p_positionobj;
	VecFx32			position_last_pos;
	int					color_frame;
	
	// 描画オブジェ部
	RE_RAIL_DRAW_OBJ draw_obj[ RM_DRAW_OBJ_MAX ];
	
	// 制御情報部
	const DEBUG_RAIL_EDITOR* cp_parent;
} RE_RAIL_DRAW_WORK;


FS_EXTERN_OVERLAY(mcs_lib);

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT DEBUG_RailEditorEvent(
    GMEVENT * p_event, int *  p_seq, void * p_work );

// レール描画処理
static void RE_DRAW_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Draw(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Create(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);
static void RE_DRAW_Delete(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work);

static RE_RAIL_DRAW_OBJ* RE_DRAW_GetClearWork( RE_RAIL_DRAW_WORK* p_wk );
static void RE_DRAW_SetUpData( RE_RAIL_DRAW_WORK* p_wk, const RAIL_SETTING* cp_setting );
static void RE_DRAW_DRAWOBJ_Clear( RE_RAIL_DRAW_OBJ* p_drawobj );
static BOOL RE_DRAW_DRAWOBJ_IsUse( const RE_RAIL_DRAW_OBJ* cp_drawobj );
static void RE_DRAW_DRAWOBJ_Set( RE_RAIL_DRAW_OBJ* p_drawobj, u32 rail_type, u32 rail_index, const VecFx32* cp_trans, const VecFx32* cp_distance );
static void RE_DRAW_DRAWOBJ_Draw( RE_RAIL_DRAW_WORK* p_wk, RE_RAIL_DRAW_OBJ* p_drawobj );
static const FLDMAPFUNC_DATA sc_RE_DRAW_FUNCDATA = 
{
	0, sizeof(RE_RAIL_DRAW_WORK),
	RE_DRAW_Create, 
	RE_DRAW_Delete, 
	RE_DRAW_Update, 
	RE_DRAW_Draw, 
};


// 受信データ、対応関数
static void RE_Recv_ClearFlag( RE_RECV_BUFF* p_buff );
static BOOL RE_Recv_IsEnd( const RE_RECV_BUFF* cp_buff );
static BOOL RE_Recv_IsReset( const RE_RECV_BUFF* cp_buff );
static void RE_RecvControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Recv_Rail( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_Area( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_SelectData( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_RailReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_AreaReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_PlayerDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_CameraDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_ResetReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_EndReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );
static void RE_Recv_RailLocationReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size );

// 受信情報の反映
static void RE_Reflect( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Area( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Reflect_Select( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk );
static void RE_JumpArea( DEBUG_RAIL_EDITOR* p_wk );

// 選択情報、コントロール関数
static void RE_InputControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );

static void RE_InitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_InitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );

static void RE_ExitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk );
static void RE_ExitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk );

// 送信関数
static void RE_SendControl( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_RailData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_AreaData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_PlayerData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_CameraData( DEBUG_RAIL_EDITOR* p_wk );
static void RE_Send_RailLocationData( DEBUG_RAIL_EDITOR* p_wk );



//----------------------------------------------------------------------------
/**
 *	@brief	レールエディターイベント開始
 *
 *	@param	gsys				システム
 *	@param	fieldmap		フィールドマップ
 *
 *	@return	イベントワーク
 */
//-----------------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_RailEditor( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT* p_event;
  DEBUG_RAIL_EDITOR* p_wk;

  p_event = GMEVENT_Create(
      gsys, NULL, DEBUG_RailEditorEvent, sizeof(DEBUG_RAIL_EDITOR) );

	p_wk = GMEVENT_GetEventWork( p_event );
	p_wk->p_fieldmap = fieldmap;

	return p_event;
}


//-----------------------------------------------------------------------------
/**
 *			private関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	レールエディターイベント
 *
 *	@retval GMEVENT_RES_CONTINUE = 0,
 *	@retval	GMEVENT_RES_FINISH,
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT DEBUG_RailEditorEvent( GMEVENT * p_event, int *  p_seq, void * p_work )
{
  DEBUG_RAIL_EDITOR* p_wk = p_work;

	switch( *p_seq )
	{
	// 初期化
	case RAIL_EDITOR_SEQ_INIT:

		GFL_OVERLAY_Load(FS_OVERLAY_ID(mcs_lib));

		// バッファのメモリ確保
		p_wk->p_recv = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), sizeof(RE_RECV_BUFF) );
		p_wk->p_tmp_buff = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID(p_wk->p_fieldmap), RE_TMP_BUFF_SIZE );

		// カメラターゲット設定
//		FIELD_CAMERA_BindTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->camera_target );


		// デフォルトターゲットの変更
		p_wk->cp_field_default_target = FIELD_CAMERA_DEBUG_GetDefaultTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ) );

		// デフォルトターゲットオフセット取得
		FIELD_CAMERA_GetTargetOffset( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->default_target_offset );

		FIELD_CAMERA_DEBUG_SetDefaultTarget( FIELDMAP_GetFieldCamera( p_wk->p_fieldmap ), &p_wk->camera_target );

		// 座標を設定
		FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &p_wk->camera_target );

		// 描画タスクを登録
		p_wk->p_rail_draw = FLDMAPFUNC_Create( FIELDMAP_GetFldmapFuncSys(p_wk->p_fieldmap), &sc_RE_DRAW_FUNCDATA );
		{
			RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	
			
			p_drawwk->cp_parent = p_wk;

			RE_DRAW_SetUpData( p_drawwk, FIELD_RAIL_LOADER_GetData( FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap ) ) );
		}
		
		(*p_seq) = RAIL_EDITOR_SEQ_CONNECT;
		break;

	// 接続
	case RAIL_EDITOR_SEQ_CONNECT:	
		if( !MCS_Init( FIELDMAP_GetHeapID(p_wk->p_fieldmap) ) )
		{
			(*p_seq) = RAIL_EDITOR_SEQ_MAIN;
		}
		break;

	// リセット起動
	case RAIL_EDITOR_SEQ_RESET:
		(*p_seq) = RAIL_EDITOR_SEQ_MAIN;
		break;


	// リクエスト対応
	case RAIL_EDITOR_SEQ_MAIN:

		// MCSメイン
		MCS_Main();

		// 受信情報フラグ部破棄
		RE_Recv_ClearFlag( p_wk->p_recv );

		// 受信情報対応
		RE_RecvControl( p_wk );

		// 終了受信
		if( RE_Recv_IsEnd( p_wk->p_recv ) )
		{
			(*p_seq)++;
			break;
		}

		// リセット受信
		if( RE_Recv_IsReset( p_wk->p_recv ) ){
			break;
		}
		
		// 受信情報反映
		RE_Reflect(p_wk);
		
		// 選択情報、操作対応
		RE_InputControl( p_wk );

		// 情報送信管理
		RE_SendControl( p_wk );

		break;

	// 切断
	case RAIL_EDITOR_SEQ_CLOSE:			
		(*p_seq) ++;
		break;

	// 破棄
	case RAIL_EDITOR_SEQ_EXIT:				
		MCS_Exit();

		GFL_OVERLAY_Unload( FS_OVERLAY_ID( mcs_lib ) );

		// バッファのメモリ破棄
		GFL_HEAP_FreeMemory( p_wk->p_recv );
		GFL_HEAP_FreeMemory( p_wk->p_tmp_buff );

		// 破棄
		FLDMAPFUNC_Delete( p_wk->p_rail_draw );
		return GMEVENT_RES_FINISH;

	default:
		GF_ASSERT( 0 );
		break;
	}

  return GMEVENT_RES_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	レール情報描画処理	作成
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Create(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	static const u32 sc_res_id[ RM_DRAW_OBJ_RES_MAX ] = 
	{
		NARC_rail_editor_rail_editor_point_nsbmd,
		NARC_rail_editor_rail_editor_line_nsbmd,
	};
	static const u32 sc_positionres_id[ RM_DRAW_OBJ_RES_POSITION_NUM ] = 
	{
		NARC_rail_editor_rail_editor_position_nsbmd,
		NARC_rail_editor_rail_editor_position_nsbca,
		NARC_rail_editor_rail_editor_position_nsbma,
	};
	RE_RAIL_DRAW_WORK* p_wk = p_work;
	
	// 描画リソース読み込み
	for( i=0; i<RM_DRAW_OBJ_RES_MAX; i++ )
	{
		
		p_wk->p_mdl[i] = GFL_G3D_CreateResourceArc( ARCID_RAIL_EDITOR, sc_res_id[i] );
		p_wk->p_rnd[i] = GFL_G3D_RENDER_Create( p_wk->p_mdl[i], 0, NULL );
		p_wk->p_obj[i] = GFL_G3D_OBJECT_Create( p_wk->p_rnd[i], NULL, 0 );
	}

	// 描画リソース読み込み
	for( i=0; i<RM_DRAW_OBJ_RES_POSITION_NUM; i++ )
	{
		p_wk->p_position[i] = GFL_G3D_CreateResourceArc( ARCID_RAIL_EDITOR, sc_positionres_id[i] );
	}
	// レンダラー作成
	p_wk->p_positionrnd = GFL_G3D_RENDER_Create( p_wk->p_position[RM_DRAW_OBJ_RES_POSITION_MDL], 0, NULL );
	// アニメ生成
	for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
	{
		p_wk->p_positionanm[i] = GFL_G3D_ANIME_Create( p_wk->p_positionrnd, p_wk->p_position[ RM_DRAW_OBJ_RES_POSITION_ICA+i ], 0 );
	}
	// オブジェ作成
	p_wk->p_positionobj = GFL_G3D_OBJECT_Create( p_wk->p_positionrnd, p_wk->p_positionanm, RM_DRAW_OBJ_ANM_POSITION_NUM );
	for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
	{
		GFL_G3D_OBJECT_EnableAnime( p_wk->p_positionobj, i );
	}
	{
		FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
		FIELD_PLAYER_GetPos( p_player, &p_wk->position_last_pos );
	}


	// ワークのクリア
	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		RE_DRAW_DRAWOBJ_Clear( &p_wk->draw_obj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報描画処理	破棄
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Delete(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	RE_RAIL_DRAW_WORK* p_wk = p_work;

	// positionリソース破棄
	{
		// オブジェ作成
		GFL_G3D_OBJECT_Delete( p_wk->p_positionobj );
		// アニメ生成
		for( i=0; i<RM_DRAW_OBJ_ANM_POSITION_NUM; i++ )
		{
			GFL_G3D_ANIME_Delete( p_wk->p_positionanm[i] );
		}
		// レンダラー作成
		GFL_G3D_RENDER_Delete( p_wk->p_positionrnd );
		// 描画リソース読み込み
		for( i=0; i<RM_DRAW_OBJ_RES_POSITION_NUM; i++ )
		{
			GFL_G3D_DeleteResource( p_wk->p_position[i] );
		}
	}
	
	// 描画リソース破棄
	for( i=0; i<RM_DRAW_OBJ_RES_MAX; i++ )
	{
		GFL_G3D_OBJECT_Delete( p_wk->p_obj[i] );
		GFL_G3D_RENDER_Delete( p_wk->p_rnd[i] );
		GFL_G3D_DeleteResource( p_wk->p_mdl[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報描画処理	アップデート
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報描画処理	描画
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_Draw(FLDMAPFUNC_WORK * p_funcwk, FIELDMAP_WORK * p_fieldmap, void * p_work)
{
	int i;
	RE_RAIL_DRAW_WORK* p_wk = p_work;
	// ワークの描画
	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		RE_DRAW_DRAWOBJ_Draw( p_wk, &p_wk->draw_obj[i] );
	}

	
	// 主人公位置に置く
	{
		FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_fieldmap );
		GFL_G3D_OBJSTATUS trans = 
		{
			{0,0,0},
			{FX32_ONE, FX32_ONE, FX32_ONE},
			{FX32_ONE,0,0, 0,FX32_ONE,0, 0,0,FX32_ONE },
		};

		// 座標の取得
		FIELD_PLAYER_GetPos( p_player, &trans.trans );

		
		// 移動中は、アニメ,移動してないなら終了
		{
			fx32 dist;

			dist = VEC_Distance( &trans.trans, &p_wk->position_last_pos );
			p_wk->position_last_pos = trans.trans;
			if( dist )
			{
				GFL_G3D_OBJECT_LoopAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_ICA, POSITION_ICA_ANMSPEED_MOVE );

				if( (p_wk->color_frame+POSITION_IMA_ADDSPEED) < FX32_CONST(60) )
				{
					p_wk->color_frame += POSITION_IMA_ADDSPEED;
				}
				else
				{
					p_wk->color_frame = FX32_CONST(60);
				}
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_IMA, &p_wk->color_frame );
			}
			else
			{
				GFL_G3D_OBJECT_LoopAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_ICA, POSITION_ICA_ANMSPEED );
				if( (p_wk->color_frame-POSITION_IMA_ADDSPEED) > 0 )
				{
					p_wk->color_frame -= POSITION_IMA_ADDSPEED;
				}
				else
				{
					p_wk->color_frame = 0;
				}
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_positionobj, RM_DRAW_OBJ_ANM_POSITION_IMA, &p_wk->color_frame );
			}
		}

		GFL_G3D_DRAW_DrawObject( p_wk->p_positionobj, &trans );
	}
	
}


//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークを取得する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static RE_RAIL_DRAW_OBJ* RE_DRAW_GetClearWork( RE_RAIL_DRAW_WORK* p_wk )
{
	int i;

	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		if( RE_DRAW_DRAWOBJ_IsUse( &p_wk->draw_obj[i] ) == FALSE )
		{
			return &p_wk->draw_obj[i];
		}
	}
	GF_ASSERT( 0 );
			
	return &p_wk->draw_obj[0];
}

//----------------------------------------------------------------------------
/**
 *	@brief	データセットアップ
 *
 *	@param	p_wk				ワーク
 *	@param	cp_setting	セッティング
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_SetUpData( RE_RAIL_DRAW_WORK* p_wk, const RAIL_SETTING* cp_setting )
{
	int  i;
	RE_RAIL_DRAW_OBJ* p_drawobj;
	const RAIL_POINT* cp_point;
	VecFx32 point_s, point_e;
	VecFx32 distance = {0,0,0};

	// 全情報クリーン
	for( i=0; i<RM_DRAW_OBJ_MAX; i++ )
	{
		RE_DRAW_DRAWOBJ_Clear( &p_wk->draw_obj[i] );
	}

	// ポイント
	for( i=0; i<cp_setting->point_count; i++ )
	{
		p_drawobj = RE_DRAW_GetClearWork( p_wk );
		
		RE_DRAW_DRAWOBJ_Set( p_drawobj, FIELD_RAIL_TYPE_POINT, i, &cp_setting->point_table[i].pos, &distance );
	}

	// ライン
	for( i=0; i<cp_setting->line_count; i++ )
	{
		p_drawobj = RE_DRAW_GetClearWork( p_wk );

		cp_point = &cp_setting->point_table[ cp_setting->line_table[i].point_s ];
		point_s = cp_point->pos;
		cp_point = &cp_setting->point_table[ cp_setting->line_table[i].point_e ];
		point_e = cp_point->pos;
		
		VEC_Subtract( &point_e, &point_s, &distance );
		RE_DRAW_DRAWOBJ_Set( p_drawobj, FIELD_RAIL_TYPE_LINE, i, &point_s, &distance );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	描画オブジェ　クリア
 *
 *	@param	p_drawobj		描画オブジェ
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Clear( RE_RAIL_DRAW_OBJ* p_drawobj )
{
	p_drawobj->use = FALSE;
	p_drawobj->rail_type = 0;
	p_drawobj->rail_index = 0;

}

//----------------------------------------------------------------------------
/**
 *	@brief	USE	フラグのチェック
 *
 *	@param	cp_drawobj 
 */
//-----------------------------------------------------------------------------
static BOOL RE_DRAW_DRAWOBJ_IsUse( const RE_RAIL_DRAW_OBJ* cp_drawobj )
{
	return cp_drawobj->use;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画おぶじぇ	情報設定
 *
 *	@param	p_drawobj			描画おぶじぇ
 *	@param	rail_type			レールタイプ
 *	@param	rail_index		テーブルインデックス
 *	@param	cp_trans			転置情報（ラインは始点）
 *	@param	cp_way				方向と距離
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Set( RE_RAIL_DRAW_OBJ* p_drawobj, u32 rail_type, u32 rail_index, const VecFx32* cp_trans, const VecFx32* cp_distance )
{
	VecFx32 vec_n;
	
	GF_ASSERT( rail_type < RM_DRAW_OBJ_RES_MAX );
	p_drawobj->rail_type	= rail_type;
	p_drawobj->rail_index = rail_index;

	MTX_Identity33( &p_drawobj->trans.rotate );
	VEC_Set( &p_drawobj->trans.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	p_drawobj->trans.trans = *cp_trans;

	if( rail_type == FIELD_RAIL_TYPE_LINE )
	{
		GF_ASSERT( cp_distance );
		VEC_Set( &p_drawobj->trans.scale, FX32_ONE, FX32_ONE, VEC_Mag(cp_distance) );
		VEC_Normalize( cp_distance, &vec_n );
		GFL_CALC3D_MTX_GetVecToRotMtxXZ( &vec_n, &p_drawobj->trans.rotate );
	}

	p_drawobj->use = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画おぶじぇ　描画処理
 *
 *	@param	p_wk				レール描画システム
 *	@param	p_drawobj		オブジェワーク
 */
//-----------------------------------------------------------------------------
static void RE_DRAW_DRAWOBJ_Draw( RE_RAIL_DRAW_WORK* p_wk, RE_RAIL_DRAW_OBJ* p_drawobj )
{
	if( p_drawobj->use )
	{
		GFL_G3D_DRAW_DrawObject( p_wk->p_obj[p_drawobj->rail_type], &p_drawobj->trans );
	}
}






// 受信データ、対応関数
//----------------------------------------------------------------------------
/**
 *	@brief	受信バッファフラグ部クリア
 *
 *	@param	p_buff バッファ
 */
//-----------------------------------------------------------------------------
static void RE_Recv_ClearFlag( RE_RECV_BUFF* p_buff )
{
	GF_ASSERT( p_buff );
	p_buff->flag = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信バッファ　終了フラグチェック
 *	
 *	@param	cp_buff		バッファ
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL RE_Recv_IsEnd( const RE_RECV_BUFF* cp_buff )
{
	GF_ASSERT( cp_buff );
	return cp_buff->end_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信バッファ	リセットフラグチェック
 *
 *	@param	cp_buff		バッファ
 *
 *	@retval	TRUE	リセット
 *	@retval	FALSE	リセットじゃない
 */
//-----------------------------------------------------------------------------
static BOOL RE_Recv_IsReset( const RE_RECV_BUFF* cp_buff )
{
	GF_ASSERT( cp_buff );
	return cp_buff->reset_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信情報管理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_RecvControl( DEBUG_RAIL_EDITOR* p_wk )
{
	const RE_MCS_HEADER* cp_header;
	u32 size;
	static void (* const cpRecv[RE_MCS_DATA_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size ) = 
	{
		NULL,
		NULL,
		RE_Recv_Rail,
		RE_Recv_Area,
		NULL,
		NULL,
		RE_Recv_SelectData,
		RE_Recv_RailReq,
		RE_Recv_AreaReq,
		RE_Recv_PlayerDataReq,
		RE_Recv_CameraDataReq,
		RE_Recv_ResetReq,
		RE_Recv_EndReq,
		NULL,
		RE_Recv_RailLocationReq
	};

	// 受信データがあるかチェック
	if( MCS_CheckRead() != 0 )
	{
		size = MCS_Read( p_wk->p_tmp_buff, RE_TMP_BUFF_SIZE );
		if( size != 0 )
		{
			cp_header = (const RE_MCS_HEADER*)p_wk->p_tmp_buff;
				
			// 未知のデータが来た
			GF_ASSERT( cp_header->data_type < RE_MCS_DATA_TYPE_MAX );

			if( cpRecv[ cp_header->data_type ] )
			{
				cpRecv[ cp_header->data_type ]( p_wk, p_wk->p_tmp_buff, size );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報の受信
 */
//-----------------------------------------------------------------------------
static void RE_Recv_Rail( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_RAIL_DATA* cp_rail = cp_data;

	GF_ASSERT( cp_rail->header.data_type == RE_MCS_DATA_RAIL );

	p_wk->p_recv->rail_recv = TRUE;
	p_wk->p_recv->reset_req = cp_rail->reset;

	GF_ASSERT( size < RE_MCS_BUFF_RAIL_SIZE );
	GFL_STD_MemCopy( cp_rail, p_wk->p_recv->rail, size );

	TOMOYA_Printf( "rail_recv size = %d\n", size );
	p_wk->p_recv->rail_size = size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	エリア情報受信
 */
//-----------------------------------------------------------------------------
static void RE_Recv_Area( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_AREA_DATA* cp_area = cp_data;

	GF_ASSERT( cp_area->header.data_type == RE_MCS_DATA_AREA );

	p_wk->p_recv->area_recv = TRUE;
	p_wk->p_recv->reset_req = cp_area->reset;

	GF_ASSERT( size < RE_MCS_BUFF_AREA_SIZE );
	GFL_STD_MemCopy( cp_area, p_wk->p_recv->area, size );
	p_wk->p_recv->area_size = size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択情報の受信
 */
//-----------------------------------------------------------------------------
static void RE_Recv_SelectData( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_SELECT_DATA* cp_select = cp_data;

	GF_ASSERT( cp_select->header.data_type == RE_MCS_DATA_SELECTDATA );

	p_wk->p_recv->select_recv = TRUE;

	GF_ASSERT( size == sizeof(RE_MCS_SELECT_DATA) );
	GFL_STD_MemCopy( cp_select, &p_wk->p_recv->select, size );
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報送信リクエスト
 */
//-----------------------------------------------------------------------------
static void RE_Recv_RailReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RAIL_REQ );
	p_wk->p_recv->rail_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	エリア情報送信リクエスト
 */
//-----------------------------------------------------------------------------
static void RE_Recv_AreaReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_AREA_REQ );
	p_wk->p_recv->area_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー情報送信リクエスト
 */
//-----------------------------------------------------------------------------
static void RE_Recv_PlayerDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_PLAYER_REQ );
	p_wk->p_recv->player_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ情報送信リクエスト
 */
//-----------------------------------------------------------------------------
static void RE_Recv_CameraDataReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_CAMERA_REQ );
	p_wk->p_recv->camera_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リセット　リクエスト
 */
//-----------------------------------------------------------------------------
static void RE_Recv_ResetReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RESET_REQ );
	p_wk->p_recv->reset_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了　リクエスト受信
 */
//-----------------------------------------------------------------------------
static void RE_Recv_EndReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_END_REQ );
	p_wk->p_recv->end_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	レールロケーション送信　リクエスト受信
 */
//-----------------------------------------------------------------------------
static void RE_Recv_RailLocationReq( DEBUG_RAIL_EDITOR* p_wk, const void* cp_data, u32 size )
{
	const RE_MCS_HEADER* cp_header = cp_data;

	GF_ASSERT( cp_header->data_type == RE_MCS_DATA_RAILLOCATION_REQ );
	p_wk->p_recv->raillocation_req = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信情報を　反映	
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Reflect( DEBUG_RAIL_EDITOR* p_wk )
{
	if( p_wk->p_recv->rail_recv )
	{
		RE_Reflect_Rail( p_wk );
	}
	if( p_wk->p_recv->area_recv )
	{
		RE_Reflect_Area( p_wk );
	}
	if( p_wk->p_recv->select_recv )
	{
		RE_Reflect_Select( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報受信反映
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FIELD_RAIL_LOADER* p_railload = FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	RE_MCS_RAIL_DATA* p_data = (RE_MCS_RAIL_DATA*)p_wk->p_recv->rail;	
	void* p_setdata;

	// ロケーション取得
	FIELD_RAIL_MAN_GetLocation( p_rail, &location );

	// レール情報ローダーに設定
	FIELD_RAIL_LOADER_Clear( p_railload );
	p_setdata = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID( p_wk->p_fieldmap ), (p_wk->p_recv->rail_size  - 8) );
	GFL_STD_MemCopy( p_data->rail, p_setdata, (p_wk->p_recv->rail_size  - 8) );
	FIELD_RAIL_LOADER_DEBUG_LoadBinary( p_railload, p_setdata, (p_wk->p_recv->rail_size  - 8) );

	// レールマネージャに登録
	FIELD_RAIL_MAN_Load( p_rail,  FIELD_RAIL_LOADER_GetData( p_railload ) );

	// ロケーション設定
	FIELD_RAIL_MAN_SetLocation( p_rail, &location );

	// 描画反映
	{
		RE_RAIL_DRAW_WORK* p_drawwk = FLDMAPFUNC_GetFreeWork( p_wk->p_rail_draw );	
		
		RE_DRAW_SetUpData( p_drawwk, FIELD_RAIL_LOADER_GetData( p_railload ) );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	エリア情報受信反映
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Area( DEBUG_RAIL_EDITOR* p_wk )
{
	FLD_SCENEAREA* p_area = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	FLD_SCENEAREA_LOADER* p_areaload = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );
	RE_MCS_AREA_DATA* p_data = (RE_MCS_AREA_DATA*)p_wk->p_recv->area;	
	void* p_setdata;

	// areaローダーに設定
	FLD_SCENEAREA_LOADER_Clear( p_areaload );
	p_setdata = GFL_HEAP_AllocClearMemory( FIELDMAP_GetHeapID( p_wk->p_fieldmap ), (p_wk->p_recv->area_size - 8) );
	GFL_STD_MemCopy( p_data->area, p_setdata, (p_wk->p_recv->area_size - 8) );
	FLD_SCENEAREA_LOADER_LoadBinary( p_areaload, p_setdata, p_wk->p_recv->area_size - 8 );

	// areaマネージャに登録
	FLD_SCENEAREA_Release( p_area );
	FLD_SCENEAREA_Load( p_area, 
			FLD_SCENEAREA_LOADER_GetData( p_areaload ),
			FLD_SCENEAREA_LOADER_GetDataNum( p_areaload ),
			FLD_SCENEAREA_LOADER_GetFunc( p_areaload ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択情報の反映
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void RE_Reflect_Select( DEBUG_RAIL_EDITOR* p_wk )
{
	// テーブル内インデックスの変更
	if( (p_wk->p_recv->select.select_data != p_wk->select_last.select_data) ||
			(p_wk->p_recv->select.select_index != p_wk->select_last.select_index) ||
			(p_wk->p_recv->select.select_seq == p_wk->select_last.select_seq) )
	{
		static void (*const cpJump[RE_SELECT_DATA_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
		{
			NULL,
			RE_JumpPoint,
			RE_JumpLine,
			RE_JumpArea,
		};

		GF_ASSERT( p_wk->p_recv->select.select_data < RE_SELECT_DATA_MAX );
		if( cpJump[p_wk->p_recv->select.select_data] )
		{
			cpJump[p_wk->p_recv->select.select_data]( p_wk );
		}
	}

	p_wk->select_last = p_wk->p_recv->select;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポイントの場所にジャンプ
 */
//-----------------------------------------------------------------------------
static void RE_JumpPoint( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	VecFx32 pos;
	const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap ) );

	if( cp_setting->point_count > p_wk->p_recv->select.select_index )
	{

		location.type					= FIELD_RAIL_TYPE_POINT;
		location.rail_index		= p_wk->p_recv->select.select_index;
		location.line_ofs			= 0;
		location.width_ofs		= 0;
		location.key					= 0;
		FIELD_RAIL_MAN_SetLocation( p_rail, &location );

		// 位置を人物に設定
		FIELD_RAIL_MAN_GetPos( p_rail, &pos );
		// 
		FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラインの開始位置場所にジャンプ
 */
//-----------------------------------------------------------------------------
static void RE_JumpLine( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	RAIL_LOCATION location;
	VecFx32 pos;
	const RAIL_SETTING* cp_setting = FIELD_RAIL_LOADER_GetData( FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap ) );

	if( cp_setting->line_count > p_wk->p_recv->select.select_index )
	{
		location.type					= FIELD_RAIL_TYPE_LINE;
		location.rail_index		= p_wk->p_recv->select.select_index;
		location.line_ofs			= 0;
		location.width_ofs		= 0;
		location.key					= 0;
		FIELD_RAIL_MAN_SetLocation( p_rail, &location );

		// 位置を人物に設定
		FIELD_RAIL_MAN_GetPos( p_rail, &pos );
		// 
		FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap ), &pos );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	areaに近いポイントにジャンプ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_JumpArea( DEBUG_RAIL_EDITOR* p_wk )
{
	// ポイントデータをなめて、area位置に近い場所に飛ぶ
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA_LOADER* p_arealoader = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );

	TOMOYA_Printf( "RE_JumpArea 未実装\n" );
}



//----------------------------------------------------------------------------
/**
 *	@brief	選択情報による、入力コントロール
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_InputControl( DEBUG_RAIL_EDITOR* p_wk )
{
	static void (*const cpInitInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_InitInputPoint_FreeNormal,
		RE_InitInputPoint_FreeCircle,
		RE_InitInputPoint_Rail,
		RE_InitInputCamera_Pos,
		RE_InitInputCamera_Target,
		RE_InitInputLinePos_CenterPos,
	};

	static void (*const cpExitInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_ExitInputPoint_FreeNormal,
		RE_ExitInputPoint_FreeCircle,
		RE_ExitInputPoint_Rail,
		RE_ExitInputCamera_Pos,
		RE_ExitInputCamera_Target,
		RE_ExitInputLinePos_CenterPos,
	};
	
	static void (*const cpInput[RE_SELECT_DATA_SEQ_TYPE_MAX])( DEBUG_RAIL_EDITOR* p_wk ) = 
	{
		NULL,
		RE_InputPoint_FreeNormal,
		RE_InputPoint_FreeCircle,
		RE_InputPoint_Rail,
		RE_InputCamera_Pos,
		RE_InputCamera_Target,
		RE_InputLinePos_CenterPos,
	};

	// 変更チェック
	if( p_wk->p_recv->select.select_seq != p_wk->last_control_type )
	{
		if(cpExitInput[ p_wk->last_control_type ])
		{
			cpExitInput[ p_wk->last_control_type ]( p_wk );
		}

		if(cpInitInput[ p_wk->p_recv->select.select_seq ])
		{
			cpInitInput[ p_wk->p_recv->select.select_seq ]( p_wk );
		}
	}
	
	// 選択状態による、操作
	GF_ASSERT( p_wk->p_recv->select.select_seq < RE_SELECT_DATA_SEQ_TYPE_MAX);	
	if( cpInput[p_wk->p_recv->select.select_seq] )
	{
		cpInput[p_wk->p_recv->select.select_seq]( p_wk );
	}

	p_wk->last_control_type = p_wk->p_recv->select.select_seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポイント	自由移動
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );

	// 半グリッドサイズでの移動にする
	FIELD_PLAYER_NOGRID_Move( p_player, GFL_UI_KEY_GetCont(), 0x8000 );

	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
}

static void RE_InputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	VecFx32 target, pl_pos, sub;
	u16 yaw;
	fx32 len;
	u32 key_cont = GFL_UI_KEY_GetRepeat();
	s16 df_angle;
	s16 df_len;
	BOOL change = FALSE;

	// カメラターゲットを中心に回転移動
	FIELD_CAMERA_GetTargetPos( p_camera, &target );
	FIELD_PLAYER_GetPos( p_player, &pl_pos );

	target.y	= pl_pos.y;
	len				= VEC_Distance( &target, &pl_pos );
	yaw = FIELD_CAMERA_GetAngleYaw( p_camera );

	if( key_cont & PAD_BUTTON_B )
	{
		df_len		= 4*RM_DEF_LEN_MOVE;
		df_angle	= 4*RM_DEF_ANGLE_MOVE;
	}
	else
	{
		df_len		= RM_DEF_LEN_MOVE;
		df_angle	= RM_DEF_ANGLE_MOVE;
	}

 
	if( key_cont & PAD_KEY_LEFT )
	{
		yaw -= df_angle;
		change = TRUE;
	}
	else if( key_cont & PAD_KEY_RIGHT )
	{
		yaw += df_angle;
		change = TRUE;
	}
	else if( key_cont & PAD_KEY_UP )
	{
		len += df_len;
		change = TRUE;
	}
	else if( key_cont & PAD_KEY_DOWN )
	{
		len -= df_len;
		change = TRUE;
	}

	if( change )
	{
		// 角度から、座標を求める
		pl_pos.x = target.x + FX_Mul( FX_SinIdx( yaw ), len );
		pl_pos.z = target.z + FX_Mul( FX_CosIdx( yaw ), len );
		FIELD_PLAYER_SetPos( p_player, &pl_pos );
	}


	// 延長線上にカメラを配置
	FIELD_CAMERA_SetAnglePitch( p_camera, RM_DEF_CAMERA_PITCH );
	FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
	FIELD_CAMERA_SetAngleLen( p_camera, RM_DEF_CAMERA_LEN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポイント	レール移動
 */
//-----------------------------------------------------------------------------
static void RE_InputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	VecFx32 pos;


	// レールシステムメイン
	FIELD_RAIL_MAN_Update(p_rail, GFL_UI_KEY_GetCont() );
	FIELD_RAIL_MAN_UpdateCamera(p_rail);
	FIELD_RAIL_MAN_GetPos( p_rail, &pos );
	FLD_SCENEAREA_Update( p_scenearea, &pos );

  FIELD_PLAYER_SetPos( p_player, &pos );
	FIELD_PLAYER_GetPos( p_player, &p_wk->camera_target );
//	FIELD_CAMERA_BindTarget( p_camera, &p_wk->camera_target );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ座標操作
 */
//-----------------------------------------------------------------------------
static void RE_InputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;

	// カメラ操作は下画面で行う
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);

  FIELD_SUBSCREEN_Main(subscreen);
}
//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット座標の操作
 */
//-----------------------------------------------------------------------------
static void RE_InputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;

	// カメラ操作は下画面で行う
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
  FIELD_SUBSCREEN_Main(subscreen);
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライン動作処理、	中心座標
 */
//-----------------------------------------------------------------------------
static void RE_InputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
	// 中心座標を調べやすいように
	RE_InputPoint_FreeNormal( p_wk );
}



// 初期化
//----------------------------------------------------------------------------
/**
 *	@brief	自由移動
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	VecFx32 pos;

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	FIELD_CAMERA_BindDefaultTarget( p_camera );

	FIELD_RAIL_MAN_SetActiveFlag(p_rail, FALSE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, FALSE);

	// プレイヤー座標をハーフグリッド単位にする
	FIELD_PLAYER_GetPos( p_player, &pos );
	pos.x -= pos.x % GRID_HALF_FX32;
	pos.y -= pos.y % GRID_HALF_FX32;
	pos.z -= pos.z % GRID_HALF_FX32;
	FIELD_PLAYER_SetPos( p_player, &pos );

	// 基本アングルの設定
/*	FIELD_CAMERA_SetAnglePitch( p_camera, 45*182 );
	FIELD_CAMERA_SetAngleYaw( p_camera, 0 );
	FIELD_CAMERA_SetAngleLen( p_camera, 0x90000 );*/

}
//----------------------------------------------------------------------------
/**
 *	@brief	サークル移動
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );
	VecFx32 target, pl_pos, sub;
	u16 yaw;
	fx32 len;
	u32 key_cont = GFL_UI_KEY_GetRepeat();
	s16 df_angle;
	s16 df_len;
	BOOL change = FALSE;

	FIELD_RAIL_MAN_SetActiveFlag(p_rail, FALSE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, FALSE);

	FIELD_CAMERA_FreeTarget( p_camera );

	// カメラターゲットを中心に回転移動
	FIELD_CAMERA_GetTargetPos( p_camera, &target );
	FIELD_PLAYER_GetPos( p_player, &pl_pos );

	target.y	= pl_pos.y;
	len				= VEC_Distance( &target, &pl_pos );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

	VEC_Subtract( &pl_pos, &target, &sub );
	yaw				= FX_Atan2Idx( sub.x, sub.z );

	// 角度から、座標を求める
	pl_pos.x = target.x + FX_Mul( FX_SinIdx( yaw ), len );
	pl_pos.z = target.z + FX_Mul( FX_CosIdx( yaw ), len );
	FIELD_PLAYER_SetPos( p_player, &pl_pos );

	// 延長線上にカメラを配置
	FIELD_CAMERA_SetAnglePitch( p_camera, RM_DEF_CAMERA_PITCH );
	FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
	FIELD_CAMERA_SetAngleLen( p_camera, RM_DEF_CAMERA_LEN );
}
//----------------------------------------------------------------------------
/**
 *	@brief	レール移動
 */
//-----------------------------------------------------------------------------
static void RE_InitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	FIELD_RAIL_MAN* p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_wk->p_fieldmap );

	// レール移動
	FIELD_RAIL_MAN_SetActiveFlag(p_rail, TRUE);
	FLD_SCENEAREA_SetActiveFlag(p_scenearea, TRUE);

	FIELD_CAMERA_FreeTarget( p_camera );
}
//----------------------------------------------------------------------------
/**
 *	@brief	カメラ操作移動
 */
//-----------------------------------------------------------------------------
static void RE_InitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;
	

	// カメラ操作は下画面で行う
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
	FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
	{ 
		void * inner_work;
		FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
		inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
		FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS);

		// 動作モード設定
		FIELD_CAMERA_SetMode( cam, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット操作
 */
//-----------------------------------------------------------------------------
static void RE_InitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_SUBSCREEN_WORK * subscreen;
	

	// カメラ操作は下画面で行う
	subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
	FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
	{ 
		void * inner_work;
		FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
		inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
		FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_TARGET_POS);

		// 動作モード設定
		FIELD_CAMERA_SetMode( cam, FIELD_CAMERA_MODE_CALC_TARGET_POS );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief		移動中心座標設定
 */
//-----------------------------------------------------------------------------
static void RE_InitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );

	// 自由移動モード
	RE_InitInputPoint_FreeNormal( p_wk );

	// 基本アングルの設定
	// かなり上から
	FIELD_CAMERA_SetAnglePitch( p_camera, 90*182 );
	FIELD_CAMERA_SetAngleYaw( p_camera, 0 );
	FIELD_CAMERA_SetAngleLen( p_camera, 0x150000 );
}

// 破棄
//----------------------------------------------------------------------------
/**
 *	@brief	自由移動
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_FreeNormal( DEBUG_RAIL_EDITOR* p_wk )
{
}
//----------------------------------------------------------------------------
/**
 *	@brief	円形移動
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_FreeCircle( DEBUG_RAIL_EDITOR* p_wk )
{
}
//----------------------------------------------------------------------------
/**
 *	@brief	レール移動
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputPoint_Rail( DEBUG_RAIL_EDITOR* p_wk )
{
}
//----------------------------------------------------------------------------
/**
 *	@brief	カメラ操作
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputCamera_Pos( DEBUG_RAIL_EDITOR* p_wk )
{
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
	FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

	{
		FIELD_SUBSCREEN_WORK * subscreen;
		

		// カメラ操作は下画面で行う
		subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
		FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_NORMAL);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット操作
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputCamera_Target( DEBUG_RAIL_EDITOR* p_wk )
{
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(p_wk->p_fieldmap);
	FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

	// オフセットに変更値が入ってしまうので、ターゲットの実座標に戻す
	FIELD_CAMERA_SetTargetOffset( cam, &p_wk->default_target_offset );

	{
		FIELD_SUBSCREEN_WORK * subscreen;
		

		// カメラ操作は下画面で行う
		subscreen = FIELDMAP_GetFieldSubscreenWork(p_wk->p_fieldmap);
		FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_NORMAL);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	カーブ移動中心座標操作
 */
//-----------------------------------------------------------------------------
static void RE_ExitInputLinePos_CenterPos( DEBUG_RAIL_EDITOR* p_wk )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief	情報送信管理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_SendControl( DEBUG_RAIL_EDITOR* p_wk )
{
	if( p_wk->p_recv->rail_req )
	{
		// レール情報の送信
		RE_Send_RailData( p_wk );
	}
	else if( p_wk->p_recv->area_req )
	{
		// area情報の送信
		RE_Send_AreaData( p_wk );
	}
	else if( p_wk->p_recv->player_req )
	{
		// player情報の送信
		RE_Send_PlayerData( p_wk );
	}
	else if( p_wk->p_recv->camera_req )
	{
		// camera情報の送信
		RE_Send_CameraData( p_wk );
	}
	else if( p_wk->p_recv->raillocation_req )
	{
		// レールロケーション送信
		RE_Send_RailLocationData( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	レール情報を送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Send_RailData( DEBUG_RAIL_EDITOR* p_wk )
{
	const FIELD_RAIL_LOADER* cp_loader = FIELDMAP_GetFieldRailLoader( p_wk->p_fieldmap );
	const void* cp_data = FIELD_RAIL_LOADER_DEBUG_GetData( cp_loader );	
	u32 datasize = FIELD_RAIL_LOADER_DEBUG_GetDataSize( cp_loader );	
	RE_MCS_RAIL_DATA* p_senddata = (RE_MCS_RAIL_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_RAIL;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->rail, datasize );
	
	// 送信
	result = MCS_Write( MCS_CHANNEL0, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	area情報を送信
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Send_AreaData( DEBUG_RAIL_EDITOR* p_wk )
{
	const FLD_SCENEAREA_LOADER* cp_loader = FIELDMAP_GetFldSceneAreaLoader( p_wk->p_fieldmap );
	const void* cp_data = FLD_SCENEAREA_LOADER_DEBUG_GetData( cp_loader );
	u32 datasize				= FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( cp_loader );
	RE_MCS_AREA_DATA* p_senddata = (RE_MCS_AREA_DATA*)p_wk->p_tmp_buff;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_AREA;
	p_senddata->reset						 = FALSE;
	GFL_STD_MemCopy( cp_data, p_senddata->area, datasize );
	
	
	// 送信
	result = MCS_Write( MCS_CHANNEL0, p_senddata, datasize+4+sizeof(RE_MCS_HEADER) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー情報を送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Send_PlayerData( DEBUG_RAIL_EDITOR* p_wk )
{
	const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
	RE_MCS_PLAYER_DATA* p_senddata = (RE_MCS_PLAYER_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos;
	BOOL result;
	
	p_senddata->header.data_type = RE_MCS_DATA_PLAYERDATA;

	FIELD_PLAYER_GetPos( cp_player, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;

	// 送信
	result = MCS_Write( MCS_CHANNEL0, p_senddata, sizeof(RE_MCS_PLAYER_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ情報を送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void RE_Send_CameraData( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
	const GFL_G3D_CAMERA * cp_core_camera = FIELD_CAMERA_GetCameraPtr( p_camera );
	RE_MCS_CAMERA_DATA* p_senddata = (RE_MCS_CAMERA_DATA*)p_wk->p_tmp_buff;
	VecFx32 pos;
	VecFx32 target;
	BOOL result;

	p_senddata->header.data_type = RE_MCS_DATA_CAMERADATA;

	FIELD_CAMERA_Main( p_camera, 0 );

	GFL_G3D_CAMERA_GetTarget( cp_core_camera, &target );
	VEC_Subtract( &target, &p_wk->default_target_offset, &target );	// ターゲットオフセット分は減らす
	GFL_G3D_CAMERA_GetPos( cp_core_camera, &pos );
	p_senddata->pos_x = pos.x;
	p_senddata->pos_y = pos.y;
	p_senddata->pos_z = pos.z;
	p_senddata->target_x = target.x;
	p_senddata->target_y = target.y;
	p_senddata->target_z = target.z;
	
	if( FIELD_CAMERA_GetMode( p_camera ) == FIELD_CAMERA_MODE_CALC_CAMERA_POS )
	{
		p_senddata->pitch = FIELD_CAMERA_GetAnglePitch( p_camera );
		p_senddata->yaw		= FIELD_CAMERA_GetAngleYaw( p_camera );
		p_senddata->len		= FIELD_CAMERA_GetAngleLen( p_camera );
	}
	else
	{
		VecFx32 camera_way;
		fx32 xz_dist;

		VEC_Subtract( &pos, &target, &camera_way );
		pos.y = 0;
		target.y = 0;
		xz_dist = VEC_Distance( &pos, &target );

		p_senddata->len		= VEC_Mag( &camera_way );
		p_senddata->pitch = FX_Atan2( xz_dist, camera_way.y );
		p_senddata->yaw		= FX_Atan2( camera_way.z, camera_way.x );
	}

	// 送信
	result = MCS_Write( MCS_CHANNEL0, p_senddata, sizeof(RE_MCS_CAMERA_DATA) );
	GF_ASSERT( result );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	レールロケーションの送信
 *
 *	@param	p_wk	わーく
 */
//-----------------------------------------------------------------------------
static void RE_Send_RailLocationData( DEBUG_RAIL_EDITOR* p_wk )
{
	FIELD_RAIL_MAN * p_rail = FIELDMAP_GetFieldRailMan( p_wk->p_fieldmap );
	RE_MCS_RAILLOCATION_DATA* p_senddata = (RE_MCS_RAILLOCATION_DATA*)p_wk->p_tmp_buff;
	RAIL_LOCATION location;
	BOOL result;

	FIELD_RAIL_MAN_GetLocation( p_rail, &location );

	p_senddata->header.data_type = RE_MCS_DATA_RAILLOCATIONDATA;
	p_senddata->rail_type = location.type;
	p_senddata->index			= location.rail_index;


	// 送信
	result = MCS_Write( MCS_CHANNEL0, p_senddata, sizeof(RE_MCS_RAILLOCATION_DATA) );
	GF_ASSERT( result );	
}



#endif // PM_DEBUG

