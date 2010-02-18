//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dpw_rap.c
 *	@brief  DPW���C�u���������ʂŎg�����߂̃\�[�X
 *	@author	Toru=Nagihashi
 *	@data		2010.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include <dpw_tr.h>
#include "print/str_tool.h"

//�Z�[�u�f�[�^
#include "savedata/mystatus.h"

//�O�����J
#include "net/dpw_rap.h"
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
 *	@brief  Dpw_Common_Profile���쐬
 *
 *	@param	*p_dc_profile         �쐬����PROFILE�󂯎��
 *	@param	MYSTATUS *p_mystatus  PROFILE�쐬�̂��߂ɕK�v�ȃf�[�^
 */
//-----------------------------------------------------------------------------
void DPW_RAP_CreateProfile( Dpw_Common_Profile *p_dc_profile, const MYSTATUS *cp_mystatus )
{ 
  GFL_STD_MemClear( p_dc_profile, sizeof(Dpw_Common_Profile));

  p_dc_profile->version     = PM_VERSION;
  p_dc_profile->language    = PM_LANG;
  p_dc_profile->countryCode = MyStatus_GetMyNation(cp_mystatus);
  p_dc_profile->localCode   = MyStatus_GetMyArea(cp_mystatus);
  p_dc_profile->playerId    = MyStatus_GetID(cp_mystatus);

  STRTOOL_Copy( MyStatus_GetMyName(cp_mystatus), p_dc_profile->playerName, DPW_TR_NAME_SIZE );

  p_dc_profile->flag = 0;   //�n���O��������\���ł��邩 �ł���Ȃ�DPW_PROFILE_FLAG_HANGEUL_AVAILABLE

  //p_dc_profile->macAddr   ���C�u�������Ŋi�[����̂ŃZ�b�g�̕K�v�Ȃ�

  //�ȉ����[��
  p_dc_profile->mailAddr[0]   = '\0'; //�V���`�ł͎g�p���Ȃ�
  p_dc_profile->mailRecvFlag  = 0;  //���[����M����Ȃ�DPW_PROFILE_MAILRECVFLAG_EXCHANGE
}
