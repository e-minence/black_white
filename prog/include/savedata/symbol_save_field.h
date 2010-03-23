//==============================================================================
/**
 * @file    symbol_save_field.h
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^�Ńt�B�[���h��ł����g�p���Ȃ�����(FIELDMAP�I�[�o�[���C�ɔz�u)
 * @author  matsuda
 * @date    2010.03.06(�y)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 SymbolSave_Field_CheckKeepZoneSpace(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type);
extern void SymbolSave_Field_Move_FreeToKeep(SYMBOL_SAVE_WORK *symbol_save, u32 now_no);
extern void SymbolSave_Field_Move_KeepToFree(SYMBOL_SAVE_WORK *symbol_save, u32 keep_no);
extern BOOL SymbolSave_Field_MoveAuto( SYMBOL_SAVE_WORK *symbol_save, u32 no );

