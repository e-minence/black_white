//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwc_error.c
 *	@brief  DWC�ėp�G���[�`�F�b�N���m�֐�
 *	@author	Toru=Nagihashi
 *	@data		2010.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include <dwc.h>

//�l�b�g���[�N
#include "system/net_err.h"
#include "net/dwc_rap.h"

//�O�����J
#include "net/dwc_error.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  DWC�ŃG���[���`�F�b�N���G���[�̌y�x�ɂ���ĕ\����؂蕪����
 *	        �y���G���[�Ȃ�΁A���̏�ŕ\��
 *	        �d���G���[�Ȃ�΁A�v���Z�X�ړ����ɕ\��
 *	        Fatal�Ȃ�΁A���̏�ŕ\�����������[�v�ɂȂ�
 *
 *	@param	TRUE�Ȃ�Όy�x���ʂ���@FALSE�Ȃ�Όy�x���d�x�Ƃ��ĕԂ�
 *
 *	@return TRUE�G���[�������\��  FALSE�Ȃ�΃G���[�ł͂Ȃ�
 */
//-----------------------------------------------------------------------------
GFL_NET_DWC_ERROR_RESULT GFL_NET_DWC_ERROR_ReqErrorDisp( BOOL is_heavy )
{ 
  if( GFL_NET_IsInit() )
  { 
    if( NetErr_App_CheckError() )
    { 
      const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

      switch( cp_error->errorType )
      { 
      case DWC_ETYPE_LIGHT:
        /* fallthru */
      case DWC_ETYPE_SHOW_ERROR:
        //�G���[�R�[�hor���b�Z�[�W��\�����邾��
        if( !is_heavy )
        { 
          NetErr_DispCallPushPop();
          GFL_NET_StateClearWifiError();
          return GFL_NET_DWC_ERROR_RESULT_PRINT_MSG;
        }
        /* fallthru */

      case DWC_ETYPE_SHUTDOWN_FM:
      case DWC_ETYPE_SHUTDOWN_GHTTP:
      case DWC_ETYPE_SHUTDOWN_ND:
        //�V���b�g�_�E��
        NetErr_App_ReqErrorDisp();
        return GFL_NET_DWC_ERROR_RESULT_RETURN_PROC;

      case DWC_ETYPE_DISCONNECT:
        //�ؒf
        NetErr_App_ReqErrorDisp();
        return GFL_NET_DWC_ERROR_RESULT_RETURN_PROC;

      case DWC_ETYPE_FATAL:
        //Fatal
        NetErr_DispCallFatal();
        return GFL_NET_DWC_ERROR_RESULT_FATAL;
      }
    }
  }

  return GFL_NET_DWC_ERROR_RESULT_NONE;
}
