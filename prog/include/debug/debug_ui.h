//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		debug_ui.h
 *	@brief  UI�@�㏑���@�f�o�b�N�@�\
 *	@author	tomoya takahashi
 *	@date		2010.04.29
 *
 *	���W���[�����F
 *
 *	*�g���������ɔz�u����邽�߁A�f�o�b�J�[��ł������삵�܂���B
 *	�R���\�[���^�C�v�̃`�F�b�N�����Ă���g�p���Ă��������B
 *	if( (OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER)) ){
 *	}
 *	
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#ifdef PM_DEBUG

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�N���^�C�v
//=====================================
typedef enum {
  DEBUG_UI_NONE,            // �Ȃ�
  DEBUG_UI_AUTO_UPDOWN,     // �����@�㉺
  DEBUG_UI_AUTO_LEFTRIGHT,  // �����@���E
  DEBUG_UI_AUTO_USER_INPUT, // �����@���̓f�[�^�Đ�

  DEBUG_UI_TYPE_MAX,
} DEBUG_UI_TYPE;

//-------------------------------------
///	�Đ��^�C�v
//=====================================
typedef enum {
  DEBUG_UI_PLAY_LOOP,       // LOOP�Đ�
  DEBUG_UI_PLAY_ONE,        // ���Đ�

  DEBUG_UI_PLAY_MAX,
} DEBUG_UI_PLAY_TYPE;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern void DEBUG_UI_SetUp( DEBUG_UI_TYPE type, DEBUG_UI_PLAY_TYPE play );
extern DEBUG_UI_TYPE DEBUG_UI_GetType( void );
extern BOOL DEBUG_UI_IsUpdate( void );


#endif // PM_DEBUG



#ifdef _cplusplus
}	// extern "C"{
#endif



