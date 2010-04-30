//==============================================================================
/**
 * @brief   �~���[�W�J���V���b�g�F�T�u�w�b�_
 */
//==============================================================================
#ifndef __GT_GDS_MUSICAL_SUB_H__
#define __GT_GDS_MUSICAL_SUB_H__


//==============================================================================
//  �萔��`
//==============================================================================
//�~���[�W�J���|�P�����̑����ӏ����ʗp
//���[�U�[���F���ł���ӏ��B
typedef enum
{
	MUS_POKE_EQU_USER_EAR_R,	//��(�E��)
	MUS_POKE_EQU_USER_EAR_L,	//��(����)
	MUS_POKE_EQU_USER_HEAD,		//��
	MUS_POKE_EQU_USER_FACE,		//��
	MUS_POKE_EQU_USER_BODY,		//��
	MUS_POKE_EQU_USER_WAIST,	//��
	MUS_POKE_EQU_USER_HAND_R,	//��(�E��)
	MUS_POKE_EQU_USER_HAND_L,	//��(����)

	MUS_POKE_EQUIP_USER_MAX,
	
	MUS_POKE_EQU_USER_INVALID,	//�����l
}MUS_POKE_EQUIP_USER;

///�~���[�W�J���̃g���[�i�[�l�[����
#define MUSICAL_TRAINER_NAME_LEN      (7+1) //�v���C���[���{EOM

//�~���[�W�J���ɎQ���\�Ȑl��
#define MUSICAL_POKE_MAX (4)

//�~���[�W�J���ő����ł���A�C�e���̌�
#define MUSICAL_ITEM_EQUIP_MAX (MUS_POKE_EQUIP_USER_MAX)	//�灕���E���E�E��E����

//�~���[�W�J���̉��ږ��̕�����
#define MUSICAL_PROGRAM_NAME_MAX (36+EOM_SIZE) //���{18�E�C�O36�{EOM


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
//  �����O�b�Y�f�[�^    8byte
//--------------------------------------------------------------
typedef struct
{
  u16 itemNo;   //�O�b�Y�ԍ�
  s16 angle;    //�p�x
  u8  equipPos; //�����ӏ�
  u8  pad[3];
}MUSICAL_SHOT_POKE_EQUIP;

//--------------------------------------------------------------
//  �|�P�����P�̂̃f�[�^�@100byte
//--------------------------------------------------------------
typedef struct
{
  u16 monsno; //�|�P�����ԍ�
  u16  sex :2; //����
  u16  rare:1; //���A�t���O
  u16  form:5; //�t�H�����ԍ�
  u16  pad :8;
  u32  perRand; //������  +8byte
  
  STRCODE trainerName[MUSICAL_TRAINER_NAME_LEN];  //�g���[�i�[��
  MUSICAL_SHOT_POKE_EQUIP equip[MUSICAL_ITEM_EQUIP_MAX];  //�����O�b�Y�f�[�^(�V��
  
}MUSICAL_SHOT_POKE;

//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�@480byte
//--------------------------------------------------------------
typedef struct
{
  u32 bgNo   :5;  //�w�i�ԍ�
  u32 spotBit:4;  //�X�|�b�g���C�g�Ώ�(bit)(�g�b�v�������|�P����
  u32 year   :7;  //�N
  u32 month  :5;  //��  ���ꂪ�O�������疳���f�[�^�Ƃ݂Ȃ�
  u32 day    :6;  //��
  u32 player :2;  //�����̔ԍ�
  u32 musVer :3;  //�~���[�W�J���o�[�W����

  MUSICAL_SHOT_POKE shotPoke[MUSICAL_POKE_MAX]; //�|�P�����f�[�^(�S��
  STRCODE title[MUSICAL_PROGRAM_NAME_MAX];  //�~���[�W�J���^�C�g��(���{18�E�C�O36�{EOM
  u8 pmVersion;   //PM_VERSION
  u8 pmLang;      //PM_LANG
}MUSICAL_SHOT_DATA;

#endif  //__GT_GDS_MUSICAL_SUB_H__
