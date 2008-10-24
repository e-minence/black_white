//======================================================================
/**
 *
 * @file	dlplay_comm_func.c	
 * @brief	DL�ʐM �e�q���ʂ̒ʐM�֌W
 * @author	ariizumi
 * @data	08/10/21
 */
//======================================================================

#ifndef DLPALY_COMM_FUNC__
#define DLPALY_COMM_FUNC__

#define TEST_DATA_LINE (16)
#define TEST_DATA_NUM  (256)
typedef struct _DLPLAY_COMM_DATA DLPLAY_COMM_DATA;
//���M�p�f�[�^
typedef struct
{
	//�|�P�����̃f�[�^��1�C100�o�C�g���炢�炵��
	char data_[TEST_DATA_LINE][TEST_DATA_NUM];
}DLPLAY_LARGE_PACKET;

//�{�̂Ɏh�����Ă���J�[�h�̎��
typedef enum {
	CARD_TYPE_DP,		//�_�C�A�����h���p�[��
	CARD_TYPE_PT,		//�v���`�i
	CARD_TYPE_GS,		//�S�[���h���V���o�[
	CARD_TYPE_INVALID,
}DLPLAY_CARD_TYPE;

extern DLPLAY_COMM_DATA* DLPlayComm_InitData( u32 heapID );
extern void	DLPlayComm_TermSystem( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_InitSystem( DLPLAY_COMM_DATA *d_comm);

extern void	DLPlayComm_InitParent( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_InitChild( DLPLAY_COMM_DATA *d_comm , u8 *macAddress );
extern BOOL	DLPlayComm_IsFinish_ConnectParent( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsFinish_InitSystem( DLPLAY_COMM_DATA *d_comm );

void	DLPlayComm_Send_LargeData( DLPLAY_LARGE_PACKET *data , DLPLAY_COMM_DATA *d_comm );
void	DLPlayComm_Send_ConnectSign( DLPLAY_COMM_DATA *d_comm );

#endif // DLPALY_COMM_FUNC__

