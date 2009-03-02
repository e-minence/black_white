//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�~���[�W�J���p �|�P�����`��
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUS_POKE_DRAW_H__
#define MUS_POKE_DRAW_H__

#include "musical/musical_system.h"
#include "mus_poke_data.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
//�`��V�X�e��
typedef struct _MUS_POKE_DRAW_SYSTEM MUS_POKE_DRAW_SYSTEM;
//�`��P�C��
typedef struct _MUS_POKE_DRAW_WORK MUS_POKE_DRAW_WORK;

//�����ӏ��̈ʒu���
typedef struct 
{
	BOOL	isEnable;
	VecFx32 pos;
	u16		rot;
}MUS_POKE_EQUIP_DATA;

//======================================================================
//	proto
//======================================================================

//�V�X�e���̏������ƊJ��
MUS_POKE_DRAW_SYSTEM*	MUS_POKE_DRAW_InitSystem( HEAPID heapId );
void MUS_POKE_DRAW_TermSystem( MUS_POKE_DRAW_SYSTEM* work );

//�X�V
void MUS_POKE_DRAW_UpdateSystem( MUS_POKE_DRAW_SYSTEM* work );
void MUS_POKE_DRAW_DrawSystem( MUS_POKE_DRAW_SYSTEM* work );

//�|�P�����̒ǉ��E�폜
MUS_POKE_DRAW_WORK* MUS_POKE_DRAW_Add( MUS_POKE_DRAW_SYSTEM* work , MUSICAL_POKE_PARAM *musPoke );
void MUS_POKE_DRAW_Del( MUS_POKE_DRAW_SYSTEM* work , MUS_POKE_DRAW_WORK *drawWork );

//�e�p�����[�^�̒���
void MUS_POKE_DRAW_SetPosition( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *pos );
void MUS_POKE_DRAW_GetPosition( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *pos );
void MUS_POKE_DRAW_SetScale( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *scale );
void MUS_POKE_DRAW_GetScale( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *scale );
MUS_POKE_EQUIP_DATA *MUS_POKE_DRAW_GetEquipData( MUS_POKE_DRAW_WORK *drawWork , const MUS_POKE_EQUIP_POS pos );

MUS_POKE_DATA_WORK*	MUS_POKE_DRAW_GetPokeData( MUS_POKE_DRAW_WORK *drawWork );

#endif MUS_POKE_DRAW_H__