//==============================================================================
/**
 * @file	connect_anm.h
 * @brief	Wifi接続画面のBGアニメのinline関数
 * @author	matsuda
 * @date	2007.12.26(水)
 *
 * Wifi接続BG画面が各アプリケーション個々で作成されているので、アニメーションさせる為の関数を
 * ここに全てinline関数として用意。(複数のオーバーレイにまたがっている為)
 */
//==============================================================================
#ifndef __CONNECT_ANM_H__
#define __CONNECT_ANM_H__

//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
#include "net_app/connect_anm_types.h"
#else                    //DL子機時処理
#include "../src/net_app/connect_anm_types.h"
#endif //MULTI_BOOT_MAKE

#include "system/gfl_use.h"
#include "system/palanm.h"

static inline void ConnectBGPalAnm_TblCreate(CONNECT_BG_PALANM *cbp);
static inline void ConnectBGPalAnm_IntrTCB(GFL_TCB* tcb, void *work);

//==============================================================================
//
//	inline関数
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   Wifi接続BGアニメ：初期化
 *
 * @param   cbp				Wifi接続BGのパレットアニメ制御構造体へのポインタ
 * @param   p_hdl			BGパレットが入っているアーカイブのハンドル
 * @param   pal_index		BGパレットのIndex
 * @param   heap_id			ヒープID(この場限りのテンポラリで使用)
 *
 * ワークの確保やTCBの生成等は行っていない為、終了関数はありません。
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_Init(CONNECT_BG_PALANM *cbp, ARCHANDLE* p_hdl, int pal_index, int heap_id)
{
	NNSG2dPaletteData *palData;
	void *p_work;
	
	GFL_STD_MemClear(cbp, sizeof(CONNECT_BG_PALANM));

  p_work = GFL_ARCHDL_UTIL_LoadPalette(p_hdl, pal_index, &palData, heap_id);
	
	//バッファにパレットデータをコピー
	GFL_STD_MemCopy(&((u16*)(palData->pRawData))[CBP_PAL_START_NUMBER * 16], 
		cbp->src_color, CBP_PAL_NUM * 0x20);
	GFL_STD_MemCopy(&((u16*)(palData->pRawData))[CBP_PAL_START_NUMBER * 16], 
		cbp->dest_color, CBP_PAL_NUM * 0x20);
	
	GFL_HEAP_FreeMemory(p_work);

	ConnectBGPalAnm_TblCreate(cbp);
	
	//有効フラグON
	cbp->occ = TRUE;
	//VIntroTCB生成
#ifdef PM_DEBUG
//	GF_ASSERT(DebugTCB_VintrFuncCheck(ConnectBGPalAnm_IntrTCB) == FALSE);
#endif
	cbp->tcb = GFUser_VIntr_CreateTCB(ConnectBGPalAnm_IntrTCB, cbp, 20);
}
static inline void ConnectBGPalAnm_InitBg(CONNECT_BG_PALANM *cbp, ARCHANDLE* p_hdl, int pal_index, int heap_id ,
                                          const u8 alpha_plane , const u8 base_plane )
{
  ConnectBGPalAnm_Init( cbp,p_hdl,pal_index,heap_id );
  if( base_plane < GFL_BG_FRAME0_S )
  {
    //メイン画面
    G2_SetBlendAlpha( 1<<alpha_plane , 1<<base_plane , 8,16 );
  }
  else
  {
    //サブ画面画面
    G2S_SetBlendAlpha( 1<<(alpha_plane-GFL_BG_FRAME0_S) , 1<<(base_plane-GFL_BG_FRAME0_S) , 12,16 );
  }
  cbp->scroll_plane = alpha_plane;
}


//--------------------------------------------------------------
/**
 * @brief   Wifi接続BGアニメ：メイン処理
 *
 * @param   cbp		Wifi接続BGのパレットアニメ制御構造体へのポインタ
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_End(CONNECT_BG_PALANM *cbp)
{
	if(cbp->tcb != NULL){
		GFL_TCB_DeleteTask(cbp->tcb);
		cbp->tcb = NULL;
		cbp->occ = FALSE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続BGアニメ：メイン処理
 *
 * @param   cbp		Wifi接続BGのパレットアニメ制御構造体へのポインタ
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_Main(CONNECT_BG_PALANM *cbp)
{
	;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続BGアニメ：有効、無効フラグをセット
 *
 * @param   cbp		Wifi接続BGのパレットアニメ制御構造体へのポインタ
 * @param   occ		TRUE:有効化。　FALSE：無効化
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_OccSet(CONNECT_BG_PALANM *cbp, BOOL occ)
{
	cbp->occ = occ;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi接続BGアニメ：メイン処理
 *
 * @param   cbp		Wifi接続BGのパレットアニメ制御構造体へのポインタ
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_TblCreate(CONNECT_BG_PALANM *cbp)
{
	int src_pos, dest_pos, next_color_pos, evy, s, next_break;
	
	dest_pos = 0;
	for(src_pos = 0; src_pos < CBP_PAL_NUM; src_pos++){
		next_color_pos = src_pos + 1;
		if( next_color_pos >= CBP_PAL_NUM )
		{
      next_color_pos -= CBP_PAL_NUM;
    }
		evy = 0;
		next_break = 0;
		do{
			GF_ASSERT(dest_pos < CBP_EVY_TBL_ALL);
			for(s = CBP_PAL_COLOR_START; s < CBP_PAL_COLOR_START + CBP_PAL_COLOR_NUM; s++){
				SoftFade(&cbp->src_color[src_pos][s], &cbp->dest_color[dest_pos][s],
					1, evy >> 8, cbp->src_color[next_color_pos][s]);
			}
			dest_pos++;
			if(next_break == TRUE){
				break;
			}
			evy += CBP_ADD_EVY;
			if(evy >= (16<<8)){
				evy = (16<<8);
				next_break = TRUE;
			}
		}while(1);
	}
	DC_FlushRange(cbp->dest_color, CBP_EVY_TBL_ALL * 16 * sizeof(u16));
}

//--------------------------------------------------------------
/**
 * @brief   VIntrでパレット転送
 *
 * @param   tcb		
 * @param   work		
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_IntrTCB(GFL_TCB* tcb, void *work)
{
	CONNECT_BG_PALANM *cbp = work;
	
	if(cbp->occ == FALSE){
		return;
	}

	cbp->intr_count ^= 1;
	if(cbp->intr_count & 1){
		return;
	}
	
	GX_LoadBGPltt((const void *)&cbp->dest_color[cbp->trans_pos][CBP_PAL_COLOR_START], 
		CBP_PAL_TRANS_NUMBER*0x20 + CBP_PAL_COLOR_START * sizeof(u16), 
		CBP_PAL_COLOR_NUM * sizeof(u16));
	GXS_LoadBGPltt((const void *)&cbp->dest_color[cbp->trans_pos][CBP_PAL_COLOR_START], 
		CBP_PAL_TRANS_NUMBER*0x20 + CBP_PAL_COLOR_START * sizeof(u16), 
		CBP_PAL_COLOR_NUM * sizeof(u16));

//	GX_LoadBGPltt((const void *)cbp->dest_color[cbp->trans_pos], 
//		CBP_PAL_START_NUMBER * 0x20, CBP_TRANS_PAL_NUM * 0x20);
//	GXS_LoadBGPltt((const void *)cbp->dest_color[cbp->trans_pos], 
//		CBP_PAL_START_NUMBER * 0x20, CBP_TRANS_PAL_NUM * 0x20);

	cbp->trans_pos++;
	if(cbp->trans_pos >= CBP_EVY_TBL_ALL)
	{
		cbp->trans_pos = 0;
	}
	
  GFL_BG_SetScroll( cbp->scroll_plane , GFL_BG_SCROLL_X_DEC , 7 );
}

#endif	//__CONNECT_ANM_H__
