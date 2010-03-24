//======================================================================
/*
 * @file	enceff_pnl3.c
 * @brief	�C�x���g�F�G���J�E���g�G�t�F�N�g
 * @author saito
 */
//======================================================================

#include "enceff.h"
#include "enceff_prg.h"
#include "fieldmap.h"

#include "system/main.h"

#include "enceff_pnl.h"

#define BUFF_SIZE (96)
//static u16 BuffIdx[BUFF_SIZE];
//static u16 BuffIdxNext[BUFF_SIZE];
//static int BuffCount = 0;
//static int BuffCountNext = 0;

typedef struct BUFFER_WORK_tag
{
  u16 BuffIdx[BUFF_SIZE];
  u16 BuffIdxNext[BUFF_SIZE];
  u16 BuffCount;
  u16 BuffCountNext;
}BUFFER_WORK;


static GMEVENT *CreateEffMainEvt(GAMESYS_WORK *gsys);

static BOOL MoveFunc(PANEL_WK *panel);
static void StartFunc(PNL_EFF_WORK *work);
static void InitFunc(PNL_EFF_WORK *work);

static BOOL CheckEntry(const int inIdx, BUFFER_WORK *buf_work);
static void EntryCore(PNL_EFF_WORK *work, const int inX, const int inZ);
static void Entry(PNL_EFF_WORK *work, const int inIdx);
static void ReferenceEntry(PNL_EFF_WORK *work);

//--------------------------------------------------------------------------------------------
/**
 * �C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inIsFadeWhite �G�t�F�N�g�I���̓z���C�g�A�E�g���H
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *ENCEFF_PNL3_Create(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const BOOL inIsFadeWhite)
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
  BUFFER_WORK *buf_work;

  param.CharX = 2;
  param.CharY = 2;
  param.InitFunc = InitFunc;
  param.StartFunc = StartFunc;
  param.MoveFunc = MoveFunc;

  event = ENCEFF_PNL_CreateEffMainEvt(gsys, &param, TRUE);

  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �������֐�
 *
 * @param   work        �p�l���G�t�F�N�g���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitFunc(PNL_EFF_WORK *work)
{
  BUFFER_WORK *buf_work = (BUFFER_WORK *)work->Work;

  buf_work->BuffCountNext = 0;
  buf_work->BuffCount = 0;
  
  {
    int x,y;
    int idx;
    x = 3;//5;
    y = 3;//5;
    idx = 16*y+x;
    buf_work->BuffIdxNext[0] = idx;
    buf_work->BuffCountNext++;

    x = 5;//10;
    y = 9;//18;
    idx = 16*y+x;
    buf_work->BuffIdxNext[1] = idx;
    buf_work->BuffCountNext++;

    x = 11;//22;
    y = 5;//10;
    idx = 16*y+x;
    buf_work->BuffIdxNext[2] = idx;
    buf_work->BuffCountNext++;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �J�n�֐�
 *
 * @param   work        �p�l���G�t�F�N�g���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void StartFunc(PNL_EFF_WORK *work)
{
  MOVE_START_PRM *prm = &work->StartPrm;
  BUFFER_WORK *buf_work = (BUFFER_WORK *)work->Work;

  if ( prm->Count < work->PanelNumW * work->PanelNumH)
  {
    if (prm->Wait == 0)
    {
      int i;
      for (i=0;i<buf_work->BuffCount;i++)
      {
        Entry(work, buf_work->BuffIdx[i]);
      }
      ReferenceEntry(work);

      prm->Wait = 1;
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
 * @param   panel        �p�l�����[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static BOOL MoveFunc(PANEL_WK *panel)
{
//  if ( panel->Count < 32 )
  if ( panel->Pos.x < FX32_ONE*256 )
  {
    panel->Pos.z = FX32_ONE;
    panel->Pos.x += panel->Spd.x;
    panel->Spd.x += FX32_ONE;
/**    
    panel->Rot.z += 0x1000;
    if ( panel->Rot.z >= 0x10000){
      panel->Rot.z = panel->Rot.z-0x10000;
    }
*/
    if ( panel->Rot.y < 0x8000){
      panel->Rot.y += 0x200;
    }
    else panel->Rot.y = 0x8000;

//    panel->Count++;
  }
  else panel->MoveEndFlg = TRUE;

  return panel->MoveEndFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * �G���g���`�F�b�N
 *
 * @param   inIdx
 * @param   buf_work    �o�b�t�@���[�N 
 *
 * @return	BOOL  TRUE�ŃG���g����
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckEntry(const int inIdx, BUFFER_WORK *buf_work)
{
  int i;
  for(i=0;i<buf_work->BuffCountNext;i++)
  {
    if ( buf_work->BuffIdxNext[i] == inIdx ) return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �G���g���`�F�b�N
 *
 * @param   work        �p�l���G�t�F�N�g���[�N�|�C���^
 * @param   inX         �G���g���p�l���w���W
 * @param   inZ         �G���g���p�l���y���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void EntryCore(PNL_EFF_WORK *work, const int inX, const int inZ)
{
  BUFFER_WORK *buf_work = (BUFFER_WORK *)work->Work;
  int idx = inZ * work->PanelNumW + inX;
  PANEL_WK *panel = &work->Panel[idx];
  if ( !panel->MoveOnFlg )
  {
    GF_ASSERT(buf_work->BuffCountNext<BUFF_SIZE);
    if ( !CheckEntry(idx, buf_work) )
    {
      buf_work->BuffIdxNext[buf_work->BuffCountNext] = idx;
      buf_work->BuffCountNext++;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �w��p�l���̎��͂̃p�l�����o�b�t�@�ɓo�^
 *
 * @param   work        �p�l���G�t�F�N�g���[�N�|�C���^
 * @param   inIdx       �w��p�l���C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Entry(PNL_EFF_WORK *work, const int inIdx)
{
  int base_x, base_z, x, z;
  base_x = inIdx % work->PanelNumW;
  base_z = inIdx / work->PanelNumW;

  //��
  {
    x = base_x;
    z = base_z-1;
    if (z >= 0) EntryCore(work, x, z);
  }
  //�E
  {
    x = base_x+1;
    z = base_z;
    if (x < work->PanelNumW) EntryCore(work, x, z);
  }
  //��
  {
    x = base_x;
    z = base_z+1;
    if (z < work->PanelNumH) EntryCore(work, x, z);
  }
  //��
  {
    x = base_x-1;
    z = base_z;
    if (x >= 0) EntryCore(work, x, z);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �G���g���𔽉f
 *
 * @param   work        �p�l���G�t�F�N�g���[�N�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ReferenceEntry(PNL_EFF_WORK *work)
{
  int i;
  MOVE_START_PRM *prm = &work->StartPrm;
  BUFFER_WORK *buf_work = (BUFFER_WORK *)work->Work;

  for (i=0;i<buf_work->BuffCountNext;i++)
  {
    PANEL_WK *panel;
    int idx;
    idx = buf_work->BuffIdxNext[i];
    panel = &work->Panel[idx];

    if ( panel->MoveOnFlg ) continue;
    
    panel->MoveOnFlg = TRUE;
    prm->Count++;
    //�����X�s�[�h
    panel->Spd.x = FX32_ONE;

    buf_work->BuffIdx[i] = buf_work->BuffIdxNext[i];
  }

  buf_work->BuffCount = buf_work->BuffCountNext;
  buf_work->BuffCountNext = 0;
}


