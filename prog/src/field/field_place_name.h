////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.h
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @data   
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>

//------------------------------------------------------------------------------
/**
 * �f�[�^�^�錾
 */
//------------------------------------------------------------------------------
typedef struct _FIELD_PLACE_NAME FIELD_PLACE_NAME;


//------------------------------------------------------------------------------
/**
 * �v���g�^�C�v�錾
 */
//------------------------------------------------------------------------------
extern FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id );
extern void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys );
extern void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys );
extern void FIELD_PLACE_NAME_Destroy( FIELD_PLACE_NAME* p_sys );
