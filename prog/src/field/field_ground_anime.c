//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_ground_anime.c
 *	@brief	フィールド地面アニメーション管理システム
 *	@author	tomoya takahshi
 *	@data		2009.04.30
 *
 *	モジュール名：FIELD_GRANM
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_ground_anime.h"
#include "texanm_sys.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	地面アニメーションワーク
// 
//=====================================
//NNSG3dAnmObjのメモリサイズ	プラチナの１ブロック内のマテリアル最大数分
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX	( 40 )
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE	( sizeof(NNSG3dAnmObj) + (2*FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX) )


//-------------------------------------
///	ITPアニメーション
//=====================================
#define FIELD_GRANM_ITP_TRANS_ADDR_NONE	( 0xffffffff )



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	フィールド地面アニメーションワーク
//=====================================
struct _FIELD_GRANM_WORK
{
	u32 bind:1;									// レンダーオブジェに関連付けられているか。
	u32 pad:31;									// 予備
	NNSG3dAnmObj* p_anmobj;			// アニメーションオブジェクト
	void*					p_anmres;			// アニメーションリソース
};

//-------------------------------------
///	ITAアニメーション管理
//=====================================
typedef struct 
{
	FIELD_GRANM_WORK* p_wkbuf;
	u32								wkmax;

	// エリアアニメ情報
	void* p_anmfile;
} FIELD_GRANM_ITA;


//-------------------------------------
///	ITPアニメーション管理
//=====================================
typedef struct 
{

	// １つ前のフレーム情報
	BOOL trans;
	TEXANM_DATA*	p_last_data;
	u32*					p_trans_addr;
	u32*					p_trans_size;
	fx32*					p_anime_frame;
	u32						anime_num;

	// 地面テクスチャ
	const GFL_G3D_RES* cp_ground_tex;
	
	// エリアアニメ情報
	void* p_anmfile;
	TEXANM_DATATBL anmtbl;
	GFL_G3D_RES* p_tex;


} FIELD_GRANM_ITP;

//-------------------------------------
///	フィールド地面アニメーション管理システム
//=====================================
struct _FIELD_GRANM 
{
	
	// アニメーションフレーム情報
	u32								anime_flag;
	fx32							anime_frame;
	fx32							anime_speed;

	// ITAアニメーション管理
	FIELD_GRANM_ITA	ita_anime;

	// ITPアニメーション管理
	FIELD_GRANM_ITP	itp_anime;

};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	ITAアニメーション管理
//=====================================
static void FIELD_GRANM_Ita_Init( FIELD_GRANM_ITA* p_wk, u32 arcID, u32 dataID, u32 block_num, u32 heapID );
static void FIELD_GRANM_Ita_Exit( FIELD_GRANM_ITA* p_wk );
static fx32 FIELD_GRANM_Ita_Main( FIELD_GRANM_ITA* p_wk, fx32 frame );
static FIELD_GRANM_WORK* FIELD_GRANM_Ita_GetWork( const FIELD_GRANM_ITA* cp_wk, u32 index );

//-------------------------------------
///	フィールド地面アニメーションワーク
//=====================================
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, u32 heapID );
static void FIELD_GRANM_Work_Exit( FIELD_GRANM_WORK* p_wk );
static void FIELD_GRANM_Work_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj );
static void FIELD_GRANM_Work_Release( FIELD_GRANM_WORK* p_wk );
static fx32 FIELD_GRANM_Work_SetAnimeFrame( FIELD_GRANM_WORK* p_wk, fx32 frame );


//-------------------------------------
///	ITPアニメーション管理
//=====================================
static void FIELD_GRANM_Itp_Init( FIELD_GRANM_ITP* p_wk, u32 arcID, u32 tex_arcID, u32 anmID, u32 texID, const GFL_G3D_RES* cp_tex, u32 heapID );
static void FIELD_GRANM_Itp_Exit( FIELD_GRANM_ITP* p_wk );
static void FIELD_GRANM_Itp_Main( FIELD_GRANM_ITP* p_wk, fx32 speed );
static u32 FIELD_GRANM_Itp_GetTransTexAddr( const GFL_G3D_RES* cp_ground_tex, const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );
static u32 FIELD_GRANM_Itp_GetTransTexSize( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );
static void* FIELD_GRANM_Itp_GetAnimeTex( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );



//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメ管理システム　	生成
 *
 *	@param	cp_setup			セットアップデータ
 *	@param	cp_tex				地面テクスチャ
 *	@param	heapID				ヒープID
 *
 *	@return	地面アニメ管理システム
 */
//-----------------------------------------------------------------------------
FIELD_GRANM* FIELD_GRANM_Create( const FIELD_GRANM_SETUP* cp_setup, const GFL_G3D_RES* cp_tex, u32 heapID )
{
	FIELD_GRANM* p_sys;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM) );

	// ITAアニメーションの初期化処理
	if( cp_setup->ita_use ){
		FIELD_GRANM_Ita_Init( &p_sys->ita_anime, cp_setup->ita_arcID, cp_setup->ita_dataID, cp_setup->block_num, heapID );
	}

	// ITPアニメーションの初期化処理
	if( cp_setup->itp_use ){
		FIELD_GRANM_Itp_Init( &p_sys->itp_anime, cp_setup->itp_arcID, cp_setup->itp_texarcID, cp_setup->itp_anmID, cp_setup->itp_texID, cp_tex, heapID );
	}

	// オートアニメーションフラグ設定
	p_sys->anime_flag		= TRUE;
	p_sys->anime_speed	= FX32_ONE;
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションシステム破棄
 *
 *	@param	p_sys			システム
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_Delete( FIELD_GRANM* p_sys )
{

	// ITPアニメーションの破棄
	FIELD_GRANM_Itp_Exit( &p_sys->itp_anime );

	// ITAアニメーションの破棄処理
	FIELD_GRANM_Ita_Exit( &p_sys->ita_anime );
	
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションシステム　メイン
 *
 *	@param	p_sys			システム
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_Main( FIELD_GRANM* p_sys )
{

	// オートアニメーション処理
	if( p_sys->anime_flag )
	{
		p_sys->anime_frame += p_sys->anime_speed;
	}

	// ITAアニメーション処理
	p_sys->anime_frame = FIELD_GRANM_Ita_Main( &p_sys->ita_anime, p_sys->anime_frame );

	// ITPアニメーション
	FIELD_GRANM_Itp_Main( &p_sys->itp_anime, p_sys->anime_speed );
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションシステム　地面アニメーションワークの取得
 *
 *	@param	cp_sys		システムワーク
 *	@param	idx				地面ブロックインデックス
 *
 *	@return	地面アニメーションワーク
 */
//-----------------------------------------------------------------------------
FIELD_GRANM_WORK* FIELD_GRANM_GetWork( const FIELD_GRANM* cp_sys, u32 idx )
{
	GF_ASSERT( cp_sys );
	return FIELD_GRANM_Ita_GetWork( &cp_sys->ita_anime, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションスピード設定
 *
 *	@param	p_sys			システムワーク
 *	@param	speed			アニメーションスピード
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_SetAnimeSpeed( FIELD_GRANM* p_sys, fx32 speed )
{
	p_sys->anime_speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションスピードの取得
 *
 *	@param	cp_sys	システムワーク
 *
 *	@return	アニメーションスピード
 */
//-----------------------------------------------------------------------------
fx32 FIELD_GRANM_GetAnimeSpeed( const FIELD_GRANM* cp_sys )
{
	return cp_sys->anime_speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オートアニメーションフラグの設定
 *
 *	@param	p_sys		システムワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_SetAutoAnime( FIELD_GRANM* p_sys, BOOL flag )
{
	p_sys->anime_flag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オートアニメーションフラグの取得
 *
 *	@param	cp_sys	システムワーク
 *		
 *	@retval	TRUE	オートアニメーション中
 *	@retval	FALSE	アニメーション停止中
 */
//-----------------------------------------------------------------------------
BOOL FIELD_GRANM_GetAutoAnime( const FIELD_GRANM* cp_sys )
{
	return  cp_sys->anime_flag;
}



//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワーク　レンダーオブジェに関連付ける
 *
 *	@param	p_wk				ワーク
 *	@param	cp_mdlres		モデルリソース
 *	@param	cp_texres		テクスチャリソース					itp以外はNULLも可能
 *	@param	p_rendobj		関連付けるレンダーオブジェ
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_WORK_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj )
{
	FIELD_GRANM_Work_Bind( p_wk, cp_mdlres, cp_texres, p_rendobj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワーク	レンダーオブジェとの関連付けを解除
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_WORK_Release( FIELD_GRANM_WORK* p_wk )
{
	FIELD_GRANM_Work_Release( p_wk );
}





//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	ITAアニメーション管理システム初期化
 *
 *	@param	p_wk				ワーク
 *	@param	arcID				アーカイブID
 *	@param	dataID			データID
 *	@param	block_num		ブロック数
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Ita_Init( FIELD_GRANM_ITA* p_wk, u32 arcID, u32 dataID, u32 block_num, u32 heapID )
{
	int i;
	void* p_anmres;

	// アニメーションファイル読み込み
	p_wk->p_anmfile = GFL_ARC_UTIL_Load( arcID, dataID, FALSE, heapID );
	p_anmres = NNS_G3dGetAnmByIdx( p_wk->p_anmfile, 0 );
	
	// 地面アニメ管理ワークバッファ作成
	p_wk->p_wkbuf	= GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM_WORK)*block_num );
	p_wk->wkmax		= block_num;
	for( i=0; i<p_wk->wkmax; i++ )
	{
		FIELD_GRANM_Work_Init( &p_wk->p_wkbuf[i], p_anmres, heapID );
	}
}

//----------------------------------------------------------------------------
//
/**
 *	@brief	ITAアニメーション管理システム破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Ita_Exit( FIELD_GRANM_ITA* p_wk )
{
	int i;

	// 初期化されていたら破棄
	if(p_wk->p_anmfile){

		for( i=0; i<p_wk->wkmax; i++ )
		{
			FIELD_GRANM_Work_Exit( &p_wk->p_wkbuf[i] );
		}
		GFL_HEAP_FreeMemory( p_wk->p_wkbuf );
		p_wk->p_wkbuf = NULL;
		GFL_HEAP_FreeMemory( p_wk->p_anmfile );
		p_wk->p_anmfile = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITAアニメーション処理	フレーム管理
 *
 *	@param	p_wk		ワーク
 *	@param	frame		フレーム数
 *
 *	@return	実際に設定したフレーム
 */
//-----------------------------------------------------------------------------
static fx32 FIELD_GRANM_Ita_Main( FIELD_GRANM_ITA* p_wk, fx32 frame )
{
	int i;

	// 初期化されていたら処理する
	if( !p_wk->p_anmfile ){
		return frame;
	}

	// フレーム数設定
	for( i=0; i<p_wk->wkmax; i++ )
	{
		// FIELD_GRANM_Work_SetAnimeFrameの中で、
		// アニメーションフレームの最大値で、値をループさせています。
		frame = FIELD_GRANM_Work_SetAnimeFrame( &p_wk->p_wkbuf[i], frame );
	}

	return frame;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ブロックのITAアニメーションワークを取得
 *
 *	@param	p_wk			ワーク
 *	@param	index			インデックス
 *
 *	@retval	ワーク
 *	@retval	NULL		ITAアニメーションなし
 */
//-----------------------------------------------------------------------------
static FIELD_GRANM_WORK* FIELD_GRANM_Ita_GetWork( const FIELD_GRANM_ITA* cp_wk, u32 index )
{

	// 初期化されていたら処理する
	if( !cp_wk->p_anmfile ){
		return NULL;
	}

	GF_ASSERT( index < cp_wk->wkmax );
	return &cp_wk->p_wkbuf[index];
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワークの初期化
 *
 *	@param	p_wk				ワーク
 *	@param	p_anmres		アニメーションリソース
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, u32 heapID )
{
	GF_ASSERT( p_wk->p_anmobj == NULL );
	p_wk->p_anmobj = GFL_HEAP_AllocClearMemory( heapID, FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE );	
	p_wk->p_anmres = p_anmres;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワークの破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Exit( FIELD_GRANM_WORK* p_wk )
{
	GF_ASSERT( p_wk->p_anmobj );
	GFL_HEAP_FreeMemory( p_wk->p_anmobj );
	p_wk->p_anmobj = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワーク　レンダーオブジェに関連付ける
 *
 *	@param	p_wk					ワーク
 *	@param	cp_mdlres			モデルリソース
 *	@param	cp_texres			テクスチャリソース					itp以外はNULLも可能
 *	@param	p_rendobj			関連付けるレンダーオブジェ
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj )
{
	const NNSG3dResMdl* cp_mdl;
	const NNSG3dResTex* cp_tex;

	GF_ASSERT( cp_mdlres );
	GF_ASSERT( p_rendobj );

	
	// モデル情報取得
	cp_mdl = NNS_G3dGetMdlByIdx( NNS_G3dGetMdlSet( GFL_G3D_GetResourceFileHeader( cp_mdlres ) ), 0 );

	// モデル内マテリアル数チェック
	GF_ASSERT( cp_mdl->info.numMat < FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX );

	if( cp_texres )
	{
		cp_tex = GFL_G3D_GetResTex( cp_texres );
	}
	else
	{
		cp_tex = NULL;
	}
	
	// アニメーションオブジェの初期化
	GFL_STD_MemClear( p_wk->p_anmobj, FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE );
	NNS_G3dAnmObjInit( p_wk->p_anmobj, p_wk->p_anmres, cp_mdl, cp_tex );

	// レンダーオブジェに関連付け
	NNS_G3dRenderObjAddAnmObj( p_rendobj, p_wk->p_anmobj );

	p_wk->bind = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワーク	レンダーオブジェとの関連付け解除
 *
 *	@param	p_wk			ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Release( FIELD_GRANM_WORK* p_wk )
{
	p_wk->bind = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションワーク　フレーム設定
 *
 *	@param	p_wk			ワーク
 *	@param	frame			フレーム
 *
 *	@return	設定フレーム
 */
//-----------------------------------------------------------------------------
static fx32 FIELD_GRANM_Work_SetAnimeFrame( FIELD_GRANM_WORK* p_wk, fx32 frame )
{
	if( p_wk->bind )
	{
		fx32 max_frame;
		
		max_frame = NNS_G3dAnmObjGetNumFrame( p_wk->p_anmobj );
		frame			= frame % max_frame;
		
		NNS_G3dAnmObjSetFrame( p_wk->p_anmobj, frame );
	}

	return frame;
}




//----------------------------------------------------------------------------
/**
 *	@brief	ITPアニメーション管理システムの初期化
 *
 *	@param	p_wk			ワーク
 *	@param	arcID			アーカイブID
 *	@param	tex_arcID	アーカイブID
 *	@param	anmID			アニメーションデータID
 *	@param	texID			テクスチャデータID
 *	@param	cp_tex		地面テクスチャ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Init( FIELD_GRANM_ITP* p_wk, u32 arcID, u32 tex_arcID, u32 anmID, u32 texID, const GFL_G3D_RES* cp_tex, u32 heapID )
{
	// 地面テクスチャ保存
	p_wk->cp_ground_tex = cp_tex;
	
	// アニメーション情報読み込み
	p_wk->p_anmfile = GFL_ARC_UTIL_Load( arcID, anmID, FALSE, heapID );
	TEXANM_UnPackLoadFile( p_wk->p_anmfile, &p_wk->anmtbl );

	// アニメーションテクスチャ読み込み
	p_wk->p_tex = GFL_G3D_CreateResourceArc( tex_arcID, texID );

	// アニメーション数分の情報作成
	{
		int i;
		
		p_wk->anime_num			= TEXANM_GetAnimeNum( &p_wk->anmtbl );
		p_wk->p_last_data		= GFL_HEAP_AllocClearMemory( heapID, sizeof(TEXANM_DATA) * p_wk->anime_num );
		p_wk->p_trans_addr	= GFL_HEAP_AllocClearMemory( heapID, sizeof(u32) * p_wk->anime_num );
		p_wk->p_trans_size	= GFL_HEAP_AllocClearMemory( heapID, sizeof(u32) * p_wk->anime_num );
		p_wk->p_anime_frame	= GFL_HEAP_AllocClearMemory( heapID, sizeof(fx32) * p_wk->anime_num );

		for( i=0; i<p_wk->anime_num; i++ ){
			// 初期フレーム取得
			p_wk->p_last_data[i]	= TEXANM_GetFrameData( &p_wk->anmtbl, i, 0 );
			p_wk->p_trans_addr[i]	= FIELD_GRANM_Itp_GetTransTexAddr( cp_tex, p_wk->p_tex, p_wk->p_last_data[i].tex_idx );
			p_wk->p_trans_size[i]	= FIELD_GRANM_Itp_GetTransTexSize( p_wk->p_tex, p_wk->p_last_data[i].tex_idx );
		}
	}

	// 強制転送命令
	p_wk->trans = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITPアニメーション管理システムの破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Exit( FIELD_GRANM_ITP* p_wk )
{
	if( p_wk->p_anmfile ){

		// アニメーション分の情報
		GFL_HEAP_FreeMemory( p_wk->p_last_data );
		GFL_HEAP_FreeMemory( p_wk->p_trans_addr );
		GFL_HEAP_FreeMemory( p_wk->p_trans_size );
		GFL_HEAP_FreeMemory( p_wk->p_anime_frame );
		
		
		GFL_HEAP_FreeMemory( p_wk->p_anmfile );
		p_wk->p_anmfile = NULL;
		GFL_G3D_DeleteResource( p_wk->p_tex );
		p_wk->p_tex = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITPテクスチャ転送アニメーション管理　メイン
 *
 *	@param	p_wk			ワーク
 *	@param	speed			アニメーションスピード
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Main( FIELD_GRANM_ITP* p_wk, fx32 speed )
{
	int i;
	TEXANM_DATA now_data;
	fx32 frame_max;
	BOOL result;
	
	// 初期化されていないなら、何もしない
	if( !p_wk->p_anmfile ){
		return ;
	}
	
	// アニメーション分更新チェック
	for( i=0; i<p_wk->anime_num; i++ ){

		// アニメーション処理
		frame_max = TEXANM_GetLastKeyFrame( &p_wk->anmtbl, i ) << FX32_SHIFT;
		p_wk->p_anime_frame[i] += speed;
		if( p_wk->p_anime_frame[i] >= frame_max ){
			p_wk->p_anime_frame[i] = p_wk->p_anime_frame[i] % frame_max;
		}
		
		// チェックの必要があるかチェック
		if( p_wk->p_trans_addr[i] != FIELD_GRANM_ITP_TRANS_ADDR_NONE ){
			
			now_data = TEXANM_GetFrameData( &p_wk->anmtbl, i, p_wk->p_anime_frame[i]>>FX32_SHIFT );	
			
			// 更新チェック
			if((now_data.tex_idx != p_wk->p_last_data[i].tex_idx) ||
				p_wk->trans ){

				// 転送処理
				result = NNS_GfdRegisterNewVramTransferTask(
						NNS_GFD_DST_3D_TEX_VRAM, 
						p_wk->p_trans_addr[i], 
						FIELD_GRANM_Itp_GetAnimeTex( p_wk->p_tex, now_data.tex_idx ), 
						p_wk->p_trans_size[i] );

				GF_ASSERT( result );

			}

			// 保存
			p_wk->p_last_data[i] = now_data;
		}
	}

	// 更新完了
	p_wk->trans = FALSE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	転送先アドレスの取得処理
 *
 *	@param	cp_ground_tex			グラウンドテクスチャ
 *	@param	cp_anmtex					アニメーションテクスチャ
 *	@param	anm_tex_idx				調べるアニメーションテクスチャのインデックス
 *
 *	@retval	転送先アドレス
 *	@retval	FIELD_GRANM_ITP_TRANS_ADDR_NONE	転送先なし
 */
//-----------------------------------------------------------------------------
static u32 FIELD_GRANM_Itp_GetTransTexAddr( const GFL_G3D_RES* cp_ground_tex, const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_ground_restex;
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResName* cp_name;
	const NNSG3dResDictTexData* cp_ground_dict_tex;
	int ground_tex_idx;
	u32	addr;

	// テクスチャ情報取得
	cp_ground_restex	= GFL_G3D_GetResTex( cp_ground_tex );
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	cp_name = NNS_G3dGetTexNameByIdx( cp_anm_restex, anm_tex_idx );
	GF_ASSERT( cp_name );

	// 地面テクスチャでのNAMEのテクスチャインデックス取得
	ground_tex_idx = NNS_G3dGetTexIdxByName( cp_ground_restex, cp_name );
	if( ground_tex_idx < 0 ){
		return FIELD_GRANM_ITP_TRANS_ADDR_NONE;
	}

	// 転送先アドレス取得
	cp_ground_dict_tex = NNS_G3dGetTexDataByIdx( cp_ground_restex, ground_tex_idx );

	// オフセット値
	addr = (cp_ground_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK)<<3;
	addr += NNS_GfdGetTexKeyAddr( cp_ground_restex->texInfo.vramKey );

	return addr;
}

//----------------------------------------------------------------------------
/**
 *	@brief	転送テクスチャサイズの取得
 *
 *	@param	cp_anmtex		アニメーションテクスチャ
 *	@param	anm_tex_idx	テクスチャインデックス
 *
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
static u32 FIELD_GRANM_Itp_GetTransTexSize( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResDictTexData* cp_dict_tex;
	u32 byte_size;
	u32 format;
	u32 t_size,s_size;
	u32 texel_size;
	// 何テクセルに1バイトなのか
	static const u8 sc_TEXFMT_SIZE[] = {
		0xff,		//   GX_TEXFMT_NONE = 0,
		1,      //   GX_TEXFMT_A3I5 = 1,
		4,      //   GX_TEXFMT_PLTT4 = 2,
		2,      //   GX_TEXFMT_PLTT16 = 3,
		1,      //   GX_TEXFMT_PLTT256 = 4,
		0xff,   //   GX_TEXFMT_COMP4x4 = 5,
		1,			//   GX_TEXFMT_A5I3 = 6,
		0xff,		//   GX_TEXFMT_DIRECT = 7
	};        


	// テクスチャ情報取得
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	// size
	cp_dict_tex = NNS_G3dGetTexDataByIdx( cp_anm_restex, anm_tex_idx );
	// アニメーションとテクスチャデータの整合性があっていない
	GF_ASSERT( cp_dict_tex );

	// formatを調べる
	format = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEXFMT_MASK)>>NNS_G3D_TEXIMAGE_PARAM_TEXFMT_SHIFT;
	texel_size = sc_TEXFMT_SIZE[format];

	// 対応していないテクスチャformatです
	GF_ASSERT( format< NELEMS(sc_TEXFMT_SIZE) );
	GF_ASSERT( texel_size != 0xff );

	//テクスチャの横サイズを取得
	s_size = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_S_SIZE_MASK)>>NNS_G3D_TEXIMAGE_PARAM_S_SIZE_SHIFT;
	s_size <<= 4;

	//テクスチャの縦サイズを取得
	t_size = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_T_SIZE_MASK)>>NNS_G3D_TEXIMAGE_PARAM_T_SIZE_SHIFT;
	t_size <<= 4;

	//バイトサイズを計算
	byte_size = (s_size*t_size)/texel_size;

	return byte_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	転送するテクスチャ実データ取得
 *
 *	@param	cp_anmtex			アニメーションテクスチャ
 *	@param	anm_tex_idx		アニメーションテクスチャインデックス
 *
 *	@return	リソース実データ
 */
//-----------------------------------------------------------------------------
static void* FIELD_GRANM_Itp_GetAnimeTex( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResDictTexData* cp_dict_tex;
	u32	offset;

	// テクスチャ情報取得
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	// size
	cp_dict_tex = NNS_G3dGetTexDataByIdx( cp_anm_restex, anm_tex_idx );
	GF_ASSERT( cp_dict_tex );	// 

	// オフセット値
	offset = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK);

	return (void*)((u8*)cp_anm_restex + cp_anm_restex->texInfo.ofsTex + (offset << 3) );
}

