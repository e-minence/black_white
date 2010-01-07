//=============================================================================
/**
 *
 *	@file		app_keycursor.c
 *	@brief  ���b�Z�[�W�A�L�[�J�[�\���i���b�Z�[�W����A�C�R���j
 *	@author	hosaka genya(kagaya����̃\�[�X���Q�l�ɋ��ʉ�)
 *	@data		2010.01.05
 *
 */
//=============================================================================
#include <gflib.h>
#include "print/printsys.h" // for PRINT_STREAM

#include "app/app_keycursor.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
/// ���C�����[�N
//--------------------------------------------------------------
struct _APP_KEYCURSOR_WORK 
{
  u16 n_col;
  u16 padding_16;
  GFL_BMP_DATA  *bmp_cursor;
  u8 cursor_anm_no;
  u8 cursor_anm_frame;
  u8 push_flag;
  u8 padding[1];
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//======================================================================
//  �L�[����J�[�\��
//======================================================================

// ����J�[�\���f�[�^
static const u8 ALIGN4 sc_skip_cursor_character[128] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x22,0x22,0x22,0x22,
  0x21,0x22,0x22,0x22,0x10,0x22,0x22,0x12, 0x00,0x21,0x22,0x11,0x00,0x10,0x12,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,
  0x11,0x00,0x00,0x00,0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� ������
 *  @param n_col      [IN] �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 *  @param heapID     [IN] HEAPID �q�[�vID
 *  @retval nothing
 */
//--------------------------------------------------------------
APP_KEYCURSOR_WORK* APP_KEYCURSOR_Create( u16 n_col, HEAPID heap_id )
{
  APP_KEYCURSOR_WORK* work;

  work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(APP_KEYCURSOR_WORK) );
 
  work->n_col   = 15;

  // BMP����
  work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)sc_skip_cursor_character,
        2, 2, GFL_BMP_16_COLOR, heap_id );

  return work;
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �폜
 * @param work ���C�����[�N
 * @retval nothing
 */
//--------------------------------------------------------------
void APP_KEYCURSOR_Delete( APP_KEYCURSOR_WORK *work )
{
  GFL_BMP_Delete( work->bmp_cursor );
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �N���A
 * @param work  ���C�����[�N
 * @param bmp   �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
void APP_KEYCURSOR_Clear( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  GFL_BMP_Fill( bmp, x, y+offs, 10, 7, n_col );
//  HOSAKA_Printf("x=%d y=%d offs=%d \n",x,y,offs);
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\��
 * @param work APP_KEYCURSOR_WORK
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
void APP_KEYCURSOR_Write( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  APP_KEYCURSOR_Clear( work, bmp, n_col );
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4*2 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }

  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 2, x, y+offs, 10, 7, 0x00 );
//  HOSAKA_Printf("x=%d y=%d offs=%d \n",x,y,offs);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �L�[�J�[�\�� �又��
 *
 *	@param	work ���C�����[�N
 *	@param	stream �v�����g�X�g���[��
 *	@param	msgwin ���b�Z�[�W�E�B���h�E��BMPWIN
 *	@note  ���b�Z�[�W�\�����ɖ�SYNC�ĂԁBstream��NULL�̎��͌Ă΂Ȃ��悤�ɂ���B
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void APP_KEYCURSOR_Proc( APP_KEYCURSOR_WORK* work, PRINT_STREAM* stream, GFL_BMPWIN* msgwin )
{
  PRINTSTREAM_STATE state;

  GF_ASSERT( work );
  GF_ASSERT( msgwin );
  GF_ASSERT( stream );

  state = PRINTSYS_PrintStreamGetState( stream );

  switch( state )
  {
  case PRINTSTREAM_STATE_DONE : // �I��
    work->push_flag = FALSE;
    break;

  case PRINTSTREAM_STATE_PAUSE : // �ꎞ��~��
    // �L�[���͑҂�
    if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL ) || GFL_UI_TP_GetTrg() )
    {
      work->push_flag = TRUE;
      APP_KEYCURSOR_Clear( work, GFL_BMPWIN_GetBmp(msgwin), work->n_col );
    }
    else
    {
      if( work->push_flag == FALSE )
      {
        APP_KEYCURSOR_Write( work, GFL_BMPWIN_GetBmp(msgwin), work->n_col );
      }
    }

    GFL_BMPWIN_TransVramCharacter( msgwin );
    break;

  case PRINTSTREAM_STATE_RUNNING :  // ���s��
    work->push_flag = FALSE;
    break;
  
  default : GF_ASSERT(0);
  };

}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

