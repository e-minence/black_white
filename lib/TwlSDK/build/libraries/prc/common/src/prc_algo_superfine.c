/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PRC - 
  File:     prc_algo_superfine.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/algo_superfine.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/

static inline u32 PRCi_GetPatternLocalBufferSize_Superfine(void);

static void
PRCi_CalcStrokeDistance_Superfine(fx32 *score,
                                  fx32 *weight,
                                  const PRCiPatternData_Common *inputData,
                                  const PRCiPatternData_Common *protoData,
                                  int normalizeSize,
                                  int lengthFilterThreshold,
                                  int lengthFilterRatio, int iStroke, void *buffer);

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

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
                                         const PRCRecognizeParam_Superfine *param)
{
    u32     size = 0;

    (void)maxStrokeCount;
    (void)param;

    if (maxPointCount < protoDB->maxPointCount)
    {
        maxPointCount = protoDB->maxPointCount;
    }

    size += PRCi_ARRAY_SIZE(int, maxPointCount * maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount * maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount * maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, ((maxPointCount + 1) * (maxPointCount + 1)),);
    size += PRCi_ARRAY_SIZE(int, maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount + 2,);
    size += PRCi_ARRAY_SIZE(int, maxPointCount + 2,);
    return size;
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
                                     u32 kindMask, const PRCRecognizeParam_Superfine *param)
{
    int     i;
    const PRCiPatternData_Common *inputData;
    int     numCompared;
    int     normalizeSize;
    int     lengthFilterThreshold, lengthFilterRatio;

    SDK_ASSERT(resultEntries);
    SDK_ASSERT(resultScores);
    SDK_ASSERT(input);
    SDK_ASSERT(protoDB);
    SDK_ASSERT(numRanking > 0);

    for (i = 0; i < numRanking; i++)
    {
        resultEntries[i] = NULL;
        resultScores[i] = 0;
    }

    normalizeSize = protoDB->normalizeSize;
    if (normalizeSize < input->normalizeSize)
    {
        normalizeSize = input->normalizeSize;
    }

    if (param == NULL)
    {
        lengthFilterThreshold = PRC_LENGTH_FILTER_DEFAULT_THRESHOLD_SUPERFINE;
        lengthFilterRatio = PRC_LENGTH_FILTER_DEFAULT_RATIO_SUPERFINE;
    }
    else
    {
        lengthFilterThreshold = param->lengthFilterThreshold;
        lengthFilterRatio = param->lengthFilterRatio;
    }

    inputData = &input->data;
    numCompared = 0;

    {
        const PRCiPrototypeEntry_Common *proto;
        int     iPattern;

        proto = protoDB->patterns;

        for (iPattern = 0; iPattern < protoDB->patternCount; iPattern++, proto++)
        {
            const PRCiPatternData_Common *protoData;
            int     iStroke;
            fx32    wholeScore, wholeWeight;

            if (!proto->entry->enabled || !(proto->entry->kind & kindMask))
                continue;

            protoData = &proto->data;

            if (inputData->strokeCount != protoData->strokeCount)
                continue;

            numCompared++;
            wholeScore = 0;
            wholeWeight = 0;

            for (iStroke = 0; iStroke < inputData->strokeCount; iStroke++)
            {
                fx32    score, weight;

                score = 0;

                PRCi_CalcStrokeDistance_Superfine(&score, &weight, inputData, protoData,
                                                  normalizeSize, lengthFilterThreshold,
                                                  lengthFilterRatio, iStroke, buffer);

                wholeScore += score;
                wholeWeight += weight;
            }

            wholeScore = FX_Div(wholeScore, wholeWeight * normalizeSize * 2);   // normalizeSize*2 == doubleWidth

            if (wholeScore < 0)
                wholeScore = 0;
            if (wholeScore >= FX32_ONE)
                wholeScore = FX32_ONE;
            if (proto->entry->correction != 0)
            {
                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction)
                    + proto->entry->correction;
            }

            if (resultScores[numRanking - 1] < wholeScore)
            {
                resultScores[numRanking - 1] = wholeScore;
                resultEntries[numRanking - 1] = (PRCPrototypeEntry *)proto->entry;
                for (i = numRanking - 2; i >= 0; i--)
                {
                    if (resultScores[i] < resultScores[i + 1])
                    {
                        fx32    tmpScore;
                        PRCPrototypeEntry *tmpEntry;
                        tmpScore = resultScores[i];
                        resultScores[i] = resultScores[i + 1];
                        resultScores[i + 1] = tmpScore;
                        tmpEntry = resultEntries[i];
                        resultEntries[i] = resultEntries[i + 1];
                        resultEntries[i + 1] = tmpEntry;
                    }
                }
            }
        }
    }

    return numCompared;
}

/*===========================================================================*
  Static Functions
 *===========================================================================*/
#define PRCi_SINGLE_ANGLE_SCORE 128

static inline int CityBlockDistance(const PRCPoint *p1, const PRCPoint *p2)
{
    int     x = p1->x - p2->x;
    int     y = p1->y - p2->y;
    if (x < 0)
        x = -x;
    if (y < 0)
        y = -y;
    return (x + y);
}

static inline void
GetMixedPoint(PRCPoint *p, const PRCPoint *p1, int w1, const PRCPoint *p2, int w2)
{
    int     a, w;
    a = (p1->x * w1 + p2->x * w2);
    SDK_TASSERTMSG(a >= 0, "a < 0: (%d, %d)*%d-(%d, %d)*%d", p1->x, p1->y, w1, p2->x, p2->y, w2);
    w = w1 + w2;
    SDK_TASSERTMSG(w > 0, "w <= 0: (%d, %d)*%d-(%d, %d)*%d", p1->x, p1->y, w1, p2->x, p2->y, w2);
    {
        OSIntrMode enabled;
        enabled = OS_DisableInterrupts();

        CP_SetDiv32_32((u32)a, (u32)w);
        a = (p1->y * w1 + p2->y * w2);
        p->x = CP_GetDivResult16();
        CP_SetDiv32_32((u32)a, (u32)w);
        p->y = CP_GetDivResult16();

        (void)OS_RestoreInterrupts(enabled);
    }
}

#define PRCi_ABS(x) (((x)>=0)?(x):-(x))
//#define PRCi_ANGLE_SCORE(input, proto) ((32768-PRCi_ABS((s16)(protoAngles[(proto)]-inputAngles[(input)])))/128)
//#define PRCi_ANGLE_SCORE(input, proto) ((32768*32768-((s16)(protoAngles[(proto)]-inputAngles[(input)]))*((s16)(protoAngles[(proto)]-inputAngles[(input)])))/(128*32768))
// ↑上に行けばいくほど早くなります。精度はそれほど落ちません。
#define PRCi_ANGLE_SCORE(input, proto) ((FX_CosIdx((u16)(protoAngles[(proto)]-inputAngles[(input)]))+FX32_ONE)/(FX32_ONE*2/256))

static void
PRCi_CalcStrokeDistance_Superfine(fx32 *score,
                                  fx32 *weight,
                                  const PRCiPatternData_Common *inputData,
                                  const PRCiPatternData_Common *protoData,
                                  int normalizeSize,
                                  int lengthFilterThreshold,
                                  int lengthFilterRatio, int iStroke, void *buffer)
{
#define nMatches_(x,y) (*(nMatches + (x) * maxPointCount + (y)))
#define sumScore_(x,y) (*(sumScore + (x) * maxPointCount + (y)))
#define direction_(x,y) (*(direction + (x) * maxPointCount + (y)))
#define angleScores_(x,y) (*(angleScores + (x) * (maxPointCount+1) + (y)))

    // ストローク同士の類似度を計算 with DP matching
    int     iInput, iProto;
    int    *nMatches;                  //[STROKE_PACKED_POINT_MAX][STROKE_PACKED_POINT_MAX];
    int    *sumScore;                  //[STROKE_PACKED_POINT_MAX][STROKE_PACKED_POINT_MAX];
    int    *direction;                 //[STROKE_PACKED_POINT_MAX][STROKE_PACKED_POINT_MAX];
    int    *angleScores;               //[STROKE_PACKED_POINT_MAX+1][STROKE_PACKED_POINT_MAX+1];

    int    *inputPair;                 //[STROKE_PACKED_POINT_MAX];
    int    *inputMaxScore;             //[STROKE_PACKED_POINT_MAX];
    int    *protoPair;                 //[STROKE_PACKED_POINT_MAX];
    int    *protoMaxScore;             //[STROKE_PACKED_POINT_MAX];
    int    *inputMatch;                //[STROKE_PACKED_POINT_MAX+2];
    int    *protoMatch;                //[STROKE_PACKED_POINT_MAX+2];

    int     protoStrokeIndex, inputStrokeIndex;
    int     protoSize, inputSize;
    const PRCPoint *protoPoints;
    const PRCPoint *inputPoints;
    const u16 *protoAngles;
    const u16 *inputAngles;
    const fx16 *protoRatios;
    const fx16 *inputRatios;
    const fx32 *protoLengths;
    const fx32 *inputLengths;
    fx32    protoStrokeLength, inputStrokeLength;
    fx32    protoStrokeRatio, inputStrokeRatio;
    int     maxPointCount;
    int     doubleWidth;

    maxPointCount = inputData->pointCount;
    if (maxPointCount < protoData->pointCount)
        maxPointCount = protoData->pointCount;

#ifdef SDK_DEBUG
    {
        int     width, tmp;

        width = inputData->wholeBoundingBox.x2 - inputData->wholeBoundingBox.x1;
        tmp = inputData->wholeBoundingBox.y2 - inputData->wholeBoundingBox.y1;
        if (tmp > width)
            width = tmp;
        tmp = protoData->wholeBoundingBox.x2 - protoData->wholeBoundingBox.x1;
        if (tmp > width)
            width = tmp;
        tmp = protoData->wholeBoundingBox.y2 - protoData->wholeBoundingBox.y1;
        if (tmp > width)
            width = tmp;

        width++;
        if (width > normalizeSize)
        {
            OS_TWarning
                ("too small normalizeSize. PRCPrototypeList.normalizeSize seems too smaller than actual data.");
        }
    }
#endif
    doubleWidth = normalizeSize * 2;

    // buffer の割り当て
    {
        int     addr;
        addr = 0;
        PRCi_ALLOC_ARRAY(nMatches, int, maxPointCount * maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(sumScore, int, maxPointCount * maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(direction, int, maxPointCount * maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(angleScores,
                         int, ((maxPointCount + 1) * (maxPointCount + 1)), buffer, addr);
        PRCi_ALLOC_ARRAY(inputPair, int, maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(inputMaxScore, int, maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(protoPair, int, maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(protoMaxScore, int, maxPointCount, buffer, addr);
        PRCi_ALLOC_ARRAY(inputMatch, int, maxPointCount + 2, buffer, addr);
        PRCi_ALLOC_ARRAY(protoMatch, int, maxPointCount + 2, buffer, addr);
    }

    protoStrokeIndex = protoData->strokes[iStroke];
    inputStrokeIndex = inputData->strokes[iStroke];
    protoSize = protoData->strokeSizes[iStroke];
    inputSize = inputData->strokeSizes[iStroke];
    protoPoints = &protoData->pointArray[protoStrokeIndex];
    inputPoints = &inputData->pointArray[inputStrokeIndex];
    protoAngles = &protoData->lineSegmentAngleArray[protoStrokeIndex];
    inputAngles = &inputData->lineSegmentAngleArray[inputStrokeIndex];
    protoRatios = &protoData->lineSegmentRatioToStrokeArray[protoStrokeIndex];
    inputRatios = &inputData->lineSegmentRatioToStrokeArray[inputStrokeIndex];
    protoLengths = &protoData->lineSegmentLengthArray[protoStrokeIndex];
    inputLengths = &inputData->lineSegmentLengthArray[inputStrokeIndex];
    protoStrokeLength = protoData->strokeLengths[iStroke];
    inputStrokeLength = inputData->strokeLengths[iStroke];
    protoStrokeRatio = protoData->strokeRatios[iStroke];
    inputStrokeRatio = inputData->strokeRatios[iStroke];

    *weight = FX32_ONE;
    *score = 0;

    if (protoSize == 0 || inputSize == 0)
        return;

    // 長さがあまりにも違う線分は類似度0としてこれ以上計算しない
    if (inputStrokeLength > lengthFilterThreshold || protoStrokeLength > lengthFilterThreshold)
    {
        if (inputStrokeLength * lengthFilterRatio < protoStrokeLength
            || protoStrokeLength * lengthFilterRatio < inputStrokeLength)
        {
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
            OS_TPrintf("Skipped because of length filter %d <=> %d\n", FX_Whole(inputStrokeLength),
                      FX_Whole(protoStrokeLength));
#endif
            return;
        }
    }

    // 角度に関するスコアの angleScores[][] の事前計算
    if (protoSize == 1 || inputSize == 1)
    {
        for (iInput = 0; iInput < inputSize; iInput++)
        {
            for (iProto = 0; iProto < protoSize; iProto++)
            {
                angleScores_(iInput, iProto) = PRCi_SINGLE_ANGLE_SCORE;
            }
        }
    }
    else
    {
        angleScores_(0, 0) = PRCi_ANGLE_SCORE(1, 1);
        angleScores_(inputSize, 0) = PRCi_ANGLE_SCORE(inputSize - 1, 1);
        angleScores_(0, protoSize) = PRCi_ANGLE_SCORE(1, protoSize - 1);
        angleScores_(inputSize, protoSize) = PRCi_ANGLE_SCORE(inputSize - 1, protoSize - 1);

        for (iInput = 1; iInput < inputSize; iInput++)
        {
            angleScores_(iInput, 0) = PRCi_ANGLE_SCORE(iInput, 1);
            angleScores_(iInput, protoSize) = PRCi_ANGLE_SCORE(iInput, protoSize - 1);
        }
        for (iProto = 1; iProto < protoSize; iProto++)
        {
            angleScores_(0, iProto) = PRCi_ANGLE_SCORE(1, iProto);
            angleScores_(inputSize, iProto) = PRCi_ANGLE_SCORE(inputSize - 1, iProto);
        }

        for (iInput = 1; iInput < inputSize; iInput++)
        {
            for (iProto = 1; iProto < protoSize; iProto++)
            {
                angleScores_(iInput, iProto) = PRCi_ANGLE_SCORE(iInput, iProto);
            }
        }
    }

    // DP Matching でストローク同士の対応点を探す
    sumScore_(0, 0) =
        (doubleWidth - CityBlockDistance(&inputPoints[0], &protoPoints[0])) * angleScores_(0,
                                                                                           0) * 2;
    nMatches_(0, 0) = 1;
    for (iInput = 1; iInput < inputSize; iInput++)
    {
        sumScore_(iInput, 0) =
            (doubleWidth -
             CityBlockDistance(&inputPoints[iInput], &protoPoints[0])) * (angleScores_(iInput,
                                                                                       0) +
                                                                          angleScores_(iInput + 1,
                                                                                       1)) +
            sumScore_(iInput - 1, 0);
        nMatches_(iInput, 0) = nMatches_(iInput - 1, 0) + 1;
        direction_(iInput, 0) = 2;
    }
    for (iProto = 1; iProto < protoSize; iProto++)
    {
        sumScore_(0, iProto) =
            (doubleWidth -
             CityBlockDistance(&inputPoints[0], &protoPoints[iProto])) * (angleScores_(0,
                                                                                       iProto) +
                                                                          angleScores_(1,
                                                                                       iProto +
                                                                                       1)) +
            sumScore_(0, iProto - 1);
        nMatches_(0, iProto) = nMatches_(0, iProto - 1) + 1;
        direction_(0, iProto) = 1;
    }

    for (iInput = 1; iInput < inputSize; iInput++)
    {
        for (iProto = 1; iProto < protoSize; iProto++)
        {
            int     sum, n, localScore;
            int     sumMax, nMax, dirMax;

            localScore =
                (doubleWidth
                 - CityBlockDistance(&inputPoints[iInput], &protoPoints[iProto]))
                * (angleScores_(iInput, iProto) + angleScores_(iInput + 1, iProto + 1));

            dirMax = 0;
            sumMax = localScore + sumScore_(iInput - 1, iProto - 1);
            nMax = nMatches_(iInput - 1, iProto - 1) + 1;

            sum = localScore + sumScore_(iInput, iProto - 1);
            n = nMatches_(iInput, iProto - 1) + 1;
            if (sum * nMax > sumMax * n)
            {
                sumMax = sum;
                nMax = n;
                dirMax = 1;
            }

            sum = localScore + sumScore_(iInput - 1, iProto);
            n = nMatches_(iInput - 1, iProto) + 1;
            if (sum * nMax > sumMax * n)
            {
                sumMax = sum;
                nMax = n;
                dirMax = 2;
            }

            sumScore_(iInput, iProto) = sumMax;
            nMatches_(iInput, iProto) = nMax;
            direction_(iInput, iProto) = dirMax;
        }
    }

#ifdef PRC_DEBUG_RECOGNIZE
    {
        int     localScore, angleScore;
        iInput = inputSize - 1;
        iProto = protoSize - 1;
        while (!(iInput == 0 && iProto == 0))
        {
            int     dx, dy;
            dx = -1 + (direction_(iInput, iProto) & 1);
            dy = -1 + ((direction_(iInput, iProto) & 2) >> 1);
            localScore = sumScore_(iInput, iProto) - sumScore_(iInput + dx, iProto + dy);
            angleScore = angleScores_(iInput, iProto) + angleScores_(iInput + 1, iProto + 1);

            OS_TPrintf(" %2d <-> %2d : 0.%03d = 0.%03d * 0.%03d, average from begin: 0.%03d\n",
                      iInput, iProto, localScore / normalizeSize,
                      localScore * 512 / angleScore / normalizeSize, angleScore * 2,
                      sumScore_(iInput, iProto) / nMatches_(iInput, iProto) / normalizeSize);

            iInput += dx;
            iProto += dy;
        }
        localScore = sumScore_(iInput, iProto);
        angleScore = angleScores_(iInput, iProto) + angleScores_(iInput + 1, iProto + 1);
        OS_TPrintf(" %2d <-> %2d : 0.%03d = 0.%03d * 0.%03d\n", iInput, iProto,
                  localScore / normalizeSize, localScore * 512 / angleScore / normalizeSize,
                  angleScore * 2);
    }

    OS_TPrintf("total: %d, matches: %d\n", sumScore_(inputSize - 1, protoSize - 1),
              nMatches_(inputSize - 1, protoSize - 1));
#endif

    // 類似度計算フェーズ
    {
        int     iMatch;
        int     localScore;
        int     nMatches;
        fx32    weightedScore, totalWeight;

        // まず、確実にマッチしているペアを抽出する
        for (iInput = 0; iInput < inputSize; iInput++)
        {
            inputPair[iInput] = -1;
            inputMaxScore[iInput] = -1;
        }

        for (iProto = 0; iProto < protoSize; iProto++)
        {
            protoPair[iProto] = -1;
            protoMaxScore[iProto] = -1;
        }

        iInput = inputSize - 1;
        iProto = protoSize - 1;
        while (!(iInput == 0 && iProto == 0))
        {
            int     dx, dy;
            dx = -1 + (direction_(iInput, iProto) & 1);
            dy = -1 + ((direction_(iInput, iProto) & 2) >> 1);
            localScore = sumScore_(iInput, iProto) - sumScore_(iInput + dx, iProto + dy);

            if (inputMaxScore[iInput] < localScore)
            {
                inputPair[iInput] = iProto;
                inputMaxScore[iInput] = localScore;
            }

            if (protoMaxScore[iProto] < localScore)
            {
                protoPair[iProto] = iInput;
                protoMaxScore[iProto] = localScore;
            }

            iInput += dx;
            iProto += dy;
        }
        // iInput==0 && iProto==0 の処理
        localScore = sumScore_(iInput, iProto);
        if (inputMaxScore[iInput] < localScore)
        {
            inputPair[iInput] = iProto;
            inputMaxScore[iInput] = localScore;
        }

        if (protoMaxScore[iProto] < localScore)
        {
            protoPair[iProto] = iInput;
            protoMaxScore[iProto] = localScore;
        }


        // 双方の判定でペアとして一致したもの以外は取り除く
        for (iInput = 0; iInput < inputSize; iInput++)
        {
            int     pair = inputPair[iInput];
            if (pair >= 0)
            {
                // 相手にとっての最良のペアが自分かどうか
                if (protoPair[pair] != iInput)
                {
                    inputPair[iInput] = -1;
                }
            }
        }

        nMatches = 0;
        inputMatch[nMatches] = 0;
        protoMatch[nMatches] = 0;
        nMatches++;
        for (iInput = 0; iInput < inputSize; iInput++)
        {
            if (inputPair[iInput] >= 0)
            {
                inputMatch[nMatches] = iInput;
                protoMatch[nMatches] = inputPair[iInput];
                nMatches++;
            }
        }
        inputMatch[nMatches] = inputSize - 1;
        protoMatch[nMatches] = protoSize - 1;
        nMatches++;

#ifdef PRC_DEBUG_RECOGNIZE
        OS_TPrintf("-----\n");
        for (iMatch = 0; iMatch < nMatches; iMatch++)
        {
            OS_TPrintf(" <%d> v <%d>\n", inputMatch[iMatch], protoMatch[iMatch]);
        }
#endif
//#define PRC_DEBUG_RECOGNIZE_DEEPLY
        weightedScore = 0;
        totalWeight = 0;
        for (iMatch = 0; iMatch < nMatches - 1; iMatch++)
        {
            fx32    inputLocalLength, protoLocalLength;
            fx32    inputCurrentLength, protoCurrentLength;
            fx32    inputPrevSumRatio, protoPrevSumRatio;
            fx32    localScore;
            fx32    weight, tmp;
            fx32    inputNextRatio, protoNextRatio;
            fx32    inputOrigNextRatio, protoOrigNextRatio;
            int     loopEnd, i;
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
            OS_TPrintf(" [%d, %d] - [%d, %d]\n", inputMatch[iMatch], protoMatch[iMatch],
                      inputMatch[iMatch + 1], protoMatch[iMatch + 1]);
#endif
            inputLocalLength = 0;
            protoLocalLength = 0;
            for (iInput = inputMatch[iMatch] + 1; iInput <= inputMatch[iMatch + 1]; iInput++)
            {
                inputLocalLength += inputLengths[iInput];
            }
            for (iProto = protoMatch[iMatch] + 1; iProto <= protoMatch[iMatch + 1]; iProto++)
            {
                protoLocalLength += protoLengths[iProto];
            }
            if (inputLocalLength == 0 && protoLocalLength == 0)
            {
                // 最初だけ [0, 0] [0, 0] と対応点が重複することがありうる
                continue;
            }
            iInput = inputMatch[iMatch] + 1;
            iProto = protoMatch[iMatch] + 1;
            inputCurrentLength = inputLengths[iInput];
            protoCurrentLength = protoLengths[iProto];
            if (inputLocalLength == 0)
            {
                inputLocalLength = inputCurrentLength = 1;
                iInput = inputMatch[iMatch + 1];
            }
            if (protoLocalLength == 0)
            {
                protoLocalLength = protoCurrentLength = 1;
                iProto = protoMatch[iMatch + 1];
            }
            inputPrevSumRatio = inputOrigNextRatio = inputNextRatio =
                FX_Div(inputCurrentLength, inputLocalLength);
            protoPrevSumRatio = protoOrigNextRatio = protoNextRatio =
                FX_Div(protoCurrentLength, protoLocalLength);
            localScore = 0;
            loopEnd = (inputMatch[iMatch + 1] - iInput) + (protoMatch[iMatch + 1] - iProto) + 1;
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
            OS_TPrintf("length: %d, %d\n", inputLocalLength, protoLocalLength);
#endif
            for (i = 0; i < loopEnd; i++)
            {
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
                OS_TPrintf(" [%d, %d]", iInput, iProto);
                OS_TPrintf("  Ratio: %3d %3d\n", FX_Whole(inputNextRatio * 100),
                          FX_Whole(protoNextRatio * 100));
#endif
                SDK_TASSERTMSG(iInput <= inputMatch[iMatch + 1],
                              "iInput(%d) > inputMatch[iMatch+1](%d)\n", iInput,
                              inputMatch[iMatch + 1]);
                SDK_TASSERTMSG(iProto <= protoMatch[iMatch + 1],
                              "iProto(%d) > protoMatch[iMatch+1](%d)\n", iProto,
                              protoMatch[iMatch + 1]);
                if (inputNextRatio <= protoNextRatio)
                {
                    fx32    ratio;
                    PRCPoint protoPoint;
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
                    OS_TPrintf(" inc iInput: score=%d, ratio=%d\n",
                              FX_Whole(inputNextRatio * (angleScores_(iInput, iProto))),
                              FX_Whole(100 * inputNextRatio));
#endif
                    protoNextRatio -= inputNextRatio;
                    if (iProto > 0)
                    {
                        GetMixedPoint(&protoPoint, &protoPoints[iProto - 1], protoNextRatio,
                                      &protoPoints[iProto], protoOrigNextRatio - protoNextRatio);
                    }
                    else
                    {
                        protoPoint = protoPoints[iProto];
                    }

                    inputCurrentLength += inputLengths[iInput + 1];
                    FX_DivAsync(inputCurrentLength, inputLocalLength);

#ifdef PRC_DEBUG_CHECK_OVERFLOW
                    {
                        fx32    prevScore = localScore;
#endif
                        localScore += inputNextRatio * (angleScores_(iInput, iProto))
                            * (doubleWidth - CityBlockDistance(&inputPoints[iInput], &protoPoint));
#ifdef PRC_DEBUG_CHECK_OVERFLOW
                        if (prevScore > localScore)
                            OS_TWarning("Internal Error: score overflow\n");
                    }
#endif

                    iInput++;
                    ratio = FX_GetDivResult();
                    inputOrigNextRatio = inputNextRatio = ratio - inputPrevSumRatio;
                    inputPrevSumRatio = ratio;
                }
                else
                {
                    fx32    ratio;
                    PRCPoint inputPoint;
#ifdef PRC_DEBUG_RECOGNIZE_DEEPLY
                    OS_TPrintf(" inc iProto: score=%d, length=%d\n",
                              FX_Whole(protoNextRatio * (angleScores_(iInput, iProto))),
                              FX_Whole(100 * protoNextRatio));
#endif
                    inputNextRatio -= protoNextRatio;
                    if (iInput > 0)
                    {
                        GetMixedPoint(&inputPoint, &inputPoints[iInput - 1], inputNextRatio,
                                      &inputPoints[iInput], inputOrigNextRatio - inputNextRatio);
                    }
                    else
                    {
                        inputPoint = inputPoints[iInput];
                    }

                    protoCurrentLength += protoLengths[iProto + 1];
                    FX_DivAsync(protoCurrentLength, protoLocalLength);

#ifdef PRC_DEBUG_CHECK_OVERFLOW
                    {
                        fx32    prevScore = localScore;
#endif
                        localScore += protoNextRatio * (angleScores_(iInput, iProto))
                            * (doubleWidth - CityBlockDistance(&protoPoints[iProto], &inputPoint));
#ifdef PRC_DEBUG_CHECK_OVERFLOW
                        if (prevScore > localScore)
                            OS_TWarning("Internal Error: score overflow\n");
                    }
#endif

                    iProto++;
                    ratio = FX_GetDivResult();
                    protoOrigNextRatio = protoNextRatio = ratio - protoPrevSumRatio;
                    protoPrevSumRatio = ratio;
                }
            }

            weight = FX_Div(inputLocalLength, inputStrokeLength);
            tmp = FX_Div(protoLocalLength, protoStrokeLength);
            if (weight < tmp)
            {
                weight = tmp;
            }
            weightedScore += FX_Mul(localScore / 256, weight);
            SDK_TASSERTMSG(weight != 0,
                          "inputStrokeLength: %d, inputLocalLength: %d, protoStrokeLength: %d, protoLocalLength: %d\n",
                          inputStrokeLength, inputLocalLength, protoStrokeLength, protoLocalLength);
            totalWeight += weight;
        }
        SDK_TASSERTMSG(totalWeight != 0, "nMatches: %d", nMatches);
        {
            fx32    ratio;
            ratio = (inputStrokeRatio >= protoStrokeRatio) ? inputStrokeRatio : protoStrokeRatio;
            *score = FX_Mul(weightedScore, ratio);
            *weight = FX_Mul(totalWeight, ratio);
        }
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
