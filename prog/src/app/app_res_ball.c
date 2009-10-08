//======================================================================
/**
 * @file	app_res_ball.c
 * @brief	���ʑf�ފ֌W�\�[�X
 * @author	genya_hosaka
 * @data	09/10/07
 *
 * ���W���[�����FAPP_RES_BALL
 */
//======================================================================


#include <gflib.h>

#include "item/item.h"
#include "app/app_res_ball.h"

#include "arc_def.h"
#include "app_ball.naix"



//=============================================================================
/**
 *	�{�[���A�C�R��
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  ARCID�̎擾
 *
 *	@param	void 
 *
 *	@retval ARCID �A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
const u32 APP_RES_BALL_GetArcId( void )
{
  return ARCID_APP_BALL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g���\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_RES_BALL_GetBallPltArcIdx( const BALL_ID ball )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_ball_ball00_NCLR + ball - 1;	//BALLID��1�I���W��
}
//----------------------------------------------------------------------------
/**
 *	@brief	�L�������\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_RES_BALL_GetBallCharArcIdx( const BALL_ID ball )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_ball_ball00_NCGR + ball - 1;	//BALLID�͂P�I���W��
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�����\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *	@param	APP_RES_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_RES_BALL_GetBallCellArcIdx( const BALL_ID ball, const APP_RES_MAPPING mapping )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_ball_ball_32k_NCER + mapping;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j�����\�[�X�ԍ��擾
 *
 *	@param	const BALL_ID ball					�{�[��ID
 *	@param	APP_RES_MAPPING mapping	�}�b�s���O���[�h
 *
 *	@return	�p���b�g�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 APP_RES_BALL_GetBallAnimeArcIdx( const BALL_ID ball, const APP_RES_MAPPING mapping )
{	
	GF_ASSERT_MSG( ball != BALLID_NULL, "�{�[��ID��NULL�ł�\n" );
	return NARC_app_ball_ball_32k_NANR + mapping;
}

