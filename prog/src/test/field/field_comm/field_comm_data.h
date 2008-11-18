//======================================================================
/**
 * @file	field_comm_data.h
 * @brief	�ʐM�ŋ��L����f�[�^(�e�L�����̍��W�Ƃ�)
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================

#include "gamesystem/playerwork.h"

#ifndef FIELD_COMM_DATA_H__
#define FIELD_COMM_DATA_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	FCCS_NONE,		//���݂Ȃ�
	FCCS_CONNECT,	//�ڑ�����
	FCCS_REQ_DATA,	//�L�����ʃf�[�^��M��
	FCCS_EXIST_DATA,	//�L�����ʃf�[�^��M��
	FCCS_FIELD,		//�t�B�[���h��
}F_COMM_CHARA_STATE;

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================
extern void FIELD_COMM_DATA_InitSystem( HEAPID heapID );
extern void	FIELD_COMM_DATA_TermSystem();
extern BOOL	FIELD_COMM_DATA_IsExistSystem(void);
extern void	FIELD_COMM_DATA_SetFieldCommMode( const u8 mode );
extern const u8 FIELD_COMM_DATA_GetFieldCommMode(void);

extern void	FIELD_COMM_DATA_SetSelfData_Pos( const ZONEID *zoneID , const VecFx32 *pos , const u16 *dir );
extern PLAYER_WORK*	FIELD_COMM_DATA_GetSelfData_PlayerWork(void);

extern void	FIELD_COMM_DATA_SetCharaData_IsExist( const idx , const BOOL );
extern void	FIELD_COMM_DATA_SetCharaData_IsValid( const idx , const BOOL );
extern const BOOL	FIELD_COMM_DATA_GetCharaData_IsExist( const idx );
extern const BOOL	FIELD_COMM_DATA_GetCharaData_IsValid( const idx );
extern const F_COMM_CHARA_STATE FIELD_COMM_DATA_GetCharaData_State( const idx );
extern void FIELD_COMM_DATA_SetCharaData_State( const idx , const F_COMM_CHARA_STATE state );
extern PLAYER_WORK*	FIELD_COMM_DATA_GetCharaData_PlayerWork( const idx );



#endif //FIELD_COMM_DATA_H__

