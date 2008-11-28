//======================================================================
/**
 * @file	field_comm_func.c
 * @brief	�t�B�[���h�ʐM�@�ʐM�@�\��
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================

#ifndef FIELD_COMM_FUNC_H__
#define FIELD_COMM_FUNC_H__

#include "field_comm_def.h"
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//�ʐM���
typedef enum
{
	FIELD_COMM_MODE_NONE,	//�ʐM�Ȃ�
	FIELD_COMM_MODE_WAIT,		//�N���҂�
	FIELD_COMM_MODE_SEARCH,		//�N����T����
	FIELD_COMM_MODE_CONNECT,	//�N����
	FIELD_COMM_MODE_TRY_CONNECT,	//�q�@�ƂȂ�e�֐ڑ������݂�
}FIELD_COMM_MODE;

typedef enum
{
	FCRT_PROFILE,	//�L�����̊�{���
}F_COMM_REQUEST_TYPE;

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

extern FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID );
extern void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc );

extern void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_InitCommData_StartTalk( FIELD_COMM_FUNC *commFunc );
//���M�p�֐�
extern const BOOL	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc );
extern const BOOL	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc );
extern const BOOL	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc );
extern const BOOL	FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID );
extern const BOOL	FIELD_COMM_FUNC_Send_UserData( F_COMM_USERDATA_TYPE type , const u8 sendID );
//�����p�R�}���h
extern void FIELD_COMM_FUNC_Send_SyncCommand( const F_COMM_SYNC_TYPE type , FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_CheckSyncFinish( const F_COMM_SYNC_TYPE type , const u8 checkBit , FIELD_COMM_FUNC *commFunc );
extern const u8 FIELD_COMM_FUNC_GetBit_TalkMember( FIELD_COMM_FUNC *commFunc );
//�e��`�F�b�N�֐�
extern const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
extern const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc );
extern const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc );
extern const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsReserveTalk( FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_ResetReserveTalk( FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc );
extern const F_COMM_ACTION_LIST FIELD_COMM_FUNC_GetSelectAction( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsActionReturn( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_GetActionReturn( FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_ResetGetUserData( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsGetUserData( FIELD_COMM_FUNC *commFunc );


#endif //FIELD_COMM_FUNC_H__

