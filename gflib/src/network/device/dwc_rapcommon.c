//=============================================================================
/**
 * @file	dwc_rapcommon.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

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

#include "dwc_rapcommon.h"

//==============================================================================
/**
 * DWC���C�u����������
 * @param   GFL_WIFI_FRIENDLIST  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
int mydwc_init(int heapID)
{
	int ret;
    u8* pWork;
    u8* pTemp;

    pWork = GFL_HEAP_AllocMemory(heapID, DWC_INIT_WORK_SIZE+32);
    pTemp = (u8 *)( ((u32)pWork + 31) / 32 * 32 );
    // DWC���C�u����������
    ret = DWC_Init( pTemp );
    GFL_HEAP_FreeMemory(pWork);

	return ret;
}


//==============================================================================
/**
 * DWC  UserData�����
 * @param   GFL_WIFI_FRIENDLIST  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
void mydwc_createUserData( GFL_WIFI_FRIENDLIST *pWifiList )
{
    // ���[�U�f�[�^�쐬������B    
    DWCUserData *userdata = WifiList_GetMyUserInfo(pWifiList);
    if( !DWC_CheckUserData( userdata ) ){
        DWC_CreateUserData( userdata, 'ADAJ' );
        DWC_ClearDirtyFlag( userdata );
        OS_TPrintf("WIFI���[�U�f�[�^���쐬\n");
    }
}

//==============================================================================
/**
 * ������GSID���擾����
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  �O�Ɓ|�P�͎��s   ���̒l�͐���
 */
//==============================================================================

int mydwc_getMyGSID(GFL_WIFI_FRIENDLIST *pWifiList)
{
    DWCUserData *userdata = WifiList_GetMyUserInfo(pWifiList);
    DWCFriendData friendData;

    DWC_CreateExchangeToken(userdata,&friendData);
    return DWC_GetGsProfileId( userdata,&friendData);
}

//==============================================================================
/**
 * ������GSID���������̂���������
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  TRUE������
 */
//==============================================================================
#if _SAVE_PROGRAM
BOOL mydwc_checkMyGSID(SAVEDATA *pSV)
{
    GFL_WIFI_FRIENDLIST* pList = SaveData_GetWifiListData(pSV);
    DWCUserData *userdata = WifiList_GetMyUserInfo(pList);

    if( DWC_CheckHasProfile( userdata )
       && DWC_CheckValidConsole( userdata ) ){
       return TRUE;
   }
   return FALSE;
}
#endif //_SAVE_PROGRAM
