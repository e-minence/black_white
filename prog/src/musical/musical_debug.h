//======================================================================
/**
 * @file	musical_debug.h
 * @brief	�~���[�W�J���f�o�b�O�p
 * @author	ariizumi
 * @data	100322
 *
 * ���W���[�����FMUSICAL_DEBUG
 */
//======================================================================

#pragma once

#include "musical/musical_define.h"

#if PM_DEBUG
extern const BOOL MUSICAL_DEBUG_CreateDummyData( MUSICAL_SHOT_DATA* shotData , const u16 monsNo , const HEAPID heapId );
#endif