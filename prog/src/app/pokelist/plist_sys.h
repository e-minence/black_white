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
  LSCL_OK,       //�o������
  LSCL_OK_FULL,  //�o������(�ł��Z�����ς�
  LSCL_NG,    //�o�����Ȃ�
  LSCL_LEARN, //���łɊo���Ă���
}PLIST_SKILL_CAN_LEARN;

extern const BOOL PLIST_InitPokeList( PLIST_WORK *work );
extern const BOOL PLIST_TermPokeList( PLIST_WORK *work );
extern const BOOL PLIST_UpdatePokeList( PLIST_WORK *work );

extern void PLIST_InitBG0_2DMenu( PLIST_WORK *work );
extern void PLIST_InitBG0_3DParticle( PLIST_WORK *work );
extern void PLIST_TermBG0_2DMenu( PLIST_WORK *work );
extern void PLIST_TermBG0_3DParticle( PLIST_WORK *work );

//--------------------------------------------------------------
//	�w��Z�ӏ��Ƀt�B�[���h�Z�����邩�H
//  �߂�l��PL_RET_***** �����Ƃ��͂O
//--------------------------------------------------------------
extern u32 PLIST_UTIL_CheckFieldWaza( const POKEMON_PARAM *pp , const u8 wazaPos );
extern const BOOL PLIST_UTIL_IsBattleMenu( const PLIST_WORK *work );
extern const BOOL PLIST_UTIL_IsBattleMenu_CanReturn( const PLIST_WORK *work );
extern const BOOL PLIST_UTIL_CanReturn( const PLIST_WORK *work );
extern const PLIST_SKILL_CAN_LEARN PLIST_UTIL_CheckLearnSkill( PLIST_WORK *work , const POKEMON_PARAM *pp , const u8 idx );
//monsno�Ԃ�
extern const u16 PLIST_UTIL_CheckItemEvolution( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo );

//�����`��p
extern void PLIST_UTIL_DrawStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawValueStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );
extern void PLIST_UTIL_DrawValueStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col );

//�A�C�e���ɓn���p
extern void PLIST_MessageWaitInit( PLIST_WORK *work , u32 msgId , const BOOL isWaitKey , PLIST_CallbackFunc msgCallBack );
extern void PLIST_MSGCB_ExitCommon( PLIST_WORK *work );
extern void PLIST_MSGCB_ForgetSkill_ForgetCheck( PLIST_WORK *work );

//���Ԑ؂ꋭ���I��
extern void PLIST_ForceExit_Timeup( PLIST_WORK *work );
