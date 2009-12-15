//============================================================================================
/**
 * @file field_bmanime_tool.c
 * @brief 配置モデルアニメコントローラー
 * @date  2009.10.25
 * @author  tamada GAME FREAK inc.
 *
 * @todo
 * event --> field_bmanime_tool --> field_buildmodel(FIELD_BMODEL-->OBJHND) --> GFL_G3D_OBJECT
 * と、階層が深くなっているのをどうするか考える。
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_player.h"
#include "field_buildmodel.h"

#include "field_bmanime_tool.h"

#include "script_def.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl" //SE指定

//プログラム内参照の定義とスクリプト内参照の定義がずれていたらassert
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_OPEN == ANM_INDEX_DOOR_OPEN );
SDK_COMPILER_ASSERT( SCR_BMANM_DOOR_CLOSE == ANM_INDEX_DOOR_CLOSE );

SDK_COMPILER_ASSERT( SCR_BMID_NULL == BM_SEARCH_ID_NULL );
SDK_COMPILER_ASSERT( SCR_BMID_DOOR == BM_SEARCH_ID_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_SANDSTREAM == BM_SEARCH_ID_SANDSTREAM );
SDK_COMPILER_ASSERT( SCR_BMID_PCMACHINE == BM_SEARCH_ID_PCMACHINE );
SDK_COMPILER_ASSERT( SCR_BMID_PC == BM_SEARCH_ID_PC );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_DOOR == BM_SEARCH_ID_PCEV_DOOR );
SDK_COMPILER_ASSERT( SCR_BMID_PCEV_FLOOR == BM_SEARCH_ID_PCEV_FLOOR );

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 配置モデルアニメ制御ワーク
 */
//------------------------------------------------------------------
struct _BMANIME_CONTROL_WORK {
  FIELD_BMODEL * entry;     ///<クローンで生成したオブジェクト
  G3DMAPOBJST * obj;        ///<オリジナルとなる配置モデルへの参照
  u16 anm_idx;              ///<アニメ指定
};

enum {
  NO_ANIME_IDX = 0xffff,    ///<アニメがない指定
};
//============================================================================================
//============================================================================================
static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos);
//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos)
{
  enum { RECT_SIZE = (FIELD_CONST_GRID_SIZE * 2) << FX32_SHIFT };
  rect->top = pos->z - RECT_SIZE;
  rect->bottom = pos->z + RECT_SIZE;
  rect->left = pos->x - RECT_SIZE;
  rect->right = pos->x + RECT_SIZE;
}

//------------------------------------------------------------------
/**
 * @brief 指定位置付近の配置モデルを検索する
 * @param bmodel_man  配置モデルマネジャーへのポインタ
 * @param id          検索ID（BM_SEARCH_ID参照）
 * @param pos         検索位置
 * @return NULL   見つからなかった
 * @return G3DMAPOBJST *  見つけたドア配置モデルへの参照
 */
//------------------------------------------------------------------
G3DMAPOBJST * BMANIME_DIRECT_Search(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos)
{
  G3DMAPOBJST * entry = NULL;
  G3DMAPOBJST ** array;
  u32 result_num;

  GF_ASSERT( id < BM_SEARCH_ID_MAX );
  {
    //検索矩形を作成
    FLDHIT_RECT rect;
    makeRect(&rect, pos);
    //矩形範囲内の配置モデルリストを生成する
    array = FIELD_BMODEL_MAN_CreateObjStatusList(bmodel_man, &rect, id, &result_num);
  }
  entry = array[0];
  //矩形範囲内の配置モデルリストを解放する
  GFL_HEAP_FreeMemory(array);
  return entry;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BMANIME_CONTROL_WORK * BMANIME_CTRL_Create(
    FIELD_BMODEL_MAN * bmodel_man, BM_SEARCH_ID id, const VecFx32 * pos)
{
  BMANIME_CONTROL_WORK * ctrl;
  G3DMAPOBJST * obj;
  FIELD_BMODEL * entry;

  obj = BMANIME_DIRECT_Search(bmodel_man, id, pos);
  if (obj == NULL) return NULL;
  entry = FIELD_BMODEL_Create( bmodel_man, obj );
  if (entry == NULL) return NULL;
  G3DMAPOBJST_changeViewFlag(obj, FALSE);

  ctrl = GFL_HEAP_AllocClearMemory(
      FIELD_BMODEL_MAN_GetHeapID(bmodel_man), sizeof(BMANIME_CONTROL_WORK) );
  ctrl->obj = obj;
  ctrl->entry = entry;
  ctrl->anm_idx = NO_ANIME_IDX;
  return ctrl;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void BMANIME_CTRL_Delete(BMANIME_CONTROL_WORK * ctrl)
{
  if (ctrl == NULL) return;
  if (ctrl->entry)
  {
    FIELD_BMODEL_Delete( ctrl->entry );
    ctrl->entry = NULL;
  }
  if (ctrl->obj)
  {
    G3DMAPOBJST_changeViewFlag(ctrl->obj, TRUE);
    ctrl->obj = NULL;
  }
  GFL_HEAP_FreeMemory( ctrl );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void BMANIME_CTRL_SetAnime(BMANIME_CONTROL_WORK * ctrl, u32 anm_idx)
{
  if (ctrl == NULL) return;
  if (ctrl->anm_idx != NO_ANIME_IDX) {
    FIELD_BMODEL_SetAnime( ctrl->entry, ctrl->anm_idx, BMANM_REQ_END);
  }
  ctrl->anm_idx = anm_idx;
  FIELD_BMODEL_SetAnime( ctrl->entry, anm_idx, BMANM_REQ_START);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void BMANIME_CTRL_StopAnime( BMANIME_CONTROL_WORK * ctrl )
{
  if (ctrl == NULL) return;
  if (ctrl->anm_idx != NO_ANIME_IDX) {
    FIELD_BMODEL_SetAnime( ctrl->entry, ctrl->anm_idx, BMANM_REQ_STOP);
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL BMANIME_CTRL_WaitAnime(BMANIME_CONTROL_WORK * ctrl)
{
  if (ctrl == NULL) return TRUE;
  if (ctrl->anm_idx == NO_ANIME_IDX) return TRUE;
  if ( FIELD_BMODEL_GetAnimeStatus( ctrl->entry, ctrl->anm_idx) == TRUE)
  {
    FIELD_BMODEL_SetAnime( ctrl->entry, ctrl->anm_idx, BMANM_REQ_STOP);
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL BMANIME_CTRL_CheckSE( const BMANIME_CONTROL_WORK * ctrl )
{
  u16 se_no;
  SEPLAYER_ID playerID;
  if ( BMANIME_CTRL_GetSENo( ctrl, ctrl->anm_idx, &se_no ) == FALSE )
  {
    return FALSE;
  }
  playerID = PMSND_GetSE_DefaultPlayerID( se_no );
  return PMSND_CheckPlaySE_byPlayerID( playerID );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL BMANIME_CTRL_GetSENo(const BMANIME_CONTROL_WORK * ctrl, u32 anm_idx, u16 * se_no)
{
  static const struct {
    u16 prog_id;
    u16 seID[BMANM_INDEX_MAX];
  } SeTbl[] = {
    { BM_PROG_ID_DOOR_NORMAL, { SEQ_SE_FLD_20, SEQ_SE_FLD_21 } },
    { BM_PROG_ID_DOOR_AUTO,   { SEQ_SE_FLD_22, SEQ_SE_FLD_22 } },
    //{ BM_PROG_ID_BADGEGATE,   { SEQ_SE_FLD_20, SEQ_SE_FLD_21 } },
    { BM_PROG_ID_PCELEVATOR,  { SEQ_SE_FLD_22, SEQ_SE_FLD_22 } },
  };
  int i;
  BM_PROG_ID id = FIELD_BMODEL_GetProgID( ctrl->entry );

  *se_no = 0;
  if ( anm_idx >= BMANM_INDEX_MAX )
  {
    TAMADA_Printf( "anm_idx(%d) >= BMANM_INDEX_MAX\n", anm_idx );
    return FALSE;
  }
  for (i = 0; i < NELEMS(SeTbl); i++)
  {
    if (SeTbl[i].prog_id == id)
    {
      *se_no = SeTbl[i].seID[anm_idx];
      //TAMADA_Printf("BMANIME_PROG_ID(%d) SE(%d)\n", id, *se_no);
      return TRUE;
    }
  }
  return FALSE;
}





