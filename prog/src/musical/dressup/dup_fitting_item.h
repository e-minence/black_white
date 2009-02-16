//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�h���X�A�b�v �������C��
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef DUP_FITTING_ITEM_H__
#define DUP_FITTING_ITEM_H__

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

typedef struct _FIT_ITEM_WORK FIT_ITEM_WORK;
typedef struct _FIT_ITEM_GROUP FIT_ITEM_GROUP;

//======================================================================
//	proto
//======================================================================

//�O���[�v�̍쐬�ƍ폜
FIT_ITEM_GROUP* DUP_FIT_ITEMGROUP_CreateGroup( HEAPID heapId , const u16 maxNum );
void DUP_FIT_ITEMGROUP_DeleteGroup( FIT_ITEM_GROUP* group );

//�A�C�e���̒ǉ��E�폜(�����N�ɂȂ������Ȃ̂Ń������̎擾�J���͂��Ȃ�
void DUP_FIT_ITEMGROUP_AddItem( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item );
void DUP_FIT_ITEMGROUP_AddItemTop( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item );
void DUP_FIT_ITEMGROUP_RemoveItem( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item );

//�����N�̃X�^�[�g���擾
FIT_ITEM_WORK* DUP_FIT_ITEMGROUP_GetStartItem( FIT_ITEM_GROUP *group );

//�A�C�e���̍쐬�E�폜
FIT_ITEM_WORK* DUP_FIT_ITEM_CreateItem( HEAPID heapId , MUS_ITEM_DRAW_SYSTEM *itemDrawSys , u16 itemId , GFL_G3D_RES *res , VecFx32 *pos );
void DUP_FIT_ITEM_DeleteItem( FIT_ITEM_WORK *item , MUS_ITEM_DRAW_SYSTEM *itemDrawSys );

//���̃A�C�e�����擾
FIT_ITEM_WORK* DUP_FIT_ITEM_GetNextItem( FIT_ITEM_WORK *item );

//�e���l�̎擾
u16 DUP_FIT_ITEM_GetItemIdx( FIT_ITEM_WORK *item );
MUS_ITEM_DRAW_WORK* DUP_FIT_ITEM_GetItemDrawWork( FIT_ITEM_WORK *item );

void DUP_FIT_ITEM_SetPosition( FIT_ITEM_WORK *item ,const GFL_POINT *pos );
GFL_POINT* DUP_FIT_ITEM_GetPosition( FIT_ITEM_WORK *item );
void DUP_FIT_ITEM_SetScale( FIT_ITEM_WORK *item , const fx32 scale );
fx32 DUP_FIT_ITEM_GetScale( FIT_ITEM_WORK *item );

//���W�Ƃ̃`�F�b�N
const BOOL DUP_FIT_ITEM_CheckHit( FIT_ITEM_WORK *item , u32 posX , u32 posY );
const u32 DUP_FIT_ITEM_CheckLength( FIT_ITEM_WORK *item , u32 posX , u32 posY );
const u16 DUP_FIT_ITEM_CheckLengthSqrt( FIT_ITEM_WORK *item , u32 posX , u32 posY );

#endif DUP_FITTING_ITEM_H__