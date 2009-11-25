//==============================================================================
/**
 * @file    monolith_mission_select.c
 * @brief   ���m���X�F�~�b�V��������
 * @author  matsuda
 * @date    2009.11.20(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "monolith_common.h"
#include "arc_def.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  u32 work;
}MISSION_SELECT_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MissionSelectProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MissionSelectProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MissionSelectProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_MissionSelect = {
  MissionSelectProc_Init,
  MissionSelectProc_Main,
  MissionSelectProc_End,
};



//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MissionSelectProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MISSION_SELECT_WORK *msw = mywk;
	ARCHANDLE *hdl;
  
  msw = GFL_PROC_AllocWork(proc, sizeof(MISSION_SELECT_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(msw, sizeof(MISSION_SELECT_WORK));
  
	//�t�@�C���I�[�v��
	hdl = GFL_ARC_OpenDataHandle(ARCID_MONOLITH, HEAPID_MONOLITH);
  {
    ;
  }
	//�t�@�C���N���[�Y
	GFL_ARC_CloseDataHandle(hdl);

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MissionSelectProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MISSION_SELECT_WORK *msw = mywk;

  switch(*seq){
  case 0:
    return GFL_PROC_RES_FINISH;
  }
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MissionSelectProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MISSION_SELECT_WORK *msw = mywk;
  
  GFL_PROC_FreeWork(proc);
  return GFL_PROC_RES_FINISH;
}
