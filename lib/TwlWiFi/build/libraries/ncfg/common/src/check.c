/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NCFG - libraries
  File:     check.c

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
#include <nitroWiFi/ncfg.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// �@�\ : �A�N�Z�X�|�C���g��񂪐��������`�F�b�N
// ���� : info  - �A�N�Z�X�|�C���g�ڑ����
// �Ԓl : BOOL  - TRUE  : �� / FALSE : �s��
//-----------------------------------------------------------------------------
BOOL NCFGi_CheckApInfo(const NCFGApInfo* info)
{
	// �ݒ���@
	if (info->setType == NCFG_SETTYPE_NOT     )  return  TRUE;
	if (info->setType >  NCFG_SETTYPE_RAKURAKU)  return  FALSE;
	// SSID
	if (! NCFGi_CheckSsid(info->ssid[0]))  return  FALSE;
	// IP �A�h���X
	if (! NCFGi_CheckIpInfo(info))  return  FALSE;

	return  TRUE;
}

//-----------------------------------------------------------------------------
// �@�\ : �g���A�N�Z�X�|�C���g��񂪐��������`�F�b�N
// ���� : info  - �A�N�Z�X�|�C���g�ڑ����
//        infoEx - �g���A�N�Z�X�|�C���g�ڑ����
// �Ԓl : BOOL  - TRUE  : �� / FALSE : �s��
//-----------------------------------------------------------------------------
BOOL NCFGi_CheckApInfoEx(const NCFGApInfo* info, const NCFGApInfoEx* infoEx)
{
    // �ݒ���@
    if (info->setType == NCFG_SETTYPE_NOT)
    {
        return  TRUE;
    }
    if (info->setType <= NCFG_SETTYPE_RAKURAKU)
    {
        // �݊��ݒ�Ɠ����`��
        return NCFGi_CheckApInfo(info);
    }

//    if (info->setType != NCFG_SETTYPE_WPS_WEP &&
//          (info->setType < NCFG_SETTYPE_MANUAL_WPA
//         || NCFG_SETTYPE_WPS_WPA < info->setType))
//    {
//        return FALSE;
//    }
    // SSID
    if (! NCFGi_CheckSsid(info->ssid[0]))
    {
        return FALSE;
    }
    // IP �A�h���X
    if (! NCFGi_CheckIpInfo(info))
    {
        return FALSE;
    }

    // proxy
    if (infoEx->proxyMode > NCFG_PROXY_MODE_NORMAL)
    {
        return FALSE;
    }
    if (infoEx->proxyAuthType > NCFG_PROXY_AUTHTYPE_BASIC)
    {
        return FALSE;
    }

    return  TRUE;
}

static inline isNullAddress(const u8* addr)
{
    return (addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[3] == 0);
}

BOOL NCFGi_CheckIpInfo(const NCFGApInfo* info)
{
	u8   snm[4];

	// IP �A�h���X
	if (! isNullAddress(info->ip)) {
		// �f�t�H���g�Q�[�g�E�F�C
		if (! NCFGi_CheckAddress(info->gateway))  return  FALSE;
		// �T�u�l�b�g�}�X�N
		if (info->netmask > 32)  return  FALSE;
		// IP �A�h���X
		NCFGi_ConvMaskAddr(info->netmask, snm);
		if (! NCFGi_CheckIp(info->ip, snm))  return  FALSE;
	}
	// DNS
	if (! isNullAddress(info->dns[0])) {
		if (   (! NCFGi_CheckAddress(info->dns[0]))
			&& (! NCFGi_CheckAddress(info->dns[1])))
		{
			return  FALSE;
		}
	}
	return TRUE;
}

//*****************************************************************************
// �@�\ : �T�u�l�b�g�}�X�N�� CIDR �ɕϊ�
// ���� : mask  - �T�u�l�b�g�}�X�N (�A�h���X)
// �Ԓl : u8    - �T�u�l�b�g�}�X�N (CIDR)
//*****************************************************************************
u8  NCFGi_ConvMaskCidr(const u8* mask)
{
	int  snm;
	int  i, j;

	for (i = 0, snm = 0; i < 4; ++ i) {
		for (j = 0; j < 8; ++ j) {
			if ((mask[i] >> j) & 1)  ++ snm;
		}
	}
	return  (u8)snm;
}

//*****************************************************************************
// �@�\ : �T�u�l�b�g�}�X�N���A�h���X�ɕϊ�
// ���� : mask  - �T�u�l�b�g�}�X�N (CIDR)
//        buf   - �T�u�l�b�g�}�X�N (�A�h���X) �i�[�o�b�t�@  <<-- �Ԓl
// �Ԓl :
//*****************************************************************************
void  NCFGi_ConvMaskAddr(int mask, u8* buf)
{
	int  i;
	u32  snm = (0xFFFFFFFF >> mask) ^ 0xFFFFFFFF;

	for (i = 0; i < 4; ++ i) {
		buf[i] = (u8)(snm >> (24 - (8 * i)));
	}
}

//*****************************************************************************
// �@�\ : SSID ���`�F�b�N
// ���� : ssid  - SSID
// �Ԓl : BOOL  - TRUE  : �� / FALSE : �s��
//*****************************************************************************
BOOL  NCFGi_CheckSsid(const u8* ssid)
{
	int  i;

	for (i = 0; i < 0x20; ++ i) {
		if (ssid[i])  return  TRUE;
	}
	return  FALSE;
}

//*****************************************************************************
// �@�\ : IP �A�h���X���`�F�b�N
// ���� : ip    - �A�h���X
//        mask  - �T�u�l�b�g�}�X�N
// �Ԓl : BOOL  - TRUE  : �� / FALSE : �s��
//*****************************************************************************
BOOL  NCFGi_CheckIp(const u8* ip, const u8* mask)
{
	u32  ipa;
	u32  snm;

	if (! NCFGi_CheckAddress(ip))  return  FALSE;
	MI_CpuCopy8(ip  , &ipa, sizeof(u32));
	MI_CpuCopy8(mask, &snm, sizeof(u32));
	if ((ipa |   snm ) == ~1)  return  FALSE;
	if ((ipa & (~snm)) ==  0)  return  FALSE;
	return  TRUE;
}

//*****************************************************************************
// �@�\ : �A�h���X���`�F�b�N
// ���� : address  - �A�h���X
// �Ԓl : BOOL     - TRUE  : �� / FALSE : �s��
//*****************************************************************************
BOOL  NCFGi_CheckAddress(const u8* address)
{
	if (address[0] == 127)  return  FALSE;
	if (address[0] <    1)  return  FALSE;
	if (address[0] >  223)  return  FALSE;
	return  TRUE;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
