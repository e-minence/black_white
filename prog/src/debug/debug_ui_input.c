//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		debug_ui_input.c
 *	@brief  �L�[���͏��̃o�b�t�@�����O
 *	@author	tomoya takahashi
 *	@date		2010.05.01
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "debug_ui_input.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

#define DEBUG_UI_INPUT_BUF_MAX  ( 60*60*20 )
GFL_UI_DEBUG_OVERWRITE DEBUG_UI_InputBuf[ DEBUG_UI_INPUT_BUF_MAX ] = {
  {
    0
  },
};
static u32 DEBUG_UI_InputCount = 0;
static BOOL DEBUG_UI_InputOn = 0;


//----------------------------------------------------------------------------
/**
 *	@brief  ���͊J�n
 */
//-----------------------------------------------------------------------------
void DEBUG_UI_INPUT_Start( void )
{
  if(DEBUG_UI_InputOn==FALSE){
    DEBUG_UI_InputCount = 0;
    DEBUG_UI_InputOn = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���͏I��
 */
//-----------------------------------------------------------------------------
void DEBUG_UI_INPUT_End( void )
{
  if(DEBUG_UI_InputOn){
    DEBUG_UI_InputOn = FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̊i�[
 */
//-----------------------------------------------------------------------------
void DEBUG_UI_INPUT_Update( void )
{
  if( DEBUG_UI_InputOn ){
    u32 tp_x, tp_y;

    // L(cont) + SELECT(trg)�̏ꍇ�A�����B
    if( (GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT) &&
        (GFL_UI_KEY_GetCont() & PAD_BUTTON_L) ){
      return ;
    }

    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].trg = GFL_UI_KEY_GetTrg();
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].cont = GFL_UI_KEY_GetCont();
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].repeat = GFL_UI_KEY_GetRepeat();
    
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].tp_cont = GFL_UI_TP_GetPointCont( &tp_x, &tp_y );
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].tp_trg = GFL_UI_TP_GetTrg();
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].tp_x = tp_x;
    DEBUG_UI_InputBuf[ DEBUG_UI_InputCount ].tp_y = tp_y;
    DEBUG_UI_InputCount ++;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̓o�b�t�@�f�[�^���擾
 *	@return 
 */
//-----------------------------------------------------------------------------
u32 DEBUG_UI_INPUT_GetInputBufNum( void )
{
  return DEBUG_UI_InputCount;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̓o�b�t�@���擾
 *	@return �o�b�t�@
 */ 
//-----------------------------------------------------------------------------
const GFL_UI_DEBUG_OVERWRITE* DEBUG_UI_INPUT_GetInputBuf( void )
{
  return DEBUG_UI_InputBuf;
}

