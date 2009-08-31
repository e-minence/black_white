//======================================================================
/**
 * @file  musical_program.h
 * @brief �~���[�W�J���p ����
 * @author  ariizumi
 * @data  09/08/21
 *
 * ���W���[�����FMUSICAL_PROGRAM
 */
//======================================================================

#pragma once
#include "musical/musical_stage_sys.h"
#include "musical/mus_item_data.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//�R���f�B�V�����l�̍��v
#define MUSICAL_PROGRAM_CONDITION_SUM (200)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _MUSICAL_PROGRAM_DATA MUSICAL_PROGRAM_DATA;
typedef struct _MUSICAL_PROGRAM_WORK MUSICAL_PROGRAM_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
MUSICAL_PROGRAM_WORK* MUSICAL_PROGRAM_GetProgramData( HEAPID heapId );
void MUSICAL_PROGRAM_CalcPokemonPoint( HEAPID heapId , MUSICAL_PROGRAM_WORK* progWork , STAGE_INIT_WORK *actInitWork );
const u8 MUSICAL_PROGRAM_GetConditionPoint( MUSICAL_PROGRAM_WORK* progWork , MUSICAL_CONDITION_TYPE conType );
