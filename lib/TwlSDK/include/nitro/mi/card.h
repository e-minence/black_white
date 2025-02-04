/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MI - include
  File:     card.h

  Copyright 2004-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_CARD_H_
#define NITRO_MI_CARD_H_


#ifdef __cplusplus
extern "C" {
#endif

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
#ifdef __DUMMY________
#include <nitro/os.h>
#include <nitro/card.h>


/******************************************************************************
 * ＣＡＲＤアクセス
 *
 *	これらは Lock / Unlock を内部で行うだけの単純な CARD API ラッパーです.
 *	CARD API への移行に伴い, これらは廃止される予定です.
 *
 ******************************************************************************/


// MI-CARD 関数用の LockID を使用して CARD 確保 / 解放.
void    MIi_LockCard(void);
void    MIi_UnlockCard(void);

// カード ID の読み出し. (同期)
static inline u32 MI_ReadCardID(void)
{
    u32     ret;
    MIi_LockCard();
    ret = CARDi_ReadRomID();
    MIi_UnlockCard();
    return ret;
}

// カード読み込み. (同期)
static inline void MIi_ReadCard(u32 dmaNo, const void *src, void *dst, u32 size)
{
    MIi_LockCard();
    /*
     * 低水準の CARD_ReadRom で可能な限りパフォーマンスを保証するため,
     * 同期 / 非同期にかかわらず割り込みを使用するように変更.
     * ただしこの関数が割り込み禁止のまま使用されている個所があるので,
     * MI については同期版で無条件に CPU 転送を使用するよう変更.
     * (マルチスレッドでなければ効率は一緒)
     */
    (void)dmaNo;
    CARD_ReadRom((MI_DMA_MAX_NUM + 1), src, dst, size);
    MIi_UnlockCard();
}

// カードデータ読み込み. (非同期)
void    MIi_ReadCardAsync(u32 dmaNo, const void *srcp, void *dstp, u32 size,
                          MIDmaCallback callback, void *arg);

// カードデータ非同期読み込み完了の確認.
static inline BOOL MIi_TryWaitCard(void)
{
    return CARD_TryWaitRomAsync();
}

// カードデータ非同期読み込み終了待ち.
static inline void MIi_WaitCard(void)
{
    CARD_WaitRomAsync();
}

#endif
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz


#ifdef __cplusplus
} /* extern "C" */
#endif


/* NITRO_MI_CARD_H_ */
#endif
