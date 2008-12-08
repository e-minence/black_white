/*---------------------------------------------------------------------------*
  Project:  RevoEX - demos - share
  File:	 init.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#include <revolution/os.h>
#include <revolution/ncd.h>
#include <revolution/so.h>
#include <revolution/net.h>
#include <string.h>
#include <stdlib.h>
#include "rexdemo/netconfig.h"
#include "rexdemo/graphic.h"

/*---------------------------------------------------------------------------*
	�ϐ���`
 *---------------------------------------------------------------------------*/

static u8 NetConfigId = 0;
static char PeerName[256];

/*---------------------------------------------------------------------------*
	�����֐��v���g�^�C�v�錾
 *---------------------------------------------------------------------------*/

BOOL
REXDEMOStartNetwork(BOOL override)
{
	s32 result = 0;
	
	/* NAND �ɕۑ�����Ă���l�b�g���[�N�ݒ���㏑�����邩�H */
	if (override)
	{
		/* ������͌��ʂɏ]���ăl�b�g���[�N�ݒ���㏑������(�f�o�b�O��p) */
		result = REXDEMOOverrideNetConfigAuto();
		if (result < NCD_RESULT_SUCCESS)
		{
			REXDEMOError("REXDEMOOverrideNetConfigAuto() failed.(%d)", result);
			return FALSE;
		}
	}

	/* �\�P�b�g���C�u�����ɗ^����q�[�v���쐬���� */
	if (!REXDEMOCreateHeapForSocket( REXDEMO_SOCKET_HEAPSIZE_DEFAULT ))
	{
		REXDEMOError("REXDEMOCreateHeapForSocket() failed.");
		return FALSE;
	}
	/* REXDEMO ���̃A���P�[�^���w�肵�ă\�P�b�g���C�u���������������� */
	result = SOInit( REXDEMOGetSOLibConfig() );
	if (result == SO_EALREADY)
	{
		REXDEMOError("SOInit() already done. (%d)", result);
	}
	else if (result < SO_SUCCESS)
	{
		REXDEMOError("SOInit() failed.(%d)", result);
		return FALSE;
	}
	
	/* �\�P�b�g���C�u�����̃l�b�g���[�N�@�\���J�n���� */
	result = SOStartup();
	if (result == SO_EALREADY)
	{
		REXDEMOError("SOStartup() already done. (%d)", result);
	}
	else if (result < SO_SUCCESS)
	{
		REXDEMOError("SOStartup() failed.(%d)", result);
		REXDEMOError("Network Error Code is %d", -NETGetStartupErrorCode(result));
		return FALSE;
	}
	
	/* SOStartup �� DHCP �̉�����҂悤�ɂȂ�܂��� */
	
	return TRUE;
}

BOOL
REXDEMOSafeSOFinish( void )
{
	s32 iRetry;
	s32 result;
	BOOL succeeded = TRUE;

	for ( iRetry = REXDEMO_NETWORK_TERMINATION_RETRY_LIMIT; iRetry > 0; iRetry-- )
	{
		result = SOFinish();
		if ( result == SO_EBUSY || result == SO_EAGAIN || result == SO_EINPROGRESS )
		{
			s32 r;
			REXDEMOError("SOFinish() failed.(%d)\n", result);

			/* SOFinish ���Ăяo���Ȃ��X�e�[�g�ɂ���̂ŁASOCleanup �����݂� */
			REXDEMOReport( "Try SOCleanup()...\n" );
			r = SOCleanup();
			if ( r < SO_SUCCESS )
			{
				REXDEMOError("SOCleanup() failed.(%d)\n", r);
			}
			/* ���̃v���Z�X�̃l�b�g���[�N�֘A�̃��\�[�X�̊J����҂� */
			OSSleepMilliseconds( REXDEMO_NETWORK_TERMINATION_RETRY_WAIT );
		}
		else
		{
			break;
		}
	}
	if (result == SO_EALREADY)
	{
		/* SOFinish �����łɌĂяo����Ă��邩�A�������� SOInit ���Ăяo����Ă��Ȃ� */
		/* SOFinish �Ăяo����Ɠ�����Ԃł͂���̂ŁA�Ԃ�l�� TRUE */
		REXDEMOError("SOFinish() already done. (%d)", result);
	}
	else if (result < SO_SUCCESS)
	{
		/* �������Ȃ��͂��̑z��O�� SOFinish �̃G���[ */
		/* �ُ�n�������K�v */
		REXDEMOError("SOFinish() failed.(%d)", result);
		succeeded = FALSE;
	}

	if ( iRetry <= 0 )
	{
		/* �Ď��s����񐔂Ɉ����������� */
		/* �l�b�g���[�N�̏I���������K�؂ɍs���Ă��Ȃ��̂ŁA�ُ�n�������K�v */
		REXDEMOError("Too many network termination retries; give up to call SOFinish().\n");
		succeeded = FALSE;
	}

	return succeeded;
}

BOOL
REXDEMOEndNetwork( void )
{
	s32 result = 0;
	BOOL succeeded = TRUE;
	
	/* �\�P�b�g���C�u�����̃l�b�g���[�N�@�\���~���� */
	result = SOCleanup();
	if (result == SO_EALREADY)
	{
		REXDEMOError("SOCleanup() already done. (%d)", result);
	}
	else if (result < SO_SUCCESS)
	{
		REXDEMOError("SOCleanup() failed.(%d)", result);
		succeeded = FALSE;
	}
	
	/* �\�P�b�g���C�u�����̃��\�[�X���J������ */
	if (REXDEMOSafeSOFinish() == FALSE)
	{
		succeeded = FALSE;
	}
	
	/* �\�P�b�g���C�u�����ɗ^����q�[�v����� */
	REXDEMODestroyHeapForSocket();
	
	return succeeded;
}

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name		: REXDEMOSetNetConfigId
  Description : 
  Arguments   : configId	 - 
  Returns	 : None.
 *---------------------------------------------------------------------------*/
void
REXDEMOSetNetConfigId( u8 configId )
{
	NetConfigId = configId;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOGetNetConfigId
  Description : 
  Arguments   : None.
  Returns	 : u8		   - 
 *---------------------------------------------------------------------------*/
u8
REXDEMOGetNetConfigId( void )
{
	return NetConfigId;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOSetPeerName
  Description : 
  Arguments   : peerName - 
  Returns	 : None.
 *---------------------------------------------------------------------------*/
void
REXDEMOSetPeerName( const char* peerName )
{
	if( peerName != NULL )
	{
		(void)strncpy(PeerName, peerName, sizeof(PeerName)-1);
		PeerName[sizeof(PeerName)-1] = 0;
	}
	else
	{
		(void)memset(PeerName, 0, sizeof(PeerName));
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOGetPeerName
  Description : 
  Arguments   : None.
  Returns	 : const u8*	- 
 *---------------------------------------------------------------------------*/
const char*
REXDEMOGetPeerName( void )
{
	return PeerName;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOWaitForNCDReady
  Description : NCD ���v�����󂯕t�������ԂɂȂ�܂ő҂�
				���̊֐��Ŗ������[�v�ɂȂ�ꍇ�̓t�@�[���E�F�A���s���ł���
  Arguments   : None.
  Returns	 : None.
 *---------------------------------------------------------------------------*/
void
REXDEMOWaitForNCDReady( void )
{
	while(NCDGetLinkStatus() == NCD_RESULT_INPROGRESS)
	{
		OSSleepMilliseconds(100);
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOOverrideNetConfig
  Description : NCD �̃l�b�g���[�N�ݒ�������㏑������B
  Arguments   : pIfConfig	- 
				pIpConfig	- 
  Returns	 : s32		  - 
 *---------------------------------------------------------------------------*/
s32
REXDEMOOverrideNetConfig( const NCDIfConfig* pIfConfig, const NCDIpConfig* pIpConfig )
{
	s32 resultNcd;
	
	REXDEMOWaitForNCDReady();
	
	//////////////////////////////////////////////////////////////////////////
	// �� ���� ��
	// NCDSetIfConfig, NCDSetIpConfig �̓e�X�g�p�r�̊֐��ł��B
	// ���i�����[�X���ɂ́A�����̊֐����Ăяo�����A
	// SOStartup �������Ăяo�������ɂ��Ă��������B
	// ���̏ꍇ�A�{�̂ɐݒ肳�ꂽ�l�b�g���[�N�ݒ肪���p����܂��B
	// �J���@�ނ̃l�b�g���[�N�ݒ�́A�V�X�e�����j���[���A
	// RevoEX/RVL/bin/tools/ncdconfigtool.elf �ōs�����Ƃ��\�ł��B
	//////////////////////////////////////////////////////////////////////////
	
	if( pIfConfig != NULL )
	{
		//OSReport("NCDSetIfConfig...\n");
		resultNcd = NCDSetIfConfig( pIfConfig );
		if( resultNcd != NCD_RESULT_SUCCESS )
		{
			OSReport( "NCDSetIfConfig failed (%d)\n", resultNcd );
			return resultNcd;
		}
	}
	
	if( pIpConfig != NULL )
	{
		//OSReport("NCDSetIpConfig...\n");
		resultNcd   = NCDSetIpConfig( pIpConfig );
		if( resultNcd != NCD_RESULT_SUCCESS )
		{
			OSReport( "NCDSetIpConfig failed (%d)\n", resultNcd );
			return resultNcd;
		}
	}
	
	//OSReport("REXDEMOOverrideNetConfig ok\n");
	return NCD_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOOverrideNetConfigAuto
  Description : NCD �̃l�b�g���[�N�ݒ�� config.c �̃l�b�g���[�N�ݒ��
				�����㏑������B
				config.c �̐ݒ�� REXDEMOSetNetConfigId() �Ő؂�ւ����\�B
  Arguments   : None.
  Returns	 : s32		  - 
 *---------------------------------------------------------------------------*/
s32
REXDEMOOverrideNetConfigAuto( void )
{
	NCDIfConfig ifConfig;
	NCDIfConfig* pIfConfig;
	NCDIpConfig ipConfig;
	NCDIpConfig* pIpConfig;
	u8 configId;
	s32 result;

	configId = REXDEMOGetNetConfigId();
	pIfConfig = NULL;
	pIpConfig = NULL;
	if( REXDEMOCreateIfConfig( &ifConfig, configId ) )
	{
		pIfConfig = &ifConfig;
	}
	if( REXDEMOCreateIpConfig( &ipConfig, configId ) )
	{
		pIpConfig = &ipConfig;
	}

	result = REXDEMOOverrideNetConfig( pIfConfig, pIpConfig );
	if (result < NCD_RESULT_SUCCESS)
	{
		return result;
	}
	return result;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOWaitForInterfaceReady
  Description : NCD ���C���^�[�t�F�[�X����������܂ő҂�
  Arguments   : msecTimeout  - 
  Returns	 : s32		  - 
 *---------------------------------------------------------------------------*/
s32
REXDEMOWaitForInterfaceReady( u32 msecTimeout )
{
	return REXDEMOWaitForInterfaceReadyEx( msecTimeout, FALSE, NULL );
}

s32
REXDEMOWaitForInterfaceReadyEx( u32 msecTimeout, BOOL wantUp, volatile BOOL* flagCancel )
{
	u32 ms;
	s32 resultNcd;
	for ( ms = 0; msecTimeout == 0 || ms < msecTimeout; ms+=100 )
	{
		if (flagCancel && *flagCancel)
		{
			break;
		}
		switch( resultNcd = NCDGetLinkStatus() )
		{
		case NCD_RESULT_INPROGRESS:
		case NCD_LINKSTATUS_WORKING:
			/* �l�b�g���[�N I/F ���܂��g�p�ł��Ȃ���� */
			break;
		case NCD_LINKSTATUS_NONE:
			/* ��؂̒ʐM�������Ȃ��悤�ݒ肳��Ă��� */
			OSReport( "Not allowed to use Network functions\n" );
			return NCD_RESULT_FAILURE;

		case NCD_LINKSTATUS_WIRELESS_DOWN:
			if (wantUp)
			{
				break;
			}
		case NCD_LINKSTATUS_WIRED:
		case NCD_LINKSTATUS_WIRELESS_UP:
			return NCD_RESULT_SUCCESS;
		default:
			/* ���̑��̃G���[�͒ʏ�Ԃ���Ȃ� */
			OSReport( "NCDGetLinkStatus() failure (%d)\n", resultNcd );
			return resultNcd;
		}
		//OSReport("NCDGetLinkStatus() = %d\n", resultNcd);
		OSSleepTicks( OSMillisecondsToTicks( 100 ) );
	}
	return NCD_RESULT_UNDECIDED;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOParseArgument
  Description : 
  Arguments   : argc		 - 
  Returns	 : None.
 *---------------------------------------------------------------------------*/
void
REXDEMOParseArgument( int argc, const char* argv[] )
{
	int curr_arg = 1;

	/* configId */
	if( curr_arg < argc )
	{
		BOOL f;
		int i;
		int id;
		const char* arg = argv[curr_arg];

		f = FALSE;
		id = 0;
		for( i = 0; arg[i] != 0 && i < 256; i++ )
		{
			if( '0' <= arg[i] && arg[i] <= '9' )
			{
				f = TRUE;
				id *= 10;
				id += arg[i] - '0';
				if( id >= 256 )
				{
					/* configId �� 0�`255 */
					f = FALSE;
					break;
				}
			}
			else
			{
				f = FALSE;
				break;
			}
		}

		if( f )
		{
			NetConfigId = (u8)id;
			curr_arg++;
		}
		else
		{
			NetConfigId = 0;
		}
	}

	/* peerName */
	if( curr_arg < argc )
	{
		REXDEMOSetPeerName(argv[curr_arg]);
	}
	else
	{
		REXDEMOSetPeerName(NULL);
	}
}

/*---------------------------------------------------------------------------*
  $Log: netconfig.c,v $
  Revision 1.10  2007/09/07 02:13:31  seiki_masashi
  REXDEMOSafeSOFinish �֐��𕪗�

  Revision 1.9  2007/09/07 00:41:27  seiki_masashi
  �l�b�g���[�N�I�������̈��S��������

  Revision 1.8  2007/06/04 09:40:54  hirose_kazuki
  SO �� result �� SO_EALREADY �̏ꍇ����������ǉ�

  Revision 1.7  2007/06/04 05:29:05  seiki_masashi
  �G���[�������̏����̕s����C��

  Revision 1.6  2007/05/28 08:37:49  hirose_kazuki
  Added REXDEMODestroyHeapForSocket() call.

  Revision 1.5  2007/01/29 07:55:24  terui
  REXDEMOCreatePeerAddressAuto �֐��̎��̂� netfunc.c �Ɉړ�
  REXDEMOGetPeerAddress �֐��̎��̂� netfunc.c �Ɉړ�

  Revision 1.4  2006/09/25 07:06:13  seiki_masashi
  configId 0 �̏ꍇ�� NAND ��̃l�b�g���[�N�ݒ���g�p����悤�ɕύX
  peerName ���R�}���h���C������������擾����悤�ɕύX

  Revision 1.3  2006/09/25 02:07:07  seiki_masashi
  small fix

  Revision 1.2  2006/09/25 02:00:37  seiki_masashi
  SOStartup �̎d�l�ύX�ɒǐ�
  NCDSetIfConfig �ɑ΂��钍�ӎ����̃R�����g��ǉ�

  Revision 1.1  2006/08/29 07:19:20  adachi_hiroaki
  �v���t�B�b�N�X�ύX�A���̂ق�����

 *---------------------------------------------------------------------------*/
