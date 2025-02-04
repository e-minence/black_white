/*---------------------------------------------------------------------------*
  Project:  TWL-System - include - nnsys - g3d
  File:     util.h

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1155 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_UTIL_H_
#define NNSG3D_UTIL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
// C++の場合こうしないと16文字目を使用できないため
#define NNS_G3D_UTIL_RESNAME_LEN 17
#else
#define NNS_G3D_UTIL_RESNAME_LEN 16
#endif


/*---------------------------------------------------------------------------*
    NNSG3dUtilResName

    リソース名をプログラム内で定義するときに使用される共用体
    NNS_G3D_DEFRESNAMEを通して使用するのがよい。
    C++でも16文字一杯使えるようにするために存在する。
 *---------------------------------------------------------------------------*/
typedef union
{
    char _0[NNS_G3D_UTIL_RESNAME_LEN];
    struct
    {
        NNSG3dResName resName;
#ifdef __cplusplus
        u32           _1;
#endif
    };
}
NNSG3dUtilResName;

/*---------------------------------------------------------------------------*
    NNS_G3D_DEFRESNAME

    プログラムでリソース名を使用する場合に使用するマクロ。
    C++でも16文字いっぱい使える。

    WARNING
    コンパイラの実装がstaticな領域はあらかじめ0でクリアされるようになって
    いなくてはならない。
 *---------------------------------------------------------------------------*/
#define NNS_G3D_DEFRESNAME(var, str) \
    static const NNSG3dUtilResName var ATTRIBUTE_ALIGN(4) = { str }


//
// 文字列リテラルを使用して検索する場合のマクロ
// NOTICE:
// インライン関数化してはいけない
//
#define NNS_G3D_GET_JNTID(pMdl, pJntID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pJntID = NNS_G3dGetNodeIdxByName(NNS_G3dGetNodeInfo(pMdl), (NNSG3dResName*)name); \
    } while(0)

#define NNS_G3D_GET_MATID(pMdl, pMatID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pMatID = NNS_G3dGetMatIdxByName(NNS_G3dGetMat(pMdl), (NNSG3dResName*)name);      \
    } while(0)

#define NNS_G3D_GET_SHPID(pMdl, pShpID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pShpID = NNS_G3dGetShpIdxByName(NNS_G3dGetShp(pMdl), (NNSG3dResName*)name);      \
    } while(0)

#define NNS_G3D_GET_MDL(pMdlSet, pMdl, literal)                                           \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        pMdl = NNS_G3dGetMdlByName(pMdlSet, (NNSG3dResName*)name);                        \
    } while(0)


//
// アニメーションリソースファイルからアニメーションリソースを取得
//
#define NNS_G3D_GET_ANM(pRes, pResAnm, literal)                                           \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        pResAnm = NNS_G3dGetAnmByName(pRes, (NNSG3dResName*)name);                                       \
    } while(0)


//
// ビットベクトル
//
NNS_G3D_INLINE BOOL NNSi_G3dBitVecCheck(const u32* vec, u32 idx);
NNS_G3D_INLINE void NNSi_G3dBitVecSet(u32* vec, u32 idx);
NNS_G3D_INLINE void NNSi_G3dBitVecReset(u32* vec, u32 idx);


//
// 行列スタックからの取得と変更
//
void NNS_G3dGetCurrentMtx(MtxFx43* m, MtxFx33* n);
BOOL NNS_G3dGetResultMtx(const NNSG3dRenderObj* pRenderObj,
                         MtxFx43* pos,
                         MtxFx33* nrm,
                         u32 nodeID);
BOOL NNS_G3dSetResultMtx(const NNSG3dRenderObj* pRenderObj,
                         const MtxFx43* pos,
                         const MtxFx33* nrm,
                         u32 nodeID);


//
// デフォルトのイニシャライズ
//
void NNS_G3dInit(void);

//
// SBC解析ユーティリティ
//
int NNS_G3dGetSbcCmdLen(const u8* c);
const u8* NNS_G3dSearchSbcCmd(const u8* c, u8 cmd);
const u8* NNS_G3dGetParentNodeID(int* parentID, const u8* sbc, u32 nodeID);
int NNS_G3dGetChildNodeIDList(u8* idList, const u8* sbc, u32 nodeID);

//
// リソースセットアップ・終了処理ユーティリティ
//
BOOL NNS_G3dResDefaultSetup(void* pResData);
void NNS_G3dResDefaultRelease(void* pResData);

//
// 座標変換ユーティリティ
//
int NNS_G3dLocalOriginToScrPos(int* px, int* py);
int NNS_G3dWorldPosToScrPos(const VecFx32* pWorld, int* px, int* py);
int NNS_G3dScrPosToWorldLine(int px, int py, VecFx32* pNear, VecFx32* pFar);


#ifdef __cplusplus
}/* extern "C" */
#endif

#include <nnsys/g3d/util_inline.h>

#endif
