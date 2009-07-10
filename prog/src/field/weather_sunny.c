//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_sunny.c
 *	@brief		�V�C�@����
 *	@author		tomoya takahashi
 *	@data		2009.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "weather_sunny.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�H�O�t�F�[�h�J�n�I�t�Z�b�g
//=====================================
#define FOG_START_OFFSET	( 0x7FFF )
#define FOG_FADE_SYNC			(160)
#define FOG_FADEOUT_SYNC	(80)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�N
//=====================================
typedef struct {
	BOOL fade_init;
} SUNNY_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 


WEATHER_TASK_DATA c_WEATHER_TASK_DATA_SUNNY = {
	//	�O���t�B�b�N���
	0,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	FALSE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
	0,			// BGTEX
	0,			// GXTexSizeS
	0,			// GXTexSizeT
	0,			// GXTexRepeat
	0,			// GXTexFlip
	0,			// GXTexFmt
	0,			// GXTexPlttColor0

	// ���[�N�T�C�Y
	sizeof(SUNNY_WORK),

	// �Ǘ��֐�
	WEATHER_SUNNY_Init,		// ������
	WEATHER_SUNNY_FadeIn,		// �t�F�[�h�C��
	WEATHER_SUNNY_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_SUNNY_Main,		// ���C������
	WEATHER_SUNNY_InitFadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SUNNY_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_SUNNY_Exit,		// �j��

	// �I�u�W�F����֐�
	NULL,
};






//-----------------------------------------------------------------------------
/**
 *			����
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	u32 fogoffset, fogslope, light;
	SUNNY_WORK* p_sunnywork;

	// ���[�J�����[�N�擾
	p_sunnywork = WEATHER_TASK_GetWorkData( p_wk );
	p_sunnywork->fade_init = FALSE;

	// �t�H�O�̐ݒ�
	if( WEATHER_TASK_IsZoneFog( p_wk ) )
	{
		fogoffset = WEATHER_TASK_GetZoneFogOffset( p_wk );
		fogslope	= WEATHER_TASK_GetZoneFogSlope( p_wk );
		WEATHER_TASK_FogSet( p_wk, fogslope, FOG_START_OFFSET, fog_cont );
	}


	// ���C�g�ύX
	if( WEATHER_TASK_IsZoneLight( p_wk ) )
	{
		light = WEATHER_TASK_GetZoneLight( p_wk );
		WEATHER_TASK_LIGHT_Change( p_wk, FIELD_ZONEFOGLIGHT_ARC_LIGHT, light, heapID );
	}
	

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	BOOL result;
	SUNNY_WORK* p_sunnywork;

	// ���[�J�����[�N�擾
	p_sunnywork = WEATHER_TASK_GetWorkData( p_wk );
	if( p_sunnywork->fade_init == FALSE )
	{
		u32 fogoffset, fogslope;
		if( WEATHER_TASK_IsZoneFog( p_wk ) )
		{
			fogoffset = WEATHER_TASK_GetZoneFogOffset( p_wk );
			fogslope	= WEATHER_TASK_GetZoneFogSlope( p_wk );

			WEATHER_TASK_FogFadeIn_Init( p_wk,
			fogslope, 
			fogoffset, 
			FOG_FADE_SYNC,
			fog_cont );
		}
		p_sunnywork->fade_init = TRUE;
	}

	result = WEATHER_TASK_FogFade_IsFade( p_wk );
		
	if( result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
		// �V�[�P���X�ύX
		return WEATHER_TASK_FUNC_RESULT_FINISH;
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	u32 fogoffset, fogslope;

	if( WEATHER_TASK_IsZoneFog( p_wk ) )
	{
		fogoffset = WEATHER_TASK_GetZoneFogOffset( p_wk );
		fogslope	= WEATHER_TASK_GetZoneFogSlope( p_wk );
		// �t�H�O�̐ݒ�
		WEATHER_TASK_FogSet( p_wk, fogslope, fogoffset, fog_cont );
	}

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	
	if( WEATHER_TASK_IsZoneFog( p_wk ) )
	{
		WEATHER_TASK_FogFadeOut_Init( p_wk,
				FOG_START_OFFSET, 
				FOG_FADEOUT_SYNC, fog_cont );
	}

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	BOOL result;

	result = WEATHER_TASK_FogFade_IsFade( p_wk );
	if( result ){
		return WEATHER_TASK_FUNC_RESULT_FINISH;
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_SUNNY_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	if( WEATHER_TASK_IsZoneFog( p_wk ) )
	{
		// FOG�I��
		WEATHER_TASK_FogClear( p_wk, fog_cont );
	}

	if( WEATHER_TASK_IsZoneLight( p_wk ) )
	{
		// ���C�g����
		WEATHER_TASK_LIGHT_Back( p_wk, heapID );
	}

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}
