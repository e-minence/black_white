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
#define MUSICAL_ITEM_EQUIP_MAX (MUS_POKE_EQUIP_USER_MAX)	//�灕���E���E�E��E����

//======================================================================
//	enum
//======================================================================

//�~���[�W�J���|�P�����̑����ӏ�
//�f�[�^��̑����ӏ�
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
//���[�U�[���F���ł���ӏ��B
typedef enum
{
	MUS_POKE_EQU_USER_EAR_R,	//��(�E��)
	MUS_POKE_EQU_USER_EAR_L,	//��(����)
	MUS_POKE_EQU_USER_HEAD,		//��
	MUS_POKE_EQU_USER_FACE,		//��
	MUS_POKE_EQU_USER_BODY,		//��
	MUS_POKE_EQU_USER_HAND_R,	//��(�E��)
	MUS_POKE_EQU_USER_HAND_L,	//��(����)

	MUS_POKE_EQUIP_USER_MAX,
	
	MUS_POKE_EQU_USER_INVALID,	//�����l
}MUS_POKE_EQUIP_USER;

//�~���[�W�J���̃L�����̎��
typedef enum
{
  MUS_CHARA_PLAYER, //����
  MUS_CHARA_COMM,   //�ʐM����
  MUS_CHARA_NPC,    //NPC

  MUS_CHARA_MAX,

  MUS_CHARA_INVALID,    //�����l
}MUSICAL_CHARA_TYPE;

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
  MUSICAL_CHARA_TYPE charaType;
	POKEMON_PARAM *pokePara;
	MUSICAL_POKE_EQUIP equip[MUS_POKE_EQUIP_MAX];
}MUSICAL_POKE_PARAM;

#endif MUSICAL_DEFINE_H__
