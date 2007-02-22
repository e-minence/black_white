//============================================================================================
/**
 * @file	wifilist.c
 * @brief	WIFI�t�����h�R�[�h�Ǘ�  > �Ƃ������蒠�̃f�[�^�Ǘ�
 * @author	mori san   >  k.ohno
 * @date	2006.03.27 ���炢
 */
//============================================================================================

#include <nitro.h>
#include <nnsys.h>
#include <dwc.h>
#include "gf_macro.h"
#include "gf_standard.h"
#include "assert.h"
#include "heapsys.h"
#include "strbuf.h"
#include "net.h"
#include "../net_def.h"

#include "wifilist.h"
#include "wifilist_local.h"
#include "dwc_rap.h"
#include "dwc_rapcommon.h"

//============================================================================================
//============================================================================================




//============================================================================================
//
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	������ԕێ����[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int WifiList_GetWorkSize(void)
{
	return sizeof(WIFI_LIST);
}

//----------------------------------------------------------
/**
 * @brief	������ԕێ����[�N�̊m��
 * @param	heapID		�������m�ۂ������Ȃ��q�[�v�w��
 * @return	WIFI_LIST	�擾�������[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
WIFI_LIST * WifiList_AllocWork(u32 heapID)
{
	int i;
	WIFI_LIST * list;
	list = GFL_HEAP_AllocMemory(heapID, sizeof(WIFI_LIST));
	
	WifiList_Init( list );

	return list;
}

//----------------------------------------------------------
/**
 * @brief	WIFI_LIST�̃R�s�[
 * @param	from	�R�s�[��WIFI_LIST�ւ̃|�C���^
 * @param	to		�R�s�[��WIFI_LIST�ւ̃|�C���^
 */
//----------------------------------------------------------
void WifiList_Copy(const WIFI_LIST * from, WIFI_LIST * to)
{
	MI_CpuCopy8(from, to, sizeof(WIFI_LIST));
}

//============================================================================================
//
//	WIFI_LIST����̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief			Wifi�p�Ƃ��������X�g�E�����p�f�[�^�̏�����
 * @param			
 */
//----------------------------------------------------------
void WifiList_Init(WIFI_LIST * list)
{
	int i;
	
	MI_CpuClearFast(list, sizeof(WIFI_LIST));

    mydwc_createUserData(list);  //GameSpy���O�C���p��user�R�[�h�̍쐬(��ΕK�v�j
}


//==============================================================================
/**
 * $brief   ������WIFI�ڑ��������̃|�C���^��Ԃ�
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 *
 * @retval  u8*		
 */
//==============================================================================
DWCUserData* WifiList_GetMyUserInfo( WIFI_LIST* list )
{
	return &(list->my_dwcuser);
}

//==============================================================================
/**
 * $brief   �Ƃ��������X�g����F�B��WIFI�p���\���̂̃|�C���^���擾
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 *
 * @retval  DWCFriendData	*		
 */
//==============================================================================
DWCFriendData	*WifiList_GetDwcDataPtr( WIFI_LIST* list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

	return &(list->friend_dwc[no]);
}

//==============================================================================
/**
 * $brief   �f�[�^�������Ă��邩�ǂ���
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  BOOL
 */
//==============================================================================
BOOL WifiList_IsFriendData( WIFI_LIST *list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );
    // �t�����h�R�[�h�����������ǂ����Ŕ��f
    return DWC_IsValidFriendData(&list->friend_dwc[no]);
}

//==============================================================================
/**
 * $brief   �����Ă���l����������
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  BOOL
 */
//==============================================================================
int WifiList_GetFriendDataNum( WIFI_LIST *list )
{
    int i,num = 0;

    for(i = 0; i < WIFILIST_FRIEND_MAX;i++){
        if(WifiList_IsFriendData(list,i)){
            num++;
        }
    }
    return num;
}

//==============================================================================
/**
 * $brief   �t�����h�̖��O������
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  none
 */
//==============================================================================
void WifiList_ResetData( WIFI_LIST *list, int no)
{
    int i;

    GF_ASSERT( no < WIFILIST_FRIEND_MAX);
    for(i = no; i < (WIFILIST_FRIEND_MAX-1); i++){
        MI_CpuCopy8(&list->friend_dwc[i+1], &list->friend_dwc[i], sizeof(DWCFriendData));
    }
    i = WIFILIST_FRIEND_MAX-1;
	MI_CpuClearFast(&list->friend_dwc[i],sizeof(DWCFriendData));
}


//==============================================================================
/**
 * $brief   �t�����h�̖��O���ړ�����
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		    �����Ɉړ�
 * @param   moveNo		�ړ�������f�[�^
 * @retval  none
 */
//==============================================================================
static void WifiList_MoveData( WIFI_LIST *list, int no, int moveNo)
{
    int i;

//    MI_CpuCopy8(&list->friend[moveNo], &list->friend[no], sizeof(WIFI_FRIEND));
    MI_CpuCopy8(&list->friend_dwc[moveNo], &list->friend_dwc[no], sizeof(DWCFriendData));
//	MI_CpuClearFast(&list->friend[moveNo], sizeof(WIFI_FRIEND));
	MI_CpuClearFast(&list->friend_dwc[moveNo],sizeof(DWCFriendData));
//    list->friend[moveNo].name[0] = EOM_;
//    list->friend[moveNo].groupName[0] = EOM_;
//    list->friend[moveNo].sex = PM_NEUTRAL;
}

//==============================================================================
/**
 * $brief   �t�����h���߂�
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  none
 */
//==============================================================================
void WifiList_FormUpData( WIFI_LIST *list)
{
    int i,blank = -1;

    for(i = 0; i < WIFILIST_FRIEND_MAX; i++){
        if(WifiList_IsFriendData(list, i)){
            if(blank != -1){
                WifiList_MoveData(list, blank, i);  // �󂫂Ɉړ�
                OS_TPrintf("friend move %d < %d\n",blank,i);
                i = -1;  // ������񌟍�
                blank = -1;
            }
        }
        else{
            if(blank == -1){
                blank = i;  // �󂫂��L��
            }
        }
    }
}


//---------------------------------------------------------------------------
/**
 * @brief	WIFI���X�g���Z�[�u�f�[�^��������o��
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	WIFI_LIST	WIFI���X�g
 */
//---------------------------------------------------------------------------
#if _SAVE_PROGRAM
WIFI_LIST* SaveData_GetWifiListData(SAVEDATA * sv)
{
	WIFI_LIST* pData;
	pData = SaveData_Get(sv, GMDATA_ID_WIFILIST);
	return pData;
}
#endif
