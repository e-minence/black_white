//============================================================================
/**
 *  @file   zukan_info.h
 *  @brief  �}�ӏ��
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.h, touchbar.h, ui_template.h���Q�l�ɂ��č쐬���܂����B
 *
 *  ���W���[�����FZUKAN_INFO
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>

// system
#include "gamesystem/gamesystem.h"

typedef enum
{
  ZUKAN_INFO_DISP_M,
  ZUKAN_INFO_DISP_S,
}
ZUKAN_INFO_DISP;

typedef enum
{
  ZUKAN_INFO_LAUNCH_TOROKU,
  ZUKAN_INFO_LAUNCH_NICKNAME,
  //ZUKAN_INFO_LAUNCH_LIST,
}
ZUKAN_INFO_LAUNCH;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _ZUKAN_INFO_WORK ZUKAN_INFO_WORK;

//=============================================================================
/**
 *  �֐��錾
 */
//=============================================================================
//-------------------------------------
/// ������
//=====================================
extern ZUKAN_INFO_WORK* ZUKAN_INFO_Init( HEAPID a_heap_id, POKEMON_PARAM* a_pp,
                                         ZUKAN_INFO_LAUNCH a_launch,
                                         ZUKAN_INFO_DISP a_disp, u8 a_bg_priority,
                                         GFL_CLUNIT* a_clunit,
                                         GFL_FONT* a_font,
                                         PRINT_QUE* a_print_que );

//-------------------------------------
/// �j��
//=====================================
extern void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ���C��
//=====================================
extern void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work );


extern void ZUKAN_INFO_DeleteOthers( ZUKAN_INFO_WORK* work );
extern void ZUKAN_INFO_Move( ZUKAN_INFO_WORK* work );  // �����ֈړ��ł���i�K�ɂȂ�����ړ����Ȃ�����A�Ƃ����t���O�𗧂Ă邾���Ȃ̂ŁA�����Ɉړ�����Ƃ͌���Ȃ�
extern BOOL ZUKAN_INFO_IsMove( ZUKAN_INFO_WORK* work );  // �ړ����J�n���Ă�����TRUE��Ԃ��B�ړ����J�n���Ă��Ȃ��Ƃ���FALSE��Ԃ��B�ړ����I�����Ă�����FALSE��Ԃ��B
extern BOOL ZUKAN_INFO_IsCenterStill( ZUKAN_INFO_WORK* work );  // �����ŐÎ~���Ă�����TRUE��Ԃ��B
extern void ZUKAN_INFO_Start( ZUKAN_INFO_WORK* work );
