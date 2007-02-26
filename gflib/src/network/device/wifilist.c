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
	return sizeof(GFL_WIFI_FRIENDLIST);
}

//----------------------------------------------------------
/**
 * @brief	������ԕێ����[�N�̊m��
 * @param	heapID		�������m�ۂ������Ȃ��q�[�v�w��
 * @return	GFL_WIFI_FRIENDLIST	�擾�������[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
GFL_WIFI_FRIENDLIST * WifiList_AllocWork(u32 heapID)
{
	int i;
	GFL_WIFI_FRIENDLIST * list;
	list = GFL_HEAP_AllocMemory(heapID, sizeof(GFL_WIFI_FRIENDLIST));
	
	WifiList_Init( list );

	return list;
}

//----------------------------------------------------------
/**
 * @brief	GFL_WIFI_FRIENDLIST�̃R�s�[
 * @param	from	�R�s�[��GFL_WIFI_FRIENDLIST�ւ̃|�C���^
 * @param	to		�R�s�[��GFL_WIFI_FRIENDLIST�ւ̃|�C���^
 */
//----------------------------------------------------------
void WifiList_Copy(const GFL_WIFI_FRIENDLIST * from, GFL_WIFI_FRIENDLIST * to)
{
	MI_CpuCopy8(from, to, sizeof(GFL_WIFI_FRIENDLIST));
}

//============================================================================================
//
//	GFL_WIFI_FRIENDLIST����̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief			Wifi�p�Ƃ��������X�g�E�����p�f�[�^�̏�����
 * @param			
 */
//----------------------------------------------------------
void WifiList_Init(GFL_WIFI_FRIENDLIST * list)
{
	int i;
	
	MI_CpuClearFast(list, sizeof(GFL_WIFI_FRIENDLIST));

    mydwc_createUserData(list);  //GameSpy���O�C���p��user�R�[�h�̍쐬(��ΕK�v�j
}


//==============================================================================
/**
 * $brief   ������WIFI�ڑ��������̃|�C���^��Ԃ�
 *
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 *
 * @retval  u8*		
 */
//==============================================================================
DWCUserData* WifiList_GetMyUserInfo( GFL_WIFI_FRIENDLIST* list )
{
	return &(list->my_dwcuser);
}

//==============================================================================
/**
 * $brief   �Ƃ��������X�g����F�B��WIFI�p���\���̂̃|�C���^���擾
 *
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 *
 * @retval  DWCFriendData	*		
 */
//==============================================================================
DWCFriendData	*WifiList_GetDwcDataPtr( GFL_WIFI_FRIENDLIST* list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

	return &(list->friend_dwc[no]);
}

//==============================================================================
/**
 * $brief   �f�[�^�������Ă��邩�ǂ���
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  BOOL
 */
//==============================================================================
BOOL WifiList_IsFriendData( GFL_WIFI_FRIENDLIST *list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );
    // �t�����h�R�[�h�����������ǂ����Ŕ��f
    return DWC_IsValidFriendData(&list->friend_dwc[no]);
}

//==============================================================================
/**
 * $brief   �����Ă���l����������
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  BOOL
 */
//==============================================================================
int WifiList_GetFriendDataNum( GFL_WIFI_FRIENDLIST *list )
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
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  none
 */
//==============================================================================
void WifiList_ResetData( GFL_WIFI_FRIENDLIST *list, int no)
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
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		    �����Ɉړ�
 * @param   moveNo		�ړ�������f�[�^
 * @retval  none
 */
//==============================================================================
static void WifiList_MoveData( GFL_WIFI_FRIENDLIST *list, int no, int moveNo)
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
 * @param   list	GFL_WIFI_FRIENDLIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  none
 */
//==============================================================================
void WifiList_FormUpData( GFL_WIFI_FRIENDLIST *list)
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
 * @return	GFL_WIFI_FRIENDLIST	WIFI���X�g
 */
//---------------------------------------------------------------------------
#if _SAVE_PROGRAM
GFL_WIFI_FRIENDLIST* SaveData_GetWifiListData(SAVEDATA * sv)
{
	GFL_WIFI_FRIENDLIST* pData;
	pData = SaveData_Get(sv, GMDATA_ID_WIFILIST);
	return pData;
}
#endif
