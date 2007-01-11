//=============================================================================
/**
 * @file	net_ring_buff.c
 * @brief	リングバッファの仕組みを管理する関数
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================

#include "gflib.h"
#include "net.h"
#include "net_ring_buff.h"



static int _ringPos(const RingBuffWork* pRing,const int i);

//==============================================================================
/**
 * @brief   リングバッファ管理構造体初期化
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    リングバッファメモリー
 * @param   size         リングバッファメモリーサイズ
 * @return  none
 */
//==============================================================================
void GFL_NET_RingInitialize(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    pRing->pWork = pDataArea;
    pRing->size = (s16)size;
    pRing->startPos = 0;
    pRing->endPos = 0;
    pRing->backupEndPos = 0;
//    pRing->backupStartPos = 0;
}

//==============================================================================
/**
 * @brief   リングバッファに書き込む
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    書き込むデータ
 * @param   size         書き込みサイズ
 * @retval  none
 */
//==============================================================================
void GFL_NET_RingPuts(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i,j;

    // ここのASSERTに引っかかるということは
    // 処理が遅延しているのに、通信は毎syncくるので
    // ストックするバッファ分まであふれると、ここで止まります。
    // 通信のバッファを増やすか、今の部分の処理を分断するか、対処が必要です。
    // 最終的にはこのエラーがあると通信を切断します。
    if(GFL_NET_RingDataRestSize(pRing) <= size){
#ifdef DEBUG_ONLY_FOR_ohno
        NET_PRINT("%d %d line %d \n",GFL_NET_RingDataRestSize(pRing),size);
        OS_TPanic(0,"GFL_NET_RingOVER %d %d",GFL_NET_RingDataRestSize(pRing),size);
#endif
        return;
    }
    j = 0;
    for(i = pRing->backupEndPos; i < pRing->backupEndPos + size; i++,j++){
        GF_ASSERT(pDataArea);
        pRing->pWork[_ringPos( pRing, i )] = pDataArea[j];
    }
    pRing->backupEndPos = (s16)_ringPos( pRing, i );
}

//==============================================================================
/**
 * @brief   リングバッファからデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みバッファサイズ
 * @return  実際に読み込んだデータ
 */
//==============================================================================
int GFL_NET_RingGets(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i;

    i = GFL_NET_RingChecks(pRing, pDataArea, size);
    pRing->startPos = (s16)_ringPos( pRing, pRing->startPos + i);

    return i;
}

//==============================================================================
/**
 * @brief   リングバッファから1byteデータを得る
 * @param   pRing        リングバッファ管理ポインタ
 * @return  1byteのデータ リングにデータがないときは0
 */
//==============================================================================
u8 GFL_NET_RingGetByte(RingBuffWork* pRing)
{
    u8 byte;

    if(1==GFL_NET_RingGets(pRing, &byte, 1)){
        return byte;
    }
    return 0;
}

//==============================================================================
/**
 * @brief   リングバッファのデータ検査  読み込むだけで位置を進めない
 * @param   pRing        リングバッファ管理ポインタ
 * @param   pDataArea    読み込みバッファ
 * @param   size         読み込みバッファサイズ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
int GFL_NET_RingChecks(const RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i,j;

    j = 0;
    for(i = pRing->startPos; i < pRing->startPos + size; i++,j++){
        if(pRing->endPos == _ringPos( pRing,i )){
            return j;
        }
        pDataArea[j] = pRing->pWork[_ringPos( pRing,i )];
    }
    return j;
}

//==============================================================================
/**
 * @brief   リングバッファのデータがいくつ入っているか得る
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  データサイズ
 */
//==============================================================================
int GFL_NET_RingDataSize(const RingBuffWork* pRing)
{
    if(pRing->startPos > pRing->endPos){
        return (pRing->size + pRing->endPos - pRing->startPos);
    }
    return (pRing->endPos - pRing->startPos);
}

//==============================================================================
/**
 * @brief   リングバッファのデータがどのくらいあまっているか検査
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  実際に読み込んだデータ
 */
//==============================================================================
int GFL_NET_RingDataRestSize(const RingBuffWork* pRing)
{
    return (pRing->size - GFL_NET_RingDataSize(pRing));
}

//==============================================================================
/**
 * @brief   ringサイズの場所を得る
 * @param   pRing       リングバッファ管理ポインタ
 * @param   i           バイト位置
 * @return  補正したバイト位置
 */
//==============================================================================
static int _ringPos(const RingBuffWork* pRing, const int i)
{
    return i % pRing->size;
}

//==============================================================================
/**
 * @brief   カウンター場所をすりかえる
 * @param   pRing        リングバッファ管理ポインタ
 * @retval  none
 */
//==============================================================================
void GFL_NET_RingEndChange(RingBuffWork* pRing)
{
    pRing->endPos = pRing->backupEndPos;
}

