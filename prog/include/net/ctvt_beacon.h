//======================================================================
/**
 * @file	ctvt_beacon.c
 * @brief	�ʐM�g�����V�[�o�[�F�r�[�R��
 * @author	ariizumi
 * @data	09/12/22
 *
 * ���W���[�����FCTVT_BCON
 */
//======================================================================
#include "buflen.h"
#include "savedata/wifilist.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define CTVT_COMM_NAME_LEN (SAVELEN_PLAYER_NAME+EOM_SIZE)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
//�r�[�R���f�[�^
typedef struct _CTVT_COMM_BEACON CTVT_COMM_BEACON;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//���g���Ă΂�Ă��邩�`�F�b�N
const BOOL CTVT_BCON_CheckCallSelf( CTVT_COMM_BEACON *beacon , WIFI_LIST *wifiList , u8* selfMacAdr );

//�r�[�R��������擾(MyStatus�̃��b�p�[
const STRCODE* CTVT_BCON_GetName( CTVT_COMM_BEACON *beacon );
const u16 CTVT_BCON_GetIDLow( CTVT_COMM_BEACON *beacon );
