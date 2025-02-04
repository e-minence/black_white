/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PRC - include
  File:     prc/algo_common.h

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

#ifndef NITRO_PRC_ALGO_COMMON_H_
#define NITRO_PRC_ALGO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/prc/types.h>
#include <nitro/prc/common.h>


/*===========================================================================*
  Macros
 *===========================================================================*/

//--- Get*BufferSize 用
#define PRCi_ARRAY_SIZE(type, size, dummy) \
    sizeof(type) * (size)

//--- Set*BufferInfo 用
#define PRCi_ALLOC_ARRAY(dst, type, size, base, offset) \
    (dst) = (type*)((u8*)(base) + (offset)); \
    (offset) += sizeof(type) * (size)


/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

#define PRC_RESAMPLE_DEFAULT_METHOD_COMMON PRC_RESAMPLE_METHOD_RECURSIVE
#define PRC_RESAMPLE_DEFAULT_THRESHOLD_COMMON 5

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  見本DBの展開に関するパラメータ
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeDBParam_Common
{
    int     dummy;
}
PRCPrototypeDBParam_Common;

/*---------------------------------------------------------------------------*
  入力パターンの解釈に関するパラメータ
    normalizeSize       正規化サイズ.
    resampleMethod      正規化に用いられるアルゴリズムの種類.
    resampleThreshold   正規化後に行われるリサンプルの閾値.
 *---------------------------------------------------------------------------*/
typedef struct PRCInputPatternParam_Common
{
    union
    {
        int     normalizeSize;
        int     regularizeSize;        // 旧名
    };
    PRCResampleMethod resampleMethod;
    int     resampleThreshold;
}
PRCInputPatternParam_Common;

/*---------------------------------------------------------------------------*
  見本DB由来および入力由来の認識パターンの共通部
 *---------------------------------------------------------------------------*/
typedef struct PRCiPatternData_Common
{
    u16     strokeCount;               // entry と重複情報
    u16     pointCount;                // entry と重複情報
    const PRCPoint *pointArray;
    int    *strokes;
    int    *strokeSizes;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16            *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    fx32   *strokeLengths;
    fx16   *strokeRatios;
    fx32    wholeLength;
    PRCBoundingBox *strokeBoundingBoxes;
    PRCBoundingBox wholeBoundingBox;
}
PRCiPatternData_Common;

/*---------------------------------------------------------------------------*
  入力時に動的にパターンを構成するための Buffer の情報
  各認識アルゴリズムはこれを元に拡張している可能性があるので、
  変更の際には各アルゴリズムの 〜BufferInfo_* を要確認
 *---------------------------------------------------------------------------*/
typedef struct PRCiPatternBufferInfo_Common
{
    PRCPoint *pointArray;
    int    *strokes;
    int    *strokeSizes;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16            *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    fx32   *strokeLengths;
    fx16   *strokeRatios;
    PRCBoundingBox *strokeBoundingBoxes;
}
PRCiPatternBufferInfo_Common;

/*---------------------------------------------------------------------------*
  見本DB由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef struct PRCiPrototypeEntry_Common
{
    PRCiPatternData_Common data;
    const PRCPrototypeEntry *entry;
}
PRCiPrototypeEntry_Common;

/*---------------------------------------------------------------------------*
  入力由来の認識パターン
 *---------------------------------------------------------------------------*/
typedef struct PRCInputPattern_Common
{
    PRCiPatternData_Common data;
    union
    {
        int     normalizeSize;
        int     regularizeSize;        // 旧名
    };

    void   *buffer;
    u32     bufferSize;
}
PRCInputPattern_Common;

/*---------------------------------------------------------------------------*
  入力時に動的に見本DBを構成するための Buffer の情報
  各認識アルゴリズムはこれを元に拡張している可能性があるので、
  変更の際には各アルゴリズムの 〜BufferInfo_* を要確認
 *---------------------------------------------------------------------------*/
typedef struct PRCiPrototypeListBufferInfo_Common
{
    PRCiPrototypeEntry_Common *patterns;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16            *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    int    *strokeArray;
    int    *strokeSizeArray;
    fx32   *strokeLengthArray;
    fx16   *strokeRatioArray;
    PRCBoundingBox *strokeBoundingBoxArray;
}
PRCiPrototypeListBufferInfo_Common;

/*---------------------------------------------------------------------------*
  メモリに展開された見本DB
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeDB_Common
{
    PRCiPrototypeEntry_Common *patterns;
    int     patternCount;

    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16                        *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    int     wholePointCount;

    int    *strokeArray;
    int    *strokeSizeArray;
    fx32   *strokeLengthArray;
    fx16   *strokeRatioArray;
    PRCBoundingBox *strokeBoundingBoxArray;
    int     wholeStrokeCount;

    int     maxPointCount;
    int     maxStrokeCount;

    union
    {
        int     normalizeSize;
        int     regularizeSize;        // 旧名
    };

    const PRCPrototypeList *prototypeList;
    void   *buffer;
    u32     bufferSize;
}
PRCPrototypeDB_Common;


/*---------------------------------------------------------------------------*
  Name:         PRC_Init_Common

  Description:  図形認識 API を初期化します.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Init_Common(void)
{
    PRCi_Init();
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSizeEx_Common

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧
                kindMask                種類指定用マスク
                ignoreDisabledEntries   enabled フラグが FALSE の見本DBエントリは
                                        そもそも展開しないかどうか
                param                   見本DBの展開に関するパラメータ

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
u32
 
 
PRC_GetPrototypeDBBufferSizeEx_Common(const PRCPrototypeList *prototypeList,
                                      u32 kindMask,
                                      BOOL ignoreDisabledEntries,
                                      const PRCPrototypeDBParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSize_Common

  Description:  見本DBを展開するのに必要なワーク領域の大きさを計算します.

  Arguments:    prototypeList           見本パターンの一覧

  Returns:      見本DBを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
u32 static inline PRC_GetPrototypeDBBufferSize_Common(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSizeEx_Common(prototypeList, PRC_KIND_ALL, FALSE, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDBEx_Common

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
BOOL
 
 PRC_InitPrototypeDBEx_Common(PRCPrototypeDB_Common *protoDB,
                              void *buffer,
                              const PRCPrototypeList *prototypeList,
                              u32 kindMask,
                              BOOL ignoreDisabledEntries, const PRCPrototypeDBParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDB_Common

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
PRC_InitPrototypeDB_Common(PRCPrototypeDB_Common *protoDB,
                           void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDBEx_Common(protoDB, buffer, prototypeList, PRC_KIND_ALL, FALSE, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternBufferSize_Common

  Description:  パターンデータを比較用に展開するのに必要なワーク領域の大きさを
                計算します.

  Arguments:    maxPointCount   入力点数の上限(pen up marker を含む)
                maxStrokeCount  画数の上限

  Returns:      パターンを展開するのに必要なメモリ量.
 *---------------------------------------------------------------------------*/
u32     PRC_GetInputPatternBufferSize_Common(int maxPointCount, int maxStrokeCount);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPatternEx_Common

  Description:  PRCInputPattern 構造体を作成します.
                param によって、入力パターンの解釈に関するパラメータを指定できます.

  Arguments:    pattern             初期化するパターン構造体.
                buffer              パターンの展開に使用するメモリ領域へのポインタ.
                                    (領域サイズ>=PRC_GetInputPatternBufferSize の返り値)
                strokes             整形前の生入力座標値.
                maxPointCount       入力点数の上限(pen up marker を含む)
                maxStrokeCount      画数の上限
                param               入力パターンの解釈に関するパラメータ

  Returns:      作成に成功したら真.
 *---------------------------------------------------------------------------*/
BOOL
 
 PRC_InitInputPatternEx_Common(PRCInputPattern_Common *pattern,
                               void *buffer,
                               const PRCStrokes *strokes,
                               int maxPointCount,
                               int maxStrokeCount, const PRCInputPatternParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPattern_Common

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
PRC_InitInputPattern_Common(PRCInputPattern_Common *pattern,
                            void *buffer,
                            const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPatternEx_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                         NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternStrokes_Common

  Description:  PRCInputPattern 構造体から点列データを得ます.

  Arguments:    strokes         得られた点列データ.
                                書き換えてはいけません.
                input           入力パターン.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    PRC_GetInputPatternStrokes_Common(PRCStrokes *strokes, const PRCInputPattern_Common *input);


// for debug
void    PRCi_PrintPatternData_Common(PRCiPatternData_Common *data);

/*===========================================================================*
  Inline Functions
 *===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PRC_ALGO_COMMON_H_ */
#endif
