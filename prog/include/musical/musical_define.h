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
#include "buflen.h"

//======================================================================
//	define
//======================================================================
//�~���[�W�J���̑����A�C�e���̍ő��(�ő�255�ōl���Ă���
#define MUSICAL_ITEM_MAX (250)
#define MUSICAL_ITEM_MAX_REAL (100) //������
#define MUSICAL_ITEM_INVALID (255)
//�~���[�W�J���ɎQ���\�Ȑl��
#define MUSICAL_POKE_MAX (4)

//�~���[�W�J���ő����ł���A�C�e���̌�
#define MUSICAL_ITEM_EQUIP_MAX (MUS_POKE_EQUIP_USER_MAX)	//�灕���E���E�E��E����

//�~���[�W�J���̉��ږ��̕�����
#define MUSICAL_PROGRAM_NAME_MAX (36+EOM_SIZE) //���{18�E�C�O36�{EOM

//�~���[�W�J���̔w�i��
#define MUSICAL_BACK_NUM (20)

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
	MUS_POKE_EQU_USER_WAIST,		//��
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

//�~���|�W�J���̔z�_���
typedef enum
{
  MCT_COOL,
  MCT_CUTE,
  MCT_ELEGANT,
  MCT_UNIQUE,
  MCT_MAX,

  MCT_RANDOM = MCT_MAX,   //�����_���z�_��(�f�[�^�̂ݑ���
  
  MCT_DATA_MAX,
}MUSICAL_CONDITION_TYPE;

//======================================================================
//	typedef struct
//======================================================================

typedef struct
{
	u16	itemNo;	//�A�C�e���̎��
	s16	angle;	//�A�C�e���̌X��
	u8  priority; //�\���D��x
}MUSICAL_POKE_EQUIP;

typedef struct
{
  u16 monsno;
  u8  sex;
  u8  form;
  u8  rare;
}MUSICAL_MCSS_PARAM;

typedef struct
{
  MUSICAL_CHARA_TYPE charaType;
	POKEMON_PARAM *pokePara;
	MUSICAL_MCSS_PARAM mcssParam;
	MUSICAL_POKE_EQUIP equip[MUS_POKE_EQUIP_MAX];
	u16                point; //�]���_
	u16                conPoint[MCT_MAX]; //�R���f�B�V�����ʕ]���_
}MUSICAL_POKE_PARAM;

//----------------------------------------------------------
//	�~���[�W�J���V���b�g�n
//----------------------------------------------------------
//�����O�b�Y�f�[�^  8byte
typedef struct
{
  u16 itemNo;   //�O�b�Y�ԍ�
  s16 angle;    //�p�x
  u8  equipPos; //�����ӏ�
  u8  pad[3];
}
MUSICAL_SHOT_POKE_EQUIP;

//�|�P�����P�̂̃f�[�^  84byte
typedef struct
{
  u16 monsno; //�|�P�����ԍ�
  u16  sex :2; //����
  u16  rare:1; //���A�t���O
  u16  form:5; //�t�H�����ԍ�
  u16  pad :8;
  
  STRCODE trainerName[SAVELEN_PLAYER_NAME+EOM_SIZE];  //�g���[�i�[��
  MUSICAL_SHOT_POKE_EQUIP equip[MUSICAL_ITEM_EQUIP_MAX];  //�����O�b�Y�f�[�^(�W��
  
}MUSICAL_SHOT_POKE;

//�~���[�W�J���V���b�g�̃f�[�^  416byte
typedef struct
{
  u32 bgNo   :5;  //�w�i�ԍ�
  u32 spotBit:4;  //�X�|�b�g���C�g�Ώ�(bit)(�g�b�v�������|�P����
  u32 year   :7;  //�N
  u32 month  :5;  //��  ���ꂪ�O�������疳���f�[�^�Ƃ݂Ȃ�
  u32 day    :6;  //��
  u32 player :2;  //�����̔ԍ�
  u32 pad    :3;

  MUSICAL_SHOT_POKE shotPoke[MUSICAL_POKE_MAX]; //�|�P�����f�[�^(�S��
  STRCODE title[MUSICAL_PROGRAM_NAME_MAX];  //�~���[�W�J���^�C�g��(���{18�E�C�O36�{EOM
  u16 pad2;
  
}MUSICAL_SHOT_DATA;


#endif MUSICAL_DEFINE_H__
