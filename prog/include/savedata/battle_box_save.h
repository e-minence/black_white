//============================================================================================
/**
 * @file	battle_box_save.h
 * @brief	�o�g���{�b�N�X�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/09/25
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"


//======================================================================
//	define
//======================================================================
#define BATTLE_BOX_PARTY_NUM (1)
#define BATTLE_BOX_PARTY_MEMBER (6)

//�{�b�N�X���B�ꉞ�p�ӂ��Ēʏ�̃{�b�N�X�ɃT�C�Y�͍��킹�Ă���
#define BATTLE_BOX_NAME_MAXLEN  (8)
#define BATTLE_BOX_NAME_BUFSIZE (20)	// ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B

//======================================================================
//	typedef struct
//======================================================================

typedef struct _BATTLE_BOX_SAVE BATTLE_BOX_SAVE;


//======================================================================
//	proto
//======================================================================

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int BATTLE_BOX_SAVE_GetWorkSize(void);
extern void BATTLE_BOX_SAVE_InitWork(BATTLE_BOX_SAVE * musSave);

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
extern BATTLE_BOX_SAVE* BATTLE_BOX_SAVE_GetBattleBoxSave( SAVE_CONTROL_WORK *sv );

extern void BATTLE_BOX_SAVE_SetPPP( BATTLE_BOX_SAVE * btlBoxSave, const u32 boxIdx, const u32 idx, const POKEMON_PASO_PARAM * ppp );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�̃Z�[�u�f�[�^����POEPARTY���쐬
 *
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @param		heapID		�q�[�v�h�c
 *
 * @return	�쐬�����f�[�^
 */
//--------------------------------------------------------------------------------------------
extern POKEPARTY * BATTLE_BOX_SAVE_MakePokeParty( BATTLE_BOX_SAVE * sv, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		POEPARTY���o�g���{�b�N�X�̃Z�[�u�f�[�^�ɐݒ�
 *
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @param		party			POKEPARTY
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BATTLE_BOX_SAVE_SetPokeParty( BATTLE_BOX_SAVE * sv, POKEPARTY * party );

//----------------------------------------------------------
//  PPP�擾
//----------------------------------------------------------
extern POKEMON_PASO_PARAM* BATTLE_BOX_SAVE_GetPPP( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx , const u32 idx );
//----------------------------------------------------------
//  �{�b�N�X���擾
//----------------------------------------------------------
extern STRCODE* BATTLE_BOX_SAVE_GetBoxName( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�Ƀ|�P�����������Ă��邩�ǂ���
 * @param		sv				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @return	������TRUE
 */
//--------------------------------------------------------------------------------------------
extern BOOL BATTLE_BOX_SAVE_IsIn( BATTLE_BOX_SAVE * sv );


//----------------------------------------------------------
//  
//----------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�����b�N����
 * @param		btlBoxSave				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @param  	flg       ���b�N����ꍇTRUE
 */
//--------------------------------------------------------------------------------------------
extern void BATTLE_BOX_SAVE_SetLockFlg( BATTLE_BOX_SAVE *btlBoxSave,int flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�g���{�b�N�X�����b�N����Ă��邩�ǂ���
 * @param		btlBoxSave				�o�g���{�b�N�X�̃Z�[�u�f�[�^
 * @return	���b�N���Ă���TRUE
 */
//--------------------------------------------------------------------------------------------
extern BOOL BATTLE_BOX_SAVE_GetLockFlg( BATTLE_BOX_SAVE *btlBoxSave );
