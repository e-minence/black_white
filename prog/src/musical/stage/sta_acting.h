//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�X�e�[�W ���Z���C��
 * @author	ariizumi
 * @data	09/03/02
 */
//======================================================================
#ifndef STA_ACTING_H__
#define STA_ACTING_H__

#include "musical/musical_system.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	ACT_RET_CONTINUE,
	ACT_RET_GO_END,
}ACTING_RETURN;

//======================================================================
//	typedef struct
//======================================================================

typedef struct _ACTING_WORK ACTING_WORK;

typedef struct
{
	HEAPID heapId;
	MUSICAL_POKE_PARAM musPoke[MUSICAL_POKE_MAX];
	u16		repertorie;	//����
}ACTING_INIT_WORK;

//======================================================================
//	proto
//======================================================================

ACTING_WORK*	STA_ACT_InitActing( ACTING_INIT_WORK *initWork );
void	STA_ACT_InitActing_2nd( ACTING_WORK *work );
void	STA_ACT_TermActing( ACTING_WORK *work );
ACTING_RETURN	STA_ACT_LoopActing( ACTING_WORK *work );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------

#endif STA_ACTING_H__