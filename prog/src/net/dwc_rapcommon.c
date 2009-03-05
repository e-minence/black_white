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
#include "gflib.h"

#if GFL_NET_WIFI

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

    pWork = GFL_HEAP_AllocClearMemory(heapID, DWC_INIT_WORK_SIZE+32);
    pTemp = (u8 *)( ((u32)pWork + 31) / 32 * 32 );
    // DWC���C�u����������
    ret = DWC_Init( pTemp );
    GFL_HEAP_FreeMemory(pWork);

	return ret;
}


//==============================================================================
/**
 * @brief   DWC  UserData�����
 * @param   DWCUserData  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  none
 */
//==============================================================================
void GFL_NET_WIFI_InitUserData( DWCUserData *userdata )
{
    // ���[�U�f�[�^�쐬������B
	MI_CpuClearFast(userdata, sizeof(DWCUserData));
    if( !DWC_CheckUserData( userdata ) ){
        DWC_CreateUserData( userdata, 'ADAJ' );
        DWC_ClearDirtyFlag( userdata );
        OS_TPrintf("WIFI���[�U�f�[�^���쐬\n");
    }
}

//==============================================================================
/**
 * @brief   ������GSID���擾����
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  �O�Ɓ|�P�͎��s   ���̒l�͐���
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
 * ������GSID���������̂���������
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  TRUE������
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
