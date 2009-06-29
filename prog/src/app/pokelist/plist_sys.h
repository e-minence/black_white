//======================================================================
/**
 * @file	plist_sys.c
 * @brief	�|�P�������X�g ���C������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPLIST
 * ���W���[�����FPLIST_UTIL
 */
//======================================================================
#pragma once

#include "plist_local_def.h"

extern const BOOL PLIST_InitPokeList( PLIST_WORK *work );
extern const BOOL PLIST_TermPokeList( PLIST_WORK *work );
extern const BOOL PLIST_UpdatePokeList( PLIST_WORK *work );

//--------------------------------------------------------------
//	�w��Z�ӏ��Ƀt�B�[���h�Z�����邩�H
//  �߂�l��PL_RET_***** �����Ƃ��͂O
//--------------------------------------------------------------
extern u32 PLIST_UTIL_CheckFieldWaza( const POKEMON_PARAM *pp , const u8 wazaPos );

