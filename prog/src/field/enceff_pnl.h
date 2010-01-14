//======================================================================
/*
 * @file	enceff_pnl.h
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================

#pragma once

#include "gamesystem/gamesystem.h"  //for GMEVENT

//��1��8���j�b�g�ōl����΁A32��24�������ł��邪�A�q�[�v�e�ʂ̓s���ōő��16��12(1��16���j�b�g)�ɂ��Ă���

#define POLY_W_NUM_MAX  (32)
#define POLY_H_NUM_MAX  (24)

#define PANEL_W_NUM_MAX  (16)
#define PANEL_H_NUM_MAX  (12)

#define WORK_SIZE   (100)

struct PNL_EFF_WORK_tag;
struct PANEL_WK_tag;

typedef void (*PANEL_INIT_FUNC)(struct PNL_EFF_WORK_tag *);
typedef void (*PANEL_START_FUNC)(struct PNL_EFF_WORK_tag *);
typedef BOOL (*PANEL_MOVE_FUNC)(struct PANEL_WK_tag *);

typedef struct PANEL_WK_tag
{
  VecFx32 Pos;
  VecFx32 Scale;
  VecFx32 Rot;
  VecFx32 Spd;
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

typedef struct PNL_EFF_WORK_tag
{
  PANEL_WK Panel[PANEL_W_NUM_MAX*PANEL_H_NUM_MAX];
  MOVE_START_PRM StartPrm;
  u16 PanelNumW;
  u16 PanelNumH;
  u8  Work[WORK_SIZE];
}PNL_EFF_WORK;

typedef struct PNL_EFF_PARAM_tag
{
  int CharX;
  int CharY;
  PANEL_INIT_FUNC InitFunc;
  PANEL_START_FUNC StartFunc;
  PANEL_MOVE_FUNC MoveFunc;
}PNL_EFF_PARAM;


extern GMEVENT * ENCEFF_PNL_CreateEffMainEvt(GAMESYS_WORK *gsys, PNL_EFF_PARAM *param);
extern void ENCEFF_PNL_DrawMesh(void *wk);
