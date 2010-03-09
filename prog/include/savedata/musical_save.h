//============================================================================================
/**
 * @file	musical_save.h
 * @brief	�~���[�W�J���p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================
#pragma once


#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "musical/musical_define.h"

//======================================================================
//	define
//======================================================================

#define MUS_SAVE_ITEM_MAX (256)
#define MUS_SAVE_ITEM_BIT_MAX (MUS_SAVE_ITEM_MAX/8)
#define MUS_SAVE_ENTRY_NUM_MAX (65535)
#define MUS_SAVE_FAN_NUM (10)

//======================================================================
//	typedef struct
//======================================================================

typedef struct
{
	u8	pos;					//�����ӏ�(MUS_POKE_EQUIP_POS�j
	u16	itemNo;	      //�A�C�e���̎��
	s16	angle;	      //�A�C�e���̌X��
}MUSICAL_EQUIP_ONE_SAVE;

typedef struct
{
	MUSICAL_EQUIP_ONE_SAVE equipData[MUSICAL_ITEM_EQUIP_MAX];
}MUSICAL_EQUIP_SAVE;

typedef struct _MUSICAL_SAVE MUSICAL_SAVE;

//�t�@�����l�n
typedef struct
{
  u8 type;          //���(MUSICAL_CONDITION_TYPE)
  u8 giftType;   //�v���[���g���
  u16 giftValue; //�A�C�e���ԍ��E�O�b�Y�ԍ�
}MUSICAL_FAN_STATE;



//======================================================================
//	proto
//======================================================================

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int MUSICAL_SAVE_GetWorkSize(void);
extern void MUSICAL_SAVE_InitWork(MUSICAL_SAVE * musSave);

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
extern MUSICAL_SAVE* MUSICAL_SAVE_GetMusicalSave( SAVE_CONTROL_WORK *sv );

extern void MUSICAL_SAVE_ResetBefEquip( MUSICAL_SAVE *musSave );
extern MUSICAL_EQUIP_SAVE* MUSICAL_SAVE_GetBefEquipData( MUSICAL_SAVE *musSave );

extern BOOL MUSICAL_SAVE_IsValidMusicalShotData( MUSICAL_SAVE *musSave );
extern MUSICAL_SHOT_DATA* MUSICAL_SAVE_GetMusicalShotData( MUSICAL_SAVE *musSave );

extern const BOOL MUSICAL_SAVE_ChackHaveItem( MUSICAL_SAVE *musSave , const u8 itemNo );
extern const BOOL MUSICAL_SAVE_ChackNewItem( MUSICAL_SAVE *musSave , const u8 itemNo );
extern void MUSICAL_SAVE_AddItem( MUSICAL_SAVE *musSave , const u8 itemNo );
extern void MUSICAL_SAVE_ResetNewItem( MUSICAL_SAVE *musSave );
extern const int MUSICAL_SAVE_GetItemNum( MUSICAL_SAVE *musSave );
extern const BOOL MUSICAL_SAVE_IsCompleteItem( MUSICAL_SAVE *musSave );

extern MUSICAL_FAN_STATE* MUSICAL_SAVE_GetFanState( MUSICAL_SAVE *musSave , const u8 idx );

extern const u16 MUSICAL_SAVE_GetEntryNum( const MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_AddEntryNum( MUSICAL_SAVE *musSave );
extern const u16 MUSICAL_SAVE_GetTopNum( const MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_AddTopNum( MUSICAL_SAVE *musSave );
extern const u8 MUSICAL_SAVE_GetBefCondition( MUSICAL_SAVE *musSave , const MUSICAL_CONDITION_TYPE conType );
extern const MUSICAL_CONDITION_TYPE MUSICAL_SAVE_GetMaxBefConditionType( MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_SetBefCondition( MUSICAL_SAVE *musSave , const MUSICAL_CONDITION_TYPE conType , const u8 value );
extern const u8 MUSICAL_SAVE_GetBefPoint( const MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_SetBefPoint( MUSICAL_SAVE *musSave , const u8 point );
extern const u8 MUSICAL_SAVE_GetProgramNumber( const MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_SetProgramNumber( MUSICAL_SAVE *musSave , const u8 num );

extern const BOOL MUSICAL_SAVE_IsEnableDistributData( const MUSICAL_SAVE *musSave );
extern void MUSICAL_SAVE_SetEnableDistributData( MUSICAL_SAVE *musSave , const BOOL isEnable );


