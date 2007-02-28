//=============================================================================
/**
 * @file	dwc_rapcommon.c
 * @bfief	DWCラッパー。常駐モジュールに置くもの
 * @author	kazuki yoshihara  -> k.ohno改造
 * @date	06/02/23
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"

#if GFL_NET_WIFI

#include "../net_def.h"
#include "dwc_rapcommon.h"

//==============================================================================
/**
 * DWCライブラリ初期化
 * @param   GFL_WIFI_FRIENDLIST  ユーザーデータがない場合作成
 * @retval  DS本体に保存するユーザIDのチェック・作成結果。
 */
//==============================================================================
int mydwc_init(int heapID)
{
	int ret;
    u8* pWork;
    u8* pTemp;

    pWork = GFL_HEAP_AllocMemory(heapID, DWC_INIT_WORK_SIZE+32);
    pTemp = (u8 *)( ((u32)pWork + 31) / 32 * 32 );
    // DWCライブラリ初期化
    ret = DWC_Init( pTemp );
    GFL_HEAP_FreeMemory(pWork);

	return ret;
}


//==============================================================================
/**
 * @brief   DWC  UserDataを作る
 * @param   DWCUserData  ユーザーデータがない場合作成
 * @retval  none
 */
//==============================================================================
void mydwc_createUserData( DWCUserData *userdata )
{
    // ユーザデータ作成をする。
	MI_CpuClearFast(userdata, sizeof(DWCUserData));
    if( !DWC_CheckUserData( userdata ) ){
        DWC_CreateUserData( userdata, 'ADAJ' );
        DWC_ClearDirtyFlag( userdata );
        OS_TPrintf("WIFIユーザデータを作成\n");
    }
}

//==============================================================================
/**
 * @brief   自分のGSIDを取得する
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  ０と−１は失敗   正の値は成功
 */
//==============================================================================

int mydwc_getMyGSID(void)
{
    DWCUserData *userdata = GFI_NET_GetMyDWCUserData();//WifiList_GetMyUserInfo(pWifiList);
    DWCFriendData friendData;

    GF_ASSERT(userdata);
    DWC_CreateExchangeToken(userdata,&friendData);
    return DWC_GetGsProfileId( userdata,&friendData);
}

//==============================================================================
/**
 * 自分のGSIDが正しいのか検査する
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  TRUEが成功
 */
//==============================================================================
BOOL mydwc_checkMyGSID(void)
{
    DWCUserData *userdata = GFI_NET_GetMyDWCUserData();

    GF_ASSERT(userdata);
    if( DWC_CheckHasProfile( userdata )
       && DWC_CheckValidConsole( userdata ) ){
       return TRUE;
   }
   return FALSE;
}

#endif GFL_NET_WIFI
