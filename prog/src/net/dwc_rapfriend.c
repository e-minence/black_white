//=============================================================================
/**
 * @file	dwc_rapfriend.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

#if GFL_NET_WIFI

#include "net/dwc_rap.h"
#include "dwc_rapfriend.h"
#include "savedata/wifilist.h"



/**
 * @brief    ���łɓ����l���o�^����Ă��Ȃ����B
 * @param[in]   data ��������t�����h�R�[�h
 * @param[in]   myUserData ������DWCUserData
 * @param[in]   keyList    �Ƃ��������X�g�̃t�����h�R�[�h
 * @retval  DWCFRIEND_INLIST �c   ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
 * @retval  DWCFRIEND_OVERWRITE �c���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
 * @retval  DWCFRIEND_NODATA �c   �ʂ̃f�[�^
 */
static int _dwcfriend_CheckFriendByToken(DWCFriendData *data, DWCUserData *myUserData,DWCFriendData *matchFriend)
{
  // ���łɓ����l���o�^���Ă��Ȃ�������

  if( DWC_IsEqualFriendData( data, matchFriend ) )
  {    // �S�������f�[�^�������B
    return DWCFRIEND_INLIST;
  }
  else if( DWC_GetGsProfileId(myUserData,data) > 0 && DWC_GetGsProfileId(myUserData,data) == DWC_GetGsProfileId(myUserData,matchFriend) )
  {    // �t�����h�R�[�h�œo�^����Ă���f�[�^������B ���̏ꍇ�́A�㏑�����Ă��炤���Ƃ𐄏��B
    return DWCFRIEND_OVERWRITE;
  }
  return DWCFRIEND_NODATA;
}

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
 * @param  WIFI_LIST�̃|�C���^
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����friend_keya���������Ȃ��B
 */
//==============================================================================
int GFL_NET_DWC_CheckFriendCodeByToken( WIFI_LIST* list,u64 friend_key,int *index)
{
  // ���łɓ����l���o�^���Ă��Ȃ�������
  int i;
  DWCUserData *myUserData = WifiList_GetMyUserInfo(list);
  DWCFriendData *keyList  = WifiList_GetDwcDataPtr(list,0);
  int numMax  = WifiList_GetFriendDataNum(list);
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
void GFL_NET_DWC_WriteMyStatus(WIFI_LIST* pList, MYSTATUS* pMyStatus, int addListIndex, int heapID)
{
  STRBUF* pBuf;

  pBuf = MyStatus_CreateNameString(pMyStatus, heapID);
  WifiList_SetFriendName(pList, addListIndex, pBuf);
  GFL_STR_DeleteBuffer(pBuf);
  WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_SEX, MyStatus_GetMySex(pMyStatus));
  WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_ID, MyStatus_GetID(pMyStatus));
  WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_GAMESYNC_ID, MyStatus_GetProfileID(pMyStatus));
  WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_UNION_GRA, MyStatus_GetTrainerView(pMyStatus));
}



//------------------------------------------------------------------
/**
 * $brief   �����p�o�^�֐�
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
void GFL_NET_DWC_FriendDataWrite(GAMEDATA* pGameData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int addListIndex, int heapID, int overWrite)
{
  WIFI_LIST* pList = GAMEDATA_GetWiFiList(pGameData);
  DWCFriendData *keyList  = WifiList_GetDwcDataPtr(pList, addListIndex);
  u8 tr;

  if(overWrite != 2){
    MI_CpuCopy8(pFriend, keyList, sizeof(DWCFriendData));
  }
  if((overWrite == 0) || (overWrite == 1)){  //�d�l�ύX �S���㏑��
    GFL_NET_DWC_WriteMyStatus(pList, pMyStatus, addListIndex, heapID);
  }
  tr = MyStatus_GetTrainerView(pMyStatus);
  WifiList_SetFriendInfo(pList, addListIndex, WIFILIST_FRIEND_UNION_GRA, tr);
}

//------------------------------------------------------------------
/**
 * $brief   �����p�Ƃ������ǉ��֐�
 * @param   SAVE_CONTROL_WORK
 * @param   MYSTATUS
 * @param   DWCFriendData
 * @param   heapID
 * @retval  �V�K�o�^�̏ꍇTRUE
 */
//------------------------------------------------------------------

BOOL GFL_NET_DWC_FriendDataAdd(GAMEDATA* pGameData, MYSTATUS* pMyStatus,DWCFriendData* pFriend, int heapID)
{
  WIFI_LIST* pList = GAMEDATA_GetWiFiList(pGameData);
  int i,ret;

  if( !DWC_IsValidFriendData( pFriend ) ){
    return FALSE;
  }

  for(i = 0; i < WIFILIST_FRIEND_MAX;i++){
    DWCFriendData* pFL = WifiList_GetDwcDataPtr( pList, i );
    if( !WifiList_IsFriendData( pList, i ) ){  //�V�K
      GFL_NET_DWC_FriendDataWrite(pGameData, pMyStatus, pFriend, i, heapID, 0);
      return TRUE;
    }
    ret = _dwcfriend_CheckFriendByToken(pFriend,WifiList_GetMyUserInfo(pList),pFL);
    if(ret == DWCFRIEND_INLIST){ //����
      return FALSE;
    }
    else if(ret == DWCFRIEND_OVERWRITE){ //�㏑��
      GFL_NET_DWC_FriendDataWrite(pGameData, pMyStatus, pFriend, i, heapID, 1);
      return FALSE;
    }
  }
  return FALSE;
}



#endif //GFL_NET_WIFI
