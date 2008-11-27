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

#include "dlplay_func.h"

#define TEST_DATA_LINE (16)
#define TEST_DATA_NUM  (256)

#define SELECT_BOX_INVALID (0xFF)

//�{�b�N�X1����PPP�f�[�^�T�C�Y
#define LARGEPACKET_POKE_SIZE (30*0x88)
typedef struct _DLPLAY_COMM_DATA DLPLAY_COMM_DATA;
//���M�p�f�[�^
typedef struct
{
	u8 pokeData_[LARGEPACKET_POKE_SIZE];
}DLPLAY_LARGE_PACKET;

//���M�p BOX�C���f�b�N�X
#define BOX_TRAY_NUM (18)
#define BOX_MONS_NUM (30)
#define BOX_TRAY_NAME_LEN (20)
#define BOX_MONS_NAME_LEN (10+1)
typedef struct
{
	u16 name_[BOX_MONS_NAME_LEN];
	u16 pokeNo_:9;
	u16	lv_:7;
	u8	sex_:2;
	u8	form_:5;
	u8	isEgg_:1;	//���̒i�K�ł͕s���ȗ����ǂ����͔��f���Ȃ�
	u8	rare_:1;	//���A���ǂ����H
	u8	padding_:7;
}DLPLAY_MONS_INDEX;
typedef struct
{
	u16	boxName_[BOX_TRAY_NUM][BOX_TRAY_NAME_LEN];
	DLPLAY_MONS_INDEX pokeData_[BOX_TRAY_NUM][BOX_MONS_NUM];
}DLPLAY_BOX_INDEX;

//�{�̂Ɏh�����Ă���J�[�h�̎��
typedef enum {
	CARD_TYPE_DP,		//�_�C�A�����h���p�[��
	CARD_TYPE_PT,		//�v���`�i
	CARD_TYPE_GS,		//�S�[���h���V���o�[
	CARD_TYPE_INVALID,
}DLPLAY_CARD_TYPE;

//�ėp�t���O���M�p�t���O
enum DLPLAY_COMMAND_FLG_TYPE
{
	DC_FLG_FINISH_SAVE1_PARENT,	//�e�@��1�Z�[�u����
	DC_FLG_FINISH_SAVE1_CHILD,	//�q�@��1�Z�[�u����
	DC_FLG_PERMIT_LASTBIT_SAVE,	//�ŏI�r�b�g�Z�[�u�J�n
	DC_FLG_FINISH_SAVE_ALL,		//�S�Z�[�u����
	DC_FLG_POST_BOX_DATA,		//�e���q�{�b�N�X�f�[�^��M�m�F
};

extern DLPLAY_COMM_DATA* DLPlayComm_InitData( u32 heapID );
extern void	DLPlayComm_TermData( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_InitSystem( DLPLAY_COMM_DATA *d_comm);
extern void	DLPlayComm_TermSystem( DLPLAY_COMM_DATA *d_comm );
#if DLPLAY_FUNC_USE_PRINT
extern void	DLPlayComm_SetMsgSys( DLPLAY_MSG_SYS *msgSys , DLPLAY_COMM_DATA *d_comm );
#endif
extern void	DLPlayComm_InitParent( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_InitChild( DLPLAY_COMM_DATA *d_comm , u8 *macAddress );
extern BOOL	DLPlayComm_IsFinish_Negotiation( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsFinish_InitSystem( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsFinish_TermSystem( DLPLAY_COMM_DATA *d_comm );
extern BOOL DLPlayComm_IsConnect( DLPLAY_COMM_DATA *d_comm );
extern BOOL DLPlayComm_IsStartPostIndex( DLPLAY_COMM_DATA *d_comm );
extern BOOL DLPlayComm_IsPostIndex( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsPost_SendIndex( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsPostData( DLPLAY_COMM_DATA *d_comm );
extern BOOL	DLPlayComm_IsPost_SendData( DLPLAY_COMM_DATA *d_comm );
extern const u8 DLPlayComm_GetSelectBoxNumber( DLPLAY_COMM_DATA *d_comm );
extern const u8 DLPlayComm_GetPostErrorState( DLPLAY_COMM_DATA *d_comm );
extern const u16	DLPlayComm_IsFinishSaveFlg( u8 flg , DLPLAY_COMM_DATA *d_comm );

extern DLPLAY_LARGE_PACKET*	DLPlayComm_GetLargePacketBuff( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_LargeData( DLPLAY_COMM_DATA *d_comm );
extern DLPLAY_BOX_INDEX* DLPlayComm_GetBoxIndexBuff( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_BoxIndex( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_ConnectSign( DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_BoxNumber( u8 idx , DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_ErrorState( u8 type , DLPLAY_COMM_DATA *d_comm );
extern void	DLPlayComm_Send_CommonFlg( u8 flg , u16 value , DLPLAY_COMM_DATA *d_comm );

#endif // DLPALY_COMM_FUNC__

