//======================================================================
/**
 *
 * @file	dlplay_data_main.c
 * @brief	�Z�[�u�f�[�^��ǂݍ��ނƂ���
 * @author	ariizumi
 * @data	
 */
//======================================================================
#include <gflib.h>
#include <backup_system.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "savedata_def.h"

#include "../../pokemon_wb/prog/src/test/ariizumi/ari_debug.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_func.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_comm_func.h"

#include "dlplay_child_common.h"
#include "dlplay_data_main.h"
#include "dlplay_child_pt.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

DLPLAY_DATA_DATA* DLPlayData_InitSystem( int heapID , DLPLAY_MSG_SYS *msgSys );
void			  DLPlayData_TermSystem( DLPLAY_DATA_DATA *d_data );

BOOL	DLPlayData_LoadDataFirst( DLPLAY_DATA_DATA *d_data );
BOOL	DLPlayData_SaveData( DLPLAY_DATA_DATA *d_data );

//������
DLPLAY_DATA_DATA* DLPlayData_InitSystem( int heapID , DLPLAY_MSG_SYS *msgSys )
{
	DLPLAY_DATA_DATA *d_data;

	d_data = GFL_HEAP_AllocClearMemory(  heapID, sizeof( DLPLAY_DATA_DATA ) );

	d_data->heapID_ = heapID;

	d_data->mainSeq_ = 0;
	d_data->subSeq_	 = 0;
	d_data->msgSys_	= msgSys;
	d_data->pBoxData_ = NULL;
	MATH_CRC16CCITTInitTable( &d_data->crcTable_ );	//CRC������

	{
		//ROM�̎��ʂ��s��
		CARDRomHeader *headerData;
		s32 lockID = OS_GetLockID();

		CARD_Enable( TRUE );
		GF_ASSERT( lockID != OS_LOCK_ID_ERROR );
		CARD_LockRom( (u16)lockID );
		headerData = (CARDRomHeader*)CARD_GetRomHeader();
		CARD_UnlockRom( (u16)lockID );
		OS_ReleaseLockID( (u16)lockID );
		{
			char str[64];
			sprintf(str,"name:[%12s] code:[%08x]",headerData->game_name ,headerData->game_code);
			DLPlayFunc_PutString(str,d_data->msgSys_);
		}
		//FIX ME:���������ʏ���������
		d_data->cardType_ = CARD_TYPE_PT;


		//�ꉞ�J�[�h�̎�ނ��m�肵�Ă��珉����
		GFL_BACKUP_Init(GFL_HEAPID_APP, GFL_HEAPID_APP);
	}
	
	return d_data;
}

//�J��
void	DLPlayData_TermSystem( DLPLAY_DATA_DATA *d_data )
{

}

//�Z�[�u�f�[�^�̓ǂݍ���
BOOL	DLPlayData_LoadDataFirst( DLPLAY_DATA_DATA *d_data )
{
	//���O�œǂݍ��ށE�E�E
	switch( d_data->cardType_ )
	{
	case CARD_TYPE_DP:
		break;

	case CARD_TYPE_PT:
		return DLPlayData_PT_LoadData( d_data );
		break;

	case CARD_TYPE_GS:
		break;
	}
	return FALSE;
}

BOOL	DLPlayData_SaveData( DLPLAY_DATA_DATA *d_data )
{
		//���O�ŏ������ށE�E�E
	switch( d_data->cardType_ )
	{
	case CARD_TYPE_DP:
		break;

	case CARD_TYPE_PT:
		return DLPlayData_PT_SaveData( d_data );
		break;

	case CARD_TYPE_GS:
		break;
	}
	return FALSE;
}





