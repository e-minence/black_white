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

typedef enum
{
  ANIT_START,
  ANIT_ACTION,
  ANIT_DISTURB,
}ACTING_NPC_ITEMUSE_TYPE;
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

void STA_ACT_UseItemRequest( ACTING_WORK *work , MUS_POKE_EQUIP_POS ePos );
void STA_ACT_UseItem( ACTING_WORK *work , u8 pokeIdx , MUS_POKE_EQUIP_POS ePos );
void STA_ACT_CalcRank( ACTING_WORK *work );

const u8 STA_ACT_GetUseItemAttentionPoke( ACTING_WORK *work );
const BOOL STA_ACT_IsUsingItemSelf( ACTING_WORK *work );
void STA_ACT_CheckUseItemNpc( ACTING_WORK *work , const ACTING_NPC_ITEMUSE_TYPE type , const u8 selfIdx );

//�G�f�B�^�p

void	STA_ACT_EDITOR_SetEditorMode( ACTING_WORK *work );
void	STA_ACT_EDITOR_SetScript( ACTING_WORK *work , void* data );
void	STA_ACT_EDITOR_StartScript( ACTING_WORK *work );

#endif STA_ACTING_H__