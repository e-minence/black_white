/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-17#$
  $Rev: 8984 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MB_MB_H_
#define NITRO_MB_MB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/misc.h>
#include <nitro/fs/file.h>
#include <nitro/wm.h>


/* ---------------------------------------------------------------------

        定数

   ---------------------------------------------------------------------*/

#define MB_MAX_CHILD                    WM_NUM_MAX_CHILD

/* ファイル名の文字数 */
#define MB_FILENAME_SIZE_MAX            (10)

/* 親機のファイル最大数 */
#define MB_MAX_FILE                     (16)

/* マルチブートワークのサイズ */
#define MB_SYSTEM_BUF_SIZE              (0xC000)

/* 親機の通信サイズの設定可能範囲 */
#define MB_COMM_PARENT_SEND_MIN         256
#define MB_COMM_PARENT_SEND_MAX         510

#define MB_COMM_PARENT_RECV_MIN         8
#define MB_COMM_PARENT_RECV_MAX         16

/* MB_ReadSegment() に必要なバッファサイズの最小値 */
#define MB_SEGMENT_BUFFER_MIN           0x10000

/* MB API 結果型 */
#define MB_SUCCESS                      MB_ERRCODE_SUCCESS      // 旧互換

/* MB_Init() に対して TGID を自動で設定するよう指定する定義値 */
#define	MB_TGID_AUTO	0x10000

/*
 * マルチブート用親機ゲーム情報
 */

#define MB_ICON_COLOR_NUM               16      // アイコン色数
#define MB_ICON_PALETTE_SIZE            (MB_ICON_COLOR_NUM * 2) // アイコンデータサイズ (16bitカラー×色数)
#define MB_ICON_DATA_SIZE               512     // アイコンデータサイズ (32×32dot 16色)
#define MB_GAME_NAME_LENGTH             48      // ゲームネーム長       (2byte単位)         ※最大で左記文字数でかつ、横幅185ドット以内に収まる範囲で指定。
#define MB_GAME_INTRO_LENGTH            96      // ゲーム説明長         (2byte単位)         ※最大で左記文字数でかつ、横幅199ドット×2に収まる範囲で指定。
#define MB_USER_NAME_LENGTH             10      // ユーザーネーム長     (2byte単位)
#define MB_MEMBER_MAX_NUM               15      // 通信メンバー最大数
#define MB_FILE_NO_MAX_NUM              64      // MbGameInfo.fileNoはビーコンに乗せる時に6bitになるので、最大は64個
#define MB_GAME_INFO_RECV_LIST_NUM      16      // 子機側でのゲーム情報受信リスト個数

/* ブートタイプ */
#define MB_TYPE_ILLEGAL                 0       /* 不正な状態 */
#define MB_TYPE_NORMAL                  1       /* ROM から起動 */
#define MB_TYPE_MULTIBOOT               2       /* マルチブート子機 */

#define MB_BSSDESC_SIZE                 (sizeof(MBParentBssDesc))
#define MB_DOWNLOAD_PARAMETER_SIZE      HW_DOWNLOAD_PARAMETER_SIZE

/* 親機ダウンロードの状態 */
typedef enum
{
    MB_COMM_PSTATE_NONE,
    MB_COMM_PSTATE_INIT_COMPLETE,
    MB_COMM_PSTATE_CONNECTED,
    MB_COMM_PSTATE_DISCONNECTED,
    MB_COMM_PSTATE_KICKED,
    MB_COMM_PSTATE_REQ_ACCEPTED,
    MB_COMM_PSTATE_SEND_PROCEED,
    MB_COMM_PSTATE_SEND_COMPLETE,
    MB_COMM_PSTATE_BOOT_REQUEST,
    MB_COMM_PSTATE_BOOT_STARTABLE,
    MB_COMM_PSTATE_REQUESTED,
    MB_COMM_PSTATE_MEMBER_FULL,
    MB_COMM_PSTATE_END,
    MB_COMM_PSTATE_ERROR,
    MB_COMM_PSTATE_WAIT_TO_SEND,

    /*  内部使用の列挙値です。
       この値の状態には遷移しません。 */
    MB_COMM_PSTATE_WM_EVENT = 0x80000000
}
MBCommPState;


/* 子機からの接続リクエストに対する応答 */
typedef enum
{
    MB_COMM_RESPONSE_REQUEST_KICK,     /* 子機からのエントリー要求を拒否する． */
    MB_COMM_RESPONSE_REQUEST_ACCEPT,   /* 子機からのエントリー要求を受け入れる． */
    MB_COMM_RESPONSE_REQUEST_DOWNLOAD, /* 子機へダウンロード開始を通知する． */
    MB_COMM_RESPONSE_REQUEST_BOOT      /* 子機へブート開始を通知する． */
}
MBCommResponseRequestType;


/*  親機イベント通知コールバック型 */
typedef void (*MBCommPStateCallback) (u16 child_aid, u32 status, void *arg);


typedef struct
{
    u16     errcode;
}
MBErrorStatus;

typedef enum
{
    /* 成功を示す値 */
    MB_ERRCODE_SUCCESS = 0,

    MB_ERRCODE_INVALID_PARAM,          /* 引数エラー */
    MB_ERRCODE_INVALID_STATE,          /* 呼び出し状態の条件不整合 */

    /* 以下のエラーは子機専用． */
    MB_ERRCODE_INVALID_DLFILEINFO,     /* ダウンロード情報が不正 */
    MB_ERRCODE_INVALID_BLOCK_NO,       /* 受信したブロック番号が不正 */
    MB_ERRCODE_INVALID_BLOCK_NUM,      /* 受信ファイルのブロック数が不正 */
    MB_ERRCODE_INVALID_FILE,           /* 要求していないファイルのブロックを受信した */
    MB_ERRCODE_INVALID_RECV_ADDR,      /* 受信アドレスが不正 */

    /* 以下のエラーはWMのエラーに起因． */
    MB_ERRCODE_WM_FAILURE,             /* WM のコールバックにおけるエラー */

    /** 以降は通信継続不可なもの (WM の再初期化が必要) **/
    MB_ERRCODE_FATAL,

    MB_ERRCODE_MAX
}
MBErrCode;

/* ---------------------------------------------------------------------

        構造体

   ---------------------------------------------------------------------*/

/*
 * マルチブートゲーム登録情報構造体.
 */
typedef struct
{
    /* プログラムバイナリのファイルパス */
    const char *romFilePathp;
    /* ゲーム名の文字列を指すポインタ */
    u16    *gameNamep;
    /* ゲーム内容の説明文文字列を指すポインタ */
    u16    *gameIntroductionp;
    /* アイコンキャラデータのファイルパス */
    const char *iconCharPathp;
    /* アイコンパレットデータのファイルパス */
    const char *iconPalettePathp;
    /* ゲームグループID */
    u32     ggid;
    /* 最大プレイ人数 */
    u8      maxPlayerNum;
    u8      pad[3];
    /* ユーザ定義の拡張パラメータ (32BYTE) */
    u8      userParam[MB_DOWNLOAD_PARAMETER_SIZE];
}
MBGameRegistry;


/*
 * アイコンデータ構造体.
 * アイコンは 16色パレット + 32dot * 32dot.
 */
typedef struct
{
    /* パレットデータ (32 BYTE) */
    u16     palette[MB_ICON_COLOR_NUM];
    u16     data[MB_ICON_DATA_SIZE / 2];
    /* キャラクタデータ (512 BUYTE) */
}
MBIconInfo;                            /* 544byte */


/*
 * ユーザー情報構造体.
 */
typedef struct
{
    /* 好きな色データ (色セット番号) */
    u8      favoriteColor:4;
    /* プレイヤー番号（親機はNo.0、子機はNo.1-15） */
    u8      playerNo:4;
    /* ニックネーム長 */
    u8      nameLength;
    /* ニックネーム */
    u16     name[MB_USER_NAME_LENGTH];
}
MBUserInfo;                            /* 22byte */


/*
 * マルチブート親機情報.
 * (WMbssDescからgameInfo[128]を除いたもの)
 */
typedef struct
{
    u16     length;                    // 2
    u16     rssi;                      // 4
    u16     bssid[3];                  // 10
    u16     ssidLength;                // 12
    u8      ssid[32];                  // 44
    u16     capaInfo;                  // 46
    struct
    {
        u16     basic;                 // 48
        u16     support;               // 50
    }
    rateSet;
    u16     beaconPeriod;              // 52
    u16     dtimPeriod;                // 54
    u16     channel;                   // 56
    u16     cfpPeriod;                 // 58
    u16     cfpMaxDuration;            // 60
}
MBParentBssDesc;


/* マルチブート子機にとっての親機情報を保持する構造体 */
typedef struct
{
    u16     boot_type;
    MBParentBssDesc parent_bss_desc;
}
MBParam;


/* 子機からのエントリーリクエスト時に受信するデータ型 */
typedef struct
{
    u32     ggid;                      //  4B
    MBUserInfo userinfo;               // 22B
    u16     version;                   //  2B
    u8      fileid;                    //  1B
    u8      pad[3];                    // 計 29B
}
MBCommRequestData;


/* ---------------------------------------------------------------------

        マルチブートライブラリ(MB)API - 親機用

   ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MB_Init

  Description:  MB ライブラリ初期化

  Arguments:    work   - MBの内部状態管理のために割り当てるメモリへのポインタ
                         このサイズは MB_SYSTEM_BUF_SIZE バイト以上である必要
                         があります。また、動的確保を行う場合、
                         MB_GetParentSystemBufSize関数にて、サイズを取得することも
                         できます。
                         割り当てたメモリはMB_End関数を呼ぶまで内部で使用され
                         ます。

                user   - ユーザデータが格納された構造体のポインタ
                         このポインタが指す内容はMB_Init関数の中でのみ参照され
                         ます。

                ggid   - GGIDを指定します。

                tgid   - TGIDの値 (値はWMのガイドラインに則った決め方で生成した
                         ものを使用してください。)
                         MB_TGID_AUTO を指定すると、内部で自動的に適切な値が
                         設定されます。

                dma    - MBの内部処理のために割り当てるDMAチャンネル
                         このDMAチャンネルはMB_End関数を呼ぶまで内部で使用され
                         ます。 
  
  Returns:      初期化が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_Init(void *work, const MBUserInfo *user, u32 ggid, u32 tgid, u32 dma);


/*---------------------------------------------------------------------------*
  Name:         MB_GetParentSystemBufSize

  Description:  MBで使用するワークメモリのサイズを取得

  Arguments:    None.
  
  Returns:      MBで使用するワークメモリのサイズ
 *---------------------------------------------------------------------------*/
int     MB_GetParentSystemBufSize(void);


/*---------------------------------------------------------------------------*
  Name:         MB_SetParentCommSize

  Description:  MB 通信に使用する親機の通信データサイズをセット。
                MB_Init を実行し、 MB_StartParent をコールする前のタイミングで
                使用してください。通信開始後は、バッファサイズの変更はできません。

  Arguments:    sendSize - 親機から各子機に送る、送信データサイズ
  
  Returns:      送受信データサイズの変更が成功すればTRUE、そうでない場合は
                FALSEを返します．
 *---------------------------------------------------------------------------*/

BOOL    MB_SetParentCommSize(u16 sendSize);

/*---------------------------------------------------------------------------*
  Name:         MB_SetParentCommParam

  Description:  MB 通信に使用する親機の通信データサイズ、最大子機接続数をセット。
                MB_Init を実行し、 MB_StartParent をコールする前のタイミングで
                使用してください。通信開始後は、バッファサイズの変更はできません。

  Arguments:    sendSize    - 親機から各子機に送る、送信データサイズ
                maxChildren - 最大子機接続数
  
  Returns:      送受信データサイズの変更が成功すればTRUE、そうでない場合は
                FALSEを返します．
 *---------------------------------------------------------------------------*/

BOOL    MB_SetParentCommParam(u16 sendSize, u16 maxChildren);

/*---------------------------------------------------------------------------*
  Name:         MB_GetTgid

  Description:  MB ライブラリにおいて、WMで使用しているTGIDを取得

  Arguments:    None.
  
  Returns:      WMで使用しているTGIDを返します．
 *---------------------------------------------------------------------------*/

u16     MB_GetTgid(void);


/*---------------------------------------------------------------------------*
  Name:         MB_End

  Description:  MBライブラリを終了

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_End(void);


/*---------------------------------------------------------------------------*
  Name:         MB_EndToIdle

  Description:  MBライブラリを終了.
                ただしWMライブラリは IDLE ステートに維持されます.

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_EndToIdle(void);

/*---------------------------------------------------------------------------*
  Name:         MB_DisconnectChild

  Description:  子機の切断

  Arguments:    aid - 切断する子機のaid 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

void    MB_DisconnectChild(u16 aid);


/*---------------------------------------------------------------------------*
  Name:         MB_StartParent

  Description:  親機パラメータ設定 & スタート

  Arguments:    channel - 親機として通信を行うチャンネル 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_StartParent(int channel);


/*---------------------------------------------------------------------------*
  Name:         MB_StartParentFromIdle

  Description:  親機パラメータ設定 & スタート.
                ただしWMライブラリが IDLE ステートであることが必要です.

  Arguments:    channel - 親機として通信を行うチャンネル 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_StartParentFromIdle(int channel);

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildrenNumber

  Description:  現在エントリーしている子の総数を取得

  Arguments:    None.

  Returns:      現在親機が認識している子機の総数を返します。
 *---------------------------------------------------------------------------*/

u8      MB_CommGetChildrenNumber(void);


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildUser

  Description:  指定 aid の子の情報を取得

  Arguments:    child_aid - 状態を取得する子機を指す aid 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

const MBUserInfo *MB_CommGetChildUser(u16 child_aid);


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetParentState

  Description:  指定 aid の子機に対する親機状態を取得

  Arguments:    child_aid - 状態を取得する子機を指す aid 
  
  Returns:      MBCommPStateで示されるいずれかの状態を返します。
 *---------------------------------------------------------------------------*/

int     MB_CommGetParentState(u16 child);


/*---------------------------------------------------------------------------*
  Name:         MB_CommSetParentStateCallback

  Description:  親機状態通知コールバックを設定

  Arguments:    callback - 親機状態を通知させるコールバック関数へのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_CommSetParentStateCallback(MBCommPStateCallback callback);


/*---------------------------------------------------------------------------*
  Name:         MB_GetSegmentLength

  Description:  指定されたバイナリファイルに必要なセグメントバッファ長を取得

  Arguments:    file - バイナリファイルを指す FSFile 構造体
  
  Returns:      正常にサイズを取得できれば正の値を, そうでなければ 0
 *---------------------------------------------------------------------------*/

u32     MB_GetSegmentLength(FSFile *file);


/*---------------------------------------------------------------------------*
  Name:         MB_ReadSegment

  Description:  指定されたバイナリファイルから必要なセグメントデータを読み出し.

  Arguments:    file - バイナリファイルを指す FSFile 構造体
                buf  - セグメントデータを読み出すバッファ
                len  - buf のサイズ
  
  Returns:      正常にセグメントデータを読み出せれば TRUE, そうでなければ FALSE
 *---------------------------------------------------------------------------*/

BOOL    MB_ReadSegment(FSFile *file, void *buf, u32 len);


/*---------------------------------------------------------------------------*
  Name:         MB_RegisterFile

  Description:  指定したファイルをダウンロードリストへ登録

  Arguments:    game_reg - 登録するプログラムの情報が格納された
                           MBGameRegistry構造体へのポインタ 
                buf      - 抽出したセグメント情報が格納されたメモリへのポインタ
                           このセグメント情報はMB_ReadSegment関数で取得します。
  
  Returns:      ファイルを正しく登録できた場合はTRUE、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

BOOL    MB_RegisterFile(const MBGameRegistry *game_reg, const void *buf);


/*---------------------------------------------------------------------------*
  Name:         MB_UnregisterFile

  Description:  指定したファイルをダウンロードリストから削除

  Arguments:    game_reg - 削除するプログラムの情報が格納された
                           MBGameRegistry構造体へのポインタ 

  Returns:      ファイルを正しく削除できた場合はTRUE、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

void   *MB_UnregisterFile(const MBGameRegistry *game_reg);


/*---------------------------------------------------------------------------*
  Name:         MB_CommResponseRequest

  Description:  子機からの接続リクエストに対する応答を指定

  Arguments:    child_aid - 応答を送信する子機を指す aid
                ack       - 子機への応答結果を指定する
                            MBCommResponseRequestType型の列挙値

  Returns:      指定した子機が接続応答待ち状態であり
                指定した応答種類が正当なものであればTRUEを、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

BOOL    MB_CommResponseRequest(u16 child_aid, MBCommResponseRequestType ack);


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartSending

  Description:  エントリー済みの指定 aid の子機へ、ブートイメージ送信を開始する

  Arguments:    child_aid - ブートイメージ送信を開始する子機を指す aid

  Returns:      指定子機がエントリーしていればTRUEを、
                そうでないならFALSEを返します。
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommStartSending(u16 child_aid)
{
    return MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_DOWNLOAD);
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartSendingAll

  Description:  エントリーしてきた子機へ、ブートイメージ送信を開始する

  Arguments:    None.

  Returns:      エントリーしている子機が存在していればTRUEを、
                そうでないならFALSEを返します。
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommStartSendingAll(void)
{
    u8      child, num;

    for (num = 0, child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (TRUE == MB_CommStartSending(child))
        {
            num++;
        }
    }

    if (num == 0)
        return FALSE;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommIsBootable

  Description:  ブート可能かを判定

  Arguments:    child_aid - 対象子機のAID

  Returns:      yes - TRUE  no - FALSE
 *---------------------------------------------------------------------------*/

BOOL    MB_CommIsBootable(u16 child_aid);


/*---------------------------------------------------------------------------*
  Name:         MB_CommBootRequest

  Description:  ダウンロード済みの指定した子機に対してブート要求を送信する

  Arguments:    child_aid - 対象子機のAID

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommBootRequest(u16 child_aid)
{
    return MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_BOOT);
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommBootRequestAll

  Description:  ダウンロード済み子機に対してブート要求を送信する

  Arguments:    None.

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommBootRequestAll(void)
{
    u8      child, num;

    for (num = 0, child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (TRUE == MB_CommBootRequest(child))
        {
            num++;
        }
    }

    if (num == 0)
        return FALSE;

    return TRUE;
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

u16     MB_GetGameEntryBitmap(const MBGameRegistry *game_reg);


/* ---------------------------------------------------------------------

        マルチブートライブラリ(MB)API - ブート後子機用

   ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootParam

  Description:  マルチブート子機にとっての親機情報が保持されたポインタを取得

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static inline const MBParam *MB_GetMultiBootParam(void)
{
    return (const MBParam *)HW_WM_BOOT_BUF;
}


/*---------------------------------------------------------------------------*
  Name:         MB_IsMultiBootChild

  Description:  マルチブート子機かどうか判定
                TRUE であれば MB_GetMultiBootParentBssDesc() が有効

  Arguments:    None.

  Returns:      マルチブート子機であれば TRUE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_IsMultiBootChild(void)
{
    return MB_GetMultiBootParam()->boot_type == MB_TYPE_MULTIBOOT;
}


/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootParentBssDesc

  Description:  マルチブート子機であれば、親機情報へのポインタを返す

  Arguments:    None.

  Returns:      マルチブート子機であれば有効なポインタ、そうでなければ NULL
 *---------------------------------------------------------------------------*/

static inline const MBParentBssDesc *MB_GetMultiBootParentBssDesc(void)
{
    return MB_IsMultiBootChild()? &MB_GetMultiBootParam()->parent_bss_desc : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootDownloadParameter

  Description:  マルチブート子機であれば、ダウンロード時に指定された
                ユーザ定義の拡張パラメータを返す

  Arguments:    None.

  Returns:      マルチブート子機であれば有効なポインタ、そうでなければ NULL
 *---------------------------------------------------------------------------*/

static inline const u8 *MB_GetMultiBootDownloadParameter(void)
{
    return MB_IsMultiBootChild()? (const u8 *)HW_DOWNLOAD_PARAMETER : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadMultiBootParentBssDesc

  Description:  マルチブート親機から引き継いだ情報をもとに、
                WM_StartConnect 関数に用いるための、WMBssDesc 構造体の情報を
                セットする

  Arguments:    p_desc            pointer to destination WMBssDesc
                                    (must be aligned 32-bytes)
                parent_max_size   max packet length of parent in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                child_max_size    max packet length of child in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                ks_flag           if use key-sharing mode, TRUE.
                                    (must be equal to parent's WMParentParam!)
                cs_flag           if use continuous mode, TRUE.
                                    (must be equal to parent's WMParentParam!)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc,
                                      u16 parent_max_size, u16 child_max_size, BOOL ks_flag,
                                      BOOL cs_flag);

/*---------------------------------------------------------------------------*
  Name:         MB_SetLifeTime

  Description:  MB のワイヤレス駆動に対してライフタイムを明示的に指定します.
                デフォルトでは ( 0xFFFF, 40, 0xFFFF, 40 ) になっています.

  Arguments:    tableNumber     ライフタイムを設定するCAMテーブル番号
                camLifeTime     CAMテーブルのライフタイム
                frameLifeTime   フレーム単位でのライフタイム
                mpLifeTime      MP通信のライフタイム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_SetLifeTime(u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime);

/*---------------------------------------------------------------------------*
  Name:         MB_SetPowerSaveMode

  Description:  省電力モードを設定します.
                省電力モードは, デフォルトで有効となっています.
                この関数は電力消費を考慮しなくてよい場面で安定に駆動する
                ためのオプションであり, 電源につながれていることを保証
                された動作環境でない限り, 通常のゲームアプリケーションが
                これを使用すべきではありません.

  Arguments:    enable           有効にするならば TRUE, 無効にするならば FALSE.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_SetPowerSaveMode(BOOL enable);

/*---------------------------------------------------------------------------*/
/* internal functions */

/*---------------------------------------------------------------------------*
  Name:         MBi_AdjustSegmentMapForCloneboot

  Description:  クローンブート用の.parentセクション配置を調整。
                従来はARM9.static[00005000-00007000]に固定配置されていたが
                TWL-SDKの機能拡張に伴ってcrt0.oのサイズが増加したため
                必ずしもこの位置に存在するとは限らなくなってしまった。
                そのため現状ではHYBRIDアプリのクローンブートは不可能だが、
                この関数を手動で呼び出しつつemuchild.exeの引数も変更すれば
                無保証ながら試験的に実行することができる。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_AdjustSegmentMapForCloneboot();


/*****************************************************************************/
/* obsolete interfaces */

#define MB_StartParentEx(channel)    SDK_ERR("MB_StartParentEx() is discontinued. please use MB_StartParentFromIdle()\n")
#define MB_EndEx()    SDK_ERR("MB_EndEx() is discontinued. please use MB_EndToIdle()\n")



#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // NITRO_MB_MB_H_
