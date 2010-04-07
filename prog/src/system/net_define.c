//=============================================================================
/**
 * @file	net_define.c
 * @brief	通信に必要なIDパスワード等の定義
 * @author	GAME FREAK Inc.
 * @date    2008.07.15
 */
//=============================================================================

#include "gflib.h"
#include "net/network_define.h"

#define _BEACON_FIXHEAD "WB"   //< ビーコンはGGIDでは判別できるが、シリーズ通して同じになるので ビーコン内に種類を負荷する
#define _GGID (0x333)   ///< 製品の基本のGGID 他のゲームにつながるときは初期化構造体に別定義


//==============================================================================
/**
 * @brief   この関数はライブラリ外に作成する関数
 *          ビーコンのヘッダーを文字列で定義 sizeバイト分必要 (６バイト)
 * @param   pHeader  文字列を入れる関数
 * @param   size     サイズ
 */
//==============================================================================
void GFLR_NET_GetBeaconHeader(u8* pHeader, int size)
{
    GFL_STD_MemCopy(_BEACON_FIXHEAD, pHeader, size);
}

//==============================================================================
/**
 * @brief   この関数はライブラリ外に作成する関数
 *          GGID取得関数 （ベースになる定義）
 * @return  GGID
 */
//==============================================================================
u32 GFLR_NET_GetGGID(void)
{
    return _GGID;
}


