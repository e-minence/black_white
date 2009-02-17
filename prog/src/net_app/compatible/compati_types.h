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
///���h�b�g��1�_�Ƃ��邩
#define CC_POINT_DOT		80//(40)

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

///�~�̒͂ݏ�
enum{
	CIRCLE_TOUCH_NULL,		///<�~��͂�ł��Ȃ�
	CIRCLE_TOUCH_HOLD,		///<�~��͂�ł���
	CIRCLE_TOUCH_OUTSIDE,	///<�͂�ł��邪�͈͊O���W���w���Ă���
};

///�Q�[�����[�h
enum{
	COMPATI_GAMEMODE_START,		///<�Q�[���J�n���
	COMPATI_GAMEMODE_GAME,		///<�Q�[����
	COMPATI_GAMEMODE_RESULT,	///<���ʔ��\���
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
	u8 game_mode;						///<COMPATI_GAMEMODE_???
	u8 point;							///<�����̓��_
	u8 partner_point;					///<����̓��_
	u8 padding[3];
	
	s32 irc_time_count_max;
}COMPATI_PARENTWORK;

///�~�ƃ^�b�`�̓����蔻��`�F�b�N�p���[�N
typedef struct{
	s32 old_len_x;
	s32 old_len_y;
	s32 total_len;		///������

	u8 touch_hold;		///<�~�̒͂ݏ󋵁FCIRCLE_TOUCH_???
	u8 hold_circle_no;	///<�͂�ł���~�̔ԍ�
	u8 padding[2];
}CCT_TOUCH_SYS;

///�Q�[�����̐ԊO���ڑ��Ǘ����[�N
typedef struct{
	u8 seq;
	u8 shutdown_req;
	u8 connect;
	u8 padding;
	s16 wait;
	s16 timer;
}COMPATI_IRC_SYS;


//--------------------------------------------------------------
//	�ʐM�Ō����������f�[�^
//--------------------------------------------------------------
///����ڑ����Ɍ����������f�[�^
typedef struct{
	CC_CIRCLE_PACKAGE circle_package;	///<�T�[�N���p�b�P�[�W
	u8 macAddress[6];					///<�ʐM�����MacAddress
	u8 padding[2];
}CCNET_FIRST_PARAM;

///�Q�[���I����Ɍ����������f�[�^
typedef struct{
	u8 point;							///<���_
	u8 dummy[3];
	s32 irc_time_count_max;
}CCNET_RESULT_PARAM;

///�ʐM�Ǘ����[�N
typedef struct{
	CCNET_FIRST_PARAM *send_first_param;		///<���M�f�[�^�ւ̃|�C���^
	CCNET_FIRST_PARAM *receive_first_param;		///<��M�f�[�^�����
	CCNET_RESULT_PARAM *send_result_param;
	CCNET_RESULT_PARAM *receive_result_param;
	u8 seq;
	u8 receive_ok;
	u8 connect_ok;
	u8 connect_bit;		///<�ڑ����Ă���netID���r�b�g�Ǘ�
	u8 lib_finish;
	u8 padding[3];
}COMPATI_CONNECT_SYS;


#endif	//__COMPATI_TYPES_H__
