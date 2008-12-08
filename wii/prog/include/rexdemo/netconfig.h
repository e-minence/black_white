/*---------------------------------------------------------------------------*
  Project:  NET Initialize demo
  File:	 netconfig.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: netconfig.h,v $
  Revision 1.12  2007/09/07 02:13:31  seiki_masashi
  REXDEMOSafeSOFinish �֐��𕪗�

  Revision 1.11  2007/09/07 00:41:38  seiki_masashi
  �l�b�g���[�N�I�������̈��S��������

  Revision 1.10  2007/06/23 12:21:28  seiki_masashi
  Added REXDEMOCreateMEM1Heap, and so on.

  Revision 1.9  2007/05/28 08:37:00  hirose_kazuki
  Added REXDEMODestroyHeapForSocket().

  Revision 1.8  2007/01/29 07:52:19  terui
  SO ���C�u�����̎�v�֐�����J���ɔ�����v�֐���p����֐��錾�� netfunc.h �Ɉړ�

  Revision 1.7  2006/09/25 07:06:13  seiki_masashi
  configId 0 �̏ꍇ�� NAND ��̃l�b�g���[�N�ݒ���g�p����悤�ɕύX
  peerName ���R�}���h���C������������擾����悤�ɕύX

  Revision 1.6  2006/08/29 07:19:20  adachi_hiroaki
  �v���t�B�b�N�X�ύX�A���̂ق�����

  Revision 1.5  2006/08/25 10:22:54  adachi_hiroaki
  REXDEMOWaitForInterfaceUp() �̖��O�Ǝd�l��ύX

  Revision 1.4  2006/08/25 09:33:01  adachi_hiroaki
  �e��u���b�N�֐��ɒ��f�t���O��ǉ�

  Revision 1.3  2006/08/25 02:14:48  adachi_hiroaki
  ���[�e�B���e�B�֐��Q��ǉ�

  Revision 1.2  2006/08/21 10:57:43  adachi_hiroaki
  NCD �̏����҂�������ǉ�

  Revision 1.1  2006/08/10 12:09:06  adachi_hiroaki
  �w�b�_�t�@�C���̈ʒu��ύX

  Revision 1.1  2006/08/09 08:46:58  adachi_hiroaki
  �l�b�g���[�N���ʐݒ�R�[�h���b��ǉ�

  Revision 1.1  2006/08/08 10:47:28  adachi_hiroaki
  �l�b�g���[�N�������̃f���� tests/so ����ړ�


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __DEMOS_SHARED_NETCONFIG_H__
#define __DEMOS_SHARED_NETCONFIG_H__

#include <revolution/types.h>
#include <revolution/ncd.h>
#include <revolution/so.h>

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================*/

#include "rexdemo/heap.h"

#define REXDEMO_OVERRIDE_NETCONFIG

#define REXDEMO_NETWORK_TERMINATION_RETRY_LIMIT 120
#define REXDEMO_NETWORK_TERMINATION_RETRY_WAIT  500 // milliseconds


BOOL	REXDEMOStartNetwork( BOOL override );
BOOL	REXDEMOEndNetwork( void );

BOOL	REXDEMOSafeSOFinish( void );


void	REXDEMOParseArgument( int argc, const char* argv[] );

s32	 REXDEMOOverrideNetConfigAuto( void );
s32	 REXDEMOOverrideNetConfig( const NCDIfConfig* pIfConfig, const NCDIpConfig* pIpConfig );
void	REXDEMOWaitForNCDReady( void );
s32	 REXDEMOWaitForInterfaceReady( u32 msecTimeout );
s32	 REXDEMOWaitForInterfaceReadyEx( u32 msecTimeout, BOOL wantUp, volatile BOOL* flagCancel );

void	REXDEMOSetNetConfigId( u8 configId );
u8	  REXDEMOGetNetConfigId( void );
void	REXDEMOSetPeerName( const char* peerName );
const char* REXDEMOGetPeerName( void );

BOOL	REXDEMOCreateIfConfig( NCDIfConfig* pIfConfig, u8 configIdSrc );
BOOL	REXDEMOCreateIpConfig( NCDIpConfig* pIpConfig, u8 configIdSrc );


/*---------------------------------------------------------------------------*/
static inline void
REXDEMOSetInAddr( u8* addr, u8 para0, u8 para1, u8 para2, u8 para3 )
{
	addr[ 0 ]   = para0;
	addr[ 1 ]   = para1;
	addr[ 2 ]   = para2;
	addr[ 3 ]   = para3;
}

/*===========================================================================*/
#ifdef __cplusplus
}
#endif
#endif  /* __DEMOS_SHARED_NETCONFIG_H__ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
