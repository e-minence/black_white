//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		tradepoke_after_save.h
 *	@brief  �|�P���������@���̌�@�Z�[�u�f�[�^
 *	@author	tomoya takahashi
 *	@date		2010.03.26
 *
 *	���W���[�����FTRPOKE_AFTER_SAVE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK

#include "gamesystem/game_data.h"

#include "poke_tool/poke_tool.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
typedef enum{
  TRPOKE_AFTER_SAVE_C02,    // �V�b�|�E
  TRPOKE_AFTER_SAVE_C05,    // �z�h���G

  TRPOKE_AFTER_SAVE_TYPE_MAX,
} TRPOKE_AFTER_SAVE_TYPE;

//-------------------------------------
///	���x������
//=====================================
#define TRPOKE_AFTER_LEVEL_ADD  (10)
#define TRPOKE_AFTER_LEVEL_MAX  (100)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�����|�P�����@���̌�@�Z�[�u���[�N
//=====================================
typedef struct _TRPOKE_AFTER_SAVE TRPOKE_AFTER_SAVE;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int TRPOKE_AFTER_SV_GetWorkSize(void);
extern TRPOKE_AFTER_SAVE* TRPOKE_AFTER_SV_AllocWork(HEAPID heapID);
extern void TRPOKE_AFTER_SV_Init(TRPOKE_AFTER_SAVE* sv);

//----------------------------------------------------------
//	GAMEDATA�̃A�N�Z�X
//----------------------------------------------------------
extern TRPOKE_AFTER_SAVE* GAMEDATA_GetTrPokeAfterSaveData( GAMEDATA* gdata );

//----------------------------------------------------------
//	�f�[�^�A�N�Z�X
//----------------------------------------------------------
// �擾
extern BOOL TRPOKE_AFTER_SV_IsData( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type );
extern u8 TRPOKE_AFTER_SV_GetLevel( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  );
extern u32 TRPOKE_AFTER_SV_GetRnd( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  ); // �ő̗���
extern const STRCODE* TRPOKE_AFTER_SV_GetNickName( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type );
extern u32 TRPOKE_AFTER_SV_GetID( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  ); // ID
extern BOOL TRPOKE_AFTER_SV_IsSpeabino3( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  ); // ��R�Ƃ�����
extern u8 TRPOKE_AFTER_SV_GetSpeabino3( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  ); // ��R�Ƃ�����

//�@�ݒ�
extern void TRPOKE_AFTER_SV_SetData( TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type, const POKEMON_PARAM * pp );



#ifdef _cplusplus
}	// extern "C"{
#endif



