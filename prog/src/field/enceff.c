//======================================================================
/*
 * @file	enceff.c
 * @brief	エンカウントエフェクト
 * @author saito
 */
//======================================================================
#include "enceff.h"
#include "gamesystem/gamesystem.h"

#include "fieldmap.h"

#define OVERLAY_STACK_MAX (3)
#define OVERLAY_NONE  (-1)

typedef GMEVENT* (*CREATE_FUNC)(GAMESYS_WORK *, FIELDMAP_WORK *, const BOOL);
typedef void (*DRAW_FUNC)(ENCEFF_CNT_PTR);

FS_EXTERN_OVERLAY(enceff_ci);
FS_EXTERN_OVERLAY(enceff_mdl);
FS_EXTERN_OVERLAY(enceff_prg);
FS_EXTERN_OVERLAY(enceff_wav);
FS_EXTERN_OVERLAY(enceff_pnl);
FS_EXTERN_OVERLAY(enceff_pnl1);
FS_EXTERN_OVERLAY(enceff_pnl2);
FS_EXTERN_OVERLAY(enceff_pnl3);

typedef struct ENCEFF_CNT_tag
{
  void *Work;
  DRAW_FUNC DrawFunc;
  FIELDMAP_WORK *FieldMapWork;
  int OverlayStack[OVERLAY_STACK_MAX];
  int OverlayStackNum;
  void *UserWorkPtr;
}ENCEFF_CNT;

typedef struct {
  CREATE_FUNC CreateFunc;
  DRAW_FUNC DrawFunc;
  FSOverlayID OverlayID;
  BOOL IsFadeWhite;
}ENCEFF_TBL;

//※ enceffno_def.h と並びを同じにすること
static const ENCEFF_TBL EncEffTbl[] = {
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE},     //野生　通常
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE},     //野生　強
  {ENCEFF_WAV_Create, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav), TRUE},      //野生　水上 釣り
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), FALSE},     //野生　屋内
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE},     //野生　砂地
  {ENCEFF_MDL_Create1, ENCEFF_MDL_Draw, FS_OVERLAY_ID(enceff_mdl), TRUE},     //トレーナー　通常
  {ENCEFF_WAV_Create, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav), TRUE},      //トレーナー　水上
  {ENCEFF_PNL1_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl1), FALSE},    //トレーナー　屋内
  {ENCEFF_PNL3_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl3), TRUE},    //トレーナー　砂地

  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE},    //バトルサブウェイ

  {ENCEFF_CI_CreateRival, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //ライバル
  {ENCEFF_CI_CreateSupport, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //サポーター
  {ENCEFF_CI_CreateGym01A, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C1ジムリーダーＡ
  {ENCEFF_CI_CreateGym01B, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C1ジムリーダーＢ
  {ENCEFF_CI_CreateGym01C, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C1ジムリーダーＣ
  {ENCEFF_CI_CreateGym02, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C2ジムリーダー
  {ENCEFF_CI_CreateGym03, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C3ジムリーダー
  {ENCEFF_CI_CreateGym04, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C4ジムリーダー
  {ENCEFF_CI_CreateGym05, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C5ジムリーダー
  {ENCEFF_CI_CreateGym06, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C6ジムリーダー
  {ENCEFF_CI_CreateGym07, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C7ジムリーダー
  {ENCEFF_CI_CreateGym08A, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C8ジムリーダーＡ
  {ENCEFF_CI_CreateGym08B, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //C8ジムリーダーＢ
  {ENCEFF_CI_CreateBigFour1, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //四天王1
  {ENCEFF_CI_CreateBigFour2, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //四天王2
  {ENCEFF_CI_CreateBigFour3, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //四天王3
  {ENCEFF_CI_CreateBigFour4, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //四天王4
  {ENCEFF_CI_CreateChamp, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //チャンプ
  {ENCEFF_CI_CreateBoss, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //N
  {ENCEFF_CI_CreateSage, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //ゲーツィス
  {ENCEFF_CI_CreatePlasma, NULL, FS_OVERLAY_ID(enceff_ci), FALSE},    //プラズマ団

  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE},    //エフェクト
  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE},    //パッケージポケ
  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE},    //移動ポケ
  {ENCEFF_PNL2_Create, ENCEFF_PNL_Draw, FS_OVERLAY_ID(enceff_pnl2), TRUE},    //三銃士ポケ

  {ENCEFF_WAV_Create2, ENCEFF_WAV_Draw, FS_OVERLAY_ID(enceff_wav), FALSE},    //パレスワープ（エンカウントには使用しない）

};


static void LoadOverlay(ENCEFF_CNT_PTR ptr, const inFSOverlayID);


//--------------------------------------------------------------
/**
 * @brief エンカウントエフェクトコントローラ作成
 * @param inHeapID    ヒープＩＤ
 * @retval  ptr   コントローラポインタ
 */
//--------------------------------------------------------------
ENCEFF_CNT_PTR ENCEFF_CreateCntPtr(const HEAPID inHeapID, FIELDMAP_WORK * fieldmap)
{
  ENCEFF_CNT_PTR ptr;
  ptr = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(ENCEFF_CNT));
  ptr->FieldMapWork = fieldmap;
  ptr->OverlayStackNum = 0;
  return ptr;
}

//--------------------------------------------------------------
/**
 * @brief   エンカウントエフェクトコントローラ破棄
 * @param ptr     コントローラポインタ
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_DeleteCntPtr(ENCEFF_CNT_PTR ptr)
{
  ENCEFF_FreeUserWork(ptr);
  //オーバーレイスタックがある場合はここでアンロード
  ENCEFF_UnloadEffOverlay(ptr);
  
  GFL_HEAP_FreeMemory( ptr );
}

//--------------------------------------------------------------
/**
 * @brief   エンカウントエフェクトセット
 * @param   ptr       コントローラポインタ
 * @param   event     イベントポインタ
 * @param   inID      エフェクトＩＤ
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_SetEncEff(ENCEFF_CNT_PTR ptr, GMEVENT * event, const ENCEFF_ID inID)
{
  GMEVENT *call_event;
  FIELDMAP_WORK * fieldmap = ptr->FieldMapWork;
  GAMESYS_WORK * gsys = FIELDMAP_GetGameSysWork( ptr->FieldMapWork );

  int no;
  no = inID;
  if (no >= ENCEFFID_MAX)
  {
    GF_ASSERT_MSG(0,"ID=%d",no);
    no = 0;
  }
#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_saitou
//  no = ENCEFFID_MAX-1;
#endif
#endif
  
  //オーバーレイロード
  if (EncEffTbl[no].OverlayID != OVERLAY_NONE)
  {
    LoadOverlay(ptr, EncEffTbl[no].OverlayID);
  }

  call_event = EncEffTbl[no].CreateFunc(gsys, fieldmap, EncEffTbl[no].IsFadeWhite);
  //イベントコール
  GMEVENT_CallEvent( event, call_event );
  //ワークポインタセット
  ptr->Work = GMEVENT_GetEventWork( call_event );
  //描画関数セット
  ptr->DrawFunc = EncEffTbl[no].DrawFunc;
}

//--------------------------------------------------------------
/**
 * @brief   ワークポインタを返す
 * @param ptr   コントローラポインタ
 * @retval  void*     ワークポインタ
 */
//--------------------------------------------------------------
void *ENCEFF_GetWork(ENCEFF_CNT_PTR ptr)
{
  if (ptr->Work == NULL) GF_ASSERT(0);

  return ptr->Work;
}

//--------------------------------------------------------------
/**
 * @brief   描画関数
 * @param   ptr     コントローラポインタ
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_Draw(ENCEFF_CNT_PTR ptr)
{
  if (ptr->DrawFunc != NULL)
  {
    ptr->DrawFunc(ptr);
  }
}

//--------------------------------------------------------------
/**
 * @brief   ユーザーワークポインタをセット
 * @param ptr   コントローラポインタ
 * @retval  void*     ワークポインタ
 */
//--------------------------------------------------------------
void *ENCEFF_AllocUserWork(ENCEFF_CNT_PTR ptr, const int size, const HEAPID inHeapID)
{
  if (ptr->UserWorkPtr != NULL)
  {
    GF_ASSERT(0);
  }

  NOZOMU_Printf("user_work_size = %x\n",size); 
  
  ptr->UserWorkPtr = GFL_HEAP_AllocClearMemory(inHeapID, size);
  return ptr->UserWorkPtr;
}

//--------------------------------------------------------------
/**
 * @brief   ユーザーワーク解放
 * @param ptr   コントローラポインタ
 * @retval  none
 */
//--------------------------------------------------------------
void ENCEFF_FreeUserWork(ENCEFF_CNT_PTR ptr)
{
  if (ptr->UserWorkPtr != NULL)
  {
    GFL_HEAP_FreeMemory( ptr->UserWorkPtr );
    ptr->UserWorkPtr = NULL;
  }
}


//--------------------------------------------------------------
/**
 * @brief   ユーザーワークポインタを取得
 * @param ptr   コントローラポインタ
 * @retval  void*     ワークポインタ
 */
//--------------------------------------------------------------
void *ENCEFF_GetUserWorkPtr(ENCEFF_CNT_PTR ptr)
{
  return ptr->UserWorkPtr;
}

//--------------------------------------------------------------------------------------------
/**
 * オーバーレイをロードしてスタックする
 *
 * @param   ptr     コントローラポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadOverlay(ENCEFF_CNT_PTR ptr, const inFSOverlayID)
{
  GFL_OVERLAY_Load( inFSOverlayID );
  ptr->OverlayStack[ ptr->OverlayStackNum++ ] = inFSOverlayID;
  GF_ASSERT( ptr->OverlayStackNum <= OVERLAY_STACK_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * パネルエフェクト用オーバーレイロード
 *
 * @param   ptr     コントローラポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_LoadPanelEffOverlay(ENCEFF_CNT_PTR ptr)
{
  //プログラム制御エフェクトオーバーレイロード
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_prg));
  //パネルエフェクトオーバーレイロード
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_pnl));
}

//--------------------------------------------------------------------------------------------
/**
 * 頂点エフェクト用オーバーレイロード
 *
 * @param   ptr     コントローラポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_LoadVertexEffOverlay(ENCEFF_CNT_PTR ptr)
{
  //プログラム制御エフェクトオーバーレイロード
  LoadOverlay(ptr, FS_OVERLAY_ID(enceff_prg));
}

//--------------------------------------------------------------------------------------------
/**
 * エフェクト用オーバーレイアンロード
 *
 * @param   
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_UnloadEffOverlay(ENCEFF_CNT_PTR ptr)
{
  if (ptr->OverlayStackNum>0)
  {
    int idx;
    while(ptr->OverlayStackNum>0)
    {
      idx = ptr->OverlayStackNum-1;
      //アンロード
      if ( ptr->OverlayStack[ idx ] != NULL )
      {
        GFL_OVERLAY_Unload( ptr->OverlayStack[ idx ] );
      }
      else GF_ASSERT_MSG(0,"Can not unload idx=%d\n", idx);

      ptr->OverlayStackNum--;
    }
  }
}


