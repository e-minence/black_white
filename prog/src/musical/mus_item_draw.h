//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�~���[�W�J���p �|�P�����`��
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUS_ITEM_DRAW_H__
#define MUS_ITEM_DRAW_H__

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
typedef struct _MUS_ITEM_DRAW_SYSTEM MUS_ITEM_DRAW_SYSTEM;
//�`��P�C��
typedef struct _MUS_ITEM_DRAW_WORK MUS_ITEM_DRAW_WORK;

//======================================================================
//	proto
//======================================================================

//�V�X�e���̏������ƊJ��
MUS_ITEM_DRAW_SYSTEM*	MUS_ITEM_DRAW_InitSystem( GFL_BBD_SYS *bbdSys , u16 itemMax , HEAPID heapId );
void MUS_ITEM_DRAW_TermSystem( MUS_ITEM_DRAW_SYSTEM* work );

//�X�V
void MUS_ITEM_DRAW_UpdateSystem( MUS_ITEM_DRAW_SYSTEM* work );
void MUS_ITEM_DRAW_DrawSystem( MUS_ITEM_DRAW_SYSTEM* work );

//�A�C�e���̒ǉ��E�폜
//�A�C�e���ԍ����烊�\�[�X�̓ǂݍ���
GFL_G3D_RES* MUS_ITEM_DRAW_LoadResource( u16 itemIdx );
void MUS_ITEM_DRAW_DeleteResource( GFL_G3D_RES *res );
//�ԍ�����ǉ��ƁA�ǂݍ��ݍς݃��\�[�X����ǉ�
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddItemId( MUS_ITEM_DRAW_SYSTEM* work , u16 itemIdx , VecFx32 *pos );
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddResource( MUS_ITEM_DRAW_SYSTEM* work , GFL_G3D_RES* g3DresTex, VecFx32 *pos );
void MUS_ITEM_DRAW_Del( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork );

void MUS_ITEM_DRAW_SetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_GetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_SetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 *sizeX , fx16 *sizeY );
void MUS_ITEM_DRAW_GetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 *sizeX , fx16 *sizeY );

#endif MUS_ITEM_DRAW_H__