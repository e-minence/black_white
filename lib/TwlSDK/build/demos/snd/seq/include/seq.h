/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SND - demos - seq
  File:     seq.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef SEQ_H_
#define SEQ_H_

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

/* クロックインターバル定数(μ秒) */
#define SEQ_CLOCK_INTERVAL_NITRO_VBLANK 16715   /* NITROのＶブランク間隔 */

/* エラーコード */
#define SEQ_ERROR_WRONG_HEADER         1        /* 不正なヘッダチャンク */
#define SEQ_ERROR_NOT_FORMAT0          2        /* フォーマット0 ではないSMF */
#define SEQ_ERROR_DIVISION_TIMECODE    3        /* SMFのディビジョンがタイムコード形式 */
#define SEQ_ERROR_WRONG_TRACK          4        /* 不正なトラックチャンク */
#define SEQ_ERROR_WRONG_DELTA_TIME     5        /* 不正なデルタタイムの可変長値 */

#ifdef __cplusplus
extern "C" {
#endif

/* サウンドハンドル */
typedef struct tSeqHandle
{
    u8      state;                     /* ステート */
    u8      running_status;            /* ランニングステータスバッファ */
    u8      next_delta_bytesize;       /* 次のイベントのデルタタイムのバイトサイズ */
    u8      padding1;
    u16     division;                  /* SMF分解能 */
    u16     padding2;
    u32     tempo;                     /* テンポ */
    u32     time_control;              /* 時間マネージャ */
    u32     time_per_callback;         /* １コールバックで進む時間(tick*μ秒/四分音符) */
    u32     chunk_size;                /* データチャンクのサイズ */
    u32     total_tick;                /* 先頭から数えた現在位置の総tick数 */
    u32     delta_time;                /* 次のイベントまでのデルタタイム */
    const u8 *current_ptr;             /* 現在位置のポインタ */
    const u8 *track_begin;             /* トラック開始位置のポインタ */
    const u8 *loop_begin;              /* ループ開始位置のポインタ */
}
SeqHandle;

void    SeqInit(u32 clock_interval, void (*callback) (const u8 *));
BOOL    SeqMain(SeqHandle * block);
BOOL    SeqPlay(SeqHandle * block, const u8 *smfdata);
void    SeqStop(SeqHandle * block);
void    SeqPause(SeqHandle * block);
u8      SeqGetErrorCode(void);

#ifdef __cplusplus
}
#endif

#endif // SEQ_H_
