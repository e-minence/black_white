//============================================================================
/**
 *
 *	@file		intro_g3d.h
 *	@brief
 *	@author	hosaka genya
 *	@data		2009.12.20
 *
 */
//============================================================================
#pragma once

#include "demo/intro.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
#define	INTRO_3D_SEL_SEX_DEF_FRAME					( 60 )		// ���ʑI���̃f�t�H���g�t���[��
#define	INTRO_3D_SEL_SEX_ENTER_MALE_FRAME		( 0 )			// ���ʑI���̒j�̎q���莞�̃t���[��
#define	INTRO_3D_SEL_SEX_ENTER_FEMALE_FRAME	( 120 )		// ���ʑI���̏��̎q���莞�̃t���[��
#define	INTRO_3D_SEL_SEX_MOVE_MALE_FRAME		( 50 )		// ���ʑI���̒j�̎q�I�����̃t���[��
#define	INTRO_3D_SEL_SEX_MOVE_FEMALE_FRAME	( 70 )		// ���ʑI���̏��̎q�I�����̃t���[��

// ���ʑI���t���[�����샂�[�h
enum {
	INTRO_3D_SEL_SEX_MODE_MOVE_DEFAULT = 0,
	INTRO_3D_SEL_SEX_MODE_MOVE_MALE,
	INTRO_3D_SEL_SEX_MODE_MOVE_FEMALE,
	INTRO_3D_SEL_SEX_MODE_ENTER_MALE,
	INTRO_3D_SEL_SEX_MODE_ENTER_FEMALE,
};


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

typedef struct _INTRO_G3D_WORK INTRO_G3D_WORK;

extern INTRO_G3D_WORK* INTRO_G3D_Create( INTRO_GRAPHIC_WORK* graphic, INTRO_SCENE_ID scene, HEAPID heap_id );
extern void INTRO_G3D_Exit( INTRO_G3D_WORK* wk );
extern void INTRO_G3D_Main( INTRO_G3D_WORK* wk );

extern BOOL INTRO_G3D_SelectStart( INTRO_G3D_WORK* wk );

extern void INTRO_G3D_SelectDecideStart( INTRO_G3D_WORK* wk, u32 is_mode );
extern BOOL INTRO_G3D_SelectDecideWait( INTRO_G3D_WORK* wk );

extern void INTRO_G3D_SelectVisible( INTRO_G3D_WORK* wk, BOOL is_visible );

extern void INTRO_G3D_SelectSet( INTRO_G3D_WORK* wk, u32 in_frame );
extern BOOL INTRO_G3D_SelectDecideReturn( INTRO_G3D_WORK* wk );

