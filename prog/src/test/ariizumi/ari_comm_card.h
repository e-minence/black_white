//======================================================================
/**
 * @file	ari_comm_trade.h
 * @brief	�t�B�[���h�ʐM�e�X�g�p�p�[�g
 * @author	ariizumi
 * @data	08/11/25
 */
//======================================================================

#ifndef ARI_COMM_CARD_H__
#define ARI_COMM_CARD_H__

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
typedef struct _ARI_COMM_CARD_WORK ARI_COMM_CARD_WORK;

//======================================================================
//	proto
//======================================================================
extern ARI_COMM_CARD_WORK* ARI_COMM_CARD_Init( HEAPID heapID );
extern const BOOL ARI_COMM_CARD_Loop( ARI_COMM_CARD_WORK *work );
extern void ARI_COMM_CARD_Term( ARI_COMM_CARD_WORK *work );

extern void ARI_COMM_CARD_GetCardString( char* str , const u8 idx , ARI_COMM_CARD_WORK *work );


#endif //ARI_COMM_CARD_H__
