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
enum
{
  //========================================================
  // �l�̕ύXOK
  //========================================================
  CURSOR_ANM_FRAME    = 4*2,  ///< �A�j���[�V�����t���[��
  CURSOR_BMP_DIFF_PX  = -11,  ///< �J�[�\���\���ʒuX( �w��BMP�̍��W� )
  CURSOR_BMP_DIFF_PY  = -9,   ///< �J�[�\���\���ʒuY( �w��BMP�̍��W� )

  //========================================================
  // �l��ύX����Ƃ��͑��̗̈�ɂ�������Ȃ��Ƃ����Ȃ��l
  //========================================================
  CURSOR_ANM_NO_MAX = 3,   ///< �A�j���p�^�[����
  CURSOR_BMP_SX     = 10,  ///< �J�[�\���\���T�C�YX
  CURSOR_BMP_SY     = 7,   ///< �J�[�\���\���T�C�YY

  CURSOR_BMP_CHR_X  = 2,
  CURSOR_BMP_CHR_Y  = 2,
};

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
  GFL_BMP_DATA  *bmp_cursor;
  u8 cursor_anm_no;
  u8 cursor_anm_frame;
  u8 push_flag;
  u8 decide_key_flag : 1;
  u8 decide_tp_flag : 1;
  u8 padding : 6;
  u16 n_col;
  u16 padding_16;
};

//=============================================================================
/**
 *							�f�[�^�e�[�u��
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
 *  @param is_decide_key  [IN] TRUE:����{�^���A�L�����Z���{�^���ŃJ�[�\���������B
 *  @param is_decide_tp   [IN] TRUE:�^�b�`�ŃJ�[�\���������B
 *  @param heapID     [IN] HEAPID �q�[�vID
 *  @retval nothing
 */
//--------------------------------------------------------------
APP_KEYCURSOR_WORK* APP_KEYCURSOR_Create( u16 n_col, BOOL is_decide_key, BOOL is_decide_tp, HEAPID heap_id )
{
  APP_KEYCURSOR_WORK* work;
  
  GF_ASSERT( is_decide_key == TRUE || is_decide_tp == TRUE ); // �ǂ��炩��TRUE�łȂ��ƃJ�[�\���������Ȃ��B

  work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(APP_KEYCURSOR_WORK) );
 
  work->n_col = n_col;
  work->decide_key_flag = is_decide_key;
  work->decide_tp_flag  = is_decide_tp;

  // BMP����
  work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)sc_skip_cursor_character,
        CURSOR_BMP_CHR_X, CURSOR_BMP_CHR_Y, GFL_BMP_16_COLOR, heap_id );

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
  
  x = GFL_BMP_GetSizeX( bmp ) + CURSOR_BMP_DIFF_PX;
  y = GFL_BMP_GetSizeY( bmp ) + CURSOR_BMP_DIFF_PY;
  offs = work->cursor_anm_no; // 0, 1, 2
  GFL_BMP_Fill( bmp, x, y+offs, CURSOR_BMP_SX, CURSOR_BMP_SY, n_col );
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
  
  APP_KEYCURSOR_Clear( work, bmp, n_col );
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= CURSOR_ANM_FRAME ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= (CURSOR_ANM_NO_MAX);
  }

  x = GFL_BMP_GetSizeX( bmp ) - CURSOR_BMP_DIFF_PX;
  y = GFL_BMP_GetSizeY( bmp ) - CURSOR_BMP_DIFF_PY;
  offs = work->cursor_anm_no; // 0, 1, 2
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 2, x, y+offs, CURSOR_BMP_SX, CURSOR_BMP_SY, 0x00 );
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
void APP_KEYCURSOR_Main( APP_KEYCURSOR_WORK* work, PRINT_STREAM* stream, GFL_BMPWIN* msgwin )
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
    {
      BOOL is_decide = FALSE;

      if( work->decide_key_flag )
      {
        // �L�[����
        is_decide = ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL );
      }

      if( is_decide == FALSE && work->decide_tp_flag )
      {
        // �^�b�`����
        is_decide = GFL_UI_TP_GetTrg();
      }

      if( is_decide )
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

