//=============================================================================
/**
 * @file	  app_res_ball.h
 * @brief	  �{�[�����\�[�X �A�N�Z�T
 * @author  genya_hosaka
 * @data	  09/10/07
 *
 * @note    2*2�L�����̃A�C�R���ł��B�傫�����̓A�C�e���A�C�R������擾���Ă�������
 *
 * ���W���[�����FAPP_RES_BALL
 */
//=============================================================================
#pragma once

#include "app_res_def.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
//�{�[���A�C�R���̃p���b�g�̖{��
#define APP_RES_BALL_PLT_NUM (1)

//=============================================================================
/**
 *							  EXTERN
 */
//=============================================================================
extern const u32 APP_RES_BALL_GetArcId( void );
extern u32 APP_RES_BALL_GetBallPltArcIdx( const BALL_ID ball );
extern u32 APP_RES_BALL_GetBallCharArcIdx( const BALL_ID ball );
extern u32 APP_RES_BALL_GetBallCellArcIdx( const BALL_ID ball, const APP_RES_MAPPING mapping );
extern u32 APP_RES_BALL_GetBallAnimeArcIdx( const BALL_ID ball, const APP_RES_MAPPING mapping );

