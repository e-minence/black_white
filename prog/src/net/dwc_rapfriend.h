//=============================================================================
/**
 * @file	dwc_rapfriend.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

#ifndef __DWC_RAPFRIEND_H__
#define  __DWC_RAPFRIEND_H__

#include <dwc.h>

//==============================================================================
/**
 * ���łɓ����l���o�^����Ă��Ȃ����B
 * @param   pSaveData �t�����h�֌W���͂����Ă���Z�[�u�f�[�^
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1 
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����friend_keya���������Ȃ��B
 */
//==============================================================================
extern int dwc_checkFriendCodeByToken( u64 friend_key, int *index);
//------------------------------------------------------------------
/**
 * $brief   �ڑ����Ă���l�̃t�����h�R�[�h����  �o�^����l������������o�^
            WIFINOTE_MODE_AUTOINPUT_CHECK
 * @param   wk		
 * @param   seq		
 * @retval  int 	
 */
//------------------------------------------------------------------
extern int dwc_friendAutoInputCheck( DWCFriendData* pFriend );

#endif //__DWC_RAPFRIEND_H__
