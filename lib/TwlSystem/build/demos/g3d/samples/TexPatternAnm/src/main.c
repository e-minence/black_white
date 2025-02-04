/*---------------------------------------------------------------------------*
  Project:  TWL-System - demos - g3d - samples - TexPatternAnm
  File:     main.c

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1155 $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//
// テクスチャパターンアニメーション再生サンプル
//
// テクスチャパターンアニメーションを再生するには、g3dcvtrによって.itpファイルから
// コンバートされた.nsbtpファイルを使用します。
//
// サンプルの動作
// テクスチャパターンアニメーションが繰り返し再生されます。
//
// HOWTO:
// 1: .nsbtpファイルをメインメモリにロードします。
// 2: NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName関数等を用いて再生したい
//    テクスチャパターンアニメーションへのポインタを取得します。
// 3: NNS_G3dAllocAnmObjを使用して、NNSG3dAnmObjのための領域を取得します。
//    なお解放する場合は、NNS_G3dFreeAnmObjを使用します。
// 4: NNS_G3dAnmObjInitを使用して、取得した領域を初期化します。
// 5: NNS_G3dRenderObjAddAnmObjを使用して、NNSG3dRenderObjに関連付けます。
//    なお、複数のNNSG3dRenderObjに関連付けることはできません。そのような場合には
//    別のNNSG3dAnmObjを確保しなくてはいけません(.nsbtpファイル内のデータは
//    共有できます)。
//
//---------------------------------------------------------------------------

#include "g3d_demolib.h"

G3DDemoCamera gCamera;        // カメラ構造体。
G3DDemoGround gGround;        // グラウンド構造体。

static void                InitG3DLib(void);
static BOOL 
LoadG3DModel(const char*            path,
             NNSG3dResMdl**         ppModel,
             NNSG3dResFileHeader**  ppResFile);

static void SetCamera(G3DDemoCamera* camera);


/* -------------------------------------------------------------------------
  Name:         NitroMain

  Description:  サンプルのメイン。

  Arguments:    なし。

  Returns:      なし。
   ------------------------------------------------------------------------- */
void
NitroMain(void)
{
    NNSG3dRenderObj      object;
    NNSG3dResMdl*        pModel;
    NNSG3dResTex*        pTexture;
    NNSG3dResFileHeader* fileHeader;
    BOOL                 bResult;
    void*                pAnmRes;
    NNSFndAllocator      allocator;
    NNSG3dAnmObj*        pAnmObj;

    G3DDemo_InitSystem();
    G3DDemo_InitMemory();
    G3DDemo_InitVRAM();

    InitG3DLib();
    G3DDemo_InitDisplay();

    //
    // 4バイトアラインメントでアロケータを作成
    //
    NNS_FndInitAllocatorForExpHeap(&allocator, G3DDemo_AppHeap, 4);
    
    // マネージャがテクスチャイメージスロットを4スロット分管理できるようにして
    // デフォルトマネージャにする。
    NNS_GfdInitFrmTexVramManager(4, TRUE);

    // マネージャがパレットを32KB分管理できるようにして
    // デフォルトマネージャにする。
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);

    bResult = LoadG3DModel("data/itp_sample.nsbmd", &pModel, &fileHeader );
    SDK_ASSERTMSG( bResult, "load failed");
    
    //
    // テクスチャ情報を取得します
    //
    pTexture = NNS_G3dGetTex(fileHeader);
    
    //
    // G3D: NNSG3dRenderObjの初期化
    //
    // NNSG3dRenderObjは１つのモデルとそのモデルに適用されるアニメに関する各種情報を
    // 保持する構造体です。
    // 内部のメンバポインタで指定される領域はユーザー自身で取得・解放する必要があります。
    // ここでmdlはNULLであってもかまいません(NNS_G3dDraw時には設定されている必要がある)。
    //
    NNS_G3dRenderObjInit(&object, pModel);
    
    //
    // テクスチャパターン・アニメを読み込んで関連付けします
    //
    {
        //
        // ファイル読み込み
        //
        const u8* pFile = G3DDemo_LoadFile("data/itp_sample.nsbtp");
        SDK_ASSERTMSG( pFile, "anm load failed" );
        {
            // アニメーションの初期化、レンダリングオブジェクトへの追加
            // 
            // 読み込んだアニメーションファイルとNNSG3dRenderObjの仲介をする
            // アニメーションオブジェクト(NNSG3dAnmObj)を確保・初期化する。
            // 全てのアニメーションが同様のコードでセットアップされる。
            //

            //
            // アニメーションオブジェクトに必要なメモリをallocatorから割り当てる。
            // モデルリソースとアニメーションリソースを指定して割り当てる必要がある。
            //

            // インデックス＃０のアニメーションを指定
            pAnmRes = NNS_G3dGetAnmByIdx(pFile, 0);
            SDK_NULL_ASSERT(pAnmRes);

            // 必要量のメモリをアロケートする。イニシャライズは別途必要になる。
            pAnmObj = NNS_G3dAllocAnmObj(&allocator, // 使用するアロケータを指定
                                         pAnmRes,    // アニメーションリソースを指定
                                         pModel);    // モデルリソースを指定
            SDK_NULL_ASSERT(pAnmObj);

            //
            // AnmObj を初期化する。
            //
            NNS_G3dAnmObjInit(pAnmObj,    // アニメーションオブジェクトへのポインタ
                              pAnmRes,    // アニメリソースへのポインタ
                              pModel,     // NNSG3dResMdlへのポインタ
                              pTexture ); // NNSG3dResTexへのポインタ(テクスチャパターンアニメ以外ならばNULLでもよい)

            //
            // RenderObj に AnmObj を追加する。
            //
            NNS_G3dRenderObjAddAnmObj( &object, pAnmObj );
        }
    }

    //
    // デモ共通の初期化
    //
    G3DDemo_InitCamera(&gCamera, 5*FX32_ONE, 16*FX32_ONE);
    G3DDemo_InitGround(&gGround, (fx32)(1.5*FX32_ONE));

	G3DDemo_InitConsole();
	G3DDemo_Print(0,0, G3DDEMO_COLOR_YELLOW, "TEX PATTERN ANM");

    for(;;)
    {
        OSTick time;

        SVC_WaitVBlankIntr();

        G3DDemo_PrintApplyToHW();
        G3DDemo_ReadGamePad();

        G3DDemo_MoveCamera(&gCamera);
        G3DDemo_CalcCamera(&gCamera);

        G3X_Reset();
        G3X_ResetMtxStack();

        SetCamera(&gCamera);

        // アニメフレームを更新する
        pAnmObj->frame += FX32_ONE;
        if( pAnmObj->frame >= NNS_G3dAnmObjGetNumFrame(pAnmObj))
        {
            pAnmObj->frame = 0;
        }

        {
            // 位置とサイズの調整
            VecFx32 scale = {FX32_ONE << 1, FX32_ONE << 1, FX32_ONE << 1};
            VecFx32 trans = {0, FX32_ONE * 5, 0};
            NNS_G3dGlbSetBaseScale(&scale);
            NNS_G3dGlbSetBaseTrans(&trans);

            //
            // G3D:
            // G3dGlbXXXXで設定した状態をまとめて転送します。
            // カメラや射影行列等が設定されています。
            //
            NNS_G3dGlbFlushP();

            //  時間測定開始
            time = OS_GetTick();
            {
                //
                // G3D: NNS_G3dDraw
                // セットアップされたNNSG3dRenderObjを渡すことにより
                // あらゆる場合の描画を行う
                //
                NNS_G3dDraw(&object);
            }
            // 時間測定結果
            time = OS_GetTick() - time;
        }

        {
            // スケールとトランスを元に戻す(地面のメッシュ描画のため)
            VecFx32 scale = {FX32_ONE, FX32_ONE, FX32_ONE};
            VecFx32 trans = {0, 0, 0};
            NNS_G3dGlbSetBaseScale(&scale);
            NNS_G3dGlbSetBaseTrans(&trans);

            //
            // G3D: 通常のG3_XXXを呼ぶときは、その前にNNS_G3dGeComFlushBuffer()
            //      をコールして同期しなくてはならない
            //
            NNS_G3dGlbFlushP();
            NNS_G3dGeFlushBuffer();
            G3DDemo_DrawGround(&gGround);
        }

        /* ジオメトリ＆レンダリングエンジン関連メモリのスワップ */
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        G3DDemo_Printf(16,0, G3DDEMO_COLOR_YELLOW, "TIME:%06d usec", OS_TicksToMicroSeconds(time));
    }
}


/* -------------------------------------------------------------------------
  Name:         InitG3DLib

  Description:  G3Dライブラリを初期化します。

  Arguments:    なし。

  Returns:      なし。
   ------------------------------------------------------------------------- */
static void
InitG3DLib(void)
{
    //
    // G3D:
    // デフォルトの初期化を行う
    // ------------------------------------------------------------------------
    NNS_G3dInit();
}


/* -------------------------------------------------------------------------
  Name:         LoadG3DModel

  Description:  ファイルからモデルデータを読み込みます。

  Arguments:    path:       モデルデータのパス名。
                ppModel:    モデルへのポインタのポインタ
                ppResFile： ロードデータファイルへのポインタのポインタ
                
  Returns:      読み込みの成否
   ------------------------------------------------------------------------- */
static BOOL 
LoadG3DModel(const char* path, NNSG3dResMdl** ppModel, NNSG3dResFileHeader** ppResFile )
{
    *ppResFile = G3DDemo_LoadFile(path);

    if(*ppResFile != NULL)
    {
        BOOL    status;

        // NNS_G3dResDefaultSetupやNNS_G3dDrawの中でDMA転送を使用するため、
        // これらの関数を呼ぶ前にモデルリソース全体をメモリへストアします。
        // リソースのサイズがある程度大きい場合はDC_StoreAllを呼んだ方が高速です。
        // DC_StoreRangeやDC_StoreAllについてはNITRO-SDK関数リファレンスマニュアルを参照して下さい。
        DC_StoreRange(*ppResFile, (*ppResFile)->fileSize);

        // デフォルトの初期化関数をコールしてセットアップする場合
        status = NNS_G3dResDefaultSetup(*ppResFile);
        NNS_G3D_ASSERTMSG(status, "NNS_G3dResDefaultSetup failed");

        //
        // G3D: モデルの取得
        // nsbmdはモデルを複数含むことができるので、インデックス(モデルが１つの場合は0)
        // を指定して１つのモデルへのポインタを取得する。
        // --------------------------------------------------------------------
        *ppModel = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(*ppResFile), 0);
        
        return TRUE;
    }

    return FALSE;
}


/* -------------------------------------------------------------------------
  Name:         SetCamera

  Description:  G3Dのグローバル状態にカメラ行列と射影行列を設定します。

  Arguments:    camera:		G3DDemoCamera構造体へのポインタ。

  Returns:      なし。
   ------------------------------------------------------------------------- */
static void
SetCamera(G3DDemoCamera* camera)
{
    G3DDemoLookAt* lookat = &camera->lookat;
    G3DDemoPersp*  persp  = &camera->persp;

    NNS_G3dGlbPerspective(persp->fovySin, persp->fovyCos, persp->aspect, persp->nearClip, persp->farClip);
    NNS_G3dGlbLookAt(&lookat->camPos, &lookat->camUp, &lookat->target);
}
