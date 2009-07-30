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
#include "print/wordset.h"

typedef enum
{
  LSCL_OK,    //�o������
  LSCL_NG,    //�o�����Ȃ�
  LSCL_LEARN, //���łɊo���Ă���
}PLIST_SKILL_CAN_LEARN;

extern const BOOL PLIST_InitPokeList( PLIST_WORK *work );
extern const BOOL PLIST_TermPokeList( PLIST_WORK *work );
extern const BOOL PLIST_UpdatePokeList( PLIST_WORK *work );

//--------------------------------------------------------------
//	�w��Z�ӏ��Ƀt�B�[���h�Z�����邩�H
//  �߂�l��PL_RET_***** �����Ƃ��͂O
//--------------------------------------------------------------
extern u32 PLIST_UTIL_CheckFieldWaza( const POKEMON_PARAM *pp , const u8 wazaPos );
extern const BOOL PLIST_UTIL_IsBattleMenu( const PLIST_WORK *work );
extern const PLIST_SKILL_CAN_LEARN PLIST_UTIL_CheckLearnSkill( PLIST_WORK *work , const POKEMON_PARAM *pp );

//�����`��p
extern void PLIST_UTIL_DrawStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawValueStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawValueStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
