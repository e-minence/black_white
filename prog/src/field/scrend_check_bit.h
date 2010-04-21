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

#ifdef PM_DEBUG

#if defined(DEBUG_ONLY_FOR_masafumi_saitou) | defined(DEBUG_ONLY_FOR_naoto_murakami) \
  | defined(DEBUG_ONLY_FOR_suginaka_katsunori) | defined(DEBUG_ONLY_FOR_mai_ando)
#define SCR_ASSERT_ON
#endif

#endif //PM_DEBUG

typedef enum
{
  SCREND_CHK_CAMERA = 0,
  SCREND_CHK_WIN_OPEN,
  SCREND_CHK_BALLON_WIN_OPEN,
  SCREND_CHK_PLAINWIN_OPEN,
  SCREND_CHK_BGWIN_OPEN,
  SCREND_CHK_SUBWIN_OPEN,
  SCREND_CHK_SPWIN_OPEN,
  SCREND_CHK_FACEUP,
  SCREND_CHK_SUBSCREEN_CHANGE,
  SCREND_CHK_MSGWIN_AUTO_PRINT,
  SCREND_CHK_BGM_PUSH_POP,
  SCREND_CHK_NET_OFF_EVENT,
  /*�����ɒ�`��ǉ����Ă�������*/
  SCREND_CHK_MAX
}SCREND_CHK;

extern void SCREND_CHK_ClearBits(void);
extern void SCREND_CHK_SetBitOn(const SCREND_CHK inCheckBit);
extern void SCREND_CHK_SetBitOff(const SCREND_CHK inCheckBit);
extern BOOL SCREND_CHK_CheckBit(const SCREND_CHK inCheckBit);

