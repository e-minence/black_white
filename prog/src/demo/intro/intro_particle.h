//============================================================================
/**
 *
 *	@file		intro_particle.h
 *	@brief  �C���g�� �p�[�e�B�N������
 *	@author		hosaka genya
 *	@data		2009.12.21
 *
 */
//============================================================================
#pragma once

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
typedef struct _INTRO_PARTICLE_WORK INTRO_PARTICLE_WORK;
//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

extern INTRO_PARTICLE_WORK* INTRO_PARTICLE_Create( INTRO_GRAPHIC_WORK* graphic, HEAPID heap_id );
extern void INTRO_PARTICLE_Exit( INTRO_PARTICLE_WORK* wk );
extern void INTRO_PARTICLE_Main( INTRO_PARTICLE_WORK* wk );
extern void INTRO_PARTICLE_CreateEmitterMonsterBall( INTRO_PARTICLE_WORK* wk, fx32 px, fx32 py, fx32 pz );





