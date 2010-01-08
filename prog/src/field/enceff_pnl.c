//======================================================================
/*
 * @file	enceff_pnl.c
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================

#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"


#include "enceff_pnl.h"

//--------------------------------------------------------------
/// ENCEFF_PNL_WORK
//--------------------------------------------------------------
typedef struct ENCEFF_PNL_WORK_tag
{
/**  
  PANEL_WK Panel[POLY_W_NUM_MAX*POLY_H_NUM_MAX];
  MOVE_START_PRM StartPrm;
  u16 PanelNumW;
  u16 PanelNumH;
*/  
  PNL_EFF_WORK PnlEffWork;
  PANEL_START_FUNC StartFunc;
}ENCEFF_PNL_WORK;

static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work );

static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);

static BOOL MoveMain(ENCEFF_PNL_WORK *evt_work);


//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_PNL_CreateEffMainEvt(GAMESYS_WORK *gsys, PNL_EFF_PARAM *param)
{
  GMEVENT * event;
  ENCEFF_PNL_WORK *work;
  int size;
  int i;
  size = sizeof(ENCEFF_PNL_WORK);
  //�C�x���g�쐬
  {
    int char_x, char_y;
    int poly_w_num, poly_h_num;
    int poly_w, poly_h;
    int disp_ofs_w, disp_ofs_h;
    event = GMEVENT_Create( gsys, NULL, EffMainEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );

    char_x = param->CharX;
    char_y = param->CharY;

    switch(char_x){
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
      break;
    default:
      GF_ASSERT(0);
    }

    switch(char_y){
    case 1:
    case 2:
    case 4:
    case 8:
      break;
    default:
      GF_ASSERT(0);
    }

    poly_w = char_x*8;
    poly_h = char_y*8;
    poly_w_num = POLY_W_NUM_MAX/char_x;
    poly_h_num = POLY_H_NUM_MAX/char_y;
    work->PnlEffWork.PanelNumW = poly_w_num;
    work->PnlEffWork.PanelNumH = poly_h_num;
    disp_ofs_w = (poly_w_num/2)*poly_w-(poly_w/2);
    disp_ofs_h = (poly_h_num/2)*poly_h-(poly_h/2);
    //�p�l��������
    for (i=0;i<poly_w_num*poly_h_num;i++)
    {
      int x,y;
      PANEL_WK* panel = &work->PnlEffWork.Panel[i];
      x = i%poly_w_num;
      y = i/poly_w_num;
      panel->Pos.x = (x * poly_w - disp_ofs_w) * FX32_ONE;
      panel->Pos.y = (-(y * poly_h) + disp_ofs_h) * FX32_ONE;
      panel->Pos.z = 0;
      panel->Rot.x = 0;
      panel->Rot.y = 0;
      panel->Rot.z = 0;
      panel->MoveOnFlg = FALSE;
      panel->MoveEndFlg = FALSE;
      panel->MoveFunc = param->MoveFunc;
    }
    work->StartFunc = param->StartFunc;

  }
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g
 *
 * @param   event       �C�x���g�|�C���^
 * @param   *seq        �V�[�P���T
 * @param   work        ���[�N�|�C���^
 *
 * @return	GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work )
{
  ENCEFF_PNL_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    {
      BOOL rc = FALSE;
      //���C������
      rc = MoveMain(evt_work);      
      if (rc)
      {
        //�z���C�g�A�E�g�J�n
        GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, 0 );
        (*seq)++;
      }
    }
    break;
  case 1:
    //�z���C�g�A�E�g�҂�
    if ( GFL_FADE_CheckFade() != FALSE ) break;
    return( GMEVENT_RES_FINISH );
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐�
 *
 * @param   evt_work  ���[�N�|�C���^
 *
 * @return	BOOL    TRUE�œ���I��
 */
//--------------------------------------------------------------------------------------------
static BOOL MoveMain(ENCEFF_PNL_WORK *evt_work)
{
  BOOL end;
  int i;
  int max, end_count;
  PNL_EFF_WORK *eff_work;
  PANEL_WK *ioPanel;
  eff_work = &evt_work->PnlEffWork;
  ioPanel = eff_work->Panel;

  evt_work->StartFunc(eff_work);
  
  end_count = 0;
  max = eff_work->PanelNumW * eff_work->PanelNumH;

  for(i=0;i<max;i++)
  {
    BOOL rc;
    PANEL_WK *panel;
    panel = &ioPanel[i];
    rc = FALSE;

    if (!panel->MoveEndFlg)
    {
      if (panel->MoveOnFlg)
      {
        //����֐����s
        if (panel->MoveFunc == NULL)
        {
          GF_ASSERT(0);
          panel->MoveEndFlg = TRUE;
        }
        else {
          rc = panel->MoveFunc(panel);
        }
      }
    }
    else rc = TRUE;

    if (rc) end_count++;
  }

  if (end_count >= max) end = TRUE;
  else end = FALSE;

  return end;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��
 *
 * @param   ptr     �Ǘ��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_PNL_Draw( ENCEFF_CNT_PTR ptr )
{
  void *wk = ENCEFF_GetWork(ptr);
  ENCEFF_PRG_PTR prg_ptr = (ENCEFF_PRG_PTR)wk;
  ENCEFF_PRG_Draw(prg_ptr);
}

//--------------------------------------------------------------------------------------------
/**
 * �`��
 *
 * @param   ptr     �Ǘ��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ENCEFF_PNL_DrawMesh(void *wk)
{
  int i;

  ENCEFF_PNL_WORK *work = (ENCEFF_PNL_WORK*)wk;

  G3_PushMtx();

  G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

  G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);


  //���C�g�J���[
  G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
  // �|���S���A�g���r���[�g�ݒ�
	G3_PolygonAttr(
				   GX_LIGHTMASK_0,			  // ���C�g�𔽉f
				   GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
				   GX_CULL_BACK,             // �J�����O
				   //GX_CULL_NONE,             // �J�����O
				   0,                         // �|���S���h�c �O
				   31,					  // �A���t�@�l
				   GX_POLYGON_ATTR_MISC_NONE );


  {
    VecFx32 pos = {0,0,0};
    VecFx32 scale = {FX32_ONE, FX32_ONE, FX32_ONE};
    fx32  s0, s1, t0, t1;
    int max, w, h;
    max = work->PnlEffWork.PanelNumW * work->PnlEffWork.PanelNumH;
    w = (POLY_W_NUM_MAX/work->PnlEffWork.PanelNumW)*8;
    h = (POLY_H_NUM_MAX/work->PnlEffWork.PanelNumH)*8;
    scale.x = FX32_ONE * (w/8);
    scale.y = FX32_ONE * (h/8);
    for (i=0;i<max;i++)
    {
      int x,y;
      PANEL_WK* panel = &work->PnlEffWork.Panel[i];
      G3_PushMtx();
      x = i%work->PnlEffWork.PanelNumW;
      y = i/work->PnlEffWork.PanelNumW;

      pos = panel->Pos;

      s0 = x * w * FX32_ONE;
      t0 = y * h * FX32_ONE;
      s1 = s0 + (w * FX32_ONE);
      t1 = t0 + (h * FX32_ONE);
      // �ʒu�ݒ�
		  G3_Translate(pos.x, pos.y, pos.z);
      // �X�P�[���ݒ�
		  G3_Scale(scale.x, scale.y, scale.z);

      {
        MtxFx33 mtx;
        MTX_RotY33(&mtx, FX_SinIdx(panel->Rot.y), FX_CosIdx(panel->Rot.y));
        G3_MultMtx33(&mtx);
        MTX_RotZ33(&mtx, FX_SinIdx(panel->Rot.z), FX_CosIdx(panel->Rot.z));
        G3_MultMtx33(&mtx);
      }

      G3_Begin( GX_BEGIN_QUADS );

      G3_Normal( 0, 0, FX32_ONE );

      G3_TexCoord(s0, t0);
	    G3_Vtx( -(FX16_ONE*4), (FX16_ONE*4), 0 * FX16_ONE );
      G3_TexCoord(s0, t1);
	    G3_Vtx( -(FX16_ONE*4), -(FX16_ONE*4), 0 * FX16_ONE );
      G3_TexCoord(s1, t1);
	    G3_Vtx( (FX16_ONE*4), -(FX16_ONE*4), 0 * FX16_ONE );
      G3_TexCoord(s1, t0);
	    G3_Vtx( (FX16_ONE*4), (FX16_ONE*4),  0 * FX16_ONE );

      G3_End();
      G3_PopMtx(1);
    }
  }

  G3_PopMtx(1);
}
