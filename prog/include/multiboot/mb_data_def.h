//======================================================================
/**
 * @file  mb_data_def.c 
 * @brief wb�_�E�����[�h�v���C��`��
 * @author  ariizumi
 * @data  09/11/20
 */
//======================================================================

#pragma once

#if 1 //�f�o�b�O�o�͐؂�ւ�

#ifndef MULTI_BOOT_MAKE
#include "test/ariizumi/ari_debug.h"
#define MB_DATA_TPrintf(...) (void)((ARI_TPrintf(__VA_ARGS__)))
#else
#include "../../src/test/ariizumi/ari_debug.h"
#define MB_DATA_TPrintf(...) (void)((ARI_TPrintf(__VA_ARGS__)))
#endif //MULTI_BOOT_MAKE
#else

#define MB_DATA_TPrintf(...) (void)((OS_TPrintf(__VA_ARGS__)))
#endif//�f�o�b�O�o�͐؂�ւ�

//ROM�R�[�h
#define MB_ROMCODE_DIAMOND  (0x4A414441)
#define MB_ROMCODE_PEARL    (0x4A415041)
#define MB_ROMCODE_PLATINUM (0x4A555043)
#define MB_ROMCODE_H_GOLD   (0x4A4B5049)
#define MB_ROMCODE_S_SILVER (0x4A475049)

//�{�̂Ɏh�����Ă���J�[�h�̎��
typedef enum {
  CARD_TYPE_DP,   //�_�C�A�����h���p�[��
  CARD_TYPE_PT,   //�v���`�i
  CARD_TYPE_GS,   //�S�[���h���V���o�[
#if PM_DEBUG
  CARD_TYPE_DUMMY,  //MB�e�X�g�_�~�[
#endif
  CARD_TYPE_INVALID,
}DLPLAY_CARD_TYPE;

enum DLPLAY_ERROR_STATE
{
  DES_NONE,
  DES_DISCONNECT,
  DES_MISS_LOAD_BACKUP,
  DES_ANOTHER_COUNTRY,

  DES_MAX,
};

enum DLPLAY_DATA_SAVEPOS
{
  DDS_FIRST,  //1�ԃf�[�^
  DDS_SECOND, //2�ԃf�[�^
  DDS_ERROR,  //�G���[
};

struct _MB_DATA_WORK
{
  int heapId;
  u8  mainSeq;
  u8  subSeq;
  u8  errorState;
  u8  selectBoxNumber_;
  BOOL  isFinishSaveFirst;
  BOOL  isFinishSaveSecond;
  BOOL  permitLastSaveFirst;
  BOOL  permitLastSaveSecond;
  BOOL  isFinishSaveAll_;

  s32 lockID_;  //�J�[�h���b�N�pID
  MATHCRC16Table  crcTable_;  //CRC�`�F�b�N�p�e�[�u��
  
  BOOL  isDummyCard;
  DLPLAY_CARD_TYPE  cardType;     //�{�̂Ɏh�����Ă���J�[�h�̎��(�f�o�b�O���ɑI������ROM���̂�
  u8          boxLoadPos;  //�{�b�N�X�f�[�^�̐V������
  u8          mainLoadPos; //���C���f�[�^�̐V������
  u8          boxSavePos;  //�Z�[�u����ق�
  u8          mainSavePos; //�Z�[�u����ق�

  u8  *pData;  //�ǂݏo�����f�[�^
  u8  *pDataMirror;  //�~���[�p�f�[�^
  void *pDataCrcCheck;  //CRC�`�F�b�N�p�f�[�^
  
  u16 cardCrcTable[4];  //���݂̃f�[�^��CRC
  u16 loadCrcTable[4];  //�ǂݍ��񂾃f�[�^��CRC
  
  u8  *pBoxData;    //�ǂݍ��ݑΏۂ�BOX�f�[�^�A�h���X(�܂��|�P�����f�[�^����Ȃ��I
  u8  *pItemData;   //�ǂݍ��ݑΏۂ̃A�C�e���f�[�^�A�h���X(�܂��A�C�e���f�[�^����Ȃ��I
  u8  *pMysteryData;//�ǂݍ��ݑΏۂ̕s�v�c�ȑ��蕨�f�[�^�A�h���X
  u8  *pMyStatus;   //�ǂݍ��ݑΏۂ̃}�C�X�e�[�^���f�[�^�A�h���X(���񍑃R�[�h�̂�
};

