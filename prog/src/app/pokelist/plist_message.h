//======================================================================
/**
 * @file	plist_message.c
 * @brief	�|�P�������X�g ���b�Z�[�W
 * @author	ariizumi
 * @data	09/06/17
 *
 * ���W���[�����FPLIST_MSG
 */
//======================================================================
#pragma once

#include "plist_local_def.h"

//���b�Z�[�W�E�B���h�E�̎��
typedef enum
{
  PMT_BAR,      //�o�[��1�s���b�Z�[�W
  PMT_MENU,     //���j���[�̏�����2�s
  PMT_MESSAGE,  //�t���T�C�Y
  
  PMT_NONE,     //�E�B���h�E���Ă�
  PMT_MAX,
}PLIST_MSGWIN_TYPE;


extern PLIST_MSG_WORK* PLIST_MSG_CreateSystem( PLIST_WORK *work );
extern void PLIST_MSG_DeleteSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );
extern void PLIST_MSG_UpdateSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );

extern void PLIST_MSG_OpenWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const PLIST_MSGWIN_TYPE winType);
extern void PLIST_MSG_CloseWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );

extern void PLIST_MSG_DrawMessageNoWait( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const u32 msgId );
extern void PLIST_MSG_DrawMessageStream( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const u32 msgId );
extern const BOOL PLIST_MSG_IsFinishMessage( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );

extern void PLIST_MSG_CreateWordSet( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );
extern void PLIST_MSG_DeleteWordSet( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );
extern void PLIST_MSG_AddWordSet_PokeName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , POKEMON_PARAM *pp );
extern void PLIST_MSG_AddWordSet_ItemName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 itemIdx );
extern void PLIST_MSG_AddWordSet_SkillName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 skillNo );

