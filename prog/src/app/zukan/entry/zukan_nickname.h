//============================================================================
/**
 *  @file   zukan_nickname.h
 *  @brief  �j�b�N�l�[����t���邩�ۂ�
 *  @author Koji Kawada
 *  @data   2009.12.10
 *  @note   
 *
 *  ���W���[�����FZUKAN_NICKNAME
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>

// system
#include "gamesystem/gamesystem.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
typedef enum
{
  ZUKAN_NICKNAME_RESULT_NO,
  ZUKAN_NICKNAME_RESULT_YES,
  ZUKAN_NICKNAME_RESULT_SELECT,
}
ZUKAN_NICKNAME_RESULT;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _ZUKAN_NICKNAME_WORK ZUKAN_NICKNAME_WORK;

//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
extern ZUKAN_NICKNAME_WORK* ZUKAN_NICKNAME_Init( HEAPID a_heap_id, GFL_CLUNIT* a_clunit, GFL_FONT* a_font, PRINT_QUE* a_print_que );
extern void ZUKAN_NICKNAME_Exit( ZUKAN_NICKNAME_WORK* work );
extern ZUKAN_NICKNAME_RESULT ZUKAN_NICKNAME_Main( ZUKAN_NICKNAME_WORK* work );

