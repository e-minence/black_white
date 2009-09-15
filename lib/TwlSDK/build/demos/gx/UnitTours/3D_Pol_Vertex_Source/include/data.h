/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/3D_Pol_Vertex_Source
  File:     data.h

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

/*
 ******************************************************************************
 *
 *  Project    :
 *               nitroSDKサンプルプログラム
 *  Title      :
 *               G3_TexImageParam()のGX_TEXGEN_VERTEX設定
 *              （視点固定テクスチャの表現）
 *  Explanation:
 *        　　　 G3_TexImageParam()のGX_TEXGEN_VERTEX設定を使用して、視点固定テ
 *             クスチャの表現を行う。
 *
 ******************************************************************************
 */
#ifndef TEX_4PLETT_H_
#define TEX_4PLETT_H_

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

/* 4色パレットテクスチャ */
extern const u16 tex_4plett64x64[];

/* 4色パレット */
extern const u16 pal_4plett[];

#endif
