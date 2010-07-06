/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WCM - demos - wcm-list-2
  File:     privacy_check.c

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

/*****************************************************************

///////////////////////////////////////////////
//                                           //
//     Infomation Element �t�H�[�}�b�g�ڍ�   //
//                                           //
///////////////////////////////////////////////
*
*WPA IE �t�H�[�}�b�g
*    Element ID          ( 1 Byte) :     221(�Œ�)
*    Length              ( 1 Byte) :     IE �̃o�C�g��
*    OUI                 ( 3 Byte) :     00:50:F2(�Œ�)
*    SubType             ( 1 Byte) :     0x01(�Œ�)
*    Version             ( 2 Byte) :     0x0001(�Œ�)
*    GroupCipherType     ( 4 Byte) :     OUI(00�F50�FF2) + KeyType(*)
*    PairwiseSetCount    ( 2 Byte) :     �T�|�[�g���� PairwiseCipherType �̐�
*    PairwiseCipherType  (per 4 Byte) :  OUI(00�F50�FF2) + KeyType(*) �̃Z�b�g��
*                                        PairwiseSetCount �̌�����
*    AuthSetCount        ( 2 Byte) :     �T�|�[�g���� AuthenticationType �̐�
*    AuthenticationType  (per 4 Byte) :  OUI(00�F50�FF2) + AuthType(*) �̃Z�b�g��
*                                        AuthSetCount �̌�����
*    RSN Capability      ( 2 Byte) :     ��{�I�ɎQ�Ƃ���K�v�Ȃ�
*                                         ���O�F��            ( 1 bit)
*                                         �y�A�Ȃ�            ( 1 bit)
*                                         PTK ���v���C�J�E���^( 2 bit)
*                                         GTK ���v���C�J�E���^( 2 bit)
*                                         Reserved            (10 bit)
*    PMK �J�E���g        ( 2 Byte) :     �ڍׂ͎d�l���Q�ƁB
*    PMK ���X�g          (per 16 Byte) : �ڍׂ͎d�l���Q�ƁB
*
*RSN(WPA2) IE �t�H�[�}�b�g
*    Element ID          ( 1 Byte) :     48(�Œ�)
*    Length              ( 1 Byte) :     IE �̃o�C�g��
*    Version             ( 2 Byte) :     0x0001(�Œ�)
*    GroupCipherType     ( 4 Byte) :     OUI(00:0F:AC) + KeyType(*)
*    PairwiseSetCount    ( 2 Byte) :     �T�|�[�g���� PairwiseCipherType �̐�
*    PairwiseCipherType  (per 4 Byte) :  OUI(00:0F:AC) + KeyType(*) �̃Z�b�g��
*                                        PairwiseSetCount �̌�����
*    AuthSetCount        ( 2 Byte) :     �T�|�[�g���� AuthenticationType �̐�
*    AuthenticationType  (per 4 Byte) :  OUI(00:0F:AC) + AuthType(*) �̃Z�b�g��
*                                        AuthSetCount �̌�����
*    RSN Capability      ( 2 Byte) :     ��{�I�ɎQ�Ƃ���K�v�Ȃ�
*                                         ���O�F��            ( 1 bit)
*                                         �y�A�Ȃ�            ( 1 bit)
*                                         PTK ���v���C�J�E���^( 2 bit)
*                                         GTK ���v���C�J�E���^( 2 bit)
*                                         Reserved            (10 bit)
*    PMK �J�E���g        ( 2 Byte) :     �ڍׂ͎d�l���Q�ƁB
*    PMK ���X�g          (per 16 Byte) : �ڍׂ͎d�l���Q�ƁB
*
*
* (*) KeyType :
*            0 ... GroupCipher �ɏ]�� (PairwiseCipher �̂ݎw��\)
*            1 ... WEP40
*            2 ... TKIP
*            3 ... Reserved
*            4 ... CCMP(AES)
*            5 ... WEP104
*
* (*) AuthType :
*            1 ... 802.1X or PMK cache
*            2 ... PSK
*
*******************************************************************/

#include "privacy_check.h"



extern u8 gDeviceID; //�����f�o�C�XID

/* �֐��v���g�^�C�v */

u8 GetPrivacyMode(const WCMBssDesc *bssDesc);
u8 GetWPA2PrivacyMode(const WCMBssDesc *bssDesc);
u8 GetWPAPrivacyMode(const WCMBssDesc *bssDesc);

static int FindWifiProtectedAccess(u8** bufPtr, const WCMBssDesc* bssDesc);
static int FindInformationElement(u8** bufPtr, const WCMBssDesc* bssDesc, u8 elementID);
static int FindVendorSpecificIE(u8** bufPtr, const WCMBssDesc* bssDesc, u8 elementID, const u8* tag, u8 version);


/*-----------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         GetWPAPrivacyMode

  Description:  �X�L�����ɂ��擾���� BSSDesc ����AAP �̃T�|�[�g���� WPA �֘A�̈Í����������擾����B
                
  Arguments:    bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B

  Returns:      u8            - WCM_PRIVACYMODE_* �̂����ꂩ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 GetWPAPrivacyMode(const WCMBssDesc *bssDesc)
{
    u8* bufPtr;
    u8 multicastCipher = WCM_PRIVACYMODE_NONE;
    u8 unicastCipher = WCM_PRIVACYMODE_NONE;
    int length = FindWifiProtectedAccess(&bufPtr, bssDesc);
    
    if( length != -1 && (u32)length >  ( sizeof(u16) + sizeof(WPACipherType) +  sizeof(u16) ) ) //�T�C�Y�`�F�b�N�̑O�i�Ƃ��āAPairwizeSetCount �����邩���`�F�b�N
    {
        u8 oui[3] = {0x00, 0x50, 0xF2};	// WPA OUI
        u16 num;

        (void)MI_CpuCopy8(bufPtr + sizeof(u16) + sizeof(WPACipherType), &num, sizeof(16)); //Num �̎擾

        if( (u32)length <  sizeof(u16) + sizeof(WPACipherType) +  sizeof(u16) + (sizeof(WPACipherType)*num) )
        {
            return WCM_PRIVACYMODE_NONE; //Unicast Cipher ���������󂯎��Ȃ��ꍇ�A��
        }

        {
            WPACipherType group;

            (void)MI_CpuCopy8(bufPtr + sizeof(u16), &group, sizeof(WPACipherType));
            if( MI_CpuComp8( group.oui, oui, sizeof(u8) * 3) == 0)
            {
                switch( group.type )
                {
                    case WPA_KEYTYPE_WEP_40:
                        multicastCipher = WCM_PRIVACYMODE_WEP40;
                        break;
                    case WPA_KEYTYPE_TKIP:
                        multicastCipher = WCM_PRIVACYMODE_WPA_TKIP;
                        break;
                    case WPA_KEYTYPE_AES:
                        multicastCipher = WCM_PRIVACYMODE_WPA_AES;
                        break;
                    case WPA_KEYTYPE_WEP104:
                        multicastCipher = WCM_PRIVACYMODE_WEP104;
                        break;
                    default:
                        multicastCipher = WCM_PRIVACYMODE_NONE;
                        break;
                }
            }
        }

        if( multicastCipher == WCM_PRIVACYMODE_WEP104 || multicastCipher == WCM_PRIVACYMODE_WEP40 )
        {
            return multicastCipher;
        }
        
        /*
        ��{�I�ɂ� Wii �Ɠ��������p���邪�A MulticastCipherType �� UnicastCipherType ���قȂ�
        �ꕔ�̖��� AP �ɑΉ����邽�߂ɁA�I�����ꂽ������ TKIP/AES �ł���� ���̕�����
        UnicastCipherType �ɒ�`����Ă��邩�m�F���A��`����Ă��Ȃ��ꍇ�� UnicastCipherType �̕������̗p����B
        */
        {
            u16 num; // AP ���T�|�[�g���Ă�������̐�
            int i;

            (void)MI_CpuCopy8(bufPtr + sizeof(u16) + sizeof(WPACipherType), &num, sizeof(16)); //PairwiseSetCount �̎擾

            for(i=0 ;i < num; i++)
            {
                WPACipherType pairwise;

                (void)MI_CpuCopy8(bufPtr + sizeof(u16) + sizeof(WPACipherType) + sizeof(u16) + (sizeof(WPACipherType) * i), &pairwise, sizeof(WPACipherType));
                if( MI_CpuComp8( pairwise.oui, oui, sizeof(u8) * 3) == 0)
                {
                    u8 tmpCipher;
                    
                    switch( pairwise.type )
                    {
                    case WPA_KEYTYPE_WEP_40:
                        tmpCipher = WCM_PRIVACYMODE_WEP40;
                        break;
                    case WPA_KEYTYPE_TKIP:
                        tmpCipher = WCM_PRIVACYMODE_WPA_TKIP;
                        break;
                    case WPA_KEYTYPE_AES:
                        tmpCipher = WCM_PRIVACYMODE_WPA_AES;
                        break;
                    case WPA_KEYTYPE_WEP104:
                        tmpCipher = WCM_PRIVACYMODE_WEP104;
                        break;
                    case WPA_KEYTYPE_GPOUP_CIPHER:
                        tmpCipher = multicastCipher;
                        break;
                    default:
                        /* ���m�̈Í��������ɂ��Ă̓m�[�P�A�Ƃ��� */
                        tmpCipher = WCM_PRIVACYMODE_NONE;
                        break;
                    }
                    
                    // MulticastCipher �Ɠ�������������� return
                    if( tmpCipher == multicastCipher )
                    {
                        return multicastCipher;
                    }
                    else
                    {
                        if( unicastCipher < tmpCipher )
                        {
                            unicastCipher = tmpCipher;
                        }
                    }
                }
            }
        return unicastCipher; //MulticastCipher �ƈ�v���Ȃ������̂ŁAUnicastCipher ���̗p
        }
    }
    return WCM_PRIVACYMODE_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         GetWPA2PrivacyMode

  Description:  �X�L�����ɂ��擾���� BSSDesc ����AAP �̃T�|�[�g���� WPA2 �֘A�̈Í����������擾����B
                
  Arguments:    bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B

  Returns:      u8            - WCM_PRIVACYMODE_* �̂����ꂩ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 GetWPA2PrivacyMode(const WCMBssDesc *bssDesc)
{
    u8* bufPtr;
    u8 multicastCipher = WCM_PRIVACYMODE_NONE;
    u8 unicastCipher = WCM_PRIVACYMODE_NONE;
    int length = FindInformationElement(&bufPtr, bssDesc, ELEMENTID_RSN);
    
    // id = 48 (RSN)
    if( length != -1 && (u32)length > ( sizeof(u16) + sizeof(WPACipherType) +  sizeof(u16) ) ) //�T�C�Y�`�F�b�N�̑O�i�Ƃ��āAPairwizeSetCount �����邩���`�F�b�N
    {
        u8 oui[3] = {0x00, 0x0F, 0xAC};	// 802.11i
        u16 num;

        (void)MI_CpuCopy8(bufPtr + sizeof(u16) + sizeof(WPACipherType), &num, sizeof(16)); //Num �̎擾

        if( (u32)length <  sizeof(u16) + sizeof(WPACipherType) +  sizeof(u16) + (sizeof(WPACipherType)*num) )
        {
            return WCM_PRIVACYMODE_NONE; //Unicast Cipher ���������󂯎��Ȃ��ꍇ�A��
        }
        
        {
            WPACipherType group;

            (void)MI_CpuCopy8(bufPtr + sizeof(u16), &group, sizeof(WPACipherType));
            if( MI_CpuComp8( group.oui, oui, sizeof(u8) * 3) == 0)
            {
                switch( group.type )
                {
                    case WPA_KEYTYPE_WEP_40:
                        multicastCipher = WCM_PRIVACYMODE_WEP40;
                        break;
                    case WPA_KEYTYPE_TKIP:
                        multicastCipher = WCM_PRIVACYMODE_WPA2_TKIP;
                        break;
                    case WPA_KEYTYPE_AES:
                        multicastCipher = WCM_PRIVACYMODE_WPA2_AES;
                        break;
                    case WPA_KEYTYPE_WEP104:
                        multicastCipher = WCM_PRIVACYMODE_WEP104;
                        break;
                    default:
                        multicastCipher = WCM_PRIVACYMODE_NONE;
                        break;
                }
            }
        }

        if( multicastCipher == WCM_PRIVACYMODE_WEP104 || multicastCipher == WCM_PRIVACYMODE_WEP40 )
        {
            return multicastCipher;
        }
        
        /*
        ��{�I�ɂ� Wii �Ɠ��������p���邪�A MulticastCipherType �� UnicastCipherType ���قȂ�
        �ꕔ�̖��� AP �ɑΉ����邽�߂ɁA�I�����ꂽ������ TKIP/AES �ł���� ���̕�����
        UnicastCipherType �ɒ�`����Ă��邩�m�F���A��`����Ă��Ȃ��ꍇ�� UnicastCipherType �̕������̗p����B
        */
        {
            u16 num; // AP ���T�|�[�g���Ă�������̐�
            int i;

            //(void)MI_CpuCopy8(bufPtr + sizeof(u8) + sizeof(WPACipherType), &num, sizeof(16));
            (void)MI_CpuCopy8(bufPtr + + sizeof(u16) + sizeof(WPACipherType), &num, sizeof(16)); // miz

            for(i=0 ;i < num; i++)
            {
                WPACipherType pairwise;

                //(void)MI_CpuCopy8(bufPtr + sizeof(u8) + sizeof(WPACipherType) + sizeof(u16) + (sizeof(WPACipherType) * i), &pairwise, sizeof(WPACipherType));
                (void)MI_CpuCopy8(bufPtr + sizeof(u16) + sizeof(WPACipherType) + sizeof(u16) + (sizeof(WPACipherType) * i), &pairwise, sizeof(WPACipherType)); // miz
                if( MI_CpuComp8( pairwise.oui, oui, sizeof(u8) * 3) == 0)
                {
                    u8 tmpCipher;
                    
                    switch( pairwise.type )
                    {
                    case WPA_KEYTYPE_WEP_40:
                        tmpCipher = WCM_PRIVACYMODE_WEP40;
                        break;
                    case WPA_KEYTYPE_TKIP:
                        tmpCipher = WCM_PRIVACYMODE_WPA2_TKIP;
                        break;
                    case WPA_KEYTYPE_AES:
                        tmpCipher = WCM_PRIVACYMODE_WPA2_AES;
                        break;
                    case WPA_KEYTYPE_WEP104:
                        tmpCipher = WCM_PRIVACYMODE_WEP104;
                        break;
                    case WPA_KEYTYPE_GPOUP_CIPHER:
                        tmpCipher = multicastCipher;
                        break;
                    default:
                        /* ���m�̈Í��������ɂ��Ă̓m�[�P�A�Ƃ��� */
                        tmpCipher = WCM_PRIVACYMODE_NONE;
                        break;
                    }
                    
                    // MulticastCipher �Ɠ�������������� return
                    if( tmpCipher == multicastCipher )
                    {
                        return multicastCipher;
                    }
                    else
                    {
                        if( unicastCipher < tmpCipher )
                        {
                            unicastCipher = tmpCipher;
                        }
                    }
                }
            }
        return unicastCipher; //MulticastCipher �ƈ�v���Ȃ������̂ŁAUnicastCipher ���̗p
        }
    }
    return WCM_PRIVACYMODE_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         GetPrivacyMode

  Description:  �X�L�����ɂ��擾���� BSSDesc ����AAP �̃T�|�[�g����Í����������擾����B
                
  Arguments:    bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B

  Returns:      u8            - WCM_PRIVACYMODE_* �̂����ꂩ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 GetPrivacyMode(const WCMBssDesc *bssDesc)
{
    u8 wpa2Key, wpaKey;

    wpa2Key = GetWPA2PrivacyMode(bssDesc);

    if ( wpa2Key != WCM_PRIVACYMODE_NONE )
    {
        return wpa2Key;
    }

    wpaKey = GetWPAPrivacyMode(bssDesc);

    if ( wpaKey != WCM_PRIVACYMODE_NONE )
    {
        return wpaKey;
    }

    /* WPA IE �� RSN(WPA) IE �����������ꍇ */
    if( wpa2Key ==  WCM_PRIVACYMODE_NONE && wpaKey == WCM_PRIVACYMODE_NONE)
    {
        /* WPA or WPA2 �̈Í������g���Ă��Ȃ� ���� �Í����͂���Ă���ꍇ�� WEP */
        if ( bssDesc && ( ( bssDesc->capaInfo & 0x0010 ) == 0x0010 ) )
        {
            return WCM_PRIVACYMODE_WEP; /* ���̏ꍇ WEP �̌����͕s���Ȏ��ɗ��� */
        }
        else /* �Í�������Ă��Ȃ� */
        {
            return WCM_PRIVACYMODE_NONE;
        }
    }

    return WCM_PRIVACYMODE_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         FindInformationElement

  Description:  ����� Information Element ���擾����B
                
  Arguments:    bufPtr        - �����������G�������g��Version�ȉ��̃f�[�^�̐擪�̃|�C���^���i�[�����B
                bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B
                elementID     - �����̑ΏۂƂȂ���G�������gID

  Returns:      int ���݂����ꍇ�� Lengh �t�B�[���h���A���݂��Ȃ��ꍇ�� -1 ��Ԃ��B
 *---------------------------------------------------------------------------*/
static int FindInformationElement(u8** bufPtr, const WCMBssDesc* bssDesc, u8 elementID)
{
    int i;
#ifdef SDK_TWL
    if(gDeviceID == WCM_DEVICEID_WM) /* ���������g�����ꍇ */
#endif
    {
        WMOtherElements elem = WM_GetOtherElements((WMBssDesc*)bssDesc);

        for(i = 0; i < elem.count; i++)
        {
            if(elem.element[i].id == elementID)
            {
                *bufPtr = elem.element[i].body;
                return (int)(elem.element[i].length);
            }
        }
    }
#ifdef SDK_TWL
    else
    {
        if(OS_IsRunOnTwl())
        {
            if(gDeviceID == WCM_DEVICEID_NWM) /* �V�������g�����ꍇ */
            {
                NWMInfoElements *elem = NWM_GetInfoElements((NWMBssDesc*)bssDesc, elementID);

                if( elem != NULL)
                {
                    *bufPtr = (u8*)(elem->element);
                    
                    return (int)(elem->length);
                }
            }
        }
    }
#endif
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         FindWifiProtectedAccess

  Description:  WPA Information Element ���擾����B
                
  Arguments:    bufPtr        - �����������G�������g��Version�ȉ��̃f�[�^�̐擪�̃|�C���^���i�[�����B
                bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B

  Returns:      ���݂����ꍇ��TRUE
 *---------------------------------------------------------------------------*/

static int FindWifiProtectedAccess(u8** bufPtr, const WCMBssDesc* bssDesc)
{
    const u8 tag[4] = { 0x00, 0x50, 0xF2, 0x01 };   // WPA �ŗL�̃^�O
    return FindVendorSpecificIE(bufPtr, bssDesc, ELEMENTID_VENDOR_SPECIFIC, tag, 0x01);
}

/*---------------------------------------------------------------------------*
  Name:         FindVendorSpecificIE

  Description:  Vendor Specific Information Element ���擾����B
                �������AID, Length, tag ���Ȃ��B
                
  Arguments:    bufPtr        - �����������G�������g��Version�ȉ��̃f�[�^�̐擪�̃|�C���^���i�[�����B
                bssDesc       - �����̑ΏۂƂȂ� WCMBssDesc �^�B
                elementID     - �����̑ΏۂƂȂ���G�������gID (Vendor Specific IE �� 221)
                tag           - WPA �ŗL�̃^�O�Ƃ��� version �t�B�[���h�̑O�ɕt������� 4�o�C�g
                version       - �������s�����G�������g�� version �l�B

  Returns:      int ���݂����ꍇ�� Lengh �t�B�[���h���A���݂��Ȃ��ꍇ�� -1 ��Ԃ��B
 *---------------------------------------------------------------------------*/
static int FindVendorSpecificIE(u8** bufPtr, const WCMBssDesc* bssDesc, u8 elementID, const u8* tag, u8 version)
{
    int result = -1;

#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        if(gDeviceID == WCM_DEVICEID_NWM) /* �V�������g�����ꍇ */
        {
            // WM �� NWM �� BssDesc �ɂ����� IE �̈����������قȂ邽�ߏ����𕪂���
            NWMInfoElements *elem = NWM_GetVenderInfoElements((NWMBssDesc*)bssDesc, elementID, (u8*)tag);
    
            if( elem != NULL)
            {
                *bufPtr = (u8*)(elem->element) + 4;
                result = (int)(elem->length);
            }
        }
    }
#endif
    if(gDeviceID == WCM_DEVICEID_WM ) /* ���������g�����ꍇ */
    {
        int i;
        WMOtherElements elem = WM_GetOtherElements((WMBssDesc*)bssDesc);

        if( bssDesc != NULL)
        {
            for( i = 0; i < elem.count; i++)
            {
                if( elementID == elem.element[i].id)
                {
                    u8 elem_version = *(elem.element[i].body + 4);
                    // tag �� version �̃`�F�b�N
                    if ( !memcmp( elem.element[i].body, tag, 4) && elem_version == version)
                    {
                        *bufPtr = elem.element[i].body + 4;
                        result = (int)(elem.element[i].length);
                        break;
                    }
                }
            }
        }
    }
    
    return result;
}