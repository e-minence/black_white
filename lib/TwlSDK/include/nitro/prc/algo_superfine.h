/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PRC - include
  File:     prc/algo_superfine.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PRC_ALGO_SUPERFINE_H_
#define NITRO_PRC_ALGO_SUPERFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/prc/types.h>
#include <nitro/prc/common.h>
#include <nitro/prc/resample.h>
#include <nitro/prc/algo_common.h>

/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

#define PRC_LENGTH_FILTER_DEFAULT_THRESHOLD_SUPERFINE    normalizeSize
#define PRC_LENGTH_FILTER_DEFAULT_RATIO_SUPERFINE        3

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Common PRCPrototypeDBParam_Superfine;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Common PRCInputPatternParam_Superfine;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Common PRCInputPattern_Superfine;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Common PRCPrototypeDB_Superfine;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef struct PRCRecognizeParam_Superfine
{
    int     lengthFilterThreshold;
    int     lengthFilterRatio;
}
PRCRecognizeParam_Superfine;



/*---------------------------------------------------------------------------*
  Name:         PRC_Init_Superfine

  Description:  図形認識 API を初期化します.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Init_Superfine(void)
{
    PRC_Init_Common();
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSize_Superfine

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetPrototypeDBBufferSize_Superfine(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Common(prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSizeEx_Superfine

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧
                kindMask                種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param                   見本DBの展開に関するパラメータ

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetPrototypeDBBufferSizeEx_Superfine(const PRCPrototypeList *prototypeList,
                                         u32 kindMask,
                                         BOOL ignoreDisabledEntries,
                                         const PRCPrototypeDBParam_Superfine *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Common(prototypeList, kindMask, ignoreDisabledEntries,
                                                 param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDB_Superfine

  Description:  PRCPrototypeDB 構造体を作成します.
                buffer には PRC_GetPrototypeDBBufferSize が返すサイズ以上の
                バッファ領域が設定されている必要があります.

  Arguments:    protoDB         初期化する見本DB構造体.
                buffer          見本DBの展開に使用するメモリ領域へのポインタ.
                                (メモリ領域サイズ>=PRC_GetPrototypeDBBufferSize の返り値)
                prototypeList   見本パターンの一覧

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitPrototypeDB_Superfine(PRCPrototypeDB_Superfine *protoDB,
                              void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Common(protoDB, buffer, prototypeList);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDBEx_Superfine

  Description:  PRCPrototypeDB 構造体を作成します.
                buffer には PRC_GetPrototypeDBBufferSize が返すサイズ以上の
                バッファ領域が設定されている必要があります.
                param によって、展開時のパラメータが指定できます.

  Arguments:    protoDB         初期化する見本DB構造体.
                buffer          見本DBの展開に使用するメモリ領域へのポインタ.
                                (メモリ領域サイズ>=PRC_GetPrototypeDBBufferSize の返り値)
                prototypeList   見本パターンの一覧
                kindMask        種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param           見本DBの展開に関するパラメータ

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitPrototypeDBEx_Superfine(PRCPrototypeDB_Superfine *protoDB,
                                void *buffer,
                                const PRCPrototypeList *prototypeList,
                                u32 kindMask,
                                BOOL ignoreDisabledEntries,
                                const PRCPrototypeDBParam_Superfine *param)
{
    return PRC_InitPrototypeDBEx_Common(protoDB, buffer, prototypeList, kindMask,
                                        ignoreDisabledEntries, param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternBufferSize_Superfine

  Description:  パターンデータを比較用に展開するのに必要なワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限

  Returns:      パターンを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32 PRC_GetInputPatternBufferSize_Superfine(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Common(maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPattern_Superfine

  Description:  PRCInputPattern 構造体を作成します.

  Arguments:    pattern             初期化するパターン構造体.
                buffer              パターンの展開に使用するメモリ領域へのポインタ.
                                    (領域サイズ>=PRC_GetInputPatternBufferSize の返り値)
                strokes             整形前の生入力座標値.
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitInputPattern_Superfine(PRCInputPattern_Superfine *pattern,
                               void *buffer,
                               const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPatternEx_Superfine

  Description:  PRCInputPattern 構造体を作成します.
                param によって、入力パターンの解釈に関するパラメータを指定できます.

  Arguments:    pattern             初期化するパターン構造体.
                buffer              パターンの展開に使用するメモリ領域へのポインタ.
                                    (領域サイズ>=PRC_GetInputPatternBufferSize の返り値)
                strokes             整形前の生入力座標値.
                param               入力パターンの解釈に関するパラメータ

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitInputPatternEx_Superfine(PRCInputPattern_Superfine *pattern,
                                 void *buffer,
                                 const PRCStrokes *strokes,
                                 int maxPointCount,
                                 int maxStrokeCount, const PRCInputPatternParam_Superfine *param)
{
    return PRC_InitInputPatternEx_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                         param);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Superfine

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.
                param           認識処理に関するパラメータ.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
u32
 
 
PRC_GetRecognitionBufferSizeEx_Superfine(int maxPointCount,
                                         int maxStrokeCount,
                                         const PRCPrototypeDB_Superfine *protoDB,
                                         const PRCRecognizeParam_Superfine *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSize_Superfine

  Description:  認識アルゴリズムが必要とするワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限
                protoDB         見本DB.

  Returns:      認識アルゴリズムが必要とするメモリ量.
 *---------------------------------------------------------------------------*/
static inline u32
PRC_GetRecognitionBufferSize_Superfine(int maxPointCount,
                                       int maxStrokeCount, const PRCPrototypeDB_Superfine *protoDB)
{
    return PRC_GetRecognitionBufferSizeEx_Superfine(maxPointCount, maxStrokeCount, protoDB, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Superfine

  Description:  見本DBの特定の kind のエントリと入力パターンを比較して認識し,
                結果の上位 numRanking 位を返します.

  Arguments:    resultEntries   認識結果へのポインタを入れる配列へのポインタ.
                                求める個数以下しか認識できなかった場合、
                                残りは NULL で埋められます.
                resultScores    認識結果のスコアの配列へのポインタ.
                numRanking      result* に返す個数
                buffer          認識アルゴリズムが使用するメモリ領域へのポインタ.
                                (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input           入力パターン.
                protoDB         見本DB.
                kindMask        各見本DBエントリの kind 値との論理積をとり,
                                非ゼロであれば有効とみなします.
                param           認識処理に関するパラメータ.

  Returns:      比較された見本DB上のパターン数.
 *---------------------------------------------------------------------------*/
int
 
 
PRC_GetRecognizedEntriesEx_Superfine(PRCPrototypeEntry **resultEntries,
                                     fx32 *resultScores,
                                     int numRanking,
                                     void *buffer,
                                     const PRCInputPattern_Superfine *input,
                                     const PRCPrototypeDB_Superfine *protoDB,
                                     u32 kindMask, const PRCRecognizeParam_Superfine *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntries_Superfine

  Description:  見本DBと入力パターンを比較して認識し,
                結果の上位 numRanking 位を返します.

  Arguments:    resultEntries   認識結果へのポインタを入れる配列へのポインタ.
                                求める個数以下しか認識できなかった場合、
                                残りは NULL で埋められます.
                resultScores    認識結果のスコアの配列へのポインタ.
                numRanking      result* に返してほしい個数
                buffer          認識アルゴリズムが使用するメモリ領域へのポインタ.
                                (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input           入力パターン.
                protoDB         見本DB.

  Returns:      比較された見本DB上のパターン数.
 *---------------------------------------------------------------------------*/
static inline int
PRC_GetRecognizedEntries_Superfine(PRCPrototypeEntry **resultEntries,
                                   fx32 *resultScores,
                                   int numRanking,
                                   void *buffer,
                                   const PRCInputPattern_Superfine *input,
                                   const PRCPrototypeDB_Superfine *protoDB)
{
    return PRC_GetRecognizedEntriesEx_Superfine(resultEntries, resultScores, numRanking, buffer,
                                                input, protoDB, PRC_KIND_ALL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntryEx_Superfine

  Description:  見本DBの特定の kind のエントリと入力パターンを比較して認識します.

  Arguments:    resultEntry 認識された見本DBエントリへのポインタを受け取るポインタ.
                            認識できなかった場合、NULL が入ります.
                buffer      認識アルゴリズムが使用するメモリ領域へのポインタ.
                            (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input       入力パターン.
                protoDB     見本DB.
                kindMask    各見本DBエントリの kind 値との論理積をとり,
                            非ゼロであれば有効とみなします.
                param       認識処理に関するパラメータ.

  Returns:      スコア値.
 *---------------------------------------------------------------------------*/
static inline fx32
PRC_GetRecognizedEntryEx_Superfine(PRCPrototypeEntry **resultEntry,
                                   void *buffer,
                                   const PRCInputPattern_Superfine *input,
                                   const PRCPrototypeDB_Superfine *protoDB,
                                   u32 kindMask, const PRCRecognizeParam_Superfine *param)
{
    fx32    score;
    (void)PRC_GetRecognizedEntriesEx_Superfine(resultEntry, &score, 1, buffer, input, protoDB,
                                               kindMask, param);
    return score;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntry_Superfine

  Description:  見本DBと入力パターンを比較して認識します.

  Arguments:    resultEntry 認識された見本DBエントリへのポインタを受け取るポインタ.
                            認識できなかった場合、NULL が入ります.
                buffer      認識アルゴリズムが使用するメモリ領域へのポインタ.
                            (領域サイズ>=PRC_GetRecognitionBufferSize の返り値)
                input       入力パターン.
                protoDB     見本DB.

  Returns:      スコア値.
 *---------------------------------------------------------------------------*/
static inline fx32
PRC_GetRecognizedEntry_Superfine(PRCPrototypeEntry **resultEntry,
                                 void *buffer,
                                 const PRCInputPattern_Superfine *input,
                                 const PRCPrototypeDB_Superfine *protoDB)
{
    return PRC_GetRecognizedEntryEx_Superfine(resultEntry, buffer, input, protoDB, PRC_KIND_ALL,
                                              NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternStrokes_Superfine

  Description:  PRCInputPattern 構造体から点列データを得ます.

  Arguments:    strokes         得られた点列データ.
                                書き換えてはいけません.
                input           入力パターン.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
PRC_GetInputPatternStrokes_Superfine(PRCStrokes *strokes, const PRCInputPattern_Superfine *input)
{
    PRC_GetInputPatternStrokes_Common(strokes, input);
}


/*===========================================================================*
  Inline Functions
 *===========================================================================*/




/*===========================================================================*
  図形認識 API の標準関数として設定
 *===========================================================================*/
#ifndef PRC_DEFAULT_ALGORITHM
#define PRC_DEFAULT_ALGORITHM "Superfine"

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDBParam_Superfine PRCPrototypeDBParam;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCInputPatternParam_Superfine PRCInputPatternParam;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef PRCInputPattern_Superfine PRCInputPattern;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef PRCPrototypeDB_Superfine PRCPrototypeDB;

/*---------------------------------------------------------------------------*
  認識処理に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef PRCRecognizeParam_Superfine PRCRecognizeParam;


/*---------------------------------------------------------------------------*
  関数
 *---------------------------------------------------------------------------*/
static inline void PRC_Init(void)
{
    PRC_Init_Superfine();
}

static inline u32 PRC_GetPrototypeDBBufferSize(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSize_Superfine(prototypeList);
}

static inline u32
PRC_GetPrototypeDBBufferSizeEx(const PRCPrototypeList *prototypeList,
                               u32 kindMask,
                               BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_GetPrototypeDBBufferSizeEx_Superfine(prototypeList, kindMask, ignoreDisabledEntries,
                                                    param);
}

static inline BOOL
PRC_InitPrototypeDB(PRCPrototypeDB *protoDB, void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDB_Superfine(protoDB, buffer, prototypeList);
}

static inline BOOL
PRC_InitPrototypeDBEx(PRCPrototypeDB *protoDB,
                      void *buffer,
                      const PRCPrototypeList *prototypeList,
                      u32 kindMask, BOOL ignoreDisabledEntries, const PRCPrototypeDBParam *param)
{
    return PRC_InitPrototypeDBEx_Superfine(protoDB, buffer, prototypeList, kindMask,
                                           ignoreDisabledEntries, param);
}

static inline u32 PRC_GetInputPatternBufferSize(int maxPointCount, int maxStrokeCount)
{
    return PRC_GetInputPatternBufferSize_Superfine(maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPattern(PRCInputPattern *pattern,
                     void *buffer, const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPattern_Superfine(pattern, buffer, strokes, maxPointCount, maxStrokeCount);
}

static inline BOOL
PRC_InitInputPatternEx(PRCInputPattern *pattern,
                       void *buffer,
                       const PRCStrokes *strokes,
                       int maxPointCount, int maxStrokeCount, const PRCInputPatternParam *param)
{
    return PRC_InitInputPatternEx_Superfine(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                            param);
}

static inline u32
PRC_GetRecognitionBufferSize(int maxPointCount,
                             int maxStrokeCount, const PRCPrototypeDB_Superfine *protoDB)
{
    return PRC_GetRecognitionBufferSize_Superfine(maxPointCount, maxStrokeCount, protoDB);
}

static inline u32
PRC_GetRecognitionBufferSizeEx(int maxPointCount,
                               int maxStrokeCount,
                               const PRCPrototypeDB_Superfine *protoDB,
                               const PRCRecognizeParam_Superfine *param)
{
    return PRC_GetRecognitionBufferSizeEx_Superfine(maxPointCount, maxStrokeCount, protoDB, param);
}

static inline fx32
PRC_GetRecognizedEntry(PRCPrototypeEntry **resultEntry,
                       void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntry_Superfine(resultEntry, buffer, input, protoDB);
}

static inline fx32
PRC_GetRecognizedEntryEx(PRCPrototypeEntry **resultEntry,
                         void *buffer,
                         const PRCInputPattern *input,
                         const PRCPrototypeDB *protoDB,
                         u32 kindMask, const PRCRecognizeParam *param)
{
    return PRC_GetRecognizedEntryEx_Superfine(resultEntry, buffer, input, protoDB, kindMask, param);
}

static inline int
PRC_GetRecognizedEntries(PRCPrototypeEntry **resultEntries,
                         fx32 *resultScores,
                         int numRanking,
                         void *buffer, const PRCInputPattern *input, const PRCPrototypeDB *protoDB)
{
    return PRC_GetRecognizedEntries_Superfine(resultEntries, resultScores, numRanking, buffer,
                                              input, protoDB);
}

static inline int
PRC_GetRecognizedEntriesEx(PRCPrototypeEntry **resultEntries,
                           fx32 *resultScores,
                           int numRanking,
                           void *buffer,
                           const PRCInputPattern *input,
                           const PRCPrototypeDB *protoDB,
                           u32 kindMask, const PRCRecognizeParam *param)
{
    return PRC_GetRecognizedEntriesEx_Superfine(resultEntries, resultScores, numRanking, buffer,
                                                input, protoDB, kindMask, param);
}

static inline void PRC_GetInputPatternStrokes(PRCStrokes *strokes, const PRCInputPattern *input)
{
    PRC_GetInputPatternStrokes_Superfine(strokes, input);
}

#endif // PRC_DEFAULT_ALGORITHM




#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PRC_ALGO_SUPERFINE_H_ */
#endif
