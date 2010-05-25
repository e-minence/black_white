//============================================================================================
/**
 * @file    corporate.h
 * @brief   ��Ж��\��
 * @author  Hiroyuki Nakamura
 * @date    2009.12.16
 */
//============================================================================================
#pragma once


#ifdef  PM_DEBUG
// �I�����[�h
enum {
  CORPORATE_RET_NORMAL = 0,   // �ʏ�I��
  CORPORATE_RET_SKIP,         // �X�L�b�v
  CORPORATE_RET_DEBUG,        // �f�o�b�O��
};
#endif

#define ALL_COPORATE_VIEW  ( 0 )  // �C�V���E�i���j�|�P�������\��
#define ONLY_DEVELOP_VIEW  ( 1 )  // �����ҁE�J���̂ݕ\��

// �N�����p�����[�^
typedef struct{
  int mode;   // �N�����[�h�iALL_CORPORATE_VIEW or ONLY_DEVELOP_VIEW)
  int ret;    // �f�o�b�O���͖߂�l������
} CORP_PARAM;

// PROC�f�[�^
extern const GFL_PROC_DATA CorpProcData;
