//============================================================================
/**
 *
 *	@file		demo3d_cmd.h
 *	@brief
 *	@author		hosaka genya
 *	@data		2009.12.09
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "demo/demo3d.h"

typedef struct _DEMO3D_CMD_WORK DEMO3D_CMD_WORK;

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================


extern DEMO3D_CMD_WORK* Demo3D_CMD_Init( DEMO3D_ID demo_id, u32 start_frame, HEAPID heap_id );

extern void Demo3D_CMD_Exit( DEMO3D_CMD_WORK* wk );

extern void Demo3D_CMD_Main( DEMO3D_CMD_WORK* wk, fx32 now_frame );

