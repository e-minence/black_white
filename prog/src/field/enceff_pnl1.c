//======================================================================
/*
 * @file	enceff_pnl1.c
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================

#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"

#include "enceff_pnl.h"

#define ROT_NUM (1)

static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);

static BOOL MoveFunc(PANEL_WK *panel);
static void StartFunc(PNL_EFF_WORK *work);

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   fieldWork   �t�B�[���h�}�b�v�|�C���^
 * @param   inIsFadeWhite �G�t�F�N�g�I���̓z���C�g�A�E�g���H
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_PNL1_Create(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const BOOL inIsFadeWhite)
{
  ENCEFF_CNT_PTR eff_cnt_ptr;
  GMEVENT * event;
  VecFx32 pos = {0, 0, FX32_ONE*(264)};
  
  //�I�[�o�[���C���[�h(�p�l���G�t�F�N�g�I�[�o�[���C)
  eff_cnt_ptr = FIELDMAP_GetEncEffCntPtr(fieldWork);
  ENCEFF_LoadPanelEffOverlay(eff_cnt_ptr);

  event = ENCEFF_PRG_Create( gsys, &pos, CreateEffMainEvt, ENCEFF_PNL_DrawMesh );
  
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
  PNL_EFF_PARAM param;

  param.CharX = 4;
  param.CharY = 4;
  param.InitFunc = NULL;
  param.StartFunc = StartFunc;
  param.MoveFunc = MoveFunc;

  event = ENCEFF_PNL_CreateEffMainEvt(gsys, &param, FALSE);

  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �t�@���N�V�����X�^�[�g
 *
 * @param   work        ���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void StartFunc(PNL_EFF_WORK *work)
{
  MOVE_START_PRM *prm = &work->StartPrm;

  if ( prm->Count < work->PanelNumW*work->PanelNumH )
  {
    if (prm->Wait == 0)
    {
      PANEL_WK *panel;
      panel = &work->Panel[prm->Count];
      panel->MoveOnFlg = TRUE;

      prm->Count++;
      prm->Wait = 0;
    }
    else
    {
      prm->Wait--;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐�
 *
 * @param   panel   �p�l�����[�N�|�C���^
 *
 * @return	BOOL    TRUE�ŏI���Z�b�g
 */
//--------------------------------------------------------------------------------------------
static BOOL MoveFunc(PANEL_WK *panel)
{
  if ( panel->Count < ROT_NUM )
  {
    panel->Rot.y += 0x1000;
    if ( panel->Rot.y >= 0x10000 )
    {
      panel->Rot.y -= 0x10000;
      panel->Count++;
    }
  }
  else if (panel->Count == ROT_NUM ){
    panel->Rot.y += 0x1000;
    if ( panel->Rot.y >= 0x8000 )
    {
      panel->Rot.y = 0x8000;
      panel->Count++;
    }
  }
  else panel->MoveEndFlg = TRUE;

  return panel->MoveEndFlg;
}



