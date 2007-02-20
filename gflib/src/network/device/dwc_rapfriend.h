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

#if _SAVE_PROGRAM

#include "savedata.h"
#include "wifilist.h"

#endif //_SAVE_PROGRAM

// ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
#define DWCFRIEND_INLIST 0
// ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
#define DWCFRIEND_OVERWRITE 1
// �����f�[�^�̓��X�g��ɂȂ��B
#define DWCFRIEND_NODATA 2
// �f�[�^���������Ȃ��B
#define DWCFRIEND_INVALID 3

#if _SAVE_PROGRAM

//==============================================================================
/**
 * ���łɓ����l���o�^����Ă��Ȃ����B
 * @param   pSaveData �t�����h�֌W���͂����Ă���Z�[�u�f�[�^
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1 
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
 * @retval  DWCFRIEND_OVERWRITE �c ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����DWCFriendData���ُ�B
 */
//==============================================================================

extern int dwc_checkfriendByToken(SAVEDATA* pSaveData, DWCFriendData *data, int *index);

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

extern int dwc_checkFriendCodeByToken(SAVEDATA* pSaveData, u64 friend_key, int *index);


extern BOOL dwc_friendAutoInputCheck( SAVEDATA* pSaveData, int* netIDList, int heapID);


extern void dwc_friendWrite(SAVEDATA* pSaveData, int netID, int addListIndex, int heapID, int overWrite);



#endif //_SAVE_PROGRAM

#endif //__DWC_RAPFRIEND_H__
