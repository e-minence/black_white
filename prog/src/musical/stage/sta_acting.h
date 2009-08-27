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
#include "musical/musical_stage_sys.h"
#include "musical/mus_item_data.h"

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


//======================================================================
//	proto
//======================================================================

ACTING_WORK*	STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId );
void	STA_ACT_TermActing( ACTING_WORK *work );
ACTING_RETURN	STA_ACT_LoopActing( ACTING_WORK *work );

u16		STA_ACT_GetStageScroll( ACTING_WORK *work );
void	STA_ACT_SetStageScroll( ACTING_WORK *work , const u16 scroll );

MUS_ITEM_DATA_SYS* STA_ACT_GetItemDataSys( ACTING_WORK *work );

//�G�f�B�^�p

void	STA_ACT_EDITOR_SetScript( ACTING_WORK *work , void* data );
void	STA_ACT_EDITOR_StartScript( ACTING_WORK *work );
#endif STA_ACTING_H__