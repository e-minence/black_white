//======================================================================
/**
 * @file	plist_sys.h
 * @brief	�|�P�����X�e�[�^�X ���C������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS
 */
//======================================================================
#pragma once

#include "p_sta_local_def.h"

extern const BOOL PSTATUS_InitPokeStatus( PSTATUS_WORK *work );
extern const BOOL PSTATUS_TermPokeStatus( PSTATUS_WORK *work );
extern const BOOL PSTATUS_UpdatePokeStatus( PSTATUS_WORK *work );
extern void PSTATUS_SetActiveBarButton( PSTATUS_WORK *work , const BOOL isLock );

extern const POKEMON_PASO_PARAM* PSTATUS_UTIL_GetCurrentPPP( PSTATUS_WORK *work );

