//======================================================================
/**
 * @file	musical_system.h
 * @brief	�~���[�W�J���p �ėp��`
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUSICAL_DEFINE_H__
#define MUSICAL_DEFINE_H__
#include "poke_tool/poke_tool.h"

//======================================================================
//	define
//======================================================================
//�~���[�W�J���̑����A�C�e���̍ő��(�ő�512�ōl���Ă���
#define MUSICAL_ITEM_MAX (250)
#define MUSICAL_ITEM_INVALID (511)
//�~���[�W�J���ɎQ���\�Ȑl��
#define MUSICAL_POKE_MAX (4)

//�~���[�W�J���ő����ł���A�C�e���̌�
#define MUSICAL_ITEM_EQUIP_MAX (4)	//�灕���E���E�E��E����

//======================================================================
//	enum
//======================================================================

//�~���[�W�J���|�P�����̑����ӏ�
typedef enum
{
	MUS_POKE_EQU_EAR_R,		//�E��
	MUS_POKE_EQU_EAR_L,		//����
	MUS_POKE_EQU_HEAD,		//��
	MUS_POKE_EQU_EYE,		//�ځE�@
	MUS_POKE_EQU_FACE,		//��
	MUS_POKE_EQU_BODY,		//��
	MUS_POKE_EQU_WAIST,		//��
	MUS_POKE_EQU_HAND_R,	//�E��
	MUS_POKE_EQU_HAND_L,	//����

	MUS_POKE_EQUIP_MAX,
	
	MUS_POKE_EQU_INVALID,	//�����l
}MUS_POKE_EQUIP_POS;

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

	MUS_POKE_EQUIP_TYPE_MAX,
	
	MUS_POKE_EQU_TYPE_INVALID,	//�����l
}MUS_POKE_EQUIP_TYPE;


//======================================================================
//	typedef struct
//======================================================================

typedef struct
{
	u16	itemNo;	//�A�C�e���̎��
	s16	angle;	//�A�C�e���̌X��
}MUSICAL_POKE_EQUIP;

typedef struct
{
	POKEMON_PARAM *pokePara;
	MUSICAL_POKE_EQUIP equip[MUS_POKE_EQUIP_MAX];
}MUSICAL_POKE_PARAM;

#endif MUSICAL_DEFINE_H__
