/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WBT - include
  File:     wbt.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_WBT_H__
#define	NITRO_WBT_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>
#include	<nitro/os.h>

/*---------------------------------------------------------------------------*
	型定義
 *---------------------------------------------------------------------------*/

/* WBT コールバック関数の形式 */
typedef void (*WBTCallback) (void *);


/*---------------------------------------------------------------------------*
	定数定義
 *---------------------------------------------------------------------------*/

/* WBT_RegisterBlock で登録可能なブロックの総数 */
#define WBT_NUM_MAX_BLOCK_INFO_ID 1000

/* WBT_RegisterBlock で登録可能なブロック ID の最小値 */
#define WBT_BLOCK_ID_MIN	WBT_NUM_MAX_BLOCK_INFO_ID

/*
 * WBT_InitParent で設定可能な, 親機 / 子機のパケットサイズ最小値.
 * この値以上かつ MP 通信に指定した最大値以下の範囲でサイズを設定すると,
 * WBT のプロトコルはそのサイズ内でブロック転送パケットを生成します.
 */
#define	WBT_PACKET_SIZE_MIN	14

/* WBT_GetBlockInfo で取得できるユーザ定義 ID 領域のサイズ */
#define WBT_USER_ID_LEN 32

/* WBT_PutUserData で指定できるユーザ定義データの最大サイズ */
#define WBT_SIZE_USER_DATA        9

/* WBT のエラーコード */
typedef s16 WBTResult;
#define WBT_RESULT_SUCCESS                       0
#define WBT_RESULT_ERROR_TIMEOUT                 1
#define WBT_RESULT_ERROR_UNKNOWN_USER_COMMAND    2
#define WBT_RESULT_ERROR_UNKNOWN_PACKET_COMMAND  3
#define WBT_RESULT_ERROR_PARSE_INVALID           4
#define WBT_RESULT_ERROR_RECV_BUFFER_OVERFLOW    5
#define WBT_RESULT_ERROR_SAVE_FAILURE            6

/* コールバックタイプ列挙型 */
typedef enum
{
    WBT_CMD_REQ_NONE = 0,
    WBT_CMD_REQ_WAIT,
    WBT_CMD_REQ_SYNC,
    WBT_CMD_RES_SYNC,
    WBT_CMD_REQ_GET_BLOCK,
    WBT_CMD_RES_GET_BLOCK,
    WBT_CMD_REQ_GET_BLOCKINFO,
    WBT_CMD_RES_GET_BLOCKINFO,
    WBT_CMD_REQ_GET_BLOCK_DONE,
    WBT_CMD_RES_GET_BLOCK_DONE,
    WBT_CMD_REQ_USER_DATA,
    WBT_CMD_RES_USER_DATA,
    WBT_CMD_SYSTEM_CALLBACK,
    WBT_CMD_PREPARE_SEND_DATA,
    WBT_CMD_REQ_ERROR,
    WBT_CMD_RES_ERROR,
    WBT_CMD_CANCEL
}
WBTCommandType;


/* パケットサイズから受信ビットマップサイズを計算するマクロ */
#define WBT_PACKET_BITMAP_SIZE(block_size, packet_size) \
  (((((block_size) + ((packet_size) - WBT_PACKET_SIZE_MIN) - 1)/((packet_size) - WBT_PACKET_SIZE_MIN)) + 31 )/sizeof(u32))  * sizeof(u32)


/* AID を指定するマクロ群 */
#define WBT_NUM_OF_AID         (WBT_AID_CHILD_LAST+1)
#define WBT_AID_PARENT         0
#define WBT_AID_CHILD_FIRST    1
#define WBT_AID_CHILD_LAST     (WBT_AID_CHILD_FIRST + 15 - 1)
#define WBT_AIDBIT_ALL         ((WBTAidBitmap)0xffff)
#define WBT_AIDBIT_PARENT      ((WBTAidBitmap)1)
#define WBT_AIDBIT_CHILD_ALL   ((WBTAidBitmap)0xfffe)
#define WBT_AIDBIT_CHILD_1     ((WBTAidBitmap)(1<< 1))
#define WBT_AIDBIT_CHILD_2     ((WBTAidBitmap)(1<< 2))
#define WBT_AIDBIT_CHILD_3     ((WBTAidBitmap)(1<< 3))
#define WBT_AIDBIT_CHILD_4     ((WBTAidBitmap)(1<< 4))
#define WBT_AIDBIT_CHILD_5     ((WBTAidBitmap)(1<< 5))
#define WBT_AIDBIT_CHILD_6     ((WBTAidBitmap)(1<< 6))
#define WBT_AIDBIT_CHILD_7     ((WBTAidBitmap)(1<< 7))
#define WBT_AIDBIT_CHILD_8     ((WBTAidBitmap)(1<< 8))
#define WBT_AIDBIT_CHILD_9     ((WBTAidBitmap)(1<< 9))
#define WBT_AIDBIT_CHILD_10     ((WBTAidBitmap)(1<< 10))
#define WBT_AIDBIT_CHILD_11     ((WBTAidBitmap)(1<< 11))
#define WBT_AIDBIT_CHILD_12     ((WBTAidBitmap)(1<< 12))
#define WBT_AIDBIT_CHILD_13     ((WBTAidBitmap)(1<< 13))
#define WBT_AIDBIT_CHILD_14     ((WBTAidBitmap)(1<< 14))
#define WBT_AIDBIT_CHILD_15     ((WBTAidBitmap)(1<< 15))




/*
 * for internal use {
 */
typedef u8 WBTPacketCommand;
typedef u8 WBTCommandCounter;
typedef u32 WBTBlockId;
typedef u16 WBTAidBitmap;
typedef s8 WBTPermission;
typedef s32 WBTBlockSeqNo;
typedef s16 WBTBlockNumEntry;
typedef s16 WBTPacketSize;
typedef s32 WBTBlockSize;
#define WBT_BLOCK_LIST_TYPE_COMMON 0
#define WBT_BLOCK_LIST_TYPE_USER   1
#define WBT_SIZE_COMMAND          sizeof(WBTPacketCommand)
#define WBT_SIZE_TARGET_BITMAP    sizeof(WBTAidBitmap)
#define WBT_SIZE_BLOCK_ID         sizeof(WBTBlockId)
#define WBT_SIZE_BLOCK_SEQ_NO     sizeof(WBTBlockseqNo)
#define WBT_SIZE_BLOCK_NUM_ENTRY  sizeof(WBTBlockNumEntry)
#define WBT_SIZE_PERMISSION       sizeof(WBTPermission)
/*
 * } for internal use
 */


/*---------------------------------------------------------------------------*
	構造体定義
 *---------------------------------------------------------------------------*/


typedef struct
{
    u32     id;
    s32     block_size;
    u8      user_id[WBT_USER_ID_LEN];
}
WBTBlockInfo;

typedef struct WBTBlockInfoList
{
    WBTBlockInfo data_info;
    struct WBTBlockInfoList *next;
    void   *data_ptr;
    WBTAidBitmap permission_bmp;
    u16     block_type;
}
WBTBlockInfoList;

typedef struct
{
    WBTBlockInfo *block_info[WBT_NUM_OF_AID];
}
WBTBlockInfoTable;

typedef struct
{
    u32    *packet_bitmap[WBT_NUM_OF_AID];
}
WBTPacketBitmapTable;

typedef struct
{
    u8     *recv_buf[WBT_NUM_OF_AID];
}
WBTRecvBufTable;

/* for command callback ******************************************************/

typedef struct
{
    WBTBlockNumEntry num_of_list;
    s16     peer_packet_size;
    s16     my_packet_size;
    u16     pad1;
    u32     padd2[2];
}
WBTRequestSyncCallback;

typedef struct
{
    u32     block_id;
}
WBTGetBlockDoneCallback;

typedef struct
{
    u32     block_id;
    s32     block_seq_no;
    void   *data_ptr;
    s16     own_packet_size;
    u16     padd;
}
WBTPrepareSendDataCallback;

typedef struct
{
    u8      data[WBT_SIZE_USER_DATA];
    u8      size;
    u8      padd[3];
}
WBTRecvUserDataCallback;

typedef struct
{
    u32     block_id;
    u32     recv_data_size;
    WBTRecvBufTable recv_buf_table;
    WBTPacketBitmapTable pkt_bmp_table;
}
WBTGetBlockCallback;


typedef struct
{
    WBTCommandType command;
    WBTCommandType event;
    u16     target_bmp;
    u16     peer_bmp;
    WBTCommandCounter my_cmd_counter;
    WBTCommandCounter peer_cmd_counter; /* デバッグ用 */
    WBTResult result;
    WBTCallback callback;
    union
    {
        WBTRequestSyncCallback sync;
        WBTGetBlockDoneCallback blockdone;
        WBTPrepareSendDataCallback prepare_send_data;
        WBTRecvUserDataCallback user_data;
        WBTGetBlockCallback get;
    };
}
WBTCommand;



/*---------------------------------------------------------------------------*
	関数定義
 *---------------------------------------------------------------------------*/


/* initialization ************************************************************/


/*---------------------------------------------------------------------------*
  Name:         WBT_InitParent

  Description:  initialize WBT-system as MP-parent state in WM.
                this must be called before any WBT function one time at least.
                when called two or more times, it is only ignored.

  Arguments:    send_packet_size  available MP-parent packet size for WBT.
                                  this must be below the value specified
                                  by WM_SetParentParameter(), and must be
                                  WBT_PACKET_SIZE_MIN at least.
                recv_packet_size  available MP-child packet size for WBT.
                                  this must be below the value specified
                                  by WM_SetParentParameter(), and must be
                                  WBT_PACKET_SIZE_MIN at least.
                callback          callback function which receives
                                  notifications from WBT-system.
                                  if specified NULL, any callback is ignored.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_InitParent(int send_packet_size, int recv_packet_size, WBTCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_InitChild

  Description:  initialize WBT-system as MP-child state in WM.
                this must be called before any WBT function one time at least.
                when called two or more times, it is only ignored.

  Arguments:    callback          callback function which receives
                                  notifications from WBT-system.
                                  if specified NULL, any callback is ignored.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_InitChild(WBTCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_End

  Description:  make WBT-system end.
                this must be called after WBT_InitParent or WBT_InitChild functions one time.
                when called two or more times, it is only ignored.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_End(void);


/* common ********************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WBT_AidbitmapToAid

  Description:  convert LSB of specified aid-bitmap to aid

  Arguments:    abmp              aid-bitmap to convert

  Returns:      converted aid.
                if argument is 0, return -1.
 *---------------------------------------------------------------------------*/
int     WBT_AidbitmapToAid(u16 abmp);

/*---------------------------------------------------------------------------*
  Name:         WBT_AidToAidbitmap

  Description:  convert specified aid to aid-bitmap

  Arguments:    aid              aid to convert

  Returns:      converted aid-bitmap.
 *---------------------------------------------------------------------------*/
static inline u16 WBT_AidToAidbitmap(int aid)
{
    return (u16)(1 << aid);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetOwnAid

  Description:  get own aid

  Arguments:    none.

  Returns:      own aid.
                if parent, result is always 0.
                if child, result is equal to recent WBT_SetOwnAid().
 *---------------------------------------------------------------------------*/
int     WBT_GetOwnAid(void);

/*---------------------------------------------------------------------------*
  Name:         WBT_CalcPacketbitmapSize

  Description:  calculate bitmap buffer size which is enough to receive
                specified block size. (WBT_GetBlock() needs)
                if child, this must be called after end of WBT_RequestSync().

  Arguments:    block_size       byte size of block to receive.

  Returns:      minimum size of bitmap buffer which can recieve
                specified block data.
 *---------------------------------------------------------------------------*/
int     WBT_CalcPacketbitmapSize(int block_size);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetCurrentDownloadProgress

  Description:  get the progress of present block reception

  Arguments:    block_id         block id to get progress
                aid              target aid
                current_count    destination pointer to store
                                 count of received packets
                total_count      destination pointer to store
                                 count of total packets

  Returns:      if under reception now, TRUE, else FALSE.
                if succeeded, current_count and total_count are set
                progress information.
 *---------------------------------------------------------------------------*/
BOOL    WBT_GetCurrentDownloadProgress(u32 block_id, int aid, int *current_count, int *total_count);

/*---------------------------------------------------------------------------*
  Name:         WBT_NumOfRegisteredBlock

  Description:  get the total of registered blocks.

  Arguments:    none.

  Returns:      total of blocks.
 *---------------------------------------------------------------------------*/
int     WBT_NumOfRegisteredBlock(void);

/*---------------------------------------------------------------------------*
  Name:         WBT_RegisterBlock

  Description:  register data-block to WBT-system so that other peers can
                enumerate it by WBT_GetBlockInfo() and
                recieve it by WBT_GetBlock().

  Arguments:    block_info_list  structure to hold registration information.
                                 if succeeded, this is under WBT-system
                                 until WBT_UnregisterBlock().
                block_id         user-defined unique block id which must be
                                 more than WBT_NUM_MAX_BLOCK_INFO_ID.
                user_id          optional user-defined information within WBT_USER_ID_LEN bytes.
                data_ptr         pointer to data buffer.
                                 if not NULL, this buffer is referred to at
                                 WBT-system until WBT_UnregisterBlock().
                                 if NULL, WBT_CMD_PREPARE_SEND_DATA callback
                                 occurs if needed.
                data_size        size of data_ptr
                permission_bmp   must be 0. (reserved)

  Returns:      if succeeded, TRUE. else, false.
                (multiple registration of same block-id will fail)
 *---------------------------------------------------------------------------*/
BOOL    WBT_RegisterBlock(WBTBlockInfoList *block_info_list, u32 block_id,
                          const void *user_id, const void *data_ptr, int data_size,
                          u16 permission_bmp);

/*---------------------------------------------------------------------------*
  Name:         WBT_UnregisterBlock

  Description:  unregister data-block from WBT-system.

  Arguments:    block_id         block-id which is already used
                                 for WBT_RegisterBlock().

  Returns:      if succeeded, pointer of structure which was registered.
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBT_UnregisterBlock(u32 block_id);

/*---------------------------------------------------------------------------*
  Name:         WBT_PrintBTList

  Description:  debug-print current internal status of WBT-system.

  Arguments:    None.

  Returns:      none.
 *---------------------------------------------------------------------------*/
void    WBT_PrintBTList(void);


/* parent settings ***********************************************************/

/*---------------------------------------------------------------------------*
  Name:         WBT_SetPacketSize

  Description:  change MP-packet size after WBT_InitParent().
                this function must be called in situation that
                any peer does not operating WBT_GetBlock() command.
				after this call, any child must call WBT_RequestSync()
                before calling WBT_GetBlock().

  Arguments:    send_packet_size  available MP-parent packet size for WBT.
                                  this must be below the value specified
                                  by WM_SetParentParameter(), and must be
                                  WBT_PACKET_SIZE_MIN at least.
                recv_packet_size  available MP-child packet size for WBT.
                                  this must be below the value specified
                                  by WM_SetParentParameter(), and must be
                                  WBT_PACKET_SIZE_MIN at least.

  Returns:      if suceeded, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    WBT_SetPacketSize(int send_packet_size, int recv_packet_size);

/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentSendHook

  Description:  set WBT protocol data for MP-parent packet.

  Arguments:    sendbuf           destination buffer which will be used
                                  by WM_SetMPDataToPort() as parent.
                send_size         allowed maximum size to send.

  Returns:      size of actual set data.
 *---------------------------------------------------------------------------*/
int     WBT_MpParentSendHook(void *sendbuf, int send_size);

/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentRecvHook

  Description:  notify received data to WBT-system.

  Arguments:    recv_buf          received buffer at WBT protocol port.
                recv_size         size of received buffer.
                aid               peer aid.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_MpParentRecvHook(const void *recv_buf, int recv_size, int aid);


/* child settings ************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WBT_SetOwnAid

  Description:  notify own aid to WBT-system.

  Arguments:    aid           own aid. (get by WM_StartConnect(), and so on)

  Returns:      none.
 *---------------------------------------------------------------------------*/
void    WBT_SetOwnAid(int aid);

/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildSendHook

  Description:  set WBT protocol data for MP-child packet.

  Arguments:    sendbuf           destination buffer which will be used
                                  by WM_SetMPDataToPort() as parent.
                send_size         allowed maximum size to send.

  Returns:      size of actual set data.
 *---------------------------------------------------------------------------*/
int     WBT_MpChildSendHook(void *sendbuf, int send_size);

/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildRecvHook

  Description:  notify received data to WBT-system.

  Arguments:    recv_buf          received buffer at WBT protocol port.
                recv_size         size of received buffer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_MpChildRecvHook(const void *recv_buf, int recv_size);


/* commands ******************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WBT_RequestSync

  Description:  synchronize to target peer.
                this function must be called before calling WBT_GetBlock().

  Arguments:    target            target peers as aid-bitmap.
                callback          callback on each completion.

  Returns:      if succeeded, TRUE.
                when previous command has been operating, it will fail.
 *---------------------------------------------------------------------------*/
BOOL    WBT_RequestSync(u16 target, WBTCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlockInfo

  Description:  get block information.

  Arguments:    target            target peers as aid-bitmap.
                index             index in registered block list.
                block_info_table  pointer of the table which holds pointer of
                                  each destination structure.
                callback          callback on each completion.

  Returns:      if succeeded, TRUE.
                when previous command has been operating, it will fail.
 *---------------------------------------------------------------------------*/
BOOL    WBT_GetBlockInfo(u16 target, int index,
                         WBTBlockInfoTable *block_info_table, WBTCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlock

  Description:  get data-block.
                
  Arguments:    target            target peers as aid-bitmap.
                block_id          block-id to get.
                recv_buf_table    pointer of the table which holds pointer of
                                  each destination structure.
                recv_size         size of data-block.
                p_bmp_table       pointer of the table which holds pointer of
                                  sequence bitmap buffer for internal tasks.
                callback          callback on each completion.

  Returns:      if succeeded, TRUE.
                when previous command has been operating, it will fail.
 *---------------------------------------------------------------------------*/
BOOL    WBT_GetBlock(u16 target, u32 block_id,
                     WBTRecvBufTable *recv_buf_table, u32 recv_size,
                     WBTPacketBitmapTable *p_bmp_table, WBTCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_PutUserData

  Description:  send free-formatted data.
                
  Arguments:    target            target peers as aid-bitmap.
                user_data         pointer of data buffer to send.
                size              size of data.
                                  this must be WBT_SIZE_USER_DATA at most.
                callback          callback on each completion.

  Returns:      if succeeded, TRUE.
                when previous command has been operating, it will fail.
 *---------------------------------------------------------------------------*/
BOOL    WBT_PutUserData(u16 target, const void *user_data, int size, WBTCallback callback);


/*---------------------------------------------------------------------------*
  Name:         WBT_CancelCurrentCommand

  Description:  cancel current WBT-commands
                
  Arguments:    target            target peers as aid-bitmap.

  Returns:      if no canceled command, FALSE.
                else TRUE.
 *---------------------------------------------------------------------------*/
BOOL    WBT_CancelCurrentCommand(u16 target);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* NITRO_WBT_H__ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
