#ifndef __DWC_RAP_FRIEND_H__
#define  __DWC_RAP_FRIEND_H__

#include <dwc.h>

#include "savedata/save_control.h"
#include "savedata/wifilist.h"

// ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
#define DWCFRIEND_INLIST 0
// ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
#define DWCFRIEND_OVERWRITE 1
// �����f�[�^�̓��X�g��ɂȂ��B
#define DWCFRIEND_NODATA 2
// �f�[�^���������Ȃ��B
#define DWCFRIEND_INVALID 3

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
extern int dwc_checkfriendByToken(SAVE_CONTROL_WORK* pSaveData, DWCFriendData *data, int *index);

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
extern int dwc_checkFriendCodeByToken(SAVE_CONTROL_WORK* pSaveData, u64 friend_key, int *index);


extern BOOL dwc_friendAutoInputCheck( SAVE_CONTROL_WORK* pSaveData, int* netIDList,
                                      int heapID);

extern void dwc_friendWrite(SAVE_CONTROL_WORK* pSaveData, int netID, int addListIndex,
                            int heapID, int overWrite);

// NetID����F�B�蒠INDEX��T��
extern int dwc_SearchNetID2WifiListIndex(SAVE_CONTROL_WORK* pSaveData,int netID);

#endif
