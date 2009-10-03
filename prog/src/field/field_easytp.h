//======================================================================
/**
 *
 * @file	field_easytp.h
 * @brief	�t�B�[���h�ȈՃ^�b�`�p�l��
 * @author	kagaya
 * @date	08.09.30
 */
//======================================================================
#ifndef __FIELD_EASYTP_H__
#define __FIELD_EASYTP_H__

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	FLDEASYTP_TCHDIR	�����ꂽ������\��
//--------------------------------------------------------------
typedef enum
{
	FLDEASYTP_TCHDIR_UP = 0,	//��
	FLDEASYTP_TCHDIR_DOWN,		//�� 
	FLDEASYTP_TCHDIR_LEFT,		//��
	FLDEASYTP_TCHDIR_RIGHT,		//�E
	FLDEASYTP_TCHDIR_LEFTUP,	//����
	FLDEASYTP_TCHDIR_LEFTDOWN,	//����
	FLDEASYTP_TCHDIR_RIGHTUP,	//�E��
	FLDEASYTP_TCHDIR_RIGHTDOWN,	//�E��
	FLDEASYTP_TCHDIR_CENTER,	//���S
	FLDEASYTP_TCHDIR_NON,		//������Ă��Ȃ�
	
	FLDEASYTP_TCHDIR_MAX = FLDEASYTP_TCHDIR_NON,	//�ő�
}FLDEASYTP_TCHDIR;

//======================================================================
//	typedef struct
//======================================================================
//typedef struct _FLD_EASYTP FLD_EASYTP;

//======================================================================
//	extern
//======================================================================
extern FLDEASYTP_TCHDIR FieldEasyTP_TouchDirGet( void );

#endif //__FIELD_EASYTP_H__
