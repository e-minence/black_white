//============================================================================================
/**
 * @file	musical_save.h
 * @brief	�~���[�W�J���p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================

#ifndef MUSICAL_SAVE_H__
#define MUSICAL_SAVE_H__

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "musical/musical_define.h"


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

#endif //MUSICAL_SAVE_H__
