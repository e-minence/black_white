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

  DEBUG_UI_TYPE_MAX,
} DEBUG_UI_TYPE;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern void DEBUG_UI_SetUp( DEBUG_UI_TYPE type );
extern DEBUG_UI_TYPE DEBUG_UI_GetType( void );


#endif // PM_DEBUG



#ifdef _cplusplus
}	// extern "C"{
#endif



