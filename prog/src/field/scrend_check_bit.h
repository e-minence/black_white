//======================================================================
/**
 * @file	scrend_check_bit.h
 * @brief	�X�N���v�g�I�����`�F�b�N�r�b�g��`
 * @author	NozomuSaito
 *
 * @note    SCREND_CHK�̕��т́Ascrcmd.c��CheckEndFuncTbl��1��1�őΉ�����悤�ɂ��Ă�������
 *
 */
//======================================================================

#pragma once

typedef enum
{
  SCREND_CHK_CAMERA = 0,
  SCREND_CHK_WIN_OPEN,
  /*�����ɒ�`��ǉ����Ă�������*/
  SCREND_CHK_MAX
}SCREND_CHK;

extern void SCREND_CHK_ClearBits(void);
extern void SCREND_CHK_SetBitOn(const SCREND_CHK inCheckBit);
extern void SCREND_CHK_SetBitOff(const SCREND_CHK inCheckBit);
extern BOOL SCREND_CHK_CheckBit(const SCREND_CHK inCheckBit);

