/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - libraries
  File:     socl_misc.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-06#$
  $Rev: 1087 $
  $Author: matsuoka_masayoshi $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>

struct errortable
{
    int code;
    const char*     name;
};

static struct errortable    errtable[] =
{
#include "errtable.h"
};

SOCLResource SOCLi_Resource;

/*---------------------------------------------------------------------------*
  Name:         SOCL_GetErrName

  Description:  SOCL_EXXXX �̃G���[�R�[�h�����擾����

  Arguments:    �G���[�R�[�h
  
  Returns:      �G���[�R�[�h�� Enum ��
 *---------------------------------------------------------------------------*/
const char* SOCL_GetErrName(int errcode)
{
    int i;

    for (i = 0; i < sizeof(errtable) / sizeof(errtable[0]); i++)
    {
        if (errtable[i].code == errcode)
        {
            return errtable[i].name;
        }
    }

    return "";
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ROUNDUP4/SOCLi_RoundUp4
  
  Description:  4 �̔{���ɐ؂�グ��
                �R�[�h�T�C�Y�����炷���߂Ƀ}�N���Ɗ֐���2��p�ӂ��Ă���
                size ���萔�̏ꍇ�̓}�N���̕����g���R���p�C���̍œK������
                �҂�������悢
  
  Arguments:    size     �؂�グ�����l
  
  Returns:      size �� 4 �ɐ؂�グ����
 *---------------------------------------------------------------------------*/
u32 SOCLi_RoundUp4(u32 size)
{
    return SOCLi_ROUNDUP4(size);
}

void SOCL_GetResource(SOCLResource* resource)
{
    *resource = SOCLi_Resource;
}

void SOCLi_InitResource(void)
{
    MI_CpuClear8(&SOCLi_Resource, sizeof(SOCLi_Resource));
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CheckNConfig
  
  Description:  �z�X�gIP�A�h���X�A�T�u�l�b�g�}�X�N�A�Q�[�g�E�F�C�A�h���X
                DNS�T�[�o�A�h���X�ɕs�����Ȃ����m�F����B

  Arguments:    �Ȃ�(CPS�̃O���[�o���ϐ��Œ�`�����A�h���X���`�F�b�N����)
  
  Returns:      1�F�s���͂Ȃ�
                0�F�s����������
 *---------------------------------------------------------------------------*/

static int SOCLi_IsIPv4ValidDNSAddr( u32 netmask, u32 dns, u32 addr )
{
    return  (
                ( 0xffffffff != dns )                       /* �f�B���N�e�B�b�h�u���[�h�L���X�g*/
            &&  ( 0 != dns )                                /* 0 */
            &&  ( addr != dns )                             /* �z�X�g�A�h���X�Ɠ��� */
            &&  ( ((addr^dns)&netmask) || (dns&~netmask) )  /* �l�b�g���[�N�A�h���X */
            &&  ( ((addr^dns)&netmask) || ((dns&~netmask)^~netmask))/* ���~�e�b�h�u���[�h�L���X�g */
            &&  ( dns&0xff000000^0x7f000000)                /* ���[�v�o�b�N 127.*.*.* */
            &&  ( dns&0xf0000000^0xe0000000));              /* �}���`�L���X�g�A�h���X */
}

static int SOCLi_IsIPv4ValidNodeAddr( u32 netmask, u32 addr )
{
    return (
//              ( netmask == 0xffffffff )  ||               /* �P��z�X�g */
                (   (addr&~netmask^~netmask)                /* �u���[�h�L���X�g */
//              &&  ( netmask != 0xfffffffe )               /* Ponit to Point */ 
                &&  (addr&~netmask)                         /* �l�b�g���[�N���̂��� */
                &&  (addr&0xff000000^0x7f000000)            /* ���[�v�o�b�N 127.*.*.* */
                &&  (addr&0xf0000000^0xe0000000))           /* �}���`�L���X�g�A�h���X */
           );
}

int SOCLi_CheckNConfig( void )
{
	if( ( 0x00000000 == CPSNetMask) || (( ~CPSNetMask & ( ~CPSNetMask + 1 ) ) != 0 ) )
	{
		return 0;
	}

	if( ! SOCLi_IsIPv4ValidNodeAddr( CPSNetMask, CPSMyIp ) )
	{
		return 0;
	}

	if (    ( 0 != CPSGatewayIp )
        &&  ( ! SOCLi_IsIPv4ValidNodeAddr( CPSNetMask, CPSGatewayIp )
        ||  ( ( CPSNetMask != 0xffffffff ) && ( ( CPSMyIp & CPSNetMask ) != ( CPSGatewayIp & CPSNetMask ) ) )
        ||  ( CPSMyIp == CPSGatewayIp ) ) )
	{
		CPSGatewayIp = 0;
	}

    if( !SOCLi_IsIPv4ValidDNSAddr( CPSNetMask, CPSDnsIp[0], CPSMyIp ) )
    {
        CPSDnsIp[0] = 0;
    }

    if( !SOCLi_IsIPv4ValidDNSAddr( CPSNetMask, CPSDnsIp[1], CPSMyIp ) )
    {
        CPSDnsIp[1] = 0;
    }
	return 1;
}
