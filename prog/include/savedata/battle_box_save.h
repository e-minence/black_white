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


//======================================================================
//	define
//======================================================================
#define BATTLE_BOX_PARTY_NUM (2)
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


//----------------------------------------------------------
//  PPP�擾
//----------------------------------------------------------
POKEMON_PASO_PARAM* BATTLE_BOX_SAVE_GetPPP( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx , const u32 idx );
//----------------------------------------------------------
//  �{�b�N�X���擾
//----------------------------------------------------------
STRCODE* BATTLE_BOX_SAVE_GetBoxName( BATTLE_BOX_SAVE *btlBoxSave , const u32 boxIdx );
