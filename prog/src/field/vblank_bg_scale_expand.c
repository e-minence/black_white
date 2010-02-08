///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  VBlank���荞�݊��Ԓ���BG�A�t�B���ϊ�
 * @file   vblank_bg_scale_expand.c
 * @author obata
 * @date   2010.02.05
 *
 * �}�b�v�`�F���W���̉�ʒ�~���Ԓ���BG�ʂ��g�傳���邽�߂�VBlank�֐�.
 * �N���X�t�F�[�h�J�n���ɃL���v�`������BG�ʂɑ΂���, �A�t�B���ϊ���K�p����.
 * ����ȊO�̏�ʂł̎g�p�͍l�����Ă��܂���B
 * ����Ώ�BG�ʂ̓��C����ʂ�BG2�ł��B
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "vblank_bg_scale_expand.h"
#include "system/gfl_use.h"

#define SCALE_UP_SPEED (FX32_CONST(0.003f))
#define MAX_SCALE      (FX32_CONST(0.8f))
#define CENTER_X       (128)
#define CENTER_Y       (92)
#define SCROLL_X       (0)
#define SCROLL_Y       (0)

static fx32 BGScale;  // BG�g�嗦


//-----------------------------------------------------------------------------
/**
 * @brief VBlank���荞�ݏ���
 */
//-----------------------------------------------------------------------------
static void VBlankFunc_BGExpand( void )
{
  MtxFx22 matrix;

  // VBlank���ԊO
  if( GX_IsVBlank() == FALSE ){ return; }

  // �g��
  BGScale -= SCALE_UP_SPEED;
  if( BGScale < MAX_SCALE ){ BGScale = MAX_SCALE; }

  // �A�t�B���ϊ�
  MTX_Scale22( &matrix, BGScale, BGScale );
  G2_SetBG2Affine( &matrix, CENTER_X, CENTER_Y, SCROLL_X, SCROLL_Y );
}


//-----------------------------------------------------------------------------
/**
 * @brief BG�̊g����J�n����
 */
//-----------------------------------------------------------------------------
void StartVBlankBGExpand()
{
  // �g�嗦��������
  BGScale = FX32_ONE;

  // VBlank�֐���o�^
  GFUser_SetVIntrFunc( VBlankFunc_BGExpand );
}

//-----------------------------------------------------------------------------
/**
 * @brief BG�̊g����I������
 */
//-----------------------------------------------------------------------------
void EndVBlankBGExpand()
{
  MtxFx22 matrix;

  // �A�t�B���ϊ�������
  MTX_Identity22( &matrix );
  G2_SetBG2Affine( &matrix, CENTER_X, CENTER_Y, SCROLL_X, SCROLL_Y );

  // VBlank�֐�������
  GFUser_ResetVIntrFunc();
}
