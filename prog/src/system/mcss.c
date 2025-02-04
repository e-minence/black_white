
//============================================================================================
/**
 * @file	mcss.c
 * @brief	マルチセルソフトウエアスプライト描画用関数
 * @author	soga
 * @date	2008.09.16
 */
//============================================================================================
#define	POKEGRA_LZ	// ポケグラ圧縮有効定義

#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include <math.h>

#include "arc_def.h"
#include "system/palanm.h"
#include "battle/battgra_wb.naix"

//#define	USE_RENDER		      //有効にすることでNNSのレンダラを使用して描画する
#include "system/mcss.h"	    //内部でUSE_RENDERを参照しているのでここより上に移動は不可
#include "system/mcss_tool.h"	//内部でUSE_RENDERを参照しているのでここより上に移動は不可
#include "mcss_def.h"

#define	MCSS_DEFAULT_SHIFT		( FX32_SHIFT - 4 )		//ポリゴン１辺の基準の長さにするシフト値
#define	MCSS_DEFAULT_LINE		( 1 << MCSS_DEFAULT_SHIFT )	//ポリゴン１辺の基準の長さ
#define MCSS_SCALE_OFFSET   ( 0 ) //0x1c )

#define	MCSS_CONST(x)	( x << MCSS_DEFAULT_SHIFT )

#define	MCSS_TEX_ADRS	(0x24000)
#define	MCSS_TEX_SIZE	(0x4000)
#define	MCSS_PAL_ADRS	(0x1000)
#define	MCSS_PAL_SIZE	(0x0020)

#define	MCSS_NORMAL_MTX	( 29 )
#define	MCSS_SHADOW_MTX	( 30 )

#define	MCSS_DEFAULT_SHADOW_ROTATE_X	( 0xC400 )
#define	MCSS_DEFAULT_SHADOW_ROTATE_Z	( 0 )

#define	MCSS_DEFAULT_SHADOW_OFFSET	( -0x1000 )	//影位置のZ方向のオフセット値

#define MCSS_NCEC_HEADER_SIZE ( 12 )

#define MCSS_FLIP_NONE	(0)  //そのまま
#define MCSS_FLIP_X		(1)   //X反転

//--------------------------------------------------------------------------
/**
 * 構造体宣言
 */
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * プロトタイプ宣言
 */
//--------------------------------------------------------------------------
static	void	MCSS_DrawAct(
                MCSS_SYS_WORK* mcss_sys,
                MCSS_WORK *mcss, 
							  fx32 pos_x,
							  fx32 pos_y,
							  fx32 scale_x,
							  fx32 scale_y,
							  fx32 tex_s,
							  fx32 tex_t,
							  NNSG2dAnimDataSRT *anim_SRT_c,
							  NNSG2dAnimDataSRT *anim_SRT_mc,
							  NNSG2dImagePaletteProxy *shadow_palette,
							  int node,
							  u32 mcss_ortho_mode,
							  fx32 *pos_z_default,const u8 isFlip );

static	void	MCSS_LoadResource( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw );
static	void	MCSS_LoadResourceByHandle( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw );
static	void	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType );
static	void	MCSS_MaterialSetup( void );
static	NNSG2dMultiCellAnimation*     GetNewMultiCellAnim_( u16 num );
static	void	MCSS_CalcPaletteFade( MCSS_SYS_WORK* mcss_sys, MCSS_WORK *mcss );
static  void  MCSS_FreeResource( MCSS_WORK* mcss );
static  void  MCSS_FreeTCBLoadResource( GFL_TCB* tcb_p );
static  void  MCSS_FreeTCBLoadPalette( GFL_TCB* tcb_p );

static	void	TCB_LoadResource( GFL_TCB *tcb, void *work );
static	void	TCB_LoadPalette( GFL_TCB *tcb, void *work );
static  void  MCSS_CalcMosaic( MCSS_WORK* mcss, TCB_LOADRESOURCE_WORK* tlw );
static  inline  void  mosaic_make( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y, int col );
static  inline  int  mosaic_pos_get( int pos_x, int pos_y, int tex_x );
static  inline  u8  mosaic_col_get( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y );

static	void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w );

#ifdef USE_RENDER
static	void	MCSS_InitRenderer( MCSS_SYS_WORK *mcss_sys );
#endif //USE_RENDER


#ifdef PM_DEBUG
MCSS_WORK*		MCSS_AddDebug( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_DEBUG_WORK *madw );
static	void	MCSS_LoadResourceDebug( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_DEBUG_WORK *madw );
#endif

//--------------------------------------------------------------------------
/**
 * @brief システム初期化
 *
 * @param[in] max     システムで使用できるMCSSのMAX値
 * @param[in] heapID  システムで使用するヒープID
 *
 * @retval  MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
MCSS_SYS_WORK*	MCSS_Init( int max, HEAPID heapID )
{
	MCSS_SYS_WORK	*mcss_sys;

	mcss_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(MCSS_SYS_WORK) );

	mcss_sys->mcss_max	= max;
	mcss_sys->heapID		= heapID;

	mcss_sys->mcAnimRate = FX32_ONE;

	mcss_sys->mcss =GFL_HEAP_AllocClearMemory( heapID, sizeof(MCSS_WORK *) * max );

#ifdef USE_RENDER
	MCSS_InitRenderer( mcss_sys );
#endif //USE_RENDER

	//テクスチャ系の転送先アドレスをセット
	mcss_sys->texAdrs = MCSS_TEX_ADRS;
	mcss_sys->palAdrs = MCSS_PAL_ADRS;

	//影リソースロード
	NNS_G2dInitImagePaletteProxy( &mcss_sys->shadow_palette_proxy );

	// load palette data
	{
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );

    tlw->mcss_sys = mcss_sys;
		tlw->palette_p = &mcss_sys->shadow_palette_proxy;
		tlw->pal_ofs = MCSS_PAL_ADRS + 0x100;
		tlw->pBufPltt = GFL_ARC_UTIL_LoadPalette( ARCID_BATTGRA, NARC_battgra_wb_shadow_NCLR, &tlw->pPlttData,
                                              GFL_HEAP_LOWID( heapID ) );
		GF_ASSERT( tlw->pBufPltt != NULL);

    mcss_sys->tcb_load_shadow = GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
	}
	
	return mcss_sys;
}

//--------------------------------------------------------------------------
/**
 * @brief システム終了
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_Exit( MCSS_SYS_WORK *mcss_sys )
{
	int	i;

  MCSS_CloseHandle( mcss_sys );

  if( mcss_sys->tcb_load_shadow )
  { 
	  TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)GFL_TCB_GetWork( mcss_sys->tcb_load_shadow );
	  GFL_HEAP_FreeMemory( tlw->pPlttData->pRawData );
	  GFL_HEAP_FreeMemory( tlw->pPlttData );
    GFL_HEAP_FreeMemory( tlw );
    GFL_TCB_DeleteTask( mcss_sys->tcb_load_shadow );
    mcss_sys->tcb_load_shadow = NULL;
  }

	for( i = 0 ; i < mcss_sys->mcss_max ; i++ ){
		if( mcss_sys->mcss[i] != NULL ){
			MCSS_Del( mcss_sys, mcss_sys->mcss[i] );
		}
	}
	GFL_HEAP_FreeMemory( mcss_sys->mcss );
	GFL_HEAP_FreeMemory( mcss_sys );
}

//--------------------------------------------------------------------------
/**
 * @brief システムメイン
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_Main( MCSS_SYS_WORK *mcss_sys )
{
	int	index;

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( ( mcss_sys->mcss[ index ] != NULL ) )
    { 
      if( mcss_sys->mcss[ index ]->anm_stop_flag == 0 ){
  			// アニメーションを更新します
  			NNS_G2dTickMCAnimation( &mcss_sys->mcss[ index ]->mcss_mcanim, mcss_sys->mcss[ index ]->mcss_anm_frame );
      }
			//パレットフェードチェック
			if( mcss_sys->mcss[ index ]->pal_fade_flag )
			{	
				MCSS_CalcPaletteFade( mcss_sys, mcss_sys->mcss[ index ] ); 
			}
			
			if( mcss_sys->mcss[ index ]->req_reset_anime == 1 )
			{
        MCSS_RestartAnime( mcss_sys->mcss[ index ] );
        //アニメを空更新
  			NNS_G2dTickMCAnimation( &mcss_sys->mcss[ index ]->mcss_mcanim, 0 );
  			mcss_sys->mcss[ index ]->req_reset_anime = 0;
      }
    }
	}
}

//--------------------------------------------------------------------------
/**
 * @brief リソースハンドルオープン
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  arcID    ハンドルオープンするリソースのARCID
 */
//--------------------------------------------------------------------------
void	MCSS_OpenHandle( MCSS_SYS_WORK *mcss_sys, ARCID arcID )
{ 
  GF_ASSERT( mcss_sys->handle == NULL );
  if( mcss_sys->handle == NULL )
  { 
    mcss_sys->arcID   = arcID;
    mcss_sys->handle  = GFL_ARC_OpenDataHandle( arcID, mcss_sys->heapID );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief リソースハンドルクローズ
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_CloseHandle( MCSS_SYS_WORK *mcss_sys )
{ 
  if( mcss_sys->handle )
  { 
    GFL_ARC_CloseDataHandle( mcss_sys->handle );
    mcss_sys->arcID   = 0;
    mcss_sys->handle  = NULL;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief TCBSYSセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  tcb_sys  TCBSYS
 */
//--------------------------------------------------------------------------
void	MCSS_SetTCBSys( MCSS_SYS_WORK *mcss_sys, GFL_TCBSYS* tcb_sys )
{ 
  mcss_sys->tcb_sys = tcb_sys;
}

//--------------------------------------------------------------------------
/**
 * @brief LoadResourceされる前に呼び出されるコールバック関数の設定
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  func     呼び出すコールバック関数
 * @param[in]  work     呼び出すコールバック関数の引数
 */
//--------------------------------------------------------------------------
void	MCSS_SetCallBackFunc( MCSS_SYS_WORK *mcss_sys, MCSS_CALLBACK_FUNC* func, u32 work )
{ 
  mcss_sys->load_resource_callback = func;
  mcss_sys->callback_work = work;
}

//--------------------------------------------------------------------------
/**
 * @brief コールバックワークの設定
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  work     呼び出すコールバック関数の引数
 */
//--------------------------------------------------------------------------
void	MCSS_SetCallBackWork( MCSS_SYS_WORK *mcss_sys, u32 work )
{ 
  mcss_sys->callback_work = work;
}

//--------------------------------------------------------------------------
/**
 * @brief リソースの再読み込み
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  mcss     再読み込みするmcss
 * @param[in]  maw      再読み込みするリソース
 */
//--------------------------------------------------------------------------
void	MCSS_ReloadResource( MCSS_SYS_WORK *mcss_sys, MCSS_WORK* mcss, MCSS_ADD_WORK* maw )
{ 
  MCSS_FreeResource( mcss );
	mcss->maw = *maw;
  MCSS_LoadResource( mcss_sys, mcss->index, maw );
}

#ifndef USE_RENDER
//独自描画
//--------------------------------------------------------------------------
/**
 * @brief 描画システム
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys )
{
	MCSS_WORK									*mcss;
	MCSS_NCEC									*ncec;
	NNSG2dImageProxy					*image_p;
	NNSG2dImagePaletteProxy		*palette_p;
	int												index;
	int												i,node,cell;
	NNSG2dAnimController			*anim_ctrl_mc;
	NNSG2dAnimDataSRT					anim_SRT_mc;
	NNSG2dAnimController			*anim_ctrl_c;
	NNSG2dAnimDataT						*anim_T_p;
	NNSG2dAnimDataSRT					anim_SRT;
	NNSG2dMCNodeCellAnimArray	*MC_Array;
	NNSG2dAnimSequenceData		*anim;
	VecFx32										pos,anim_pos;
	MtxFx44										inv_camera;
	u16												rotate;
	u8							          flipFlg;

  NNS_G3dGeFlushBuffer();

	G3_PushMtx();
	G3_MtxMode( GX_MTXMODE_PROJECTION );
	G3_StoreMtx( 0 );
	G3_MtxMode( GX_MTXMODE_TEXTURE );
	G3_Identity();
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );

	//ビルボード回転行列を求める
	{
		MtxFx43						camera;
		MtxFx33						rotate_mtx;

		MTX_Inverse43( NNS_G3dGlbGetCameraMtx(), &camera );	//カメラ逆行列取得
		MTX_Copy43To33( &camera, &rotate_mtx );		//カメラ逆行列から回転行列を取り出す
		MTX_Copy33To44( &rotate_mtx, &inv_camera );
	}

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( ( mcss_sys->mcss[index] != NULL ) && 
		    ( mcss_sys->mcss[index]->vanish_flag == 0 ) &&
		    ( mcss_sys->mcss[index]->is_load_resource == 1 ) ){
	    fx32    pos_z_default = 0;
	    int     ortho_mode = ( mcss_sys->mcss_ortho_mode == 0 ) ? 0 : mcss_sys->mcss[ index ]->ortho_mode;

			mcss		= mcss_sys->mcss[index];
			image_p		= &mcss->mcss_image_proxy;

	    if( ortho_mode == 0 ){ 
		    G3_LookAt( NNS_G3dGlbGetCameraPos(), NNS_G3dGlbGetCameraUp(), NNS_G3dGlbGetCameraTarget(), NULL );
	    }
	    else{
		    // カメラ行列を設定します。
		    // 単位行列と等価
		    VecFx32 Eye    = { 0, 0, 0 };          // Eye position
		    VecFx32 vUp    = { 0, FX32_ONE, 0 };  // Up
		    VecFx32 at     = { 0, 0, -FX32_ONE }; // Viewpoint

		    G3_LookAt( &Eye, &vUp, &at, NULL );
	    }

			anim_ctrl_mc= NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim);
			MC_Array = (NNSG2dMCNodeCellAnimArray*)&mcss->mcss_mcanim.multiCellInstance.pCellAnimInstasnces;

			switch( anim_ctrl_mc->pAnimSequence->animType & 0xffff ){
			case NNS_G2D_ANIMELEMENT_INDEX:		// Index のみ
				anim_SRT_mc.index	= *( (u16 *)anim_ctrl_mc->pCurrent->pContent );
				anim_SRT_mc.rotZ	= 0;
				anim_SRT_mc.sx		= 0x1000;
				anim_SRT_mc.sy		= 0x1000;
				anim_SRT_mc.px		= 0;
				anim_SRT_mc.py		= 0;
				break;
			case NNS_G2D_ANIMELEMENT_INDEX_SRT:	// Index + SRT 
				anim_SRT_mc = *( (NNSG2dAnimDataSRT *)anim_ctrl_mc->pCurrent->pContent );
				break;
			case NNS_G2D_ANIMELEMENT_INDEX_T:	// Index + T 
				anim_T_p			= anim_ctrl_mc->pCurrent->pContent;
				anim_SRT_mc.index	= anim_T_p->index;
				anim_SRT_mc.rotZ	= 0;
				anim_SRT_mc.sx		= 0x1000;
				anim_SRT_mc.sy		= 0x1000;
				anim_SRT_mc.px		= anim_T_p->px;
				anim_SRT_mc.py		= anim_T_p->py;
				break;
			default:
				GF_ASSERT(0);
				break;
			}

			pos.x = mcss->pos.x + mcss->ofs_pos.x;
			pos.y = mcss->pos.y + mcss->ofs_pos.y;
			pos.z = mcss->pos.z + mcss->ofs_pos.z;

	    if( ortho_mode == 0 )
      {
				anim_pos.x = MCSS_CONST( anim_SRT_mc.px );
				anim_pos.y = MCSS_CONST( -anim_SRT_mc.py );
				anim_pos.x = FX_Mul( anim_pos.x,mcss_sys->mcAnimRate );
				anim_pos.y = FX_Mul( anim_pos.y,mcss_sys->mcAnimRate );
			}
			else
      {
      	MtxFx44	mtx;
	      fx32	w;
        fx32  scale_x, scale_y;

        scale_x = FX_Mul( mcss->scale.x, mcss->ofs_scale.x );
        scale_y = FX_Mul( mcss->scale.y, mcss->ofs_scale.y );

	      MTX_Copy43To44( NNS_G3dGlbGetCameraMtx(), &mtx );
	      MTX_Concat44( &mtx, NNS_G3dGlbGetProjectionMtx(), &mtx );

	      MTX_MultVec44( &pos, &mtx, &pos, &w );

	      pos.x = FX_Mul( FX_Div( pos.x, w ), 128 * FX32_ONE );
	      pos.y = FX_Mul( FX_Div( pos.y, w ),  96 * FX32_ONE );
        pos.z = -pos.z;

				anim_pos.x = FX32_CONST( anim_SRT_mc.px );
				anim_pos.y = FX32_CONST( -anim_SRT_mc.py );
				anim_pos.x = FX_Mul( anim_pos.x, scale_x / 16 );
				anim_pos.y = FX_Mul( anim_pos.y, scale_y / 16 );
			}

			//前もって、不変なマルチセルデータをカレント行列にかけておく
			G3_Translate( pos.x, pos.y, pos.z );
			//カメラの逆行列を掛け合わせる（ビルボード処理）
	    if( ortho_mode == 0 ){ 
				G3_MultMtx44( &inv_camera );
			}

			rotate = ( anim_SRT_mc.rotZ + ( mcss->rotate.z >> FX32_SHIFT ) ) & 0xffff; 

			G3_Translate( anim_pos.x, anim_pos.y, 0 );
			G3_RotZ( -FX_SinIdx( rotate ), FX_CosIdx( rotate ) );
			{	
				VecFx32	scale;

				scale.x = FX_Mul( anim_SRT_mc.sx, mcss->scale.x );
				scale.y = FX_Mul( anim_SRT_mc.sy, mcss->scale.y );

				G3_Scale( FX_Mul( scale.x, mcss->ofs_scale.x ), FX_Mul( scale.y, mcss->ofs_scale.y ), FX32_ONE );
			}

			flipFlg = MCSS_FLIP_NONE;
			if( mcss->scale.x < 0 )
			{
				flipFlg += MCSS_FLIP_X;
			}
      
			G3_StoreMtx( MCSS_NORMAL_MTX );

			//影描画用の行列生成
			G3_LookAt( NNS_G3dGlbGetCameraPos(), NNS_G3dGlbGetCameraUp(), NNS_G3dGlbGetCameraTarget(), NULL );

			//前もって、不変なマルチセルデータをカレント行列にかけておく
			G3_Translate( mcss->pos.x + mcss->ofs_pos.x + mcss->shadow_offset.x, 
			              mcss->pos.y + mcss->shadow_offset.y , 
			              mcss->pos.z + mcss->ofs_pos.z + mcss->shadow_offset.z );

			//影用の回転
			G3_RotX( FX_SinIdx( mcss->shadow_rotate_x ), FX_CosIdx( mcss->shadow_rotate_x ) );
			{
        VecFx32 shadowAnmPos;
        shadowAnmPos.x = FX_Mul(MCSS_CONST( anim_SRT_mc.px ),mcss_sys->mcAnimRate);
        shadowAnmPos.y = FX_Mul(MCSS_CONST( -anim_SRT_mc.py ),mcss_sys->mcAnimRate);
        shadowAnmPos.x = FX_Mul(shadowAnmPos.x,mcss->shadow_scale.x);
        shadowAnmPos.y = FX_Mul(shadowAnmPos.y,mcss->shadow_scale.y);
  			G3_Translate( shadowAnmPos.x , shadowAnmPos.y, 0 );
  		}
			G3_RotZ( -FX_SinIdx( mcss->shadow_rotate_z ), FX_CosIdx( mcss->shadow_rotate_z ) );
			G3_RotZ( -FX_SinIdx( rotate ), FX_CosIdx( rotate ) );
			{	
				VecFx32	scale;

				scale.x = FX_Mul( anim_SRT_mc.sx, mcss->shadow_scale.x );
				scale.y = FX_Mul( anim_SRT_mc.sy, mcss->shadow_scale.y );
				scale.x = FX_Mul( scale.x, mcss->shadow_ofs_scale.x );
				scale.y = FX_Mul( scale.y, mcss->shadow_ofs_scale.y );

				G3_Scale( FX_Mul( scale.x, mcss->ofs_scale.x ), FX_Mul( scale.y, mcss->ofs_scale.y ), FX32_ONE );
			}

			G3_StoreMtx( MCSS_SHADOW_MTX );

			MCSS_MaterialSetup();

			G3_TexImageParam(image_p->attr.fmt,
							 GX_TEXGEN_TEXCOORD,
							 image_p->attr.sizeS,
							 image_p->attr.sizeT,
							 GX_TEXREPEAT_ST,
							 GX_TEXFLIP_ST,  ///< ここ
							 image_p->attr.plttUse,
							 image_p->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN]);
			for( i = 0 ; i < mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].numNodes ; i++ ){
        //半透明なら逆から描画するようにする
        if( ( mcss->alpha != 31 ) || ( mcss->reverse_draw ) )
        { 
          node = ( mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].numNodes - 1 ) - i;
        }
        else
        { 
          node = i;
        }
				cell = (mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].pHierDataArray[node].nodeAttr & 0xff00 ) >> 8;
				anim_ctrl_c = NNS_G2dGetCellAnimAnimCtrl(&MC_Array->cellAnimArray[cell].cellAnim);
				switch( anim_ctrl_c->pAnimSequence->animType & 0xffff ){
				case NNS_G2D_ANIMELEMENT_INDEX:		// Index のみ
					anim_SRT.index	= *( (u16 *)anim_ctrl_c->pCurrent->pContent );
					anim_SRT.rotZ	= 0;
					anim_SRT.sx		= 0x1000;
					anim_SRT.sy		= 0x1000;
					anim_SRT.px		= 0;
					anim_SRT.py		= 0;
					break;
				case NNS_G2D_ANIMELEMENT_INDEX_SRT:	// Index + SRT 
					anim_SRT = *( (NNSG2dAnimDataSRT *)anim_ctrl_c->pCurrent->pContent );
					break;
				case NNS_G2D_ANIMELEMENT_INDEX_T:	// Index + T 
					anim_T_p		= anim_ctrl_c->pCurrent->pContent;
					anim_SRT.index	= anim_T_p->index;
					anim_SRT.rotZ	= 0;
					anim_SRT.sx		= 0x1000;
					anim_SRT.sy		= 0x1000;
					anim_SRT.px		= anim_T_p->px;
					anim_SRT.py		= anim_T_p->py;
					break;
				default:
					GF_ASSERT(0);
					break;
				}

				ncec = &mcss->mcss_ncec->ncec[anim_SRT.index];

				//メパチ処理
				if( ( ncec->mepachi_size_x ) && ( mcss->reverse_draw == 0 ) )
        { 
					if( mcss->mepachi_flag ){
						MCSS_DrawAct(
                    mcss_sys,
                    mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t + ncec->mepachi_size_y,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
	                  ortho_mode,
									  &pos_z_default, flipFlg );
					}
					else{
						MCSS_DrawAct(
                    mcss_sys,
                    mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
	                  ortho_mode,
									  &pos_z_default, flipFlg );
					}
				}
				MCSS_DrawAct(
                mcss_sys,
                mcss,
							  ncec->pos_x,
							  ncec->pos_y,
							  ncec->size_x,
							  ncec->size_y,
							  ncec->tex_s,
							  ncec->tex_t,
							  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
	              ortho_mode,
							  &pos_z_default, flipFlg );
				//メパチ処理
				if( ( ncec->mepachi_size_x ) && ( mcss->reverse_draw == 1 ) )
        { 
					if( mcss->mepachi_flag ){
						MCSS_DrawAct(
                    mcss_sys,
                    mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t + ncec->mepachi_size_y,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
	                  ortho_mode,
									  &pos_z_default, flipFlg );
					}
					else{
						MCSS_DrawAct(
                    mcss_sys,
                    mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
	                  ortho_mode,
									  &pos_z_default, flipFlg );
					}
				}
			}
		}
	}
	G3_PopMtx(1);
}
#else
//レンダラシステムを用いた描画
//--------------------------------------------------------------------------
/**
 * @brief 描画システム
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys )
{
	MCSS_WORK	*mcss;
	int			index;
	int			cell;
	VecFx32		pos,scale;

	NNS_G2dSetupSoftwareSpriteCamera();
	G3_MtxMode( GX_MTXMODE_TEXTURE );
	G3_Identity();
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( mcss_sys->mcss[index] != NULL ){
			mcss=mcss_sys->mcss[index];
			//暫定でRendererを使用しているが、実際は独自で描画処理を作成しなければならない
			NNS_G2dBeginRendering( &mcss_sys->mcss_render );
			NNS_G2dPushMtx();
			NNS_G2dTranslate( mcss->pos.x,
							  mcss->pos.y,
							  mcss->pos.z);
			NNS_G2dSetRendererImageProxy( &mcss_sys->mcss_render,
										  &mcss->mcss_image_proxy,
										  &mcss->mcss_palette_proxy );
			NNS_G2dDrawMultiCellAnimation( &mcss->mcss_mcanim );
			NNS_G2dPopMtx();
			NNS_G2dEndRendering();
		}
	}
}
#endif

//--------------------------------------------------------------------------
/**
 * @brief マルチセルを描画
 *
 * @param[in]  mcss             MCSSワーク構造体のポインタ
 * @param[in]  pos_x            描画X座標
 * @param[in]  pos_y            描画Y座標
 * @param[in]  size_x           X方向サイズ
 * @param[in]  size_y           Y方向サイズ
 * @param[in]  tex_s            テクスチャS値
 * @param[in]  tex_t            テクスチャT値
 * @param[in]  anm_SRT_c        セルアニメデータ構造体のポインタ
 * @param[in]  anm_SRT_mc       マルチセルアニメデータ構造体のポインタ
 * @param[in]  shadow_palette   影パレットプロキシ
 * @param[in]  node             マルチセルのノード
 * @param[in]  mcss_ortho_mode  射影モードフラグ（0:透視射影　1:正射影）
 * @param[in]  pos_z_default    セルを描画する度にずらずZ方向のオフセット値
 */
//--------------------------------------------------------------------------
#ifdef DEBUG_ONLY_for_saitou
vu32  mcss_ortho_far = FX32_ONE * 512;
vu32  mcss_ortho_z_offset = MCSS_DEFAULT_Z_ORTHO_512;
#endif
static	void	MCSS_DrawAct(
                MCSS_SYS_WORK* mcss_sys,
                MCSS_WORK *mcss,
							  fx32 pos_x,
							  fx32 pos_y,
							  fx32 size_x,
							  fx32 size_y,
							  fx32 tex_s,
							  fx32 tex_t,
							  NNSG2dAnimDataSRT *anim_SRT_c,
							  NNSG2dAnimDataSRT *anim_SRT_mc,
							  NNSG2dImagePaletteProxy *shadow_palette,
							  int node,
							  u32 mcss_ortho_mode,
							  fx32 *pos_z_default,
                const u8 isFlip )
{
	VecFx32	pos;
  int polyID;
	//影のアルファ値計算
  const u8 shadow_alpha = (mcss->shadow_alpha == MCSS_SHADOW_ALPHA_AUTO ?(mcss->alpha/2):mcss->shadow_alpha); 

  if( ( mcss->alpha == 31 ) || ( mcss->alpha_same_polyID ) )
  { 
    polyID = 0;
  }
  else
  { 
    polyID = ( node > 63 ) ? 63 : node;
  }

	if( mcss_ortho_mode ){
#ifdef DEBUG_ONLY_for_saitou
		G3_OrthoW( FX32_CONST( 96 ),
				   -FX32_CONST( 96 ),
				   -FX32_CONST( 128 ),
				   FX32_CONST( 128 ),
				   FX32_ONE * 1,
				   //FX32_ONE * 1024,
				   mcss_ortho_far,
				   FX32_ONE,
				   NULL );
#else
		G3_OrthoW( FX32_CONST( 96 ),
				   -FX32_CONST( 96 ),
				   -FX32_CONST( 128 ),
				   FX32_CONST( 128 ),
				   FX32_ONE * 1,
				   //FX32_ONE * 1024,
				   FX32_ONE * ( ( mcss_sys->ortho_far_flag ) ? 512 : 1024 ),
				   FX32_ONE,
				   NULL );
#endif
		G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
	}

	G3_RestoreMtx( MCSS_NORMAL_MTX );

	G3_TexPlttBase(mcss->mcss_palette_proxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN],
				   mcss->mcss_palette_proxy.fmt);
	G3_PolygonAttr( GX_LIGHTMASK_NONE,				// no lights
                  GX_POLYGONMODE_MODULATE,	// modulation mode
                  GX_CULL_NONE,		    			// cull back
                  polyID,        						// polygon ID(0 - 63)
                  mcss->alpha,	       			// alpha(0 - 31)
                  GX_POLYGON_ATTR_MISC_NONE	// OR of GXPolygonAttrMisc's value
                );

	//マルチセルデータから取得した位置で書き出し
	pos.x = MCSS_CONST( mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc->index].pHierDataArray[node].posX ) + MCSS_CONST( anim_SRT_c->px );
	pos.y = MCSS_CONST( -mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc->index].pHierDataArray[node].posY ) + MCSS_CONST( -anim_SRT_c->py );

  G3_Translate( pos.x, pos.y, *pos_z_default );

	G3_RotZ( -FX_SinIdx( anim_SRT_c->rotZ ), FX_CosIdx( anim_SRT_c->rotZ ) );

  if( isFlip == MCSS_FLIP_NONE )
	{
		G3_Scale( anim_SRT_c->sx, anim_SRT_c->sy, FX32_ONE );

		G3_Translate( pos_x, pos_y, 0 );

		G3_Scale( size_x, size_y, FX32_ONE );
	}
	else
	if( isFlip & MCSS_FLIP_X )
	{
		//反転による調整
		G3_Scale(-anim_SRT_c->sx, anim_SRT_c->sy, FX32_ONE );

		G3_Translate( -pos_x, pos_y, 0 );

		G3_Scale( size_x, size_y, FX32_ONE );
		G3_Translate( -MCSS_DEFAULT_LINE, 0, 0 );
		//テクスチャのリピートでフリップした所を使う
		tex_s = FX32_CONST(256.0f) + FX32_CONST(256.0f) - tex_s - size_x;
	}

	G3_Begin(GX_BEGIN_QUADS);
	G3_TexCoord( tex_s,				tex_t );
	G3_Vtx( 0, 0, 0 );
	G3_TexCoord( tex_s,				tex_t + size_y );
	G3_Vtx( 0, -MCSS_DEFAULT_LINE, 0 );
	G3_TexCoord( tex_s + size_x,	tex_t + size_y );
	G3_Vtx( MCSS_DEFAULT_LINE, -MCSS_DEFAULT_LINE, 0 );
	G3_TexCoord( tex_s + size_x,	tex_t );
	G3_Vtx( MCSS_DEFAULT_LINE, 0, 0 );
	G3_End();

  //影描画
 	G3_MtxMode( GX_MTXMODE_PROJECTION );
 	G3_RestoreMtx( 0 );
 	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
 	G3_RestoreMtx( MCSS_SHADOW_MTX );

  if( ( mcss->shadow_vanish_flag == 0 ) && ( shadow_alpha ) )
  { 
   	G3_TexPlttBase( shadow_palette->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN], shadow_palette->fmt);
  	G3_PolygonAttr( GX_LIGHTMASK_NONE,				// no lights
        				    GX_POLYGONMODE_MODULATE,	// modulation mode
        				    GX_CULL_NONE,					    // cull back
        				    1,								        // polygon ID(0 - 63)
        				    shadow_alpha,             // alpha(0 - 31)
        				    GX_POLYGON_ATTR_MISC_NONE	// OR of GXPolygonAttrMisc's value
        				   );

  	G3_Translate( pos.x, pos.y, 0 );

  	G3_RotZ( -FX_SinIdx( anim_SRT_c->rotZ ), FX_CosIdx( anim_SRT_c->rotZ ) );

  	G3_Scale( anim_SRT_c->sx, anim_SRT_c->sy, FX32_ONE );

  	G3_Translate( pos_x, pos_y, 0 );

  	G3_Scale( size_x, size_y, FX32_ONE );

  	G3_Begin(GX_BEGIN_QUADS);
  	G3_TexCoord( tex_s,				tex_t );
  	G3_Vtx( 0, 0, 0 );
  	G3_TexCoord( tex_s,				tex_t + size_y );
  	G3_Vtx( 0, -MCSS_DEFAULT_LINE, 0 );
  	G3_TexCoord( tex_s + size_x,	tex_t + size_y );
  	G3_Vtx( MCSS_DEFAULT_LINE, -MCSS_DEFAULT_LINE, 0 );
  	G3_TexCoord( tex_s + size_x,	tex_t );
  	G3_Vtx( MCSS_DEFAULT_LINE, 0, 0 );
  	G3_End();
  }

	if( mcss_ortho_mode == 0 )
  { 
    if( mcss_sys->perspective_z_offset == 0 )
    {
		  *pos_z_default -= ( mcss->reverse_draw | ( mcss->alpha != 31 ) ) ? -MCSS_DEFAULT_Z : MCSS_DEFAULT_Z;
    }
    else
    { 
		  *pos_z_default -= ( mcss->reverse_draw | ( mcss->alpha != 31 ) ) ? -mcss_sys->perspective_z_offset :
                                                                          mcss_sys->perspective_z_offset;
    }
	}
	else{
    if( mcss_sys->ortho_far_flag )
    { 
#ifdef DEBUG_ONLY_for_saitou
		  *pos_z_default -= ( mcss->reverse_draw | ( mcss->alpha != 31 ) ) ? -mcss_ortho_z_offset : mcss_ortho_z_offset;
#else
		  *pos_z_default -= ( mcss->reverse_draw | ( mcss->alpha != 31 ) ) ? -MCSS_DEFAULT_Z_ORTHO_512 : MCSS_DEFAULT_Z_ORTHO_512;
#endif
    }
    else
    { 
		  *pos_z_default -= ( mcss->reverse_draw | ( mcss->alpha != 31 ) ) ? -MCSS_DEFAULT_Z_ORTHO : MCSS_DEFAULT_Z_ORTHO;
    }
	}
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセル登録
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  pos_x    描画X座標
 * @param[in]  pos_y    描画Y座標
 * @param[in]  pos_Z    描画Z座標
 * @param[in]  maw      マルチセル登録用パラメータ構造体のポインタ
 *
 * @retval  MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
MCSS_WORK*	MCSS_Add( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_WORK *maw )
{
	int			count;

	for( count = 0 ; count < mcss_sys->mcss_max ; count++ ){
		if( mcss_sys->mcss[ count ] == NULL ){
			mcss_sys->mcss[ count ] = GFL_HEAP_AllocClearMemory( mcss_sys->heapID, sizeof( MCSS_WORK ) );
			mcss_sys->mcss[ count ]->index = count;
			mcss_sys->mcss[ count ]->heapID = mcss_sys->heapID;
			mcss_sys->mcss[ count ]->pos.x = pos_x;
			mcss_sys->mcss[ count ]->pos.y = pos_y;
			mcss_sys->mcss[ count ]->pos.z = pos_z;
			mcss_sys->mcss[ count ]->scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_pos.x = 0;
			mcss_sys->mcss[ count ]->ofs_pos.y = 0;
			mcss_sys->mcss[ count ]->ofs_pos.z = 0;
			mcss_sys->mcss[ count ]->ofs_scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->alpha = 31;
			mcss_sys->mcss[ count ]->shadow_alpha = MCSS_SHADOW_ALPHA_AUTO;
			mcss_sys->mcss[ count ]->vanish_flag = MCSS_VANISH_OFF;
			mcss_sys->mcss[ count ]->shadow_rotate_x = MCSS_DEFAULT_SHADOW_ROTATE_X;
			mcss_sys->mcss[ count ]->shadow_rotate_z = MCSS_DEFAULT_SHADOW_ROTATE_Z;
			mcss_sys->mcss[ count ]->shadow_offset.x = 0;
			mcss_sys->mcss[ count ]->shadow_offset.y = 0;
			mcss_sys->mcss[ count ]->shadow_offset.z = MCSS_DEFAULT_SHADOW_OFFSET;
			mcss_sys->mcss[ count ]->shadow_ofs_scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->shadow_ofs_scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->shadow_ofs_scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->maw = *maw;
			mcss_sys->mcss[ count ]->mcss_anm_frame = FX32_ONE;
			mcss_sys->mcss[ count ]->ortho_mode = 1;
			MCSS_LoadResource( mcss_sys, count, maw );
			break;
		}
	}
	//登録MAX値をオーバー
	GF_ASSERT( count < mcss_sys->mcss_max );

	return mcss_sys->mcss[ count ];
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセル削除
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  mcss     MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss )
{
  MCSS_FreeResource( mcss );

  //リソースロードのTCBが動いていたら一緒に削除する
  if( mcss->tcb_load_resource )
  { 
    MCSS_FreeTCBLoadResource( mcss->tcb_load_resource );
    mcss->tcb_load_resource = NULL;
  }

  if( mcss->tcb_load_palette )
  { 
    MCSS_FreeTCBLoadPalette( mcss->tcb_load_palette );
    mcss->tcb_load_palette = NULL;
  }

  if( mcss->tcb_load_base_palette )
  { 
    MCSS_FreeTCBLoadPalette( mcss->tcb_load_base_palette );
    mcss->tcb_load_base_palette = NULL;
  }

	mcss_sys->mcss[ mcss->index ] = NULL;
	GFL_HEAP_FreeMemory( mcss );
}

//--------------------------------------------------------------------------
/**
 * @brief 正射影描画モードをセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetOrthoMode( MCSS_SYS_WORK *mcss_sys )
{
	mcss_sys->mcss_ortho_mode = 1;
}

//--------------------------------------------------------------------------
/**
 * @brief 正射影描画モードをリセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetOrthoMode( MCSS_SYS_WORK *mcss_sys )
{
	mcss_sys->mcss_ortho_mode = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief 正射影描画モードをセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetOrthoModeMcss( MCSS_WORK *mcss )
{
	mcss->ortho_mode = 1;
}

//--------------------------------------------------------------------------
/**
 * @brief 正射影描画モードをリセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetOrthoModeMcss( MCSS_WORK *mcss )
{
	mcss->ortho_mode = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief ポジションゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  取得したポジションを格納するワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	pos->x = mcss->pos.x;
	pos->y = mcss->pos.y;
	pos->z = mcss->pos.z;
}

//--------------------------------------------------------------------------
/**
 * @brief ポジションセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするポジションが格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	mcss->pos.x = pos->x;
	mcss->pos.y = pos->y;
	mcss->pos.z = pos->z;
}

//--------------------------------------------------------------------------
/**
 * @brief オフセットポジションゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  取得したポジションを格納するワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	pos->x = mcss->ofs_pos.x;
	pos->y = mcss->ofs_pos.y;
	pos->z = mcss->ofs_pos.z;
}

//--------------------------------------------------------------------------
/**
 * @brief オフセットポジションセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするポジションが格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	mcss->ofs_pos.x = pos->x;
	mcss->ofs_pos.y = pos->y;
	mcss->ofs_pos.z = pos->z;
}

//--------------------------------------------------------------------------
/**
 * @brief スケールゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  取得したスケール値を格納するワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	scale->x = mcss->scale.x;
	scale->y = mcss->scale.y;
	scale->z = mcss->scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief スケールセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->scale.x = scale->x;
	mcss->scale.y = scale->y;
	mcss->scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief オフセットスケールゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  取得したスケール値を格納するワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	scale->x = mcss->ofs_scale.x;
	scale->y = mcss->ofs_scale.y;
	scale->z = mcss->ofs_scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief オフセットスケールセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->ofs_scale.x = scale->x;
	mcss->ofs_scale.y = scale->y;
	mcss->ofs_scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief ローテートゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  取得したローテート値を格納するワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetRotate( MCSS_WORK *mcss, VecFx32 *rotate )
{
	rotate->x = mcss->rotate.x;
	rotate->y = mcss->rotate.y;
	rotate->z = mcss->rotate.z;
}

//--------------------------------------------------------------------------
/**
 * @brief ローテートセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするローテート値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetRotate( MCSS_WORK *mcss, VecFx32 *rotate )
{
	mcss->rotate.x = rotate->x;
	mcss->rotate.y = rotate->y;
	mcss->rotate.z = rotate->z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影描画用スケールゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetShadowScale( MCSS_WORK *mcss, VecFx32 *scale )
{
  scale->x = mcss->shadow_scale.x;
  scale->y = mcss->shadow_scale.y;
  scale->z = mcss->shadow_scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影描画用スケールセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->shadow_scale.x = scale->x;
	mcss->shadow_scale.y = scale->y;
	mcss->shadow_scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影描画用スケールゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_GetShadowOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
  scale->x = mcss->shadow_ofs_scale.x;
  scale->y = mcss->shadow_ofs_scale.y;
  scale->z = mcss->shadow_ofs_scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影描画用スケールセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->shadow_ofs_scale.x = scale->x;
	mcss->shadow_ofs_scale.y = scale->y;
	mcss->shadow_ofs_scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影描画用回転角セット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  rot  回転角
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowRotate( MCSS_WORK *mcss, const u16 rot )
{
  mcss->shadow_rotate_x = rot;
}
//--------------------------------------------------------------------------
/**
 * @brief 影描画用回転角セット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  rot  回転角
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowRotateZ( MCSS_WORK *mcss, const u16 rot )
{
  mcss->shadow_rotate_z = rot;
}

u16	MCSS_GetShadowRotateZ( MCSS_WORK *mcss )
{
  return  mcss->shadow_rotate_z;
}

//--------------------------------------------------------------------------
/**
 * @brief 影オフセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 * @param[in]  pos  セットするスケール値が格納されたワークのポインタ
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs )
{
	mcss->shadow_offset.x = ofs->x;
	mcss->shadow_offset.y = ofs->y;
	mcss->shadow_offset.z = ofs->z;
}
void  MCSS_GetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs )
{
	ofs->x = mcss->shadow_offset.x;
	ofs->y = mcss->shadow_offset.y;
	ofs->z = mcss->shadow_offset.z;
}

//--------------------------------------------------------------------------
/**
 * @brief メパチフラグセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag = MCSS_MEPACHI_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief メパチフラグリセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag = MCSS_MEPACHI_OFF;
}

//--------------------------------------------------------------------------
/**
 * @brief メパチフラグフリップ
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_FlipMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag ^= MCSS_MEPACHI_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief アニメストップフラグゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 *
 * @retval  フラグ
 */
//--------------------------------------------------------------------------
int MCSS_GetAnmStopFlag( MCSS_WORK *mcss )
{
	return mcss->anm_stop_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief アニメストップフラグセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnmStopFlag( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag |= MCSS_ANM_STOP_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief アニメストップフラグセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnmStopFlagAlways( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag |= MCSS_ANM_STOP_ALWAYS;
}

//--------------------------------------------------------------------------
/**
 * @brief アニメストップフラグリセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetAnmStopFlag( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag &= ( MCSS_ANM_STOP_ON ^ 0xffffffff );
}

//--------------------------------------------------------------------------
/**
 * @brief アニメストップフラグリセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetAnmStopFlagAlways( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag &= ( MCSS_ANM_STOP_ALWAYS ^ 0xffffffff );
}

//--------------------------------------------------------------------------
/**
 * @brief バニッシュフラグゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
int		MCSS_GetVanishFlag( MCSS_WORK *mcss )
{
	return mcss->vanish_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief バニッシュフラグセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag = MCSS_VANISH_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief バニッシュフラグリセット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag = MCSS_VANISH_OFF;
}

//--------------------------------------------------------------------------
/**
 * @brief バニッシュフラグフリップ
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_FlipVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag ^= MCSS_VANISH_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメコントロールゲット
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
NNSG2dMultiCellAnimation* MCSS_GetAnimCtrl( MCSS_WORK *mcss )
{
	return &mcss->mcss_mcanim;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメのシーケンス数を取得
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u16  MCSS_GetMCellAnmNum( MCSS_WORK *mcss )
{ 
  return mcss->mcss_nmar->numSequences;
}

//--------------------------------------------------------------------------
/**
 * @brief 静止アニメ中でもパターンアニメするセルの数を取得
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u8  MCSS_GetStopCellAnms( MCSS_WORK *mcss )
{
	return mcss->mcss_ncen->stop_cellanms;
}

//--------------------------------------------------------------------------
/**
 * @brief 静止アニメ中でもパターンアニメするセルのノードを取得
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 * @param[in]  index  ノードを取得するインデックス
 */
//--------------------------------------------------------------------------
u8  MCSS_GetStopNode( MCSS_WORK *mcss, u8 index )
{
  GF_ASSERT( index < mcss->mcss_ncen->stop_cellanms );
  
	return mcss->mcss_ncen->stop_node[ index ];
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルのおおよそのXサイズを取得
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u16  MCSS_GetSizeX( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->size_x;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルのおおよそのYサイズを取得
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u16  MCSS_GetSizeY( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->size_y;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルのX方向のズレを取得
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
s16  MCSS_GetOffsetX( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->ofs_x;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルのY方向のズレを取得
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
s16  MCSS_GetOffsetY( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->ofs_y;
}

//--------------------------------------------------------------------------
/**
 * @brief 浮いてるアニメパターンなのかを取得
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u8  MCSS_GetFlyFlag( MCSS_WORK *mcss )
{
	return mcss->mcss_ncen->fly_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief セルの枚数を取得
 *
 * @param[in]  mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u32 MCSS_GetCells( MCSS_WORK *mcss )
{
	return mcss->mcss_ncec->cells;
}

//--------------------------------------------------------------------------
/**
 * @brief 指定したフレームになったらコールバック関数を呼ぶようにAnmCtrlに登録
 *
 * @param[in]  mcss     MCSSワーク構造体のポインタ
 * @param[in]  param  ユーザが自由に使用できるコールバック引数
 * @param[in]  pFunc    呼ばれるコールバック関数のポインタ
 * @param[in]  frameIdx コールバックを呼ぶフレーム
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnimCtrlCallBack( MCSS_WORK *mcss, u32 param, NNSG2dAnmCallBackPtr pFunc, u16 frameIdx )
{	

#if 1
	NNS_G2dSetAnimCtrlCallBackFunctor( NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim),
																		 NNS_G2D_ANMCALLBACKTYPE_LAST_FRM, param, pFunc );
#else
  NNS_G2dSetCallBackFunctorAtAnimFrame( NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim), param, pFunc, frameIdx );
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief ノード巡回してコールバック関数を呼ぶように登録
 *
 * @param[in]  mcss   MCSSワーク構造体のポインタ
 * @param[in]  param  ユーザが自由に使用できるコールバック引数
 * @param[in]  pFunc  呼ばれるコールバック関数のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetTraverseMCNodesCallBack( MCSS_WORK *mcss, u32 param, NNSG2dMCTraverseNodeCallBack pFunc )
{	
	NNS_G2dTraverseMCNodes( &mcss->mcss_mcanim.multiCellInstance, pFunc, param );
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェードセット
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 * @param[in]	start_evy	セットするパラメータ（フェードさせる色に対する開始割合16段階）
 * @param[in]	end_evy		セットするパラメータ（フェードさせる色に対する終了割合16段階）
 * @param[in]	wait			セットするパラメータ（ウェイト）（−値を入れるとevyの増減値を増やしてさらに早くなります）
 * @param[in]	rgb				セットするパラメータ（フェードさせる色）
 */
//--------------------------------------------------------------------------
void	MCSS_SetPaletteFade( MCSS_WORK *mcss, u8 start_evy, u8 end_evy, s8 wait, u32 rgb )
{	
	GF_ASSERT( mcss );

	mcss->pal_fade_flag				= 1;
	mcss->pal_fade_start_evy	= start_evy;
	mcss->pal_fade_end_evy		= end_evy;
	mcss->pal_fade_rgb				= rgb;
  mcss->pal_fade_wait				= 0;

  if( wait < 0 )
  { 
	  mcss->pal_fade_wait_tmp		= 0;
    mcss->pal_fade_value			= ( wait - 1 ) * -1;
  }
  else
  { 
	  mcss->pal_fade_wait_tmp		= wait;
    mcss->pal_fade_value			= 1;
  }

  if( start_evy > end_evy )
  { 
    mcss->pal_fade_value			*= -1;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief α値をゲット
 *
 * @param[in] mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u8	MCSS_GetAlpha( MCSS_WORK *mcss )
{ 
  return mcss->alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief α値をセット
 *
 * @param[in]	mcss		セットするマルチセルワーク構造体
 * @param[in]	alpha		セットするα値(0-31)
 */
//--------------------------------------------------------------------------
void	MCSS_SetAlpha( MCSS_WORK *mcss, u8 alpha )
{ 
  mcss->alpha = alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief alpha_same_polyIDに値をセット
 *
 * @param[in]	mcss		セットするマルチセルワーク構造体
 * @param[in]	flag		セットする値
 */
//--------------------------------------------------------------------------
void	MCSS_SetAlphaSamePolyID( MCSS_WORK *mcss, BOOL flag )
{ 
  mcss->alpha_same_polyID = flag;
}

//--------------------------------------------------------------------------
/**
 * @brief 影のα値をゲット
 *
 * @param[in] mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u8    MCSS_GetShadowAlpha( MCSS_WORK *mcss )
{
  return mcss->shadow_alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief 影のα値をセット
 *
 * @param[in]	mcss		セットするマルチセルワーク構造体
 * @param[in]	alpha		セットするα値(0-31)
                      MCSS_SHADOW_ALPHA_AUTO 影のアルファ値を本体の半分にします  
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowAlpha( MCSS_WORK *mcss, u8 shadow_alpha )
{
  mcss->shadow_alpha = shadow_alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief 影のバニッシュフラグをゲット
 *
 * @param[in] mcss MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u8    MCSS_GetShadowVanishFlag( MCSS_WORK *mcss )
{
  return mcss->shadow_vanish_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief 影のバニッシュフラグをセット
 *
 * @param[in]	mcss		セットするマルチセルワーク構造体
 * @param[in]	flag		セットする値
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowVanishFlag( MCSS_WORK *mcss, u8 flag )
{
  mcss->shadow_vanish_flag = flag;
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェード中かチェック
 *
 * @param[in]	mcss		チェックするマルチセルワーク構造体
 *
 * @retval  FALSE:フェード終了　TRUE:フェード中
 */
//--------------------------------------------------------------------------
BOOL  MCSS_CheckExecutePaletteFade( MCSS_WORK*  mcss )
{ 
  return ( mcss->pal_fade_flag != 0 );
}

//--------------------------------------------------------------------------
/**
 * @brief テクスチャ転送開始アドレスセット
 *
 * @param[in]	mcss_sys  MCSSシステム管理構造体のポインタ
 * @param[in]	adrs      セットするアドレス
 */
//--------------------------------------------------------------------------
void  MCSS_SetTextureTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs )
{ 
	mcss_sys->texAdrs = adrs;
}

//--------------------------------------------------------------------------
/**
 * @brief テクスチャパレット転送開始アドレスセット
 *
 * @param[in]	mcss_sys  MCSSシステム管理構造体のポインタ
 * @param[in]	adrs      セットするアドレス
 */
//--------------------------------------------------------------------------
void  MCSS_SetTexPaletteTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs )
{ 
	mcss_sys->palAdrs = adrs;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメーションセット
 *
 * @param[in] mcss  MCSSワーク構造体のポインタ
 * @param[in] index セットするアニメーションインデックス
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnimeIndex( MCSS_WORK* mcss, int index )
{
	const NNSG2dMultiCellAnimSequence* pSequence;

	// 再生するシーケンスを取得
	pSequence = NNS_G2dGetAnimSequenceByIdx( mcss->mcss_nmar, index );
	GF_ASSERT( pSequence != NULL );

	// マルチセルアニメーションを構築
	if( mcss->mcss_mcanim_buf )
  { 
	  GFL_HEAP_FreeMemory( mcss->mcss_mcanim_buf );
  }
	MCSS_GetNewMultiCellAnimation( mcss, NNS_G2D_MCTYPE_SHARE_CELLANIM );

	// マルチセルアニメーションに再生するシーケンスをセット
	NNS_G2dSetAnimSequenceToMCAnimation( &mcss->mcss_mcanim, pSequence );

  //リセットフラグを落としておく
  mcss->req_reset_anime = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief モザイク
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  mcss     再読み込みするmcss
 * @param[in]  mosaic   モザイク加減
 */
//--------------------------------------------------------------------------
u8	MCSS_GetMosaic( MCSS_WORK* mcss )
{ 
  return mcss->mosaic;
}

//--------------------------------------------------------------------------
/**
 * @brief モザイク
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  mcss     再読み込みするmcss
 * @param[in]  mosaic   モザイク加減
 */
//--------------------------------------------------------------------------
void	MCSS_SetMosaic( MCSS_SYS_WORK *mcss_sys, MCSS_WORK* mcss, int mosaic )
{ 
  if( mcss->mosaic == mosaic )
  { 
    return;
  }
  if( mosaic > 15 )
  { 
    mosaic = 15;
  }
  mcss->mosaic = mosaic;
  MCSS_FreeResource( mcss );
  MCSS_LoadResource( mcss_sys, mcss->index, &mcss->maw );
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメレート
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
fx32	MCSS_GetMultiCellAnimeRate( MCSS_SYS_WORK *mcss_sys )
{ 
  return mcss_sys->mcAnimRate;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメレート
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_SetMultiCellAnimeRate( MCSS_SYS_WORK *mcss_sys, const fx32 rate )
{ 
  mcss_sys->mcAnimRate = rate;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメフレームセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  frame    セットするフレーム数
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnimeFrame( MCSS_WORK *mcss, const fx32 frame )
{ 
  mcss->mcss_anm_frame = frame;
}

//--------------------------------------------------------------------------
/**
 * @brief マルチセルアニメーションの設定されている数を取得
 *
 * @param[in] mcss  MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
u16 MCSS_GetAnimeNum( MCSS_WORK* mcss )
{ 
  return mcss->mcss_nmar->numSequences;
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェード用ベースカラー変更
 * パレットフェードに使用するベースカラーをカラー加減算を適応したパレットに変更
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 * @param[in]	evy	      セットするパラメータ（フェードさせる色に対する開始割合16段階）
 * @param[in]	rgb				セットするパラメータ（フェードさせる色）
 */
//--------------------------------------------------------------------------
void	MCSS_SetPaletteFadeBaseColor( MCSS_SYS_WORK* mcss_sys, MCSS_WORK* mcss, u8 evy, u32 rgb )
{ 
  TCB_LOADRESOURCE_WORK*	tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );

  tlw->mcss_sys = mcss_sys;
  tlw->tcb_flag = ( mcss_sys->tcb_sys != NULL );
  tlw->mcss = mcss;
  tlw->palette_p = &mcss->mcss_palette_proxy;
  tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * mcss->index;

  tlw->pPlttData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( NNSG2dPaletteData ) );

  tlw->pPlttData->fmt = mcss->mcss_palette_proxy.fmt;
  tlw->pPlttData->bExtendedPlt = FALSE;
  tlw->pPlttData->szByte = mcss->pltt_data_size;
  tlw->pPlttData->pRawData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), mcss->pltt_data_size );

  SoftFade( (void *)mcss->base_pltt_data, (void *)tlw->pPlttData->pRawData, mcss->pltt_data_size / 2, evy, rgb );
  MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->fade_pltt_data, mcss->pltt_data_size );

  if( mcss->tcb_load_base_palette )
  { 
    MCSS_FreeTCBLoadPalette( mcss->tcb_load_base_palette );
    mcss->tcb_load_base_palette = NULL;
  }

  if( mcss->pal_fade_flag == 0 )
  { 
    if( mcss_sys->tcb_sys ) 
    { 
	    mcss->tcb_load_base_palette = GFL_TCB_AddTask( mcss_sys->tcb_sys, TCB_LoadPalette, tlw, 0 );
    }
    else
    { 
	     mcss->tcb_load_base_palette = GFUser_VIntr_CreateTCB( TCB_LoadPalette, tlw, 0 );
    }
  }
  else
  { 
    GFL_HEAP_FreeMemory( tlw->pPlttData->pRawData );
    GFL_HEAP_FreeMemory( tlw->pPlttData );
    GFL_HEAP_FreeMemory( tlw );
  }
  mcss->fade_pltt_data_flag = 1;
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェード用ベースカラー変更解除
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void	MCSS_ResetPaletteFadeBaseColor( MCSS_SYS_WORK* mcss_sys, MCSS_WORK *mcss )
{ 
  TCB_LOADRESOURCE_WORK*	tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );

  tlw->mcss_sys = mcss_sys;
  tlw->tcb_flag = ( mcss_sys->tcb_sys != NULL );
  tlw->mcss = mcss;
  tlw->palette_p = &mcss->mcss_palette_proxy;
  tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * mcss->index;

  tlw->pPlttData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( NNSG2dPaletteData ) );

  tlw->pPlttData->fmt = mcss->mcss_palette_proxy.fmt;
  tlw->pPlttData->bExtendedPlt = FALSE;
  tlw->pPlttData->szByte = mcss->pltt_data_size;
  tlw->pPlttData->pRawData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), mcss->pltt_data_size );

  MI_CpuCopy16( mcss->base_pltt_data, tlw->pPlttData->pRawData, mcss->pltt_data_size );

  if( mcss->tcb_load_base_palette )
  { 
    MCSS_FreeTCBLoadPalette( mcss->tcb_load_base_palette );
    mcss->tcb_load_base_palette = NULL;
  }

  if( mcss_sys->tcb_sys ) 
  { 
    mcss->tcb_load_base_palette = GFL_TCB_AddTask( mcss_sys->tcb_sys, TCB_LoadPalette, tlw, 0 );
  }
  else
  { 
    mcss->tcb_load_base_palette = GFUser_VIntr_CreateTCB( TCB_LoadPalette, tlw, 0 );
  }
  mcss->fade_pltt_data_flag = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェード用ベースカラー変更フラグ取得
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
int   MCSS_GetFadePlttDataFlag( MCSS_WORK *mcss )
{ 
  return mcss->fade_pltt_data_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief アニメーションリスタート(アニメ終了コールバック中に呼ばないこと
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void   MCSS_RestartAnime( MCSS_WORK *mcss )
{ 
  NNS_G2dRestartMCAnimation( &mcss->mcss_mcanim );
}
//--------------------------------------------------------------------------
/**
 * @brief アニメーションリスタートリクエスト(アニメ終了コールバック内から呼ぶ場合はこっち
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 */
//--------------------------------------------------------------------------
void   MCSS_ReqRestartAnime( MCSS_WORK *mcss )
{ 
  mcss->req_reset_anime = 1;
}

//--------------------------------------------------------------------------
/**
 * @brief MCSSのセル描画を逆から描画フラグのセット
 *
 * @param[in] mcss      MCSSワーク構造体のポインタ
 * @param[in]	flag			セットするフラグ
 */
//--------------------------------------------------------------------------
void	MCSS_SetReverseDraw( MCSS_WORK* mcss, MCSS_REVERSE_DRAW flag )
{ 
  mcss->reverse_draw = flag;
}

//--------------------------------------------------------------------------
/**
 * @brief 透視射影時のZOffset値をセット
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 */
//--------------------------------------------------------------------------
void   MCSS_SetPerspectiveZOffset( MCSS_SYS_WORK *mcss_sys , fx32 z_offset )
{ 
  mcss_sys->perspective_z_offset = z_offset;
}

//--------------------------------------------------------------------------
/**
 * @brief 正射影FARの値を切り替え
 *
 * @param[in] mcss_sys  MCSSシステム管理構造体のポインタ
 * @param[in] flag      FALSE:1024  TRUE:512
 */
//--------------------------------------------------------------------------
void   MCSS_SetOrthoFarFlag( MCSS_SYS_WORK *mcss_sys , const BOOL flg )
{ 
  mcss_sys->ortho_far_flag = flg;
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * @brief リソースロード
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  count    登録場所指定
 * @param[in]  maw      マルチセル登録用パラメータ構造体のポインタ
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResource( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw )
{
	MCSS_WORK	*mcss = mcss_sys->mcss[ count ];
  HEAPID  heapID = ( maw->heap_low == TRUE ) ? GFL_HEAP_LOWID( mcss->heapID ) : mcss->heapID;

  if( ( mcss_sys->handle ) && ( mcss_sys->arcID == maw->arcID ) )
  { 
    MCSS_LoadResourceByHandle( mcss_sys, count, maw );
    return;
  }

	mcss->is_load_resource = 0;

	//プロキシ初期化
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// セルデータ、セルアニメーション、マルチセルデータ、
	// マルチセルアニメーションをロード。
	mcss->mcss_ncer_buf = GFL_ARC_UTIL_LoadCellBank(		maw->arcID, maw->ncer, FALSE, &mcss->mcss_ncer, heapID );
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );
#ifdef	POKEGRA_LZ
	mcss->mcss_nanr_buf = GFL_ARC_UTIL_LoadAnimeBank(		maw->arcID, maw->nanr, TRUE, &mcss->mcss_nanr, heapID );
#else	// POKEGRA_LZ
	mcss->mcss_nanr_buf = GFL_ARC_UTIL_LoadAnimeBank(		maw->arcID, maw->nanr, FALSE, &mcss->mcss_nanr, heapID );
#endif	// POKEGRA_LZ
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );
	mcss->mcss_nmcr_buf = GFL_ARC_UTIL_LoadMultiCellBank(	maw->arcID, maw->nmcr, FALSE, &mcss->mcss_nmcr, heapID );
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );
	mcss->mcss_nmar_buf = GFL_ARC_UTIL_LoadMultiAnimeBank(	maw->arcID, maw->nmar, FALSE, &mcss->mcss_nmar, heapID );
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// マルチセルアニメーションの実体を初期化します
	//
  MCSS_SetAnimeIndex( mcss, 0 );

	//1枚の板ポリで表示するための情報の読み込み（独自フォーマット）
	mcss->mcss_ncec = GFL_ARC_LoadDataAlloc( maw->arcID, maw->ncec, heapID );
  { 
	  //静止アニメーション時にパターンアニメするノードデータ（独自フォーマット）
    u32 size = MCSS_NCEC_HEADER_SIZE + sizeof( MCSS_NCEC ) * mcss->mcss_ncec->cells;
    mcss->mcss_ncen = ( MCSS_NCEN_WORK *)(mcss->mcss_ncec);
    mcss->mcss_ncen += ( size / sizeof( MCSS_NCEN_WORK ) );
  }

	//
	// VRAM 関連の初期化
	//
	{
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );
    tlw->mcss_sys = mcss_sys;
    tlw->tcb_flag = ( mcss_sys->tcb_sys != NULL );
		tlw->image_p = &mcss->mcss_image_proxy;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->chr_ofs = mcss_sys->texAdrs + MCSS_TEX_SIZE * count;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * count;
		tlw->mcss	 = mcss;
		// load character data for 3D (software sprite)
		{
#ifdef	POKEGRA_LZ
			tlw->pBufChar = GFL_ARC_UTIL_LoadBGCharacter( maw->arcID, maw->ncbr, TRUE, &tlw->pCharData,
                                                    GFL_HEAP_LOWID( mcss->heapID ) );
#else	// POKEGRA_LZ
			tlw->pBufChar = GFL_ARC_UTIL_LoadBGCharacter( maw->arcID, maw->ncbr, FALSE, &tlw->pCharData,
                                                    GFL_HEAP_LOWID( mcss->heapID ) );
#endif	// POKEGRA_LZ
			GF_ASSERT( tlw->pBufChar != NULL);
		}

		// load palette data
		{
			tlw->pBufPltt = GFL_ARC_UTIL_LoadPalette( maw->arcID, maw->nclr, &tlw->pPlttData, GFL_HEAP_LOWID( mcss->heapID ) );
			GF_ASSERT( tlw->pBufPltt != NULL);
			mcss->base_pltt_data = GFL_HEAP_AllocMemory( heapID, tlw->pPlttData->szByte );
			mcss->fade_pltt_data = GFL_HEAP_AllocMemory( heapID, tlw->pPlttData->szByte );
			mcss->pltt_data_size = tlw->pPlttData->szByte;
			MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->base_pltt_data, tlw->pPlttData->szByte );
		}

    if( mcss_sys->load_resource_callback )
    { 
      if( mcss_sys->load_resource_callback( maw, tlw, mcss_sys->callback_work ) == TRUE )
      { 
        mcss_sys->load_resource_callback = NULL;
        mcss_sys->callback_work = NULL;

      }
    }
    if( mcss->mosaic )
    { 
      MCSS_CalcMosaic( mcss, tlw );
    }
    if( mcss_sys->tcb_sys ) 
    { 
		  mcss->tcb_load_resource = GFL_TCB_AddTask( mcss_sys->tcb_sys, TCB_LoadResource, tlw, 0 );
    }
    else
    { 
		  mcss->tcb_load_resource = GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
    }
	}
}

//--------------------------------------------------------------------------
/**
 * @brief リソースロード（ハンドル版）
 *
 * @param[in]  mcss_sys MCSSシステム管理構造体のポインタ
 * @param[in]  count    登録場所指定
 * @param[in]  maw      マルチセル登録用パラメータ構造体のポインタ
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResourceByHandle( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw )
{
	MCSS_WORK*  mcss    = mcss_sys->mcss[ count ];
  ARCHANDLE*  handle  = mcss_sys->handle;

	mcss->is_load_resource = 0;

	//プロキシ初期化
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// セルデータ、セルアニメーション、マルチセルデータ、
	// マルチセルアニメーションをロード。
	mcss->mcss_ncer_buf = GFL_ARCHDL_UTIL_LoadCellBank( handle, maw->ncer, FALSE, &mcss->mcss_ncer, mcss->heapID );
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );
#ifdef	POKEGRA_LZ
	mcss->mcss_nanr_buf = GFL_ARCHDL_UTIL_LoadAnimeBank( handle, maw->nanr, TRUE, &mcss->mcss_nanr, mcss->heapID );
#else	// POKEGRA_LZ
	mcss->mcss_nanr_buf = GFL_ARCHDL_UTIL_LoadAnimeBank( handle, maw->nanr, FALSE, &mcss->mcss_nanr, mcss->heapID );
#endif	// POKEGRA_LZ
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );
	mcss->mcss_nmcr_buf = GFL_ARCHDL_UTIL_LoadMultiCellBank( handle, maw->nmcr, FALSE, &mcss->mcss_nmcr, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );
	mcss->mcss_nmar_buf = GFL_ARCHDL_UTIL_LoadMultiAnimeBank( handle, maw->nmar, FALSE, &mcss->mcss_nmar, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// マルチセルアニメーションの実体を初期化します
	//
  MCSS_SetAnimeIndex( mcss, 0 );

	//1枚の板ポリで表示するための情報の読み込み（独自フォーマット）
	mcss->mcss_ncec = GFL_ARC_LoadDataAllocByHandle( handle, maw->ncec, mcss->heapID );
  { 
	  //静止アニメーション時にパターンアニメするノードデータ（独自フォーマット）
    u32 size = MCSS_NCEC_HEADER_SIZE + sizeof( MCSS_NCEC ) * mcss->mcss_ncec->cells;
    mcss->mcss_ncen = ( MCSS_NCEN_WORK *)(mcss->mcss_ncec);
    mcss->mcss_ncen += ( size / sizeof( MCSS_NCEN_WORK ) );
  }

	//
	// VRAM 関連の初期化
	//
	{
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );
    tlw->mcss_sys = mcss_sys;
    tlw->tcb_flag = ( mcss_sys->tcb_sys != NULL );
		tlw->image_p = &mcss->mcss_image_proxy;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->chr_ofs = mcss_sys->texAdrs + MCSS_TEX_SIZE * count;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * count;
		tlw->mcss	 = mcss;
		// load character data for 3D (software sprite)
		{
#ifdef	POKEGRA_LZ
			tlw->pBufChar = GFL_ARCHDL_UTIL_LoadBGCharacter( handle, maw->ncbr, TRUE, &tlw->pCharData,
                                                       GFL_HEAP_LOWID( mcss->heapID ) );
#else	// POKEGRA_LZ
			tlw->pBufChar = GFL_ARCHDL_UTIL_LoadBGCharacter( handle, maw->ncbr, FALSE, &tlw->pCharData,
                                                       GFL_HEAP_LOWID( mcss->heapID ) );
#endif	// POKEGRA_LZ
			GF_ASSERT( tlw->pBufChar != NULL);
		}

		// load palette data
		{
			tlw->pBufPltt = GFL_ARCHDL_UTIL_LoadPalette( handle, maw->nclr, &tlw->pPlttData, GFL_HEAP_LOWID( mcss->heapID ) );
			GF_ASSERT( tlw->pBufPltt != NULL);
			mcss->base_pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->fade_pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->pltt_data_size = tlw->pPlttData->szByte;
			MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->base_pltt_data, tlw->pPlttData->szByte );
		}

    if( mcss_sys->load_resource_callback )
    { 
      if( mcss_sys->load_resource_callback( maw, tlw, mcss_sys->callback_work ) == TRUE )
      { 
        mcss_sys->load_resource_callback = NULL;
        mcss_sys->callback_work = NULL;

      }
    }
    if( mcss->mosaic )
    { 
      MCSS_CalcMosaic( mcss, tlw );
    }
    if( mcss_sys->tcb_sys ) 
    { 
		  mcss->tcb_load_resource = GFL_TCB_AddTask( mcss_sys->tcb_sys, TCB_LoadResource, tlw, 0 );
    }
    else
    { 
		  mcss->tcb_load_resource = GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
    }
	}
}

//--------------------------------------------------------------------------
/**
 * @brief リソースをVRAMに転送
 */
//--------------------------------------------------------------------------
static	void	TCB_LoadResource( GFL_TCB *tcb, void *work )
{
	TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)work;
#ifdef PM_DEBUG
    u16 before;
#endif

  if( tlw->tcb_flag == FALSE )
  { 
    u16 *v_count = (u16 *)REG_VCOUNT_ADDR;
    //VCountを確認してちらつきを防ぐ
    if( ( *v_count < MCSS_VCOUNT_BORDER_LOW ) ||
        ( *v_count > MCSS_VCOUNT_BORDER_HIGH ) )
    { 
      return;
    }
  }

#ifdef PM_DEBUG
  before = *(u16 *)REG_VCOUNT_ADDR;
#endif

	if( tlw->pBufChar )
  {
		// Loading For 3D Graphics Engine.（本来は、VRAMマネージャを使用したい）
		NNS_G2dLoadImage2DMapping(
			tlw->pCharData,
			tlw->chr_ofs,
			NNS_G2D_VRAM_TYPE_3DMAIN,
			tlw->image_p );
	
		GFL_HEAP_FreeMemory( tlw->pBufChar );
//    SOGABE_Printf("tex before:%d after:%d\n",before,*(u16 *)REG_VCOUNT_ADDR);
    if( tlw->mcss->mosaic == 0 )
    { 
	    tlw->pBufChar = NULL;
      return;
    }
	}

	if( tlw->mcss )
  {
    tlw->mcss->is_load_resource = 1;
	}

	if( tlw->pBufPltt )
  {
		// Loading For 3D Graphics Engine.
		NNS_G2dLoadPalette(
			tlw->pPlttData,
			tlw->pal_ofs,
			NNS_G2D_VRAM_TYPE_3DMAIN,
			tlw->palette_p );

		GFL_HEAP_FreeMemory( tlw->pBufPltt );
//    SOGABE_Printf("plt before:%d after:%d\n",before,*(u16 *)REG_VCOUNT_ADDR);
	}

	if( tlw->mcss )
  {
    tlw->mcss->tcb_load_resource = NULL;
  }

	if( tlw->mcss_sys )
  {
	  if( tlw->mcss_sys->tcb_load_shadow )
    { 
	    tlw->mcss_sys->tcb_load_shadow = NULL;
    }
  }

	GFL_HEAP_FreeMemory( work );
	GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------------------
/**
 * @brief パレットをVRAMに転送
 */
//--------------------------------------------------------------------------
static	void	TCB_LoadPalette( GFL_TCB *tcb, void *work )
{	
	TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)work;

  if( tlw->tcb_flag == FALSE )
  { 
    u16 *v_count = (u16 *)REG_VCOUNT_ADDR;
    //VCountを確認してちらつきを防ぐ
    if( ( *v_count < MCSS_VCOUNT_BORDER_LOW ) ||
        ( *v_count > MCSS_VCOUNT_BORDER_HIGH ) )
    { 
      return;
    }
  }

	NNS_G2dInitImagePaletteProxy( tlw->palette_p );

	// Loading For 3D Graphics Engine.
	NNS_G2dLoadPalette(
		tlw->pPlttData,
		tlw->pal_ofs,
		NNS_G2D_VRAM_TYPE_3DMAIN,
		tlw->palette_p );

	if( tlw->mcss )
  {
    if( tlw->mcss->tcb_load_palette == tcb )
    { 
      tlw->mcss->tcb_load_palette = NULL;
    }
    if( tlw->mcss->tcb_load_base_palette == tcb )
    { 
      tlw->mcss->tcb_load_base_palette = NULL;
    }
  }

	GFL_HEAP_FreeMemory( tlw->pPlttData->pRawData );
	GFL_HEAP_FreeMemory( tlw->pPlttData );
	GFL_HEAP_FreeMemory( work );
	GFL_TCB_DeleteTask( tcb );
}

/*---------------------------------------------------------------------------*
  Name:         MCSS_GetNewMultiCellAnimation

  Description:  新しい NNSG2dMultiCellAnimation を取得して初期化します。

  Arguments:    pAnimBank:          アニメーションデータバンク
                pCellDataBank:      セルデータバンク
                pMultiCellDataBank: マルチセルデータバンク
                mcType:             マルチセル実体の種類
                
  Returns:      プールから取得した初期化済みの NNSG2dMultiCellAnimation へ
                のポインタ
 *---------------------------------------------------------------------------*/
static	void	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType )
{
	const u32 szWork = NNS_G2dGetMCWorkAreaSize( mcss->mcss_nmcr, mcType );
	mcss->mcss_mcanim_buf = GFL_HEAP_AllocMemory( mcss->heapID, szWork );

	// 初期化
	NNS_G2dInitMCAnimationInstance( &mcss->mcss_mcanim, 
                                    mcss->mcss_mcanim_buf,
                                    mcss->mcss_nanr, 
                                    mcss->mcss_ncer, 
                                    mcss->mcss_nmcr, 
                                    mcType );
}

/*---------------------------------------------------------------------------*
  Name:         G2DDemo_MaterialSetup

  Description:  マテリアルの共通設定を行います。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static	void	MCSS_MaterialSetup(void)
{
	// for software sprite-setting
	{
		G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),		// diffuse
								GX_RGB(16, 16, 16),		// ambient
								TRUE					// use diffuse as vtx color if TRUE
								);

		G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),		// specular
								GX_RGB( 0,  0,  0),		// emission
                                FALSE					// use shininess table if TRUE
                                );
	}
}

//--------------------------------------------------------------------------
/**
 * @brief パレットフェードアニメ計算
 *
 * @param[in]	mcss	マルチセルシステム管理構造体
 * @param[in]	mcss	マルチセルワーク構造体
 * @param[in]	flag	パレットフェード実行制御フラグ
 */
//--------------------------------------------------------------------------
static	void	MCSS_CalcPaletteFade( MCSS_SYS_WORK* mcss_sys, MCSS_WORK *mcss )
{	
	if( mcss->pal_fade_wait == 0 )
	{	
		TCB_LOADRESOURCE_WORK*	tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );

    tlw->mcss_sys = mcss_sys;
    tlw->tcb_flag = ( mcss_sys->tcb_sys != NULL );
    tlw->mcss = mcss;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * mcss->index;

		tlw->pPlttData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( NNSG2dPaletteData ) );

		tlw->pPlttData->fmt = mcss->mcss_palette_proxy.fmt;
		tlw->pPlttData->bExtendedPlt = FALSE;
		tlw->pPlttData->szByte = mcss->pltt_data_size;
		tlw->pPlttData->pRawData = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( mcss->heapID ), mcss->pltt_data_size );

    if( mcss->fade_pltt_data_flag )
    { 
		  SoftFade( (void *)mcss->fade_pltt_data,
							  (void *)tlw->pPlttData->pRawData,
							  mcss->pltt_data_size / 2, 
							  mcss->pal_fade_start_evy,
							  mcss->pal_fade_rgb );
    }
    else
    { 
		  SoftFade( (void *)mcss->base_pltt_data,
							  (void *)tlw->pPlttData->pRawData,
							  mcss->pltt_data_size / 2, 
							  mcss->pal_fade_start_evy,
							  mcss->pal_fade_rgb );
    }

    if( mcss->tcb_load_palette )
    { 
      MCSS_FreeTCBLoadPalette( mcss->tcb_load_palette );
      mcss->tcb_load_palette = NULL;
    }

    if( mcss_sys->tcb_sys ) 
    { 
		  mcss->tcb_load_palette = GFL_TCB_AddTask( mcss_sys->tcb_sys, TCB_LoadPalette, tlw, 0 );
    }
    else
    { 
		  mcss->tcb_load_palette = GFUser_VIntr_CreateTCB( TCB_LoadPalette, tlw, 0 );
    }

	  if( mcss->pal_fade_start_evy == mcss->pal_fade_end_evy )
    { 
      mcss->pal_fade_flag = 0;
    }
    else
    { 
		  mcss->pal_fade_start_evy += mcss->pal_fade_value;
		  if( ( ( mcss->pal_fade_value >= 0 ) && ( mcss->pal_fade_start_evy >= mcss->pal_fade_end_evy ) ) ||
		      ( ( mcss->pal_fade_value < 0 ) && ( mcss->pal_fade_start_evy <= mcss->pal_fade_end_evy ) ) )
      { 
		    mcss->pal_fade_start_evy = mcss->pal_fade_end_evy;
		  }
    }
		mcss->pal_fade_wait = mcss->pal_fade_wait_tmp;
	}
	else
	{	
		mcss->pal_fade_wait--;
	}
}

//--------------------------------------------------------------------------
/**
 * @brief 確保していたリソースをフリー
 *
 * @param[in]	mcss	マルチセルワーク構造体
 */
//--------------------------------------------------------------------------
static  void  MCSS_FreeResource( MCSS_WORK* mcss )
{ 
	GF_ASSERT( mcss->mcss_ncer_buf   != NULL );
	GF_ASSERT( mcss->mcss_nanr_buf   != NULL );
	GF_ASSERT( mcss->mcss_nmcr_buf   != NULL );
	GF_ASSERT( mcss->mcss_nmar_buf   != NULL );
	GF_ASSERT( mcss->mcss_mcanim_buf != NULL );
	GF_ASSERT( mcss->mcss_ncec       != NULL );
	GF_ASSERT( mcss->base_pltt_data  != NULL );
	GF_ASSERT( mcss->fade_pltt_data  != NULL );

	GFL_HEAP_FreeMemory( mcss->mcss_ncer_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nanr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmcr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmar_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_mcanim_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_ncec );
	GFL_HEAP_FreeMemory( mcss->base_pltt_data );
	GFL_HEAP_FreeMemory( mcss->fade_pltt_data );

	mcss->mcss_ncer_buf   = NULL;
	mcss->mcss_nanr_buf   = NULL;
	mcss->mcss_nmcr_buf   = NULL;
	mcss->mcss_nmar_buf   = NULL;
	mcss->mcss_mcanim_buf = NULL;
	mcss->mcss_ncec       = NULL;
	mcss->base_pltt_data  = NULL;
	mcss->fade_pltt_data  = NULL;

  if( mcss->tcb_load_resource )
  { 
    MCSS_FreeTCBLoadResource( mcss->tcb_load_resource );
    mcss->tcb_load_resource = NULL;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief TCB_LoadResourceをフリー
 *
 * @param[in]	tcb	tcb構造体
 */
//--------------------------------------------------------------------------
static  void  MCSS_FreeTCBLoadResource( GFL_TCB* tcb )
{ 
  TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)GFL_TCB_GetWork( tcb );
  if( tlw->pBufChar )
  {
    GFL_HEAP_FreeMemory( tlw->pBufChar );
  }
  if( tlw->pBufPltt )
  {
    GFL_HEAP_FreeMemory( tlw->pBufPltt );
  }
  GFL_HEAP_FreeMemory( tlw );
  GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------------------
/**
 * @brief TCB_LoadPaletteをフリー
 *
 * @param[in]	tcb	tcb構造体
 */
//--------------------------------------------------------------------------
static  void  MCSS_FreeTCBLoadPalette( GFL_TCB* tcb )
{ 
  TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)GFL_TCB_GetWork( tcb );
  GFL_HEAP_FreeMemory( tlw->pPlttData->pRawData );
  GFL_HEAP_FreeMemory( tlw->pPlttData );
  GFL_HEAP_FreeMemory( tlw );
  GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------------------
/**
 * @brief モザイク処理
 *
 * @param[in]	mcss	マルチセルワーク構造体
 * @param[in]	tlw	  リソースデータ構造体
 */
//--------------------------------------------------------------------------
static  void  MCSS_CalcMosaic( MCSS_WORK* mcss, TCB_LOADRESOURCE_WORK* tlw )
{ 
  //現状16色パレットしか対応しません
  if( mcss->mcss_image_proxy.attr.fmt != GX_TEXFMT_PLTT16 )
  { 
    return;
  }
  { 
    int tex_x = pow( 2, ( mcss->mcss_image_proxy.attr.sizeS + 2 ) );
    int cells;
    u8* buf = tlw->pCharData->pRawData;

    for( cells = 0 ; cells < mcss->mcss_ncec->cells ; cells++ )
    { 
      int pos_x = mcss->mcss_ncec->ncec[ cells ].tex_s >> FX32_SHIFT;		//テクスチャs値
      int pos_y = mcss->mcss_ncec->ncec[ cells ].tex_t >> FX32_SHIFT;		//テクスチャt値
      int size_x = mcss->mcss_ncec->ncec[ cells ].size_x >> FX32_SHIFT;		//セルサイズX
      int size_y = mcss->mcss_ncec->ncec[ cells ].size_y >> FX32_SHIFT;		//セルサイズY
      int mosaic = mcss->mosaic + 1;
      int col;
      int pos;
      int ofs_x;
      int ofs_y;
      int cnt_y = 0;

      for( ofs_y = 0 ; ofs_y < size_y ; ofs_y += mosaic )
      { 
        int cnt_x = 0;
        for( ofs_x = 0 ; ofs_x < size_x ; ofs_x += mosaic )
        { 
          int put_size_x = ( ( ofs_x + mosaic ) > size_x ) ? ( size_x - mosaic * cnt_x ) : mosaic; 
          int put_size_y = ( ( ofs_y + mosaic ) > size_y ) ? ( size_y - mosaic * cnt_y ) : mosaic; 

          col = mosaic_col_get( buf, pos_x + ofs_x, pos_y + ofs_y, tex_x, put_size_x, put_size_y );
          mosaic_make( buf, pos_x + ofs_x, pos_y + ofs_y, tex_x, put_size_x, put_size_y, col );

          cnt_x++;
        }
        cnt_y++;
      }

      if( mcss->mcss_ncec->ncec[ cells ].mepachi_size_x )
      { 
        int pos_mx = mcss->mcss_ncec->ncec[ cells ].mepachi_tex_s >> FX32_SHIFT;		//テクスチャs値
        int pos_my = mcss->mcss_ncec->ncec[ cells ].mepachi_tex_t >> FX32_SHIFT;		//テクスチャt値
        int size_mx = mcss->mcss_ncec->ncec[ cells ].mepachi_size_x >> FX32_SHIFT;		//セルサイズX
        int size_my = ( mcss->mcss_ncec->ncec[ cells ].mepachi_size_y >> FX32_SHIFT ) * 2;		//セルサイズY
        int cnt_my = 0;

        for( ofs_y = 0 ; ofs_y < size_my ; ofs_y += mosaic )
        { 
          int cnt_mx = 0;
          for( ofs_x = 0 ; ofs_x < size_mx ; ofs_x += mosaic )
          { 
            int put_size_mx = ( ( ofs_x + mosaic ) > size_mx ) ? ( size_mx - mosaic * cnt_mx ) : mosaic; 
            int put_size_my = ( ( ofs_y + mosaic ) > size_my ) ? ( size_my - mosaic * cnt_my ) : mosaic; 

            col = mosaic_col_get( buf, pos_mx + ofs_x, pos_my + ofs_y, tex_x, put_size_mx, put_size_my );
            mosaic_make( buf, pos_mx + ofs_x, pos_my + ofs_y, tex_x, put_size_mx, put_size_my, col );

            cnt_mx++;
          }
          cnt_my++;
        }
      }
    }
  }
}

static  inline  void  mosaic_make( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y, int col )
{ 
  int x, y;
  int col_l = col;
  int col_h = col << 4;

  for( y = 0 ; y < size_y ; y++ )
  { 
    for( x = 0 ; x < size_x ; x++ )
    { 
      int put_x = pos_x + x;
      int put_y = pos_y + y;
      int pos = mosaic_pos_get( put_x, put_y, tex_x );

		  if( pos < MCSS_TEX_SIZE )
      { 
        if( put_x & 1 )
        { 
          buf[ pos ] = ( buf[ pos ] & 0x0f ) | col_h;
        }
        else
        { 
          buf[ pos ] = ( buf[ pos ] & 0xf0 ) | col_l;
        }
      }
    }
  }
}

static  inline  int  mosaic_pos_get( int pos_x, int pos_y, int tex_x )
{ 
  return pos_x / 2 + pos_y * tex_x;
}


static  inline  u8  mosaic_col_get( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y )
{ 
#if 1
#if 0
  int i;
  int get_x;
  int get_y;
  int pos;
  int col[ 16 ];
  int ret_col = 0;
  int col_cnt = 0;

  for( i = 0 ; i < 16 ; i++ )
  { 
    col[ i ] = 0;
  }

  for( get_y = 0 ; get_y < size_y ; get_y++ )
  { 
    for( get_x = 0 ; get_x < size_x ; get_x++ )
    { 
      pos = mosaic_pos_get( get_x + pos_x, get_y + pos_y, tex_x );
      i = ( get_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
      col[ i ]++;
    }
  }
  for( i = 1 ; i < 16 ; i++ )
  { 
    if( col_cnt < col[ i ] )
    { 
      col_cnt = col[ i ];
      ret_col = i;
    }
  }
  return ret_col;
#endif
  int i;
  int get_x;
  int get_y;
  int pos;
  int ret_col = 0;

  for( get_y = 0 ; get_y < size_y ; get_y++ )
  { 
    for( get_x = 0 ; get_x < size_x ; get_x++ )
    { 
      pos = mosaic_pos_get( get_x + pos_x, get_y + pos_y, tex_x );
      i = ( get_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
      if( i )
      { 
        ret_col = i;
        break;
      }
    }
    if( ret_col )
    { 
      break;
    }
  }
  return ret_col;
#else
  int pos = mosaic_pos_get( pos_x, pos_y, tex_x );
  return ( pos_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
#endif
}

//神王蟲からいただき
//----------------------------------------------------------------------------
/**
 *	@brief	4x4行列に座標を掛け合わせる
 *			Vecをx,y,z,1として計算し、計算後のVecとwを返します。
 *
 *	@param	*cp_src	Vector座標
 *	@param	*cp_m   4*4行列
 *	@param	*p_dst	Vecror計算結果
 *	@param	*p_w    4つ目の要素の値
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static	void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w )
{
	fx32 x = cp_src->x;
  fx32 y = cp_src->y;
  fx32 z = cp_src->z;
	fx32 w = FX32_ONE;

	GF_ASSERT( cp_src );
	GF_ASSERT( cp_m );
	GF_ASSERT( p_dst );
	GF_ASSERT( p_w );

  p_dst->x = (fx32)(((fx64)x * cp_m->_00 + (fx64)y * cp_m->_10 + (fx64)z * cp_m->_20) >> FX32_SHIFT);
  p_dst->x += cp_m->_30;	//	W=1なので足すだけ

  p_dst->y = (fx32)(((fx64)x * cp_m->_01 + (fx64)y * cp_m->_11 + (fx64)z * cp_m->_21) >> FX32_SHIFT);
  p_dst->y += cp_m->_31;//	W=1なので足すだけ

  p_dst->z = (fx32)(((fx64)x * cp_m->_02 + (fx64)y * cp_m->_12 + (fx64)z * cp_m->_22) >> FX32_SHIFT);
  p_dst->z += cp_m->_32;//	W=1なので足すだけ

	*p_w	= (fx32)(((fx64)x * cp_m->_03 + (fx64)y * cp_m->_13 + (fx64)z * cp_m->_23) >> FX32_SHIFT);
  *p_w	+= cp_m->_33;//	W=1なので足すだけ
}

#ifdef USE_RENDER
/*---------------------------------------------------------------------------*
  Name:         MCSS_InitRenderer

  Description:  Renderer と Surface を初期化します。

  Arguments:    pRenderer:  初期化する Renderer へのポインタ。
                pSurface:   初期化する Surface へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static	void	MCSS_InitRenderer( MCSS_SYS_WORK *mcss_sys )
{
	NNSG2dViewRect* pRect = &(mcss_sys->mcss_surface.viewRect);
 
	NNS_G2dInitRenderer( &mcss_sys->mcss_render );
	NNS_G2dInitRenderSurface( &mcss_sys->mcss_surface );

	pRect->posTopLeft.x = FX32_ONE * 0;
	pRect->posTopLeft.y = FX32_ONE * 0;
	pRect->sizeView.x = FX32_ONE * SCREEN_WIDTH;
	pRect->sizeView.y = FX32_ONE * SCREEN_HEIGHT;

	//OAM描画はしないので、コメントアウト
//	pSurface->pFuncOamRegister       = CallBackAddOam;
//	pSurface->pFuncOamAffineRegister = CallBackAddAffine;

	NNS_G2dAddRendererTargetSurface( &mcss_sys->mcss_render, &mcss_sys->mcss_surface );
	NNS_G2dSetRendererSpriteZoffset( &mcss_sys->mcss_render, DEFAULT_Z_OFFSET );

	mcss_sys->mcss_surface.type = NNS_G2D_SURFACETYPE_MAIN3D;
}
#endif //USE_RENDER

#ifdef PM_DEBUG
//--------------------------------------------------------------------------
/**
 * @brief マルチセル登録（デバッグ用）
 */
//--------------------------------------------------------------------------
MCSS_WORK*	MCSS_AddDebug( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_DEBUG_WORK *madw )
{
	int			count;

	for( count = 0 ; count < mcss_sys->mcss_max ; count++ ){
		if( mcss_sys->mcss[ count ] == NULL ){
			mcss_sys->mcss[ count ] = GFL_HEAP_AllocClearMemory( mcss_sys->heapID, sizeof(MCSS_WORK) );
			mcss_sys->mcss[ count ]->index = count;
			mcss_sys->mcss[ count ]->heapID = mcss_sys->heapID;
			mcss_sys->mcss[ count ]->pos.x = pos_x;
			mcss_sys->mcss[ count ]->pos.y = pos_y;
			mcss_sys->mcss[ count ]->pos.z = pos_z;
			mcss_sys->mcss[ count ]->scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->alpha = 31;
			mcss_sys->mcss[ count ]->shadow_alpha = MCSS_SHADOW_ALPHA_AUTO;
			mcss_sys->mcss[ count ]->vanish_flag = MCSS_VANISH_OFF;
			mcss_sys->mcss[ count ]->shadow_rotate_x = MCSS_DEFAULT_SHADOW_ROTATE_X;
			mcss_sys->mcss[ count ]->shadow_rotate_z = MCSS_DEFAULT_SHADOW_ROTATE_Z;
			mcss_sys->mcss[ count ]->shadow_offset.x = 0;
			mcss_sys->mcss[ count ]->shadow_offset.y = 0;
			mcss_sys->mcss[ count ]->shadow_offset.z = MCSS_DEFAULT_SHADOW_OFFSET;
			mcss_sys->mcss[ count ]->mcss_anm_frame = FX32_ONE;
			mcss_sys->mcss[ count ]->ortho_mode = 1;
			MCSS_LoadResourceDebug( mcss_sys, count, madw );
			break;
		}
	}
	//登録MAX値をオーバー
	GF_ASSERT( count < mcss_sys->mcss_max );

	return mcss_sys->mcss[ count ];
}

//--------------------------------------------------------------------------
/**
 * @brief リソースロード（デバッグ用）
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResourceDebug( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_DEBUG_WORK *madw )
{
	MCSS_WORK	*mcss = mcss_sys->mcss[ count ];

	mcss->is_load_resource = 0;

	//プロキシ初期化
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// セルデータ、セルアニメーション、マルチセルデータ、
	// マルチセルアニメーションをロード。
	NNS_G2dGetUnpackedCellBank( madw->ncer, &mcss->mcss_ncer );
	mcss->mcss_ncer_buf = madw->ncer;
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );

	NNS_G2dGetUnpackedAnimBank( madw->nanr, &mcss->mcss_nanr );
	mcss->mcss_nanr_buf = madw->nanr;
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );

	NNS_G2dGetUnpackedMultiCellBank( madw->nmcr, &mcss->mcss_nmcr );
	mcss->mcss_nmcr_buf =  madw->nmcr;
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );

	NNS_G2dGetUnpackedMCAnimBank( madw->nmar, &mcss->mcss_nmar );
	mcss->mcss_nmar_buf = madw->nmar;
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// マルチセルアニメーションの実体を初期化します
	//
  MCSS_SetAnimeIndex( mcss, 0 );

	//1枚の板ポリで表示するための情報の読み込み（独自フォーマット）
	mcss->mcss_ncec = madw->ncec;
  { 
	  //静止アニメーション時にパターンアニメするノードデータ（独自フォーマット）
    u32 size = MCSS_NCEC_HEADER_SIZE + sizeof( MCSS_NCEC ) * mcss->mcss_ncec->cells;
    mcss->mcss_ncen = ( MCSS_NCEN_WORK *)(mcss->mcss_ncec);
    mcss->mcss_ncen += ( size / sizeof( MCSS_NCEN_WORK ) );
  }

  //
  // VRAM 関連の初期化
  //
  {
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( mcss->heapID ), sizeof( TCB_LOADRESOURCE_WORK ) );
		tlw->mcss_sys	= mcss_sys;
		tlw->image_p = &mcss->mcss_image_proxy;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->chr_ofs = mcss_sys->texAdrs + MCSS_TEX_SIZE * count;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * count;
		tlw->mcss	 = mcss;
		// load character data for 3D (software sprite)
		{
			NNS_G2dGetUnpackedBGCharacterData( madw->ncbr, &tlw->pCharData );
			tlw->pBufChar = madw->ncbr;
			GF_ASSERT( tlw->pBufChar != NULL);
    }

		// load palette data
		{
			NNS_G2dGetUnpackedPaletteData( madw->nclr, &tlw->pPlttData );
			tlw->pBufPltt = madw->nclr;
			GF_ASSERT( tlw->pBufPltt != NULL);
			mcss->base_pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->fade_pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->pltt_data_size = tlw->pPlttData->szByte;
			MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->base_pltt_data, tlw->pPlttData->szByte );
    }
    mcss->tcb_load_resource = GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
  }
}

#endif
