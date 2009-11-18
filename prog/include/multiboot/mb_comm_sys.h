//======================================================================
/**
 * @file	mb_comm_sys.c
 * @brief	�}���`�u�[�g �ʐM�V�X�e��
 * @author	ariizumi
 * @data	09/11/13
 *
 * ���W���[�����FMB_COMM
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCFT_GAMEDATA_SIZE,
  MCFT_GAMEDATA_POST,
}MB_COMM_FLG_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_COMM_WORK MB_COMM_WORK;

typedef struct
{
  int  msgSpeed;
  
}MB_COMM_INIT_DATA;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//�ʐM�m���O
extern MB_COMM_WORK* MB_COMM_CreateSystem( const HEAPID heapId );
extern void MB_COMM_DeleteSystem( MB_COMM_WORK* commWork );
extern void MB_COMM_UpdateSystem( MB_COMM_WORK* commWork );
extern void MB_COMM_InitComm( MB_COMM_WORK* commWork );
extern void MB_COMM_ExitComm( MB_COMM_WORK* commWork );
extern const BOOL MB_COMM_IsInitComm( MB_COMM_WORK* commWork);
extern const BOOL MB_COMM_IsFinishComm( MB_COMM_WORK* commWork );

extern void MB_COMM_InitParent( MB_COMM_WORK* commWork );
extern void MB_COMM_InitChild( MB_COMM_WORK* commWork , u8 *macAddress );

//�ʐM�m����
extern MB_COMM_INIT_DATA* MB_COMM_GetInitData( MB_COMM_WORK* commWork );

//�`�F�b�N�n
extern const BOOL MB_COMM_IsSendEnable( const MB_COMM_WORK* commWork );
extern const BOOL MB_COMM_IsPostInitData( const MB_COMM_WORK* commWork );

//���M�n
extern void MB_COMM_InitSendGameData( MB_COMM_WORK* commWork , void* gameData , u32 size );
extern const BOOL MB_COMM_Send_Flag( MB_COMM_WORK *commWork , const MB_COMM_FLG_TYPE type , const u32 value );
//���M�I���܂Ńf�[�^�̕ێ���
extern const BOOL MB_COMM_Send_InitData( MB_COMM_WORK *commWork , MB_COMM_INIT_DATA *initData );
extern const BOOL MB_COMM_Send_GameData( MB_COMM_WORK *commWork , void *gameData , const u32 size );

