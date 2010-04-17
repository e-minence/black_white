//======================================================================
/*
 * @file	enc_cutin_no.h
 * @brief	�G���J�E���g�J�b�g�C���i���o�[
 * @author saito
 */
//======================================================================

#pragma once

typedef enum{
  ENC_CUTIN_RIVAL,
  ENC_CUTIN_SUPPORT,
  ENC_CUTIN_GYM1A,
  ENC_CUTIN_GYM1B,
  ENC_CUTIN_GYM1C,
  ENC_CUTIN_GYM2,
  ENC_CUTIN_GYM3,
  ENC_CUTIN_GYM4,
  ENC_CUTIN_GYM5,
  ENC_CUTIN_GYM6,
  ENC_CUTIN_GYM7,
  ENC_CUTIN_GYM8A,
  ENC_CUTIN_GYM8B,
  ENC_CUTIN_BIGFOUR1,
  ENC_CUTIN_BIGFOUR2,
  ENC_CUTIN_BIGFOUR3,
  ENC_CUTIN_BIGFOUR4,
  ENC_CUTIN_CHAMP,
  ENC_CUTIN_BOSS,
  ENC_CUTIN_SAGE,
  ENC_CUTIN_PLASMA,
  ENC_CUTIN_PACKAGE,
  ENC_CUTIN_MOVEPOKE,
  ENC_CUTIN_THREE,
  ENC_CUTIN_ZOROARK,
}ENC_CUTIN_NO;

typedef enum {
  GRA_TRANS_NONE,   //�]���Ȃ�
  GRA_TRANS_ALL,    //�����A����A�Ƃ��ɓ]��
  GRA_TRANS_ENEMY,  //����̂ݓ]��
}GRA_TRANS_TYPE;

typedef struct ENC_CUTIN_DAT_tag
{
  int CutinNo;      //�J�b�g�C���̃i���o�[
  int ChrArcIdx;   //���葤�̃L�����]���f�[�^�A�[�J�C�u�C���f�b�N�X
  int PltArcIdx;    //���葤�̃p���b�g�]���f�[�^�A�[�J�C�u�C���f�b�N�X
  int MsgIdx;       //���葤�̖��O�f�[�^�C���f�b�N�X
  GRA_TRANS_TYPE TransType;    //�]���f�[�^�̎��
}ENC_CUTIN_DAT;

/**
 * ���J�b�g�C���p�g���[�i�[�����b�Z�[�W�f�[�^�̕��т�
 * �@�]���摜�̕��т͍��킹��悤�ɂ���
 *
 * �@�]���摜�f�[�^�A�[�J�C�u�̓L�����f�[�^�A�p���b�g�f�[�^�̏��Ԃň�g�ɂ��āA���Ԃ�����
 *
*/

extern const ENC_CUTIN_DAT *ENC_CUTIN_NO_GetDat(const int inEncCutinNo);

