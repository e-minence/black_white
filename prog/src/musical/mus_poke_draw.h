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
#include "musical/musical_mcss.h"
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
	VecFx32 ofs;
	VecFx32 scale;
	VecFx32 rotOfs;
	u16		rot;
	u16		itemRot;
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
MUS_POKE_DRAW_WORK* MUS_POKE_DRAW_Add( MUS_POKE_DRAW_SYSTEM* work , MUSICAL_POKE_PARAM *musPoke , const BOOL useBack );
void MUS_POKE_DRAW_Del( MUS_POKE_DRAW_SYSTEM* work , MUS_POKE_DRAW_WORK *drawWork );
MUS_MCSS_SYS_WORK* MUS_POKE_DRAW_GetMcssSys( MUS_POKE_DRAW_SYSTEM *work );

//�e�p�����[�^�̒���
void MUS_POKE_DRAW_SetPosition( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *pos );
void MUS_POKE_DRAW_GetPosition( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *pos );
void MUS_POKE_DRAW_SetScale( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *scale );
void MUS_POKE_DRAW_GetScale( MUS_POKE_DRAW_WORK *drawWork , VecFx32 *scale );
void MUS_POKE_DRAW_SetRotation( MUS_POKE_DRAW_WORK *drawWork , u16 rot );
void MUS_POKE_DRAW_GetRotation( MUS_POKE_DRAW_WORK *drawWork , u16 *rot );
void MUS_POKE_DRAW_SetShowFlg( MUS_POKE_DRAW_WORK *drawWork , const BOOL flg );
BOOL MUS_POKE_DRAW_GetShowFlg( MUS_POKE_DRAW_WORK *drawWork );
void MUS_POKE_DRAW_StartAnime( MUS_POKE_DRAW_WORK *drawWork );
void MUS_POKE_DRAW_StopAnime( MUS_POKE_DRAW_WORK *drawWork );
void MUS_POKE_DRAW_ChangeAnime( MUS_POKE_DRAW_WORK *drawWork , const u8 anmIdx );

void MUS_POKE_DRAW_FlipFrontBack( MUS_POKE_DRAW_WORK *drawWork );
void MUS_POKE_DRAW_SetFrontBack( MUS_POKE_DRAW_WORK *drawWork , const BOOL isFront );

//MCSS�V�X�e���̃e�N�X�`���ǂݍ��݃A�h���X�̕ύX(�ǂݍ��ݑO�ɁI
void MUS_POKE_DRAW_SetTexAddres( MUS_POKE_DRAW_SYSTEM* work , u32 adr );
void MUS_POKE_DRAW_SetPltAddres( MUS_POKE_DRAW_SYSTEM* work , u32 adr );

MUS_POKE_EQUIP_DATA *MUS_POKE_DRAW_GetEquipData( MUS_POKE_DRAW_WORK *drawWork , const MUS_POKE_EQUIP_POS pos );
VecFx32 *MUS_POKE_DRAW_GetShadowOfs( MUS_POKE_DRAW_WORK *drawWork );
VecFx32 *MUS_POKE_DRAW_GetRotateOfs( MUS_POKE_DRAW_WORK *drawWork );
//�f�o�b�O�p
BOOL* MUS_POKE_DRAW_GetEnableRotateOfs( MUS_POKE_DRAW_WORK *drawWork );
BOOL* MUS_POKE_DRAW_GetEnableShadowOfs( MUS_POKE_DRAW_WORK *drawWork );

#if PM_DEBUG
extern void  MUS_POKE_StepAnime( MUS_POKE_DRAW_SYSTEM* work , MUS_POKE_DRAW_WORK *drawWork , fx32 frame );
#endif //PM_DEBUG

#endif MUS_POKE_DRAW_H__