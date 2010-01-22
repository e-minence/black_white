//==============================================================================
/**
 * @file    symbol_save.h
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.01.06(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
//�V���{���G���J�E���g�Z�[�u�f�[�^
typedef struct _SYMBOL_SAVE_WORK SYMBOL_SAVE_WORK;
//�V���{���G���J�E���g1�C���̃Z�[�u
typedef struct _SYMBOL_POKEMON SYMBOL_POKEMON;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 SymbolSave_GetWorkSize( void );
extern void SymbolSave_WorkInit(void *work);
extern void SymbolSave_Set(SYMBOL_SAVE_WORK *symbol_save, u16 monsno, u16 wazano, u8 sex, u8 form_no);

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern SYMBOL_SAVE_WORK* SymbolSave_GetSymbolData(SAVE_CONTROL_WORK* pSave);
