//======================================================================
/**
 * @file	mus_item_data.h
 * @brief	�~���[�W�J���p �A�C�e���f�[�^
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================

#pragma once

#include "musical/musical_system.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//�~���[�W�J���|�P�����̑����ӏ����ʗp
//�A�C�e���f�[�^�ɐݒ肳���B�����ł���ꏊ�̎��
typedef enum
{
	MUS_POKE_EQU_TYPE_EAR,		//��(�E���E����)
	MUS_POKE_EQU_TYPE_HEAD,		//��
	MUS_POKE_EQU_TYPE_EYE,		//��
	MUS_POKE_EQU_TYPE_FACE,		//��
	MUS_POKE_EQU_TYPE_BODY,		//��
	MUS_POKE_EQU_TYPE_WAIST,	//��
	MUS_POKE_EQU_TYPE_HAND,		//��(�E��E����)

	MUS_POKE_EQU_IS_BACK,			//�w�ʑ�������
	MUS_POKE_EQU_IS_FRONT,		//�O�ʑ�������

	MUS_POKE_EQUIP_TYPE_MAX,
	
	MUS_POKE_EQU_TYPE_INVALID,	//�����l
}MUS_POKE_EQUIP_TYPE;

//�~���|�W�J���̃A�C�e���g�p�^�C�v
typedef enum
{
  MUT_SPIN,
  MUT_FLASH,
  MUT_FLYING,
  MUT_THROW,
  MUT_USE,
  
  MUT_MAX,
}MUSICAL_ITEM_USETYPE;

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
MUS_ITEM_DATA_WORK* MUS_ITEM_DATA_GetMusItemData( MUS_ITEM_DATA_SYS* sysWork , const u16 itemNo );

void	MUS_ITEM_DATA_GetDispOffset( MUS_ITEM_DATA_WORK*  dataWork , GFL_POINT* ofs );
GFL_BBD_TEXSIZ	MUS_ITEM_DATA_GetTexType( MUS_ITEM_DATA_WORK*  dataWork );
const BOOL	MUS_ITEM_DATA_CanEquipPos( MUS_ITEM_DATA_WORK*  dataWork , const MUS_POKE_EQUIP_POS pos );
const BOOL	MUS_ITEM_DATA_CanEquipPosUserData( MUS_ITEM_DATA_WORK*  dataWork , const MUS_POKE_EQUIP_USER pos );

//�w�肳�ꂽ�ӏ������C���ƂȂ鑕���ӏ����H
const BOOL	MUS_ITEM_DATA_CheckMainPosUserData( MUS_ITEM_DATA_WORK*  dataWork , const MUS_POKE_EQUIP_USER pos );

//�������w�ʂɗ���A�C�e�����H
const BOOL	MUS_ITEM_DATA_IsBackItem( MUS_ITEM_DATA_WORK*  dataWork );
//�������O�ʖʂɗ���A�C�e�����H
const BOOL	MUS_ITEM_DATA_IsFrontItem( MUS_ITEM_DATA_WORK*  dataWork );

//�����ӏ����烆�[�U�[�^�C�v�����ӏ��֕ϊ�
const MUS_POKE_EQUIP_USER MUS_ITEM_DATA_EquipPosToUserType( const MUS_POKE_EQUIP_USER pos );

//�A�C�e���̃R���f�B�V�����^�C�v���擾
const MUSICAL_CONDITION_TYPE MUS_ITEM_DATA_GetItemConditionType( MUS_ITEM_DATA_SYS* sysWork , const u16 itemNo );

//�A�C�e���̎g�p�^�C�v���擾
const MUSICAL_ITEM_USETYPE MUS_ITEM_DATA_GetItemUseType( MUS_ITEM_DATA_SYS* sysWork , const u16 itemNo );
