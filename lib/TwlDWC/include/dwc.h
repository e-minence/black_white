/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_H_
#define DWC_H_

// �g�p���Ă���DWC
#define DWC_TWL

// �o�[�W�������
#include <dwc_version.h>

// �v���b�g�t�H�[���ˑ��R�[�h
#include <nonport/dwc_nonport.h>

// ���ʃ��W���[��
#include <common/dwc_base64.h>
#include <common/dwc_error.h>
#include <common/dwc_init.h>
#include <common/dwc_memfunc.h>
#include <common/dwc_report.h>
#include <common/dwc_ssl.h>

// �F�؃T�[�o�֘A
#include <auth/dwc_nas.h>

// �T�[�r�X���P�[�^
#include <auth/dwc_svl.h>

// �s��������`�F�b�N
#include <auth/dwc_prof.h>

// �����ڑ��֘A
#include <ac/dwc_connectinet.h>

// WiFi�ݒ�֘A
#include <util/dwc_utility.h>

// �_�E�����[�h�֘A
#include <nd/dwc_nd.h>

#ifndef TWLDWC_NOGS
// �A�J�E���g�֘A
#include <account/dwc_account.h>

// �}�b�`���C�N�֘A
#include <match/dwc_core.h>

// �����L���O�֘A
#include <ranking/dwc_ranking.h>
#endif

#endif // DWC_H_
