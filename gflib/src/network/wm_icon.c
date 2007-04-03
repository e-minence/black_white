//=============================================================================
/**
 * @file	wm_icon.c
 * @brief	通信中に表示されるアイコン
 * @author	gamefreak
 * @date    2007.3.1
 */
//=============================================================================

#include "gflib.h"
#include "net.h"
#include "wm_icon.h"
#include "device/net_wireless.h"

// パレットファイルの大きさ
#define WM_ICON_PALFILE_SIZE	( 600 )
#define WM_ICON_CHARFILE_SIZE	( 600 )

// 通信アイコンだすために転送するPAL_VRAMの位置・大きさ(14番パレット使用）
#define WM_ICON_PAL_POS		( 14 )
#define WM_ICON_PAL_OFFSET	( 16 * 2 * WM_ICON_PAL_POS )
#define WM_ICON_PAL_SIZE	( 16 * 2 )


// 転送サイズ
#define WM_ICON_CHAR_SIZE    ( 4*4*32  )		

// アニメパターンの数
#define WM_ICON_ANIME_MAX	 ( 4 )




#define WM_ICON_MAIN_OAM_ADRS	(GXOamAttr*)(HW_OAM)
#define WM_ICON_SUB_OAM_ADRS	(GXOamAttr*)(HW_OAM_END)

// メイン画面・サブ画面の切り替えに合わせる動作モード
enum {
	MAIN_SUB_MODE_DEFAULT,	///< 何もしない（メインに出続ける）
	MAIN_SUB_MODE_TOP,		///< 上画面に出続ける
	MAIN_SUB_MODE_BOTTOM,	///< 下画面に出続ける
};

//----------------------------------------------
/**
 *	管理用構造体定義
 */
//----------------------------------------------
typedef struct _VINTR_WIRELESS_ICON		VINTR_WIRELESS_ICON;

struct _VINTR_WIRELESS_ICON {
	int		anime_seq;
	int     anime;
	u16     x,y;
    u8      bWifi;
    u8      main_sub_mode;
    u8      sub_wrote_flag;
	const 	VOamAnm *table;
	GXOamAttr*  prevOamPtr;
};



//==============================================================
// Prototype
//==============================================================
static void WirelessIconAnimeFunc( void *work );
static void trans_palette_data( int vramType, BOOL bWifi, u32 offset, HEAPID heapID );
static void trans_cgx_data( int vramType, BOOL bWifi, HEAPID heapID );
static int inline get_target_vram( VINTR_WIRELESS_ICON* vwi );
static int inline calc_anime_index( int targetVram, int anime_ptn );


//==============================================================================
/**
 * Vblank通信アイコンアニメ開始関数
 *
 * @param   objVRAM		
 * @param   HeapId		ヒープ
 *
 * @retval  VINTR_WIRELESS_ICON *		
 */
//==============================================================================
//==============================================================================
/**
 * $brief   Vblank通信アイコンアニメ開始関数
 *
 * @param   objVRAM		未使用（消します）
 * @param   HeapId		
 * @param   x			表示位置X
 * @param   y			表示位置Y
 * @param   bWifi		DS通信アイコンか？wifi通信アイコンか？
 * @param   tbl[]		アニメテーブル（消すかも）
 *
 * @retval  VINTR_WIRELESS_ICON *		
 */
//==============================================================================
static VINTR_WIRELESS_ICON *AddWirelessIconOAM(u32 objVRAM, u32 HeapId, int x, int y, BOOL bWifi, const VOamAnm *tbl)
{
	VINTR_WIRELESS_ICON *vwi;

	// パレット読み込みメイン14番に転送
	trans_palette_data( NNS_G2D_VRAM_TYPE_2DMAIN, bWifi, WM_ICON_PAL_OFFSET, HeapId );

	// VRAMの最後にCGXデータ転送
	trans_cgx_data( NNS_G2D_VRAM_TYPE_2DMAIN, bWifi, HeapId );

	// タスク登録・ワーク初期化
	vwi        = (VINTR_WIRELESS_ICON*)GFL_HEAP_AllocMemoryLo(HeapId, sizeof(VINTR_WIRELESS_ICON));

	vwi->x     = x;
	vwi->y     = y;
	vwi->anime_seq = 0;
    vwi->anime     = 3;		// 0:3本 1:2本 2:1本 0:切断 (の通信アイコン）
//	vwi->table     = tbl;
    vwi->bWifi     = bWifi;
    vwi->sub_wrote_flag = FALSE;
    vwi->main_sub_mode = MAIN_SUB_MODE_DEFAULT;
    vwi->prevOamPtr = (GXOamAttr *)HW_OAM;

	return vwi;
}


//------------------------------------------------------------------
/**
 * 通信アイコンTCBメイン【タスク】
 *
 * @param   tcb		
 * @param   work	
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void WirelessIconAnimeFunc( void *work )
{
	VINTR_WIRELESS_ICON *vwi = (VINTR_WIRELESS_ICON *)work;

	int targetVram, anime_index;
	GXOamAttr* pOam;

	targetVram = get_target_vram( vwi );
	anime_index = calc_anime_index( targetVram, vwi->anime );
	pOam = ( targetVram == NNS_G2D_VRAM_TYPE_2DMAIN )? WM_ICON_MAIN_OAM_ADRS : WM_ICON_SUB_OAM_ADRS;

	G2_SetOBJAttr(
//    	(GXOamAttr *)(HW_OAM + sizeof(GXOamAttr)*127),		
		pOam,
    	vwi->x,
    	vwi->y,
    	0,
    	GX_OAM_MODE_NORMAL,
    	0,
    	GX_OAM_EFFECT_NONE,
    	GX_OAM_SHAPE_16x16,
	    GX_OAM_COLORMODE_16,
	    anime_index,
	    WM_ICON_PAL_POS,
	    0
	);

	if( pOam != vwi->prevOamPtr )
	{
		G2_SetOBJAttr(
			vwi->prevOamPtr,
	    	0,
	    	0,
	    	0,
	    	GX_OAM_MODE_NORMAL,
	    	0,
	    	GX_OAM_EFFECT_NODISPLAY,
	    	GX_OAM_SHAPE_16x16,
		    GX_OAM_COLORMODE_16,
		    0,
		    0,
		    0
		);
		vwi->prevOamPtr = pOam;
	}

}
//------------------------------------------------------------------
/**
 * 現在のターゲットVRAMを取得
 *
 * @param   vwi		
 *
 * @retval  int 	NNS_G2D_VRAM_TYPE_2DMAIN or NNS_G2D_VRAM_TYPE_2DSUB
 */
//------------------------------------------------------------------
static int inline get_target_vram( VINTR_WIRELESS_ICON* vwi )
{
	switch( vwi->main_sub_mode ){
	case MAIN_SUB_MODE_TOP:
		return (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB )? NNS_G2D_VRAM_TYPE_2DMAIN : NNS_G2D_VRAM_TYPE_2DSUB;

	case MAIN_SUB_MODE_BOTTOM:
		return (GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB )? NNS_G2D_VRAM_TYPE_2DSUB : NNS_G2D_VRAM_TYPE_2DMAIN;

	default:
		return NNS_G2D_VRAM_TYPE_2DMAIN;
	}
}

//------------------------------------------------------------------
/**
 * アニメインデックス計算
 *
 * @param   targetVram		
 * @param   anime_ptn		
 *
 * @retval  int 			アニメインデックス
 */
//------------------------------------------------------------------
static int inline calc_anime_index( int targetVram, int anime_ptn )
{
	int vramMode, bank;

	if( targetVram == NNS_G2D_VRAM_TYPE_2DMAIN )
	{
		vramMode = GX_GetOBJVRamModeChar();
		bank = GX_GetBankForOBJ();
	}
	else
	{
		vramMode = GXS_GetOBJVRamModeChar();
		bank = GX_GetBankForSubOBJ();
	}

	switch( vramMode ){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		if( (bank == GX_VRAM_OBJ_16_G) || (bank == GX_VRAM_OBJ_16_F) )
		{
			return 512-16+4*anime_ptn;
		}
		else
		{
			return 1024-16+4*anime_ptn;
		}
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		if( (bank == GX_VRAM_OBJ_80_EF) || (bank == GX_VRAM_OBJ_80_EG) )
		{
			return 640-4+1*anime_ptn;
		}
		else
		{
			return 1024-4+1*anime_ptn;
		}
	default:
		return 1024-8+2*anime_ptn;
	}
}
//------------------------------------------------------------------
/**
 * 後が残らないように、アイコンが消えた状態のOAMを直書きしておく
 *
 * @param   oam		書き込み先アドレス
 *
 */
//------------------------------------------------------------------
static inline void clear_oam( GXOamAttr* oam )
{
	G2_SetOBJAttr(
		oam,
		0,
		0,
		0,
		GX_OAM_MODE_NORMAL,
		0,
		GX_OAM_EFFECT_NODISPLAY,
		GX_OAM_SHAPE_16x16,
		GX_OAM_COLORMODE_16,
		0,
		0,
		0
	);
}



//==============================================================================
/**
 * 通信アイコンアニメチェンジ
 *
 * @param   vwi		
 * @param   anime		
 *
 * @retval  none		
 */
//==============================================================================
static void WirelessIconAnimeChange(VINTR_WIRELESS_ICON *vwi, int anime)
{
	// アニメチェンジ
	if(anime<WM_ICON_ANIME_MAX)
	{
		vwi->anime = anime;
	}
}

//==============================================================================
/**
 * Vblank通信アイコン表示処理終了
 *
 * @param   vwi		
 *
 * @retval  none		
 */
//==============================================================================
static void WirelessIconEnd(VINTR_WIRELESS_ICON *vwi)
{
	clear_oam( WM_ICON_MAIN_OAM_ADRS );
	if( vwi->sub_wrote_flag )
	{
		clear_oam( WM_ICON_SUB_OAM_ADRS );
	}
	// ワーク解放
	GFL_HEAP_FreeMemory( vwi );
}


//==============================================================================
/**
 * 通信アイコンの表示LCDを上画面 or 下画面に固定する
 *
 * @param   vwi			アイコンワークポインタ
 * @param   bTop		TRUEだと上画面に固定／FALSEだと下画面に固定
 * @param   heapID		テンポラリ使用ヒープID
 *
 */
//==============================================================================
static void WirelessIconHoldLCD( VINTR_WIRELESS_ICON *vwi, BOOL bTop, HEAPID heapID )
{
	trans_palette_data( NNS_G2D_VRAM_TYPE_2DSUB, vwi->bWifi, WM_ICON_PAL_OFFSET, heapID );
	trans_cgx_data( NNS_G2D_VRAM_TYPE_2DSUB, vwi->bWifi, heapID );
	vwi->main_sub_mode = (bTop)? MAIN_SUB_MODE_TOP : MAIN_SUB_MODE_BOTTOM;
	vwi->sub_wrote_flag = TRUE;
}


//==============================================================================
/**
 * 通信アイコン位置の表示LCD固定をやめてデフォルトの挙動に戻す
 *
 * @param   vwi			アイコンワークポインタ
 *
 */
//==============================================================================
static void WirelessIconDefaultLCD( VINTR_WIRELESS_ICON* vwi )
{
	if( vwi->main_sub_mode != MAIN_SUB_MODE_DEFAULT )
	{
		vwi->main_sub_mode = MAIN_SUB_MODE_DEFAULT;
		vwi->sub_wrote_flag = FALSE;
		clear_oam( WM_ICON_SUB_OAM_ADRS );
	}
}






//======================================================================================================
//======================================================================================================

//------------------------------------------------------------------
/**
 * パレットデータ転送
 *
 * @param   vramType	VRAMタイプ（MAIN or SUB）
 * @param   bWifi		TRUEならWi-Fi通信アイコン／FALSEならDS通信アイコン
 * @param   offset		転送位置バイトオフセット
 * @param   HeapId		テンポラリヒープID
 */
//------------------------------------------------------------------
static void trans_palette_data( int vramType, BOOL bWifi, u32 offset, HEAPID heapID )
{
	void* pal = GFL_HEAP_AllocMemoryLo( heapID, WM_ICON_PALFILE_SIZE );
	if( pal )
	{
		NNSG2dPaletteData* palData;

		GF_ReadFile( "src/gfl_graphic/wm.nclr", pal );
		DC_FlushRange( pal, WM_ICON_PALFILE_SIZE );
		NNS_G2dGetUnpackedPaletteData( pal, &palData );    // 展開

		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			GX_LoadOBJPltt( palData->pRawData, offset, WM_ICON_PAL_SIZE );
		}
		else
		{
			GXS_LoadOBJPltt( palData->pRawData, offset, WM_ICON_PAL_SIZE );
		}

		GFL_HEAP_FreeMemory( pal );
	}
}


//------------------------------------------------------------------
/**
 * アイコンCGXデータ転送
 *
 * @param   vramType	VRAMタイプ（MAIN or SUB）
 * @param   bWifi		TRUEならWi-Fi通信アイコン／FALSEならDS通信アイコン
 * @param   HeapId		テンポラリヒープID
 */
//------------------------------------------------------------------
static void trans_cgx_data( int vramType, BOOL bWifi, HEAPID heapID )
{
	void *ncg = GFL_HEAP_AllocMemoryLo( heapID, WM_ICON_CHARFILE_SIZE );

	if( ncg )
	{
		NNSG2dCharacterData *ppCharData;
		int offset, vramMode, objBank;

		// DS通信アイコンか、wifiアイコンを読み込む
	    if(bWifi)
	    {
            GF_ReadFile( "src/gfl_graphic/wifi.ncgr", ncg );
	    }
	    else
	    {
            GF_ReadFile( "src/gfl_graphic/wm.ncgr", ncg );
	    }
		DC_FlushRange( ncg, WM_ICON_CHARFILE_SIZE );
		NNS_G2dGetUnpackedCharacterData( ncg, &ppCharData );   // 展開

		// VRAM設定に合わせて転送位置を決定
		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			vramMode = GX_GetOBJVRamModeChar();
			objBank = GX_GetBankForOBJ();
		}
		else
		{
			vramMode = GX_GetOBJVRamModeChar();
			objBank = GX_GetBankForOBJ();
		}
		switch( vramMode ){
		case GX_OBJVRAMMODE_CHAR_1D_32K:
			if( objBank==GX_VRAM_OBJ_16_G || objBank==GX_VRAM_OBJ_16_F ) 
			{
				offset = WM_ICON_CHAR_OFFSET16;
			}
			else
			{
				offset = WM_ICON_CHAR_OFFSET32;
			}
			break;
		case GX_OBJVRAMMODE_CHAR_1D_128K:
			if( objBank==GX_VRAM_OBJ_80_EF || objBank==GX_VRAM_OBJ_80_EG ) 
			{
				offset = WM_ICON_CHAR_OFFSET80;
			}
			else
			{
				offset = WM_ICON_CHAR_OFFSET128;
			}
			break;
		default:
			offset = WM_ICON_CHAR_OFFSET64;
			break;
		}

		// OBJキャラをVRAMへ転送
		if( vramType == NNS_G2D_VRAM_TYPE_2DMAIN )
		{
			GX_LoadOBJ( ppCharData->pRawData, offset, WM_ICON_CHAR_SIZE );
		}
		else
		{
			GXS_LoadOBJ( ppCharData->pRawData, offset, WM_ICON_CHAR_SIZE );
		}

        GFL_HEAP_FreeMemory( ncg );
	}
}



//アイコンの位置
static const VOamAnm WM_IconAnimTbl[]={
	{512-16   },
	{512-16+4 },
	{512-16+8 },
    {512-16+12},
};
static VINTR_WIRELESS_ICON *VintrWirelessIconPtr = NULL;


//==============================================================================
/**
 * $brief   超絶らくちん通信アイコン
 *
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasy(BOOL bWifi, HEAPID heapID)
{
    WirelessIconEasyXY(240, 0, bWifi, heapID);
}

//==============================================================================
/**
 * $brief   超絶らくちん通信アイコン 引数あり版
 *
 * @param   mode		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasyXY(int x,int y, BOOL bWifi,HEAPID heapID)
{
    if(!GFL_NET_WLIsVRAMDStart()){  // イクニューモンが無い場合通信してない
        return ;
    }
    if(VintrWirelessIconPtr){
        WirelessIconEasyEnd();
    }
    VintrWirelessIconPtr = AddWirelessIconOAM(0,heapID, x, y, bWifi, WM_IconAnimTbl);
}


//==============================================================================
/**
 * $brief   超絶らくちん通信アイコン削除
 *
 * @param   none		
 *
 * @retval  none		
 */
//==============================================================================
void WirelessIconEasyEnd(void)
{
    if(VintrWirelessIconPtr){
        WirelessIconEnd(VintrWirelessIconPtr);
        VintrWirelessIconPtr = NULL;
    }
}

//==============================================================================
/**
 * $brief   通信レベルをセットする
 * @param   level
 * @retval  none
 */
//==============================================================================
void WirelessIconEasy_SetLevel(int level)
{
    if(VintrWirelessIconPtr){
        WirelessIconAnimeChange(VintrWirelessIconPtr, level);
    }
}

//------------------------------------------------------------------
/**
 * 超絶らくちん通信アイコン：表示LCDを上画面or下画面に固定する
 *
 * @param   bTop		TRUEだと上画面／FALSEだと下画面
 * @param   heapID		テンポラリ使用ヒープID
 *
 */
//------------------------------------------------------------------
void WirelessIconEasy_HoldLCD( BOOL bTop, HEAPID heapID )
{
    if(VintrWirelessIconPtr)
    {
		WirelessIconHoldLCD(VintrWirelessIconPtr, bTop, heapID );
	}
}

//------------------------------------------------------------------
/**
 * 超絶らくちん通信アイコン：表示LCDの固定を止め、デフォルトの挙動に戻す
 */
//------------------------------------------------------------------
void WirelessIconEasy_DefaultLCD( void )
{
    if(VintrWirelessIconPtr){
		WirelessIconDefaultLCD( VintrWirelessIconPtr );
	}
}


//==============================================================================
/**
 * $brief   通信アイコン実行関数
 * @retval  none
 */
//==============================================================================
void WirelessIconEasyFunc(void)
{
    if(VintrWirelessIconPtr){
        WirelessIconAnimeFunc( VintrWirelessIconPtr );
    }
}

