/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PRC - 
  File:     prc_algo_standard.c

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

#include <nitro.h>
#include <nitro/prc/algo_standard.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/
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

static inline void GetMiddlePoint(PRCPoint *p, const PRCPoint *p1, const PRCPoint *p2)
{
    p->x = (s16)((p1->x + p2->x) / 2);
    p->y = (s16)((p1->y + p2->y) / 2);
}

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Standard

  Description:  �F���A���S���Y�����K�v�Ƃ��郏�[�N�̈�̑傫����
                �v�Z���܂�.

  Arguments:    maxPointCount   ���͓_���̏��(pen up marker ���܂�)
                maxStrokeCount  �搔�̏��
                protoDB         ���{DB.
                param           �F�������Ɋւ���p�����[�^.

  Returns:      �F���A���S���Y�����K�v�Ƃ��郁������.
 *---------------------------------------------------------------------------*/
u32
PRC_GetRecognitionBufferSizeEx_Standard(int maxPointCount,
                                        int maxStrokeCount,
                                        const PRCPrototypeDB_Standard *protoDB,
                                        const PRCRecognizeParam_Standard *param)
{
    (void)maxPointCount;
    (void)maxStrokeCount;
    (void)protoDB;
    (void)param;

    return 1;                          // OS_Alloc(0) �̓G���[�ƂȂ邽��
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Standard

  Description:  ���{DB�̓���� kind �̃G���g���Ɠ��̓p�^�[�����r���ĔF����,
                ���ʂ̏�� numRanking �ʂ�Ԃ��܂�.

  Arguments:    resultEntries   �F�����ʂւ̃|�C���^������z��ւ̃|�C���^.
                                ���߂���ȉ������F���ł��Ȃ������ꍇ�A
                                �c��� NULL �Ŗ��߂��܂�.
                resultScores    �F�����ʂ̃X�R�A�̔z��ւ̃|�C���^.
                numRanking      result* �ɕԂ���
                buffer          �F���A���S���Y�����g�p���郁�����̈�ւ̃|�C���^.
                                (�̈�T�C�Y>=PRC_GetRecognitionBufferSize �̕Ԃ�l)
                input           ���̓p�^�[��.
                protoDB         ���{DB.
                kindMask        �e���{DB�G���g���� kind �l�Ƃ̘_���ς��Ƃ�,
                                ��[���ł���ΗL���Ƃ݂Ȃ��܂�.
                param           �F�������Ɋւ���p�����[�^.

  Returns:      ��r���ꂽ���{DB��̃p�^�[����.
 *---------------------------------------------------------------------------*/
int
PRC_GetRecognizedEntriesEx_Standard(PRCPrototypeEntry **resultEntries,
                                    fx32 *resultScores,
                                    int numRanking,
                                    void *buffer,
                                    const PRCInputPattern_Standard *input,
                                    const PRCPrototypeDB_Standard *protoDB,
                                    u32 kindMask, const PRCRecognizeParam_Standard *param)
{
    int     i;
    const PRCiPatternData_Common *inputData;
    int     numCompared;
    int     normalizeSize;
    int     doubleWidth;

    (void)buffer;
    (void)param;

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
    doubleWidth = normalizeSize * 2;

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
            fx32    wholeScore;
            fx32    wholeWeight;

            if (!proto->entry->enabled || !(proto->entry->kind & kindMask))
                continue;

            protoData = &proto->data;

            if (inputData->strokeCount != protoData->strokeCount)
                continue;

            wholeScore = 0;
            wholeWeight = 0;

            for (iStroke = 0; iStroke < inputData->strokeCount; iStroke++)
            {
                int     iProto, iInput;
                int     protoStrokeIndex, inputStrokeIndex;
                int     protoSize, inputSize;
                const PRCPoint *protoPoints;
                const PRCPoint *inputPoints;
                const u16 *protoAngle;
                const u16 *inputAngle;
                const fx16 *protoRatio;
                const fx16 *inputRatio;
//                PRCPoint protoMidPoint, inputMidPoint;
                fx16    protoNextRatio, inputNextRatio;
                fx32    strokeScore;
                fx16    strokeRatio;

                strokeScore = 0;

                protoStrokeIndex = protoData->strokes[iStroke];
                inputStrokeIndex = inputData->strokes[iStroke];
                protoSize = protoData->strokeSizes[iStroke];
                inputSize = inputData->strokeSizes[iStroke];
                protoPoints = &protoData->pointArray[protoStrokeIndex];
                inputPoints = &inputData->pointArray[inputStrokeIndex];
                protoAngle = &protoData->lineSegmentAngleArray[protoStrokeIndex];
                inputAngle = &inputData->lineSegmentAngleArray[inputStrokeIndex];
                protoRatio = &protoData->lineSegmentRatioToStrokeArray[protoStrokeIndex];
                inputRatio = &inputData->lineSegmentRatioToStrokeArray[inputStrokeIndex];

                SDK_ASSERT(protoSize >= 2);
                SDK_ASSERT(inputSize >= 2);

                iProto = iInput = 1;
                protoNextRatio = protoRatio[iProto];
                inputNextRatio = inputRatio[iInput];
//                GetMiddlePoint(&protoMidPoint, &protoPoints[iProto-1], &protoPoints[iProto]);
//                GetMiddlePoint(&inputMidPoint, &inputPoints[iInput-1], &inputPoints[iInput]);
                for (i = 0; i < protoSize + inputSize - 3; i++)
                {
                    int     diff, score;
                    SDK_ASSERT(iProto < protoSize);
                    SDK_ASSERT(iInput < inputSize);
                    diff = (s16)(protoAngle[iProto] - inputAngle[iInput]);
                    if (diff < 0)
                    {
                        diff = -diff;
                    }
                    score = ((32768 - diff) / 128);
                    if (protoNextRatio <= inputNextRatio)
                    {
                        // ���{�̉��1���H��
                        // ���͂̉�̒��ŋ߂��ق��̓_�Ƃ̋������X�R�A�ɔ��f
                        inputNextRatio -= protoNextRatio;
                        score *= (inputNextRatio < inputRatio[iInput] / 2)      // �c�肪������菭�Ȃ������̓_�̂ق����߂�
                            ? (doubleWidth -
                               CityBlockDistance(&inputPoints[iInput],
                                                 &protoPoints[iProto])) : (doubleWidth -
                                                                           CityBlockDistance
                                                                           (&inputPoints
                                                                            [iInput - 1],
                                                                            &protoPoints[iProto]));
                        strokeScore += protoNextRatio * score;
                        iProto++;
                        protoNextRatio = protoRatio[iProto];
//                        GetMiddlePoint(&protoMidPoint, &protoPoints[iProto-1], &protoPoints[iProto]);
                    }
                    else
                    {
                        // ���͂̉��1���H��
                        // ���{�̉�̒��ŋ߂��ق��̓_�Ƃ̋������X�R�A�ɔ��f
                        protoNextRatio -= inputNextRatio;
                        score *= (protoNextRatio < protoRatio[iProto] / 2)      // �c�肪������菭�Ȃ������̓_�̂ق����߂�
                            ? (doubleWidth -
                               CityBlockDistance(&inputPoints[iInput],
                                                 &protoPoints[iProto])) : (doubleWidth -
                                                                           CityBlockDistance
                                                                           (&inputPoints[iInput],
                                                                            &protoPoints[iProto -
                                                                                         1]));
                        strokeScore += inputNextRatio * score;
                        iInput++;
                        inputNextRatio = inputRatio[iInput];
//                        GetMiddlePoint(&inputMidPoint, &inputPoints[iInput-1], &inputPoints[iInput]);
                    }
                }

                strokeRatio = protoData->strokeRatios[iStroke];
                if (strokeRatio < inputData->strokeRatios[iStroke])
                {
                    strokeRatio = inputData->strokeRatios[iStroke];
                }

                wholeScore += FX_Mul(strokeScore, strokeRatio);
                wholeWeight += strokeRatio;
            }

            wholeScore = FX_Div(wholeScore, wholeWeight * doubleWidth);
            wholeScore /= 256;

            if (proto->entry->correction != 0)
            {
                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction)
                    + proto->entry->correction;
            }

            if (wholeScore < 0)
                wholeScore = 0;
            if (wholeScore >= FX32_ONE)
                wholeScore = FX32_ONE;

            numCompared++;
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
    // �X�R�A�̐��K��

    return numCompared;
}

/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
