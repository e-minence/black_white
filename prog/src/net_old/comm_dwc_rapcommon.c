//=============================================================================
/**
 * @file	dwc_rapcommon.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

#include <gflib.h>
/*��[GS_CONVERT_TAG]*/
#include <dwc.h>
#include "net_old/comm_dwc_rap.h"
#include "net_old/comm_dwc_rapcommon.h"

//==============================================================================
/**
 * DWC���C�u����������
 * @param   WIFI_LIST  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
int OLDmydwc_initdwc(int heapID)
{
	int ret;
    u8* pWork;
    u8* pTemp;
   
    pWork = GFL_HEAP_AllocMemory(heapID, DWC_INIT_WORK_SIZE+32);
/*��[GS_CONVERT_TAG]*/
    pTemp = (u8 *)( ((u32)pWork + 31) / 32 * 32 );
    // DWC���C�u����������
    ret = DWC_Init( pTemp );
    GFL_HEAP_FreeMemory(pWork);
/*��[GS_CONVERT_TAG]*/
    return ret;
}

#if 0 //�g�p���Ă��Ȃ��̂Ŗ����� 2009.06.04(��) matsuda
int OLDmydwc_init(int heapID)
{
	int ret;

    DwcOverlayStart();
    DpwCommonOverlayStart();
    
    // DWC���C�u����������
    ret = OLDmydwc_initdwc(heapID);

    DwcOverlayEnd();
    DpwCommonOverlayEnd();
	return ret;
}

//==============================================================================
/**
 * DWC  UserData�����
 * @param   WIFI_LIST  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
void OLDmydwc_createUserData( WIFI_LIST *pWifiList )
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
 * @param   WIFI_LIST  
 * @retval  �O�Ɓ|�P�͎��s   ���̒l�͐���
 */
//==============================================================================

int OLDmydwc_getMyGSID(WIFI_LIST *pWifiList)
{
    DWCUserData *userdata = WifiList_GetMyUserInfo(pWifiList);
    DWCFriendData friendData;

    DWC_CreateExchangeToken(userdata,&friendData);
    return DWC_GetGsProfileId( userdata,&friendData);
}

//==============================================================================
/**
 * ������GSID���������̂���������
 * @param   WIFI_LIST  
 * @retval  TRUE������
 */
//==============================================================================

BOOL OLDmydwc_checkMyGSID(SAVEDATA *pSV)
{
    WIFI_LIST* pList = SaveData_GetWifiListData(pSV);
    DWCUserData *userdata = WifiList_GetMyUserInfo(pList);

    if( DWC_CheckHasProfile( userdata )
       && DWC_CheckValidConsole( userdata ) ){
       return TRUE;
   }
   return FALSE;
}
#endif

