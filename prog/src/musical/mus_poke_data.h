//======================================================================
/**
 * @file	mus_poke_data.h
 * @brief	�~���[�W�J���p �|�P�����f�[�^
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUS_POKE_DATA_H__
#define MUS_POKE_DATA_H__

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
//�`��P�C��
typedef struct _MUS_POKE_DATA_WORK MUS_POKE_DATA_WORK;

//======================================================================
//	proto
//======================================================================

//MUS_POKE_DRAW_WORK�̍쐬�œ����ō쐬����Ă���̂ŁA�`����ꏏ�Ɏg���ꍇ�͂���������擾���Ă�������
MUS_POKE_DATA_WORK* MUS_POKE_DATA_LoadMusPokeData( MUSICAL_POKE_PARAM *musPoke , HEAPID heapId );

//�A�C�e�������ӏ��֌W
const BOOL MUS_POKE_DATA_EnableEquipPosition( MUS_POKE_DATA_WORK *pokeData , const MUS_POKE_EQUIP_POS pos );
GFL_POINT* MUS_POKE_DATA_GetEquipPosition( MUS_POKE_DATA_WORK *pokeData , const MUS_POKE_EQUIP_POS pos );
//�w��_����߂��ʒu��T��(���΍��W�ɂ��ēn���Ă�������
//len�͔��苗����n���B�߂�l�Ŏ��ۂ̋�����"���"���A���Ă���
MUS_POKE_EQUIP_POS MUS_POKE_DATA_SearchEquipPosition( MUS_POKE_DATA_WORK *pokeData , GFL_POINT *pos , u16 *len );


#endif MUS_POKE_DATA_H__