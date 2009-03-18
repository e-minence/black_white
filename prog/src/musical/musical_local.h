
//============================================================================================
/**
 * @file	musical_local.h
 * @brief	�~���[�W�J�����p��`
 * @author	ariizumi
 * @date	2008.11.14
 */
//============================================================================================

#ifndef	MUSICAL_LOCAL_H_
#define	MUSICAL_LOCAL_H_

//�~���[�W�J���|�P�����̑����ӏ�(�p���b�g�Ή�
typedef enum
{
	MUS_POKE_PLT_SHADOW		=0x04,	//�e�p���W
	MUS_POKE_PLT_ROTATE		=0x05,	//��]�p���W
	
	MUS_POKE_PLT_HEAD		=0x08,	//��
	MUS_POKE_PLT_EAR_R		=0x09,	//�E��
	MUS_POKE_PLT_EAR_L		=0x0A,	//����
	MUS_POKE_PLT_BODY		=0x0B,	//��
	MUS_POKE_PLT_WAIST		=0x0C,	//��
	MUS_POKE_PLT_HAND_R		=0x0D,	//�E��
	MUS_POKE_PLT_HAND_L		=0x0E,	//����
	MUS_POKE_PLT_TAIL		=0x0F,	//�K��
}MUS_POKE_EQUIP_PLT;

//�p���b�g�ԍ����瑕���ӏ��֕ϊ�
#define MUS_POKE_PLT_TO_POS(val) ((MUS_POKE_EQUIP_POS)(val-MUS_POKE_PLT_HEAD))
//�O���[�v�ԍ������]�֕ϊ�(32����)
//#define MUS_POKE_GRP_TO_ROT(val) (val*(360/32)*65536/360);
#define MUS_POKE_GRP_TO_ROT(val) (val*(65536/32))	//360��ʕ�

#endif	//MUSICAL_LOCAL_H_
