#pragma once
#ifdef __cplusplus
extern "C" {
#endif
//============================================================================================
/**
 * @file	net_icondata.h
 * @brief	通信アイコンデータ部分の定義
            この二つの関数はgflibでは定義できないので 外部に作成すること
            skelのnet_icondata.cを参考にしてください
 * @author	ohno
 * @date	2007.05.29
 */
//============================================================================================


#ifndef __NET_ICONDATA__
#define __NET_ICONDATA__

//==============================================================================
/**
 * @brief    通信アイコングラフィックARCIDを得る
 * @param    none
 * @return   グラフィックARCID
 */
//==============================================================================

extern const u32 NET_ICONDATA_GetTableID(void);


//==============================================================================
/**
 * @brief    通信アイコングラフィックNOを得る
 * @param    pNoBuff  グラフィック番号を返す配列
 * @return   none
 */
//==============================================================================

extern void NET_ICONDATA_GetNoBuff(int* pNoBuff);


#endif //__NET_ICONDATA__
#ifdef __cplusplus
} /* extern "C" */
#endif
