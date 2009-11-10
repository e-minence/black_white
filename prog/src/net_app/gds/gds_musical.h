//==============================================================================
/**
 *
 * �~���[�W�J���V���b�g
 *
 */
//==============================================================================
#ifndef __GT_GDS_MUSICAL_H__
#define __GT_GDS_MUSICAL_H__

#include "gds_define.h"
#include "gds_profile_local.h"


//==============================================================================
//  �萔��`
//==============================================================================
//�~���[�W�J���|�P�����̑����ӏ����ʗp
//���[�U�[���F���ł���ӏ��B
typedef enum
{
	GT_MUS_POKE_EQU_USER_EAR_R,	//��(�E��)
	GT_MUS_POKE_EQU_USER_EAR_L,	//��(����)
	GT_MUS_POKE_EQU_USER_HEAD,		//��
	GT_MUS_POKE_EQU_USER_FACE,		//��
	GT_MUS_POKE_EQU_USER_BODY,		//��
	GT_MUS_POKE_EQU_USER_HAND_R,	//��(�E��)
	GT_MUS_POKE_EQU_USER_HAND_L,	//��(����)

	GT_MUS_POKE_EQUIP_USER_MAX,
	
	GT_MUS_POKE_EQU_USER_INVALID,	//�����l
}GT_MUS_POKE_EQUIP_USER;

///�~���[�W�J���̃g���[�i�[�l�[����
#define GT_MUSICAL_TRAINER_NAME_LEN      (7+1) //�v���C���[���{EOM

//�~���[�W�J���ɎQ���\�Ȑl��
#define GT_MUSICAL_POKE_MAX (4)

//�~���[�W�J���ő����ł���A�C�e���̌�
#define GT_MUSICAL_ITEM_EQUIP_MAX (GT_MUS_POKE_EQUIP_USER_MAX)	//�灕���E���E�E��E����

//�~���[�W�J���̉��ږ��̕�����
#define GT_MUSICAL_PROGRAM_NAME_MAX (36+EOM_SIZE) //���{18�E�C�O36�{EOM


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
}GT_MUSICAL_SHOT_POKE_EQUIP;

//--------------------------------------------------------------
//  �|�P�����P�̂̃f�[�^�@76byte
//--------------------------------------------------------------
typedef struct
{
  u16 monsno; //�|�P�����ԍ�
  u16  sex :2; //����
  u16  rare:1; //���A�t���O
  u16  form:5; //�t�H�����ԍ�
  u16  pad :8;
  
  GT_STRCODE trainerName[GT_MUSICAL_TRAINER_NAME_LEN];  //�g���[�i�[��
  GT_MUSICAL_SHOT_POKE_EQUIP equip[GT_MUSICAL_ITEM_EQUIP_MAX];  //�����O�b�Y�f�[�^(�V��
  
}GT_MUSICAL_SHOT_POKE;

//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�@384byte
//--------------------------------------------------------------
typedef struct
{
  u32 bgNo   :5;  //�w�i�ԍ�
  u32 spotBit:4;  //�X�|�b�g���C�g�Ώ�(bit)
  u32 year   :7;  //�N
  u32 month  :5;  //��  ���ꂪ�O�������疳���f�[�^�Ƃ݂Ȃ�
  u32 day    :6;  //��
  u32 pad    :5;
  
  GT_STRCODE title[GT_MUSICAL_PROGRAM_NAME_MAX];  //�~���[�W�J���^�C�g��(���{18�E�C�O36�{EOM
  GT_MUSICAL_SHOT_POKE shotPoke[GT_MUSICAL_POKE_MAX]; //�|�P�����f�[�^(�S��
}GT_MUSICAL_SHOT_DATA;


//--------------------------------------------------------------
/**
 *	���M�f�[�^�F�~���[�W�J���V���b�g
 *		512byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;
	GT_MUSICAL_SHOT_DATA mus_shot;
}GT_MUSICAL_SHOT_SEND;

//--------------------------------------------------------------
/**
 *	��M�f�[�^�F�~���[�W�J���V���b�g
 *		512byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;
	GT_MUSICAL_SHOT_DATA mus_shot;
}GT_MUSICAL_SHOT_RECV;


#endif  //__GT_GDS_MUSICAL_H__
