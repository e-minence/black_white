/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  BmpWin + ������`�� �p�b�P�[�W�I�u�W�F�N�g
 * @file   bg_font.h
 * @author obata
 * @date   2010.02.09
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "bg_font.h"
#include "research_common.h"
#include "print/printsys.h"  // for PRINTSYS_xxxx


//===============================================================================
// ��BG_FONT
//===============================================================================
struct _BG_FONT
{
  BG_FONT_PARAM param;    // ��{�p�����[�^
  GFL_FONT*     font;     // �g�p����t�H���g
  GFL_MSGDATA*  message;  // �Q�Ƃ��郁�b�Z�[�W�f�[�^
  GFL_BMPWIN*   bmpWin;   // �r�b�g�}�b�v�E�B���h�E
};


//===============================================================================
// ���v���g�^�C�v�錾
//=============================================================================== 
// �Z���^�����O���邽�߂�x�`��I�t�Z�b�g���v�Z����
static int CalcXOffsetForCentering( const BG_FONT* BGFont, const STRBUF* str );



//-------------------------------------------------------------------------------
/**
 * @brief BG_FONT �I�u�W�F�N�g�𐶐�����
 *
 * @param param   �����I�u�W�F�N�g�̃p�����[�^
 * @param font    �g�p����t�H���g
 * @param message �Q�Ƃ��郁�b�Z�[�W�f�[�^
 * @param heapID  �g�p����q�[�vID
 *
 * @return �������� BG_FONT �I�u�W�F�N�g
 */
//-------------------------------------------------------------------------------
BG_FONT* BG_FONT_Create( const BG_FONT_PARAM* param, 
                         GFL_FONT* font, GFL_MSGDATA* message, HEAPID heapID )
{
  BG_FONT* BGFont;

  // ����
  BGFont = GFL_HEAP_AllocMemory( heapID, sizeof(BG_FONT) );

  // ������
  BGFont->param   = *param;
  BGFont->font    = font;
  BGFont->message = message;
  BGFont->bmpWin  = GFL_BMPWIN_Create( param->BGFrame, 
                                       param->posX, param->posY, 
                                       param->sizeX, param->sizeY,
                                       param->paletteNo,
                                       GFL_BMP_CHRAREA_GET_F ); 
  return BGFont;
}

//-------------------------------------------------------------------------------
/**
 * @brief BG_FONT �I�u�W�F�N�g��j������
 *
 * @param BGFont
 */
//-------------------------------------------------------------------------------
void BG_FONT_Delete( BG_FONT* BGFont )
{
  GF_ASSERT( BGFont );
  GF_ASSERT( BGFont->bmpWin );

  GFL_BMPWIN_Delete( BGFont->bmpWin );
  GFL_HEAP_FreeMemory( BGFont );
}

//-------------------------------------------------------------------------------
/**
 * @brief �������ݒ肷��
 *
 * @param BGFont
 * @param strID  �������ޕ�����ID
 */
//-------------------------------------------------------------------------------
void BG_FONT_SetMessage( BG_FONT* BGFont, u32 strID )
{
  STRBUF* strbuf;

  // ��������擾
  strbuf = GFL_MSG_CreateString( BGFont->message, strID );

  // ��������
  BG_FONT_SetString( BGFont, strbuf );

  GFL_HEAP_FreeMemory( strbuf );
}

//-------------------------------------------------------------------------------
/**
 * @brief �������ݒ肷��
 *
 * @param BGFont
 * @param strbuf �������ޕ�����
 */
//------------------------------------------------------------------------------- 
void BG_FONT_SetString( BG_FONT* BGFont, const STRBUF* strbuf )
{
  BG_FONT_PARAM* param;
  GFL_BMP_DATA*  bmpData;
  PRINTSYS_LSB   color;
  int xOffset, yOffset;

  param   = &(BGFont->param);
  color   = PRINTSYS_LSB_Make( param->colorNo_L, param->colorNo_S, param->colorNo_B );
  bmpData = GFL_BMPWIN_GetBmp( BGFont->bmpWin );

  // �N���A
  GFL_BMP_Clear( bmpData, BGFont->param.colorNo_B );

  // �������݃I�t�Z�b�g���W������
  if( BGFont->param.centeringFlag ) {
    // �Z���^�����O����
    xOffset = CalcXOffsetForCentering( BGFont, strbuf );
    yOffset = param->offsetY;
  }
  else { 
    // �Z���^�����O�Ȃ�
    xOffset = param->offsetX;
    yOffset = param->offsetY;
  }

  // ��������
  PRINTSYS_PrintColor( bmpData, xOffset, yOffset, strbuf, BGFont->font, color ); 

  // VRAM�֓]��
  GFL_BMPWIN_MakeTransWindow( BGFont->bmpWin );
}

//-------------------------------------------------------------------------------
/**
 * @brief �\���E��\����ݒ肷��
 *
 * @param BGFont
 * @param enable �\������ꍇTRUE, ��\���ɂ���ꍇFALSE
 */
//-------------------------------------------------------------------------------
void BG_FONT_SetDrawEnable( BG_FONT* BGFont, BOOL enable )
{
  if( enable ) {
    // �X�N���[�����쐬
    GFL_BMPWIN_MakeScreen( BGFont->bmpWin );
  }
  else {
    // �X�N���[�����N���A
    GFL_BMPWIN_ClearScreen( BGFont->bmpWin );
  }

  // VRAM�֓]��
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame( BGFont->bmpWin ) );
}


//-------------------------------------------------------------------------------
/**
 * @brief �Z���^�����O���邽�߂�x�`��I�t�Z�b�g���v�Z����
 *
 * @param BGFont �������ݑΏ�
 * @param str    �������ޕ�����
 *
 * @return �Z���^�����O���ď������ނ��߂�x�`��I�t�Z�b�g
 */
//-------------------------------------------------------------------------------
static int CalcXOffsetForCentering( const BG_FONT* BGFont, const STRBUF* str )
{
  int targetWidth; // �������ݐ�̕�
  int strWidth;    // ������̕�
  int offset;

  targetWidth = GFL_BMPWIN_GetSizeX( BGFont->bmpWin ) * DOT_PER_CHARA;
  strWidth    = PRINTSYS_GetStrWidth( str, BGFont->font, 0 );

  // �Z���^�����O�̂��߂̃I�t�Z�b�g�l���Z�o
  offset = (targetWidth - strWidth) * 0.5f; 

  return offset;
}
