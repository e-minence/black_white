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
void MUS_ITEM_DRAW_UpdateSystem_VBlank( MUS_ITEM_DRAW_SYSTEM* work );
void MUS_ITEM_DRAW_DrawSystem( MUS_ITEM_DRAW_SYSTEM* work );

//�A�C�e���̒ǉ��E�폜
//�A�C�e���ԍ����烊�\�[�X�̓ǂݍ���
GFL_G3D_RES* MUS_ITEM_DRAW_LoadResource( u16 itemIdx );
void MUS_ITEM_DRAW_DeleteResource( GFL_G3D_RES *res );
//�ԍ�����ǉ��ƁA�ǂݍ��ݍς݃��\�[�X����ǉ�
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddItemId( MUS_ITEM_DRAW_SYSTEM* work , u16 itemIdx , VecFx32 *pos );
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddResource( MUS_ITEM_DRAW_SYSTEM* work ,  u16 itemIdx ,GFL_G3D_RES* g3DresTex, VecFx32 *pos );
void MUS_ITEM_DRAW_DelItem( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork );

//�A�C�e���ԍ�����ARC�̔ԍ��𒲂ׂ�
u16 MUS_ITEM_DRAW_GetArcIdx( const u16 itemIdx );
void MUS_ITEM_DRAW_GetPicSize( MUS_ITEM_DRAW_WORK *itemWork , u8 *sizeXRate , u8 *sizeYRate );

const BOOL MUS_ITEM_DRAW_CanEquipPos( MUS_ITEM_DRAW_WORK *itemWork , const MUS_POKE_EQUIP_POS pos );

//�G�̕ύX
void MUS_ITEM_DRAW_ChengeGraphic( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , u16 newId , GFL_G3D_RES *newRes );
void MUS_ITEM_DRAW_SetShadowPallet( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , GFL_G3D_RES *shadowRes );

//�e�p�Ɋe���l���R�s�[����
void MUS_ITEM_DRAW_CopyItemDataToShadow(  MUS_ITEM_DRAW_SYSTEM* work ,MUS_ITEM_DRAW_WORK *baseItem , MUS_ITEM_DRAW_WORK *shadowItem);

//�l�ϊ�
void MUS_ITEM_DRAW_SetDrawEnable( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , BOOL flg );
void MUS_ITEM_DRAW_SetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_GetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_SetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 sizeX , fx16 sizeY );
void MUS_ITEM_DRAW_GetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 *sizeX , fx16 *sizeY );
void MUS_ITEM_DRAW_SetRotation( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , u16 rotZ );
void MUS_ITEM_DRAW_GetRotation( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , u16 *rotZ );
void MUS_ITEM_DRAW_SetAlpha( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , u8 alpha );
void MUS_ITEM_DRAW_SetFlipS( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , const BOOL );
void MUS_ITEM_DRAW_SetUseOffset( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , const BOOL flg );
const BOOL MUS_ITEM_DRAW_GetUseOffset( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork );
void MUS_ITEM_DRAW_GetOffsetPos( MUS_ITEM_DRAW_WORK *itemWork , GFL_POINT *pos );

//���W�`�F�b�N�n
void MUS_ITEM_DRAW_CheckHit( MUS_ITEM_DRAW_WORK *itemWork , fx16 posX , fx16 posY );

#if DEB_ARI
void MUS_ITEM_DRAW_Debug_DumpResData( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork );
#endif

#endif MUS_ITEM_DRAW_H__