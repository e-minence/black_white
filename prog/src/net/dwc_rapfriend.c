//=============================================================================
/**
 * @file	dwc_rapfriend.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"

#if GFL_NET_WIFI

#include "dwc_rapfriend.h"
#include "savedata/wifilist.h"


//==============================================================================
/**
 * ���łɓ����l���o�^����Ă��Ȃ����B
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1 
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
 * @retval  DWCFRIEND_OVERWRITE �c ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����DWCFriendData���ُ�B
 */
//==============================================================================
int GFL_NET_DWC_CheckFriendByToken(DWCFriendData *data, int *index)
{
	// ���łɓ����l���o�^���Ă��Ȃ�������
	int i;
    DWCUserData *myUserData = GFI_NET_GetMyDWCUserData();
    DWCFriendData *keyList  = GFI_NET_GetMyDWCFriendData();
    int numMax  = GFI_NET_GetFriendNumMax();
    *index = -1;

    GF_ASSERT(myUserData);
    
    if( !DWC_IsValidFriendData( data ) ) return DWCFRIEND_INVALID;
    
	for( i = 0; i < numMax; i++ )
	{

		if( DWC_IsEqualFriendData( data, keyList + i ) ) 
		{
			// �S�������f�[�^�������B
			*index = i;
			return DWCFRIEND_INLIST;
		}
		else if( DWC_GetGsProfileId(myUserData,data) > 0 && DWC_GetGsProfileId(myUserData,data) == DWC_GetGsProfileId(myUserData,keyList + i) ) 
		{
			// �t�����h�R�[�h�œo�^����Ă���f�[�^������B
			// ���̏ꍇ�́A�㏑�����Ă��炤���Ƃ𐄏��B
			*index = i;
			return DWCFRIEND_OVERWRITE;
		}
		else if( *index < 0 && !DWC_IsValidFriendData( keyList + i ) )
		{
			*index = i;
		}
	}
	
	// ���X�g�ɂȂ��B
	return DWCFRIEND_NODATA;
}

//==============================================================================
/**
 * ���łɓ����l���o�^����Ă��Ȃ����B
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1 
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����friend_keya���������Ȃ��B
 */
//==============================================================================
int GFL_NET_DWC_CheckFriendCodeByToken( u64 friend_key, int *index)
{
	// ���łɓ����l���o�^���Ă��Ȃ�������
	int i;
    DWCUserData *myUserData = GFI_NET_GetMyDWCUserData();
    DWCFriendData *keyList  = GFI_NET_GetMyDWCFriendData();
    int numMax  = GFI_NET_GetFriendNumMax();
    DWCFriendData token;

    // �F�B�o�^�������������ǂ�������
    if( !DWC_CheckFriendKey( myUserData, friend_key ) ) return DWCFRIEND_INVALID;
    DWC_CreateFriendKeyToken( &token, friend_key );   
    if( DWC_GetGsProfileId(myUserData, &token) <= 0 ) return DWCFRIEND_INVALID;
    
    *index = -1;
	for( i = 0; i < numMax; i++ )
	{
		if( DWC_GetGsProfileId(myUserData, &token) == DWC_GetGsProfileId(myUserData, keyList + i) ) 
		{
			// �o�^����Ă���f�[�^������B
			*index = i;
			return DWCFRIEND_INLIST;
		}
		else if( *index < 0 && !DWC_IsValidFriendData( keyList + i ) )
		{
			*index = i;	
		}
	}
	
	// ���X�g�ɂȂ��B
	return DWCFRIEND_NODATA;	
}

//------------------------------------------------------------------
/**
 * $brief   �ڑ����Ă���l�̃t�����h�R�[�h����  �o�^����l������������o�^
            WIFINOTE_MODE_AUTOINPUT_CHECK
 * @param   wk		
 * @param   seq		
 * @retval  int 	
 */
//------------------------------------------------------------------

int GFL_NET_DWC_FriendAutoInputCheck( DWCFriendData* pFriend )
{
    int i,hit = FALSE,pos,ret;
    DWCFriendData *keyList  = GFI_NET_GetMyDWCFriendData();

    GF_ASSERT(keyList);
    GF_ASSERT(pFriend);
    
    ret = GFL_NET_DWC_CheckFriendByToken(pFriend, &pos);

    NET_PRINT("%d check  \n",ret);
    GF_ASSERT(ret != DWCFRIEND_INVALID);  // �󂯎��f�[�^�Ɉُ킪����

    if(ret == DWCFRIEND_INLIST){ //����
    }
    else if(ret == DWCFRIEND_OVERWRITE){ //�㏑��
        MI_CpuCopy8(pFriend, &keyList[pos], sizeof(DWCFriendData));
    }
    else if(ret == DWCFRIEND_NODATA){  // �V�K�f�[�^
        hit = TRUE;
    }
    return  hit;
}

//------------------------------------------------------------------
/**
 * $brief   �����p�o�^�֐�
 * @param   wk		
 * @param   seq		
 * @retval  int 	
 */
//------------------------------------------------------------------
void GFL_NET_DWC_FriendDataWrite(SAVE_CONTROL_WORK* pSaveData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int addListIndex, int heapID, int overWrite)
{
    WIFI_LIST* pList = SaveData_GetWifiListData(pSaveData);
    DWCFriendData *keyList  = WifiList_GetDwcDataPtr(pList, addListIndex);
    STRBUF* pBuf;

    if(overWrite != 2){
//        pFriend = CommInfoGetDWCFriendCode(netID);
        MI_CpuCopy8(pFriend, keyList, sizeof(DWCFriendData));
    }
    if(overWrite == 0){  // �㏑�����ɂ͖��O�����������Ȃ�
        pBuf = MyStatus_CreateNameString(pMyStatus, heapID);
        WifiList_SetFriendName(pList, addListIndex, pBuf);
        GFL_STR_DeleteBuffer(pBuf);
        WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_SEX, MyStatus_GetMySex(pMyStatus));
        WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_ID, MyStatus_GetID(pMyStatus));
    }
    else if(overWrite == 1){
        // �㏑�������ǐ��ʂ��܂�������Ԃ̏ꍇ����
        if(WifiList_GetFriendInfo(pList,addListIndex,WIFILIST_FRIEND_SEX) == PM_NEUTRAL){
            WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_SEX, MyStatus_GetMySex(pMyStatus));
            WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_ID, MyStatus_GetID(pMyStatus));
        }
    }
    pBuf =  GFL_STR_CreateBuffer(120, heapID );
    //GFL_STR_SetStringCode(pBuf, CommInfoGetGroupName(netID));
    //WifiList_SetFriendGroupName(pList, addListIndex, pBuf);
    GFL_STR_DeleteBuffer(pBuf);
    WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_UNION_GRA, MyStatus_GetTrainerView(pMyStatus));
    //CommInfoWriteResult( pSaveData );

}

//------------------------------------------------------------------
/**
 * $brief   �����p�Ƃ������ǉ��֐�
 * @param   SAVE_CONTROL_WORK		
 * @param   MYSTATUS		
 * @param   DWCFriendData		
 * @param   heapID		
 * @retval  int 	
 */
//------------------------------------------------------------------

void GFL_NET_DWC_FriendDataAdd(SAVE_CONTROL_WORK* pSaveData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int heapID)
{
    WIFI_LIST* pList = SaveData_GetWifiListData(pSaveData);
    int i;

    for(i = 0; i < WIFILIST_FRIEND_MAX;i++){
        if( !WifiList_IsFriendData( pList, i ) ){
            GFL_NET_DWC_FriendDataWrite(pSaveData, pMyStatus, pFriend, i, heapID, 0);
            break;
        }
    }
}


#endif //GFL_NET_WIFI
