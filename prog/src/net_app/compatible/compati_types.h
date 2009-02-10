//==============================================================================
/**
 * @file	compati_types.h
 * @brief	�����`�F�b�N�̋��ʃw�b�_
 * @author	matsuda
 * @date	2009.02.10(��)
 */
//==============================================================================
#ifndef __COMPATI_TYPES_H__
#define __COMPATI_TYPES_H__

#include <gflib.h>


//==============================================================================
//	�萔��`
//==============================================================================
///�~�A�N�^�[�𓯎��ɏo���鐔
#define CC_CIRCLE_MAX	(8)

///�~�̃^�C�v	���A�j���̃V�[�P���X�ԍ��ƕ��т𓯂��ɂ��Ă����ƁI
enum{
	CC_CIRCLE_TYPE_XL,
	CC_CIRCLE_TYPE_L,
	CC_CIRCLE_TYPE_M,
	CC_CIRCLE_TYPE_S,
	
	CC_CIRCLE_TYPE_NULL,	//�f�[�^�I�[�R�[�h�Ƃ��Ďg�p
};

//==============================================================================
//	�^��`
//==============================================================================
///�~�̃p�����[�^
typedef struct{
	u8 type;			//�~�̎��(CC_CIRCLE_TYPE_NULL�̏ꍇ�̓f�[�^�I�[)
	u8 x;
	u8 y;
	u8 point_limit;		//�_���̏��
}CC_CIRCLE_PARAM;

///����Ŏg�p����~�p�����[�^�̉�
typedef struct{
	CC_CIRCLE_PARAM data[CC_CIRCLE_MAX];
}CC_CIRCLE_PACKAGE;

///�R���g���[��PROC����qPROC�֓n��ParentWork
typedef struct{
	CC_CIRCLE_PACKAGE circle_package;	///<�T�[�N���p�b�P�[�W
	
	u8 partner_macAddress[6];			///<�ʐM�����MacAddress
	u8 padding[2];
}COMPATI_PARENTWORK;


#endif	//__COMPATI_TYPES_H__
