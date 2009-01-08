//======================================================================
/**
 * @file	trainercard.h
 * @brief	�g���[�i�[�J�[�h
 * @author	ariizumi
 * @data	08/11/25
 */
//======================================================================

#ifndef TRAINERCARD_H__
#define TRAINERCARD_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	CMI_ID_TITLE,		//ID
	CMI_ID_USER,
	CMI_NAME_TITLE,		//���O
	CMI_NAME_USER,
	CMI_MONEY_TITLE,	//������
	CMI_MONEY_USER,
	CMI_BOOK_TITLE,		//�}�ӕC��
	CMI_BOOK_USER,
	CMI_SCORE_TITLE,	//�X�R�A
	CMI_SCORE_USER,	
	CMI_TIME_TITLE,		//�v���C����
	CMI_TIME_USER,
	CMI_DAY_TITLE,		//�v���C�J�n��
	CMI_DAY_USER_YEAR,		//�N
	CMI_DAY_USER_MONTH,		//��
	CMI_DAY_USER_DAY,		//��
	CMI_MAX,
}CARD_MESSAGE_INDEX;


//======================================================================
//	typedef struct
//======================================================================
typedef struct _TRAINER_CARD_WORK TRAINER_CARD_WORK;

//======================================================================
//	proto
//======================================================================
extern void TRAINER_CARD_GetCardString( STRBUF* str , const u8 idx , TRAINER_CARD_WORK *work );


#endif //TRAINERCARD_H__

