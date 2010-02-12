/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  BmpWin + ������`�� �p�b�P�[�W�I�u�W�F�N�g
 * @file   bg_font.h
 * @author obata
 * @date   2010.02.09
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "print/gf_font.h"


//===============================================================================
// ���s���S�`�錾
//===============================================================================
typedef struct _BG_FONT BG_FONT;


//===============================================================================
// ��BG_FONT �I�u�W�F�N�g�����p�����[�^
//===============================================================================
typedef struct
{
  u8 BGFrame;   // �Ώ�BG�t���[��
  u8 posX;      // x���W (�L�����N�^�[�P��)
  u8 posY;      // x���W (�L�����N�^�[�P��)
  u8 sizeX;     // x�T�C�Y (�L�����N�^�[�P��)
  u8 sizeY;     // y�T�C�Y (�L�����N�^�[�P��)
  u8 offsetX;   // �������ݐ�x���W (�h�b�g�P��)
  u8 offsetY;   // �������ݐ�y���W (�h�b�g�P��)
  u8 paletteNo; // �g�p�p���b�g�ԍ�
  u8 colorNo_L; // �����̃J���[�ԍ�
  u8 colorNo_S; // �e�̃J���[�ԍ�
  u8 colorNo_B; // �w�i�̃J���[�ԍ�

} BG_FONT_PARAM; 


//===============================================================================
// ��
//===============================================================================
// ����/�j��
extern BG_FONT* BG_FONT_Create( const BG_FONT_PARAM* param, 
                                GFL_FONT* font, GFL_MSGDATA* message, HEAPID heapID );
extern void     BG_FONT_Delete( BG_FONT* BGFont );

// ������̐ݒ�
extern void BG_FONT_SetString( BG_FONT* BGFont, u32 strID );
