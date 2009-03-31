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
#include "savedata/save_control.h"
#include "savedata/mystatus.h"

//------------------------------------------------------------------
/**
 * $brief   �ڑ����Ă���l�̃t�����h�R�[�h����  �o�^����l������������o�^
            WIFINOTE_MODE_AUTOINPUT_CHECK
 * @param   wk		
 * @param   seq		
 * @retval  int 	
 */
//------------------------------------------------------------------

extern int GFL_NET_DWC_FriendAutoInputCheck( DWCFriendData* pFriend );

//------------------------------------------------------------------
/**
 * $brief   �����p�o�^�֐�
 * @param   wk		
 * @param   seq		
 * @retval  int 	
 */
//------------------------------------------------------------------
extern void GFL_NET_DWC_FriendDataWrite(SAVE_CONTROL_WORK* pSaveData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int addListIndex, int heapID, int overWrite);

extern void GFL_NET_DWC_FriendDataAdd(SAVE_CONTROL_WORK* pSaveData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int heapID);

#endif //__DWC_RAPFRIEND_H__
