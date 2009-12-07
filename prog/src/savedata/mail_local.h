/**
 *  @file mail_local.h
 *  @brief  ���[���Z�[�u�f�[�^���䃂�W���[���݂̂�include���ėǂ�
 *  @author Miyuki Iwasawa
 *  @date 06.03.02
 */

#ifndef __MAIL_LOCAL_H__
#define __MAIL_LOCAL_H__

#include "savedata/mail.h"

///���[���f�[�^�^
//=============================================================
//  �O�����J���Ă��܂����A���ڍ\���̃����o�փA�N�Z�X���邱�Ƃ�
//  �����Ă��܂���B
//
//  �����o�A�N�Z�X��savedata/mail_util.h�ɒ�`���ꂽ
//  �A�N�Z�X�n�֐�����Ă�������
//
//  size=56byte
//
//  �\���̃T�C�Y��4byte���E�ɍ��킹�邽�߁AMAIL_ICON�z��v�f����+1����
//  �p�f�B���O���s���Ă��܂�
//=============================================================
typedef struct _MAIL_DATA{
  u32 writerID; //<�g���[�i�[ID 4
  u8  sex;      //<��l���̐��� 1
  u8  region;   //<���R�[�h 1
  u8  version;  //<�J�Z�b�g�o�[�W���� 1
  u8  design;   //<�f�U�C���i���o�[ 1
  STRCODE name[PERSON_NAME_SIZE+EOM_SIZE];  // 16
  MAIL_ICON icon[MAILDAT_ICONMAX];          //<�A�C�R��NO�i�[�ꏊ[]
  u16 form_bit;                             //padding�̈���v���`�i����3�̂̃|�P������
                                            // �t�H�����ԍ��Ƃ��Ďg�p(5bit�P��)
  PMS_DATA  msg[MAILDAT_MSGMAX];            //<���̓f�[�^
}_MAIL_DATA;


///���[���f�[�^�Z�[�u�f�[�^�u���b�N��`�\����
typedef struct _MAIL_BLOCK{

  MAIL_DATA paso[MAIL_STOCK_PASOCOM];   // ���[���ő吔20��

}_MAIL_BLOCK;

#endif  //__MAIL_LOCAL_H__

