//======================================================================
/**
 * @file	musical_system.h
 * @brief	�~���[�W�J���p �ėp��`
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUSICAL_SYSTEM_H__
#define MUSICAL_SYSTEM_H__
#include "poke_tool/poke_tool.h"

//======================================================================
//	define
//======================================================================
//�~���[�W�J���̑����A�C�e���̍ő��
#define MUSICAL_ITEM_MAX (240)

//======================================================================
//	enum
//======================================================================

//�~���[�W�J���|�P�����̑����ӏ�
typedef enum
{
	MUS_POKE_EQU_EAR_R,		//�E��
	MUS_POKE_EQU_EAR_L,		//����
	MUS_POKE_EQU_HEAD,		//��
	MUS_POKE_EQU_FACE,		//��
	MUS_POKE_EQU_BODY,		//��
	MUS_POKE_EQU_HAND_R,	//�E��
	MUS_POKE_EQU_HAND_L,	//����
	MUS_POKE_EQU_WAIST,		//��
	MUS_POKE_EQU_TAIL,		//�K��

	MUS_POKE_EQUIP_MAX,
	
	MUS_POKE_EQU_INVALID,	//�����l
}MUS_POKE_EQUIP_POS;

//======================================================================
//	typedef struct
//======================================================================

typedef struct
{
	u16	type;	//�A�C�e���̎��
	u8	angle;	//�A�C�e���̌X��
}MUSICAL_POKE_EQUIP;

typedef struct
{
	POKEMON_PARAM *pokePara;
	MUSICAL_POKE_EQUIP equip[MUS_POKE_EQUIP_MAX];
}MUSICAL_POKE_PARAM;


//�~���[�W�J���̎Q�����i�����邩���ׂ�(��)
BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );

#endif MUSICAL_SYSTEM_H__
