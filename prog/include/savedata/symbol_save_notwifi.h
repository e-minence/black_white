//==============================================================================
/**
 * @file    symbol_save_notwifi.h
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^�Fnotwifi�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.03.06(�y)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern SYMBOL_MAP_LEVEL_LARGE SymbolSave_GetMapLevelLarge(SYMBOL_SAVE_WORK *symbol_save);
extern SYMBOL_MAP_LEVEL_SMALL SymbolSave_GetMapLevelSmall(SYMBOL_SAVE_WORK *symbol_save);

extern u16 SymbolSave_GetKeepLargeSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, u8 *occ_num);
extern u16 SymbolSave_GetKeepSmallSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, u8 *occ_num);
extern u16 SymbolSave_GetFreeLargeSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, int map_no, u8 *occ_num);
extern u16 SymbolSave_GetFreeSmallSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, int map_no, u8 *occ_num);


