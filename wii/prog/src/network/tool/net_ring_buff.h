//=============================================================================
/**
 * @file	net_ring_buff.h
 * @brief	リングバッファの仕組みを管理する関数
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __NET_RING_BUFF_H__
#define __NET_RING_BUFF_H__

#include "common_def.h"

/// @brief リングバッファ構造体
typedef struct{
    u8* pWork;                   ///< 保存領域の先頭ポインタ
    int size;                    ///< 保存領域サイズ
    s16 startPos;                ///< 読み出していないデータの位置
    volatile s16 backupEndPos;   ///< 最後の位置のバックアップ
} RingBuffWork;

//==============================================================================
/**
 * @brief   リングバッファ管理構造体初期化
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    リングバッファメモリー
 * @param   size         リングバッファメモリーサイズ
 * @return  none
 */
//==============================================================================
extern void GFL_NET_RingInitialize(RingBuffWork* pRing, u8* pDataArea, const int size);

//==============================================================================
/**
 * @brief   リングバッファに書き込む
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    書き込むデータ
 * @param   size         書き込みサイズ
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_RingPuts(RingBuffWork* pRing, u8* pDataArea, const int size);

//==============================================================================
/**
 * @brief   リングバッファからデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みサイズ
 * @param   worksize     読み込みバッファサイズ
 * @return  実際に読み込んだデータ
 */
//==============================================================================
extern int GFL_NET_RingGets(RingBuffWork* pRing, u8* pDataArea, const int size, const int worksize);

//==============================================================================
/**
 * @brief   リングバッファから1byteデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @return  1byteのデータ リングにデータがないときは0
 */
//==============================================================================
extern u8 GFL_NET_RingGetByte(RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   リングバッファから2byteデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @return  2byteのデータ リングにデータがないときは0
 */
//==============================================================================
extern u16 GFL_NET_RingGetShort(RingBuffWork* pRing);
//==============================================================================
/**
 * @brief   リングバッファに2byte書き込む
 * @param   pRing        リングバッファ管理ポインタ
 * @param   data        u16データ
 * @return  none
 */
//==============================================================================
extern void GFL_NET_RingPutShort(RingBuffWork* pRing, u16 data);
//==============================================================================
/**
 * @brief   リングバッファのデータ検査  読み込むだけで位置を進めない
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みバッファサイズ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
extern int GFL_NET_RingChecks(const RingBuffWork* pRing, u8* pDataArea, const int size, const int worksize);

//==============================================================================
/**
 * @brief   リングバッファのデータがいくつ入っているか得る
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  データサイズ
 */
//==============================================================================
extern int GFL_NET_RingDataSize(const RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   リングバッファのデータがどのくらいあまっているか検査
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
extern int GFL_NET_RingDataRestSize(const RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   カウンター場所をすりかえる
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_RingEndChange(RingBuffWork* pRing);

#endif// __NET_RING_BUFF_H__


#ifdef __cplusplus
} /* extern "C" */
#endif
