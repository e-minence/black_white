//======================================================================================
/**
 * @file  event_debug_menu_wifinote.c
 * @author  tamada
 * @date  2010.07.10
 *
 */
//======================================================================================
#ifdef  PM_DEBUG

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "event_debug_menu_wifinote.h"
#include "net/dwc_rapfriend.h"

#include "system/main.h"  //HEAPID_
#include "debug/debug_str_conv.h"

//======================================================================================
//======================================================================================
//-------------------------------------
/// �F�B�f�[�^�V�K�ݒ�֐��@�߂�l
//=====================================
enum{
  RCODE_NEWFRIEND_SET_OK, // �ݒ�ł���
  RCODE_NEWFRIEND_SET_FRIENDKEYNG,  // �F�B�R�[�h���s��
  RCODE_NEWFRIEND_SET_SETING, // �����ݒ肵�Ă�����
  RCODE_NEWFRIEND_SET_FULL,   // �蒠�������ς�
  RCODE_NEWFRIEND_SET_NUM

};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
typedef struct {
  u64 code;
  const char * name;
}DEBUG_WIFINOTE_CONST_DATA, DWCD;

#include "../../../resource/wifinote_list/gamefreak_list.cdat"

static const char * const DebugResultStr[] = {
  "SET_OK ",
  "NG CODE",
  "ALREADY",
  "FULL   ",
};
//======================================================================================
//======================================================================================
static u64 WFNOTE_STRBUF_GetNumber( STRBUF *str , BOOL *pbFlag );


static void entryCDat( GAMEDATA * gamedata );
static u32 DEBUG_SetNewFriendData( GAMEDATA * gamedata, u64 friendcode, STRBUF* p_name );

//======================================================================================
//======================================================================================
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DEBUG_EntryWifiNote( GAMESYS_WORK * gsys )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  entryCDat( gamedata );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void entryCDat( GAMEDATA * gamedata )
{ 
  const DEBUG_WIFINOTE_CONST_DATA * dwncd = DebugWifiNoteConstData;
  const int dwncd_max = NELEMS(DebugWifiNoteConstData);
  STRBUF * tempBuf;
  u16 pStrCode[64];
  int i;
  int result;
  
  // �F�B���S�N���A
  {
    WIFI_LIST* p_list = GAMEDATA_GetWiFiList(gamedata); 
    for( i=0; i<WIFILIST_FRIEND_MAX; i++ ){
      WifiList_ResetData( p_list, 0 );
    }
  }

  tempBuf = GFL_STR_CreateBuffer( 16, HEAPID_FIELDMAP );

  for ( i = 0; i < dwncd_max; dwncd ++, i ++ )
  {
    DEB_STR_CONV_SjisToStrcode( dwncd->name, pStrCode, 64 );
    GFL_STR_SetStringCode( tempBuf, pStrCode );
    result = DEBUG_SetNewFriendData( gamedata, dwncd->code, tempBuf );
    OS_Printf("RESULT:%d ID:%012d\n", result, dwncd->code );
  }

  GFL_STR_DeleteBuffer( tempBuf );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ŐV�̗F�B�f�[�^�ݒ�
 *
 *  @param  p_data    ���[�N
 *  @param  p_code    �R�[�h������
 *  @param  p_name    ���O������
 */
//-----------------------------------------------------------------------------
static u32 DEBUG_SetNewFriendData( GAMEDATA * gamedata, u64 friendcode, STRBUF* p_name )
{
  int i;
  WIFI_LIST* p_list;
  u64 mycode;

  p_list = GAMEDATA_GetWiFiList(gamedata); //SaveData_GetWifiListData( p_data->p_save );

  mycode = DWC_CreateFriendKey( WifiList_GetMyUserInfo(p_list) );

  if ( mycode == friendcode ){
    // �F�B�R�[�h���Ⴄ
    return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
  }

  for( i=0; i<WIFILIST_FRIEND_MAX; i++ ){
    BOOL result;
    result = WifiList_IsFriendData( p_list, i );
    if( result == FALSE ){
      DWCFriendData *p_dwc_list = NULL;
      DWCUserData *p_dwc_userdata = NULL;
      int ret;
      int pos;

      p_dwc_userdata = WifiList_GetMyUserInfo( p_list );
      if(!DWC_CheckFriendKey(p_dwc_userdata, friendcode))
      {
        // �F�B�R�[�h���Ⴄ
        return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
      }
      ret = GFL_NET_DWC_CheckFriendCodeByToken(p_list, friendcode, &pos);
      if(ret == DWCFRIEND_INLIST){
        // �����ݒ肸��
        return RCODE_NEWFRIEND_SET_SETING;
      }
      // ����
      p_dwc_list = WifiList_GetDwcDataPtr(p_list, i);
      DWC_CreateFriendKeyToken(p_dwc_list, friendcode);
      // ���O�Ɛ��ʐݒ� �����Őݒ�
      WifiList_SetFriendName(p_list, i, p_name);
      WifiList_SetFriendInfo(p_list, i, WIFILIST_FRIEND_SEX, PM_NEUTRAL);
      return RCODE_NEWFRIEND_SET_OK;
    }
  }
  return RCODE_NEWFRIEND_SET_FULL;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ŐV�̗F�B�f�[�^�ݒ�
 *
 *  @param  p_data    ���[�N
 *  @param  p_code    �R�[�h������
 *  @param  p_name    ���O������
 */
//-----------------------------------------------------------------------------
static u32 Data_NewFriendDataSet( GAMEDATA * gamedata, STRBUF* p_code, STRBUF* p_name )
{
  u64 friendcode;
  BOOL set_check;
  friendcode = WFNOTE_STRBUF_GetNumber(p_code, &set_check);
  if (set_check == FALSE )
  {
    return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
  }
  return DEBUG_SetNewFriendData( gamedata, friendcode, p_name );
#if 0
  int i;
  WIFI_LIST* p_list;
  BOOL result;
  BOOL set_check;
  u64 friendcode;
  u64 mycode;
  DWCFriendData *p_dwc_list = NULL;
  DWCUserData *p_dwc_userdata = NULL;
  int ret;
  int pos;

  p_list = GAMEDATA_GetWiFiList(gamedata); //SaveData_GetWifiListData( p_data->p_save );

  mycode = DWC_CreateFriendKey( WifiList_GetMyUserInfo(p_list) );

  for( i=0; i<WIFILIST_FRIEND_MAX; i++ ){
    result = WifiList_IsFriendData( p_list, i );
    if( result == FALSE ){
      friendcode = WFNOTE_STRBUF_GetNumber(p_code, &set_check);

      if( (set_check == TRUE) && (friendcode !=mycode) ){

        p_dwc_userdata = WifiList_GetMyUserInfo( p_list );
        if(!DWC_CheckFriendKey(p_dwc_userdata, friendcode))
        {
          // �F�B�R�[�h���Ⴄ
          return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
        }
        ret = GFL_NET_DWC_CheckFriendCodeByToken(p_list, friendcode, &pos);
        if(ret == DWCFRIEND_INLIST){
          // �����ݒ肸��
          return RCODE_NEWFRIEND_SET_SETING;
        }
        // ����
        p_dwc_list = WifiList_GetDwcDataPtr(p_list, i);
        DWC_CreateFriendKeyToken(p_dwc_list, friendcode);
        // ���O�Ɛ��ʐݒ� �����Őݒ�
        WifiList_SetFriendName(p_list, i, p_name);
        WifiList_SetFriendInfo(p_list, i, WIFILIST_FRIEND_SEX, PM_NEUTRAL);
        return RCODE_NEWFRIEND_SET_OK;
      }else{
        // �F�B�R�[�h���Ⴄ
        return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
      }
    }
  }

  GF_ASSERT(0);
  // �����͂Ƃ���Ȃ��͂�
  return RCODE_NEWFRIEND_SET_OK;
#endif
}

//------------------------------------------------------------------
/**
 * ���������𐔎��ɕϊ�����  18�����Ή�  ���̊֐��͕����R�[�h�ɍ��E�����
 *
 * @param src     �ϊ������������������Ă���o�b�t�@
 * @param pbFlag    �ϊ��ł������ǂ���
 * @return  ���������ꍇ�ϊ�����
 *
 *  GS�ł�WFNOTE���ł����g���Ă��Ȃ��̂ł����ɈڐA
 *  ���[�`���������R�[�h����UNICODE�ɕς�����̂ŕύX
 */
//------------------------------------------------------------------
static u64 WFNOTE_STRBUF_GetNumber( STRBUF *str , BOOL *pbFlag )
{
  const STRCODE eonCode = GFL_STR_GetEOMCode();
  const STRCODE *code = GFL_STR_GetStringCodePointer( str );

  u64 num = 0;
  u64 digit = 1;
  u16 arrDigit = 0;
  u16 tempNo;

  static const u8 MAX_DIGIT = 18;

  *pbFlag = FALSE;
  while( code[arrDigit] != eonCode )
  {
    if( arrDigit >= MAX_DIGIT )
    {
      //���I�[�o�[
      return num;
    }

    tempNo = code[arrDigit] - L'0';
    if( tempNo >= 10 )
    {
      tempNo = code[arrDigit] - L'�O';
      if( tempNo >= 10 )
      {
        //�z��O�̕����������Ă���
        return num;
      }
    }
    /*
    num += tempNo * digit;
    digit *= 10;
    */
    num *= 10;
    num += tempNo;
    arrDigit++;
  }
  *pbFlag = TRUE;
  return num;
}

#endif  //PM_DEBUG
