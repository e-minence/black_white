//======================================================================
/**
 * @file	dressup_system.h
 * @brief	ドレスアップ 試着メイン
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef DUP_FITTING_H__
#define DUP_FITTING_H__

#include "musical/musical_system.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	FIT_RET_CONTINUE,
	FIT_RET_GO_CHECK,
	FIT_RET_GO_END,
}FITTING_RETURN;

//======================================================================
//	typedef struct
//======================================================================

typedef struct _FITTING_WORK FITTING_WORK;

typedef struct
{
	HEAPID heapId;
	MUSICAL_POKE_PARAM *musPoke;
}FITTING_INIT_WORK;

//======================================================================
//	proto
//======================================================================

FITTING_WORK*	DUP_FIT_InitFitting( FITTING_INIT_WORK *initWork );
void	DUP_FIT_TermFitting( FITTING_WORK *work );
FITTING_RETURN	DUP_FIT_LoopFitting( FITTING_WORK *work );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------

#endif DUP_FITTING_H__