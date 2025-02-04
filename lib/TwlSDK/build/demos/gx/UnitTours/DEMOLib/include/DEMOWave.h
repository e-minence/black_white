/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/DEMOLib
  File:     DEMOWave.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
  
 *---------------------------------------------------------------------------*/
#ifndef DEMO_WAVE_H_
#define DEMO_WAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         DEMOReadWave

  Description:  Waveデータの読み込み

  Arguments:    dst - 波形データを取得するバッファ
                filename - ストリーム再生するファイル名

  Returns:      波形データのサイズを返します
 *---------------------------------------------------------------------------*/
int DEMOReadWave(char* dst, const char *filename);

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* DEMO_WAVE_H_ */
