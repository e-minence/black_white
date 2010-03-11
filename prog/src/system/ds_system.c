//======================================================================
/**
 * @file	ds_system.c
 * @brief	DS�̐ݒ蓙�̎擾���b�p�[
 * @author	ariizumi
 * @data	20/01/23
 *
 * ���W���[�����FDS_SYSTEM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/ds_system.h"
#include "debug/debug_flg.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//--------------------------------------------------------------
//	@berif DSi�ŋN�����Ă��邩�H
//
//  @return BOOL TRUE DSi
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRunOnTwl( void )
{
  return OS_IsRunOnTwl();
}


//--------------------------------------------------------------
//	@berif �y�A�����^���R���g���[���Ŏʐ^�̑���M�����ۂ��Ă��邩�H
//
//  @return BOOL TRUE ����M����
//  @return BOOL FALSE ����M�� or DS
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRestrictPhotoExchange( void )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_RestricPhoto ) == TRUE )
  {
    return TRUE;
  }
  
#if (defined(SDK_TWL))
  if( DS_SYSTEM_IsRunOnTwl() )
  { 
    return OS_IsRestrictPhotoExchange();
  }
#endif
  return FALSE;
}

//--------------------------------------------------------------
//	@berif �y�A�����^���R���g���[���Ń��[�U�[�쐬�R���e���c��
//         ����M�����ۂ��Ă��邩�H
//
//  @return BOOL TRUE ����M����
//  @return BOOL FALSE ����M�� or DS
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRestrictUGC( void )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_RestricUGC ) == TRUE )
  {
    return TRUE;
  }

#if (defined(SDK_TWL))
  if( DS_SYSTEM_IsRunOnTwl() )
  { 
    return OS_IsRestrictUGC();
  }
#endif
  return FALSE;
}


//--------------------------------------------------------------
//	@berif DS�̖����ʐM�ݒ�̎擾
//
//  @return BOOL TRUE �ʐM���� or DS
//  @return BOOL FALSE �ʐM�s��
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsAvailableWireless( void )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_DisableWL ) == TRUE )
  {
    return FALSE;
  }

#if (defined(SDK_TWL))
  return OS_IsAvailableWireless();
#else
  return TRUE;
#endif
}

//--------------------------------------------------------------
//	@berif Wifi�̎g�p�K�񓯈Ӄ`�F�b�N
//         ������TRUE�ł�Wifi�ڑ����̃`�F�b�N�E�o�[�W�����X�V�ȂǂłȂ��Ȃ���������B
//
//  @return BOOL TRUE ���Ӎς�
//  @return BOOL FALSE ���ӂ��ĂȂ�
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsAgreeEULA( void )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_DisableEULA ) == TRUE )
  {
    return FALSE;
  }

#if (defined(SDK_TWL))
  if( DS_SYSTEM_IsRunOnTwl() == TRUE )
  {
    return OS_IsAgreeEULA();
  }
  else
  {
    //�{������DS�̏ꍇFALSE��Ԃ����A���֐��̂���TRUE
    return TRUE;
  }
#else
  return TRUE;
#endif
}

//--------------------------------------------------------------
//	@berif DS�̒a�����ݒ�̎擾
//
//	@param  month ���i�[��
//	@param  day   ���i�[��
//--------------------------------------------------------------
void DS_SYSTEM_GetBirthDay( u8* month, u8* day )
{
  OSOwnerInfo info;
  OS_GetOwnerInfo( &info );
  *month   = info.birthday.month;
  *day     = info.birthday.day;
}



