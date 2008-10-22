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

typedef struct _DLPLAY_COMM_DATA DLPLAY_COMM_DATA;
//���M�p�f�[�^
typedef struct
{
	//�|�P�����̃f�[�^��1�C100�o�C�g���炢�炵��
	char data_[16][256];
}DLPLAY_LARGE_PACKET;


extern DLPLAY_COMM_DATA* DLPlayComm_InitData( u32 heapID );
extern void	DLPlayComm_TermSystem( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_InitSystem( DLPLAY_COMM_DATA *d_comm);

extern void	DLPlayComm_InitParent( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_InitChild( DLPLAY_COMM_DATA *d_comm , u8 *macAddress );
extern BOOL	DLPlayComm_IsFinish_ConnectParent( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsFinish_InitSystem( DLPLAY_COMM_DATA *d_comm );

void	DLPlayComm_Send_LargeData( DLPLAY_LARGE_PACKET *data , DLPLAY_COMM_DATA *d_comm );

#endif // DLPALY_COMM_FUNC__

