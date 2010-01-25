//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc.h
 *	@brief  White Forest  Black City �f�[�^
 *	@author	tomoya takahashi
 *	@date		2009.11.09
 *
 *	���W���[�����FFIELD_WFBC
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

#include "map/dp3format.h"

#include "field_g3d_mapper.h"

#include "field/field_wfbc_data.h"
#include "field/eventdata_system.h"

#include "debug/debugwin_sys.h"

#include "field_status_local.h"
#include "field_buildmodel.h"

#include "fldmmdl.h"
#include "field_camera.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�l�����
//=====================================
typedef enum 
{
  FIELD_WFBC_PEOPLE_STATUS_NONE,    // ���Ȃ�
  FIELD_WFBC_PEOPLE_STATUS_NORMAL,  // �ʏ�
  FIELD_WFBC_PEOPLE_STATUS_AWAY,    // �N���̊X�ւ�����

  FIELD_WFBC_PEOPLE_STATUS_MAX,

} FIELD_WFBC_PEOPLE_STATUS;



//-------------------------------------
///	�u���b�N�z�u���
//=====================================
#define FIELD_WFBC_BLOCK_SIZE_X     (8)
#define FIELD_WFBC_BLOCK_SIZE_Z     (8)
#define FIELD_WFBC_BLOCK_TO_GRID_X(x)  ((x) * 8)
#define FIELD_WFBC_BLOCK_TO_GRID_Z(z)  ((z) * 8)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WFBC���[�N
//=====================================
typedef struct _FIELD_WFBC FIELD_WFBC;

//-------------------------------------
///	�l�����
//=====================================
typedef struct _FIELD_WFBC_PEOPLE FIELD_WFBC_PEOPLE;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�����E�j��
//=====================================
extern FIELD_WFBC* FIELD_WFBC_Create( HEAPID heapID );
extern void FIELD_WFBC_Delete( FIELD_WFBC* p_wk );

//-------------------------------------
///	�S�̏��̎擾
//=====================================
extern void FIELD_WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff );

extern u32 FIELD_WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );
extern const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, u32 npc_id );

// �X�^�C�v
extern FIELD_WFBC_CORE_TYPE FIELD_WFBC_GetType( const FIELD_WFBC* cp_wk );
// ����^�C�v
extern MAPMODE FIELD_WFBC_GetMapMode( const FIELD_WFBC* cp_wk );

// �u���b�N�z�u���
extern int FIELD_WFBC_SetUpBlock( FIELD_WFBC* p_wk, NormalVtxFormat* p_attr, FIELD_BMODEL_MAN* p_bm, GFL_G3D_MAP* g3Dmap, int build_count, u32 block_x, u32 block_z, HEAPID heapID );

// �C�x���g�f�[�^�̏�����
extern void FILED_WFBC_EventDataOverwrite( const FIELD_WFBC* cp_wk, EVENTDATA_SYSTEM* p_evdata, MAPMODE mapmode, HEAPID heapID );

// �J�����ݒ�
extern void FIELD_WFBC_SetUpCamera( const FIELD_WFBC* cp_wk, FIELD_CAMERA* p_camera, HEAPID heapID );

//-------------------------------------
///	�S�̏��̐ݒ�
//=====================================
extern void FIELD_WFBC_SetUp( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_core, MAPMODE mapmode, HEAPID heapID );
extern void FIELD_WFBC_Clear( FIELD_WFBC* p_wk );

extern void FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_core );
extern void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, u32 npc_id );

// �A��čs���ꂽ�ݒ�
extern void FIELD_WFBC_SetAwayPeople( FIELD_WFBC* p_wk, u32 npc_id );

// ��b����
extern void FIELD_WFBC_AddTalkPointPeople( FIELD_WFBC* p_wk, u32 npc_id );

// �����̐l�̏���WordSet�ɐݒ�
extern void FIELD_WFBC_SetWordSetParentPlayer( const FIELD_WFBC* cp_wk, WORDSET* p_wordset, u32 npc_id, u32 word_set_idx, HEAPID heapID );

// �o�g���������Ƃ�ݒ�
extern void FIELD_WFBC_SetBattlePeople( FIELD_WFBC* p_wk, u32 npc_id );
extern BOOL FIELD_WFBC_IsBattlePeople( const FIELD_WFBC* cp_wk, u32 npc_id );




//-------------------------------------
///	�l�����̎擾
//=====================================
extern u32 FIELD_WFBC_PEOPLE_GetOBJCode( const FIELD_WFBC_PEOPLE* cp_people );
extern FIELD_WFBC_PEOPLE_STATUS FIELD_WFBC_PEOPLE_GetStatus( const FIELD_WFBC_PEOPLE* cp_people );
extern const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_GetPeopleData( const FIELD_WFBC_PEOPLE* cp_people );
extern const FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_PEOPLE_GetPeopleCore( const FIELD_WFBC_PEOPLE* cp_people );


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//  �����̃w�b�_�Ő錾���Ă��邪���Ԃ�field_g3d_mapper.c�ɂ���
//------------------------------------------------------------------
extern FIELD_WFBC* FLDMAPPER_GetWfbcWork( const FLDMAPPER* g3Dmapper);

//-------------------------------------
///	�}�b�p�[��WFBC���[�N�ɊX����ݒ肷��
//=====================================
extern void FLDMAPPER_SetWfbcData( FLDMAPPER* g3Dmapper, FIELD_WFBC_CORE* p_core, MAPMODE mapmode );


#ifdef PM_DEBUG
extern void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId, void* p_gdata );
extern void FIELD_FUNC_RANDOM_GENERATE_TermDebug();
#endif


#ifdef _cplusplus
}	// extern "C"{
#endif



