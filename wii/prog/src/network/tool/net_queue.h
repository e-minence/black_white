//=============================================================================
/**
 * @file	net_queue.h
 * @brief	（送信）キューの仕組みを管理する関数
 * @author	katsumi ohno
 * @date	06/01/29
 */
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NET_QUEUE_H__
#define __NET_QUEUE_H__

#include "net_ring_buff.h"

/// @brief キュー構造体定義
typedef struct _SEND_QUEUE SEND_QUEUE;

/// @brief キュー構造体
struct _SEND_QUEUE{
    u8* pQData;     ///< データアドレス
    SEND_QUEUE* prev;      ///< 手前のキュー
    SEND_QUEUE* next;      ///< 次のキュー
    u16 size;     ///< サイズ
    u16 command;   ///< コマンド
    u8 recvBit;    ///< 受け取る人
    u8 bNext;  ///< ひきつづき次のフレームで送信をする場合TRUE
  u8 dummy;
  u8 dummy2;
} ;

///  @brief  サイズを必ず含むようになります  最大５バイトヘッダー
#define _GFL_NET_QUEUE_HEADERBYTE (5)
// |--- commnand 2 --|--size 2--| recvBIT --| |

#define _DATA_QUEUE_MAX (0xffff)


/// @brief 送るデータの管理
typedef struct{
    u8* pData;      ///<  データ
    u16 size;       ///<  サイズ
    u16 dummy;
} SEND_BUFF_DATA;


/// @brief 送信キュー管理 最初と最後
typedef struct{
    SEND_QUEUE* pTop;     ///< 送信キューの初め
    SEND_QUEUE* pLast;    ///< 送信キューの最後
} SEND_TERMINATOR;


/// @brief 送信キュー管理
typedef struct{
    SEND_TERMINATOR fast;     ///< すぐ送る送信キュー
    SEND_TERMINATOR stock;    ///< 後で送ればいいキュー
    SEND_QUEUE* pNow;   ///< 今送っている最中のキュー
    RingBuffWork* pSendRing;  ///< リングバッファワークポインタ
    void* heapTop;   ///< キューHEAP
    int max;         ///< キューの数
    int incNo;   // 送信時のインクリメント番号
} SEND_QUEUE_MANAGER;


//==============================================================================
/**
 * @brief   キューに何か入っているかどうかを確認する
 * @param   pQueueMgr キューマネージャーのポインタ
 * @retval  TRUE   空
 * @retval  FALSE  入っている
 */
//==============================================================================
extern BOOL GFL_NET_QueueIsEmpty(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   キューMANAGERの初期化
 * @param   pQueueMgr キューマネージャーのポインタ
 * @param   queueMax  キュー数
 * @param   pSendRing 実データを保存する場合のリングバッファワーク
 * @param   heapid    メモリー確保ID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerInitialize(SEND_QUEUE_MANAGER* pQueueMgr, int queueMax, RingBuffWork* pSendRing,HEAPID heapid);

//==============================================================================
/**
 * @brief   キューMANAGERのリセット
 * @param   pQueueMgr キューマネージャーのポインタ
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerReset(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   キューMANAGERの終了
 * @param   pQueueMgr キューマネージャーのポインタ
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerFinalize(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   キューにデータを入れる
 * @param   pQueueMgr キューマネージャーのポインタ
 * @param   command   送信コマンド
 * @param   pDataArea  送信データ
 * @param   size    サイズ
 * @param   bFast  優先度が高いデータ?
 * @param   bSave  保存するかどうか
 * @param   recvBit      受け取る人
 * @retval  TRUE 蓄えた
 * @retval  FALSE キューに入らなかった
 */
//==============================================================================
extern BOOL GFL_NET_QueuePut(SEND_QUEUE_MANAGER* pQueueMgr,int command, u8* pDataArea, int size, BOOL bFast, BOOL bSave,int recvBit);

//==============================================================================
/**
 * @brief   指定バイト分バッファに入れる
 * @param   pQueueMgr  キューマネージャーのポインタ
 * @param   pSendBuff  送信バッファ構造体のポインタ
 * @param   bNextPlus   データ末尾にヘッダーだけを含めるかどうか
 * @retval  TRUE  続きデータがない場合
 * @retval  FALSE データが連続している場合
 */
//==============================================================================
extern BOOL GFL_NET_QueueGetData(SEND_QUEUE_MANAGER* pQueueMgr, SEND_BUFF_DATA *pSendBuff);

//==============================================================================
/**
 * @brief   キューが存在するかどうか
 * @param   pQueueMgr  キューマネージャーのポインタ
 * @param   command    調べるコマンド
 * @retval  TRUE ある
 * @retval  FALSE ない
 */
//==============================================================================
extern BOOL GFL_NET_QueueIsCommand(SEND_QUEUE_MANAGER* pQueueMgr, int command);

//==============================================================================
/**
 * @brief   キューの数を得る
 * @param   pQueueMgr キューマネージャーのポインタ
 * @return  使用しているキューの数
 */
//==============================================================================
extern int GFL_NET_QueueGetNowNum(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   送信回数をインクリメント
 * @param   pQueueMgr  キューマネージャーのポインタ
 * @param   inc        送信回数を入れるポインタ
 */
//==============================================================================

extern void GFL_NET_QueueInc(SEND_QUEUE_MANAGER* pQueueMgr, u8* inc);

#ifdef PM_DEBUG
extern void GFL_NET_QueueDebugTest(void);
#endif

#endif// __COMM_QUEUE_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
