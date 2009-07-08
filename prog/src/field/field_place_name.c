////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.c
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @data   
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "field_place_name.h"


//===================================================================================
/**
 * �V�X�e���E���[�N
 */
//===================================================================================
struct _FIELD_PLACE_NAME
{
	int temp;
};


FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id )
{
	FIELD_PLACE_NAME* p_sys;

	// �V�X�e���E���[�N�쐬
	p_sys = (FIELD_PLACE_NAME*)GFL_HEAP_AllocClearMemory( heap_id, sizeof( FIELD_PLACE_NAME ) );

	// �쐬�����V�X�e����Ԃ�
	return p_sys;
}

void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys ){}
void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys ){}
void FIELD_PLACE_NAME_Destroy( FIELD_PLACE_NAME* p_sys )
{
	// �V�X�e���E���[�N���
	GFL_HEAP_FreeMemory( p_sys );
}
