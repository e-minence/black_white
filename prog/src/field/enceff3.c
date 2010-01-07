//======================================================================
/*
 * @file	enceff3.c
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================

#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"

#define POLY_W_NUM_MAX  (32)
#define POLY_H_NUM_MAX  (24)

#define DISP_OFSET_W  ((16*8-4)*FX32_ONE)
#define DISP_OFSET_H  ((12*8-4)*FX32_ONE)

//�O���錾
struct PANEL_WK_tag;

typedef BOOL (*PANEL_MOVE_FUNC)(struct PANEL_WK_tag *panel);

typedef struct PANEL_WK_tag
{
  VecFx32 Pos;
  VecFx32 Scale;
  VecFx32 Rot;
  int Count;
  BOOL MoveOnFlg;
  BOOL MoveEndFlg;
  PANEL_MOVE_FUNC MoveFunc;
}PANEL_WK;

typedef struct MOVE_START_PRM_tag
{
  int Count;
  int Wait;
}MOVE_START_PRM;

//--------------------------------------------------------------
/// ENCEFF3_WORK
//--------------------------------------------------------------
typedef struct
{
  PANEL_WK Panel[POLY_W_NUM_MAX*POLY_H_NUM_MAX];
  MOVE_START_PRM StartPrm;
  u16 PanelNumW;
  u16 PanelNumH;
}ENCEFF3_WORK;

static void DrawMesh(void *wk);
static GMEVENT_RESULT EffMainEvt( GMEVENT* event, int* seq, void* work );
//static GMEVENT_RESULT EffEvt( GMEVENT* event, int* seq, void* work );

static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);

static BOOL _MoveFunc(PANEL_WK *panel);
static BOOL _MoveFunc2(PANEL_WK *panel);
static BOOL MoveMain(ENCEFF3_WORK *evt_work);
static void MoveStart(ENCEFF3_WORK *evt_work);
static void MoveStart2(ENCEFF3_WORK *evt_work);
//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   fieldWork   �t�B�[���h�}�b�v�|�C���^
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_CreateEff3(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork)
{
  GMEVENT * event;
  VecFx32 pos = {0, 0, FX32_ONE*(264)};
  //�I�[�o�[���C���[�h
  ;
  event = ENCEFF_PRG_Create( gsys, &pos, CreateEffMainEvt, DrawMesh );
  
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  ENCEFF3_WORK *work;
  int size;
  int i;
  size = sizeof(ENCEFF3_WORK);
  //�C�x���g�쐬
  {
    int char_x, char_y;
    int poly_w_num, poly_h_num;
    int poly_w, poly_h;
    int disp_ofs_w, disp_ofs_h;
    event = GMEVENT_Create( gsys, NULL, EffMainEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );

    char_x = 2;
    char_y = 2;

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
    work->PanelNumW = poly_w_num;
    work->PanelNumH = poly_h_num;
    disp_ofs_w = (poly_w_num/2)*poly_w-(poly_w/2);
    disp_ofs_h = (poly_h_num/2)*poly_h-(poly_h/2);
    //�p�l��������
    for (i=0;i<poly_w_num*poly_h_num;i++)
    {
      int x,y;
      PANEL_WK* panel = &work->Panel[i];
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
      panel->MoveFunc = _MoveFunc2; //@todo
    }

  }
  return event;
}
#if 0
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
static GMEVENT_RESULT EffEvt( GMEVENT* event, int* seq, void* work )
{
  ENCEFF3_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
    (*seq)++;
  case 1:
    if( GFL_FADE_CheckFade() == FALSE ){
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 16, 0, -1 );
      (*seq)++;
    }
    break;
  case 2:
    if( GFL_FADE_CheckFade() == FALSE ){
      evt_work->Count++;
      
      if( evt_work->Count < 3 ){
        GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
        (*seq) = 1;
      }else{
				(*seq)++;
      }
    }
    break;
  case 3:
    {
      //�L���v�`�����N�G�X�g
      ReqCapture(evt_work);
      (*seq)++;
    }
    break;
  case 4:
    //�L���v�`���I���҂�
    if (evt_work->CapEndFlg){
      //�L���v�`���I�������A�v���C�I���e�B�ύX�ƕ\���ύX
      GFL_BG_SetPriority( 0, 0 );
      GFL_BG_SetPriority( 2, 3 );

      evt_work->dMode = FIELDMAP_GetDraw3DMode(fieldmap);

      // VRAM�o���N�Z�b�g�A�b�v(VRAM_D���e�N�X�`���Ƃ��Ďg�p)
      evt_work->vTex = GX_GetBankForTex(); 
      evt_work->vTexRecover = FALSE;
      if(!(evt_work->vTex & GX_VRAM_D)){
        evt_work->vTex |= GX_VRAM_D;
        evt_work->vTexRecover = TRUE;
      }
      GX_DisableBankForLCDC();
      GX_SetBankForTex(evt_work->vTex);
      SetVramKey(evt_work);
      
      (*seq)++;
    }
    break;
  case 5:
    //�Z�b�g�A�b�v�I�������̂�3�c�ʂ��I��
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
    
    //�t�B�[���h�\�����[�h�ؑ�
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_ENCEFF);

    (*seq)++;
    break;
  case 6:
    {
      BOOL rc = FALSE;
      //����J�n�Ǘ� @todo
      MoveStart2(evt_work);
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
  case 7:
    //�z���C�g�A�E�g�҂�
    if ( GFL_FADE_CheckFade() != FALSE ) break;

    // VRAM�o���N���A
		if(evt_work->vTexRecover == TRUE){
			evt_work->vTex &= (GX_VRAM_D^0xffffffff);
			GX_SetBankForTex(evt_work->vTex); 
		}

		FIELDMAP_SetDraw3DMode(fieldmap, evt_work->dMode);
    //�J�������
    GFL_G3D_CAMERA_Delete(evt_work->g3Dcamera);
    return( GMEVENT_RES_FINISH );
  }

  return GMEVENT_RES_CONTINUE;
}
#endif
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
  ENCEFF3_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    {
      BOOL rc = FALSE;
      //����J�n�Ǘ� @todo
      MoveStart2(evt_work);
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



//����J�n�Ǘ��֐�
static void MoveStart(ENCEFF3_WORK *evt_work)
{
  MOVE_START_PRM *prm = &evt_work->StartPrm;
/**  
  if ( prm->Count < 32 )
  {
    if (prm->Wait == 0)
    {
      int i;
      for(i=0;i<24;i++)
      {
        PANEL_WK *panel = &evt_work->Panel[(i*32)+prm->Count];
        panel->MoveOnFlg = TRUE;
      }
      prm->Count++;
      prm->Wait = 1;
    }
    else
    {
      prm->Wait--;
    }
  }
*/
  if ( prm->Count < evt_work->PanelNumW/2 )
  {
    ;if (prm->Wait == 0)
    {
      int i;
      for(i=0;i<evt_work->PanelNumH;i++)
      {
        PANEL_WK *panel;
        int left = prm->Count;
        int right = (evt_work->PanelNumW-1)-prm->Count;
        panel = &evt_work->Panel[(i*evt_work->PanelNumW)+left];
        panel->MoveOnFlg = TRUE;
        panel = &evt_work->Panel[(i*evt_work->PanelNumW)+right];
        panel->MoveOnFlg = TRUE;
      }
      prm->Count++;
      prm->Wait = 1;
    }
    else
    {
      prm->Wait--;
    }
  }

}

static void MoveStart2(ENCEFF3_WORK *evt_work)
{
  MOVE_START_PRM *prm = &evt_work->StartPrm;

  if ( prm->Count < evt_work->PanelNumW )
  {
    if (prm->Wait == 0)
    {
      int i;
      for(i=0;i<evt_work->PanelNumH;i++)
      {
        PANEL_WK *panel = &evt_work->Panel[(i*evt_work->PanelNumW)+prm->Count];
        panel->MoveOnFlg = TRUE;
      }
      prm->Count++;
      prm->Wait = 1;
    }
    else
    {
      prm->Wait--;
    }
  }
}


//����̃p�l���ɑ΂��铮��֐�
static BOOL _MoveFunc(PANEL_WK *panel)
{
  if ( panel->Count < 32 )
  {
//    panel->Pos.x += (FX32_ONE*4);
    panel->Pos.z += (FX32_ONE*8);
    panel->Count++;
  }
  else panel->MoveEndFlg = TRUE;

  return panel->MoveEndFlg;
}

//����̃p�l���ɑ΂��铮��֐�
static BOOL _MoveFunc2(PANEL_WK *panel)
{
  if ( panel->Count < 64 )
  {
    panel->Rot.y += 0x200;
    panel->Count++;
  }
  else panel->MoveEndFlg = TRUE;

  return panel->MoveEndFlg;
}



static BOOL MoveMain(ENCEFF3_WORK *evt_work)
{
  BOOL end;
  int i;
  int max, end_count;

  PANEL_WK *ioPanel = evt_work->Panel;
  
  end_count = 0;
  max = evt_work->PanelNumW*evt_work->PanelNumH;

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
void ENCEFF_DrawEff3( ENCEFF_CNT_PTR ptr )
{
  void *wk = ENCEFF_GetWork(ptr);
  ENCEFF_PRG_PTR prg_ptr = (ENCEFF_PRG_PTR)wk;
  ENCEFF_PRG_Draw(prg_ptr);
  //DrawMesh(ptr);
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
static void DrawMesh(void *wk)
{
  int i;

  ENCEFF3_WORK *work = (ENCEFF3_WORK*)wk;

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
    max = work->PanelNumW * work->PanelNumH;
    w = (POLY_W_NUM_MAX/work->PanelNumW)*8;
    h = (POLY_H_NUM_MAX/work->PanelNumH)*8;
    scale.x = FX32_ONE * (w/8);
    scale.y = FX32_ONE * (h/8);
    for (i=0;i<max;i++)
    {
      int x,y;
      PANEL_WK* panel = &work->Panel[i];
      G3_PushMtx();
      x = i%work->PanelNumW;
      y = i/work->PanelNumW;

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

