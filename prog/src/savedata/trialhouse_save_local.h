//==============================================================================
/**
 * @file    trialhouse_save.h
 * @brief   �g���C�A���n�E�X�Z�[�u�f�[�^
 * @author  saito
 */
//==============================================================================
#pragma once


#define TH_MONS_DATA_MAX  (4)
#define TH_TITLE_MSG_MAX (16+1)   ///< ���b�Z�[�W�̐� battle_exmination.h��BATTLE_EXAMINATION_TITLE_MSG_MAX�Ɠ����T�C�Y

//==============================================================================
//  �\���̒�`
//==============================================================================
///�Z�[�u���[�N
typedef struct TH_MONS_DATA_tag
{
  u16 MonsNo;
  u8 FormNo;
  u8 Sex;
}TH_MONS_DATA;      //4byte

typedef struct TH_SV_COMMON_WK_tag
{
  u8 Valid;         //0�@or 1  1�ŗL���f�[�^
  u8 IsDouble;      //�_�u���o�g���f�[�^���H    0:�V���O���@1�G�_�u��
  u16 Point;        //����|�C���g
  TH_MONS_DATA MonsData[TH_MONS_DATA_MAX];   //�ő�S�C
}TH_SV_COMMON_WK;   //4+4x4 = 20byte
 
struct _THSV_WORK
{
  TH_SV_COMMON_WK CommonData[2];      //�Y�����@0�G�q�n�l�f�[�^�@1�F�_�E�����[�h�f�[�^
  u8 DownloadBits[16];                   //�_�E�����[�h�p�r�b�g�Q
  STRCODE Name[TH_TITLE_MSG_MAX];                        //���薼�@�_�E�����[�h���薼
  u8 padding[2];
};    //20x2+16+34+2 = 92byte
