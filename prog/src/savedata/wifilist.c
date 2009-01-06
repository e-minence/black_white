//============================================================================================
/**
 * @file	wifilist.c
 * @brief	WIFI�t�����h�R�[�h�Ǘ�  > �Ƃ������蒠�̃f�[�^�Ǘ�
 * @author	mori san   >  k.ohno
 * @date	2006.03.27 ���炢
 */
//============================================================================================

//#include "savedata/savedata_def.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���

#include "gflib.h"
#include <dwc.h>

#include "system/rtc_tool.h"
#include "savedata/save_tbl.h"
#include "savedata/wifilist.h"
#include "wifilist_local.h"

FS_EXTERN_OVERLAY(dev_wifi);

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
	GFL_STD_MemCopy(from, to, sizeof(WIFI_LIST));
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
	
	GFL_STD_MemClear(list, sizeof(WIFI_LIST));

    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifi ) );
    GFL_NET_WIFI_InitUserData(&list->my_dwcuser);  //GameSpy���O�C���p��user�R�[�h�̍쐬(��ΕK�v�j
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifi ) );

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
 * $brief   WIFI�F�B�����^�C�v���ɕԂ�
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���Ԗڂ̗F�B��
 * @param   type	�ݒ肷����̃^�C�v�iwifilist.h��enum�錾����)
 *
 * @retval  u32		
 */
//==============================================================================
u32 WifiList_GetFriendInfo( WIFI_LIST* list, int no, int type )
{
	u32 result;

	GF_ASSERT( no < WIFILIST_FRIEND_MAX );
	
	switch(type){
      case WIFILIST_FRIEND_ID:
        result = list->friendData[no].id;
        break;
      case WIFILIST_FRIEND_BATTLE_WIN:
        result = list->friendData[no].battle_win;
        break;
      case WIFILIST_FRIEND_BATTLE_LOSE:
        result = list->friendData[no].battle_lose;
        break;
      case WIFILIST_FRIEND_TRADE_NUM:
        result = list->friendData[no].trade_num;
        break;
      case WIFILIST_FRIEND_LASTBT_YEAR:
        result = list->friendData[no].year;
        break;
      case WIFILIST_FRIEND_LASTBT_MONTH:
        result = list->friendData[no].month;
        break;
      case WIFILIST_FRIEND_LASTBT_DAY:
        result = list->friendData[no].day;
        break;
      case WIFILIST_FRIEND_SEX:
        result = list->friendData[no].sex;
        break;
      case WIFILIST_FRIEND_UNION_GRA:
        result = list->friendData[no].unionGra;
        break;
	  case WIFILIST_FRIEND_POFIN_NUM:
		result = list->friendData[no].pofin_num;
		break;
	  case WIFILIST_FRIEND_BALLSLOW_NUM:
		result = list->friendData[no].minigame_ballslow;
		break;
	  case WIFILIST_FRIEND_BALANCEBALL_NUM:
		result = list->friendData[no].minigame_balanceball;
		break;
	  case WIFILIST_FRIEND_BALLOON_NUM:
		result = list->friendData[no].minigame_balloon;
		break;
	}
	return result;
}


//==============================================================================
/**
 * $brief   
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���Ԗڂ̗F�B��
 * @param   type	�ݒ肷����̃^�C�v�iwifilist.h��enum�錾����)
 * @param   value	�ݒ肷��l
 *
 * @retval  none	
 */
//==============================================================================
void WifiList_SetFriendInfo( WIFI_LIST* list, int no, int type, u32 value )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

    switch(type){
      case WIFILIST_FRIEND_ID:
        list->friendData[no].id = value;
        break;
      case WIFILIST_FRIEND_BATTLE_WIN:
        GF_ASSERT(0);
        break;
      case WIFILIST_FRIEND_BATTLE_LOSE:
        GF_ASSERT(0);
        break;
      case WIFILIST_FRIEND_TRADE_NUM:
        GF_ASSERT(0);
        break;
      case WIFILIST_FRIEND_LASTBT_YEAR:
        list->friendData[no].year = value;
        break;
      case WIFILIST_FRIEND_LASTBT_MONTH:
        list->friendData[no].month = value;
        break;
      case WIFILIST_FRIEND_LASTBT_DAY:
        list->friendData[no].day = value;
        break;
      case WIFILIST_FRIEND_SEX:
        list->friendData[no].sex = value;
        break;
      case WIFILIST_FRIEND_UNION_GRA:
        list->friendData[no].unionGra = value;
        break;
      case WIFILIST_FRIEND_POFIN_NUM:
        GF_ASSERT(0);
		break;
    }
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
 * $brief   
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 *
 * @retval  u8 *		
 */
//==============================================================================
STRCODE *WifiList_GetFriendNamePtr( WIFI_LIST *list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

	return list->friendData[no].name;
}

//==============================================================================
/**
 * $brief   
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 *
 * @retval  u8 *		
 */
//==============================================================================
void WifiList_SetFriendName( WIFI_LIST *list, int no, STRBUF* pBuf )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

    GFL_STR_SetStringCodeOrderLength(pBuf, list->friendData[no].name, sizeof(list->friendData[no].name));
}

//==============================================================================
/**
 * $brief   
 *
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 *
 * @retval  u8 *		
 */
//==============================================================================
STRCODE *WifiList_GetFriendGroupNamePtr( WIFI_LIST *list, int no )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

	return list->friendData[no].groupName;
}

//==============================================================================
/**
 * $brief   
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  u8 *		
 */
//==============================================================================
void WifiList_SetFriendGroupName( WIFI_LIST *list, int no, STRBUF* pBuf )
{
	GF_ASSERT( no < WIFILIST_FRIEND_MAX );

    GFL_STR_GetStringCode(pBuf, list->friendData[no].groupName, sizeof(list->friendData[no].groupName));
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
    if(no >= WIFILIST_FRIEND_MAX){
        return FALSE;
    }
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

//----------------------------------------------------------------------------
/**
 *	@brief	�����Ă���F�B�̍Ō�̃C���f�b�N�X���擾����
 *
 *	@param	list	�\����
 *
 *	@retval	�Ō�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
int WifiList_GetFriendDataLastIdx( WIFI_LIST *list )
{
    int i,last = 0;

    for(i = 0; i < WIFILIST_FRIEND_MAX;i++){
        if(WifiList_IsFriendData(list,i)){
            last = i+1;
        }
    }
    return last;
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
    if(no < WIFILIST_FRIEND_MAX){
        for(i = no; i < (WIFILIST_FRIEND_MAX-1); i++){
            GFL_STD_MemCopy(&list->friendData[i+1], &list->friendData[i], sizeof(WIFI_FRIEND));
            GFL_STD_MemCopy(&list->friend_dwc[i+1], &list->friend_dwc[i], sizeof(DWCFriendData));
        }
        i = WIFILIST_FRIEND_MAX-1;
        GFL_STD_MemClear(&list->friendData[i], sizeof(WIFI_FRIEND));
        GFL_STD_MemClear(&list->friend_dwc[i],sizeof(DWCFriendData));
    }
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

    GFL_STD_MemCopy(&list->friendData[moveNo], &list->friendData[no], sizeof(WIFI_FRIEND));
    GFL_STD_MemCopy(&list->friend_dwc[moveNo], &list->friend_dwc[no], sizeof(DWCFriendData));
	GFL_STD_MemClear(&list->friendData[moveNo], sizeof(WIFI_FRIEND));
	GFL_STD_MemClear(&list->friend_dwc[moveNo],sizeof(DWCFriendData));
}

//==============================================================================
/**
 * $brief   �t�����h�̃f�[�^���R�s�[����
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		    �����Ɉړ�
 * @param   moveNo		�ړ�������f�[�^
 * @retval  none
 */
//==============================================================================
#ifdef PM_DEBUG
void WifiList_CopyData( WIFI_LIST *list, int no, int copyNo)
{
    GFL_STD_MemCopy(&list->friendData[copyNo], &list->friendData[no], sizeof(WIFI_FRIEND));
    GFL_STD_MemCopy(&list->friend_dwc[copyNo], &list->friend_dwc[no], sizeof(DWCFriendData));
}

void WifiList_SetCountStopNum( WIFI_LIST *list, int no )
{
    list->friendData[no].battle_win = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].battle_lose = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].trade_num = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].pofin_num = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].minigame_ballslow = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].minigame_balanceball = WIFILIST_COUNT_RANGE_MAX;
    list->friendData[no].minigame_balloon = WIFILIST_COUNT_RANGE_MAX;

    WifiList_SetLastPlayDate(list, no);

}

#endif

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

//==============================================================================
/**
 * $brief   �����̋L�^������
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @retval  none
 */
//==============================================================================
void WifiList_SetLastPlayDate( WIFI_LIST *list, int no)
{
    RTCDate	rtc;
    GFL_RTC_GetDate(&rtc);
    list->friendData[no].year = rtc.year+2000;
    list->friendData[no].month = rtc.month;
    list->friendData[no].day = rtc.day;
}

//==============================================================================
/**
 * $brief   ���������̋L�^������
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @param   winNum  ������
 * @param   loseNum ������
 * @param   trade   ������
 * @retval  none
 */
//==============================================================================
void WifiList_SetResult( WIFI_LIST *list, int no, int winNum, int loseNum,int trade)
{
    list->friendData[no].battle_win += winNum;
    if(list->friendData[no].battle_win > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].battle_win = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].battle_lose += loseNum;
    if(list->friendData[no].battle_lose > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].battle_lose = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].trade_num += trade;
    if(list->friendData[no].trade_num > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].trade_num = WIFILIST_COUNT_RANGE_MAX;
    }
    OS_TPrintf("list->friendData[  %d ].trade_num  %d\n",no,list->friendData[no].trade_num);
    WifiList_SetLastPlayDate(list, no);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�t�B���񐔑����Z
 *
 *	@param	list		WIFILIST�|�C���^
 *	@param	no			���l�ڂ̗F�B��
 *	@param	pofin		�����Z��
 */
//-----------------------------------------------------------------------------
void WifiList_AddPorin( WIFI_LIST *list, int no, int pofin )
{
    list->friendData[no].pofin_num += pofin;
    if(list->friendData[no].pofin_num > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].pofin_num = WIFILIST_COUNT_RANGE_MAX;
    }
    WifiList_SetLastPlayDate(list, no);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʓ����~�j�Q�[���񐔃J�E���g
 *
 *	@param	list		���[�N
 *	@param	no			�F�Bnumber
 *	@param	addnum		�����l
 */
//-----------------------------------------------------------------------------
void WifiList_AddMinigameBallSlow( WIFI_LIST *list, int no, int addnum )
{
    list->friendData[no].minigame_ballslow += addnum;
    if(list->friendData[no].minigame_ballslow > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_ballslow = WIFILIST_COUNT_RANGE_MAX;
    }
    WifiList_SetLastPlayDate(list, no);
}
void WifiList_AddMinigameBalanceBall( WIFI_LIST *list, int no, int addnum )
{
    list->friendData[no].minigame_balanceball += addnum;
    if(list->friendData[no].minigame_balanceball > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_balanceball = WIFILIST_COUNT_RANGE_MAX;
    }
    WifiList_SetLastPlayDate(list, no);
}
void WifiList_AddMinigameBalloon( WIFI_LIST *list, int no, int addnum )
{
    list->friendData[no].minigame_balloon += addnum;
    if(list->friendData[no].minigame_balloon > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_balloon = WIFILIST_COUNT_RANGE_MAX;
    }
    WifiList_SetLastPlayDate(list, no);
}



//==============================================================================
/**
 * $brief   A�̐l�̃f�[�^��B�̐l�Ƀ}�[�W����
 * @param   list	WIFI_LIST�\���̂̃|�C���^
 * @param   no		���l�ڂ̗F�B��
 * @param   winNum  ������
 * @param   loseNum ������
 * @param   trade   ������
 * @retval  none
 */
//==============================================================================
void WifiList_DataMarge( WIFI_LIST *list, int delNo, int no)
{
    list->friendData[no].battle_win += list->friendData[delNo].battle_win;
    if(list->friendData[no].battle_win > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].battle_win = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].battle_lose += list->friendData[delNo].battle_lose;
    if(list->friendData[no].battle_lose > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].battle_lose = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].trade_num += list->friendData[delNo].trade_num;
    if(list->friendData[no].trade_num > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].trade_num = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].pofin_num += list->friendData[delNo].pofin_num;
    if(list->friendData[no].pofin_num > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].pofin_num = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].minigame_ballslow += list->friendData[delNo].minigame_ballslow;
    if(list->friendData[no].minigame_ballslow > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_ballslow = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].minigame_balanceball += list->friendData[delNo].minigame_balanceball;
    if(list->friendData[no].minigame_balanceball > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_balanceball = WIFILIST_COUNT_RANGE_MAX;
    }
    list->friendData[no].minigame_balloon += list->friendData[delNo].minigame_balloon;
    if(list->friendData[no].minigame_balloon > WIFILIST_COUNT_RANGE_MAX){
        list->friendData[no].minigame_balloon = WIFILIST_COUNT_RANGE_MAX;
    }

	// delNo�̂ق����ŐV�̏��Ȃ̂ŁA�O���[�v���͍ŐV�̂ق����c��
	// �f�[�^�Ƃ��Ďc��̂͑�{�̂ق��i�Â��ق��j�Ȃ̂ŁA���O�́A����Ă����B
	GFL_STD_MemCopy( list->friendData[delNo].groupName, list->friendData[no].groupName, ( sizeof(STRCODE)*(PERSON_NAME_SIZE + EOM_SIZE) ) );
	
	GFL_STD_MemClear(&list->friendData[delNo], sizeof(WIFI_FRIEND));
}



//---------------------------------------------------------------------------
/**
 * @brief	WIFI���X�g���Z�[�u�f�[�^��������o��
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	WIFI_LIST	WIFI���X�g
 */
//---------------------------------------------------------------------------
WIFI_LIST* SaveData_GetWifiListData(SAVE_CONTROL_WORK * sv)
{
	WIFI_LIST* pData;
	pData = SaveControl_DataPtrGet(sv, GMDATA_ID_WIFILIST);
	return pData;
}

