//==============================================================================
/**
 * @file	gf_overlay.h
 * @brief	オーバーレイ制御のヘッダ
 * @author	GAME FREAK inc.
 * @date	2006.03.06
 */
//==============================================================================
#ifndef __GF_OVERLAY_H__
#define __GF_OVERLAY_H__



//==============================================================================
//	外部関数宣言
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	オーバーレイ制御システムの初期化
 * @param	heapID		使用するヒープの指定
 * @param	main_num	通常オーバーレイの最大数
 * @param	itcm_num	ITCMオーバーレイの最大数
 * @param	dtcm_num	DTCMオーバーレイの最大数
 */
//--------------------------------------------------------------
extern void GFL_OVERLAY_SysInit(HEAPID heapID, int main_num, int itcm_num, int dtcm_num);

//--------------------------------------------------------------
/**
 * @brief	オーバーレイ制御システムの終了
 */
//--------------------------------------------------------------
extern void GFL_OVERLAY_SysExit(void);

//--------------------------------------------------------------
/**
 * @brief   指定したオーバーレイをロードする
 *
 * @param   id				ロードするオーバーレイID
 *
 * @retval  TRUE:成功
 * @retval  FALSE:失敗
 *
 * 複数のオーバーレイを実行している時、領域が被った場合はオーバーレイは失敗します。
 * (デバッグ中はGF_ASSERTで停止します)
 *
 * ロードはFS_SetDefaultDMA(or FS_Init)で設定されている方法で行います。
 * 使用するDMA,CPUを変更したい場合は、この関数実行前にFS_SetDefaultDMAで設定を行ってください。
 * ITCM,DTCMへのロードは内部で一時的にCPUに変更してロードを行います。(ロード後は元の設定に戻します)
 */
//--------------------------------------------------------------
extern BOOL GFL_OVERLAY_Load(const FSOverlayID id);

//--------------------------------------------------------------
/**
 * @brief   指定したオーバーレイIDのオーバーレイをアンロードします
 *
 * @param   id		オーバーレイID
 *
 */
//--------------------------------------------------------------
extern void GFL_OVERLAY_Unload(const FSOverlayID id);




#endif	//__GF_OVERLAY_H__


