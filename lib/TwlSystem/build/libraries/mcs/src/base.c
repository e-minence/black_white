/*---------------------------------------------------------------------------*
  Project:  TWL-System - libraries - mcs
  File:     base.c

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1155 $
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/ringBuffer.h>
#include <nnsys/mcs/config.h>
#ifndef SDK_TWLLTD
#include <isdbglib.h>
#endif
#include <istdbglib.h>
#include "baseCommoni.h"
#include "basei.h"


/* ========================================================================
    定数
   ======================================================================== */

static const int SEND_RETRY_MAX = 10;
static const int UIC_WAIT_TIMEOUT_FRAME = 60 * 2;


/* ========================================================================
    型定義
   ======================================================================== */

typedef struct DefRecvCBInfo DefRecvCBInfo;
struct DefRecvCBInfo
{
    NNSMcsRecvCBInfo    cbInfo;
    NNSMcsRingBuffer    ringBuf;
};

typedef struct NNSiMcsEnsWork NNSiMcsEnsWork;
struct NNSiMcsEnsWork
{
    NNSMcsRingBuffer    rdRb;
    NNSMcsRingBuffer    wrRb;
    NNSiMcsEnsMsgBuf    msgBuf;
};


/* ========================================================================
    static変数
   ======================================================================== */

static NNSiMcsWork*     spMcsWork   = NULL;     // 初期化されていれば NULL 以外になる

static NNSMcsDeviceCaps sDeviceCaps =
                        {
                            NITRODEVID_NULL,    // デバイス識別ID
                            0x00000000,         // リソース識別情報のフラグ
                        };


/* ========================================================================
    static関数
   ======================================================================== */

static NNS_MCS_INLINE
BOOL
IsInitialized()
{
    return spMcsWork != NULL;
}

static NNS_MCS_INLINE
uint32_t
RoundUp(
    uint32_t    value,
    uint32_t    alignment
)
{
    return (value + alignment -1) & ~(alignment -1);
}

static NNS_MCS_INLINE
uint32_t
RoundDown(
    uint32_t    value,
    uint32_t    alignment
)
{
    return value & ~(alignment -1);
}

static NNS_MCS_INLINE
u32
min_u32(
    u32     a,
    u32     b
)
{
    return a < b ? a: b;
}

static NNS_MCS_INLINE void
Lock()
{
    if (OS_IsThreadAvailable())
    {
        OS_LockMutex(&spMcsWork->mutex);
    }
}

static NNS_MCS_INLINE void
Unlock()
{
    if (OS_IsThreadAvailable())
    {
        OS_UnlockMutex(&spMcsWork->mutex);
    }
}

/*
    チャンネルを指定して、NNSMcsRecvCBInfoへのポインタを得る
*/
static NNSMcsRecvCBInfo*
GetRecvCBInfo(
    NNSFndList* pList,
    uint32_t    channel
)
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    while (NULL != (pInfo = NNS_FndGetNextListObject(pList, pInfo)))
    {
        if (pInfo->channel == channel)
        {
            return pInfo;
        }
    }

    return NULL;
}

static void
CallbackRecv(
    u32         userData,
    u32         channel,
    const void* pRecv,
    uint32_t    recvSize
)
{
    const NNSiMcsMsg *const pMsg = (NNSiMcsMsg*)pRecv;
    const u32 dataSize = recvSize - NNSi_MCS_MSG_HEAD_SIZE;

    // TWL用通信ライブラリでは、受信コールバック関数が別スレッドで呼び出される
    // ので、排他制御を行います。
    Lock();

    if ( recvSize < NNSi_MCS_MSG_HEAD_SIZE
      || NNSi_MCS_MSG_HEAD_VERSION != pMsg->head.version
    )
    {
        // OS_Printf("CallbackRecv() protocol error.\n");
        spMcsWork->bProtocolError = TRUE;
    }
    else
    {
        if (NNSi_MCS_SYSMSG_CHANNEL == channel)
        {
            u32 bConnect;

            if (dataSize == sizeof(bConnect))
            {
                bConnect = *(u32*)pMsg->data;
                spMcsWork->bHostDataRecived = (u8)bConnect;
                // OS_Printf("CallbackRecv() NNSi_MCS_SYSMSG_CHANNEL connect %d\n", bConnect);
            }
            else
            {
                // OS_Printf("CallbackRecv() NNSi_MCS_SYSMSG_CHANNEL data size error %d\n", dataSize);
            }
        }
        else
        {
            NNSFndList *const pList = (NNSFndList*)userData;
            NNSMcsRecvCBInfo *const pInfo = GetRecvCBInfo(pList, channel);

            // OS_Printf("CallbackRecv() [%04X, %p, %05d %08X, %p]\n", channel, pRecv, recvSize, userData, pInfo);

            if (pInfo)
            {
                (*pInfo->cbFunc)(pMsg->data, dataSize, pInfo->userData, pMsg->head.offset, pMsg->head.totalLen);
            }
        }
    }

    Unlock();
}

static void
CallbackRecvDummy(
    uintptr_t   /* userData */,
    uint32_t    /* channel */,
    const void* /* pRecv */,
    uint32_t    /* recvSize */
)
{
    // 何もしない
}

static void
DataRecvCallback(
    const void* pData,
    u32         dataSize,
    u32         userData,
    u32         offset,
    u32         totalSize
)
{
    DefRecvCBInfo* cbInfo = (DefRecvCBInfo*)userData;

    if (NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf) < offset + dataSize)
    {
        // エラー - バッファが底をついた
        NNS_WARNING(FALSE, "NNS mcs error : buffer is not enough. writable %d, data size %d, offset %d, total size %d\n",
            NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf),
            dataSize,
            offset,
            totalSize);
        return;
    }

    {
        BOOL ret = NNS_McsWriteRingBufferEx(
            cbInfo->ringBuf,
            pData,
            offset,
            dataSize,
            totalSize);
        NNS_ASSERT(ret);
    }
}

static NNS_MCS_INLINE NNSiMcsEnsWork*
GetEnsWorkPtr()
{
    return (NNSiMcsEnsWork*)NNSi_MCS_ENS_WRITE_RB_END;
}

static void
SetMaskResource(u32 maskResource)
{
    const BOOL preIRQ = OS_DisableIrq();
    sDeviceCaps.maskResource = maskResource;
    (void)OS_RestoreIrq(preIRQ);
}

static void
WaitSendData(void)
{
    NNS_McsPollingIdle();   // PC側が書き込み待ちでブロックしてしまわないようにしておく

    NNSi_McsSleep(8);
}

static void
WaitDeviceEnable(void)
{
    NNS_McsPollingIdle();

    NNSi_McsSleep(16);
}

int         _isdbusmgr_isresourceavailable(int /*typeResource*/);

__declspec ( weak )
int
_isdbusmgr_isresourceavailable(int /*typeResource*/)
{
    return FALSE;
}

static BOOL
IsDebuggerPresent()
{
    static const int    ISDRESOURCE_SW_DEBUGGER    =   0x00200;

    return _isdbusmgr_isresourceavailable(ISDRESOURCE_SW_DEBUGGER);
}

/*
    TWLハードウェア上で動作しているかどうか
*/
static NNS_MCS_INLINE BOOL
IsRunOnTwlHw()
{
    return OS_IsRunOnTwl() || OSi_IsNitroModeOnTwl();
}

/*
    ensata上で動作しているかどうか
*/
static NNS_MCS_INLINE BOOL
IsRunOnEnsata()
{
    #ifdef SDK_TWL
        return FALSE;   // ensataはNITRO用にビルドされたものでないと動作しない。
    #else
        return OS_IsRunOnEmulator();
    #endif
}

static BOOL
Open_Ensata(NNSMcsDeviceCaps* pCaps)
{
    #ifndef SDK_TWL
        const NNSMcsRingBuffer rdRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_READ_RB_START;
        const NNSMcsRingBuffer wrRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_WRITE_RB_START;

        if (! NNS_McsCheckRingBuffer(rdRb) || ! NNS_McsCheckRingBuffer(wrRb))
        {
            return FALSE;
        }

        {
            NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
            pWork->rdRb = rdRb;
            pWork->wrRb = wrRb;
        }

        pCaps->deviceID     = 0;
        pCaps->maskResource = NITROMASK_RESOURCE_POLL;

        return TRUE;
    #else
        (void)pCaps;
        return FALSE;
    #endif
}

static BOOL
Open_NITRO(NNSMcsDeviceCaps* pCaps)
{
    #ifndef SDK_TWLLTD
        // デバイスの優先度 PRIority
        enum
        {
            DEV_PRI_NONE,               // デバイスなし
            DEV_PRI_UNKNOWN,            // 未知のデバイス
            DEV_PRI_NITRODBG,           // IS-NITRO-DEBUGGER
            DEV_PRI_NITROUIC,           // IS-NITRO-UIC
            DEV_PRI_NITROUSB,           // IS-NITRO-EMULATOR

            DEV_PRI_MAX
        };

        int devPri = DEV_PRI_NONE;
        int devID = 0;
        const int devNum = NNS_McsGetMaxCaps();
        int findDevID = 0;
        const NITRODEVCAPS* pNITROCaps;
        // OS_Printf("on DEBBUGER ? %s\n", IsDebuggerPresent() ? "yes": "no");

        for (findDevID = 0; findDevID < devNum; ++findDevID)
        {
            int findDevPri = DEV_PRI_NONE;
            pNITROCaps = NITROToolAPIGetDeviceCaps(findDevID);
            switch (pNITROCaps->m_nDeviceID)
            {
            case NITRODEVID_NITEMULATOR:    // IS-NITRO-EMULATOR
                if (IsDebuggerPresent())
                {
                    findDevPri = DEV_PRI_NITRODBG;
                }
                else    // IS-NITRO-DEBUGGER ソフトウェアから起動していないとき
                {
                    findDevPri = DEV_PRI_NITROUSB;
                }
                break;
            case NITRODEVID_NITUIC:         // IS-NITRO-UIC CARTRIDGE
                findDevPri = DEV_PRI_NITROUIC;
                break;
            default:
                findDevPri = DEV_PRI_UNKNOWN;
            }

            if (devPri < findDevPri)
            {
                devPri = findDevPri;
                devID  = findDevID;
            }
        }

        if (devPri == DEV_PRI_NONE || devPri == DEV_PRI_UNKNOWN)
        {
            OS_Warning("no device.\n");     // 接続可能なデバイスが見つかりません。
            return FALSE;
        }

        pNITROCaps = NITROToolAPIGetDeviceCaps(devID);
        if (! NITROToolAPIOpen(pNITROCaps))
        {
            return FALSE;   // デバイスのオープン&初期化に失敗しました
        }

        pCaps->deviceID     = pNITROCaps->m_nDeviceID;
        pCaps->maskResource = pNITROCaps->m_dwMaskResource;
        spMcsWork->bLengthEnable = FALSE;

        {
            // コールバック関数の登録（Widows側のアプリケーションからデータが送信されれば、この関数が呼ばれる）
            BOOL bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecv, (u32)&spMcsWork->recvCBInfoList);
            NNS_ASSERT(bSuccess);
        }

        return TRUE;
    #else
        (void)pCaps;
        return FALSE;
    #endif
}

static BOOL
Open_TWL(NNSMcsDeviceCaps* pCaps)
{
    u32 nDevMask;

    // コールバック登録
    ISTDSIOSetRecvCallback( (ISTDSIORecvCbFunc)CallbackRecv, (void*)&spMcsWork->recvCBInfoList );

    //
    nDevMask = ISTDHIOGetDevMask();
    if (nDevMask == 0 || !ISTDHIOOpen(nDevMask))
    {
        OS_Printf(" ISHIO: device open failure. - DevMask %08X\n", nDevMask);
        return FALSE;
    }

    pCaps->deviceID     = 0;
    pCaps->maskResource = NITROMASK_RESOURCE_POLL;

    return TRUE;

}

static NNS_MCS_INLINE BOOL
Close_Ensata()
{
    #ifndef SDK_TWL
        NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
        pWork->rdRb = 0;
        pWork->wrRb = 0;

        return TRUE;
    #else
        return FALSE;
    #endif
}

static BOOL
Close_NITRO()
{
    #ifndef SDK_TWLLTD
        return NITROToolAPIClose();
    #else
        return FALSE;
    #endif
}

static BOOL
Close_TWL()
{
    return ISTDHIOClose();
}

/* ========================================================================
    外部関数(公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_McsInit

  Description:  デバイスと通信するための初期化関数です。
                NNS_Mcs関数を使用するときは、
                使用する前に必ずこの関数を呼び出しておく必要があります。

  Arguments:    workMem:  MCSが使用するワーク用メモリ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static void
Init_TWL()
{
    ISTDHIOInit();
}

//---------------------------------------------------------
static void
Init_Ensata()
{
    // 何もしない
}

//---------------------------------------------------------
static void
Init_NITRO()
{
    #ifndef SDK_TWLLTD
        NITROToolAPIInit();
    #endif
}

//---------------------------------------------------------
static void
Init_InVariousDevice()
{
    if (IsRunOnTwlHw())
        Init_TWL();
    else if (IsRunOnEnsata())
        Init_Ensata();
    else
        Init_NITRO();
}

//---------------------------------------------------------
void
NNS_McsInit(void* workMem)
{
    if (IsInitialized())
    {
        return;
    }

    NNS_NULL_ASSERT(workMem);

    Init_InVariousDevice();

    spMcsWork = (NNSiMcsWork*)workMem;

    spMcsWork->bProtocolError = FALSE;
    spMcsWork->bLengthEnable = FALSE;
    spMcsWork->bHostDataRecived = FALSE;
    OS_InitMutex(&spMcsWork->mutex);
    NNS_FND_INIT_LIST(&spMcsWork->recvCBInfoList, NNSMcsRecvCBInfo, link);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetMaxCaps

  Description:  現在ターゲットに接続されている通信デバイスの総数を取得します。

  Arguments:    なし。

  Returns:      現在ターゲットに接続されている通信デバイスの総数を返します。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static int
GetMaxCaps_TWL()
{
    return 1;
}

//---------------------------------------------------------
static int
GetMaxCaps_Ensata()
{
    return 1;
}

//---------------------------------------------------------
static int
GetMaxCaps_NITRO()
{
    #ifndef SDK_TWLLTD
        return NITROToolAPIGetMaxCaps();
    #else
        return 0;
    #endif
}

//---------------------------------------------------------
static int
GetMaxCaps_InVariousDevice()
{
    if (IsRunOnTwlHw())
        return GetMaxCaps_TWL();
    else  if (IsRunOnEnsata())
        return GetMaxCaps_Ensata();
    else
        return GetMaxCaps_NITRO();
}

//---------------------------------------------------------
int
NNS_McsGetMaxCaps()
{
    int num;

    NNS_ASSERT(IsInitialized());
    Lock();

    num = GetMaxCaps_InVariousDevice();

    Unlock();
    return num;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsOpen

  Description:  デバイスをオープンし、オープンしたデバイスの情報を引数で指定した
                変数へ格納します。
                接続されているデバイスが複数存在するときは、
                以下の順で見つかったデバイスをオープンします。
                  1. IS-NITRO-UIC
                  2. IS-NITRO-EMULATOR
                  4. 未知のデバイス

  Arguments:    pCaps:  通信デバイスの情報を取得するための構造体へのポインタ。

  Returns:      オープンに成功した場合、TRUEを返します。
                失敗した場合、FALSEを返します。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static BOOL
Open_InVariousDevice(NNSMcsDeviceCaps* pCaps)
{
    if (IsRunOnTwlHw())
        return Open_TWL(pCaps);
	else if (IsRunOnEnsata())
        return Open_Ensata(pCaps);
    else
        return Open_NITRO(pCaps);
}

//---------------------------------------------------------
BOOL
NNS_McsOpen(NNSMcsDeviceCaps* pCaps)
{
    BOOL bSuccess;

    NNS_ASSERT(IsInitialized());
    Lock();

    bSuccess = Open_InVariousDevice(pCaps);

    if (bSuccess)
    {
        sDeviceCaps.deviceID = pCaps->deviceID;
        SetMaskResource(pCaps->maskResource);

        spMcsWork->bHostDataRecived = FALSE;
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsClose

  Description:  オープンしているデバイスをクローズします。

  Arguments:    なし。

  Returns:      クローズに成功した場合、TRUEを返します。
                失敗した場合、FALSEを返します。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static BOOL
Close_InVariousDevice()
{
    if (IsRunOnTwlHw())
        return Close_TWL();
    else if (IsRunOnEnsata())
        return Close_Ensata();
    else
        return Close_NITRO();
}

//---------------------------------------------------------
BOOL
NNS_McsClose()
{
    BOOL bSuccess;

    NNS_ASSERT(IsInitialized());
    Lock();

    bSuccess = Close_InVariousDevice();

    if (bSuccess)
    {
        sDeviceCaps.deviceID = NITRODEVID_NULL; // デバイス識別IDの初期化
        SetMaskResource(0x00000000);            // リソース識別情報のフラグ初期化
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsRegisterRecvCallback

  Description:  データを受信したときにコールバックされる関数を登録します。

                登録するコールバック関数内ではデータの送受信を行わないでください。
                また、割り込みが禁止されている場合があるため、
                割り込み待ちループも行わないでください。

  Arguments:    pInfo:     コールバック関数の情報を保持する構造体へのポインタ。
                channel:   ユーザー任意に決められたデータ送信の識別用の値。
                cbFunc:    登録するコールバック関数。
                userData:  ユーザー任意の値。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_McsRegisterRecvCallback(
    NNSMcsRecvCBInfo*   pInfo,
    u16                 channel,
    NNSMcsRecvCallback  cbFunc,
    u32                 userData
)
{
    NNS_ASSERT(IsInitialized());
    Lock();

    NNS_ASSERT(NULL == GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel));   // 既に同じチャンネルを使用しているものが無いこと

    pInfo->channel  = channel;
    pInfo->cbFunc   = cbFunc;
    pInfo->userData = userData;

    NNS_FndAppendListObject(&spMcsWork->recvCBInfoList, pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsRegisterStreamRecvBuffer

  Description:  データ受信用のバッファを登録します。
                内部で、関数NNS_McsRegisterRecvCallback()を呼び出すため、
                データ受信のコールバックと同時に使用できません。
                指定したバッファに内部で使用するための情報エリアを確保します。
                そのため、バッファのサイズは少なくとも48以上である必要があります。

                受信用バッファが受信データで一杯になり、新規に受信したデータを格納する
                だけの空き容量が無い場合は、その受信データは捨てられます。
                従って、通信で使用するデータ量に合わせてバッファサイズを十分な大きさに
                設定する必要があります。

  Arguments:    channel:   ユーザー任意に決められたデータ送信の識別用の値。
                buf:       登録する受信用バッファ。
                bufSize:   登録する受信用バッファのサイズ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_McsRegisterStreamRecvBuffer(
    u16     channel,
    void*   buf,
    u32     bufSize
)
{
    uintptr_t start = (uintptr_t)buf;
    uintptr_t end = start + bufSize;
    u8* pBBuf;
    DefRecvCBInfo* pInfo;

    NNS_ASSERT(IsInitialized());
    Lock();

    start = RoundUp(start, 4);
    end = RoundDown(end, 4);

    NNS_ASSERT(start < end);

    buf = (void*)start;
    bufSize = end - start;

    NNS_ASSERT(bufSize >= sizeof(DefRecvCBInfo) + sizeof(NNSiMcsRingBufferHeader) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));

    pBBuf = buf;
    pInfo = buf;

    pInfo->ringBuf = NNS_McsInitRingBuffer(pBBuf + sizeof(DefRecvCBInfo), bufSize - sizeof(DefRecvCBInfo));

    NNS_McsRegisterRecvCallback(
        &pInfo->cbInfo,
        channel,
        DataRecvCallback,
        (uintptr_t)pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsUnregisterRecvResource

  Description:  NNS_McsRegisterRecvCallback()で登録した受信用コールバック関数、
                あるいはNNS_McsRegisterStreamRecvBuffer()で登録した受信用バッファの
                登録を解除します。

  Arguments:    channel:  コールバック関数やバッファを登録するときに指定した値。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_McsUnregisterRecvResource(u16 channel)
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    NNS_ASSERT(IsInitialized());
    Lock();

    pInfo = GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel);
    NNS_ASSERT(pInfo);

    NNS_FndRemoveListObject(&spMcsWork->recvCBInfoList, pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetStreamReadableSize

  Description:  一度の関数NNS_McsReadStream()呼び出しで読み込めるデータのサイズを
                取得します。

  Arguments:    channel:    データ受信の識別用の値。ユーザー任意に決められます。

  Returns:      読み込み可能なデータのサイズを返します。
 *---------------------------------------------------------------------------*/
u32
NNS_McsGetStreamReadableSize(u16 channel)
{
    u32 size;

    NNS_ASSERT(IsInitialized());
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        size = NNS_McsGetRingBufferReadableBytes(pCBInfo->ringBuf);
    }

    Unlock();
    return size;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetTotalStreamReadableSize

  Description:  受信用バッファにある読み込み可能なデータのサイズの合計を取得します。

  Arguments:    channel:    データ受信の識別用の値。ユーザー任意に決められます。

  Returns:      データのサイズの合計を返します。
 *---------------------------------------------------------------------------*/
u32
NNS_McsGetTotalStreamReadableSize(u16 channel)
{
    u32 size;

    NNS_ASSERT(IsInitialized());
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        size =  NNS_McsGetRingBufferTotalReadableBytes(pCBInfo->ringBuf);
    }

    Unlock();
    return size;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_McsReadStream

  Description:  データの受信を行います。

                受信したデータの長さがsizeで指定したバッファサイズより大きい場合は、
                sizeで指定した分のみデータを読み込みます。このときの戻り値は FALSE
                になります。残りのサイズは、NNS_McsGetStreamReadableSize()で求められます。

  Arguments:    channel:    データ受信の識別用の値。ユーザー任意に決められます。
                data:       読み込むデータを格納するバッファへのポインタ。
                size:       読み込むデータを格納するバッファのサイズ。
                pReadSize:  実際に読み込まれたデータのサイズ。

  Returns:      関数が成功した場合は TRUE、失敗した場合は FALSE を返します。
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsReadStream(
    u16         channel,
    void*       data,
    u32         size,
    u32*        pReadSize
)
{
    BOOL bSuccess;

    NNS_ASSERT(IsInitialized());
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&spMcsWork->recvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        bSuccess = NNS_McsReadRingBuffer(pCBInfo->ringBuf, data, size, pReadSize);
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetStreamWritableLength

  Description:  データ送信するためのバッファの最大長を取得します。
                この長さ以上は送信できません。

  Arguments:    pLength:  データ送信バッファの長さを入れる変数へのポインタ。

  Returns:      関数が成功した場合は TRUE、失敗した場合は FALSE が返ります。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static BOOL
GetStreamWritableLength_TWL(u32* pLength)
{
    *pLength = ISTDSIO_MAX_PAYLOAD_SIZE;
    return TRUE;
}

//---------------------------------------------------------
static BOOL
GetStreamWritableLength_Ensata(u32* pLength)
{
    #ifndef SDK_TWL
        *pLength = NNS_McsGetRingBufferWritableBytes(GetEnsWorkPtr()->wrRb);
        return TRUE;
    #else
        (void)pLength;
        return FALSE;
    #endif
}

//---------------------------------------------------------
static BOOL
GetStreamWritableLength_NITRO(u32* pLength)
{
    #ifndef SDK_TWLLTD
        BOOL ret = FALSE;
        u32 i;

        // UICの場合は、デバイスオープンあとの数十ミリ秒間は0を返してくる
        for (i = 0; i < UIC_WAIT_TIMEOUT_FRAME; ++i)
        {
            ret = 0 != NITROToolAPIStreamGetWritableLength(pLength);

            if (! ret)
            {
                OS_Printf("NNS Mcs error: faild NITROToolAPIStreamGetWritableLength()\n");
                break;
            }

            if (spMcsWork->bLengthEnable)
            {
                break;
            }

            if (*pLength > 0)
            {
                spMcsWork->bLengthEnable = TRUE;
                break;
            }

            WaitDeviceEnable();
        }

        return ret;
    #else
        (void)pLength;
        return FALSE;
    #endif
}

//---------------------------------------------------------
static BOOL
GetStreamWritableLength_InVariousDevice(u32* pLength)
{
    if (IsRunOnTwlHw())
        return GetStreamWritableLength_TWL(pLength);
    else if (IsRunOnEnsata())
        return GetStreamWritableLength_Ensata(pLength);
    else
        return GetStreamWritableLength_NITRO(pLength);
}

//---------------------------------------------------------
BOOL
NNS_McsGetStreamWritableLength(u32* pLength)
{
    BOOL ret = FALSE;
    u32 length;

    NNS_ASSERT(IsInitialized());
    Lock();

    ret = GetStreamWritableLength_InVariousDevice(&length);

    if (ret)
    {
        if (length < NNSi_MCS_MSG_HEAD_SIZE)
        {
            *pLength = 0;
        }
        else
        {
            *pLength = length - NNSi_MCS_MSG_HEAD_SIZE;
        }
    }

    Unlock();
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsWriteStream

  Description:  データの送信を行います。

  Arguments:    channel:  データ送信の識別用の値。ユーザー任意に決められます。
                data:     送信するデータを格納するバッファへのポインタ。
                size:     送信するデータのサイズ。

  Returns:      関数が成功した場合は TRUE、失敗した場合は FALSE が返ります。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static BOOL
WriteStream_TWL(
    u16         channel,
    NNSiMcsMsg* pBlock,
    u32         length
)
{
    if (! ISTDSIOSend(
        channel,
        (const void*)pBlock,
        NNSi_MCS_MSG_HEAD_SIZE + length)
    )
    {
        NNS_WARNING(FALSE, "NNS Mcs error: failed ISTDSIOSend()\n");
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------
static BOOL
WriteStream_Ensata(
    u16         channel,
    NNSiMcsMsg* pBlock,
    u32         length
)
{
    #ifndef SDK_TWL
        NNSiMcsEnsWork *const pWork = GetEnsWorkPtr();

        pWork->msgBuf.channel = channel;
        MI_CpuCopy8(pBlock, pWork->msgBuf.buf, NNSi_MCS_MSG_HEAD_SIZE + length);
        if (! NNS_McsWriteRingBuffer(
            pWork->wrRb,
            &pWork->msgBuf,
            offsetof(NNSiMcsEnsMsgBuf, buf) + NNSi_MCS_MSG_HEAD_SIZE + length)
        )
        {
            NNS_WARNING(FALSE, "NNS Mcs error: send error\n");
            return FALSE;
        }

        return TRUE;
    #else
        (void)channel;
        (void)pBlock;
        (void)length;
        return FALSE;
    #endif
}

//---------------------------------------------------------
static BOOL
WriteStream_NITRO(
    u16         channel,
    NNSiMcsMsg* pBlock,
    u32         length
)
{
    #ifndef SDK_TWLLTD
        if (! NITROToolAPIWriteStream(
            channel,
            pBlock,
            NNSi_MCS_MSG_HEAD_SIZE + length)
        )
        {
            NNS_WARNING(FALSE, "NNS Mcs error: failed NITROToolAPIWriteStream()\n");
            return FALSE;
        }

        return TRUE;
    #else
        (void)channel;
        (void)pBlock;
        (void)length;
        return FALSE;
    #endif
}

//---------------------------------------------------------
static BOOL
WriteStream_InVariousDevice(
    u16         channel,
    NNSiMcsMsg* pBlock,
    u32         length
)
{
    if (IsRunOnTwlHw())
        return WriteStream_TWL(channel, pBlock, length);
    else if (IsRunOnEnsata())
        return WriteStream_Ensata(channel, pBlock, length);
    else
        return WriteStream_NITRO(channel, pBlock, length);
}

//---------------------------------------------------------
BOOL
NNS_McsWriteStream(
    u16         channel,
    const void* data,
    u32         size
)
{
    const u8 *const pSrc = (const u8*)data;
    int retryCnt = 0;
    u32 offset = 0;

    NNS_ASSERT(IsInitialized());
    Lock();

    // OS_Printf("NNS_McsWriteStream channel %d, size %d\n", (int)(channel), size);

    while (offset < size)
    {
        const u32 restSize = size - offset;
        u32 length;

        if (! NNS_McsGetStreamWritableLength(&length))  // 1度に書き込み可能なサイズを取得
        {
            break;
        }

        if (restSize > length && length < NNSi_MCS_MSG_DATA_SIZE_MIN)
        {
            if (++retryCnt > SEND_RETRY_MAX)
            {
                NNS_WARNING(FALSE, "NNS Mcs error: send time out writable bytes %d, rest bytes %d\n", length, restSize);
                break;
            }
            WaitSendData(); // 書き込みできるようになるまで待つ
        }
        else
        {
            NNSiMcsMsg* pBlock = &spMcsWork->writeBuf;

            length = min_u32(min_u32(restSize, length), NNSi_MCS_MSG_DATA_SIZE_MAX);

            pBlock->head.version = NNSi_MCS_MSG_HEAD_VERSION;
            pBlock->head.reserved = 0;
            pBlock->head.offset = offset;
            pBlock->head.totalLen = size;
            MI_CpuCopy8(pSrc + offset, pBlock->data, length);

            // OS_Printf("1 write offset %d, length [%d/%d]\n", pBlock->head.offset, length, pBlock->head.totalLen);

            if (! WriteStream_InVariousDevice(channel, pBlock, length))
            {
                break;
            }

            offset += length;
        }
    }

    Unlock();
    return offset == size;  // 最後まで書き込めたら真を返す
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsClearBuffer

  Description:  通信用のバッファの内容を破棄します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static void ClearBuffer_TWL()
{
    // ダミーのコールバック関数を登録
    ISTDSIOSetRecvCallback((ISTDSIORecvCbFunc)CallbackRecvDummy, (void*)0);

    // 本来のコールバック関数を再登録
    ISTDSIOSetRecvCallback( (ISTDSIORecvCbFunc)CallbackRecv, (void*)&spMcsWork->recvCBInfoList );
}

//---------------------------------------------------------
static void
ClearBuffer_Ensata()
{
    #ifndef SDK_TWL
        NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
        NNS_McsClearRingBuffer(pWork->rdRb);
    #endif
}

//---------------------------------------------------------
static void
ClearBuffer_NITRO()
{
    #ifndef SDK_TWLLTD
        // ダミーのコールバック関数を登録
        BOOL bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecvDummy, 0);
        NNS_ASSERT(bSuccess);

        NITROToolAPIPollingIdle();

        // 本来のコールバック関数を再登録
        bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecv, (u32)&spMcsWork->recvCBInfoList);
        NNS_ASSERT(bSuccess);
    #endif
}

//---------------------------------------------------------
static void
ClearBuffer_InVariousDevice()
{
    if (IsRunOnTwlHw())
        ClearBuffer_TWL();
    else if (IsRunOnEnsata())
        ClearBuffer_Ensata();
    else
        ClearBuffer_NITRO();
}

//---------------------------------------------------------
void
NNS_McsClearBuffer()
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    NNS_ASSERT(IsInitialized());
    Lock();

    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL)
    {
        ClearBuffer_InVariousDevice();
    }

    while (NULL != (pInfo = NNS_FndGetNextListObject(&spMcsWork->recvCBInfoList, pInfo)))
    {
        if (pInfo->cbFunc == DataRecvCallback)  // リングバッファを使用しているもの
        {
            DefRecvCBInfo* pDefCBInfo = (DefRecvCBInfo*)pInfo->userData;
            NNS_McsClearRingBuffer(pDefCBInfo->ringBuf);
        }
    }

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsIsServerConnect

  Description:  mcsサーバがNITROハードウェアに対して接続しているか
                切断しているかの状態を返す。

  Arguments:    なし。

  Returns:      mcsサーバがNITROハードウェアに接続しに来ていたら真、
                切断しようとしていたら偽を返す。
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsIsServerConnect()
{
    BOOL bSuccess;

    NNS_ASSERT(IsInitialized());
    Lock();

    NNS_McsPollingIdle();

    bSuccess = spMcsWork->bHostDataRecived;

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPollingIdle

  Description:  メインループ内でこの関数を呼び出してください。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static void PollingIdle_TWL()
{
    // なし
}

//---------------------------------------------------------
static void
PollingIdle_Ensata()
{
    #ifndef SDK_TWL
        NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
        uint32_t readableBytes;
        uint32_t readBytes;

        while (0 < (readableBytes = NNS_McsGetRingBufferReadableBytes(pWork->rdRb)))
        {
            NNS_ASSERT(offsetof(NNSiMcsEnsMsgBuf, buf) < readableBytes && readableBytes <= sizeof(NNSiMcsEnsMsgBuf));

            {
                BOOL bRet = NNS_McsReadRingBuffer(pWork->rdRb, &pWork->msgBuf, readableBytes, &readBytes);
                NNS_ASSERT(bRet);
            }

            CallbackRecv((u32)&spMcsWork->recvCBInfoList, pWork->msgBuf.channel, pWork->msgBuf.buf, readBytes - offsetof(NNSiMcsEnsMsgBuf, buf));
        }
    #endif
}

//---------------------------------------------------------
static void
PollingIdle_NITRO()
{
    #ifndef SDK_TWLLTD
        NITROToolAPIPollingIdle();
    #endif
}

//---------------------------------------------------------
static void
PollingIdle_InVariousDevice()
{
    if (IsRunOnTwlHw())
        PollingIdle_TWL();
    else if (IsRunOnEnsata())
        PollingIdle_Ensata();
    else
        PollingIdle_NITRO();
}

//---------------------------------------------------------
void
NNS_McsPollingIdle()
{
    NNS_ASSERT(IsInitialized());
    Lock();

    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL)
    {
        PollingIdle_InVariousDevice();

        if (spMcsWork->bProtocolError)
        {
            const u32 data = TRUE;  // 接続
            u32 length;
            if ( NNS_McsGetStreamWritableLength(&length)
              || sizeof(data) <= length
            )
            {
                // NITRO側のヘッダバージョンを伝えるためにメッセージ送信
                if (NNS_McsWriteStream(NNSi_MCS_SYSMSG_CHANNEL, &data, sizeof(data)))
                {
                    // 書き込めたら、NITRO側は停止しておく
                    OS_Panic("mcs message version error.\n");
                }
            }
        }
    }

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsVBlankInterrupt

  Description:  VBlank割り込みハンドラ内でこの関数を呼び出してください。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static void
VBlankInterrupt_TWL()
{
    // 何もしない
}

//---------------------------------------------------------
static void
VBlankInterrupt_Ensata()
{
    // 何もしない
}

//---------------------------------------------------------
static void
VBlankInterrupt_NITRO()
{
    #ifndef SDK_TWLLTD
        NITROToolAPIVBlankInterrupt();
    #endif
}

//---------------------------------------------------------
static void
VBlankInterrupt_InVariousDevice()
{
    if (IsRunOnTwlHw())
        VBlankInterrupt_TWL();
    else if (IsRunOnEnsata())
        VBlankInterrupt_Ensata();
    else
        VBlankInterrupt_NITRO();
}

//---------------------------------------------------------
void
NNS_McsVBlankInterrupt()
{
    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_VBLANK)
    {
        VBlankInterrupt_InVariousDevice();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsCartridgeInterrupt

  Description:  カートリッジ割り込みハンドラ内でこの関数を呼び出してください。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------
static void
CartridgeInterrupt_TWL()
{
    // 何もしない
}

//---------------------------------------------------------
static void
CartridgeInterrupt_Ensata()
{
    // 何もしない
}

//---------------------------------------------------------
static void
CartridgeInterrupt_NITRO()
{
    #ifndef SDK_TWLLTD
        NITROToolAPICartridgeInterrupt();
    #endif
}

//---------------------------------------------------------
static void
CartridgeInterrupt_InVariousDevice()
{
    if (IsRunOnTwlHw())
        CartridgeInterrupt_TWL();
    else if (IsRunOnEnsata())
        CartridgeInterrupt_Ensata();
    else
        CartridgeInterrupt_NITRO();
}

void
NNS_McsCartridgeInterrupt()
{
    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_CARTRIDGE)
    {
        CartridgeInterrupt_InVariousDevice();
    }
}

/* #if ! defined(NNS_FINALROM) */
#endif
