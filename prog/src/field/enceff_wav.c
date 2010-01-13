//======================================================================
/*
 * @file	enceff2.c
 * @brief	イベント：エンカウントエフェクト
 * @author saito
 */
//======================================================================
#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"


#define MUL_VAL (16)
#define Z_MUL_VAL (64)
#define POLY_W_NUM  (32)
#define POLY_H_NUM  (24)

#define VTX_W_NUM (POLY_W_NUM+1)
#define VTX_H_NUM (POLY_H_NUM+1)

#define WAVE_WAIT (8)
#define WAVE_COUNT (7)

#define WAVE_AFTER_WAIT (6)

#define WAVE_HEIGHT (FX16_ONE/4)

typedef struct
{
  VecFx16 Vtx[VTX_W_NUM*VTX_H_NUM];
  fx16 TmpZ[VTX_W_NUM*VTX_H_NUM];
  int WaveWait;
  int WaveCount;
}ENCEFF_WAV_WORK;

static void DrawMesh(void *wk);
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work );
static BOOL WaveMain(void *work);
static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);

//--------------------------------------------------------------------------------------------
/**
 * イベント作成
 *
 * @param   gsys        ゲームシステムポインタ
 * @param   fieldWork   フィールドマップポインタ
 *
 * @return	event       イベントポインタ
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_WAV_Create(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork)
{
  ENCEFF_CNT_PTR eff_cnt_ptr;
  GMEVENT * event;
  VecFx32 pos = {0, 0, FX32_ONE*(33)};
  //オーバーレイロード(頂点操作エフェクトオーバーレイ)
  eff_cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldWork);
  ENCEFF_LoadVertexEffOverlay(eff_cnt_ptr);

  event = ENCEFF_PRG_Create( gsys, &pos, CreateEffMainEvt, DrawMesh );
  
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * イベント作成
 *
 * @param   gsys        ゲームシステムポインタ
 *
 * @return	event       イベントポインタ
 */
//--------------------------------------------------------------------------------------------
static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  ENCEFF_WAV_WORK *work;
  int size;
  int i;
  size = sizeof(ENCEFF_WAV_WORK);
  //イベント作成
  {
    event = GMEVENT_Create( gsys, NULL, EffMainEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );

    //波の回数セット
    work->WaveCount = WAVE_COUNT;
    work->WaveWait = 0;

    //頂点初期化
    for (i=0;i<VTX_H_NUM*VTX_W_NUM;i++)
    {
      VecFx16 *vec;
      vec = &work->Vtx[i];
      vec->x = ((i%VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->y = ((i/VTX_W_NUM)*FX16_ONE) /  MUL_VAL;
      vec->z = 0;
    }
  }
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * イベント
 *
 * @param   event       イベントポインタ
 * @param   *seq        シーケンサ
 * @param   work        ワークポインタ
 *
 * @return	GMEVENT_RESULT    イベント結果
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work )
{
  ENCEFF_WAV_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    {
      BOOL rc;
      rc = WaveMain(evt_work);
      if (rc)
      {
        //ホワイトアウト開始
        GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, 0 );
        evt_work->WaveWait = WAVE_AFTER_WAIT;
        (*seq)++;
      }
    }
    break;
  case 1:
    //ウェイト
    if ( evt_work->WaveWait != 0)
    {
      evt_work->WaveWait--;
      break;
    }
    //ホワイトアウト待ち
    if ( GFL_FADE_CheckFade() != FALSE ) break;
    return( GMEVENT_RES_FINISH );
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	伝播処理
 *
 *	@param	GFL_TCB *p_tcb
 *	@param	*p_work 
 *
 *	@return BOOL TRUEで終了
 */
//-----------------------------------------------------------------------------
static BOOL WaveMain(void *work)
{
  ENCEFF_WAV_WORK *evt_work;
  int i,max;
  fx16 vtx[VTX_W_NUM*VTX_H_NUM];

  max = VTX_W_NUM*VTX_H_NUM;
  evt_work = (ENCEFF_WAV_WORK *)work;

  //現在の頂点情報を保存
  for(i=0;i<max;i++)
  {
/**    
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
    {
      evt_work->Vtx[i].z = 0;
      evt_work->TmpZ[i] = evt_work->Vtx[i].z;
    }
*/    
    vtx[i] = evt_work->Vtx[i].z;
  }

  if (evt_work->WaveWait == 0)
  {
    //波をたてる
    evt_work->Vtx[12*33+16].z = WAVE_HEIGHT;
    //ウェイトセット
    evt_work->WaveWait = WAVE_WAIT;
    evt_work->WaveCount--;
  }else evt_work->WaveWait--;
  
  //全ての頂点に計算をかける
  for(i=0;i<max;i++)
  {
    fx16 v1,v2,v3,v4;
    int idx;
    int base_w,base_h;
    int w,h;
		//2次座標に変換
		base_w = i%VTX_W_NUM;
		base_h = i/VTX_W_NUM;
    if (i==411){
      OS_Printf("411\n");
    }

    //周囲4つのデータを取得

    //左
    if (base_w-1>=0)
    {
      w = base_w-1;
      h = base_h;
      idx = h*VTX_W_NUM+w;
      v1 = vtx[idx];
    }
    else v1 = 0;
    //右
		if (base_w+1<VTX_W_NUM)
    {
      w = base_w+1;
      h = base_h;
      idx = h*VTX_W_NUM+w;
      v2 = vtx[idx];
    }
    else v2 = 0;
		//上
		if (base_h-1>=0)
    {
      h = base_h-1;
      w = base_w;
      idx = h*VTX_W_NUM+w;
      v3 = vtx[idx];
    }
    else v3 = 0;
		//下
		if(base_h+1<VTX_H_NUM)
    {
      h = base_h+1;
      w = base_w;
      idx = h*VTX_W_NUM+w;
      v4 = vtx[idx];
    }
    else v4 = 0;

    evt_work->TmpZ[i] += ( (v1+v2+v3+v4)/4 - evt_work->Vtx[i].z );

    evt_work->Vtx[i].z += evt_work->TmpZ[i];
    if ( evt_work->Vtx[i].z > FX16_ONE ) evt_work->Vtx[i].z = FX16_ONE;
    if ( evt_work->Vtx[i].z < -FX16_ONE ) evt_work->Vtx[i].z = -FX16_ONE;

    //縁は伝播対象からはずす処理（画面外を見えなくするため）
    {
      int base_w,base_h;
      //2次座標に変換
      base_w = i%VTX_W_NUM;
      base_h = i/VTX_W_NUM;

      if ( (base_w == 0) || (base_w == VTX_W_NUM-1) ||
          (base_h == 0) || (base_h == VTX_H_NUM-1))
      {
        evt_work->Vtx[i].z = 0;
      }
    }
  }

  if (evt_work->WaveCount<=0)
  {
    evt_work->WaveWait = WAVE_AFTER_WAIT;
    return TRUE;
  }

  return FALSE;

}

//--------------------------------------------------------------------------------------------
/**
 * 描画
 *
 * @param   ptr     管理ポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_WAV_Draw( ENCEFF_CNT_PTR ptr )
{
  void *wk = ENCEFF_GetWork(ptr);
  ENCEFF_PRG_PTR prg_ptr = (ENCEFF_PRG_PTR)wk;
  ENCEFF_PRG_Draw(prg_ptr);

//  DrawMesh(ptr);
}

//--------------------------------------------------------------------------------------------
/**
 * 描画
 *
 * @param   ptr     管理ポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawMesh(void *wk)
{
  int i,j;
  
  ENCEFF_WAV_WORK *work = (ENCEFF_WAV_WORK*)wk;

  G3_PushMtx();
  G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

  G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);

  //ライトカラー
  G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
  // ポリゴンアトリビュート設定
	G3_PolygonAttr(
				   GX_LIGHTMASK_0,			  // ライトを反映
				   GX_POLYGONMODE_MODULATE,	  // モジュレーションポリゴンモード
				   //GX_CULL_BACK,             // カリング
				   GX_CULL_NONE,             // カリング
				   0,                         // ポリゴンＩＤ ０
				   31,					  // アルファ値
				   GX_POLYGON_ATTR_MISC_NONE );


  {
    VecFx32 pos = {-FX32_ONE*16,FX32_ONE*12,0};
    VecFx32 scale = {FX32_ONE*MUL_VAL, FX32_ONE*MUL_VAL, FX32_ONE*Z_MUL_VAL};
    fx32  s0, t0, t1;

    // 位置設定
		G3_Translate(pos.x, pos.y, pos.z);
    // スケール設定
		G3_Scale(scale.x, scale.y, scale.z);

    for (i=0;i<POLY_H_NUM;i++)
    {
      G3_Begin( GX_BEGIN_QUAD_STRIP );
      for (j=0;j<VTX_W_NUM;j++)
      {
        fx16 x1,x2,y1,y2,z1,z2;
        

        s0 = j * 8 * FX32_ONE;
        t0 = i * 8 * FX32_ONE;
        t1 = t0 + (8 * FX32_ONE);
        
        x1 = work->Vtx[i*VTX_W_NUM+j].x;
        x2 = work->Vtx[(i+1)*VTX_W_NUM+j].x;
        y1 = work->Vtx[i*VTX_W_NUM+j].y;
        y2 = work->Vtx[(i+1)*VTX_W_NUM+j].y;
        z1 = work->Vtx[i*VTX_W_NUM+j].z;
        z2 = work->Vtx[(i+1)*VTX_W_NUM+j].z;

        G3_Normal( 0, 0, FX16_ONE );

        G3_TexCoord(s0, t0);
	      G3_Vtx( x1, -y1, z1 );
        G3_TexCoord(s0, t1);
	      G3_Vtx( x2, -y2, z2 );
      }
      G3_End();
    }
  }

  G3_PopMtx(1);
}


