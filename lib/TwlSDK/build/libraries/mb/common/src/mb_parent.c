/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - libraries
  File:     mb_parent.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-11#$
  $Rev: 10742 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// ----------------------------------------------------------------------------
// definition

#define MB_SEND_THRESHOLD                       2       // sendのとき2blockさきまで送る

// ----------------------------------------------------------------------------
// variable

/* 最新の MP で子機から転送要求を受けたファイルIDを示すビットセット */
static int any_recv_bitmap = 0;

// ----------------------------------------------------------------------------
// static funciton's prototypes

// --- for parent
static void MBi_CommChangeParentState(u16 child, int state, void *arg);
static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg);
static void MBi_CommParentRecvDataPerChild(void *arg, u16 child);
static void MBi_CommParentRecvData(void *arg);
static int MBi_CommParentSendMsg(u8 type, u16 pollbmp);
static int MBi_CommParentSendDLFileInfo(void);
static int MBi_CommParentSendBlock(void);
static int MBi_CommParentSendData(void);
static void MBi_calc_sendblock(u8 file_id);
static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req);

// --- miscellany
static inline u16 max(u16 a, u16 b);
static BOOL IsChildAidValid(u16 child_aid);
static void MBi_CommCallParentError(u16 aid, u16 errcode);

/*  ============================================================================

    親機用関数

    ============================================================================*/

/*---------------------------------------------------------------------------*
  Name:         MB_CommSetParentStateCallback

  Description:  親機イベントコールバックの設定

  Arguments:    callback       指定するコールバック関数.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void MB_CommSetParentStateCallback(MBCommPStateCallback callback)
{
    OSIntrMode enabled;

    SDK_ASSERT(pPwork != 0);

    enabled = OS_DisableInterrupts();  /* 割り込み禁止 */

    pPwork->parent_callback = callback;

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetParentState

  Description:  各子機に対する親機状態の取得

  Arguments:    child_aid(WMのAID表現形式に合わせて1-15で扱う)

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommGetParentState(u16 child_aid)
{

    if (pPwork && IsChildAidValid(child_aid))
    {
        return pPwork->p_comm_state[child_aid - 1];
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildUser

  Description:  接続している子機のユーザー情報取得

  Arguments:    child(WMのAID表現形式に合わせて1-15で扱う)

  Returns:      MbUser structure
 *---------------------------------------------------------------------------*/

const MBUserInfo *MB_CommGetChildUser(u16 child_aid)
{
    OSIntrMode enabled = OS_DisableInterrupts();        /* 割り込み禁止 */

    if (pPwork && IsChildAidValid(child_aid))
    {
        MI_CpuCopy8(&pPwork->childUser[child_aid - 1], &pPwork->childUserBuf, sizeof(MBUserInfo));
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return &pPwork->childUserBuf;
    }
    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildrenNumber

  Description:  接続している子機の人数取得

  Arguments:    

  Returns:      number of children
 *---------------------------------------------------------------------------*/

u8 MB_CommGetChildrenNumber(void)
{
    if (pPwork)
    {
        return pPwork->child_num;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetGameEntryBitmap

  Description:  配信中のGameRegistryに対してエントリーしている
                AIDビットマップを取得する。

  Arguments:    game_req      - 対象となるGameRegistryへのポインタ

  Returns:      指定したGameRegistryにエントリーしているAIDビットマップ
                (親機AID:0、子機AID:1-15)
                ゲームが配信中ではない場合、返り値は0となる。
                （ゲームが配信中の場合、必ず親機AID:0がエントリーメンバーに
                  含まれている。）
 *---------------------------------------------------------------------------*/

u16 MB_GetGameEntryBitmap(const MBGameRegistry *game_reg)
{
    int     i;
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if ((pPwork->fileinfo[i].active) && ((u32)pPwork->fileinfo[i].game_reg == (u32)game_reg))
        {
            return pPwork->fileinfo[i].gameinfo_child_bmp;
        }
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommIsBootable

  Description:  ブート可能かを判定

  Arguments:    child_aid - 対象子機のAID

  Returns:      yes - TRUE  no - FALSE
 *---------------------------------------------------------------------------*/

BOOL MB_CommIsBootable(u16 child_aid)
{
    SDK_ASSERT(pPwork != NULL);

    if (pPwork && IsChildAidValid(child_aid))
    {
        /* 送信完了した子機の場合、TRUEを返す。 */
        if (pPwork->p_comm_state[child_aid - 1] == MB_COMM_PSTATE_SEND_COMPLETE)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommResponseRequest

  Description:  子機からの接続リクエストに対する応答を指定

  Arguments:    None.

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

BOOL MB_CommResponseRequest(u16 child_aid, MBCommResponseRequestType ack)
{
    u16     req;
    int     state;
    OSIntrMode enabled;

    SDK_ASSERT(pPwork != NULL);

    enabled = OS_DisableInterrupts();  /* 割り込み禁止 */

    switch (ack)
    {
    case MB_COMM_RESPONSE_REQUEST_KICK:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_KICK;
        break;
    case MB_COMM_RESPONSE_REQUEST_ACCEPT:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_ACCEPT;
        break;

    case MB_COMM_RESPONSE_REQUEST_DOWNLOAD:
        state = MB_COMM_PSTATE_WAIT_TO_SEND;
        req = MB_COMM_USER_REQ_SEND_START;
        break;

    case MB_COMM_RESPONSE_REQUEST_BOOT:
        state = MB_COMM_PSTATE_SEND_COMPLETE;
        req = MB_COMM_USER_REQ_BOOT;
        break;
    default:
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return FALSE;
    }

    if (pPwork && IsChildAidValid(child_aid))
    {
        if (pPwork->p_comm_state[child_aid - 1] == state)
        {
            pPwork->req2child[child_aid - 1] = req;
            (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
            return TRUE;
        }
    }

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeParentState

  Description:  親機状態の変更

  Arguments:    child(WMのAID表現形式に合わせて1-15で扱う。0番は親を表す), 
                state

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeParentState(u16 child, int state, void *arg)
{
    SDK_ASSERT(pPwork && child >= 0 && child <= WM_NUM_MAX_CHILD);

    /* childの番号が指定されているなら、その子機に対する状態を変更。 */
    if (IsChildAidValid(child))
    {
        pPwork->p_comm_state[child - 1] = state;
    }

    MBi_CommChangeParentStateCallbackOnly(child, state, arg);

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeParentStateCallbackOnly

  Description:  子機状態通知をコールバック呼び出しのみで行う
                内部の状態は変更せず

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg)
{
    if (pPwork->parent_callback)       // 状態遷移コールバック
    {
        (*pPwork->parent_callback) (child, (u32)state, arg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentCallback

  Description:  親機コールバック本体

  Arguments:    type:WM_TYPE event arg:callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

void MBi_CommParentCallback(u16 type, void *arg)
{
    MB_COMM_WMEVENT_OUTPUT(type, arg);

    switch (type)
    {
    case MB_CALLBACK_INIT_COMPLETE:
        /* 初期化完了 */
        /* Callback引数に、WMStartParentCallback型のargを渡す。 */
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_INIT_COMPLETE, arg);
        break;

    case MB_CALLBACK_END_COMPLETE:
        /* 終了完了 */
        // 関数の最後で実行

        break;

    case MB_CALLBACK_CHILD_CONNECTED:
        {
            WMstartParentCallback *sparg = (WMstartParentCallback *)arg;

            /* AIDが0(親機)の場合は無視 */
            if (sparg->aid == 0)
                break;

            if (sparg->aid >= 16)
            {
                OS_TWarning("Connected Illegal AID No. ---> %2d\n", sparg->aid);
                break;
            }

            MB_DEBUG_OUTPUT("child %d connected to get bootimage!\n", sparg->aid);

            /* Callback引数に、WMStartParentCallback型のargを渡す。 */
            MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_CONNECTED, arg);
        }
        break;

    case MB_CALLBACK_CHILD_DISCONNECTED:
        {
            WMstartParentCallback *sparg = (WMstartParentCallback *)arg;

            /* AIDが0(親機)の場合は無視 */
            if (sparg->aid == 0)
                break;

            if (sparg->aid >= 16)
            {
                OS_TWarning("Disconnected Illegal AID No. ---> %2d\n", sparg->aid);
                break;
            }

            MB_DEBUG_OUTPUT("child %d disconnected \n", sparg->aid);

            /*  子機情報を消去
               (切断が起こった場合、そのAIDに関する子機情報は消去する) */
            pPwork->childversion[sparg->aid - 1] = 0;
            MI_CpuClear8(&pPwork->childggid[sparg->aid - 1], sizeof(u32));
            MI_CpuClear8(&pPwork->childUser[sparg->aid - 1], sizeof(MBUserInfo));

            /* 受信バッファをクリア */
            MBi_ClearParentPieceBuffer(sparg->aid);

            pPwork->req2child[sparg->aid - 1] = MB_COMM_USER_REQ_NONE;

            /* 要求FileIDがセットされていた場合。IDを-1にクリアする。 */
            if (pPwork->fileid_of_child[sparg->aid - 1] != -1)
            {
                u8      fileID = (u8)pPwork->fileid_of_child[sparg->aid - 1];
                u16     nowChildFlag = pPwork->fileinfo[fileID].gameinfo_child_bmp;
                u16     child = sparg->aid;

                pPwork->fileinfo[fileID].gameinfo_child_bmp &= ~(MB_GAMEINFO_CHILD_FLAG(child));
                pPwork->fileinfo[fileID].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
                pPwork->fileid_of_child[child - 1] = -1;
                pPwork->fileinfo[fileID].pollbmp &= ~(0x0001 << (child));

                MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
            }

            /* 接続情報をクリア */
            if (pPwork->child_entry_bmp & (0x0001 << (sparg->aid)))
            {
                pPwork->child_num--;
                pPwork->child_entry_bmp &= ~(0x0001 << (sparg->aid));
            }

            /* MB_COMM_PSTATE_BOOT_REQUESTの時に、Disconnectされた場合、
               ブートによる通信終了とみなし、MB_COMM_PSTATE_BOOT_STARTABLE状態を通知 */
            if (pPwork->p_comm_state[sparg->aid - 1] == MB_COMM_PSTATE_BOOT_REQUEST)
            {
                MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_BOOT_STARTABLE, NULL);
            }

            /* Callback引数に、WMStartParentCallback型のargを返す。 */
            MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_DISCONNECTED, arg);
            pPwork->p_comm_state[sparg->aid - 1] = MB_COMM_PSTATE_NONE;
        }
        break;

    case MB_CALLBACK_MP_PARENT_RECV:
        MBi_CommParentRecvData(arg);
        break;

    case MB_CALLBACK_MP_SEND_ENABLE:
        (void)MBi_CommParentSendData();
        break;

    case MB_CALLBACK_BEACON_SENT:
        {
            u8      i;
            /* 各ゲームに対して、GameInfoメンバー情報を更新 */
            for (i = 0; i < MB_MAX_FILE; i++)
            {
                /* 登録ファイルがアクティブで、
                   ゲームのエントリーメンバーの変更フラグ立っていた場合に
                   GameInfoのメンバー情報を更新する. */
                if (pPwork->fileinfo[i].active && pPwork->fileinfo[i].gameinfo_changed_bmp)
                {
                    MB_UpdateGameInfoMember(&pPwork->fileinfo[i].game_info,
                                            &pPwork->childUser[0],
                                            pPwork->fileinfo[i].gameinfo_child_bmp,
                                            pPwork->fileinfo[i].gameinfo_changed_bmp);
                    /* 更新後、変化分bmpをクリア */
                    pPwork->fileinfo[i].gameinfo_changed_bmp = 0;
                }
            }
        }
        /* BeaconにGameInfoを乗せて発信 */
        MB_SendGameInfoBeacon(MBi_GetGgid(), MBi_GetTgid(), MBi_GetAttribute());
        break;

    case MB_CALLBACK_API_ERROR:
        /* ARM9でWM APIをコールした時点での返り値エラー */
        {
            u16     apiid, errcode;

            apiid = ((u16 *)arg)[0];
            errcode = ((u16 *)arg)[1];

            switch (errcode)
            {
            case WM_ERRCODE_INVALID_PARAM:
            case WM_ERRCODE_FAILED:
            case WM_ERRCODE_WM_DISABLE:
            case WM_ERRCODE_NO_DATASET:
            case WM_ERRCODE_FIFO_ERROR:
            case WM_ERRCODE_TIMEOUT:
                MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
                break;
            case WM_ERRCODE_OPERATING:
            case WM_ERRCODE_ILLEGAL_STATE:
            case WM_ERRCODE_NO_CHILD:
            case WM_ERRCODE_OVER_MAX_ENTRY:
            case WM_ERRCODE_NO_ENTRY:
            case WM_ERRCODE_INVALID_POLLBITMAP:
            case WM_ERRCODE_NO_DATA:
            case WM_ERRCODE_SEND_QUEUE_FULL:
            case WM_ERRCODE_SEND_FAILED:
            default:
                MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            /* WM APIコール後に返ってきたコールバックにおけるエラー */
            WMCallback *pWmcb = (WMCallback *)arg;
            switch (pWmcb->apiid)
            {
            case WM_APIID_INITIALIZE:
            case WM_APIID_SET_LIFETIME:
            case WM_APIID_SET_P_PARAM:
            case WM_APIID_SET_BEACON_IND:
            case WM_APIID_START_PARENT:
            case WM_APIID_START_MP:
            case WM_APIID_SET_MP_DATA:
            case WM_APIID_START_DCF:
            case WM_APIID_SET_DCF_DATA:
            case WM_APIID_DISCONNECT:
            case WM_APIID_START_KS:
                /* 以上のエラーは、WM最初期化の必要なエラー */
                MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
                break;
            case WM_APIID_RESET:
            case WM_APIID_END:
            default:
                /* その他のエラーについては、コールバックエラーとして返す */
                MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;

    }


#if ( CALLBACK_WM_STATE == 1 )
    MBi_CommChangeParentState(0, (u32)(MB_COMM_PSTATE_WM_EVENT | type), arg);
#endif

    if (type == MB_CALLBACK_END_COMPLETE)
    {
        MBCommPStateCallback tmpCb = pPwork->parent_callback;

        /* ワークの解放 */
        MI_CpuClearFast(pPwork, sizeof(MB_CommPWork));
        pPwork = NULL;
        if (tmpCb)                     // 状態遷移コールバック
        {
            (*tmpCb) (0, MB_COMM_PSTATE_END, NULL);
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentRecvDataPerChild

  Description:  各子機に対する受信データ処理

  Arguments:    arg : pointer to callback argument, child : child AID

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommParentRecvDataPerChild(void *arg, u16 child)
{
    MBCommChildBlockHeader hd;
    int     state;
    u8     *p_data;

    // child の範囲は1-15
    if (child == 0 || child > WM_NUM_MAX_CHILD)
    {
        return;
    }

    p_data = MBi_SetRecvBufferFromChild((u8 *)&((WMMpRecvData *)arg)->cdata[0], &hd, child);

    state = pPwork->p_comm_state[child - 1];

    MB_DEBUG_OUTPUT("RECV (CHILD:%2d)", child);
    MB_COMM_TYPE_OUTPUT(hd.type);

    switch (hd.type)                   // データタイプ別の処理
    {
    case MB_COMM_TYPE_CHILD_FILEREQ:

        if (p_data != NULL)
        {
            // MB_COMM_PSTATE_CONNECTEDの場合のみ受け入れる
            if (state == MB_COMM_PSTATE_CONNECTED)
            {
                MBCommRequestData req_data;
                // 子機のリクエストデータがすべて揃うまで次のステートには移行しない。
                MI_CpuCopy8(p_data, &req_data, MB_COMM_REQ_DATA_SIZE);
                pPwork->childggid[child - 1] = req_data.ggid;
                pPwork->childversion[child - 1] = req_data.version;
                MB_DEBUG_OUTPUT("Child [%2d] MB_IPL_VERSION : %04x\n", child, req_data.version);
                MI_CpuCopy8(&req_data.userinfo, &pPwork->childUser[child - 1], sizeof(MBUserInfo));
                pPwork->childUser[child - 1].playerNo = child;
                /* Callback 引数に受け取ったMBCommRequestDataを渡す。 */
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQUESTED, &req_data.userinfo);
            }

            if (state == MB_COMM_PSTATE_REQUESTED)
            {
                u8      i, entry_num = 0;
                u8      fileid = ((MBCommRequestData *)p_data)->fileid;

                /* ACTIVEではないfileNoの要求、不正なfileid、
                   もしくはfileNoのGGIDとリクエストしてきたGGIDが一致しなかったら
                   KICKする. */
                if (fileid >= MB_MAX_FILE
                    || pPwork->fileinfo[fileid].active == 0
                    || pPwork->childggid[child - 1] != pPwork->fileinfo[fileid].game_reg->ggid)
                {
                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_KICK;
                }
                else
                {
                    /* 設定人数を超過していたら、KICKする */
                    for (i = 0; i < WM_NUM_MAX_CHILD + 1; i++)
                    {
                        if (pPwork->fileinfo[fileid].gameinfo_child_bmp & (0x0001 << i))
                        {
                            entry_num++;
                        }
                    }

                    if (entry_num >= pPwork->fileinfo[fileid].game_reg->maxPlayerNum)
                    {
                        MB_DEBUG_OUTPUT("Member full (AID:%2d)\n", child);
                        /* リクエストは強制キャンセル */
                        pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                        MBi_CommChangeParentState(child, MB_COMM_PSTATE_MEMBER_FULL, NULL);
                        break;
                    }
                }

                switch (pPwork->req2child[child - 1])
                {
                case MB_COMM_USER_REQ_ACCEPT:
                    {

                        if (0 == (pPwork->child_entry_bmp & (0x0001 << (child))))
                        {
                            pPwork->child_num++;
                            pPwork->child_entry_bmp |= (0x0001 << (child));
                            pPwork->fileid_of_child[child - 1] = (s8)fileid;

                            pPwork->fileinfo[fileid].gameinfo_child_bmp |=
                                MB_GAMEINFO_CHILD_FLAG(child);
                            pPwork->fileinfo[fileid].gameinfo_changed_bmp |=
                                MB_GAMEINFO_CHILD_FLAG(child);
                            MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
                            pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;

                            MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQ_ACCEPTED, NULL);
                        }
                    }
                    break;

                case MB_COMM_USER_REQ_KICK:
                    MB_DEBUG_OUTPUT("Kick (AID:%2d)\n", child);
                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                    MBi_CommChangeParentState(child, MB_COMM_PSTATE_KICKED, NULL);
                    break;
                }
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:

        /* MB_COMM_PSTATE_REQ_ACCEPTEDの場合、MB_COMM_PSTATE_WAIT_TO_SENDに遷移させるのみ */
        if (state == MB_COMM_PSTATE_REQ_ACCEPTED)
        {
            MBi_CommChangeParentState(child, MB_COMM_PSTATE_WAIT_TO_SEND, NULL);
        }

        /* MB_COMM_PSTATE_WAIT_TO_SENDの場合、
           リクエストされたファイルに対し、pollbitmapを追加して
           MB_COMM_PSTATE_SEND_PROCEEDに遷移させる */
        else if (state == MB_COMM_PSTATE_WAIT_TO_SEND)
        {
            // SendStartのトリガがかかっていた場合、ブロック送信状態に遷移
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_SEND_START)
            {
                u8      fid = (u8)pPwork->fileid_of_child[child - 1];
                pPwork->fileinfo[fid].pollbmp |= (0x0001 << (child));
                pPwork->fileinfo[fid].currentb = 0;

                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_PROCEED, NULL);
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_CONTINUE:
        if (state == MB_COMM_PSTATE_SEND_PROCEED)
        {
            u8      fileid = (u8)pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            // 複数の child からきた nextSend のうち最大のものを送信対象とする
            SDK_ASSERT(fileid < MB_MAX_FILE);
            SDK_ASSERT(pPwork->fileinfo[fileid].pollbmp);

            pPwork->fileinfo[fileid].nextb =
                MBi_calc_nextsendblock(pPwork->fileinfo[fileid].nextb, hd.data.req);
            any_recv_bitmap |= (1 << fileid);
        }
        break;

    case MB_COMM_TYPE_CHILD_STOPREQ:
        if (state == MB_COMM_PSTATE_SEND_PROCEED)
        {
            u8      fileid = (u8)pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            SDK_ASSERT(fileid < MB_MAX_FILE);

            pPwork->fileinfo[fileid].pollbmp &= ~(0x0001 << (child));

            MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_COMPLETE, NULL);       // 送信完了
        }
        else if (state == MB_COMM_PSTATE_SEND_COMPLETE)
        {
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_BOOT)
            {
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_BOOT_REQUEST, NULL);
                break;
            }
        }
        break;

    case MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED:
        if (state == MB_COMM_PSTATE_BOOT_REQUEST)
        {
            /* 子機からのBOOTREQ_ACCEPTEDは、状態遷移に使用しません． */
            break;
        }

        break;

    default:
        break;
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentRecvData

  Description:  親機データ受信

  Arguments:    arg : pointer to callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommParentRecvData(void *arg)
{
    // 引数argに受信バッファのポインタが入っている
    WMmpRecvHeader *mpHeader = (WMmpRecvHeader *)arg;

    u16     i;
    WMmpRecvData *datap;

    // MBi_CommParentRecvDataPerChildにて統計的に評価する為、ここで初期化
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if (pPwork->fileinfo[i].active)
            pPwork->fileinfo[i].nextb = 0;
    }
    any_recv_bitmap = 0;

    // 各子機からの受信データを表示
    for (i = 1; i <= WM_NUM_MAX_CHILD; ++i)
    {
        // AID==i の子機データの先頭アドレスを取得
        datap = WM_ReadMPData(mpHeader, (u16)i);
        // AID==i の子機データが存在した場合
        if (datap != NULL)
        {
            // 受信データの表示
            if (datap->length == 0xffff)
            {
            }
            else if (datap->length != 0)
            {
                // 各子機データについての処理
                MBi_CommParentRecvDataPerChild(datap, i);
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendMsg

  Description:  親機からメッセージを送信

  Arguments:    pollbmp

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendMsg(u8 type, u16 pollbmp)
{
    MBCommParentBlockHeader hd;

    /* 送信開始通知. (引数無し) */
    hd.type = type;

    (void)MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    return MBi_BlockHeaderEnd(MB_COMM_PARENT_HEADER_SIZE, pollbmp, pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendDLFileInfo

  Description:  親機からDownloadFileInfoを送信

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendDLFileInfo(void)
{
    MBCommParentBlockHeader hd;
    u8     *databuf = ((u8 *)pPwork->common.sendbuf) + MB_COMM_PARENT_HEADER_SIZE;
    u16     child;
    u8      i, fid;
    s8      send_candidate_fid = -1;
    static s8 prev_fid = -1;
    u8      file_req_num[MB_MAX_FILE];
    u16     pollbmp = 0;

    MI_CpuClear8(&file_req_num[0], sizeof(u8) * MB_MAX_FILE);

    // 子が要求しているFileIDを集計
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED)
        {
            // MB_COMM_PSTATE_REQ_ACCEPTED対象の子機のみカウントする
            ++(file_req_num[pPwork->fileid_of_child[child - 1]]);
        }
    }

    fid = (u8)prev_fid;

    for (i = 0; i < MB_MAX_FILE; i++)  // 送信ファイルIDを決定する
    {
#if 1
        fid = (u8)((fid + 1) % MB_MAX_FILE);

        if (pPwork->fileinfo[fid].active && file_req_num[fid] > 0)
        {
            send_candidate_fid = (s8)fid;
            break;
        }

#else
        if (pPwork->fileinfo[i].active)
        {
            if (file_req_num[i] > 0)
            {

                /* 
                   多数決法
                   （  この場合、複数子機が異なるファイルでダウンロード開始待ちしていると
                   DownloadFileInfoの送信が、多数の方のファイルにロックされた状態になる。
                   マルチブート動作に関しての問題はないが、
                   子機アプリのステートがMB_COMM_CSTATE_REQ_ENABLEから進まなくなるので
                   (親機からのDownload決定後にそれらの子機に関してはエントリー処理が進む。)
                   対処が必要か。）
                 */

                if (send_candidate_fid == -1 || file_req_num[i] > file_req_num[send_candidate_fid])
                {
                    send_candidate_fid = i;
                }

            }
        }
#endif

    }

    if (send_candidate_fid == -1)
        return MB_SENDFUNC_STATE_ERR;

    prev_fid = send_candidate_fid;

    // poll bitmap設定(送信するFile番号と一緒のリクエストを出している子機のみ)
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED
            && pPwork->fileid_of_child[child - 1] == send_candidate_fid)
        {
            pollbmp |= (1 << child);
        }
    }

    MB_DEBUG_OUTPUT("DLinfo No %2d Pollbmp %04x\n", send_candidate_fid, pollbmp);

    // 一番最後に要求を受けた子機のFileInfoを送る
    hd.type = MB_COMM_TYPE_PARENT_DL_FILEINFO;
    hd.fid = send_candidate_fid;

    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    if (databuf)
    {
        // 送信バッファにデータをコピー
        MI_CpuCopy8(&pPwork->fileinfo[send_candidate_fid].dl_fileinfo,
                    databuf, sizeof(MBDownloadFileInfo));
    }

    return MBi_BlockHeaderEnd(sizeof(MBDownloadFileInfo) + MB_COMM_PARENT_HEADER_SIZE, pollbmp,
                              pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_ReloadCache

  Description:  指定された廃棄キャッシュにデータをリロード.

  Arguments:    p_task           param[0] に MBiCacheInfo を格納したタスク

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_ReloadCache(MBiTaskInfo * p_task)
{
    MBiCacheInfo *const p_info = (MBiCacheInfo *) p_task->param[0];
    MBiCacheList *const p_list = (MBiCacheList *) p_task->param[1];
    FSFile  file[1];
    FSArchive   *arc;
    u32          length = p_info->len;
    arc = FS_FindArchive(p_list->arc_name, (int)p_list->arc_name_len);
    if (!arc)
    {
        arc = p_list->arc_pointer;
    }
#ifdef SDK_TWL
    // TWLモードで実行中ならROMの範囲外リードを避ける。
    if (OS_IsRunOnTwl() && (arc == FS_FindArchive("rom", 3)))
    {
        const CARDRomHeaderTWL *header = (const CARDRomHeaderTWL *)CARD_GetOwnRomHeaderTWL();
        // 配信プログラムをTWL専用ファイルとして保持するケースもある点に注意。
        const u32   boundary_ntr = header->digest_area_ntr.offset + header->digest_area_ntr.length;
        const u32   boundary_ltd = header->digest_area_ltd.offset + header->digest_area_ltd.length;
        if ((p_info->src < boundary_ntr) && (p_info->src + length > boundary_ntr))
        {
            length = boundary_ntr - p_info->src;
        }
        else if ((p_info->src < boundary_ltd) && (p_info->src + length > boundary_ltd))
        {
            length = boundary_ltd - p_info->src;
        }
    }
#endif

    FS_InitFile(file);

    /* MB_ReadSegment() 時の対象アーカイブを指定 */
    if (FS_OpenFileDirect(file,
                          arc,
                          p_info->src, p_info->src + length, (u32)~0))
    {
        if (FS_ReadFile(file, p_info->ptr, (int)length) == length)
        {
            /* この場で READY にしても構わない */
            p_info->state = MB_CACHE_STATE_READY;
        }
        (void)FS_CloseFile(file);
    }

    /* カード抜けなどでファイルの読み込みに失敗 */
    if (p_info->state != MB_CACHE_STATE_READY)
    {
        /*
         * このページをMB_CACHE_STATE_BUSYのまま置いておくと
         * リークしてキャッシュ全体のページ数が減少してしまうので,
         * 常にインデックス[0]に存在するROMヘッダ(0x00000000-)
         * と同じアドレス設定でMB_CACHE_STATE_READYにしておく.
         * これにより, バッファに不定な内容が含まれたままであっても
         * 決して参照されることなく再び次回のページフォルト対象になる.
         * (ただし, 実際にはカードが抜けた時点で二度と呼ばれないはず)
         */
        p_info->src = 0;
        p_info->state = MB_CACHE_STATE_READY;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendBlock

  Description:  親機からBlockデータを送信

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendBlock(void)
{
    MBCommParentBlockHeader hd;
    u8     *databuf;
    u8      i;
    MB_BlockInfo bi;

    // 送信File番号の決定
    if (pPwork->file_num == 0)
        return MB_SENDFUNC_STATE_ERR;

    // ↓ここからblock転送本体

    // 送信対象のファイルを決定
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        pPwork->cur_fileid = (u8)((pPwork->cur_fileid + 1) % MB_MAX_FILE);
        if (pPwork->fileinfo[pPwork->cur_fileid].active
            && pPwork->fileinfo[pPwork->cur_fileid].pollbmp)
        {
            MB_DEBUG_OUTPUT("Send File ID:%2d\n", pPwork->cur_fileid);
            break;
        }
    }
    if (i == MB_MAX_FILE)              // 送信対象のファイルがない
    {
        return MB_SENDFUNC_STATE_ERR;
    }

    /* 送るファイルに対する送信ブロックの算定 */
    MBi_calc_sendblock(pPwork->cur_fileid);

    // ブロック情報の取得
    if (!MBi_get_blockinfo(&bi,
                           &pPwork->fileinfo[pPwork->cur_fileid].blockinfo_table,
                           pPwork->fileinfo[pPwork->cur_fileid].currentb,
                           &pPwork->fileinfo[pPwork->cur_fileid].dl_fileinfo.header))
    {
        return MB_SENDFUNC_STATE_ERR;
    }
    /* 送信パケットの準備 */
    hd.type = MB_COMM_TYPE_PARENT_DATA;
    hd.fid = pPwork->cur_fileid;
    hd.seqno = pPwork->fileinfo[pPwork->cur_fileid].currentb;
    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);

    /* キャッシュ経由のアクセス. (メモリ充分なら常にヒット) */
    {
        /* ブロックオフセットから CARD アドレスを計算する */
        u32     card_addr = (u32)(bi.offset -
                                  pPwork->fileinfo[pPwork->cur_fileid].blockinfo_table.
                                  seg_src_offset[bi.segment_no] +
                                  pPwork->fileinfo[pPwork->cur_fileid].card_mapping[bi.segment_no]);
        /* 指定の CARD アドレスに対してキャッシュリード */
        MBiCacheList *const pl = pPwork->fileinfo[pPwork->cur_fileid].cache_list;
        if (!MBi_ReadFromCache(pl, card_addr, databuf, bi.size))
        {
            /* キャッシュミスならタスクスレッドへリロード要求 */
            MBiTaskInfo *const p_task = &pPwork->cur_task;
            if (!MBi_IsTaskBusy(p_task))
            {
                /* 連続ページフォルトを避けるためのライフタイム */
                if (pl->lifetime)
                {
                    --pl->lifetime;
                }
                else
                {
                    /*
                     * 一番アドレスの若いキャッシュを破棄.
                     * この部分の動作は今後も引き続き検討.
                     */
                    MBiCacheInfo *pi = pl->list;
                    MBiCacheInfo *trg = NULL;
                    int     i;
                    for (i = 0; i < MB_CACHE_INFO_MAX; ++i)
                    {
                        if (pi[i].state == MB_CACHE_STATE_READY)
                        {
                            if (!trg || (trg->src > pi[i].src))
                                trg = &pi[i];
                        }
                    }
                    if (!trg)
                    {
                        OS_TPanic("cache-list is invalid! (all the pages are locked)");
                    }
                    pl->lifetime = 2;
                    trg->state = MB_CACHE_STATE_BUSY;
                    trg->src = (card_addr & ~31);
                    p_task->param[0] = (u32)trg;        /* MBiCacheInfo* */
                    p_task->param[1] = (u32)pl; /* MBiCacheList* */
                    MBi_SetTask(p_task, MBi_ReloadCache, NULL, 4);
                }
            }
            return MB_SENDFUNC_STATE_ERR;
        }
    }

    return MBi_BlockHeaderEnd((int)(bi.size + MB_COMM_PARENT_HEADER_SIZE),
                              pPwork->fileinfo[pPwork->cur_fileid].pollbmp, pPwork->common.sendbuf);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommParentSendData

  Description:  親機データ送信

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommParentSendData(void)
{
    struct bitmap
    {
        u16     connected;
        u16     req;
        u16     kick;
        u16     boot;
        u16     mem_full;
    };
    struct bitmap bmp;
    u16     child;
    int     errcode;

    MI_CpuClear16(&bmp, sizeof(struct bitmap));

    // 各子機に対しての親機状態を評価
    for (child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {

        switch (pPwork->p_comm_state[child - 1])
        {
        case MB_COMM_PSTATE_CONNECTED:
            bmp.connected |= (1 << child);
            break;

        case MB_COMM_PSTATE_REQ_ACCEPTED:
            bmp.req |= (1 << child);
            break;

        case MB_COMM_PSTATE_KICKED:
            bmp.kick |= (1 << child);
            break;

        case MB_COMM_PSTATE_SEND_PROCEED:
            break;

        case MB_COMM_PSTATE_BOOT_REQUEST:
            bmp.boot |= (1 << child);
            break;

        case MB_COMM_PSTATE_MEMBER_FULL:
            bmp.mem_full |= (1 << child);
            break;

        default:
            break;                     // 上記以外の状態はノーカウント
        }

    }
    /*
       Startmsg > DLFileInfo > Block
       の優先順位で送信を行う
     */
    if (bmp.boot)
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_BOOTREQ, bmp.boot);
    }
    else if (bmp.connected)            // エントリー要求許可メッセージ送信
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_SENDSTART, bmp.connected);
    }
    else if (bmp.mem_full)             // メンバー超過メッセージ送信
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_MEMBER_FULL, bmp.mem_full);
    }
    else if (bmp.kick)                 // エントリー拒否メッセージ送信
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_KICKREQ, bmp.kick);
    }
    else if (bmp.req)                  // MbDownloadFileInfoの送信
    {
        errcode = MBi_CommParentSendDLFileInfo();
    }
    else                               // Blockデータの送信
    {
        errcode = MBi_CommParentSendBlock();
    }

    // Connection維持のためのMP送信
    if (MB_SENDFUNC_STATE_ERR == errcode)
    {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_DUMMY, 0xffff);
    }

    return errcode;

}


/*---------------------------------------------------------------------------*
  Name:         MBi_calc_sendblock

  Description:  送信するブロックを計算

  Arguments:    file_id - 送信するファイルのID

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_calc_sendblock(u8 file_id)
{
    /* 子機から指定ブロックへの要求を受信していなければ更新しない */
    if ((any_recv_bitmap & (1 << file_id)) == 0)
    {
        return;
    }

    if (pPwork->fileinfo[file_id].active && pPwork->fileinfo[file_id].pollbmp)
    {
        if (pPwork->fileinfo[file_id].nextb <= pPwork->fileinfo[file_id].currentb &&
            pPwork->fileinfo[file_id].currentb <=
            pPwork->fileinfo[file_id].nextb + MB_SEND_THRESHOLD)
        {
            pPwork->fileinfo[file_id].currentb++;
        }
        else
        {
            pPwork->fileinfo[file_id].currentb = pPwork->fileinfo[file_id].nextb;
        }
        MB_DEBUG_OUTPUT("**FILE %2d SendBlock %d\n", file_id, pPwork->fileinfo[file_id].currentb);
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_calc_nextsendblock

  Description:  次に送るブロックを返す

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req)
{
    return max(next_block_req, next_block);
}


/*  ============================================================================

    miscellany functions

    ============================================================================*/

static inline u16 max(u16 a, u16 b)
{
    return (u16)((a > b) ? a : b);
}

static BOOL IsChildAidValid(u16 child_aid)
{
    return (child_aid >= 1 && child_aid <= WM_NUM_MAX_CHILD) ? TRUE : FALSE;
}

static void MBi_CommCallParentError(u16 aid, u16 errcode)
{
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeParentStateCallbackOnly(aid, MB_COMM_PSTATE_ERROR, &e_stat);
}
