//======================================================================
/**
 * @file	plist_comm.h
 * @brief	�|�P�������X�g�ʐM(�o�g����������
 * @author	ariizumi
 * @data	10/03/10
 *
 * ���W���[�����FPLIST_COMM
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define PLIST_COMM_TIMMIN_INIT_LIST (1) //�J�n���E�R�}���h�̒ǉ�
#define PLIST_COMM_TIMMIN_EXIT_LIST (2) //�I�����E�R�}���h�̍폜
//�������̏����Ń`�F�b�N����悤��
#define PLIST_COMM_TIMMIN_EXIT_LIST_TEMP (128) //�I�����E�R�}���h�̍폜

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  PCFT_FINISH_SELECT,
}PLIST_COMM_FLG_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern void PLIST_COMM_InitComm( PLIST_WORK *work );
extern void PLIST_COMM_ReqExitComm( PLIST_WORK *work );
extern const BOOL PLIST_COMM_IsExitComm( PLIST_WORK *work );
extern void PLIST_COMM_UpdateComm( PLIST_WORK *work );

extern const BOOL PLIST_COMM_SendFlg( PLIST_WORK *work , const u8 flgType , const u16 flgValue );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
