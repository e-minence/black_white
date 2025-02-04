//======================================================================
/**
 * @file	  delivery_beacon.h
 * @brief	  ビーコン配信
 * @author	k.ohno
 * @data	  10/02/27
 */
//======================================================================

#pragma once
#include "net/delivery_base.h"


typedef struct _DELIVERY_BEACON_LOCALWORK DELIVERY_BEACON_WORK;


//--------------------------------------------------------------
/**
 * @brief   ビーコン配送初期化
 * @param   DELIVERY_BEACON_INIT 初期化構造体 ローカル変数でも大丈夫
 * @retval  DELIVERY_BEACON_WORK 管理ワーク
 */
//--------------------------------------------------------------
extern DELIVERY_BEACON_WORK* DELIVERY_BEACON_Init(DELIVERY_BEACON_INIT* pInit);

//--------------------------------------------------------------
/**
 * @brief   ビーコン配信開始
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_SendStart(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   ビーコン受信開始
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 * @retval  開始できたらTRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvStart(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   ビーコン受信完了かどうか
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 * @retval  受け取れた場合TRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvCheck(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   ビーコンが１つでもあるかどうか
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 * @retval  ある場合TRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvSingleCheck(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   ビーコン配送メイン
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 */
//--------------------------------------------------------------

extern void DELIVERY_BEACON_Main(DELIVERY_BEACON_WORK*  pWork);

//--------------------------------------------------------------
/**
 * @brief   ビーコン配送終了
 * @param   DELIVERY_BEACON_WORK 管理ワーク
 */
//--------------------------------------------------------------
extern void DELIVERY_BEACON_End(DELIVERY_BEACON_WORK*  pWork);
