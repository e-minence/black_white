//======================================================================
/**
 * @file	mus_item_data.h
 * @brief	�~���[�W�J���p �A�C�e���f�[�^
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUS_ITEM_DATA_H__
#define MUS_ITEM_DATA_H__

#include "musical/musical_system.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
typedef struct _MUS_ITEM_DATA_SYS MUS_ITEM_DATA_SYS;
//�P��
typedef struct _MUS_ITEM_DATA_WORK MUS_ITEM_DATA_WORK;

//======================================================================
//	proto
//======================================================================

MUS_ITEM_DATA_SYS* MUS_ITEM_DATA_InitSystem( HEAPID heapId );
void MUS_ITEM_DATA_ExitSystem( MUS_ITEM_DATA_SYS *sysWork );

//MUS_ITEM_DRAW_WORK�̍쐬�œ����ō쐬����Ă���̂ŁA�`����ꏏ�Ɏg���ꍇ�͂���������擾���Ă�������
MUS_ITEM_DATA_WORK* MUS_ITEM_DATA_LoadMusItemData( MUS_ITEM_DATA_SYS* sysWork , const u16 itemNo , HEAPID heapId );

void	MUS_ITEM_DATA_GetDispOffset( MUS_ITEM_DATA_WORK*  dataWork , GFL_POINT* ofs );
GFL_BBD_TEXSIZ	MUS_ITEM_DATA_GetTexType( MUS_ITEM_DATA_WORK*  dataWork );
const BOOL	MUS_ITEM_DATA_CanEquipPos( MUS_ITEM_DATA_WORK*  dataWork , const MUS_POKE_EQUIP_POS pos );

#endif //MUS_ITEM_DATA_H__
