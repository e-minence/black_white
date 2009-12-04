/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     util.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define WCM_NETWORK_CAPABILITY_KIND_MASK        0x0003
#define WCM_NETWORK_CAPABILITY_KIND_ESS         0x0001
#define WCM_NETWORK_CAPABILITY_KIND_IBSS        0x0002
#define WCM_NETWORK_CAPABILITY_PRIVACY          0x0010
#define WCM_NETWORK_CAPABILITY_PREAMBLE_MASK    0x0020
#define WCM_NETWORK_CAPABILITY_PREAMBLE_LONG    0x0000
#define WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT   0x0020

#define WCM_RSSI_COUNT_MAX                      16
#define WCM_RSSI_BORDER_HIGH                    28
#define WCM_RSSI_BORDER_MIDDLE                  22
#define WCM_RSSI_BORDER_LOW                     16

/*---------------------------------------------------------------------------*
    ���[�J���֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/

WCMInfoElement* WCMi_GetInfoElement(WCMBssDesc* bssDesc, u8 elementId, const u8 oui[3]);


/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/
static u8       wcmRssi[WCM_RSSI_COUNT_MAX];
static u8       wcmRssiIndex = 0;

static BOOL     WcmCompareMemory(const u8* a, const u8* b, u32 len)
{
    u32 i;

    for (i = 0; i < len; i++)
    {
        if (a[i] != b[i])
        {
            return FALSE;
        }
    }

    return TRUE;

}
/*---------------------------------------------------------------------------*
  Name:         WCM_CompareBssID

  Description:  �Q�� BSSID ���r����B�����֐��B

  Arguments:    a       -   ��r�Ώۂ� BSSID �ւ̃|�C���^���w�肷��B
                b       -   ��r�Ώۂ� BSSID �ւ̃|�C���^���w�肷��B

  Returns:      BOOL    -   �Q�� BSSID �������Ȃ�� TRUE ��Ԃ��B
                            �قȂ� BSSID �ł���ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_CompareBssID(const u8* a, const u8* b)
{
    return WcmCompareMemory(a, b, WCM_BSSID_SIZE);
}

BOOL WCM_CompareEssID(const u8* a, const u8* b)
{
    return WcmCompareMemory(a, b, WCM_ESSID_SIZE);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetLinkLevel

  Description:  DCF �ʐM���̃����N���x���擾����B�����֐��B

  Arguments:    None.

  Returns:      WMLinkLevel -   4�i�K�ɕ]�����������N���x��Ԃ��B
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_GetLinkLevel(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMLinkLevel ret = WM_LINK_LEVEL_0;

    // �������m�F
    if (w != NULL)
    {
        if (w->phase == WCM_PHASE_DCF)
        {
            ret = WCM_CalcLinkLevel( WCMi_GetRssiAverage() );
        }
    }

    (void)OS_RestoreInterrupts(e);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_CalcLinkLevel

  Description:  WCMBssDesc �̒��� RSSI �l����w�肵�������f�o�C�X�ɂ�����
                �����N���x�����߂�

  Arguments:    rssi         - WCMBssDesc �̒��� RSSI �l

  Returns:      WCMLinkLevel - 4�i�K�ɕ]�����������N���x��Ԃ��B
 *---------------------------------------------------------------------------*/
WCMLinkLevel WCM_CalcLinkLevel(u8 rssi)
{
    WCMWork*    w = WCMi_GetSystemWork();

    switch(w->deviceId)
    {
    default:
    case WCM_DEVICEID_WM:
        if (rssi >= WCM_RSSI_BORDER_HIGH)
        {
            return WM_LINK_LEVEL_3;
        }
        else if (rssi >= WCM_RSSI_BORDER_MIDDLE)
        {
            return WM_LINK_LEVEL_2;
        }
        else if (rssi >= WCM_RSSI_BORDER_LOW)
        {
            return WM_LINK_LEVEL_1;
        }
        return WM_LINK_LEVEL_0;
        break;
#ifdef SDK_TWL /* �V���� */
    case WCM_DEVICEID_NWM:
        if (rssi >= NWM_RSSI_INFRA_LINK_LEVEL_3)
        {
            return WM_LINK_LEVEL_3;
        }
        else if (rssi >= NWM_RSSI_INFRA_LINK_LEVEL_2)
        {
            return WM_LINK_LEVEL_2;
        }
        else if (rssi >= NWM_RSSI_INFRA_LINK_LEVEL_1)
        {
            return WM_LINK_LEVEL_1;
        }
        return WM_LINK_LEVEL_0;
        break;
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsInfrastructureNetwork

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��񂪃C���t���X�g���N�`���l�b�g���[�N
                �̂��̂ł��邩�ǂ����𒲂ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   �C���t���X�g���N�`���l�b�g���[�N�ł���ꍇ�� TRUE ���A
                            �����łȂ��ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsInfrastructureNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_ESS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsAdhocNetwork

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��񂪃A�h�z�b�N�l�b�g���[�N�̂���
                �ł��邩�ǂ����𒲂ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   �A�h�z�b�N�l�b�g���[�N�ł���ꍇ�� TRUE ���A
                            �����łȂ��ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsAdhocNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_KIND_MASK) == WCM_NETWORK_CAPABILITY_KIND_IBSS)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsPrivacyNetwork

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��� WEP �Z�L�����e�B�̓K�p���ꂽ
                �l�b�g���[�N�̂��̂ł��邩�ǂ����𒲂ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   WEP �Z�L�����e�B�̓K�p���ꂽ�l�b�g���[�N�ł���ꍇ��
                            TRUE ���A�����łȂ��ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsPrivacyNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if (bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsShortPreambleNetwork

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��񂪃V���[�g�v���A���u�����g�p����
                �l�b�g���[�N�̂��̂ł��邩�ǂ����𒲂ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   �V���[�g�v���A���u�����g�p�����l�b�g���[�N�ł���ꍇ��
                            TRUE ���A�����łȂ��ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsShortPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_SHORT)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsLongPreambleNetwork

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��񂪃����O�v���A���u�����g�p����
                �l�b�g���[�N�̂��̂ł��邩�ǂ����𒲂ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   �����O�v���A���u�����g�p�����l�b�g���[�N�ł���ꍇ��
                            TRUE ���A�����łȂ��ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsLongPreambleNetwork(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        if ((bssDesc->capaInfo & WCM_NETWORK_CAPABILITY_PREAMBLE_MASK) == WCM_NETWORK_CAPABILITY_PREAMBLE_LONG)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_IsEssidHidden

  Description:  AP �T�[�`���ʂƂ��ē���ꂽ��� ESS-ID ���B�����Ă��邩�ǂ���
                ���ׂ�B

  Arguments:    bssDesc -   ���ׂ� AP ��� ( �l�b�g���[�N��� )�B

  Returns:      BOOL    -   ESS-ID ���B������Ă���ꍇ�� TRUE ��Ԃ��B
                            �L���� ESS-ID �����J����Ă���ꍇ�ɂ� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCM_IsEssidHidden(WMBssDesc* bssDesc)
{
    if (bssDesc != NULL)
    {
        s32 i;

        if (bssDesc->ssidLength == 0)
        {
            return TRUE;    // ���� 0 �̏ꍇ�X�e���X
        }

        for (i = 0; (i < bssDesc->ssidLength) && (i < WM_SIZE_SSID); i++)
        {
            if (bssDesc->ssid[i] != 0x00)
            {
                return FALSE;
            }
        }

        return TRUE;        // �����Ԃ�S�� 0x00 �̏ꍇ�X�e���X
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetRssiAverage

  Description:  �����o�b�t�@�ɒ~�ς���Ă����M�d�g���x�̗�������A���ς�
                RSSI �l���擾����B

  Arguments:    None.

  Returns:      u8      -   ���� RSSI �l��Ԃ��B�����̕��ςȂ̂ŁA�ڑ������
                            ���Ȃ��ꍇ�Ȃǂł��ߋ��̗��������ɗL���Ȓl��Ԃ�
                            ���Ƃɒ��ӁB
 *---------------------------------------------------------------------------*/
u8 WCMi_GetRssiAverage(void)
{
    s32 sum = 0;
    s32 i;

    if (wcmRssiIndex > WCM_RSSI_COUNT_MAX)
    {
        for (i = 0; i < WCM_RSSI_COUNT_MAX; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= WCM_RSSI_COUNT_MAX;
    }
    else if (wcmRssiIndex > 0)
    {
        for (i = 0; i < wcmRssiIndex; i++)
        {
            sum += wcmRssi[i];
        }

        sum /= wcmRssiIndex;
    }

    return(u8) sum;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_ShelterRssi

  Description:  DCF �f�[�^����M�����ۂɁA��M�d�g���x�� RSSI �l������o�b�t�@
                �ɒ~�ς���B

  Arguments:    rssi    -   ��M�d�g���x�� RSSI �l���w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_ShelterRssi(u8 rssi)
{
    u8  raw;
    WCMWork*    w = WCMi_GetSystemWork();

    switch(w->deviceId)
    {
    default:
    case WCM_DEVICEID_WM:
        if (rssi & 0x02)
        {
            raw = (u8) (rssi >> 2);
        }
        else
        {
            raw = (u8) (25 + (rssi >> 2));
        }
        break;
#ifdef SDK_TWL /* �V���� */
    case WCM_DEVICEID_NWM:
            raw = rssi;
            break;
#endif
    }
    wcmRssi[wcmRssiIndex % WCM_RSSI_COUNT_MAX] = raw;
    if (wcmRssiIndex >= WCM_RSSI_COUNT_MAX)
    {
        wcmRssiIndex = (u8) (((1 + wcmRssiIndex) % WCM_RSSI_COUNT_MAX) + WCM_RSSI_COUNT_MAX);
    }
    else
    {
        wcmRssiIndex++;
    }

}

WCMInfoElement* WCM_GetInfoElement(WCMBssDesc* bssDesc, u8 elementId)
{
    return WCMi_GetInfoElement(bssDesc, elementId, NULL);
}

WCMInfoElement* WCM_GetVenderInfoElement(WCMBssDesc* bssDesc, const u8 oui[3])
{
    return WCMi_GetInfoElement(bssDesc, WCM_IE_ID_VENDOR_SPECIFIC, oui);
}

WCMInfoElement* WCMi_GetInfoElement(WCMBssDesc* bssDesc, u8 elementId, const u8 oui[3])
{
    WCMInfoElement* pIe = (WCMInfoElement*)(bssDesc->elements);
    
    int index;
    for ( index = 0; index < bssDesc->elementCount; ++index )
    {
        if (pIe->identifier == elementId
            && (!oui || MI_CpuComp8(oui, pIe->vender.oui, 3) == 0))
        {
            return pIe;
        }
        pIe = (WCMInfoElement*)&pIe->information[pIe->length];
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
