/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - demos - httpclient
  File:     ssl.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef HTTPCLIENT_SSL_H_
#define HTTPCLIENT_SSL_H_

#ifdef __cplusplus

extern "C" {
#endif

#include <nitroWiFi.h>

/*===========================================================================*/

/*---------------------------------------------------------------------------*
  �萔��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  �\���̒�`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  �֐��v���g�^�C�v��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         SslInit

  Description:  SSL ������.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SslInit(void);

/*---------------------------------------------------------------------------*
  Name:         SslSetDefaultConncetion

  Description:  �f�t�H���g��SOCLSslConnection�\���̂�o�^����.

  Arguments:    psslCon		�o�^����SOCLSslConnection�ւ̃|�C���^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SslSetDefaultConncetion(SOCSslConnection* psslCon);

/*---------------------------------------------------------------------------*
  Name:         SslGetDefaultConnection

  Description:  ssl.c�ɓo�^����Ă���f�t�H���g��SOCLSslConnection�ւ̃|�C���^��Ԃ�.

  Arguments:    None.

  Returns:      ssl.c�ɓo�^����Ă���SOCLSslConnection�ւ̃|�C���^.
                InitSSL�O��NULL��Ԃ�.
 *---------------------------------------------------------------------------*/
SOCLSslConnection*  SslGetDefaultConnection();

/*---------------------------------------------------------------------------*
  Name:         SslSetDefaultAuthCallback

  Description:  SSL�F�ؗp�̃R�[���o�b�N��ݒ肷��.

  Arguments:    auth_callback	�R�[���o�b�N�֐��ւ̃|�C���^.

  Returns:		None.
 *---------------------------------------------------------------------------*/
void    SslSetDefaultAuthCallback(int (*auth_callback) (int, struct _CPSSslConnection*, int));

/*---------------------------------------------------------------------------*
  Name:         SslAuthCallback

  Description:  SSL �T�[�o�F�؃R�[���o�b�N.

  Arguments:    con           SSL�ڑ����
                level         ���x��

  Returns:      None.
 *---------------------------------------------------------------------------*/
int     SslAuthCallback(int result, SOCSslConnection* con, int level);

/*---------------------------------------------------------------------------*
  Name:         SslSetDefaultCA

  Description:  SSL�̏ؖ�����ݒ肷��.

  Arguments:    ca_list		�ؖ������X�g
                builtins.	�ؖ����̐�

  Returns:		None.
 *---------------------------------------------------------------------------*/
void    SslSetDefaultCA(SOCCaInfo ** ca_list, int builtins);

/*---------------------------------------------------------------------------*
  Name:         SslCreateConnection

  Description:  SSL�̏ؖ�����ݒ肷��.

  Arguments:    servername	CN�Ƃ̃}�b�`������ڑ���̃T�[�o��

  Returns:		�V�����m�ۂ��ꂽSslCreateConnection�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
SOCLSslConnection*  SslCreateConnection(const char* servername);

/*---------------------------------------------------------------------------*
  Name:         SslDestroyConnection

  Description:  �\�P�b�g�Ɋ֘A�t����ꂽSslCreateConnection�\���̂�j������.

  Arguments:    socket		SSL���L���ɂȂ��Ă���\�P�b�g�f�B�X�N���v�^

  Returns:		�V�����m�ۂ��ꂽSslCreateConnection�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
void    SslDestroyConnection(SOCSslConnection* con);

void    SslPrintCA(const SOCSslConnection* con);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif /* __cplusplus */
#endif /* HTTPCLIENT_SSL_H_ */
