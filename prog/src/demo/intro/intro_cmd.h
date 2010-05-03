//============================================================================
/**
 *
 *	@file		intro_cmd.h
 *	@brief  �C���g���f���p�R�}���h
 *	@author		hosaka genya
 *	@data		2009.12.09
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "demo/intro.h"

#include "intro_particle.h"

typedef struct _INTRO_CMD_WORK INTRO_CMD_WORK;

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

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================
extern INTRO_CMD_WORK* Intro_CMD_Init( INTRO_G3D_WORK* g3d, INTRO_PARTICLE_WORK* ptc ,INTRO_MCSS_WORK* mcss, INTRO_PARAM* init_param, INTRO_GRAPHIC_WORK	*graphic, HEAPID heap_id );

extern void Intro_CMD_Exit( INTRO_CMD_WORK* wk );

extern BOOL Intro_CMD_Main( INTRO_CMD_WORK* );

