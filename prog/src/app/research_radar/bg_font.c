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
#include "print/printsys.h"  // for PRINTSYS_xxxx


//===============================================================================
// ��BG_FONT
//===============================================================================
struct _BG_FONT
{
  BG_FONT_PARAM param;
  GFL_FONT*     font;     // �g�p����t�H���g
  GFL_MSGDATA*  message;  // �Q�Ƃ��郁�b�Z�[�W�f�[�^
  GFL_BMPWIN*   bmpWin;   // �r�b�g�}�b�v�E�B���h�E
};


//===============================================================================
// ���v���g�^�C�v�錾
//===============================================================================


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
 */
//-------------------------------------------------------------------------------
void BG_FONT_SetString( BG_FONT* BGFont, u32 strID )
{
  BG_FONT_PARAM* param;
  GFL_BMP_DATA*  bmpData;
  STRBUF*        strbuf;
  PRINTSYS_LSB   color;

  param   = &(BGFont->param);
  color   = PRINTSYS_LSB_Make( param->colorNo_L, param->colorNo_S, param->colorNo_B );
  strbuf  = GFL_MSG_CreateString( BGFont->message, strID );
  bmpData = GFL_BMPWIN_GetBmp( BGFont->bmpWin );

  // ��������
  PRINTSYS_PrintColor( bmpData, 
                       param->offsetX, param->offsetY, 
                       strbuf, BGFont->font, color ); 
  // VRAM�֓]��
  GFL_BMPWIN_MakeTransWindow( BGFont->bmpWin );

  GFL_HEAP_FreeMemory( strbuf );
}
