//======================================================================
/**
 * @file	dup_local_def.h
 * @brief	�~���[�W�J���@�h���X�A�b�v�p��`
 * @author	ariizumi
 * @data	09/05/29
 */
//======================================================================

#pragma once

#include "musical/musical_system.h"

#if 1
#define DUP_TPrintf(...) (void)((MUS_TPrintf(__VA_ARGS__)))
#define DUP_Printf(...)  (void)((ARI_Printf(__VA_ARGS__)))
#else
#define DUP_TPrintf(...) ((void)0)
#define DUP_Printf(...)  ((void)0)
#endif //DEB_ARI
