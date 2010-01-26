//============================================================================
/**
 *
 *	@file		comm_battle_demo.h
 *	@brief  �ʐM�o�g���f�� 
 *	@author hosaka genya
 *	@data		2010.01.20
 *
 */
//============================================================================
#pragma once

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//--------------------------------------------------------------
///	�ʐM�o�g���f�� �N���^�C�v
//==============================================================
typedef enum
{ 
  COMM_BTL_DEMO_TYPE_NORMAL_START,  ///< �m�[�}���J�n
  COMM_BTL_DEMO_TYPE_NORMAL_END,    ///< �m�[�}���I��
  COMM_BTL_DEMO_TYPE_MULTI_START,   ///< �}���`�o�g���J�n
  COMM_BTL_DEMO_TYPE_MULTI_END,     ///< �}���`�o�g���I��
} COMM_BTL_DEMO_TYPE;

//--------------------------------------------------------------
/// �o�g���̏��s
//==============================================================
typedef enum {
  COMM_BTL_DEMO_RESULT_WIN = 0, ///< ����
  COMM_BTL_DEMO_RESULT_LOSE,    ///< ����
  COMM_BTL_DEMO_RESULT_DRAW,    ///< ��������
  COMM_BTL_DEMO_RESULT_MAX,
} COMM_BTL_DEMO_RESULT;

//--------------------------------------------------------------
///	�g���[�i�[ID�z���ID
//==============================================================
enum
{ 
  COMM_BTL_DEMO_TRDATA_A = 0,    ///< �}���`�o�g���̎��A���j�I�����[���ō���̈ʒu / �ʏ펞�͎���
  COMM_BTL_DEMO_TRDATA_B,        ///< �}���`�o�g���̎��A���j�I�����[���ō����̈ʒu / �ʏ펞�͑���
  COMM_BTL_DEMO_TRDATA_C,        ///< �}���`�o�g���̎��A���j�I�����[���ŉE��̈ʒu
  COMM_BTL_DEMO_TRDATA_D,        ///< �}���`�o�g���̎��A���j�I�����[���ŉE���̈ʒu

  COMM_BTL_DEMO_TRDATA_MAX,
};

#define TRAINER_NAME_BUF_LEN ( PERSON_NAME_SIZE*2+EOM_SIZE ) ///< �v���C���[��BUF�̒���

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	�g���[�i�[���̃f�[�^
//==============================================================
typedef struct {
  const POKEPARTY* party; ///< ���L�|�P�����f�[�^
  //@TODO STRCODE��STRBUF�����߂�
  STRCODE trname[ PERSON_NAME_SIZE + EOM_SIZE ];  // 16
  STRBUF* str_trname;     ///< �g���[�i�[��
  u8 trsex;               ///< �g���[�i�[�̐���:PM_MALE or PM_FEMALE
  u8 server_version;      ///< ROM�̃T�[�o�o�[�W����
  u8 padding[2];
} COMM_BTL_DEMO_TRAINER_DATA;

//--------------------------------------------------------------
///	�������p�����[�^
//==============================================================
typedef struct {
  // [IN]
  COMM_BTL_DEMO_TYPE type;      ///< �ʐM�o�g���f�� �N���^�C�v
  COMM_BTL_DEMO_RESULT result;  ///< �o�g���̏��s
  COMM_BTL_DEMO_TRAINER_DATA trainer_data[ COMM_BTL_DEMO_TRDATA_MAX ];  ///< �g���[�i�[���̃f�[�^
} COMM_BTL_DEMO_PARAM;

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

FS_EXTERN_OVERLAY(comm_btl_demo);
extern const GFL_PROC_DATA CommBtlDemoProcData;

