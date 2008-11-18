//======================================================================
/**
 * @file	field_comm_data.c
 * @brief	�ʐM�ŋ��L����f�[�^(�e�L�����̍��W�Ƃ�)
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/playerwork.h"
#include "field_comm_main.h"
#include "field_comm_data.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
//�L�����l�̃f�[�^
typedef struct
{
	BOOL	isExist_;	//�L���������݂���
	BOOL	isValid_;	//�f�[�^���L��
	F_COMM_CHARA_STATE	state_;
	PLAYER_WORK plWork_;
}FIELD_COMM_CHARA_DATA;

typedef struct 
{
	u8	backupCommState_;	//FieldCommFunc�̃X�e�[�g�̃o�b�N�A�b�v
	FIELD_COMM_CHARA_DATA selfData_;	//���g�̃f�[�^�B
	FIELD_COMM_CHARA_DATA charaData_[FIELD_COMM_MEMBER_MAX];
}FIELD_COMM_DATA;

//�����ēƗ����Ċm�ۂ�����
FIELD_COMM_DATA *commData = NULL;

//======================================================================
//	proto
//======================================================================
void	FIELD_COMM_DATA_InitSystem( HEAPID heapID );
void	FIELD_COMM_DATA_TermSystem(void);
BOOL	FIELD_COMM_DATA_IsExistSystem(void);

void	FIELD_COMM_DATA_SetFieldCommMode( const u8 mode );
const u8 FIELD_COMM_DATA_GetFieldCommMode(void);

void	FIELD_COMM_DATA_SetSelfData_PlayerWork( const PLAYER_WORK *plWork );
void	FIELD_COMM_DATA_SetSelfDataPos( const ZONEID zoneID , const VecFx32 *pos , const u16 *dir );
PLAYER_WORK* FIELD_COMM_DATA_GetSelfData_PlayerWork(void);

void	FIELD_COMM_DATA_SetCharaData_IsExist( const idx , const BOOL );
void	FIELD_COMM_DATA_SetCharaData_IsValid( const idx , const BOOL );
const BOOL	FIELD_COMM_DATA_GetCharaData_IsExist( const idx );
const BOOL	FIELD_COMM_DATA_GetCharaData_IsValid( const idx );
const F_COMM_CHARA_STATE FIELD_COMM_DATA_GetCharaData_State( const idx );
void FIELD_COMM_DATA_SetCharaData_State( const idx , const F_COMM_CHARA_STATE state );
PLAYER_WORK* FIELD_COMM_DATA_GetCharaData_PlayerWork( const idx);

static void FIELD_COMM_DATA_InitOneCharaData( FIELD_COMM_CHARA_DATA *charaData );

//--------------------------------------------------------------
//	�ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
void FIELD_COMM_DATA_InitSystem( HEAPID heapID )
{
	u8 i;
	//GF_ASSERT( commData == NULL );
	if( commData == NULL )
	{
		//�ŏ��̓q�[�v�̊m�ۂƑS������
		commData = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_DATA) );
		for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
		{
			FIELD_COMM_DATA_InitOneCharaData( &commData->charaData_[i] );
		}
	}
	else
	{
		//�f�[�^�����łɂ���Ƃ���exist�����؂��Ă���(COMM_ACTOR�Đ����̂���
		for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
		{
			commData->charaData_[i].isExist_ = FALSE;
		}
	}
}

//--------------------------------------------------------------
//	�ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
void	FIELD_COMM_DATA_TermSystem( void )
{
	GF_ASSERT( commData != NULL );
	GFL_HEAP_FreeMemory( commData );
	commData = NULL;
}

//--------------------------------------------------------------
//	�f�[�^�����݂��邩�H
//--------------------------------------------------------------
BOOL	FIELD_COMM_DATA_IsExistSystem(void)
{
	return( commData != NULL );
}

//--------------------------------------------------------------
//	FieldCommFunc�̃X�e�[�g�̃o�b�N�A�b�v
//--------------------------------------------------------------
void	FIELD_COMM_DATA_SetFieldCommMode( const u8 state )
{
	GF_ASSERT( commData != NULL );
	commData->backupCommState_ = state;
}
const u8 FIELD_COMM_DATA_GetFieldCommMode(void)
{
	GF_ASSERT( commData != NULL );
	return commData->backupCommState_;
}

//--------------------------------------------------------------
//	�����̃f�[�^�Z�b�g
//--------------------------------------------------------------
void	FIELD_COMM_DATA_SetSelfData_Pos( const ZONEID *zoneID , const VecFx32 *pos , const u16 *dir )
{
	GF_ASSERT( commData != NULL );
	PLAYERWORK_setZoneID( &commData->selfData_.plWork_ , *zoneID );
	PLAYERWORK_setPosition( &commData->selfData_.plWork_ , pos );
	PLAYERWORK_setDirection( &commData->selfData_.plWork_ , *dir );
}
//--------------------------------------------------------------
//	�����̃f�[�^�擾
//--------------------------------------------------------------
PLAYER_WORK*	FIELD_COMM_DATA_GetSelfData_PlayerWork( void )
{
	GF_ASSERT( commData != NULL );
	return &commData->selfData_.plWork_;
}

//--------------------------------------------------------------
//	�w��ԍ��̃t���O�擾�E�ݒ�
//--------------------------------------------------------------
void	FIELD_COMM_DATA_SetCharaData_IsExist( const idx , const BOOL flg)
{
	commData->charaData_[idx].isExist_ = flg;
}
void	FIELD_COMM_DATA_SetCharaData_IsValid( const idx , const BOOL flg)
{
	commData->charaData_[idx].isValid_ = flg;
}
const BOOL	FIELD_COMM_DATA_GetCharaData_IsExist( const idx )
{
	return commData->charaData_[idx].isExist_;
}
const BOOL	FIELD_COMM_DATA_GetCharaData_IsValid( const idx )
{
	return commData->charaData_[idx].isValid_;
}

//--------------------------------------------------------------
//	�w��ԍ��̃X�e�[�^�X�擾
//--------------------------------------------------------------
const F_COMM_CHARA_STATE FIELD_COMM_DATA_GetCharaData_State( const idx )
{
	return commData->charaData_[idx].state_;
}
void FIELD_COMM_DATA_SetCharaData_State( const idx , const F_COMM_CHARA_STATE state )
{
	commData->charaData_[idx].state_ = state;
}

//--------------------------------------------------------------
//	�w��ԍ��̃f�[�^�擾
//--------------------------------------------------------------
PLAYER_WORK*	FIELD_COMM_DATA_GetCharaData_PlayerWork( const idx )
{
	GF_ASSERT( commData != NULL );
	return &commData->charaData_[idx].plWork_;
}

//--------------------------------------------------------------
//	�ʐM�p�f�[�^�Ǘ�������
//--------------------------------------------------------------
static void FIELD_COMM_DATA_InitOneCharaData( FIELD_COMM_CHARA_DATA *charaData )
{
	charaData->isExist_ = FALSE;
	charaData->isValid_ = FALSE;
	charaData->state_ = FCCS_NONE;
	PLAYERWORK_init( &charaData->plWork_ );
}

